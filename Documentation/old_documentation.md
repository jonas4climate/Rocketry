# Building a Falcon 9 Rocket Model

## Basic information

This project is performed by Jonas Schäfer and Phan Minh Cuong. Initial planning started 22.02.2019.

## What is the goal of the project?

The final goal is to build a functional Falcon 9 rocket model.  
The model is planned to use Thrust Vectoring Control (TVC). The current principle of choice would be a motorised gimbal that stabilises the rocket by moving the rocket engine/nozzle and therefore adjust the angle of the engine thrust. The TVC will be controlled over a programmed Arduino micro-controller that reads sensor data from multiple sensors and returns appropriate behaviour instructions to the gimbal system.
While current plans don't go as far as landing the rocket, we consider different approaches how we could achieve this (by being able to throttle the engine or reigniting the propulsion system used).

## Step by Step

Obviously this is a very ambitious project and many small steps must be taken before we can take on this challenge. Below is the concept by which we plan to progress. Every sub-project will be of increased complexity and difficulty and is supposed to build on the previous one in order to get closer to the final project execution.

***

**1. Getting to know the 3-Axis Accelerometer and Gyroscope**  
To gain a better understanding of the usage of the accelerometer and gyroscope (precisely the [GY-521 3 Axis Gyroscope + Accelerometer Module MPU-6050](https://playground.arduino.cc/Main/MPU-6050)) we plan to do some basic testing of its functionalities using the Arduino Uno as the receiving and processing board.

**2 Getting to know basic Rocketry by building a model rocket**  
To get a better view on the practical side of things, we plan to build a basic rocket and use a [industry-standard certified _Estes_ rocket motor](https://www.estesrockets.com/rockets/engines) to launch it. We hope to understand practical issues we could encounter and important concepts we need to research further to enable our rocket to fly as stable as possible while remaining as close to an actual rocket design (no or minimal use of fins inducing drag) as possible.

**3 Building a 2D stabilised robot**  
Using the knowledge from Project 1 we plan to build a robot that is able to self-stabilise itself and compensate for exterior forces using the accelerometer and gyroscope data.

**4. Radio Control**  
Before moving on to Project 5 we could introduce the concept of radio control which would enable us to control the rocket manually while not having to rely on the software. Before launching a proper controllable rocket, a connection system between the rocket and Ground Control must be established in order to guarantee safety and enable and controlled remote takeoff and landing.

**5. Introducing a gimbal system and using a motor with propeller as propulsion technology**  
In a way a step back, we convert our rocket into a type of drone or even plane. We exchange the hard-to-control rocket motor with an electric motor with propellers. This way we will be able to introduce the gimbaling system without having to worry about the complex rocket engine thrust and we will be able to throttle the rotor to easier test the gimbaling system with less damages and potential safety issues. The gimbaling system will bring all the challenges the final project version will encounter using rocket engines but is safer and easier to handle.

**6. The final step**  
The last step will be to re-introduce the rocket engine and optimise the gimbal system to successfully stabilise the rocket during flight. If we reach this point, further project ideas on re-ignitable rocket engines or thrust-control to enable a takeoff and landing procedure could follow.
