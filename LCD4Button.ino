#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup (adjust I2C address if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Button pins
#define BUTTON1 33 // Okay
#define BUTTON2 32 // Back
#define BUTTON3 35 // Up
#define BUTTON4 34 // Down

enum MenuState { MAIN_MENU, LINE_MODE_MENU, SPEED_MENU, EMOTE_MENU, RUNNING };
MenuState menuState = MAIN_MENU;

const char* mainMenuItems[] = {" Start Car", " Line Mode", " Speed", " Emote"};
byte selectedOption = 0;
byte lineMode = 0;         // 0 = Black/White, 1 = White/Black
int carSpeed = 50;         // Percentage (0-100)
int16_t speed = 0;         // 12-bit PWM value (-4095 to 4095)
uint8_t steering_angle = 90; // 0-180 degrees (90 = center)

// Button state tracking
bool prevBtn1 = false;
bool prevBtn2 = false;
bool prevBtn3 = false;
bool prevBtn4 = false;

void setup() {
  Serial.begin(115200);
  
  // Initialize buttons
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Self-Driving Car");
  lcd.setCursor(0, 1);
  lcd.print("PMLD2 Team Hitam");
  delay(2000);

  printMainMenu();
}

void loop() {
  readButtons();
  
  if (menuState == RUNNING) {
    // Throttle LCD updates to every 100ms
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 100) {
      printRunningScreen();
      lastUpdate = millis();
    }
  }
}

void readButtons() {
  bool currentBtn1 = !digitalRead(BUTTON1);
  bool currentBtn2 = !digitalRead(BUTTON2);
  bool currentBtn3 = !digitalRead(BUTTON3);
  bool currentBtn4 = !digitalRead(BUTTON4);

  if (currentBtn1 && !prevBtn1) processButton(1);
  if (currentBtn2 && !prevBtn2) processButton(2);
  if (currentBtn3 && !prevBtn3) processButton(3);
  if (currentBtn4 && !prevBtn4) processButton(4);

  prevBtn1 = currentBtn1;
  prevBtn2 = currentBtn2;
  prevBtn3 = currentBtn3;
  prevBtn4 = currentBtn4;

  delay(10);
}

void processButton(int button) {
  switch (menuState) {
    case MAIN_MENU:
      handleMainMenu(button);
      break;
    case LINE_MODE_MENU:
      handleLineModeMenu(button);
      break;
    case SPEED_MENU:
      handleSpeedMenu(button);
      break;
    case EMOTE_MENU:
      handleEmoteMenu(button);
      break;
    case RUNNING:
      handleRunningState(button);
      break;
  }
}

// New Running State Handler
void handleRunningState(int button) {
  if (button == 2) {  // Square button to return
    menuState = MAIN_MENU;
    printMainMenu();
  }
}

void handleMainMenu(int button) {
  switch (button) {
    case 3:  // Up
      if (selectedOption > 0) {
        selectedOption--;
        printMainMenu();
      }
      break;
    case 4:  // Down
      if (selectedOption < 3) {
        selectedOption++;
        printMainMenu();
      }
      break;
    case 1:  // OK
      if (selectedOption == 0) {
        menuState = RUNNING;
        printRunningScreen();
      } else if (selectedOption == 1) {
        menuState = LINE_MODE_MENU;
        printLineModeMenu();
      } else if (selectedOption == 2) {
        menuState = SPEED_MENU;
        printSpeedMenu();
      } else {
        menuState = EMOTE_MENU;
        printEmoteMenu();
      }
      break;
  }
}

void handleLineModeMenu(int button) {
  switch (button) {
    case 3:  // Up
    case 4:  // Down
      lineMode = !lineMode;
      printLineModeMenu();
      break;
    case 1:  // OK
      // Save line mode setting
      menuState = MAIN_MENU;
      printMainMenu();
      break;
    case 2:  // Back
      menuState = MAIN_MENU;
      printMainMenu();
      break;
  }
}

// Modified speed menu handling
void handleSpeedMenu(int button) {
  switch (button) {
    case 3:  // Up
      carSpeed = min(carSpeed + 5, 100);
      // updatePWM();
      printSpeedMenu();
      break;
    case 4:  // Down
      carSpeed = max(carSpeed - 5, 0);
      // updatePWM();
      printSpeedMenu();
      break;
    case 1:  // OK
    case 2:  // Back
      menuState = MAIN_MENU;
      printMainMenu();
      break;
  }
}

void handleEmoteMenu(int button) {
  if (button == 1) {  // OK
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Emoting");
    lcd.setCursor(3, 1);
    lcd.print(":D :X :O :)");
    // Add your motor control logic here
    delay(10000);
    menuState = MAIN_MENU;
    printMainMenu();
  } else if (button == 2) {  // Back
    menuState = MAIN_MENU;
    printMainMenu();
  }
}

void printMainMenu() {
  lcd.clear();
  lcd.print(">");
  lcd.print(mainMenuItems[selectedOption]);
  
  lcd.setCursor(0, 1);
  if (selectedOption < 3) {
    lcd.print(" ");
    lcd.print(mainMenuItems[selectedOption + 1]);
  }
}

void printLineModeMenu() {
  lcd.clear();
  lcd.print("Line Mode:");
  lcd.setCursor(0, 1);
  lcd.print(lineMode ? "White/Black" : "Black/White");
}

void printSpeedMenu() {
  lcd.clear();
  lcd.print("Current Speed:");
  lcd.setCursor(0, 1);
  lcd.print(carSpeed);
  lcd.print("% ");
}

void printEmoteMenu() {
  lcd.clear();
  lcd.print("Start Emote?");
  lcd.setCursor(0, 1);
  lcd.print("OK-Confirm Back");
}

void printRunningScreen() {
  lcd.clear();
  lcd.print("Mode:");
  lcd.print(lineMode ? "W/B" : "B/W");
  lcd.print(" SPD:");
  lcd.print(carSpeed);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("ANG:");
  lcd.print(steering_angle);
  lcd.write(0xDF); // Degree symbol
  lcd.print(" PWM:");

  // Handle negative speed values
  if (speed < 0) {
    lcd.print("-");
    lcd.print(abs(speed));
  } else {
    lcd.print(speed);
  }
}