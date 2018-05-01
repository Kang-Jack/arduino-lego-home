
#include <Wire.h>
#include "SSD1306.h"

const int pinSDA = 4;
const int pinSCL = 5;
SSD1306  display(0x3c, pinSDA, pinSCL);

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP280 bmp; // I2C

#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const int buttonPin = 14;    // the number of the pushbutton pin

const int delayTime = 2000; 
bool isProMode = false;

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
    display.println("Object (*F)");
    display.println( mlx.readObjectTempF());
    Serial.println("====================");
    display.println("Ambient (*F)");
    display.println( mlx.readAmbientTempF());
    display.drawLogBuffer(0, 0);
    display.display();
    delay(delayTime);

}

void display_object_temp(){
    display.clear();
    set_m_text();
    display.drawString(0, 0,"Object (*C)");
    set_b_text();
    display.drawString(0,22,String(mlx.readObjectTempC()));
    display.display();
    delay(delayTime);
}

void display_ambient_temp(){
    display.clear();  
    set_m_text();
    display.drawString(0,0,"Ambient (*C)");
    set_b_text();
    display.drawString(0,22, String(mlx.readAmbientTempC()));
    display.display();
    delay(delayTime);
}

void display_bme_temp() {

    display.clear();  
    set_m_text();
    display.drawString(0,0,"Temperature (*C)");
    set_b_text();
    display.drawString(0,22, String(bmp.readTemperature()));
    display.display();
    delay(1000);
}

void display_bme_Pressure() {

    display.clear();  
    set_m_text();
    display.drawString(0,0,"Pressure (hPa)");
    set_b_text();
    display.drawString(0,22, String(bmp.readPressure() / 100.0F));
    display.display();
    delay(1000);
}

void display_bme_Altitude() {
    display.clear();  
    set_m_text();
    display.drawString(0,0,"Calc.Altitude (m)");
    set_b_text();
    display.drawString(0,22, String(bmp.readAltitude(SEALEVELPRESSURE_HPA)));
    display.display();
    delay(delayTime);
}

void setup(void){
  //Wire.pins(int sda, int scl);
  Wire.pins(pinSDA, pinSCL);
  Serial.begin(115200);
  init_oled();
  Serial.println("init");
  
 if (!mlx.begin()) 
  {
    set_s_text();
    new_screen_oled(0,0,"Not find MLX90614");
    while (1) {}
  }
  
  bool status;
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bmp.begin();  
  if (!status) {
      set_s_text();
      new_screen_oled(0,0,"Not find BMP280");
      while (1);
  }

  pinMode(buttonPin, INPUT);
  Serial.println("start");
  set_m_text();
  new_screen_oled(0,0,"Starting ESP12S");
  
  int readMode = digitalRead(buttonPin);
  for (int i=0;i<5;i++){
    if (readMode==LOW){
      isProMode=true;
      set_m_text();
      new_screen_oled(0,0,"Pro. Mode");
      continue;
    }
    else{
      set_m_text();
        new_screen_oled(0,0,"Simple Mode");
        delay(500); 
        set_s_text();
        new_screen_oled(0,28,"Push button for Pro. mode");
        delay(500);   
        readMode = digitalRead(buttonPin);
      }
  }    
}

void loop(void){
  int reading = digitalRead(buttonPin);
  if(isProMode){
    if(reading==LOW)
    {
        set_m_text();
        new_screen_oled(0,0," button pushed");
        display_object_temp();
        display_ambient_temp(); 
        display_F_temp();
    }
    else
    {
      set_m_text();
      new_screen_oled(0,0," no button pushed");
      display_bme_temp();
      display_bme_Pressure();
      display_bme_Altitude();
    }
  }
  else{
    if(reading==LOW)
    {
        set_m_text();
        new_screen_oled(0,0," button pushed");
        display_object_temp();
    }
    else
    {
      set_m_text();
      new_screen_oled(0,0," no button pushed");
      display_bme_temp();
      display_bme_Pressure();
    }
  }
}

