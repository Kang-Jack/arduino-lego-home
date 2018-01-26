#include "SoftwareSerial.h"

int redPin = 3;  // Nano 3 R - Arduino - 11
int bluePin = 5;  //Nano5, B - Arduino - 10
int greenPin = 6;  //Nano6, G - Arduino - 9 
const int max_length=100;
char msg[max_length];   // initialise storage buffer (i haven't tested to this capacity.)

//char* ssid = "xxx"; //Wi-Fi AP Name
//char* password = "xxx"; //Wi-Fi Password

SoftwareSerial soft(8,9); // RX, TX
void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  soft.begin(9600);
}

void loop() {
  if(msg[0]!='\0'){ 
    Serial.print((char*)msg);
    String msgString((char*)msg);
    if (msgString == "red") color(255, 0, 0); // red
    if (msgString == "blue") color(0, 0, 255); // blue  
    if (msgString == "green") color(0,255, 0); //green
    if (msgString == "yellow") color(255,255,0); // yellow  
    if (msgString == "purple") color(128,0,255); // purple 
    if (msgString == "close") color(0,0,0); // close 
    soft.print("received");
    msg[0] ='\0';
  }
  if (msg[0]!='\0') color(255,255,255); // white  
  listen_esp();
  talk_esp();
  delay(100);
}

void listen_esp()
{
    int i=0;
    while(soft.available() > 0) 
    {
      char a = soft.read();
      if(a == '\0')
        continue;
      if(a != '\r' && a != '\n' && (a < 32))
        continue;
      if (i < max_length) msg[i] = a;

      Serial.print(a);
      i++;
    }
    if (i<max_length) msg[i] = '\0';
    else  msg[max_length-1] = '\0';
}

void talk_esp()
{
    while(Serial.available() > 0)
    {
      char a = Serial.read();
      Serial.write(a);
      soft.write(a);
    }
 }
 
void color (unsigned char red, unsigned char green, unsigned char blue) 
{
  analogWrite(redPin, 255-red);
  analogWrite(greenPin, 255-green);
  analogWrite(bluePin, 255-blue);
}
