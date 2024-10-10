/*
http://www.sparkfun.com/datasheets/Sensors/gp2y1010au_e.pdf
Sharp pin 1 (V-LED) => 5V (connected to 150ohm resister)
Sharp pin 2 (LED-GND) => Arduino GND pin
Sharp pin 3 (LED) => Arduino pin 2
Sharp pin 4 (S-GND) => Arduino GND pin
Sharp pin 5 (Vo) => Arduino A0 pin
Sharp pin 6 (Vcc) => 5V
*/

int dustPin=0;
int ledPower=2;
int delayTime=280;
int delayTime2=40;
float offTime=9680; //10000-280-40

int dustVal=0;
char s[32];
float voltage = 0;
float dustdensity = 0;

void setup(){
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);  
}

void loop(){
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(delayTime);
  dustVal=analogRead(dustPin); // read the dust value
  delayMicroseconds(delayTime2);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(offTime);
  
  voltage = dustVal*0.0049;//dustval*5/1024
  dustdensity = 0.172*voltage-0.1;
  
  if (dustdensity < 0 )
  dustdensity = 0;
  if (dustdensity > 0.5)
  dustdensity = 0.5;
  String dataString = "";
  dataString += dtostrf(voltage, 9, 4, s);
  dataString += "V,";
  dataString += dtostrf(dustdensity*1000.0, 5, 2, s);
  dataString += "ug/m3";

  Serial.println(dataString);
  delay(1000);
}
