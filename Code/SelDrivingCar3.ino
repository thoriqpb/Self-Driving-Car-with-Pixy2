#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
#include <Pixy2.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Button Pins
#define BUTTON_UP 4
#define BUTTON_DOWN 2
#define BUTTON_OK 5
#define BUTTON_CANCEL 12

// Buzzer Pin
#define BUZZER_PIN 13

// Menu States
enum MenuState { 
  MAIN_MENU, 
  CONFIG_MENU,
  SPEED_CONFIG, 
  KP_CONFIG,
  KI_CONFIG,
  KD_CONFIG,
  VERTICAL_SERVO_CONFIG,
  ALPHA_CONFIG,
  ABOUT_SCREEN, 
  RUNNING_MODE 
};

MenuState menuState = MAIN_MENU;

// Menu Items
const char* mainMenuItems[] = {"Run Line Follower", "Configuration", "About"};
const int mainMenuCount = 3;

const char* configMenuItems[] = {"Speed", "Kp", "Ki", "Kd", "Vert Servo", "Alpha"};
const int configMenuCount = 6;

int selectedItem = 0;
int configSelectedItem = 0;

// Configurable Parameters
struct Config {
  int speed = 20;          // 0-100%
  float kp = 1.0;         // PID Proportional
  float ki = 0.7;         // PID Integral
  float kd = 0.1;         // PID Derivative
  int verticalServo = 30; // Default vertical servo position
  float alpha = 0.2;      // Smoothing factor for neck servo
} config;

// Motor and Servo Pins
#define MOTOR_PIN1 25
#define MOTOR_PIN2 26
#define MOTOR_PIN3 27
#define MOTOR_PIN4 32
const int SERVO_PIN = 33;
const int SERVO_MIN_US = 500;
const int SERVO_MAX_US = 2500;

// Pixy2 Line Tracking
const int TARGET_X = 39;
const int MAX_TURN_ANGLE = 45;

// Global Variables
Pixy2 pixy;
Servo steeringServo;
Servo horizontalNeckServo;  // Servo15
Servo verticalNeckServo;    // Servo14

float integral = 0;
float prev_error = 0;
unsigned long last_time = 0;
float smooth_neck_angle = 90;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 300;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize OLED
  Wire.begin(21, 22);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initialize buttons
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_OK, INPUT);
  pinMode(BUTTON_CANCEL, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  startupBeep();

  // Initialize Pixy and servos
  pixy.init();
  steeringServo.attach(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
  verticalNeckServo.attach(14, 500, 2500);
  horizontalNeckServo.attach(15, 500, 2500);
  horizontalNeckServo.write(90);

  // Initialize motors
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(MOTOR_PIN3, OUTPUT);
  pinMode(MOTOR_PIN4, OUTPUT);

  stopMotors();
  Serial.println("System ready!");
}

void loop() {
  if (menuState == RUNNING_MODE) {
    runLineFollower();
    if (digitalRead(BUTTON_CANCEL)) {
      menuState = MAIN_MENU;
      stopMotors();
      shortBeep();
      delay(300);
    }
  } else {
    handleButtons();
    drawMenu();
  }
}

// Menu System Functions
void shortBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(10);
  digitalWrite(BUZZER_PIN, LOW);
}

void startupBeep() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

