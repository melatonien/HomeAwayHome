//const int buzzer = 11;
const int led = 9;
const int trig_pin = 2;
const int echo_pin = 3;
float timing = 0.0;
float distance = 0.0;

void setup()
{
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
 // pinMode(buzzer, OUTPUT);
 pinMode(led, OUTPUT);
  
  digitalWrite(trig_pin, LOW);
  //digitalWrite(buzzer, LOW);
  digitalWrite(led, LOW);
    
  Serial.begin(9600);
}

void loop()
{
  digitalWrite(trig_pin, LOW);
  delay(2);
  
  digitalWrite(trig_pin, HIGH);
  //delayMicroseconds(10); // 10 microseconds
  digitalWrite(trig_pin, LOW);
  
  timing = pulseIn(echo_pin, HIGH);
  distance = (timing * 0.034) / 2;
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print("cm | ");
  Serial.print(distance / 2.54);
  Serial.println("in");
  
    
  if (distance <= 50) {
  	//tone(buzzer, 500);
    digitalWrite (led, HIGH);
  } else {
  	//noTone(buzzer);
    digitalWrite (led, LOW);
  }
  
  delay(100);
}