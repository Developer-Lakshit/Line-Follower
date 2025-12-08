# LED Blinking on White Box Detection

## Modification for Competition
According to competition requirements, the robot needs an LED that blinks when it reaches the white box.

## Implementation Details

### Hardware Setup
- **LED Pin**: Pin 13 (Arduino Nano built-in LED)
- **Connection**: Digital output pin on Arduino Nano
- **LED**: Can use built-in LED or external LED with appropriate current limiting resistor

### Code Implementation

#### 1. Pin Definition
```cpp
const int LED_PIN = 13;  // Built-in LED on Arduino Nano
const uint16_t WHITE_BOX_THRESHOLD = 6500;  // Threshold to detect white surface
```

#### 2. LED Initialization (in setup())
```cpp
pinMode(LED_PIN, OUTPUT);
digitalWrite(LED_PIN, LOW);
```

#### 3. White Box Detection Function
```cpp
bool isWhiteBoxDetected(uint16_t* sensors) {
  uint8_t whiteCount = 0;
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensors[i] > WHITE_BOX_THRESHOLD) {
      whiteCount++;
    }
  }
  return (whiteCount >= 6);  // If 6+ sensors detect white
}
```

#### 4. LED On Function
```cpp
void turnOnLED() {
  digitalWrite(LED_PIN, HIGH);
}
```

#### 5. Detection Logic (in loop())
```cpp
if (isWhiteBoxDetected(sensorValues)) {
  digitalWrite(LED_PIN, HIGH);  // Turn LED on when white box detected
}
```

## How It Works
1. Robot follows the black line as normal using PID control
2. When the robot reaches the white box (destination), all 8 line sensors detect the white surface
3. Detection threshold: At least 6 out of 8 sensors must read high values (>6500)
4. Upon detection, the LED turns on and stays on continuously
5. The LED remains on as long as the white box is detected
6. The robot continues line following even after reaching the white box

## Customization Options
- **Detection Sensitivity**: Adjust `WHITE_BOX_THRESHOLD` (higher = less sensitive)
- **Required Sensors**: Change the `6` in `whiteCount >= 6` (current requirement is 6 out of 8)
- **LED Reset**: To turn off the LED, add `digitalWrite(LED_PIN, LOW);` in the else condition if needed