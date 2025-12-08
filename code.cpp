/*
 * LINE FOLLOWER BOT - ARDUINO NANO
 * ================================
 * Components:
 * - Arduino Nano
 * - QTR-8RC Line Reflectance Sensor Array
 * - TB6612FNG Dual Motor Driver
 * - 2x N20 Gear Motors
 * - MP1584 DC-DC Converter (to step down battery voltage)
 * - Castor Wheel
 * - LiPo Battery (7.4V recommended)
 * 
 * PINOUT:
 * Arduino Nano -> TB6612FNG Motor Driver
 *   D2 -> AIN1 (Motor A Direction)
 *   D3 -> PWMA (Motor A PWM Speed)
 *   D4 -> AIN2 (Motor A Direction)
 *   D5 -> PWMB (Motor B PWM Speed)
 *   D6 -> BIN1 (Motor B Direction)
 *   D7 -> BIN2 (Motor B Direction)
 *   GND -> GND
 *   5V -> +5V (through MP1584 output)
 *   
 * Arduino Nano -> QTR-8RC Sensor
 *   D8  -> Sensor 1
 *   D9  -> Sensor 2
 *   D10 -> Sensor 3
 *   D11 -> Sensor 4
 *   D12 -> Sensor 5
 *   A0  -> Sensor 6
 *   A1  -> Sensor 7
 *   A2  -> Sensor 8
 *   GND -> GND
 *   5V  -> +5V (through MP1584 output)
 *   
 * TB6612FNG Motor Driver
 *   Motor A (Left) connected to OUT1, OUT2
 *   Motor B (Right) connected to OUT3, OUT4
 *   Dual Power Input: ~9-12V (from battery via MP1584 output)
 *   GND -> Common GND with Arduino
 *   
 * Battery & Power
 *   LiPo Battery (7.4V) -> MP1584 input
 *   MP1584 output (5V) -> Arduino Nano, Motor Driver, Sensors
 */

#include <QTRSensors.h>

// --- Tuning Parameters (ADJUST FOR YOUR ROBOT) ---
// Kp: Proportional gain - faster reaction to error
// Kd: Derivative gain - dampening to reduce overshooting
// BaseSpeed: Nominal forward speed (0-255)
// MaxSpeed: Maximum motor speed limit (0-255)
float Kp = 0.12;   // Start at 0.1, increase gradually for faster response
float Kd = 2.0;    // Start at 2.0, increase if robot wobbles
int BaseSpeed = 155;
int MaxSpeed = 255;

// --- Pin Definitions (Arduino Nano) ---
// Motor A (Left Motor)
const int PWMA = 3;   // PWM for left motor speed
const int AIN1 = 2;   // Left motor direction control 1
const int AIN2 = 4;   // Left motor direction control 2

// Motor B (Right Motor)
const int PWMB = 5;   // PWM for right motor speed
const int BIN1 = 6;   // Right motor direction control 1
const int BIN2 = 7;   // Right motor direction control 2

// LED for White Box Detection
const int LED_PIN = 13;  // Built-in LED on Arduino Nano

// QTR-8RC Reflectance Sensor Array
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
const uint16_t CENTER_POSITION = 3500;  // Center value of QTR reading (0-7000)
const uint16_t WHITE_BOX_THRESHOLD = 6500;  // Threshold to detect white surface

void setup() {
  // Initialize Serial Communication for debugging
  Serial.begin(9600);
  
  // 1. Motor Pins Setup (configure as outputs)
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 2. QTR-8RC Sensor Setup
  // Configure sensor type as RC (resistor-capacitor)
  qtr.setTypeRC();
  // Set the 8 sensor pins (digital and analog pins)
  qtr.setSensorPins((const uint8_t[]){8, 9, 10, 11, 12, A0, A1, A2}, SensorCount);
  // Optional: set emitter control pin if needed
  // qtr.setEmitterPin(13);

  // 3. Calibration Routine
  // Blink LED during calibration to indicate robot is ready
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.println("\n=== LINE FOLLOWER BOT INITIALIZED ===");
  Serial.println("Calibrating sensors...");
  Serial.println("Move robot back and forth over the line for 2-3 seconds.");
  delay(1000);
  
  // Calibrate: read line and background 400 times
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }
  
  digitalWrite(LED_BUILTIN, LOW); 
  Serial.println("Calibration Complete!");
  Serial.println("Starting line following...\n");
  delay(1000);
}

