
void setup() {  
  SerialUSB.begin(9600);
  Serial2.begin(9600);

  pinMode(VCC_SW, OUTPUT);
  digitalWrite(VCC_SW, HIGH);
}

void loop() {  
  char c;
  if (SerialUSB.available()) {
    c = SerialUSB.read();
    Serial2.print(c);
  }
  if (Serial2.available()) {
    c = Serial2.read();
    SerialUSB.print(c);    
  }
}
