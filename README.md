# uStepper_PLC_Open

This code in this project is designed to be run on uStepper controllers:  
http://www.ustepper.com/index/  

It is designed for when you want to use the uStepper as an "intelligent" drive, controlled from a master controller that just wants to send simple commands to drive the motion and let the uStepper take care of the rest. This allows you to have an application running multiple uSteppers in a way that gives more control over timing and build more complex applications.

It follows the practice conventionally used in industrial control and automation using the functions described in PLC_Open motion control using the IEC 61131-3 Programming Languages:  
http://www.plcopen.org/pages/tc2_motion_control/part_1_2/index.htm  

The setup requires serial communication to each uStepper. I2C may have been a  better protocol however currently the uStepper boards can not act as I2C slaves due to the encoder.

This project is not in any way associated to the uStepper project on Github,  it is independent, issues related to the native uStepper codebase should be  raised with the official uStepper page:

https://github.com/uStepper/uStepper
