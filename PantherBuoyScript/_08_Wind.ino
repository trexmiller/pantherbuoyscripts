//function to read wind direction
void readWindDir() {
  tcaselect(7);
  mcp.pinMode(11, OUTPUT);
  mcp.digitalWrite(11, HIGH);
  delay(100);
  readIMU();
  VaneValue = analogRead(A2);
  WindDir = map(VaneValue, 0, 1023, 0, 360);
  CalDirection = WindDir + Heading;

  if (CalDirection > 360)
    CalDirection = CalDirection - 360;

  if (CalDirection < 0)
    CalDirection = CalDirection + 360;

  mcp.digitalWrite(11, LOW);
  delay(100);
}

//Function to count anemometer rotations
void isr_rotation () {
  if ((millis() - ContactBounceTime) > 15 ) { // debounce the switch contact.
    Rotations++;
    ContactBounceTime = millis();
  }
}


//Function to read wind speed
void readWindSpeed() {
  tcaselect(7);
  mcp.pinMode(11, OUTPUT);
  mcp.digitalWrite(11, HIGH);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);
  Rotations = 0; // Set Rotations count to 0 ready for calculations
  delay(5000); // Wait 5 seconds to average
  WindSpeed = Rotations * 0.75;
  mcp.digitalWrite(11, LOW);
}
