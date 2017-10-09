#ifndef PID_H
#define PID_H

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  /*
  * Coefficients
  */ 
  double Kp;
  double Ki;
  double Kd;

  /*
    Control output
   */
  double steering_angle_out;
  double throttle_out;

  /*
    State tracking
   */
  bool prev_cte_set;
  double prev_cte;
  double int_cte;
  int n_samples;

  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double Kp, double Ki, double Kd);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

  /*
  * Calculate the total PID error.
  */
  double TotalError();

  /*
    Control steering angle and throttle based on 
   */
  void move(double cte, double steering_angle, double speed, double throttle);
};

#endif /* PID_H */
