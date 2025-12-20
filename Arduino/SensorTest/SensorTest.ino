/*
  Arduino Uno + L298N + HC-SR04 - Obstacle Avoidance Robot
  
  Logic:
  1. Forward clear → move forward
  2. Forward blocked → turn right
  3. Right blocked → turn left
  4. All blocked → spin 180°
  
  Wiring:
    L298N: ENA->D9, IN1->D3, IN2->D4, ENB->D10, IN3->D5, IN4->D6
    HC-SR04: TRIG->D12, ECHO->D11, VCC->5V, GND->GND
*/

// Motor Driver Pins
const int ENA = 9;
const int IN1 = 3;
const int IN2 = 4;
const int ENB = 10;
const int IN3 = 5;
const int IN4 = 6;

// Ultrasonic Sensor Pins
const int TRIG_PIN = 12;
const int ECHO_PIN = 11;

// Parameters
const int OBSTACLE_CM = 15;
const int DRIVE_PWM = 200;
const int TURN_TIME = 800;       
const int SPIN_TIME = 1500;     
const int CHECK_TURN_TIME = 300; 
/**
 * Read distance from HC-SR04
 */
long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  unsigned long us = pulseIn(ECHO_PIN, HIGH, 25000UL);
  
  if (us == 0) return 999;  // No echo = path clear
  
  long cm = us / 58;
  return (cm >= 2 && cm <= 200) ? cm : 999;
}

/**
 * Motor control function
 */
void setMotors(int leftSpeed, int rightSpeed) {
  // Left motor
  if (leftSpeed >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, leftSpeed);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, -leftSpeed);
  }
  
  // Right motor
  if (rightSpeed >= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, rightSpeed);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, -rightSpeed);
  }
}

void moveForward() {
  setMotors(DRIVE_PWM, DRIVE_PWM);
}

void stopMotors() {
  setMotors(0, 0);
}

void turnRight() {
  setMotors(DRIVE_PWM, DRIVE_PWM / 3);
  delay(TURN_TIME);
}

void turnLeft() {
  setMotors(DRIVE_PWM / 3, DRIVE_PWM);
  delay(TURN_TIME);
}

void spin180() {
  setMotors(DRIVE_PWM, -DRIVE_PWM);
  delay(SPIN_TIME);
  stopMotors();
}

/**
 * Turn briefly to check side, measure, then turn back
 */
long checkRight() {
  setMotors(DRIVE_PWM, -DRIVE_PWM);  // Pivot right
  delay(CHECK_TURN_TIME);
  stopMotors();
  delay(100);
  
  long dist = readDistanceCm();
  
  setMotors(-DRIVE_PWM, DRIVE_PWM);  // Pivot back
  delay(CHECK_TURN_TIME);
  stopMotors();
  delay(100);
  
  return dist;
}

long checkLeft() {
  setMotors(-DRIVE_PWM, DRIVE_PWM);  // Pivot left
  delay(CHECK_TURN_TIME);
  stopMotors();
  delay(100);
  
  long dist = readDistanceCm();
  
  setMotors(DRIVE_PWM, -DRIVE_PWM);  // Pivot back
  delay(CHECK_TURN_TIME);
  stopMotors();
  delay(100);
  
  return dist;
}

void setup() {
  Serial.begin(9600);
  Serial.println("=== Obstacle Avoidance Robot ===");
  
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  stopMotors();
  delay(1000);
  Serial.println("Ready!");
}

void loop() {
  long distance = readDistanceCm();
  
  Serial.print("Forward: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  if (distance > OBSTACLE_CM) {
    // Path clear - move forward
    moveForward();
    Serial.println("-> FORWARD");
  } 
  else {
    // Forward blocked - stop and check sides
    stopMotors();
    delay(200);
    
    Serial.println("Blocked! Checking right...");
    long rightDist = checkRight();
    Serial.print("Right: ");
    Serial.print(rightDist);
    Serial.println(" cm");
    
    if (rightDist > OBSTACLE_CM) {
      Serial.println("-> TURN RIGHT");
      turnRight();
    } 
    else {
      Serial.println("Right blocked. Checking left...");
      long leftDist = checkLeft();
      Serial.print("Left: ");
      Serial.print(leftDist);
      Serial.println(" cm");
      
      if (leftDist > OBSTACLE_CM) {
        Serial.println("-> TURN LEFT");
        turnLeft();
      } 
      else {
        Serial.println("-> SPIN 180");
        spin180();
      }
    }
  }
  
  delay(50);
}