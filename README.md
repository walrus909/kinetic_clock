Arduino MEGA
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

Install Library RTCLib in Arduino IDE

Determine which servoes go to which Digital IO pins
-update index numbers in code

Calibrate each servo to determine two numbers:
- Out position that is level with the rest and maintains gear engagement for retract
- In position that sits flush and does not jam servo (bottom out)

Update numbers in to code 
