# Control Simulated Car with PID Controller

## The effect each of the P, I, D components had in my implementation.

The P component pulls the car towards the "ideal" trace line implied by the CTE input values. The larger the value, the harder the car gets pulled. The [video](./PID_Ponly.mov) shows the effects of the P component. In this video, only the P component is used.

The D component dampens oscillations caused by the P component. Without the D component, the car over-corrects repeatedly, leading to undesirable oscillations which will eventually grow large enough to cause it to fall off the course. The [video](./PID_PD.mov) shows how the D component successfully keeps the car on the course by minimizing oscillations. In this video, the P and D components are used.

The I component is supposed to correct permanent mis-configurations (e.g. steering misalignment). The [video](./PID_all.mov) shows how the car drives when all three components are used, though the effect of the I component is not clear, if there is any.

## How I chose the hyper-parameters.

I have decided to tweak parameters manually to find a reasonably good combination of values.

First, I only set the P value, keeping the other two to zero. The goal was find a value that seems to give the car sufficient "eagerness" to make all corners at 30MPH. Since the car failed to do a wrap with the P value only, there was some guessing work there.

Then, I set the D component as well to keep oscillations under control. By basically doing bisections manually, I could find a value that allowed the car to do wraps successfully.

I also experimented with the I component for a while, but I am not sure if I found a good value for it. The car's steering seems to be aligned well enough so that the P and D components alone could correct misalignment, if there is any.

Finally, I added a simple, rule based throttling control to allow the car to move as fast as possible. The rule keeps the gas pedal on the floor (i.e. going Vin Diesel) unless the car is sufficiently off or is about to move away from the ideal trace line. When the gas pedal is not on the floor, it applies a brake if the speed exceeds 70MPH or just coasts otherwise.
