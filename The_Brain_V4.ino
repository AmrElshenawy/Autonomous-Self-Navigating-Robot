//Amr Elshenawy MEng Individual Major Project
//Autonomous Self-Navigating Robot
//FINAL VERSION - AUTOMATED ROTATION
//3 Ultrasonic Sensors - Forward, Left, Right
//4WD Drive Chassis
//Bluetooth Module
//--------------------------------------------------------------------------------------------------------------------------------
//Sensors Intialisations, Flags and Variables

int ForwardEcho = A12;
int ForwardTrig = A13;
int RightEcho = A10;
int RightTrig = A11;
int LeftEcho = A8;
int LeftTrig = A9;

long FSduration, LSduration, RSduration;
float IFSdistance, FSdistance, ILSdistance, LSdistance, IRSdistance, RSdistance;

unsigned int sum = 0;
unsigned int sum2 = 0;
unsigned int sum3 = 0;
unsigned int Sample_Count = 0;
unsigned int Sample_Count2 = 0;
unsigned int Sample_Count3 = 0;

int Num_Samples = 10;
int KeepLooking = 250;

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//Motor Intialisations

#define ENALeft 5
#define ENBRight 6
#define LeftForward 7     //IN1
#define RightBackward 9   //IN3
#define LeftBackward 8    //IN2
#define RightForward 11   //IN4

int FRota = 80;
int RRota = 200;

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//Enumeration List

enum StatusMode
{
  IDLE,
  Autonomous,
  Manual,
  Test
} func_mode = IDLE;

enum MovementMode 
{
  STOP,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
} mov_mode = STOP;

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//Operation Functions

void MoveForward()
{
  analogWrite(ENALeft, FRota);
  analogWrite(ENBRight, FRota);
  digitalWrite(RightForward, HIGH); 
  digitalWrite(LeftForward, HIGH);    //MOVE FORWARD FUNCTION
  digitalWrite(RightBackward, LOW);
  digitalWrite(LeftBackward, LOW);
}

void MoveBackward()
{
  analogWrite(ENALeft, FRota);
  analogWrite(ENBRight, FRota);
  digitalWrite(RightForward, LOW);
  digitalWrite(LeftForward, LOW);   //MOVE BACKWARD FUNCTION
  digitalWrite(RightBackward, HIGH);
  digitalWrite(LeftBackward, HIGH);
}

void TurnLeft()
{
  analogWrite(ENALeft, RRota);
  analogWrite(ENBRight, RRota);
  digitalWrite(RightForward, HIGH);
  digitalWrite(LeftForward, LOW);   //TURN LEFT FUNCTION
  digitalWrite(RightBackward, LOW);
  digitalWrite(LeftBackward, HIGH);
}

void TurnRight()
{
  analogWrite(ENALeft, RRota);
  analogWrite(ENBRight, RRota);
  digitalWrite(RightForward, LOW);
  digitalWrite(LeftForward, HIGH);    //TURN RIGHT FUNCTION
  digitalWrite(RightBackward, HIGH);
  digitalWrite(LeftBackward, LOW);
}

void Stop()
{
  digitalWrite(ENALeft, LOW);
  digitalWrite(ENBRight, LOW);
  digitalWrite(RightForward, LOW);
  digitalWrite(LeftForward, LOW);   //STOP FUNCTION
  digitalWrite(RightBackward, LOW);
  digitalWrite(LeftBackward, LOW);
}

float ForwardSensor()
{
  digitalWrite(ForwardTrig, LOW); //Set soundwaves transmitter to LOW
  delayMicroseconds(5);   //Wait 5us

  digitalWrite(ForwardTrig, HIGH);  //Set soundwaves transmitter to HIGH
  delayMicroseconds(10); // Wait 10us
  digitalWrite(ForwardTrig, LOW);   //Set soundwaves trasnmitter to LOW

  FSduration = pulseIn(ForwardEcho, HIGH);    //Receiver soundwaves duration dumped into variable

  IFSdistance = FSduration * 0.0343 / 2;    //distance calculation

  if(IFSdistance > 3 && IFSdistance < 350)
  {
    while(Sample_Count < Num_Samples) 
    {
      sum += IFSdistance;                 //Further distance calibration
      Sample_Count++;                        
      delay(10);                           
    }
  }

  FSdistance = ((float)sum / (float)Num_Samples); //Average distance calculated

  Sample_Count = 0;
  sum = 0; 
  
  Serial.print("Distance: ");
  Serial.println(FSdistance);

  return FSdistance; 
}