// Variables for PID control
int lastError = 0;

void loop() {
  // 1. Read Line Position from QTR Sensor
  // Returns 0-7000: 0=far left, 3500=center, 7000=far right
  uint16_t position = qtr.readLineBlack(sensorValues);

  // 2. Calculate PID Error
  // Positive error: line is to the right (turn right)
  // Negative error: line is to the left (turn left)
  int error = position - CENTER_POSITION;
  
  // PID calculation: proportional + derivative terms
  int motorSpeed = (Kp * error) + (Kd * (error - lastError));
  lastError = error;

  // 3. Calculate Individual Motor Speeds
  // Reduce right motor when error is positive (line to right)
  // Reduce left motor when error is negative (line to left)
  int rightMotorSpeed = BaseSpeed + motorSpeed;
  int leftMotorSpeed = BaseSpeed - motorSpeed;

  // 4. Constrain Speeds to Safe Range (0-MaxSpeed)
  rightMotorSpeed = constrain(rightMotorSpeed, 0, MaxSpeed);
  leftMotorSpeed = constrain(leftMotorSpeed, 0, MaxSpeed);

  // 5. Apply Motor Speeds via TB6612FNG Driver
  setMotorSpeeds(leftMotorSpeed, rightMotorSpeed);
  
  // 6. Detect White Box and Turn On LED
  if (isWhiteBoxDetected(sensorValues)) {
    digitalWrite(LED_PIN, HIGH);  // Turn LED on when white box detected
  }
}

/*
 * White Box Detection Function
 * 
 * Detects if the robot has reached a white box by checking if all sensors
 * read high values (indicating a bright/white surface)
 */
bool isWhiteBoxDetected(uint16_t* sensors) {
  uint8_t whiteCount = 0;
  // Count how many sensors detect white (high values)
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensors[i] > WHITE_BOX_THRESHOLD) {
      whiteCount++;
    }
  }
  // If at least 6 out of 8 sensors detect white, it's the white box
  return (whiteCount >= 6);
}

/*
 * LED On Function
 * 
 * Turns the LED on to indicate white box detection
 * The LED will stay on once the white box is detected
 */
void turnOnLED() {
  digitalWrite(LED_PIN, HIGH);
}

/*
 * Motor Control Function
 * 
 * Controls direction and speed of both motors via TB6612FNG driver
 * Motor A = Left Motor
 * Motor B = Right Motor
 * 
 * Direction Control:
 *   AIN1=HIGH, AIN2=LOW  -> Motor A Forward
 *   AIN1=LOW,  AIN2=HIGH -> Motor A Backward
 *   
 *   BIN1=HIGH, BIN2=LOW  -> Motor B Forward
 *   BIN1=LOW,  BIN2=HIGH -> Motor B Backward
 * 
 * Speed Control: PWM value (0-255)
 */
void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  // Motor A (Left) - Forward Direction
  digitalWrite(AIN1, HIGH);   // Direction pin 1
  digitalWrite(AIN2, LOW);    // Direction pin 2
  analogWrite(PWMA, leftSpeed);  // Speed control via PWM (0-255)

  // Motor B (Right) - Forward Direction
  digitalWrite(BIN1, HIGH);   // Direction pin 1
  digitalWrite(BIN2, LOW);    // Direction pin 2
  analogWrite(PWMB, rightSpeed); // Speed control via PWM (0-255)
}

/*
 * Optional: Function to stop all motors
 */
void stopMotors() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);
  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0);
}