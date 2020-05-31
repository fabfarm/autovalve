void lcd_write()
{
  if( myRTC.dayofmonth != day)
  {
    Serial.write(131);
    if (myRTC.dayofmonth<9)
    {
      Serial.print(0);
    }
    Serial.print(myRTC.dayofmonth);
    day = myRTC.dayofmonth;
  }
  if (myRTC.month != month)
  {
    Serial.write(134);
    if (myRTC.month<9)
    {
      Serial.print(0);
    }
    Serial.print(myRTC.month);
    month = myRTC.month;
  }
  if (myRTC.year != year)
  {
    Serial.write(137);
    Serial.print(myRTC.year);
    year = myRTC.year;
  }
  if (myRTC.hours != hours)
  {
    Serial.write(151);
    if (myRTC.hours<9)
    {
      Serial.print(0);
    }
    Serial.print(myRTC.hours);
    hours = myRTC.hours;
  }
  if (myRTC.minutes != minutes)
  {
    Serial.write(154);
    if (myRTC.minutes<9)
    {
      Serial.print(0);
    }
    Serial.print(myRTC.minutes);
    minutes = myRTC.minutes;
  }
  if (myRTC.seconds != seconds)
  {
    Serial.write(157);
    if (myRTC.seconds<9)
    {
      Serial.print(0);
    }
    Serial.print(myRTC.seconds);
    seconds = myRTC.seconds;
  }
  delay(500);
} 
