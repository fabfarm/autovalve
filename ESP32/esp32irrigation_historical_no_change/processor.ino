// Replaces placeholder with saved values
String processor(const String& var){
  //Serial.println(var);
  if(var == "valveRelay1_OnHour"){
    return readFile(SPIFFS, "/valveRelay1_OnHour.txt");
  }
  else if(var == "valveRelay1_OnMin"){
    return readFile(SPIFFS, "/valveRelay1_OnMin.txt");
  }
  else if(var == "valveRelay1_OffHour"){
    return readFile(SPIFFS, "/valveRelay1_OffHour.txt");
  }
  else if(var == "valveRelay1_OffMin"){
    return readFile(SPIFFS, "/valveRelay1_OffMin.txt");
  }
  
  else if(var == "valveRelay2_OnHour"){
    return readFile(SPIFFS, "/valveRelay2_OnHour.txt");
  }
  else if(var == "valveRelay2_OnMin"){
    return readFile(SPIFFS, "/valveRelay2_OnMin.txt");
  }
  else if(var == "valveRelay2_OffHour"){
    return readFile(SPIFFS, "/valveRelay2_OffHour.txt");
  }
  else if(var == "valveRelay2_OffMin"){
    return readFile(SPIFFS, "/valveRelay2_OffMin.txt");
  }
  
  else if(var == "valveRelay3_OnHour"){
    return readFile(SPIFFS, "/valveRelay3_OnHour.txt");
  }
  else if(var == "valveRelay3_OnMin"){
    return readFile(SPIFFS, "/valveRelay3_OnMin.txt");
  }
  else if(var == "valveRelay3_OffHour"){
    return readFile(SPIFFS, "/valveRelay3_OffHour.txt");
  }
  else if(var == "valveRelay3_OffMin"){
    return readFile(SPIFFS, "/valveRelay3_OffMin.txt");
  }
  
  else if(var == "LowCurrentLimit"){
    return readFile(SPIFFS, "/LowCurrentLimit.txt");
  }
  else if(var == "HighCurrentLimit"){
    return readFile(SPIFFS, "/HighCurrentLimit.txt");
  }
  // return the RTC time
  else if(var == "RTChour"){
    return readFile(SPIFFS, "/HighCurrentLimit.txt");
  }
  return String();
}
