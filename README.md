# autovalve
This repo contains Arduino-based projects for solenoid valve irrigation automation system.
***
* ### Automated timed relay controller using Arduino UNO R3 and DS1302 RTC module
  Arduino UNO R3 with RTC module controls relay. Timer is set to activate relay every hour for 40 s.

* ### ESP-01 WiFi relay controller via local network and with MIT Android app
  Control relay using ESP-01 via self-hosted web page using IP address (can be configured as dynamic or static) in browser on local network and simultaneously via Android app built using MIT App Inventor (http://ai2.appinventor.mit.edu/).
  
![alt text](https://github.com/fabfarm/autovalve/blob/master/ESP01-pins.png "ESP-01 pins")

* ### ESP-01 WiFi LEDs controller using 4 GPIO pins
  ESP-01 controls 4 LEDs independently via self-hosted web page using GPIO 0, 1 (TX), 2 and 3 (RX). 
  Note: GPIO 2 should not be connected to LED at boot up but it can be set as INPUT to sensor.

* ### Measure AC Current using ACS712 and Arduino UNO

  #### ADC pin

![alt text](https://github.com/fabfarm/autovalve/blob/master/ESP8266EX.jpg "ESP-01 ADC pin")

  #### Extra GPIO pins

![alt text](https://github.com/fabfarm/autovalve/blob/master/esp8266_extra_gpio.jpg "ESP-01 extra GPIOs")