void handleButtons() {
  unsigned long currentMillis = millis();
  
  if ((currentMillis - lastDebounceTime) < debounceDelay) {
    return;
  }

  bool upPressed = digitalRead(BUTTON_UP);
  bool downPressed = digitalRead(BUTTON_DOWN);
  bool okPressed = digitalRead(BUTTON_OK);
  bool cancelPressed = digitalRead(BUTTON_CANCEL);

  if (!upPressed && !downPressed && !okPressed && !cancelPressed) {
    return;
  }

  switch(menuState) {
    case ABOUT_SCREEN:
      if (okPressed || cancelPressed) {
        menuState = MAIN_MENU;
        selectedItem = 0;
        shortBeep();
        lastDebounceTime = currentMillis;
      }
      return;
      
    case CONFIG_MENU:
      if (cancelPressed) {
        menuState = MAIN_MENU;
        configSelectedItem = 0;
        shortBeep();
        lastDebounceTime = currentMillis;
        return;
      }
      break;
      
    case SPEED_CONFIG:
    case KP_CONFIG:
    case KI_CONFIG:
    case KD_CONFIG:
    case VERTICAL_SERVO_CONFIG:
    case ALPHA_CONFIG:
      if (upPressed) {
        adjustParameter(1);
        shortBeep();
        lastDebounceTime = currentMillis;
        return;
      } 
      else if (downPressed) {
        adjustParameter(-1);
        shortBeep();
        lastDebounceTime = currentMillis;
        return;
      }
      else if (okPressed || cancelPressed) {
        menuState = CONFIG_MENU;
        shortBeep();
        lastDebounceTime = currentMillis;
        return;
      }
      break;
  }

  if (upPressed) {
    if (menuState == MAIN_MENU) {
      selectedItem = (selectedItem - 1 + mainMenuCount) % mainMenuCount;
    } 
    else if (menuState == CONFIG_MENU) {
      configSelectedItem = (configSelectedItem - 1 + configMenuCount) % configMenuCount;
    }
    shortBeep();
    lastDebounceTime = currentMillis;
  } 
  else if (downPressed) {
    if (menuState == MAIN_MENU) {
      selectedItem = (selectedItem + 1) % mainMenuCount;
    } 
    else if (menuState == CONFIG_MENU) {
      configSelectedItem = (configSelectedItem + 1) % configMenuCount;
    }
    shortBeep();
    lastDebounceTime = currentMillis;
  } 
  else if (okPressed) {
    shortBeep();
    handleOK();
    lastDebounceTime = currentMillis;
  }
}

void adjustParameter(int direction) {
  switch(menuState) {
    case SPEED_CONFIG:
      config.speed = constrain(config.speed + direction * 5, 0, 100);
      break;
    case KP_CONFIG:
      config.kp = constrain(config.kp + (direction * 0.1), 0.1, 5.0);
      break;
    case KI_CONFIG:
      config.ki = constrain(config.ki + (direction * 0.1), 0.1, 5.0);
      break;
    case KD_CONFIG:
      config.kd = constrain(config.kd + (direction * 0.1), 0.0, 2.0);
      break;
    case VERTICAL_SERVO_CONFIG:
      config.verticalServo = constrain(config.verticalServo + direction, 0, 180);
      verticalNeckServo.write(config.verticalServo);
      break;
    case ALPHA_CONFIG:
      config.alpha = constrain(config.alpha + (direction * 0.01), 0.01, 0.5);
      break;
  }
}

void handleOK() {
  switch (menuState) {
    case MAIN_MENU:
      switch (selectedItem) {
        case 0: // Run Line Follower
          menuState = RUNNING_MODE;
          displayStatus("Starting...");
          delay(500);
          break;
        case 1: // Configuration
          menuState = CONFIG_MENU;
          configSelectedItem = 0;
          break;
        case 2: // About
          menuState = ABOUT_SCREEN;
          break;
      }
      break;
      
    case CONFIG_MENU:
      switch (configSelectedItem) {
        case 0: menuState = SPEED_CONFIG; break;
        case 1: menuState = KP_CONFIG; break;
        case 2: menuState = KI_CONFIG; break;
        case 3: menuState = KD_CONFIG; break;
        case 4: menuState = VERTICAL_SERVO_CONFIG; break;
        case 5: menuState = ALPHA_CONFIG; break;
      }
      break;
  }
}

void drawMenu() {
  display.clearDisplay();

  switch(menuState) {
    case MAIN_MENU:
      drawCenteredMenu(mainMenuItems, mainMenuCount, "Main Menu");
      break;
      
    case CONFIG_MENU:
      drawScrollableConfigMenu();
      break;
      
    case SPEED_CONFIG:
      drawParameterConfig("Speed", config.speed, "%");
      break;
      
    case KP_CONFIG:
      drawParameterConfig("Kp (Proportional)", config.kp, "");
      break;
      
    case KI_CONFIG:
      drawParameterConfig("Ki (Integral)", config.ki, "");
      break;
      
    case KD_CONFIG:
      drawParameterConfig("Kd (Derivative)", config.kd, "");
      break;
      
    case VERTICAL_SERVO_CONFIG:
      drawParameterConfig("Vert Servo Pos", config.verticalServo, "°");
      break;
      
    case ALPHA_CONFIG:
      drawParameterConfig("Alpha (Smoothing)", config.alpha, "");
      break;
      
    case ABOUT_SCREEN:
      showAbout();
      break;
  }

  display.display();
}

