#include <MaxMatrix.h>
const int ledRPin = 10;
const int ledYPin = 6;            
const int ledGPin = 7;
const int buzzerPin = 8;

const int buttonPin =2;     
//
//int buttonState = 0; 
//
//int lightState = 0;


unsigned char i;
unsigned char j; 
 
const int Max7219_pinCLK = 11;
const int Max7219_pinCS = 12;
const int Max7219_pinDIN = 13;
int maxInUse = 1;

MaxMatrix m(Max7219_pinDIN, Max7219_pinCS, Max7219_pinCLK, maxInUse); 

char K[] = {8, 8, B00000000,B00100100,B00101000,B00110000,B00101000,B00100100,B00100100,B00000000};

char A[] = {8, 8, B00000000,B00011000,B00100100,B00100100,B00111100,B00100100,B00100100,B00000000};
           
char N[] = {8, 8, B00000000,B00100010,B00110010,B00101010,B00100110,B00100010,B00000000,B00000000};

char G[] = {8, 8, B00000000,B00111110,B00100000,B00100000,B00101110,B00100010,B00111110,B00000000};

char L[] = {8, 8, B00000000,B00100000,B00100000,B00100000,B00100000,B00100000,B00111100,B00000000};

char O[] = {8, 8, B00000000,B00111100,B01000010,B01000010,B01000010,B01000010,B00111100,B00000000};

char V[] = {8, 8, B00000000,B00100010,B00100010,B00100010,B00010100,B00010100,B00001000,B00000000};

char E[] = {8, 8, B00000000,B00111100,B00100000,B00111000,B00100000,B00100000,B00111100,B00000000};

char heart[] =
{8,8,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};


char neutral[]= {8,8,0x3C,0x42,0xA5,0x81,0xBD,0x81,0x42,0x3C};
char smile[]=   {8,8,0x3C,0x42,0xA5,0x81,0xA5,0x99,0x42,0x3C};
char smart[]= {8,8,0x08,0x04,0x02,0x11,0x02,0x04,0x08,0x00};


String data="Hello From Arduino!";

void setup() 
{ 
  Serial.begin(9600);
  
  pinMode(ledRPin,OUTPUT);         //定义引脚为输出模式
  pinMode(ledGPin,OUTPUT);
  pinMode(ledYPin,OUTPUT);

  pinMode(buzzerPin,OUTPUT);

  pinMode(buttonPin, INPUT);   


  m.init(); // MAX7219 initialization
  m.setIntensity(8); // initial led matrix intensity, 0-15
  
} 
void loop() 
{ 
  Serial.println(data);//data that is being Sent
  if (Serial.available()) {
    trafficLight((Serial.read()-'0'),5000,true);
  }
  delay(500);
  
  m.clear();
  m.writeSprite(0, 0, heart);
  
  delay(1000);

  trafficLights();

  m.clear();
  m.writeSprite(0, 0, smart);
  delay(1000);
  m.writeSprite(0, 0, neutral);
  delay(1000);
  m.writeSprite(0, 0, smile);
  delay(1000);
  
  
  m.writeSprite(0, 0, L);
  delay(1000);

  m.writeSprite(0, 0, O);
  delay(1000);
  
  m.writeSprite(0, 0, V);
  delay(1000);
  
  m.writeSprite(0, 0, E);
  delay(1000);  
  
  m.writeSprite(0, 0, K);
  delay(1000);  
  
  // Displaying the character at x,y (upper left corner of the character)  
  m.writeSprite(0, 0, A);
  delay(1000);

  m.writeSprite(0, 0, N);
  delay(1000);  
  
  m.writeSprite(0, 0, G);
  delay(1000);  

  m.writeSprite(0, 0, heart);
  delay(1000);
  
  for (int i=0; i<8; i++){
    m.shiftUp(false);
    delay(300);
  }
  m.clear();

  delay(500);
}

void buttonClose()
{
  digitalWrite(ledRPin,LOW);
  digitalWrite(ledGPin,LOW);
  digitalWrite(ledYPin,LOW);
  digitalWrite(buzzerPin,LOW);
}
 
void trafficLights()
{
   buttonClose();
   trafficYellowLight();
   trafficLight(ledRPin,5000,true);
   trafficYellowLight();
   trafficLight(ledGPin,1000,false);

 }
 
void trafficLight(int lightPin,int msDelay,bool isOff)
{
   digitalWrite(lightPin,HIGH);
   if(isOff)
   {
      delay(msDelay);
      digitalWrite(lightPin,LOW);
   }
   delay(500); 

  }

void trafficYellowLight()
{
   blinkLight(ledYPin);
   buzzering(80,1);
   blinkLight(ledYPin);
   buzzering(80,1);
   blinkLight(ledYPin);
   buzzering(80,1);
   blinkLight(ledYPin);
   buzzering(200,2);
 }
  
 void blinkLight(int lightPin)
{
   digitalWrite(lightPin,HIGH);
   delay(500); 
   digitalWrite(lightPin,LOW);
   delay(500); 
  }

 void buzzering(int h,int z)
 {
    unsigned int i;
    for(i=0;i<h;i++)
    { 
      digitalWrite(buzzerPin,HIGH);//发声音 
      delay(z);
      digitalWrite(buzzerPin,LOW);//不发声音 
      delay(z); 
    } 
  }

