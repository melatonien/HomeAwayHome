/*
 This is the Arduino code for DF C&C EXP4 AltController
 by Melanie Yan
 *
 Code refernece provided by C&C, using distance_motionDetection code, and DFpong_controller_startTemplate
 Motion Detection code link : https://github.com/DigitalFuturesOCADU/df-pong/blob/main/controller/examples/Sensors/Distance/distance_motionDetection/distance_motionDetection.ino
 Pong Controller Start Template link: https://github.com/DigitalFuturesOCADU/df-pong/blob/main/controller/examples/BLE/DFpong_controller_startTemplate/DFpong_controller_startTemplate.ino
 * 
 Game Link : https://digitalfuturesocadu.github.io/df-pong/
 * 
 * Movement Values:
 * 0 = Still 
 * 1 = Paddle up
 * 2 = Paddle down
 *
 I used this code from Arduino Project Hub for testing:
 https://projecthub.arduino.cc/lucasfernando/ultrasonic-sensor-with-arduino-complete-guide-284faf#section5

*/

//Including all codes here, BLE for connection, buzzer for feedback, and distance sensor code
#include <ArduinoBLE.h>
#include "ble_functions.h"
#include "buzzer_functions.h"
#include <HCSR04.h>



// ============================================
const int DEVICE_NUMBER = 23;  // Melanie Yan 23
// ============================================

// Device name is generated from device number
String deviceNameStr = "DFPONG-" + String(DEVICE_NUMBER);
const char* deviceName = deviceNameStr.c_str();

// Pin definitions buzzer/LED
const int BUZZER_PIN = 11;       // Pin for buzzer
const int LED_PIN = 9; // Pin for LED

// Movement state tracking
int currentMovement = 0;         // Current movement value (0 = still, 1 = up, 2 = down)

// Variables
float distance = 0.0f;
float smoothedDistance = 0.0f;
float lastSmoothedDistance = 0.0f;
float totalMotion = 0.0f;
unsigned long lastReadTime = 0;
const unsigned int readInterval = 30;
const float MOTION_THRESHOLD = 0.3;

// Rolling average
const int AVERAGE_WINDOW = 5;
float readings[AVERAGE_WINDOW];
int readIndex = 0;
float totalValue = 0;


// Pin definitions for input (trigger and echo)
const int TRIGGER_PIN = 2;  // Pin for trigger
const int ECHO_PIN = 3;   // Pin for echo


UltraSonicDistanceSensor distanceSensor(TRIGGER_PIN, ECHO_PIN, 40);
//

void setup() 
{
  
  Serial.begin(9600);
  
  // Small delay allows Serial Monitor to catch output if connected
  delay(1000);
  
  Serial.println("=== DF Pong Controller Starting ===");

  for(int i = 0; i < AVERAGE_WINDOW; i++) readings[i] = 0;
  Serial.println("Distance Motion Detection Started");
  Serial.println("--------------------------------");
 
  // Configure LED for connection status indication
  pinMode(LED_PIN, OUTPUT);
  
  // Initialize Bluetooth Low Energy with device name, number, and status LED
  setupBLE(deviceName, DEVICE_NUMBER, LED_PIN);
  
  // Initialize buzzer for feedback
  setupBuzzer(BUZZER_PIN);
}

void loop() 
{
  // Update BLE connection status and handle incoming data
  updateBLE();
  
  //read the inputs determine the current state
  //results in changing the value of currentMovement
  handleInput();

  //send the movement state to P5  
  sendMovement(currentMovement);

  //make the correct noise
  updateBuzzer(currentMovement);
  
  
}

void handleInput() 
{
//The following code are modified based on the sample code "distance_motionDetection" provided by C&C
//Current movements are assigned as: 0 = still, 1 = move up, 2 = move down
  unsigned long currentTime = millis();
  if (currentTime - lastReadTime >= readInterval) {
    float newDistance = distanceSensor.measureDistanceCm();
    
    Serial.println(newDistance);
    if (newDistance > 0) {
      distance = newDistance;
      
      // Update rolling average
      totalValue = totalValue - readings[readIndex];
      readings[readIndex] = distance;
      totalValue = totalValue + distance;
      readIndex = (readIndex + 1) % AVERAGE_WINDOW;
      lastSmoothedDistance = smoothedDistance;
      smoothedDistance = totalValue / AVERAGE_WINDOW;
      
      // Calculate motion
      float change = smoothedDistance - lastSmoothedDistance;
      totalMotion += abs(change);
      
      // Print verbose status
      Serial.print("Raw Distance: ");
      Serial.print(distance);
      Serial.print(" cm | ");
      
      Serial.print("Smoothed: ");
      Serial.print(smoothedDistance);
      Serial.print(" cm | ");
      
      Serial.print("Current Motion: ");
      Serial.print(abs(change));
      Serial.print(" cm | ");
      
      Serial.print("State: ");
      if(abs(change) < MOTION_THRESHOLD) {
        Serial.print("STILL");
        digitalWrite (LED_PIN, LOW);
        currentMovement = 0; //Stay still
       

      } else if(change < 0) {
        Serial.print("AWAY ");
        digitalWrite(LED_PIN, LOW);
        currentMovement = 2; //Move down

      } else {
        Serial.print("TOWARD");
        digitalWrite (LED_PIN, HIGH);
        currentMovement = 1; //Move up

      }
      Serial.print(" | ");
      
      Serial.print("Total Motion: ");
      Serial.print(totalMotion);
      Serial.println(" cm");
    }
    lastReadTime = currentTime;
  }
}





