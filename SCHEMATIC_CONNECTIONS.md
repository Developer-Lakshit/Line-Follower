# LINE FOLLOWER BOT - SCHEMATIC CONNECTIONS GUIDE

## Complete Component List

| Component | Model | Quantity | Purpose |
|-----------|-------|----------|---------|
| Microcontroller | Arduino Nano | 1 | Main control unit |
| Line Sensor | QTR-8RC Reflectance Sensor | 1 | Detects black line (8 sensors) |
| Motor Driver | TB6612FNG Dual Motor Driver | 1 | Controls motor speed & direction |
| DC Motors | N20 Gear Motors | 2 | Left and Right wheel drive |
| Power Converter | MP1584 DC-DC Step-Down | 1 | Converts 7.4V to 5V |
| Battery | LiPo 7.4V (2S) | 1 | Power source |
| Wheel | Castor Wheel | 1 | Rear support wheel |

---

## PINOUT DIAGRAM

### Arduino Nano → TB6612FNG Motor Driver

| Arduino Pin | Signal | TB6612FNG Pin | Motor | Function |
|------------|--------|---------------|-------|----------|
| D2 | AIN1 | AIN1 | Motor A (Left) | Direction Control 1 |
| D3 | PWMA | PWMA | Motor A (Left) | Speed Control (PWM) |
| D4 | AIN2 | AIN2 | Motor A (Left) | Direction Control 2 |
| D5 | PWMB | PWMB | Motor B (Right) | Speed Control (PWM) |
| D6 | BIN1 | BIN1 | Motor B (Right) | Direction Control 1 |
| D7 | BIN2 | BIN2 | Motor B (Right) | Direction Control 2 |
| GND | GND | GND | - | Common Ground |
| 5V | 5V | +5V | - | Power (from MP1584) |

### Arduino Nano → QTR-8RC Reflectance Sensor

| Arduino Pin | Sensor | Type | Function |
|------------|--------|------|----------|
| D8 | Sensor 1 | Digital | IR Reflectance Detection |
| D9 | Sensor 2 | Digital | IR Reflectance Detection |
| D10 | Sensor 3 | Digital | IR Reflectance Detection |
| D11 | Sensor 4 | Digital | IR Reflectance Detection |
| D12 | Sensor 5 | Digital | IR Reflectance Detection |
| A0 | Sensor 6 | Analog | IR Reflectance Detection |
| A1 | Sensor 7 | Analog | IR Reflectance Detection |
| A2 | Sensor 8 | Analog | IR Reflectance Detection |
| 5V | VCC | Power | +5V Supply (from MP1584) |
| GND | GND | Power | Common Ground |

---

## POWER DISTRIBUTION

```
LiPo Battery (7.4V)
        ↓
    MP1584 DC-DC Converter
        ↓
    5V Output
        ├→ Arduino Nano
        ├→ QTR-8RC Sensor
        └→ TB6612FNG Driver
        
TB6612FNG Motor Outputs:
        ├→ OUT1, OUT2 → Motor A (Left N20)
        └→ OUT3, OUT4 → Motor B (Right N20)
```

---

## MOTOR CONTROL TRUTH TABLE

### Motor A (Left Motor)
| AIN1 | AIN2 | PWMA | Result |
|------|------|------|--------|
| HIGH | LOW | 0-255 | Forward (speed controlled) |
| LOW | HIGH | 0-255 | Backward (speed controlled) |
| LOW | LOW | X | Stop |
| HIGH | HIGH | X | Stop |

### Motor B (Right Motor)
| BIN1 | BIN2 | PWMB | Result |
|------|------|------|--------|
| HIGH | LOW | 0-255 | Forward (speed controlled) |
| LOW | HIGH | 0-255 | Backward (speed controlled) |
| LOW | LOW | X | Stop |
| HIGH | HIGH | X | Stop |

---

## CODE PIN MAPPING

```cpp
// Motor A (Left)
const int PWMA = 3;   // PWM control
const int AIN1 = 2;   // Direction pin 1
const int AIN2 = 4;   // Direction pin 2

// Motor B (Right)
const int PWMB = 5;   // PWM control
const int BIN1 = 6;   // Direction pin 1
const int BIN2 = 7;   // Direction pin 2

// QTR Sensor Array
// D8, D9, D10, D11, D12, A0, A1, A2 → 8 sensors
```

---

## LINE FOLLOWING ALGORITHM

```
1. READ SENSOR INPUT
   → QTR reads line position (0-7000)
   → 0 = far left, 3500 = center, 7000 = far right

2. CALCULATE ERROR
   error = position - 3500

3. CALCULATE CORRECTION
   motorSpeed = Kp × error + Kd × (error - lastError)
   (Kp = 0.12, Kd = 2.0)

4. SET MOTOR SPEEDS
   leftSpeed = BaseSpeed - motorSpeed
   rightSpeed = BaseSpeed + motorSpeed

5. APPLY TO MOTORS
   → Set direction pins (AIN1, AIN2, BIN1, BIN2)
   → Apply PWM speeds (PWMA, PWMB)

6. REPEAT ~100 times per second
```

### Control Logic Flow

```
Line to Right (error > 0)
    ↓
motorSpeed increases (positive)
    ↓
rightSpeed = BaseSpeed + motorSpeed (FASTER)
leftSpeed = BaseSpeed - motorSpeed (SLOWER)
    ↓
Robot turns RIGHT to follow line
    ↓
Error decreases, motorSpeed decreases
    ↓
Speeds equalize, robot continues straight


Line to Left (error < 0)
    ↓
motorSpeed decreases (negative)
    ↓
rightSpeed = BaseSpeed + motorSpeed (SLOWER)
leftSpeed = BaseSpeed - motorSpeed (FASTER)
    ↓
Robot turns LEFT to follow line
    ↓
Error increases, motorSpeed becomes less negative
    ↓
Speeds equalize, robot continues straight
```

---

## HARDWARE ASSEMBLY NOTES

### Physical Layout
1. **Arduino Nano**: Mount on top or side of chassis
2. **TB6612FNG**: Mount near motors for short wiring
3. **QTR-8RC Sensor**: Mount on front underside (2-3mm above ground, perpendicular to line direction)
4. **N20 Motors**: Mount on sides with wheels
5. **MP1584 Converter**: Mount near battery for minimal voltage drop
6. **Castor Wheel**: Mount at rear for stability

### Wiring Best Practices
- Use colored wires: Red (5V), Black (GND), Other (Signal)
- Keep motor wires short to minimize noise
- Separate power and signal wires
- Add capacitors (100µF) near power inputs for stability
- Use connectors (JST/XT60) for battery for easy disconnect

### Tuning PID Parameters

**If robot oscillates/wobbles:**
- Decrease Kp (0.10 or lower)
- Increase Kd (2.5 or higher)

**If robot doesn't respond to line:**
- Increase Kp (0.15 or higher)
- Decrease Kd (1.5 or lower)

**If robot loses line easily:**
- Increase BaseSpeed
- Calibrate QTR sensor over actual track

---

## DEBUGGING CHECKLIST

- [ ] All connections match schematic
- [ ] QTR sensor calibrated over actual line
- [ ] Motor directions correct (both should move forward when commanded)
- [ ] Motor speeds can be controlled independently
- [ ] Battery voltage is 7.4V
- [ ] MP1584 output is 5V
- [ ] Serial monitor shows line position values
- [ ] PID values tuned for your specific robot

