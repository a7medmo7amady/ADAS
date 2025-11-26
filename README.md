# **ADAS-RC: Advanced Driver-Assistance System for RC Vehicle**

A compact, RTOS-inspired embedded implementation of **Advanced Driver-Assistance Systems (ADAS)** using an **ATmega32** microcontroller as the primary ECU. The system handles throttle control, braking logic, collision avoidance, automatic headlights, pre-trip safety checks, LCD display, and Bluetooth telemetry. An **ESP32** is used for **TinyML lane detection** and visual perception.

---

## **Features**

### **1. Drive Control**

* ADC-based accelerator pedal input
* PWM motor control using ATmega32 timers
* Smooth acceleration with rate limiting
* Optional PI/PID motor control
* Real-time speed shown on LCD

### **2. Braking System**

* Brake input overrides accelerator via semaphores/flags
* Smooth deceleration for realistic dynamics
* Emergency braking sequence

### **3. Collision Avoidance**

* Ultrasonic sensor distance measurement
* Time-To-Collision (TTC) calculations
* Automatic deceleration or full braking when TTC is unsafe

### **4. Ambient Light & Headlight Control**

* LDR/photodiode input using ADC
* Automatic headlights ON/OFF
* LCD light-status indicator

### **5. Pre-Trip Safety Checks**

* Seat belt sensor
* Door lock sensor
* Vehicle will not move unless all checks pass

### **6. Bluetooth Telemetry**

* BLE module for continuous reporting:

  * Speed
  * Distance
  * Brake events
  * Light status
  * Safety checks
* Real-time monitoring via laptop/mobile

### **7.TinyML Lane Detection (ESP32)**

* ESP32 handles camera input
* Lightweight TinyML model for lane estimation
* ESP32 sends lane-adjustment hints back to ATmega32 (UART/I2C)

---

## **System Architecture**

### **Microcontroller Roles**

| Component                                                   | MCU          | Role                |
| ----------------------------------------------------------- | ------------ | ------------------- |
| Motor control, braking, TTC, lights, safety, LCD, telemetry | **ATmega32** | Main ECU            |
| Camera processing, inference, ML lane detection             | **ESP32**    | Vision co-processor |

Communication: UART between ESP32 and ATmega32.

### **ATmega32 Task Model (RTOS-Inspired Structure)**

Although ATmega32 does not run a full RTOS, the firmware uses *task scheduling patterns* and prioritized execution:

| "Task"            | Priority | Responsibilities                    |
| ----------------- | -------- | ----------------------------------- |
| Emergency/Safety  | Highest  | Braking override, fault flags       |
| Motor Control     | High     | PWM, ramp control, PI/PID           |
| Sensor Processing | Mid-High | Ultrasonic, LDR, encoder            |
| Input Handling    | Mid      | Accelerator ADC, brake switch       |
| LCD/UI            | Mid-Low  | Dashboard updates                   |
| Telemetry         | Low      | BLE packets                         |
| ML Input Handler  | Low      | Receive lane corrections from ESP32 |

---

## **Core Algorithms (ATmega32)**

### **Speed, Acceleration & Braking**

* Ramp limiter for smooth throttle
* PI/PID loop for motor control
* Emergency braking cuts throttle immediately

### **Collision Avoidance**

```
t_stop = v / a
d_stop = v² / (2a)
if d_obs <= d_stop + margin → emergency brake
```

or TTC:

```
TTC = d_obs / v
if TTC < t_stop + margin → brake
```

### **Sensor Filtering**

* Moving average / low-pass filtering for noisy ADC
* Median-filter for ultrasonic readings

---

## **Hardware Components**

* **ATmega32** microcontroller
* **ESP32** for TinyML
* DC motor + driver
* Ultrasonic sensor (HC-SR04)
* Light sensor (LDR)
* LCD (16×2 or 20×4, I2C)
* BLE module
* Seatbelt & door lock sensors
* Camera for lane detection (ESP32-Cam)

---

## **Non-Volatile Storage (ATmega32)**

Stored via EEPROM/flash emulation:

* ADC calibration
* Light switching threshold
* PID gains
* Stopping margin constants
* BLE pairing/config
* Fault snapshots with CRC

---

## **Fault Handling**

* Hardware kill-switch
* Watchdog timer enabled
* Brown-out reset
* BLE loss detection
* Emergency braking override

---

## **Testing**

* Individual module tests (motor, ultrasonic, BLE)
* TTC validation at various speeds
* Safety-check tests (seatbelt/door)
* Telemetry consistency tests
* ESP32 → ATmega32 lane-data integration testing

---

## **Example Telemetry Packet (ATmega32, BLE)**

```json
{
  "speed": 42,
  "distance": 76,
  "brake": 1,
  "lights": 0,
  "seatbelt": true,
  "door": true,
  "fault": 0
}
```

---

## **How to Run**

1. Flash ATmega32 firmware
2. Flash TinyML model to ESP32 (optional)
3. Power vehicle & connect BLE logger
4. Use accelerator pedal to drive
5. ATmega32 handles safety, TTC, braking, and lights
6. ESP32 performs lane detection and sends corrections

---
