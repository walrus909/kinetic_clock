// bug going from 0 to 1, segment 31 pops out instead of in
// bug when uploading time, it is off by 30 seconds

#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>
#include <string>
#include <iostream>

RTC_DS3231 rtc;

Servo servos[30];

// Servo pin assignments for each digit and dots (change these according to your wiring)
/*   Servo pin assignment map:

                        FRONT VIEW
    2             9                    24           31
7      3      14     10     22      29    25     36    32
    8             15                   30           37
6      4      13     11     23      28    26     35    33
    5             12                   27           34

*/  
const int servoPins[30] = {
                            2, 3, 4, 5, 6, 7, 8, 
                            9, 10, 11, 12, 13, 14, 15,
                            24, 25, 26, 27, 28, 29, 30, 
                            31, 32, 33, 34, 35, 36, 37, 
                            22, 23
                          };

const int ON_ANGLES[30] = {
                            82, 95, 90, 86, 77, 81, 85, 
                            86, 95, 86, 86, 86, 108, 100,
                            82, 95, 95, 72, 82, 95, 68, 
                            90, 82, 82, 90, 86,86, 95, 
                            103, 104
                          };
const int OFF_ANGLES[30] = {
                              140, 154, 154, 149, 149, 145, 149, 
                              144, 167, 140, 140, 144, 162, 158,
                              154, 154, 154, 144, 144, 162, 131, 
                              149, 154, 154, 159, 154, 150, 158, 
                              172, 167
                            };
// digitConfig is a map of each of the 7 segnments where 1=on, 0 = 0ff for each number to display
const int digitConfig[10][7] = {
    {1, 1, 1, 1, 1, 1, 0},  // 0
    {0, 1, 1, 0, 0, 0, 0},  // 1
    {1, 1, 0, 1, 1, 0, 1},  // 2
    {1, 1, 1, 1, 0, 0, 1},  // 3
    {0, 1, 1, 0, 0, 1, 1},  // 4
    {1, 0, 1, 1, 0, 1, 1},  // 5
    {1, 0, 1, 1, 1, 1, 1},  // 6
    {1, 1, 1, 0, 0, 0, 0},  // 7
    {1, 1, 1, 1, 1, 1, 1},  // 8
    {1, 1, 1, 1, 0, 1, 1}}; // 9

int lastServoPositions[30];
bool format12h = true;       //set to false if you want 24h format
bool isPM;

void setup() {
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));     //get the last compilation time and date
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

  if (format12h) {
    // convert from 24 to 12h format
    if (hours == 0) {
      hours = 12;         //midnight
      isPM = false;
    } else if (hours <12) {
      hours = hours;      //morning
      isPM = false; 
    } else if (hours == 12) {
      hours = 12;         //noon
      isPM = true;
    } else {
      hours = hours -12;  //afternoon
      isPM = true;
    }
  }


  if (hours >= 0 && hours < 23) {
    for (int i = 0; i < 30; i++) {
      servos[i].attach(servoPins[i]);
    }

    // don't display leading zero if 12h format
    if (format12h && hours <10 ) {
      clearDigit(0);
    } else {
      displayDigit(hours / 10, 0);
    }
 
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
  // for each segment in a digit
  for (int i = 0; i < 7; i++) {
     // determine the servo to move bsaed on the segment(i) and position (hh:mm)
    int servoIndex = position * 7 + i;   

     // determine whether to pop out or push in this segment based on digitConfig map
    int targetAngle = (digitConfig[digit][i] == 1) ? ON_ANGLES[servoIndex] : OFF_ANGLES[servoIndex];

      // if the segment isn't already in position, move it.
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
  //Serial.print("moving servo ");
  //Serial.print(Servo);
  servo.write(targetAngle);
}

void clearDigit(int position) {
  //flatten out the entire digit
  for (int i = 0; i < 7; i++) {
     // determine the servo to move bsaed on the segment(i) and position (hh:mm)
    int servoIndex = position * 7 + i;   

     // set to off position
    int targetAngle = OFF_ANGLES[servoIndex];

      // if the segment isn't already in position, move it.
    if (targetAngle != lastServoPositions[servoIndex]) {
      moveServoSmooth(servos[servoIndex], targetAngle);
      lastServoPositions[servoIndex] = targetAngle;
    }  
  }

  string subtractUploadTime(std::string compileTime) {
    //given a time string, subtract a set number of seconds it takes to upload and reset the RTC
    std::string newTime;
    int secondsGap = 30;
    
    // get hrs, mins, secs from time string (hh:mm:ss)
    String strSec = compileTime.substring(compileTime.length()-2);
    String strMin = compileTime.substring(compileTime.length()-5,2);
    String strHrs = compileTime.substring(0,2);
    //convert to int
    int intSec = std::stoi(strSec);
    int intMin = std::stio(strMin);

    if (intSec >= secondsGap) {
      intSec = intSec - secondsGap; //subtract gap
    } else {
      intMin = intMin - 1; //  subtract 1 from minute, 
      intSec = 60 - abs(secondsGap - intMin); // find abs between gap and minutes,subtract from 60
    }
    // new time 
    newTime = strHrs + ":" + std::to_string(intMin) + ":" + std::to_string(intSec);
    return newTime;
  } 
  
}

