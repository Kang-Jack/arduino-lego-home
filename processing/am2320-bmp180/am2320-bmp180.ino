#include <AM2320.h>
#include <Adafruit_BMP085.h>

AM2320 th;
Adafruit_BMP085 bmp;
 
void setup() 
{
  Serial.begin(9600);

   th.begin();
   
  //Wire.begin (4, 5);
  if (!bmp.begin()) 
  {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77");
    while (1) {}
  }
}
 
void loop() 
{
  get_bmp_data();
  get_th_data();
}

void get_bmp_data(){
  Serial.println("Chip = bmp180");
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" Celsius");
 
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pascal");

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");

  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");

// you can get a more precise measurement of altitude
// if you know the current sea level pressure which will
// vary with weather and such. If it is 1015 millibars
// that is equal to 101500 Pascals.
  Serial.print("Real altitude = ");
  Serial.print(bmp.readAltitude(101500));
  Serial.println(" meters");
    
 
  Serial.println();
  delay(1000);
}

void get_th_data(){
    if (th.measure()) {
    Serial.print("Temperature: ");
    Serial.println(th.getTemperature());
    Serial.print("Humidity: ");
    Serial.println(th.getHumidity());
  }
  else {  // error has occured
    int errorCode = th.getErrorCode();
    switch (errorCode) {
      case 1: Serial.println("ERR: Sensor is offline"); break;
      case 2: Serial.println("ERR: CRC validation failed."); break;
    }    
  }

  delay(1000);
  
}


