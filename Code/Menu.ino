#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

// Menu states
enum MenuState { MAIN_MENU, SPEED_CONFIG, ABOUT_SCREEN };
MenuState menuState = MAIN_MENU;

// Menu items
const char* mainMenuItems[] = {"Line Follower Mode", "Speed Config", "About"};
const int mainMenuCount = 3;

int selectedItem = 0;
int currentSpeed = 50;  // Default speed (50%)

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 300;

void setup() {
  Serial.begin(9600);
  Wire.begin(21, 22);
  pinMode(BUZZER_PIN, OUTPUT);
  startupBeep();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Configure buttons with external pull-down resistors
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_OK, INPUT);
  pinMode(BUTTON_CANCEL, INPUT);
}

void loop() {
  handleButtons();
  drawMenu();
}

// Buzzer functions
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

// Button handling - modified for pull-down (now checking for HIGH)
void handleButtons() {
  if ((millis() - lastDebounceTime) < debounceDelay) return;

  if (menuState == ABOUT_SCREEN || menuState == SPEED_CONFIG) {
    if (digitalRead(BUTTON_CANCEL) == HIGH) {
      menuState = MAIN_MENU;
      selectedItem = 0;
      shortBeep();
      lastDebounceTime = millis();
      return;
    }
  }

  if (menuState == SPEED_CONFIG) {
    if (digitalRead(BUTTON_UP) == HIGH) {
      currentSpeed = min(currentSpeed + 5, 100);
      shortBeep();
      lastDebounceTime = millis();
    } 
    else if (digitalRead(BUTTON_DOWN) == HIGH) {
      currentSpeed = max(currentSpeed - 5, 0);
      shortBeep();
      lastDebounceTime = millis();
    }
    else if (digitalRead(BUTTON_OK) == HIGH) {
      menuState = MAIN_MENU;
      selectedItem = 0;
      shortBeep();
      lastDebounceTime = millis();
    }
    return;
  }

  if (menuState == ABOUT_SCREEN) {
    if (digitalRead(BUTTON_OK) == HIGH) {
      menuState = MAIN_MENU;
      selectedItem = 0;
      shortBeep();
      lastDebounceTime = millis();
    }
    return;
  }

  // Main menu navigation
  if (digitalRead(BUTTON_UP) == HIGH) {
    selectedItem--;
    if (selectedItem < 0) selectedItem = mainMenuCount - 1;
    shortBeep();
    lastDebounceTime = millis();
  } 
  else if (digitalRead(BUTTON_DOWN) == HIGH) {
    selectedItem++;
    if (selectedItem > mainMenuCount - 1) selectedItem = 0;
    shortBeep();
    lastDebounceTime = millis();
  } 
  else if (digitalRead(BUTTON_OK) == HIGH) {
    shortBeep();
    handleOK();
    lastDebounceTime = millis();
  }
}

// OK button handling
void handleOK() {
  switch (menuState) {
    case MAIN_MENU:
      switch (selectedItem) {
        case 0: // Line Follower Mode
          lineFollowerMode();
          break;
        case 1: // Speed Config
          menuState = SPEED_CONFIG;
          break;
        case 2: // About
          menuState = ABOUT_SCREEN;
          break;
      }
      break;
  }
}

// Drawing the menu
void drawMenu() {
  display.clearDisplay();

  if (menuState == MAIN_MENU) {
    drawCenteredMenu(mainMenuItems, mainMenuCount, "Main Menu");
  } 
  else if (menuState == ABOUT_SCREEN) {
    showAbout();
  }
  else if (menuState == SPEED_CONFIG) {
    drawSpeedConfig();
  }

  display.display();
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

  for (int i = 0; i < count; i++) {
    display.getTextBounds(items[i], 0, 0, &x, &y, &w, &h);
    int16_t xPos = (SCREEN_WIDTH - w) / 2;
    int16_t yPos = startY + (i * 10);

    if (i == selectedItem) {
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

void drawSpeedConfig() {
  // Draw title
  display.setTextSize(1);
  display.setCursor(0, 2);
  display.println("Speed Configuration");
  display.drawFastHLine(0, 12, SCREEN_WIDTH, SSD1306_WHITE);

  // Draw current speed
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Speed: ");
  display.print(currentSpeed);
  display.print("%");

  // Draw speed bar
  int barWidth = map(currentSpeed, 0, 100, 0, SCREEN_WIDTH - 4);
  display.drawRect(2, 40, SCREEN_WIDTH - 4, 10, SSD1306_WHITE);
  display.fillRect(2, 40, barWidth, 10, SSD1306_WHITE);

  // Draw instructions
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.println("UP/DOWN: Adjust  OK: Save");
}

// About screen
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

// Action functions
void lineFollowerMode() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Starting Line Follower...");
  display.print("Speed: ");
  display.print(currentSpeed);
  display.println("%");
  display.display();
  delay(1000);
}