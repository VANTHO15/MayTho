void setup() {
 
Serial.begin(500000);
}

void loop() {
  
int a= analogRead(A2);
Serial.println(String(a));
}
