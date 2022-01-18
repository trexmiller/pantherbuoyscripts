
//Function to read Miller lab PAR sensor
void readPAR() {
  tcaselect(7);
  mcp.pinMode(11, OUTPUT);
  mcp.digitalWrite(11, HIGH);
  delay(100);
  PARStats.clear();
  float val = 0;
  for (int i = 0; i < 50; ++i) {
    val = analogRead(PARpin);
    val *= 2;
    val *= (3.3 / 4096);
    val *= 1000;
    PARStats.add(val);
    delay(50);
  }
  PARAvg = PARStats.average();
  PARStd = PARStats.pop_stdev();
  PARStats.clear();
  mcp.digitalWrite(11, LOW);
  delay(100);
}
