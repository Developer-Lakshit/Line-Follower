#include <QTRSensors.h>

// --- Tuning Parameters (TUNE THESE FOR YOUR ROBOT) ---
// Kp: Response to current error. Higher = faster reaction but more wobbling.
// Kd: Damping. Helps reduce overshooting.
// BaseSpeed: The normal forward speed (0-255).
// MaxSpeed: The maximum allowed speed (0-255).
float Kp = 0.1;   // Start with 0.1, increase slowly (e.g., 0.15, 0.2)
float Kd = 2.0;   // Start with 2.0, adjust if robot wobbles
int BaseSpeed = 150; 
int MaxSpeed = 200;

// --- Pin Definitions ---
// Motor A (Left)
const int PWMA = 3;
const int AIN1 = 2;
const int AIN2 = 4;

// Motor B (Right)
const int PWMB = 5;
const int BIN1 = 6;
const int BIN2 = 7;

// Sensors
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

void setup() {
  // 1. Motor Pins Setup
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  // 2. Sensor Setup
  // We use D8-D12 and A0-A2
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){8, 9, 10, 11, 12, A0, A1, A2}, SensorCount);

  // 3. Calibration
  // The LED on the Nano will turn on during calibration.
  // Move the robot back and forth over the line so all sensors see black and white.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.begin(9600);
  Serial.println("Calibrating... Move robot over line.");
  
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }
  
  digitalWrite(LED_BUILTIN, LOW); 
  Serial.println("Calibration Done.");
  delay(1000);
}

// Variables for PID
int lastError = 0;

void loop() {
  // 1. Get Line Position
  // Returns 0 to 7000. Center is 3500.
  uint16_t position = qtr.readLineBlack(sensorValues);

  // 2. Calculate PID
  int error = position - 3500;
  int motorSpeed = Kp * error + Kd * (error - lastError);
  lastError = error;

  // 3. Calculate Motor Speeds
  int rightMotorSpeed = BaseSpeed + motorSpeed;
  int leftMotorSpeed = BaseSpeed - motorSpeed;

  // 4. Constrain Speeds (Keep within 0-MaxSpeed)
  if (rightMotorSpeed > MaxSpeed) rightMotorSpeed = MaxSpeed;
  if (leftMotorSpeed > MaxSpeed) leftMotorSpeed = MaxSpeed;
  if (rightMotorSpeed < 0) rightMotorSpeed = 0;
  if (leftMotorSpeed < 0) leftMotorSpeed = 0;

  // 5. Drive Motors
  setMotorSpeeds(leftMotorSpeed, rightMotorSpeed);
}

// Function to control driver pins
void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  // Motor A (Left)
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, leftSpeed);

  // Motor B (Right)
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, rightSpeed);
}