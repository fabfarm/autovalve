# autovalve
This repo contains Arduino-based projects for solenoid valve irrigation automation system.
***
* ### Automated timed relay controller using Arduino UNO R3 and DS1302 RTC module
  Arduino UNO R3 with RTC module controls relay. Timer is set to activate relay every hour for 40 s.

* ### ESP-01 WiFi relay controller via local network and with MIT Android app
  ESP-01 controls relay using self-hosted web page via IP (can be configured as dynamic or static) on local network and simultaneously with MIT Android app.
  
![alt text](https://github.com/fabfarm/autovalve/blob/master/ESP01-pinout.png "ESP-01 pinout")

* ### ESP-01 WiFi LEDs controller using 4 GPIO pins
  ESP-01 controls 4 LEDs independently via self-hosted web page using GPIO 0, 1 (TX), 2 and 3 (RX).
Note: GPIO 2 should not be connected to LED at boot up. The pin can be set as INPUT.

