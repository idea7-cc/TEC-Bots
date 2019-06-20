/**
  @example basic-example.ino
  @file basic-example.ino
  @author Evert Chin
  @brief Fast and Simple Stepper Driver
  
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
*/

//example of driving a nema 17 stepper motor with A4988
#include "SimpleStepper.h"

#define LM_STEP 2             // stepper step pin
#define LM_DIR 5             // stepper dir pin
#define RM_STEP 3             // stepper step pin
#define RM_DIR 6             // stepper dir pin
#define STEPS_PER_REV 800        // 360/1.8 = 200 full steps * 4 microsteps = number of steps per revolution 
#define FWD 1            // Rotation of the stepper motor, reverse if it is swapped
#define REV 0               // Rotation of the stepper motor, reverse if it is swapped
#define ENABLE_PIN 8               // enable pin

//SimpleStepper leftMotor(LM_DIR, LM_STEP);
SimpleStepper rightMotor(RM_DIR, RM_STEP);
SimpleStepper leftMotor(LM_DIR, LM_STEP);
//uint8_t counter = 0;
//uint8_t stepsRemaining = 0;

void setup() {
  //initialize the object
  leftMotor.init();
  rightMotor.init();
}

void loop() {
  
  //once the stepper finished stepping to remaining ticks/steps
 // if(stepper.isStopped()){

   rightMotor.step(10, 0, 420);
   leftMotor.step(10, 0, 420);
 //  stepper.stop();
 //  digitalWrite(ENABLE_PIN,0);

   
  

}
