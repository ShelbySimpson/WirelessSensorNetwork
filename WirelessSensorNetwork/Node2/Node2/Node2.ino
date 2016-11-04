void setup(){
  Serial.begin(9600);
  pinMode(A0,INPUT);
}

void loop(){
  //Serial.println("atesty test");
  //delay(500);
  char cmd = '0';
  
  cmd = Serial.read();
  if(cmd == 'R'){
    delay(100);
    Serial.println("NODE 2");
  }

  
  //Serial.println(analogRead(A0));
  //delay(500);
}

