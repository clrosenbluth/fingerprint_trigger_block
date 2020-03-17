#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
#include <Servo.h>
#include <Keypad.h>

//keypad setup
byte redOne = 8;
byte orangeTwo = 9;
byte yellowThree = 10;
byte greenFour = 11;
byte blackRow = 12;

char code[] = {'1','2','3'};
int currentPos = 0;
int endingPos = sizeof(code);
bool allowAccess = false;


//******************set up keypad starts
const byte ROWS = 1; //one row
const byte COLS = 4; //four columns
//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','4'}
};
byte rowPins[ROWS] = {blackRow}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {redOne, orangeTwo, yellowThree, greenFour}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad ourKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
//***************set up keypad ends

//pushbutton setup
const int buttonPin = 2;
int buttonState = 0;
bool beginKeypad = false;

//LEDs setup
int redLight = 6;
int greenLight = 7; 

//servo setup
Servo myservo;
int pos = 60;

//fps setup
FPS_GT511C3 fps(4, 5);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(100);
  fps.Open();
  fps.SetLED(true);
  myservo.attach(3);
  pinMode (buttonPin, INPUT);
  pinMode (redLight, OUTPUT);
  pinMode (greenLight, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:


buttonState = digitalRead(buttonPin);
if (buttonState == HIGH) beginKeypad = true;

if (fps.IsPressFinger()) verify();

else if (beginKeypad){
  keypadCodeCheck();
  if (allowAccess){
    Serial.println ("ready to enroll");
    fps.SetLED(true);
    enroll();
    allowAccess = false;
    beginKeypad = false;
  }
}

}

void verify(){
  Serial.println("Ready to begin.");
  
  while (fps.IsPressFinger() == false) delay(100);

  if (fps.IsPressFinger()){

    //steady red light
    digitalWrite(greenLight, LOW);
    digitalWrite(redLight, HIGH);
    
    while (fps.CaptureFinger(false) == false) fps.CaptureFinger(false);
    
    int id = fps.Identify1_N();
    if (id <200)
    {
      Serial.print("Verified ID:");
      Serial.println(id);

      //steady green light
      digitalWrite(redLight, LOW);
      digitalWrite(greenLight, HIGH);
      delay(1000);
      digitalWrite(greenLight, LOW);

      //move the servo
      servoCode();
      
    }
    else{
      //blinking red light twice
      digitalWrite(greenLight, LOW);
      digitalWrite(redLight, HIGH);
      delay(10);
      digitalWrite(redLight, LOW);
      delay(10);
      digitalWrite(redLight, HIGH);
      delay(10);
      digitalWrite(redLight, LOW);
      }
    }
  
  }

void servoCode(){
    for (pos = 90; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    Serial.println(pos);
    delay(1);                       // waits 15ms for the servo to reach the position
  }
  
  Serial.println("Wait three seconds");
  delay(3000);
  
  for (pos = 180; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    Serial.println(pos);
    delay(1);                       // waits 15ms for the servo to reach the position
  }
  }

void keypadCodeCheck(){
Serial.println("Begin typing the code");

//blink green twice

  char correctKey = code[currentPos];     //defines correct key
  char inputKey = ourKeypad.getKey();     //gets and prints the key that the user input
  Serial.print(inputKey);
    
  if (inputKey){                        //cpde only runs if there is an input
  if (inputKey == correctKey){          //if you put in the right key
    currentPos = currentPos + 1;        //code advances

    if (currentPos == endingPos){
      Serial.println(" correct code");

      //green light on
      digitalWrite(greenLight,HIGH);
      delay(5000);
      digitalWrite(greenLight,LOW);
      allowAccess = true;
      }
      
    }
    else{
      Serial.println(" wrong code, start over");

      //red light on
      digitalWrite(redLight,HIGH);
      delay(5000);
      digitalWrite(redLight,LOW);
      currentPos = 0;
    }
  }
}

void enroll(){ //start enrollment function
   
  // get lowest available ID
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true) {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid == true) enrollid++;
    }

  //begin enrollment
  Serial.print("Enrollment will now begin with ID #");
  Serial.println(enrollid);
  fps.EnrollStart(enrollid);

  //get first fingerpress
  Serial.println("Press finger to enroll");

//green light on
digitalWrite(greenLight,HIGH);
  
  while(fps.IsPressFinger() == false) delay (100); //wait for the finger to be pressed

  //capture first fingerpress
  delay(1000);
  Serial.println("Hold finger");
  while (fps.CaptureFinger(true) == false){
  fps.CaptureFinger(true); //capture fingerprint in high resolution
  Serial.println("Finger was not yet captured, keep it on");
  }

  //first enrollment
  fps.Enroll1();

  //end first fingerpress
  delay(1000);
  Serial.println("Remove finger");

//green light off
digitalWrite (greenLight,LOW);
  
  while(fps.IsPressFinger()) delay (100); //wait for finger to be removed

  //get second fingerpress
  delay(1000);
  Serial.println("Press the same finger");

//green light on
  digitalWrite(greenLight,HIGH);
  
  while(fps.IsPressFinger() == false) delay (100); //wait for the finger to be pressed

  //capture second fingerpress
  delay(1000);
  Serial.println("Hold finger");
  while (fps.CaptureFinger(true) == false){
  fps.CaptureFinger(true); //capture fingerprint in high resolution
  Serial.println("Finger was not yet captured, keep it on");
  }

  //second enrollment
  fps.Enroll2();

  //remove second fingerpress
  delay(1000);

//green light off
digitalWrite(greenLight,LOW);
  
  Serial.println("Remove finger");
  while(fps.IsPressFinger()) delay (100); //wait for finger to be removed

  //get third fingerpress
  delay(1000);
  Serial.println("Press the same finger");

//green light on
  digitalWrite(greenLight,HIGH);
  
  while(fps.IsPressFinger() == false) delay (100); //wait for the finger to be pressed

  //capture third fingerpress
  delay(1000);
  Serial.println("Hold finger");
  while (fps.CaptureFinger(true) == false){
  fps.CaptureFinger(true); //capture fingerprint in high resolution
  Serial.println("Finger was not yet captured, keep it on");
  }

  //third enrollment
  fps.Enroll3();

  //remove third fingerpress
  delay(1000);

//green light off
  digitalWrite(greenLight,LOW);
  
  Serial.println("Remove finger");
  while(fps.IsPressFinger()) delay (100); //wait for finger to be removed

  //check if enrollment worked
  if (fps.CheckEnrolled(enrollid) == 0){
    delay(1000);

//blink green twice
    digitalWrite(greenLight,HIGH);
    delay(10);
    digitalWrite(greenLight,LOW);
    delay(10);
    digitalWrite(greenLight,HIGH);
    delay(10);
    digitalWrite(greenLight,LOW);
    delay(1000);
    Serial.println("Enrollment was successful");
  }
  else {
    delay(1000);
    Serial.println("Enrollment was not successful");

    //red light turns on
    digitalWrite(redLight,HIGH);
    delay(5000);
    digitalWrite(redLight, LOW);
  }
  
  } //end enrollment function
