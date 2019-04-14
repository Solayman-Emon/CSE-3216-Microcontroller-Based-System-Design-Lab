/*
Authors ::  Solayman Hossain Emon,
            Lamia Nazrin, 
            Nahid Hasan, 
            Md. Toasin Habib.
           
Aust CSE 37th Batch

IDs::       16.02.04.091
            16.02.04.103
            16.02.04.106
            16.02.04.107
*/

#include <Servo.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <AFMotor.h>

Servo ServoMotor;
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Set the LCD address to 0x27 
AF_DCMotor Left_Motor(2, MOTOR12_64KHZ);// create motor #2, 64KHz pwm
AF_DCMotor Right_Motor(3 , MOTOR34_64KHZ);

//Initialize the Variables
byte SmileIcon[] = {
  B00000,
  B01010,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};

byte SadIcon[] = {
  B00000,
  B01010,
  B00000,
  B00000,
  B01110,
  B10001,
  B00000,
  B00000
};

byte DegreeIcon[] = {
  B00000,
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000
};

const int servoPin = 9;
const int trigPin = 52;
const int echoPin = 53;
const int trigPin1 = 50;
const int echoPin1 = 51;
const int Lm35Pin = A1;  
long duration, distance, average_Distance = 0, average_Distance1 = 0;
long temp_Distance[3]; 
int CloseCoverDelay = 7;                // Open the Cover of the bucket for 7 seconds
float temp_Val, temperature = 0;
float temp_Arr[5];
int state = 0;


void setup() {
  
  pinMode(trigPin, OUTPUT);      // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);       // Sets the echoPin as an Input
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  
  ServoMotor.attach(servoPin);   // Attaches the servo on pin 4 
  ServoMotor.write(100);         // Initially Closed the cover of the bucket
  delay(100);
  ServoMotor.detach(); 
  
  Serial.begin(9600);            // Starts the serial communication
  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, SmileIcon);
  lcd.createChar(1, SadIcon);
  lcd.createChar(2, DegreeIcon);

}

void temp_Measure(){
   
  temp_Val = analogRead(Lm35Pin);        // Read Temperature 
  temp_Val = (temp_Val * 0.48828125);    // Convert adc value to equivalent voltage 
   
}

float avg_Temperature(){

   float avg_Temp;
   for(int i = 0; i < 10; i++)
   {
      temp_Measure();
      temp_Arr[i] = temp_Val;
      delay(10);   
   }
   avg_Temp = (temp_Arr[0] + temp_Arr[1] + temp_Arr[2] + temp_Arr[3] + temp_Arr[4] + temp_Arr[5] + temp_Arr[6] + temp_Arr[7] + temp_Arr[8] + temp_Arr[9]);
   return avg_Temp;
}

void distance_Measure(const int trigPin, const int echoPin) { 
   
  digitalWrite(trigPin, LOW);         // Clears the trigPin 
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);        // Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);  // Reads the echoPin, returns the sound wave travel time in microseconds
  distance = duration*0.034/2;        // Calculating the distance

}

long long int avg_Distance_Measure(const int trigPin, const int echoPin){

 long long int avg_Distance;
 for (int i = 0; i < 3; i++)      
 {
   distance_Measure(trigPin,echoPin);               
   temp_Distance[i] = distance;           
   delay(10);               
 }
 
 avg_Distance = (temp_Distance[0] + temp_Distance[1] + temp_Distance[2])/3;   //calculate the average distance  
 return avg_Distance;
 
}

// Displays the object distance in LCD
void display_Distance(String msg1, String msg2, int distance) { 
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg1);               // Prints the given custom String 
  lcd.setCursor(0,1);
  lcd.print(msg2);               // Prints the given custom String 
  lcd.print(distance);           // Prints the distance on the LCD
  lcd.print(" cm");

}

