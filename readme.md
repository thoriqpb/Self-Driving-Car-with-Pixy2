# 🚗 Pixy2 Line Follower Robot with OLED Menu

<div align="center">
  <img src="images/pixyLineDetection.gif" alt="Demo of the Car in Action">
</div>

This project is a **line follower robot** using an **ESP32**, **Pixy2 camera**, and **OLED display** with an interactive button-based menu system. It supports **PID steering**, **servo control**, and **parameter configuration** for precision navigation. The menu is displayed on an OLED and controlled using four buttons.

---

## 📦 Features

- 📋 OLED menu system with 4 buttons (UP, DOWN, OK, CANCEL)
- 🔧 PID configuration: Kp, Ki, Kd
- ⚙️ Adjustable speed and smoothing (Alpha)
- 🎯 Pixy2 vector-based line tracking (X-center = 39)
- 🧠 Ackermann-style steering using servo
- 👀 Animating neck servos (vertical & horizontal)
- 🔊 Buzzer for feedback
- 📺 OLED shows live PID and motion data

---

## 🔌 Hardware Required

| Component             | Details                     |
|----------------------|-----------------------------|
| Microcontroller      | ESP32                       |
| Camera Module        | Pixy2                       |
| Display              | OLED 128x64 (SSD1306, I2C)  |
| Drive Motors         | 2x DC Motors + Motor Driver |
| Steering Servo       | 1x Servo (GPIO 33)          |
| Neck Servos          | 2x Servos (GPIO 14, 15)     |
| Buttons              | 4x Push Buttons             |
| Buzzer               | Active Buzzer               |

---

## ⚙️ Pin Configuration

| Purpose            | ESP32 Pin |
|--------------------|-----------|
| Button UP          | GPIO 4    |
| Button DOWN        | GPIO 2    |
| Button OK          | GPIO 5    |
| Button CANCEL      | GPIO 12   |
| Buzzer             | GPIO 13   |
| OLED SDA           | GPIO 21   |
| OLED SCL           | GPIO 22   |
| Motor A (Right)    | GPIO 25, 26 |
| Motor B (Left)     | GPIO 27, 32 |
| Steering Servo     | GPIO 33   |
| Vertical Neck Servo| GPIO 14   |
| Horizontal Neck    | GPIO 15   |

---

## 📖 Menu Structure

```text
Main Menu
├── Run Line Follower
├── Configuration
│   ├── Speed
│   ├── Kp (Proportional)
│   ├── Ki (Integral)
│   ├── Kd (Derivative)
│   ├── Vertical Servo
│   └── Alpha (Smoothing)
└── About
```
---

## 🛠 Libraries Used

Install these libraries via the Arduino Library Manager:

- [`Pixy2`](https://github.com/charmedlabs/pixy2)
- [`Adafruit SSD1306`](https://github.com/adafruit/Adafruit_SSD1306)
- [`Adafruit GFX`](https://github.com/adafruit/Adafruit-GFX-Library)
- [`ESP32Servo`](https://github.com/jkb-git/ESP32Servo)

---

## 🎛 PixyMon Settings

<div align="center">
  <img src="images/PixyMon Parameters.png" alt="PixyMon Configuration Parameters" width="40%">
</div>

Before running the robot, make sure your **Pixy2 camera** is properly configured using **PixyMon**:

- Select **Line Tracking Mode** from the features tab.
- Adjust parameters like:
  - **Max Segments**
  - **Vector Threshold**
  - **Edge Threshold**
  - **White Line on Black Background** (or vice versa based on your arena)
- Save settings to Pixy2’s flash using `File → Save Parameters to Flash`.

These settings help ensure Pixy2 detects lines accurately and provides clean vector data for PID tracking.


---


## 🚦 How It Works

<div align="center">
  <img src="images/pixyData.gif" alt="Pixy2 Data Visualization">
</div>

- Press **OK** to start the line following mode.
- Pixy2 tracks line vectors and calculates the **center X deviation**.
- A **PID controller** calculates the steering angle.
- Motors drive forward while the steering adjusts dynamically.
- A **neck servo** mimics camera scanning based on error value.
- Press **CANCEL** to return to the main menu.

---

## 📟 OLED Display (During Run)

### Line Follower Mode
```
Speed: 60%
PID: 1.0 / 0.01 / 0.001
Servo: 25°
Alpha: 0.20
CANCEL: Return to Menu
```

The OLED provides real-time feedback during operation, showing current speed, PID values, servo angle, and smoothing factor. This allows on-the-fly adjustments and immediate diagnostics when something goes off-track.

---

## 👥 Team

- Thoriq Putra Belligan  
- Dimas Akmal Azzaki  
- Ahmad Sirozul M  
- M. Firas Khairiansyah  
- Ilham Muhammad Ismaya  

---

## 📌 Notes

- Set Pixy2 to **line-following mode** using PixyMon before starting.
- This robot uses **Ackermann steering**—steered by servo, not tank drive.
- PID and other configuration values can be **adjusted live** through the menu.
- The system will display **"No line detected!"** when the line is lost.

---


