//23/12/15 ver.1.1

#include <virtuabotixRTC.h>
virtuabotixRTC myRTC(6, 7, 8);
int day,year,hours,minutes,seconds, month;

void setup()
{
  Serial.begin(9600);
  init_lcd();
}

void loop()
{
  updateclock();
  lcd_write();
}
