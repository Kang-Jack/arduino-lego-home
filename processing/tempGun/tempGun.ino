
#include <Wire.h>
#include "SSD1306.h"

const int pinSDA = 4;
const int pinSCL = 5;
SSD1306  display(0x3c, pinSDA, pinSCL);

#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const int buttonPin = 14;    // the number of the pushbutton pin

void init_oled() {
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 0, "Init OLED");
}
void set_s_text(){
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);;
}
void set_m_text(){
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);;
}
void set_b_text(){
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);
}

void new_screen_oled(int x,int y,char* mesg){
  display.clear();
  display.drawString(x, y, mesg);
  display.display();
  delay(1000);
}


void display_F_temp(){
    display.clear();
    display.setLogBuffer(5, 30);
    set_s_text();
    display.println("Object (*F) = ");
    display.println( mlx.readObjectTempF());
    Serial.println("====================");
    display.println("Ambient (*F) = ");
    display.println( mlx.readAmbientTempF());
    display.drawLogBuffer(0, 0);
    display.display();
    delay(2000);

}

void display_object_temp(){
    display.clear();
    set_m_text();
    display.drawString(0, 0,"Object (*C) = ");
    set_b_text();
    display.drawString(0,22,String(mlx.readObjectTempC()));
    display.display();
    delay(2000);
}

void display_ambient_temp(){
    display.clear();  
    set_m_text();
    display.drawString(0,0,"Ambient (*C) = ");
    set_b_text();
    display.drawString(0,22, String(mlx.readAmbientTempC()));
    display.display();
    delay(2000);
}

void setup(void){
  //Wire.pins(int sda, int scl);
  Wire.pins(pinSDA, pinSCL);
  Serial.begin(115200);
  init_oled();
  Serial.println("init");
 if (!mlx.begin()) 
  {
     new_screen_oled(0,0,"Could not find MLX90614 sensor");
    while (1) {}
  }

  pinMode(buttonPin, INPUT);
  Serial.println("start");
  new_screen_oled(0,0,"Starting ESP12F2");

}

  void loop(void){
    int reading = digitalRead(buttonPin);
    Serial.println(reading); 
    if(reading==LOW)
    {
       //new_screen_oled(0,0," button pushed");
        display_object_temp();
        display_ambient_temp(); 
        display_F_temp();
        Serial.println("===================="); 
    }
    else
    {
      new_screen_oled(0,0,"No button pushed");
    }
}
