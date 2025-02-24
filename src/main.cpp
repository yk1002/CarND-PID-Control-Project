#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include <chrono>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int sample_count = 0;
bool start_time_set = false;
std::chrono::system_clock::time_point start_time;
int sampling_duration_sec = INT_MAX;

int main(int argc, char** argv)
{
  uWS::Hub h;

  PID pid;
  double Kp, Ki, Kd;

  if (argc == 1) {
      // These are optimal params discovered via experiments.
      Kp = 0.15;
      Ki = 0.00005;
      Kd = 8.0;
  }
  else {
      assert(argc >= 5);
      sscanf(argv[1], "%lf", &Kp);
      sscanf(argv[2], "%lf", &Ki);
      sscanf(argv[3], "%lf", &Kd);
      sscanf(argv[4], "%d", &sampling_duration_sec);
  }
  pid.Init(Kp, Ki, Kd);

  h.onMessage([&pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double steering_angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double throttle = std::stod(j[1]["throttle"].get<std::string>());

          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */
          pid.move(cte, steering_angle, speed, throttle);
          steering_angle = pid.steering_angle_out;
          throttle = pid.throttle_out;
          ++sample_count;
          
          // DEBUG
          // std::cout << sample_count
          //           << ", steering: " << steering_angle
          //           << ", throttle: " << throttle
          //           << ", speed: " << speed
          //           << std::endl;

          // terminate after preset amount of time has passed
          if (start_time_set) {
              const auto now = std::chrono::system_clock::now();
              if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() >= sampling_duration_sec) {
                  std::cout << "Error: " << pid.TotalError() << std::endl;
                  exit(0);
              }
          }
          else {
              start_time = std::chrono::system_clock::now();
              start_time_set = true;
          }

          json msgJson;
          msgJson["steering_angle"] = steering_angle;
          msgJson["throttle"] = throttle;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