void drawScrollableConfigMenu() {
  const int itemsPerScreen = 4;
  int totalItems = configMenuCount;
  
  int firstItem = max(0, min(configSelectedItem - itemsPerScreen/2, totalItems - itemsPerScreen));
  firstItem = max(0, min(firstItem, totalItems - itemsPerScreen));

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Configuration");
  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);

  for (int i = 0; i < min(itemsPerScreen, totalItems); i++) {
    int itemIndex = firstItem + i;
    if (itemIndex >= totalItems) break;

    int16_t x, y;
    uint16_t w, h;
    display.getTextBounds(configMenuItems[itemIndex], 0, 0, &x, &y, &w, &h);
    
    int yPos = 15 + (i * 13);
    
    if (itemIndex == configSelectedItem) {
      display.fillRoundRect(2, yPos - 2, SCREEN_WIDTH - 4, 12, 2, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(5, yPos);
    display.print(configMenuItems[itemIndex]);

    display.setCursor(SCREEN_WIDTH - 30, yPos);
    switch(itemIndex) {
      case 0: display.print(config.speed); display.print("%"); break;
      case 1: display.print(config.kp, 1); break;
      case 2: display.print(config.ki, 1); break;
      case 3: display.print(config.kd, 1); break;
      case 4: display.print(config.verticalServo); display.print("°"); break;
      case 5: display.print(config.alpha, 2); break;
    }
  }
  display.setTextColor(SSD1306_WHITE);

  if (firstItem > 0) {
    display.drawTriangle(SCREEN_WIDTH/2 - 4, 10, SCREEN_WIDTH/2 + 4, 10, SCREEN_WIDTH/2, 6, SSD1306_WHITE);
  }
  if (firstItem + itemsPerScreen < totalItems) {
    display.drawTriangle(SCREEN_WIDTH/2 - 4, SCREEN_HEIGHT - 6, 
                        SCREEN_WIDTH/2 + 4, SCREEN_HEIGHT - 6, 
                        SCREEN_WIDTH/2, SCREEN_HEIGHT - 2, SSD1306_WHITE);
  }
}

void drawCenteredMenu(const char* items[], int count, const char* title) {
  int16_t x, y;
  uint16_t w, h;

  display.getTextBounds(title, 0, 0, &x, &y, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 2);
  display.print(title);
  display.drawFastHLine(0, 12, SCREEN_WIDTH, SSD1306_WHITE);

  int startY = (SCREEN_HEIGHT - (count * 10)) / 2;
  if (startY < 16) startY = 16;

  int currentSelection = (menuState == CONFIG_MENU) ? configSelectedItem : selectedItem;
  
  for (int i = 0; i < count; i++) {
    display.getTextBounds(items[i], 0, 0, &x, &y, &w, &h);
    int16_t xPos = (SCREEN_WIDTH - w) / 2;
    int16_t yPos = startY + (i * 10);

    if (i == currentSelection) {
      display.fillRoundRect(xPos - 4, yPos - 2, w + 8, h + 4, 2, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(xPos, yPos);
    display.print(items[i]);
  }
  display.setTextColor(SSD1306_WHITE);
}

void drawParameterConfig(const char* name, float value, const char* unit) {
  display.setTextSize(1);
  display.setCursor(0, 2);
  display.println(name);
  display.drawFastHLine(0, 12, SCREEN_WIDTH, SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(value);
  display.print(unit);

  if (strcmp(unit, "%") == 0 || strcmp(unit, "°") == 0) {
    int maxValue = (strcmp(unit, "%") == 0) ? 100 : 180;
    int barWidth = map(value, 0, maxValue, 0, SCREEN_WIDTH - 4);
    display.drawRect(2, 40, SCREEN_WIDTH - 4, 10, SSD1306_WHITE);
    display.fillRect(2, 40, barWidth, 10, SSD1306_WHITE);
  }

  display.setTextSize(1);
  display.setCursor(0, 55);
  display.println("UP/DOWN: Adjust  OK: Save");
}

void showAbout() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 2);
  display.println("     Team Member  ");
  display.println("");
  display.println("Thoriq Putra Belligan");
  display.println("Dimas Akmal Azzaki");
  display.println("Ahmad Sirozul M");
  display.println("M. Firas Khairiansyah");
  display.println("Ilham Muhammad Ismaya");
}

void displayStatus(const char* message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Line Follower Mode");
  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setCursor(0, 15);
  display.print("Speed: ");
  display.print(config.speed);
  display.println("%");
  
  display.setCursor(0, 25);
  display.print("PID: ");
  display.print(config.kp, 1);
  display.print("/");
  display.print(config.ki, 1);
  display.print("/");
  display.print(config.kd, 1);
  
  display.setCursor(0, 35);
  display.print("Servo: ");
  display.print(config.verticalServo);
  display.println("°");
  
  display.setCursor(0, 45);
  display.print("Alpha: ");
  display.print(config.alpha, 2);
  
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.println("CANCEL: Return to Menu");
  
  display.display();
}

// Line Follower Functions
void runLineFollower() {
  verticalNeckServo.write(config.verticalServo);
  pixy.line.getMainFeatures();

  if (pixy.line.numVectors == 0) {
    analogWrite(MOTOR_PIN1, 100);
    digitalWrite(MOTOR_PIN2, LOW);
    analogWrite(MOTOR_PIN3, 100);
    digitalWrite(MOTOR_PIN4, LOW);
    displayStatus("No line detected!");
    verticalNeckServo.write(80);
  } else {
    processLineData();
  }
}

void processLineData() {
  int x0 = pixy.line.vectors[0].m_x0;
  int y0 = pixy.line.vectors[0].m_y0;
  int x1 = pixy.line.vectors[0].m_x1;
  int y1 = pixy.line.vectors[0].m_y1;
  int x_mid;

  if (y1 == y0) {
    x_mid = (x0 + x1) / 2;
  } else {
    int y_mid = (y0 + y1) / 2;
    x_mid = x0 + ((x1 - x0) * (y_mid - y0)) / (y1 - y0);
  }

  int deviation = x_mid - TARGET_X;
  int steering_angle = calculateSteeringAngle(deviation);

  steeringServo.write(steering_angle);
  controlMotors(abs(deviation));

  int neck_min = 50;
  int neck_max = 145;
  int max_dev = 40;
  int limited_dev = constrain(abs(deviation), 0, max_dev);
  int neck_angle = map(limited_dev, 0, max_dev, 90, (deviation > 0) ? neck_min : neck_max);

  smooth_neck_angle += config.alpha * (neck_angle - smooth_neck_angle);
  horizontalNeckServo.write((int)smooth_neck_angle);

  char status[30];
  snprintf(status, sizeof(status), "Dev: %d Str: %d", deviation, steering_angle);
  displayStatus(status);
}

int calculateSteeringAngle(int error) {
  unsigned long now = millis();
  float dt = (now - last_time) / 1000.0;

  float P = config.kp * error;
  if (abs(error) < 10) integral += error * dt;
  if (abs(error) < 3) integral = 0;
  integral = constrain(integral, -5, 5);
  float I = config.ki * integral;

  float derivative = (error - prev_error) / dt;
  float D = config.kd * derivative;

  int output = 90 - (P + I + D);
  output = constrain(output, 0, 180);

  prev_error = error;
  last_time = now;

  return output;
}

void controlMotors(int deviation) {
  int baseSpeed = map(config.speed, 0, 100, 80, 255);
  int speed = map(deviation, 0, 40, baseSpeed, baseSpeed * 0.8);
  speed = constrain(speed, 80, 255);

  analogWrite(MOTOR_PIN1, speed);
  digitalWrite(MOTOR_PIN2, LOW);
  analogWrite(MOTOR_PIN3, speed);
  digitalWrite(MOTOR_PIN4, LOW);
}

void stopMotors() {
  delay(100);
  analogWrite(MOTOR_PIN1, 0);
  digitalWrite(MOTOR_PIN2, LOW);
  analogWrite(MOTOR_PIN3, 0);
  digitalWrite(MOTOR_PIN4, LOW);
}