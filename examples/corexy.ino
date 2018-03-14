/*
   This script is designed to be run on a Arduino Mega and demonstrates the implementation of a CoreXY motion system using 2 uStepper drivers.
   http://corexy.com/theory.html

   Serial2 is assigned for the A motor
   Serial3 is assigned for the B motor

   Delays are needed after sending serial commands to allow uStepper to process commands before sending another instruction
*/


/****** Global Variables ******/
#define PI 3.1415926535897932384626433832795

//IO
const int X_Datum = 43;   //sensor for datuming x axis in coreXY
const int Y_Datum = 44;   //sensor for datuming  y axis in coreXY

//Variable to know if Motor is moving, true if moving
boolean motorAMoving;
boolean motorBMoving;

//Variables for XY System
const float XYPulleyRadius = 5.08;
const float GlobalVelocity = 3200; // maximum velocity of stepper XY motors steps/s
const float deg_p_Step = 1.8;
const float microsteps = 16;
float actX; //coordinate actual value for x axis
float actY; //coordinate actual value for y axis
String strSetX; //coordiante set value for x axis
String strSetY; //coordinate set value for y axis
float actA; //actual value for motorA
float actB; //actual value for motorB

//Others
String status; //Global status Variable

/******Global Variables End ******/

/****** Setup Code ******/

/*
   When this code starts, we first want to establish comms with usteppers,
   and do the homing routine before we are ready to accept other commands
*/
void setup()
{
  //MicroSwitch Setup
  pinMode(X_Datum, INPUT);
  pinMode(Y_Datum, INPUT);

  //arduino-uStepper communication
  Serial2.begin(9600);
  Serial3.begin(9600);

  //Poll uSteppers to make sure comms is established, will escape while loop due
  //to serialEvent()
  motorAMoving = true;
  while (motorAMoving == true)
  {
    Serial2.write("22");
    delay(500);
    serialEvent2();
  }

  motorBMoving = true;
  while (motorBMoving == true)
  {
    Serial3.write("22");
    delay(500);
    serialEvent3();
  }

  Serial2.write("01");//set motor a in pid mode before the start of core xy
  Serial3.write("01");//set motor b in pid mode before the start of core xy

  delay(2000); //delay to allow motors to enable
  //Datum X and Y
  Serial2.write("151000"); //slow motors down for homing
  Serial3.write("151000");
  delay(250);
  actX = datum(X_Datum); //datum before printing a new page
  actX = 400; //end of stroke
  delay(500);
  actY = datum(Y_Datum);
  actY = 400; // //end of stroke
  delay(250);
  Serial2.write("20"); //reset uStepper home positions
  Serial3.write("20");
  actA = 0;
  actB = 0;
  delay(250);
  status = "ready";
}
/****** Setup Code End******/

/***** Serial Events from uSteppers ******/
void serialEvent2()
{
  char strx;
  String uStepperfb;
  uStepperfb = "";
  while (Serial2.available())
  {
    // get the new byte:
    strx = char(Serial2.read());
    // add it to the inputString:
    uStepperfb += strx;
    delay(5);
  }
  if (uStepperfb == "011") //get motor state mode feedback
  {
    motorAMoving = true;
    uStepperfb = "";
  }
  else if (uStepperfb == "010")
  {
    motorAMoving = false;
    uStepperfb = "";
  }
}

void serialEvent3()
{
  char strx;
  String uStepperfb;
  uStepperfb = "";
  while (Serial3.available())
  {
    // get the new byte:
    strx = char(Serial3.read());
    // add it to the inputString:
    uStepperfb += strx;
    delay(5);
  }
  if (uStepperfb == "011") //get motor state mode feedback
  {
    motorBMoving = true;
    uStepperfb = "";
  }
  else if (uStepperfb == "010")
  {
    motorBMoving = false;
    uStepperfb = "";
  }
}

/****** Serial Events End ******/

float datum(int sensor)
{
  if (sensor == X_Datum && digitalRead(sensor) == LOW )
  {
    Serial2.write("04"); //move horizontal(continuous cw for both motors)
    Serial3.write("04");
  }
  else if (sensor == Y_Datum && digitalRead(sensor) == LOW )
  {
    Serial2.write("04"); //move vertical (B changes direction, as assembled)
    Serial3.write("05");
  }

  while (digitalRead(sensor) == LOW)
  {
    delay(1);
  }
  Serial2.write("19");  //hardstop
  Serial3.write("19");  //hardstop
  return float(0);
}

float mmToDeg(float set)
{
  float rad = set / XYPulleyRadius;
  return rad * (180 / PI);
}

//use this function just before every case of "while loop" checking of status
//to prevent continous polling
int delay_t (float dis, float velo) //d and v imported
{
  velo = (velo / microsteps) * deg_p_Step; //changing vel from steps/s to deg/s
  int t = int((dis / velo) * 1000); //time is ms
  delay(t);
}

void coreXY(float setX, float setY)
{
  // corexy equations/motions http://corexy.com/theory.html
  float dY = setY - actY;
  float dA = dX + dY;
  float dB = dX - dY;
  float setA = actA + dA;
  float setB = actB + dB;

  //converting mm to degrees
  float degA = mmToDeg(setA);
  float degB = mmToDeg(setB);

  //calculate speed based on ratio to start and stop motors at the same time
  String vel;
  float float_vel;

  if ((abs(dA) / abs(dB)) == 1 || dA == 0 || dB == 0)
  { //set both speeds to global velocity
    vel = "15" + String(GlobalVelocity);
    Serial3.print(vel);
    Serial2.print(vel);
  }
  else
  {
    float x = abs(dA) / abs(dB); //ratio
    if (abs(dA) > abs(dB))
    {
      //new velocity of motor B
      float_vel = GlobalVelocity / x;
      vel = "15" + String(int(float_vel));
      Serial3.print(vel);
      //new velocity of motor A
      vel = "15" + String(GlobalVelocity);
      Serial2.print(vel);
    }
    else if (abs(dB) > abs(dA))
    {
      //new velocity of motorA
      float_vel = GlobalVelocity * x;
      vel = "15" + String(int(float_vel));
      Serial2.print(vel);
      //new velocity of motorB
      vel = "15" + String(GlobalVelocity);
      Serial3.print(vel);
    }
  }

  //send move commands to uSteppers
  String strA = String(int(degA));
  String strB = String(int(degB));
  String absA = "08" + strA;
  String absB = "08" + strB;

  delay(100);
  Serial2.print(absA);
  Serial3.print(absB);
  delay(100);
  motorAMoving = true;
  motorBMoving = true;

  //calculate how long move should take roughly
  float d;
  if (abs(dA) > abs(dB))
  {
    d = abs(dA);
  }
  else
  {
    d = abs(dB);
  }
  d = mmToDeg(d); //distance in deg
  delay_t(d , GlobalVelocity);

  while (motorAMoving == true || motorBMoving == true)
  {
    Serial2.write("22");
    Serial3.write("22");
    delay(250);
    serialEvent2(); //Need to call as otherwise only called after loop()
    serialEvent3();
  }
  actX = setX;  //reseting starting positions
  actY = setY;
  actB = setB;
  actA = setA;
}

void loop()
{
  delay(100);
  coreXY(50, 350);
  delay(100)
  coreXY(350, 50)
}
