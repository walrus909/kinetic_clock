Need 5V 6A power supply

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
