/*  Simon Wilmots
*   Project PinchBot (ELSY 3831 - KU Leuven)
*   Licence: MIT
*   www.simonwilmots.com
*/

#include <Servo.h> 

// Initialising of variables

/* Potentiometer */
const int potentioPIN = A0;
int potentioValue;
int servoAngle;

/* DIP-switch pins & default values */
const int DIPpin0 = A4;
const int DIPpin1 = A3;
const int DIPpin2 = A2;
const int DIPpin3 = A1;

bool DIPbit0 = false;
bool DIPbit1 = false;
bool DIPbit2 = false;
bool DIPbit3 = false;

/* LEDs */
const int redLed = 8;
const int greenLed = 9;

/* Activate button */
const int buttonPin = A5;
bool buttonPressed = false;
bool currentButtonState = false;
bool lastButtonState = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 25; 

/* Servos */
const int MAX_NUMBER_OF_SERVOS = 6;
const int MIN_NUMBER_OF_SERVOS = 1;
int currentServoId;
const int SERVO_PINS[MAX_NUMBER_OF_SERVOS] = {2, 3, 4, 5, 6, 7};
Servo SERVOS[MAX_NUMBER_OF_SERVOS];
Servo currentServo;

/* ----- FUNCTIONS ----- */

int fromBinaryToDigit(int msb, int bit2, int bit1, int lsb) {
  return  lsb + (bit1 * 2) + (bit2 * 4) + (msb * 8);
}

Servo findCurrentServo(int currentServoId) {

  if(currentServoId > MAX_NUMBER_OF_SERVOS) {
    currentServoId = MAX_NUMBER_OF_SERVOS-1;
    return SERVOS[MAX_NUMBER_OF_SERVOS-1];
  } 
  else if(currentServoId < MIN_NUMBER_OF_SERVOS) {
    currentServoId = MIN_NUMBER_OF_SERVOS;
    return SERVOS[MIN_NUMBER_OF_SERVOS];
  } 
  else {
    return SERVOS[currentServoId-1];
  }
}

int getServoAngle(int potentioValue) {
  return map(potentioValue, 0, 1023, 0, 180);
}

/* ----- Setup & Loop ----- */

void setup()
{ 
  pinMode(DIPpin0, INPUT_PULLUP);
  pinMode(DIPpin1, INPUT_PULLUP);
  pinMode(DIPpin2, INPUT_PULLUP);
  pinMode(DIPpin3, INPUT_PULLUP);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  // Initialising all servos
  for(int i=0; i<MAX_NUMBER_OF_SERVOS; i++) {
    SERVOS[i].attach(SERVO_PINS[i]);
  }

  // Init the "claw" facing forward
  SERVOS[3].write(0);

  
  Serial.begin(9600);
  delay(50);
} 

void loop() {
  
  buttonPressed = digitalRead(buttonPin);

  if (buttonPressed != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if(buttonPressed != currentButtonState) {
      currentButtonState = buttonPressed;
    }
  }
  
  if (currentButtonState) {
    // Turn on green LED and turn off red LED
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);

    /* 
        Read potentiometer value
        <---- (-) | (+) ---->
        Decrease  | Increase (degrees)
    */
    potentioValue = analogRead(potentioPIN);

    // Read DIP-switch bits
    DIPbit0 = digitalRead(DIPpin0);
    DIPbit1 = digitalRead(DIPpin1);
    DIPbit2 = digitalRead(DIPpin2);
    DIPbit3 = digitalRead(DIPpin3);

    // Convert bits to digit
    currentServoId = fromBinaryToDigit(DIPbit3, DIPbit2, DIPbit1, DIPbit0);
    
    // Find corresponding servo with digit
    currentServo = findCurrentServo(currentServoId);
    
    // Convert potentio value to degrees (0-180) 
    servoAngle = getServoAngle(potentioValue);
    currentServo.write(servoAngle);
    
    Serial.print("Moving Servo nr. "); Serial.print(currentServoId); Serial.print(" to ");
    Serial.print(servoAngle); Serial.println(" degrees");
    
   } else {
      digitalWrite(greenLed, LOW);
      digitalWrite(redLed, HIGH);
   }
   
   lastButtonState = buttonPressed;
} 
