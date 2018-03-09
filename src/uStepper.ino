/* This script is loaded onto each uStepper motor so that all motors are
 *  independent, code can be kept the same on each uStepper from project to project.
 *  A master device communicating over serial is required to initialise movements.
 *  The Idea is that the system wll behave like the PLC_Open standard.
 *
 *  Supporting Notes:
 *  HARD mode - keeps brake on motor
 *  SOFT mode - removes brake from motor
 *  SOFT mode only work when not using PID otherwise
 *  softstop - Stop with deceleration figure
 *  hardstop - stop fast as you can
 *
 *  Commands sent from Master
 *  01 - Enable PID mode
 *  02 - Remove brake in PID (workaround)
 *  03 - Disable PID mode and brake
 *  04 - Move Continuous CW
 *  05 - Move Continuous CCW
 *  06 - Move Relative SOFT
 *  07 - Move Realtive HARD
 *  08 - Move Absolute SOFT
 *  09 - Move Absolute HARD
 *  10 - Move Steps CW SOFT
 *  11 - Move Steps CCW SOFT
 *  12 - Move Steps CW HARD
 *  13 - Move Steps CCW HARD
 *  14 - Set new max accel
 *  15 - Set new max velo
 *  16 - softstop SOFT
 *  17 - softstop HARD
 *  18 - hardstop SOFT
 *  19 - hardstop HARD
 *  20 - set home position to current position
 *  22 - get motorstate (motor moving or not)
 */

#include <uStepper.h>
#define maxacceleration 9000// steps/s^2
#define maxvelocity 3000// steps/s
uStepper stepper(maxacceleration,maxvelocity);//instanstiate
String command;


void setup()
{
  stepper.setup();
  Serial.begin(9600);
}


void serialEvent()
{
    /*  Essentially this function just reads the commands into the a string and
     *  the code in the main loop chooses what to do with it.
     */
    char strX;
    command = "";
    while (Serial.available())
    {
     // get the new byte:
     strX = char(Serial.read());
     // add it to the inputString:
     command += strX;
     delay(5);
    }
}

void negativecmd(String order, int* mysub)
{    
    if(*mysub == 0) // toInt() return 0, if the first value of the string isn't an integer. As the character "-" isn't one
                   // when transmitting a negative value in a string it will return 0
      {
          *mysub = order.substring(3).toInt();
          *mysub = - (*mysub);
      }
}

void loop()
{
    int sub = command.substring(2).toInt(); //converting the substring to integer for move positions

    if(command.substring(0,2)=="01") //enable PID motor mode
    {
      stepper.setup(PID,SIXTEEN,10,5,1.0,0.02,0.006,0);
      command = "";
    }
    else if(command.substring(0,2)=="02") //Remove Brake from Motor motor in PID mode (keeps track of the position if you switch back to PID)
    {
      stepper.setup(PID,SIXTEEN,400000,100000,1.0,0.02,0.002,0);
      command = "";
    }
    else if(command.substring(0,2)=="03") //disable PID mode and brake(loses position if you go back to PID)
    {
      stepper.setup();
      stepper.softStop(SOFT);
      stepper.softStop(SOFT);
      command = "";
    }
    else if (command.substring(0,2)=="04") //move continuous cw
    {
      stepper.runContinous(CW);
      command = "";
    }
    else if (command.substring(0,2)=="05") //move continuous ccw
    {
      stepper.runContinous(CCW);
      command = "";
    }
    else if (command.substring(0,2)=="06" & !stepper.getMotorState()) //move relative(soft) mode
    {
      negativecmd(command, &sub);
      stepper.moveAngle(sub,SOFT);
      command = "";
    }
     else if (command.substring(0,2)=="07" & !stepper.getMotorState()) //move relative(hard) mode
    {
      negativecmd(command, &sub);
      stepper.moveAngle(sub,HARD);
      command = "";
    }
    else if(command.substring(0,2)=="08" & !stepper.getMotorState()) //move absolute(SOFT) mode
    {
      negativecmd(command, &sub);
      stepper.moveToAngle(sub,SOFT);
      command = "";
    }
     else if(command.substring(0,2)=="09" & !stepper.getMotorState()) //move absolute(HARD) mode
    {
      negativecmd(command, &sub);
      stepper.moveToAngle(sub,HARD);
      command = "";
    }
    else if(command.substring(0,2)=="10" & !stepper.getMotorState()) //move predefined number of steps(CW,SOFT) mode
    {
      stepper.moveSteps(sub,CW,SOFT);
      command = "";
    }
    else if(command.substring(0,2)=="11" & !stepper.getMotorState()) //move predefined number of steps(CCW,SOFT) mode
    {
      stepper.moveSteps(sub,CCW,SOFT);
      command = "";
    }
     else if(command.substring(0,2)=="12" & !stepper.getMotorState()) //move predefined number of steps(CW,HARD) mode
    {
      stepper.moveSteps(sub,CW,HARD);
      command = "";
    }
     else if(command.substring(0,2)=="13" & !stepper.getMotorState()) //move predefined number of steps(CCW,HARD) mode
    {
      stepper.moveSteps(sub,CCW,HARD);
      command = "";
    }
      else if(command.substring(0,2)=="14" & !stepper.getMotorState()) //set new maxAcceleration mode
    {
      stepper.setMaxAcceleration(sub);
     command = "";
    }
      else if(command.substring(0,2)=="15" & !stepper.getMotorState()) //set new maxVelocity mode
    {
      stepper.setMaxVelocity(sub);
      command = "";
    }
    else if(command.substring(0,2)=="16") //soft stop of the motor (SOFT parameter)
    {
      stepper.softStop(SOFT);
      stepper.softStop(SOFT);
      command = "";
    }
    else if(command.substring(0,2)=="17") //soft stop of the motor(HARD parameter)
    {
      stepper.softStop(HARD);
      command = "";
    }
    else if(command.substring(0,2)=="18") //hard stop of the motor(SOFT parameter)
    {
      stepper.hardStop(SOFT);
      stepper.hardStop(SOFT);
      command = "";
    }
    else if(command.substring(0,2)=="19") //hard stop of the motor(HARD parameter)
    {
      stepper.hardStop(HARD);
      command = "";
    }
    else if(command.substring(0,2)=="20") //homing of the motor(set new home position)
    {
      stepper.encoder.setHome();
      command = "";
    }
    else if(command.substring(0,2)=="22") //get motor state (moving or not)
    {
      if(stepper.getMotorState() == 1) //Motor Moving
      {
        Serial.write("011");
        command = "";
      }
      else if(stepper.getMotorState() == 0) //Motor Not Moving
      {
        Serial.write("010");
        command = "";
      }
    }
}
