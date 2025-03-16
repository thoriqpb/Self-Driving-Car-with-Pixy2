# Self-Driving Car with ESP32 and Pixy2

This project implements a self-driving car using an ESP32 microcontroller, a Pixy2 camera for line tracking, and a PID controller for steering and motor control. The car follows a line using computer vision and adjusts its speed and direction dynamically.

---

## Table of Contents
1. [Components](#components)
2. [Wiring Diagram](#wiring-diagram)
3. [Setup Instructions](#setup-instructions)
4. [Code Structure](#code-structure)
5. [How It Works](#how-it-works)
6. [License](#license)

---

## Components
- **ESP32 Microcontroller**: Main controller for the car.
- **Pixy2 Camera**: Detects and tracks the line using computer vision.
- **Steering Servo**: Controls the steering angle of the car.
- **Motor Driver**: Drives the motors for forward and backward movement.
- **DC Motors**: Two motors for driving the car.
- **External Power Supply**: Provides power to the motors and servo.
- **Jumper Wires**: For connecting components.
- **Chassis**: A car chassis to mount all components.

---

## Wiring Diagram
### Pixy2 (SPI)
- **MISO** -> ESP32 GPIO 19
- **MOSI** -> ESP32 GPIO 23
- **SCK**  -> ESP32 GPIO 18
- **GND**  -> ESP32 GND
- **VCC**  -> ESP32 5V

### Fixed Neck Servo Angle
- **Vertical Angle**: 40°
- **Horizontal Angle**: 90°

### Steering Servo
- **Signal** -> ESP32 GPIO 4
- **VCC**    -> External 5V Power Supply
- **GND**    -> ESP32 GND (shared with power supply)

### Motor Driver
- **IN2** -> ESP32 GPIO 21 (Motor A)
- **IN4** -> ESP32 GPIO 22 (Motor B)
- **GND** -> ESP32 GND
- **VCC** -> External Power Supply (match motor voltage)

---

## Setup Instructions
1. **Hardware Setup**:
   - Assemble the car chassis and mount the ESP32, Pixy2, motor driver, and servo.
   - Connect the components as per the wiring diagram above.
   - Ensure all GND connections are shared between the ESP32, Pixy2, and power supplies.

2. **Software Setup**:
   - Install the [Arduino IDE](https://www.arduino.cc/en/software).
   - Add the ESP32 board to the Arduino IDE:
     1. Go to `File > Preferences`.
     2. Add the following URL to the "Additional Boards Manager URLs":
        ```
        https://dl.espressif.com/dl/package_esp32_index.json
        ```
     3. Go to `Tools > Board > Boards Manager`, search for `ESP32`, and install the package.
   - Install the required libraries:
     - `ESP32Servo` (for servo control).
     - `Pixy2` (for Pixy2 camera communication).

3. **Upload the Code**:
   - Clone this repository or download the code.
   - Open the `main.ino` file in the Arduino IDE.
   - Select the correct board (`ESP32 Dev Module`) and port.
   - Upload the code to the ESP32.

4. **Run the Car**:
   - Place the car on a line track.
   - Power on the car and observe it following the line.

---

## Code Structure
The code is organized into modular files for better readability and maintainability:
- **`main.ino`**: Main program logic.
- **`config.h`**: Hardware configuration (pins, constants).
- **`motor_control.h`**: Functions for controlling the motors.
- **`servo_control.h`**: Functions for controlling the steering servo.
- **`pid_controller.h`**: PID control logic for steering.
- **`pixy2_utils.h`**: Functions for processing Pixy2 camera data.
- **`diagnostics.h`**: Debugging and diagnostics utilities.

---

## How It Works
1. **Line Detection**:
   - The Pixy2 camera detects the line and provides the coordinates of the line vectors.
   - The midpoint of the line is calculated to determine the car's deviation from the center.

2. **PID Control**:
   - A PID controller calculates the steering angle based on the deviation from the centerline.
   - The steering servo adjusts the car's direction to follow the line.

3. **Motor Control**:
   - The motors are controlled based on the deviation to maintain a consistent speed.
   - If no line is detected, the motors stop.

4. **Diagnostics**:
   - Debugging information (e.g., line coordinates, deviation, steering angle) is printed to the Serial Monitor.

---

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Acknowledgments
- [Pixy2 Camera](https://pixycam.com/pixy2/) for line tracking.
- [ESP32](https://www.espressif.com/en/products/socs/esp32) for microcontroller support.

---

## Contributing
Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.