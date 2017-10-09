#include "PID.h"

#include <algorithm>
#include <cmath>

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;

    steering_angle_out = 0.0;
    throttle_out = 0.0;

    prev_cte_set = false;
    int_cte = 0.0;
    n_samples = 0;
}

void PID::UpdateError(double cte) {
}

double PID::TotalError() {
    return int_cte / n_samples;
}

void PID::move(double cte, double steering_angle, double speed, double throttle) {
    ++n_samples;
    int_cte += cte;
    const double delta_cte = prev_cte_set ? cte - prev_cte : 0.0;
    prev_cte = cte;
    prev_cte_set = true;

    steering_angle_out = -Kp * cte - Ki * int_cte / n_samples - Kd * delta_cte;
    steering_angle_out = min(max(steering_angle_out, -1.0), 1.0);
    
    /*
      Throttle control
    */
    const auto abs_cte =  abs(cte);
    const auto abs_delta_cte = abs(delta_cte);

    throttle_out = 0.99; // Floor the gas (i.e. going Diesel)
    if (abs_delta_cte > 0.2 || abs_cte > 0.5) {  // ... except when the car starts moving away from the track or is being off the track
        if (speed > 70) throttle_out = -0.1;     // break if it is doing more than 70mph
        else if (speed > 40) throttle_out = 0.0; // release the gas if it is doing more than 40mph
    }
}
