ELEGOO (Arduino MEGA2560)
4 kB EEPROM storage

Keystudio MEGA Sensor Shield v1.3
https://wiki.keyestudio.com/Ks0006_MEGA_Sensor_Shield_V1

DV Adaptor requirements:
1. 5V DC for arduino
2. 30 servos : SG-90 servos rated at 250mA peak
    So 30 x .250A = 7.5A  (that is a lot of power, but they do not all technically run instantaneously at once)
3. Tested with variable power supply to work at 5V, 3.0A ok
4. 5V 3A adaptor might work
    - If insiffucient, change code timing to wait for each servo to move one at a time
    - https://www.reddit.com/r/arduino/comments/18wtgmm/help_powering_30_servos_and_arduino_from_a_single/

RTC Module connections to Keyestudio Digital Sensor Shield:
- GND any GND
- Vcc any Vcc (5V)
- SDA on Digital 20
- SCL on Digital 21

Servo test board connections to Keystudio Digital Sensor Shield:
- Signal on Digital 18 (must be an interrupt pin)
- GND any GND
- Vcc and Vcc (5v)
  
Install Libraries in Arduino IDE
- RTCLib
- ServoInput

Determine which servoes go to which Digital IO pins
- make a visual map
- update index numbers in code

Calibrate each servo to determine two numbers associated with each servo:
- ON_ANGLES position that is level with the rest and maintains gear engagement for retract
- OFF_ANGLES position that sits flush and does not jam servo (bottom out)
  
- connect servo tester (with potentiometer) to pin 16 with Vcc and GND
- Open 2 programs: CLOCK_CALIB and also segment_Clock_RTC02) in Arduino IDE
- Connect USB cable to PC and then turn serial monitor on in IDE Tools menu
- remove all segments
- Start at first servo in array (e.g. 2)
- in CLOCK_CALIB, change variable ServoInputPin to the correspnding servo index to calibrate (e.g. 2)
- turn potentiometer to extreme counterclockwise position to rotate gear to furthest out point
- insert segment into socket while turning potentiometer clockwise to catch the gears and pull segment in to face
- continue turning potentiometer until segment sits flush with face.
- Read the number in the serial output stream and enter that in code segment_clock_RTC02 in the respective servo position (e.g. 2) of OFF_ANGLES array
- turn the potentiometer counterclockwise until the segment is pushed out to the desired distance from face.  Try to get all segments to same level
- read the number in the serial output stream and enter that in code segment_clock_RTC02 in the respective servo position (e.g. 2) of ON_ANGLES array
- go to next servo in arary (e.g. 3) and repeat