// Displays message on LCD After Opening the Cover of the bucket
void OpenBucket_Display(String msg1, String msg2, String msg3){
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.home();
  lcd.write(0);
  lcd.setCursor(2,0);
  lcd.print(msg1);
  lcd.setCursor(15,0);
  lcd.write(0);
  lcd.setCursor(2,1);
  lcd.print(msg2);
  
// Seven(7) Seconds delay before Closing the Cover  
for(int i = CloseCoverDelay; i >= 0; i--)
{
   lcd.setCursor(6,1);
   lcd.print(i);
   lcd.setCursor(8,1);
   lcd.print(msg3);
   delay(1000);
   lcd.print(" ");
}
  
}

// Displays message on LCD After Closing the Cover of the bucket
void CloseBucket_Display(String msg1, String msg2, String msg3){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.home();
  lcd.write(1);
  lcd.setCursor(2,0);
  lcd.print(msg1);
  lcd.setCursor(15,0);
  lcd.write(1);
  lcd.setCursor(0,1);
  lcd.print(msg2);
  delay(3000);
  temperature = avg_Temperature();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg3);
  lcd.setCursor(0,1);
  lcd.print(temperature);
  lcd.setCursor(6,1);
  lcd.write(2);
  lcd.print("C");
  delay(5000);
  
}

// Display message When the bucket is Full
void FullBucket_Display(String msg1){
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg1);
  lcd.setCursor(0,1);
  lcd.write(1);
  lcd.setCursor(1,1);
  lcd.write(1);
  lcd.setCursor(2,1);
  lcd.write(1);
  lcd.setCursor(3,1);
  lcd.write(1);
  lcd.setCursor(4,1);
  lcd.write(1);
  lcd.setCursor(5,1);
  lcd.write(1);
}

void Forward()
{
  Left_Motor.setSpeed(255); //Define maximum velocity
  Left_Motor.run(FORWARD); //rotate the motor clockwise 
  Right_Motor.setSpeed(200);
  Right_Motor.run(FORWARD);
}

void Backward()
{
  Left_Motor.setSpeed(255); 
  Left_Motor.run(BACKWARD); //rotate the motor counterclockwise 
  Right_Motor.setSpeed(200);
  Right_Motor.run(BACKWARD); 
}

void Right(){
  Left_Motor.setSpeed(255);
  Left_Motor.run(FORWARD);
  Right_Motor.setSpeed(200);
  Right_Motor.run(BACKWARD);
}

void Left(){
  Left_Motor.setSpeed(255);
  Left_Motor.run(BACKWARD);
  Right_Motor.setSpeed(200);
  Right_Motor.run(FORWARD);
}
 

void loop() {

// Measure the distance of the object using Sonar Sensor
  average_Distance = avg_Distance_Measure(trigPin, echoPin);
  average_Distance1 = avg_Distance_Measure(trigPin1, echoPin1);
  Serial.println(average_Distance1);
   
  display_Distance("Measuring ...","Distance :", average_Distance);   // Diplay the average distance of the object on LCD
  
// Condition for Open the Cover of the bucket
if (average_Distance < 50)     
{

  if(average_Distance1 <= 4)
  {
     FullBucket_Display("Bucket Full!!!");
     delay(4000);
  }
  else
  {
    
    ServoMotor.attach(servoPin);
    delay(1);
    ServoMotor.write(0);   // Open the Cover of the Bucket
  
    OpenBucket_Display("Opened Cover","For","Seconds");  // Display message on LCD after the Cover Open
  
    ServoMotor.write(100);  // Close the Cover of the Bucket
    CloseBucket_Display("Cover Closed","Thank You !!","The Temperature is :");  // Display message on LCD after Closed the Cover
    ServoMotor.detach(); 

  }

  Serial1.print("tick");
    if(Serial1.available() > 0){ // Checks whether data is comming from the serial port
    state = Serial1.read(); // Reads the data from the serial port
 }

 
if (state == '1') {
   Forward();
}  

else if(state == '2'){
   Backward();
}

else if(state == '3'){
   Right();
}

else if(state == '4'){
   Left();
}
 
}

}

