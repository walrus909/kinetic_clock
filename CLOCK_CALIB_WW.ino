/*
This will read signal from servo tester and move a target servo and display the angle in serial port monitor
*/
/* The signal pin for ServoInput MUST be an interrupt-capable pin!
 *     Uno, Nano, Mini (328P): 2, 3
 *     Micro, Leonardo (32U4): 0, 1, 2, 3, 7
 *             Mega, Mega2560: 2, 3, 18, 19, 20, 21
 * Reference: https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 */


#include <ServoInput.h>
#include <Servo.h>

					// DO NOT CHANGE
ServoInputPin<18> servo;	// this is for the potentiometer board, do not change this from 18
Servo myservo;      // create servo object to control a servo

void setup() {
	Serial.begin(9600);
	servo.attach();  // attaches the servo input interrupt

					//   CHANGE
  myservo.attach(10);  // This is the servo to calibrate

	while (servo.available() == false) {
		Serial.println("Waiting for servo signal...");
		delay(500);
	}
}

void loop() {
	float angle = servo.getAngle();  // get angle of servo (0 - 180)
	Serial.println(angle);
  myservo.write(angle);

}