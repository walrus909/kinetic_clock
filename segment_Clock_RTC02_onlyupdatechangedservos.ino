#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>

RTC_DS3231 rtc;

Servo servos[30];

// Servo pin assignments for each digit and dots (change these according to your wiring)
const int servoPins[30] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 14, 15,
                           24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 22, 23};

const int ON_ANGLES[30] = {90, 100, 85, 100, 97, 100, 102, 100, 100, 100, 105, 95, 110, 100,
                           105, 95, 90, 85, 105, 100, 100, 100, 100, 90, 100, 100, 80, 90, 80, 100};
const int OFF_ANGLES[30] = {150, 160, 145, 160, 157, 160, 162, 160, 160, 160, 160, 160, 170, 160,
                            165, 160, 155, 150, 165, 165, 160, 160, 160, 150, 160, 160, 140, 150, 150, 160};
const int digitConfig[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, {0, 1, 1, 0, 0, 0, 0}, {1, 1, 0, 1, 1, 0, 1}, {1, 1, 1, 1, 0, 0, 1},
    {0, 1, 1, 0, 0, 1, 1}, {1, 0, 1, 1, 0, 1, 1}, {1, 0, 1, 1, 1, 1, 1}, {1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 0, 1, 1}};

int lastServoPositions[30];

void setup() {
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Display initial time and DST status
  printTimeAndDSTInfo();
}

void loop() {
  DateTime now = rtc.now();
  now = adjustForDST(now); // Adjust for DST

  int hours = now.hour();
  int minutes = now.minute();
  int seconds = now.second();

  if (hours >= 7 && hours < 23) {
    for (int i = 0; i < 30; i++) {
      servos[i].attach(servoPins[i]);
    }

    displayDigit(hours / 10, 0);
    displayDigit(hours % 10, 1);
    displayDigit(minutes / 10, 2);
    displayDigit(minutes % 10, 3);

    for (int i = 0; i < 30; i++) {
      servos[i].detach();
    }
  }

  if (seconds == 0) {
    // Print the time and DST info every minute
    printTimeAndDSTInfo();
  }

  delay(1000);
}

DateTime adjustForDST(DateTime now) {
  int month = now.month();
  int day = now.day();
  int hour = now.hour();
  int weekday = now.dayOfTheWeek();

  // Check for DST start (Second Sunday in March)
  if (month == 3 && (day - weekday >= 7) && (weekday != 0 || day >= 14)) {
    if (hour >= 2) {
      now = now + TimeSpan(3600); // Add 1 hour
    }
  }
  // Check for DST end (First Sunday in November)
  else if (month == 11 && (day - weekday < 7) && (weekday != 0 || day <= 7)) {
    if (hour >= 2) {
      now = now - TimeSpan(3600); // Subtract 1 hour
    }
  }

  return now;
}

void printTimeAndDSTInfo() {
  DateTime now = rtc.now();
  int month = now.month();
  int day = now.day();
  int weekday = now.dayOfTheWeek();

  bool isDST = false;
  String nextDSTChange;

  // Check DST status
  if ((month > 3 && month < 11) || // Between April and October
      (month == 3 && (day - weekday >= 7) && (weekday != 0 || day >= 14)) || // After second Sunday in March
      (month == 11 && (day - weekday < 7) && (weekday == 0 && day < 7))) {  // Before first Sunday in November
    isDST = true;
    nextDSTChange = "DST ends on the first Sunday of November.";
  } else {
    nextDSTChange = "DST begins on the second Sunday of March.";
  }

  // Print the current time
  Serial.print("Current time: ");
  Serial.print(now.year());
  Serial.print("-");
  Serial.print(now.month());
  Serial.print("-");
  Serial.print(now.day());
  Serial.print(" ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  // Print DST status
  if (isDST) {
    Serial.println("Daylight Saving Time (DST) is currently active.");
  } else {
    Serial.println("Standard Time (DST is not active).");
  }

  // Print next DST change
  Serial.println(nextDSTChange);
}

void displayDigit(int digit, int position) {
  for (int i = 0; i < 7; i++) {
    int servoIndex = position * 7 + i;
    int targetAngle = (digitConfig[digit][i] == 1) ? ON_ANGLES[servoIndex] : OFF_ANGLES[servoIndex];

    if (targetAngle != lastServoPositions[servoIndex]) {
      moveServoSmooth(servos[servoIndex], targetAngle);
      lastServoPositions[servoIndex] = targetAngle;
    }
  }
}

void moveServoSmooth(Servo &servo, int targetAngle) {
  int currentAngle = servo.read();
  int step = (currentAngle < targetAngle) ? 1 : -1;

  for (int angle = currentAngle; angle != targetAngle; angle += step) {
    servo.write(angle);
    delay(7);
  }

  servo.write(targetAngle);
}
