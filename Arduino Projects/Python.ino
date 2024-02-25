/*
 * Firmata is a generic protocol for communicating with microcontrollers
 * from software on a host computer. It is intended to work with
 * any host computer software package.
 *
 * To download a host software package, please click on the following link
 * to open the list of Firmata client libraries in your default browser.
 *
 * https://github.com/firmata/arduino#firmata-client-libraries
 */

/* Supports as many analog inputs and analog PWM outputs as possible.
 *
 * This example code is in the public domain.
 */
#include <Firmata.h>
#include <LiquidCrystal.h> 

LiquidCrystal lcd(12,9,5,4,3,2);  
#include <Adafruit_MLX90614.h>

#include <SoftwareSerial.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
// GPIO where relay is connected to

#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library


byte analogPin = 0;

void analogWriteCallback(byte pin, int value)
{
  if (IS_PIN_PWM(pin)) {
    pinMode(PIN_TO_DIGITAL(pin), OUTPUT);
    analogWrite(PIN_TO_PWM(pin), value);
  }
}


// GPIO where relay is connected to
const int relayPin =  5;


// Handle received messages
String message = "";

// Create software serial object to communicate with HC-05
SoftwareSerial mySerial(10, 11); //HC-05 Tx & Rx is connected to Arduino #3 & #2

PulseSensorPlayground pulseSensor;  // Creates an object

const int PulseWire = 0;       // 'S' Signal pin connected to A0
const int LED13 = 13;          // The on-board Arduino LED
int Threshold = 600;           // Determine which Signal to "count as a beat" and which to ignore




void setup()
{
  Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
  Firmata.begin(9600);
  // Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  lcd.begin(16, 4);
  // Initialize relayPin as an output
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  
  //Begin serial communication with Arduino and HC-05
  mySerial.begin(9600);
  lcd.setCursor(0, 0);
  lcd.print("Pair Bluetooth!");
  delay(100);
  infraredsetup();
  HeartBeatsetup();
}

void loop()
{
  while (Firmata.available()) {
    Firmata.processInput();
  }
  // do one analogRead per loop, so if PC is sending a lot of
  // analog write messages, we will only delay 1 analogRead
  Firmata.sendAnalog(analogPin, analogRead(analogPin));
  analogPin = analogPin + 1;
  if (analogPin >= TOTAL_ANALOG_PINS) analogPin = 0;

   if (mySerial.available()){
    message = "";
    char incomingChar = mySerial.read();
    if (incomingChar != '\n'){
      message += String(incomingChar);
    }
    else{
      message = "";
    }
  }
  // Check received message and control output accordingly
  if (message == "1"){
  infraredloop();
  HeartBeatvoid();  
  }
  else if (message == "0"){
    
    message = "";
  }
  delay(20);

}



void infraredsetup(){
  while (!Serial);

  if (!mlx.begin()) {
    lcd.setCursor(0, 3);
    lcd.print("Error MLX Sensor");
    while (1);
  }
}


void infraredloop(){
  lcd.setCursor(0, 0);
  lcd.print("Body Temperature");
  lcd.setCursor(0, 1);
  lcd.print("Body Temp = "); lcd.print(mlx.readObjectTempC()); lcd.print("*C");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("Body Temp = "); lcd.print("        ");
  delay(500);
}

void HeartBeatsetup() {
     // Configure the PulseSensor object, by assigning our variables to it
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       // Blink on-board LED with heartbeat
  pulseSensor.setThreshold(Threshold);   

  // Double-check the "pulseSensor" object was created and began seeing a signal
  if (pulseSensor.begin()) {
    lcd.setCursor(0, 1);
    lcd.print("PulseSensor work!");
    delay(100);
    }
  }

void HeartBeatvoid() {
  int myBPM = pulseSensor.getBeatsPerMinute();      // Calculates BPM

  if (pulseSensor.sawStartOfBeat()) {   // Constantly test to see if a beat happened
    lcd.setCursor(4, 2);
    lcd.print("HeartBeat! "); // If true, print a message
    lcd.setCursor(4, 3);
    lcd.print("BPM: ");  lcd.print(myBPM);  // Print the BPM value
    delay(1000);
    lcd.setCursor(4,3);
    lcd.print("BPM: "); lcd.print("           ");
    }

  delay(20);
  }
