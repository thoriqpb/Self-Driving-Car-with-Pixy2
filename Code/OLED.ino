#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Button pins
#define BUTTON1 4 // Okay
#define BUTTON2 5 // Back
#define BUTTON3 8 // Up
#define BUTTON4 27 // Down

enum MenuState { MAIN_MENU, LINE_MODE_MENU, SPEED_MENU, EMOTE_MENU, RUNNING };
MenuState menuState = MAIN_MENU;

const char* mainMenuItems[] = {" Start Car", " Line Mode", " Speed", " Emote"};
byte selectedOption = 0;
byte lineMode = 0;
int carSpeed = 50;
int16_t speed = 0;
uint8_t steering_angle = 90;

bool prevBtn1 = false;
bool prevBtn2 = false;
bool prevBtn3 = false;
bool prevBtn4 = false;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(10, 10);
  display.println("Self-Driving Car");
  display.setCursor(15, 30);
  display.println("PMLD2 Team Hitam");
  display.display();
  delay(2000);

  printMainMenu();
}

void loop() {
  readButtons();

  if (menuState == RUNNING) {
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
    case MAIN_MENU: handleMainMenu(button); break;
    case LINE_MODE_MENU: handleLineModeMenu(button); break;
    case SPEED_MENU: handleSpeedMenu(button); break;
    case EMOTE_MENU: handleEmoteMenu(button); break;
    case RUNNING: handleRunningState(button); break;
  }
}

void handleRunningState(int button) {
  if (button == 2) {
    menuState = MAIN_MENU;
    printMainMenu();
  }
}

void handleMainMenu(int button) {
  switch (button) {
    case 3: if (selectedOption > 0) selectedOption--; printMainMenu(); break;
    case 4: if (selectedOption < 3) selectedOption++; printMainMenu(); break;
    case 1:
      if (selectedOption == 0) menuState = RUNNING, printRunningScreen();
      else if (selectedOption == 1) menuState = LINE_MODE_MENU, printLineModeMenu();
      else if (selectedOption == 2) menuState = SPEED_MENU, printSpeedMenu();
      else menuState = EMOTE_MENU, printEmoteMenu();
      break;
  }
}

void handleLineModeMenu(int button) {
  switch (button) {
    case 3:
    case 4:
      lineMode = !lineMode;
      printLineModeMenu();
      break;
    case 1:
    case 2:
      menuState = MAIN_MENU;
      printMainMenu();
      break;
  }
}

void handleSpeedMenu(int button) {
  switch (button) {
    case 3: carSpeed = min(carSpeed + 5, 100); printSpeedMenu(); break;
    case 4: carSpeed = max(carSpeed - 5, 0); printSpeedMenu(); break;
    case 1:
    case 2: menuState = MAIN_MENU; printMainMenu(); break;
  }
}

void handleEmoteMenu(int button) {
  if (button == 1) {
    display.clearDisplay();
    display.setCursor(10, 10);
    display.println("Emoting");
    display.setCursor(10, 30);
    display.println(":D :X :O :)");
    display.display();
    delay(3000);
    menuState = MAIN_MENU;
    printMainMenu();
  } else if (button == 2) {
    menuState = MAIN_MENU;
    printMainMenu();
  }
}

// ------------------- OLED DISPLAY FUNCTIONS -------------------

void printMainMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(">");
  display.println(mainMenuItems[selectedOption]);

  if (selectedOption < 3) {
    display.setCursor(10, 20);
    display.print(" ");
    display.println(mainMenuItems[selectedOption + 1]);
  }
  display.display();
}

void printLineModeMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Line Mode:");
  display.setCursor(0, 20);
  display.println(lineMode ? "White/Black" : "Black/White");
  display.display();
}

void printSpeedMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Current Speed:");
  display.setCursor(0, 20);
  display.print(carSpeed);
  display.println("%");
  display.display();
}

void printEmoteMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Start Emote?");
  display.setCursor(0, 20);
  display.println("OK=Confirm  Back");
  display.display();
}

void printRunningScreen() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Mode: ");
  display.print(lineMode ? "W/B" : "B/W");
  display.print(" SPD: ");
  display.print(carSpeed);
  display.println("%");

  display.setCursor(0, 20);
  display.print("ANG: ");
  display.print(steering_angle);
  display.write(247);  // Degree symbol on OLED
  display.print(" PWM: ");
  display.println(speed);
  display.display();
}
