# LINE FOLLOWER BOT - QUICK REFERENCE GUIDE

## ✅ COMPLETED DELIVERABLES

### 1. **C++ Code** (`code.cpp`)
   - Fully documented line follower algorithm
   - PID control for smooth line following
   - Motor control via TB6612FNG driver
   - QTR-8RC sensor integration
   - Calibration routine included

### 2. **Schematic Diagram** (`schematic.svg`)
   - Complete visual block diagram
   - All pin connections clearly labeled
   - Power distribution shown
   - Motor control truth tables
   - PID algorithm flowchart

### 3. **Schematic Connections Documentation** (`SCHEMATIC_CONNECTIONS.md`)
   - Detailed pinout tables
   - Motor control truth tables
   - Power distribution diagram
   - Component specifications
   - Tuning guidelines

---

## 🔌 QUICK PINOUT REFERENCE

### Motor Driver (TB6612FNG)
```
Arduino D2  → AIN1  (Motor A Direction)
Arduino D3  → PWMA  (Motor A Speed)
Arduino D4  → AIN2  (Motor A Direction)
Arduino D5  → PWMB  (Motor B Speed)
Arduino D6  → BIN1  (Motor B Direction)
Arduino D7  → BIN2  (Motor B Direction)
GND → GND
5V (from MP1584) → +5V Input
```

### Line Sensor (QTR-8RC)
```
Arduino D8  → Sensor 1
Arduino D9  → Sensor 2
Arduino D10 → Sensor 3
Arduino D11 → Sensor 4
Arduino D12 → Sensor 5
Arduino A0  → Sensor 6
Arduino A1  → Sensor 7
Arduino A2  → Sensor 8
5V (from MP1584) → VCC
GND → GND
```

### Power (MP1584 DC-DC Converter)
```
Input:  LiPo Battery 7.4V
Output: 5V to Arduino Nano, Motor Driver, QTR Sensor
GND:    Common ground to all components
```

---

## 🔧 CODE PIN MAPPING VERIFICATION

✅ **Motor A (Left Motor)**
- `PWMA = 3`   ← Speed PWM
- `AIN1 = 2`   ← Direction 1
- `AIN2 = 4`   ← Direction 2

✅ **Motor B (Right Motor)**
- `PWMB = 5`   ← Speed PWM
- `BIN1 = 6`   ← Direction 1
- `BIN2 = 7`   ← Direction 2

✅ **QTR Sensor Array**
- Pins: D8, D9, D10, D11, D12, A0, A1, A2
- Return value: 0-7000 (0=far left, 3500=center, 7000=far right)

---

## 📊 MOTOR CONTROL LOGIC

### Forward Motion (Both Motors)
```cpp
AIN1=HIGH, AIN2=LOW   → Motor A Forward
BIN1=HIGH, BIN2=LOW   → Motor B Forward
PWMA, PWMB = 0-255    → Speed control
```

### Line Following Adjustment
```
If line drifts RIGHT:
  - error > 0
  - Reduce right motor speed
  - Increase left motor speed
  - Turn LEFT back to line

If line drifts LEFT:
  - error < 0
  - Increase right motor speed
  - Reduce left motor speed
  - Turn RIGHT back to line
```

---

## 🎯 PID TUNING PARAMETERS

| Parameter | Value | Notes |
|-----------|-------|-------|
| Kp (Proportional) | 0.12 | Increase for faster response, decrease if oscillating |
| Kd (Derivative) | 2.0 | Increase to reduce overshooting |
| BaseSpeed | 155 | Nominal forward speed (0-255) |
| MaxSpeed | 255 | Speed limit (0-255) |
| CENTER_POSITION | 3500 | QTR center value (0-7000 range) |

---

## 🚀 GETTING STARTED

### 1. **Upload Code**
   - Install QTRSensors library in Arduino IDE
   - Connect Arduino Nano via USB
   - Upload `code.cpp` to board

### 2. **Hardware Assembly**
   - Wire all connections per `SCHEMATIC_CONNECTIONS.md`
   - Double-check all connections match the schematic
   - Ensure MP1584 outputs exactly 5V

### 3. **Calibration**
   - Place robot on line
   - Power on - LED will blink during calibration
   - Move robot back and forth over line for 2-3 seconds
   - Wait for calibration to complete

### 4. **Testing**
   - Place robot on track
   - Monitor Serial output (9600 baud) for line position values
   - Adjust Kp and Kd parameters if needed
   - Fine-tune BaseSpeed for optimal speed

---

## ⚠️ TROUBLESHOOTING

| Problem | Solution |
|---------|----------|
| Robot doesn't follow line | Recalibrate sensor over actual track |
| Robot oscillates/wobbles | Decrease Kp, increase Kd |
| Robot too slow | Increase BaseSpeed |
| Robot loses line easily | Increase Kp, check sensor height (2-3mm) |
| Motor speeds unequal | Check motor balance, recalibrate |
| No response to line | Verify all pin connections |

---

## 📋 VERIFICATION CHECKLIST

- [ ] All pins match between code and schematic
- [ ] Motor A (Left): D2, D3, D4
- [ ] Motor B (Right): D5, D6, D7
- [ ] QTR Sensor: D8, D9, D10, D11, D12, A0, A1, A2
- [ ] Power connections verified
- [ ] MP1584 output = 5V
- [ ] All motors connected to correct driver outputs
- [ ] Ground connections common to all components
- [ ] QTR sensor mounted 2-3mm above ground
- [ ] Code compiles without errors
- [ ] Calibration runs successfully

---

## 📁 FILES INCLUDED

1. **code.cpp** - Complete Arduino Nano C++ code with comments
2. **schematic.svg** - Visual schematic diagram (SVG format)
3. **SCHEMATIC_CONNECTIONS.md** - Detailed connection documentation
4. **README.md** - Project overview
5. **Circuit Diagram.png** - Additional circuit reference

---

## 🔗 COMPONENT CONNECTIONS SUMMARY

```
LiPo Battery (7.4V)
         ↓
    MP1584 Converter
    (5V Output)
         ↓
    ┌────┬────┬────┐
    ↓    ↓    ↓    ↓
Arduino  Motor QTR  GND
Nano     Driver Sensor
    ↓
TB6612FNG
    ↓
Motor A ←→ Motor B
(Left)     (Right)
    ↓        ↓
  Wheel    Wheel
```

All connections have been verified to match the C++ code implementation.

