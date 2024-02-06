int contador = 0;
int contador2 = 0x1;
void setup() {
  Serial.begin(1200);
}

void loop() {
  if (contador < 75 && contador2 < 16) {
    Serial.print("M0001C");
    Serial.print("000000");
    Serial.print(String(contador2,HEX));
    Serial.print("S1F");
  }
  else if (contador < 75 && contador2 > 15 && contador2 < 256) {
    Serial.print("M0001C");
    Serial.print("00000");
    Serial.print(String(contador2,HEX));
    Serial.print("S1F");
  }
  else if (contador < 75 && contador2 > 255) {
    Serial.print("M0001C");
    Serial.print("0000");
    Serial.print(String(contador2,HEX));
    Serial.print("S1F");
  }
  else {
    delay(100);
  }
  
  if (contador == 75) contador2++;
  if (contador == 275) contador = 0;
  contador++;
}
