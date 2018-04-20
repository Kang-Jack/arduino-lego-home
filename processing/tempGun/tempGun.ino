
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const int buttonPin = 11;    // the number of the pushbutton pin

void set_oled(int x,int y,int font_size) {
  display.setTextSize(font_size);
  display.setTextColor(WHITE);
  display.setCursor(x,y);
  display.clearDisplay();
}
void new_line_oled(char* mesg){
    display.clearDisplay();
    display.println(mesg);
    display.display();
    delay(100);
}
void append_line_oled(char* mesg){
    display.println(mesg);
    delay(100);
}

void display_temp(){
    set_oled(0,0,2);
    display.println("Object = ");
    display.println(mlx.readObjectTempC());
    display.print(" *C");
    display.println( mlx.readObjectTempF());
    display.print(" *F");
    display.println("====================");
    display.println("Ambient = ");
    display.println( mlx.readAmbientTempC());
    display.print(" *C");
    display.println( mlx.readAmbientTempF());
    display.print(" *F");
    delay(1000);
    display.clearDisplay();
}

void setup(void){

  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //0x3C initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  delay(2000);
  set_oled(0,0,1);

   if (!mlx.begin()) 
  {
     new_line_oled("Could not find MLX90614 sensor at 0x5A");
    while (1) {}
  }

  pinMode(buttonPin, INPUT);
    
  new_line_oled("Starting ESP12F2");

}

  void loop(void){
    int reading = digitalRead(buttonPin);
    if(reading==HIGH)
    {
        display_temp();  
    }
}