float LeftSensor()        // SAME COMMENTS FROM FORWARD SENSOR
{
  digitalWrite(LeftTrig, LOW);
  delayMicroseconds(2);

  digitalWrite(LeftTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(LeftTrig, LOW);

  LSduration = pulseIn(LeftEcho, HIGH);

  ILSdistance = LSduration * 0.0343 / 2;

  if(ILSdistance > 3 && ILSdistance < 350)
  {
    while(Sample_Count2 < Num_Samples) 
    {
      sum2 += ILSdistance;              
      Sample_Count2++;                        
      delay(10);                           
    }
  }

  LSdistance = ((float)sum2 / (float)Num_Samples);

  Sample_Count2 = 0;
  sum2 = 0; 
  
  Serial.print("Distance: ");
  Serial.println(LSdistance);

  return LSdistance;
}

float RightSensor()   // SAME COMMENTS FROM FORWARD SENSOR
{
  digitalWrite(RightTrig, LOW);
  delayMicroseconds(5);

  digitalWrite(RightTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(RightTrig, LOW);

  RSduration = pulseIn(RightEcho, HIGH);

  IRSdistance = RSduration * 0.0343 / 2;

  if(IRSdistance > 3 && IRSdistance < 350)
  {
    while(Sample_Count3 < Num_Samples) 
    {
      sum3 += IRSdistance;              
      Sample_Count3++;                        
      delay(10);                           
    }
  }

  RSdistance = ((float)sum3 / (float)Num_Samples);

  Sample_Count3 = 0;
  sum3 = 0; 
  
  Serial.print("Distance: ");
  Serial.println(RSdistance);

  return RSdistance;
}

void BluetoothData()    //BLUETOOTH function for categorizing the data recieved from the app
{
  if(Serial.available())
  {
    switch(Serial.read())
      {
        case 'f': func_mode = Manual; mov_mode = FORWARD;      break;
        case 'b': func_mode = Manual; mov_mode = BACKWARD;     break;
        case 'l': func_mode = Manual; mov_mode = LEFT;         break;
        case 'r': func_mode = Manual; mov_mode = RIGHT;        break;
        case 's': func_mode = Manual; mov_mode = STOP;         break;
        case 'a': func_mode = Autonomous;                      break;
        case 't': func_mode = Test;                            break; 
        default:                                               break;
     }
  }
}

void AutonomousMode()     //Autonomous Navigation Algorithm Control
{
    if(func_mode == Autonomous)
    {
      ForwardSensor();
      RightSensor();
      LeftSensor();
    
      if(FSdistance > 47 && RSdistance > 37 && LSdistance > 37)
      {
        MoveForward();
      }
      else if(FSdistance > 47 && RSdistance < 37 && LSdistance > 37)
      {
        TurnLeft();
        delay(80);
        MoveForward();
      }
      else if(FSdistance > 47 && LSdistance < 37 && RSdistance > 37)
      {
        TurnRight();
        delay(80);
        MoveForward();
      }
      if(FSdistance < 47)
      {
        MoveBackward();
        delay(400);
        while(FSdistance < 53)
        {
          if(RSdistance > LSdistance)
          {
            TurnRight();
            delay(250);
            ForwardSensor();
          }
          else if(LSdistance > RSdistance)
          {
            TurnLeft();
            delay(250);
            ForwardSensor();
          }
          
        }
        ForwardSensor();
        MoveForward();
      }
    }
}

void ManualMode()   //Manual Control Function
{
  if(func_mode == Manual)
  {
    switch(mov_mode)
    {
      case FORWARD: MoveForward();    break;
      case BACKWARD: MoveBackward();  break;
      case LEFT: TurnLeft();          break;
      case RIGHT: TurnRight();        break;
      case STOP: Stop();              break;
      default:                        break;
    }
  }
}

void TestMode()   //Automated Hardware Testing Algorithm
{
  if(func_mode == Test)
  {
    ForwardSensor();
    LeftSensor();
    RightSensor();
    
    if(FSdistance == 0 || LSdistance == 0 || RSdistance == 0)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(300);
      digitalWrite(LED_BUILTIN, LOW);
      delay(300);
    }
    else if(FSdistance > 0 && LSdistance > 0 && RSdistance > 0)
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
  if(func_mode == Manual || func_mode == Autonomous)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void setup() 
{
  pinMode(ENALeft, OUTPUT);
  pinMode(ENBRight, OUTPUT);
  pinMode(LeftForward, OUTPUT);
  pinMode(RightForward, OUTPUT);    //INTILISATIONS OF MOTOR PINS FUNCTION MODE
  pinMode(LeftBackward, OUTPUT);
  pinMode(RightBackward, OUTPUT);
  
  pinMode(ForwardEcho, INPUT);
  pinMode(ForwardTrig, OUTPUT); 
  pinMode(LeftEcho, INPUT);
  pinMode(LeftTrig, OUTPUT);    //INITIALISATIONS OF SENSOR PINS FUNCTION MODE
  pinMode(RightEcho, INPUT);
  pinMode(RightTrig, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);     //SERIAL COMMUNCATION BAUD RATE SPPED
}

void loop() 
{
  BluetoothData();
  AutonomousMode();   //CALLING OF ALL MAIN FUNCTIONS
  ManualMode();
  TestMode();
  /*unsigned long startt = micros();
  AutonomousMode();
  unsigned long endt = micros();
  unsigned long delta = endt - startt;
  Serial.println(delta);*/ 
}
