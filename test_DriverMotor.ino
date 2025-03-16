// Motor Driver Pin
#define MOTOR_PIN 21  // Connect this to your motor driver input

// PWM Channel Configuration
const int pwmChannel = 0;  // Use channel 0 for PWM
const int pwmFrequency = 5000;  // 5 kHz frequency
const int pwmResolution = 8;  // 8-bit resolution (0–255)

void setup() {
    Serial.begin(115200);

    // Configure LEDC channel
    ledcSetup(pwmChannel, pwmFrequency, pwmResolution);

    // Attach the motor pin to the LEDC channel
    ledcAttachPin(MOTOR_PIN, pwmChannel);

    Serial.println("Motor Control Initialized...");
}

void loop() {
    // Gradually increase motor speed
    for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
        ledcWrite(pwmChannel, dutyCycle);
        Serial.print("Motor Speed: ");
        Serial.println(dutyCycle);
        delay(20);  // Small delay for smooth speed increase
    }

    // Gradually decrease motor speed
    for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
        ledcWrite(pwmChannel, dutyCycle);
        Serial.print("Motor Speed: ");
        Serial.println(dutyCycle);
        delay(20);  // Small delay for smooth speed decrease
    }

    // Stop the motor for 2 seconds
    ledcWrite(pwmChannel, 0);
    Serial.println("Motor Stopped");
    delay(2000);
}
