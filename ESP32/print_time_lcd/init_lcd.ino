void init_lcd()
{
  int backlight_on = 17;
  int backlight_off = 18;
  int line_feed = 10;
  int clear_disp = 12; //clears screen; provide 5ms delay
  delay(5020);

  Serial.write(0);
  delay(10);
  Serial.write(12);
  delay(500);
  Serial.write(18);
  delay(50);
}
