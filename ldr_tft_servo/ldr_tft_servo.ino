/**************************************************************************

 Based on the following example code (BSD licence):
  https://github.com/adafruit/Adafruit_SSD1306/blob/5fa3388757f5a3db2dcf01f888e4b2fa61eebf7e/examples/ssd1306_128x32_spi/ssd1306_128x32_spi.ino

 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define OLED_TIME_DELAY_BETWEEN_FRAMES 20

#define LDR_PIN A0
#define LDR_READING_CHANGE_THRESHOLD 300
#define LDR_READINGS_COUNT 10

int ldr_readings[LDR_READINGS_COUNT];
int ldr_readings_counter = 0;

Servo myservo;
#define SERVO_ROTATE_TIME_INTERVAL 2000
#define SERVO_RESET_TIME_INTERVAL 1000
#define SERVO_ROTATE_POSITION 90
#define SERVO_RESET_POSITION 180
unsigned long timeOfLastServoRotation = 0;
boolean isServoRotated = false;

void setup() {
  Serial.begin(9600);

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  myservo.write(SERVO_RESET_POSITION);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  int sensorValue = analogRead(LDR_PIN);
  Serial.println(sensorValue);

  for (int i = 0; i < LDR_READINGS_COUNT; ++i) {
    ldr_readings[i] = sensorValue;
  }
}

void loop() {
  int sensorValue = analogRead(LDR_PIN);

  int total_of_readings = 0;
  for (int i = 0; i < LDR_READINGS_COUNT; ++i) {
    total_of_readings += ldr_readings[ldr_readings_counter];
  }

  int mean_of_readings = total_of_readings / LDR_READINGS_COUNT;

  ldr_readings[ldr_readings_counter] = sensorValue;
  if (ldr_readings_counter == LDR_READINGS_COUNT - 1) {
    ldr_readings_counter = 0;
  } else {
    ldr_readings_counter++;
  }

  drawSensorValue(sensorValue);

  if (sensorValue > (mean_of_readings + LDR_READING_CHANGE_THRESHOLD) && millis() - timeOfLastServoRotation > SERVO_ROTATE_TIME_INTERVAL) {
    myservo.write(SERVO_ROTATE_POSITION);
    isServoRotated = true;
    timeOfLastServoRotation = millis();
  }

  if (isServoRotated && millis() - timeOfLastServoRotation > SERVO_RESET_TIME_INTERVAL) {
      myservo.write(SERVO_RESET_POSITION);
      isServoRotated = false;
  }
}

void drawSensorValue(int sensorValue) {
  display.clearDisplay();

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  display.print(F("LAST: "));
  display.println((millis() - timeOfLastServoRotation) / 1000);

  int total_of_readings = 0;
  for (int i = 0; i < LDR_READINGS_COUNT; ++i) {
    total_of_readings += ldr_readings[ldr_readings_counter];
  }

  int mean_of_readings = total_of_readings / LDR_READINGS_COUNT;

  display.print(F("LDR : "));
  if (sensorValue > (mean_of_readings + LDR_READING_CHANGE_THRESHOLD)) {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.println(sensorValue);
    display.setTextColor(SSD1306_WHITE);
  } else {
    display.println(sensorValue);
  }

  display.print(F("MIN : "));
  display.println(mean_of_readings + LDR_READING_CHANGE_THRESHOLD);

  display.display();
  delay(OLED_TIME_DELAY_BETWEEN_FRAMES);
}
