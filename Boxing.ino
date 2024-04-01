/*
description:

Punch Detection: The player throws a punch when the Z-axis acceleration data from the MPU6050 accelerometer exceeds a certain threshold. Each punch is counted.

LED Feedback: Upon detecting a punch, both LEDs connected to pins D3 and D4 will flash to provide visual feedback.

Random Timing for Defense: After a punch is detected, the game waits for a random duration between 2 to 5 seconds. Then, it activates a buzzer on either pin D5 or D6.

Parry Defense: The buzzer's activation indicates the direction of the expected "parry" defense. The player must tilt the device in the correct direction (detected via the X-axis acceleration) to successfully parry. If the tilt exceeds a certain threshold, the parry is considered successful.

AI Opponent: The player is fighting against an AI opponent. The AI's health is represented by the brightness of an LED connected to pin 9. A successful punch by the player decreases the AI's health.

Player Health: The player's health is represented by the brightness of an LED connected to pin 10. If the player fails to parry successfully, their health decreases.

Game Over: The game continues until either the player's or the AI's health is depleted.

*/

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// Pin definitions
const int ledPin1 = 3;
const int ledPin2 = 4;
const int buzzerPin1 = 5;
const int buzzerPin2 = 6;
const int aiHealthLedPin = 9;
const int playerHealthLedPin = 10;

int cycleTime = 20; // Set the PWM cycle time in milliseconds


// Thresholds
const int punchThreshold = 15; // Arbitrary threshold for punch detection
const int parryThreshold = 15; // Arbitrary threshold for parry detection

// Health
int aiHealth = 255; // AI health starts at max brightness
int playerHealth = 255; // Player health starts at max brightness


void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.begin();

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(buzzerPin1, OUTPUT);
  pinMode(buzzerPin2, OUTPUT);
  pinMode(aiHealthLedPin, OUTPUT);
  pinMode(playerHealthLedPin, OUTPUT);
}

void loop() {
  analogWrite(aiHealthLedPin, aiHealth);
  analogWrite(playerHealthLedPin, playerHealth);
  delay(100);
  
  // Read accelerometer 
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  int ax = a.acceleration.x;
  int ay = a.acceleration.y;
  int az = a.acceleration.z;
  //mpu.getAcceleration(&ax, &ay, &az);
  Serial.print("A_z: ");
  Serial.println(az);

  // Check for punch
  if (az > punchThreshold) {
    Serial.print("Punch Started");
    aiHealth = aiHealth - 150;


    // Flash LEDs
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    for (int c = 0; c < 10; c++) {
      digitalWrite(aiHealthLedPin, HIGH);
      delay(100);
      digitalWrite(aiHealthLedPin, LOW);
      delay(100);
    }
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);

    // Wait for a random time between 2 to 5 seconds
    delay(random(2000, 5001));

    // Activate buzzer for parry indication
    int buzzerPin = random(2) ? buzzerPin1 : buzzerPin2;
    if (buzzerPin) {
      digitalWrite(ledPin1, HIGH);
      delay(100);
      digitalWrite(ledPin1, LOW);
    } else {
      digitalWrite(ledPin2, HIGH);
      delay(100);
      digitalWrite(ledPin2, LOW);
    }
    digitalWrite(buzzerPin, HIGH);
    delay(1000); // Buzzer on for 1 second
    digitalWrite(buzzerPin, LOW);

    // Check for parry
    if ((buzzerPin == buzzerPin1 && ax < -parryThreshold) || (buzzerPin == buzzerPin2 && ax > parryThreshold)) {
      // Successful parry, decrease AI health
      aiHealth = max(0, aiHealth - 25);
      analogWrite(aiHealthLedPin, aiHealth);
    } else {
      // Failed parry, decrease player health
      playerHealth = max(0, playerHealth - 125);
      analogWrite(playerHealthLedPin, playerHealth);
      for (int c = 0; c < 10; c++) {
      digitalWrite(playerHealthLedPin, HIGH);
      delay(100);
      digitalWrite(playerHealthLedPin, LOW);
      delay(100);
    }
    }
  }

  // Check for game over
  if (aiHealth == 0 || playerHealth == 0) {
    // Game over, flash all LEDs rapidly
    for (int i = 0; i < 10; i++) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(aiHealthLedPin, HIGH);
      digitalWrite(playerHealthLedPin, HIGH);
      delay(100);
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
      digitalWrite(aiHealthLedPin, LOW);
      digitalWrite(playerHealthLedPin, LOW);
      delay(100);
    }
    // Reset health for next game
    aiHealth = 255;
    playerHealth = 255;
    analogWrite(aiHealthLedPin, aiHealth);
    analogWrite(playerHealthLedPin, playerHealth);
  }
}