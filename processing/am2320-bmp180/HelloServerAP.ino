#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <AM2320.h>
#include <Adafruit_BMP085.h>

AM2320 th;
Adafruit_BMP085 bmp;

#define ACCESS_POINT_NAME  "ESP12FWeather"
#define ACCESS_POINT_PASSWORD  "12345678"
//const char* ssid = "........";
//const char* password = "........";

ESP8266WebServer server(80);


void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){

  Serial.begin(115200);
  th.begin();
  if (!bmp.begin()) 
  {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77");
    while (1) {}
  }
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
  //WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  /*while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
*/
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/bmp", handleBMP);

  server.on("/th", handleTH);
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void handleBMP(){
  //get_bmp_data();
  String message = "Chip = bmp180\n\n";
  message += "Temperature = ";
  message += bmp.readTemperature();
  message += " Celsius\n\n";
 
  message += "Pressure = ";
  message += bmp.readPressure();
  message += " Pascal\n\n";

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  message += "Altitude = ";
  message += bmp.readAltitude();
  message += " meters\n\n";

  message += "Pressure at sealevel (calculated) = ";
  message += bmp.readSealevelPressure();
  message += " Pa\n\n";

// you can get a more precise measurement of altitude
// if you know the current sea level pressure which will
// vary with weather and such. If it is 1015 millibars
// that is equal to 101500 Pascals.
  message += "Real altitude = ";
  message += bmp.readAltitude(101500);
  message += " meters\n\n";
 Serial.print(message);
 server.send(200, "text/plain", message);
}

void handleTH(){
  //get_bmp_data();
  String message = "Chip = am2320\n\n";
  if (th.measure()) {
    message += "Temperature = ";
    message += th.getTemperature();
    message += " Celsius\n\n";
  
    message += "Humidity = ";
    message += th.getHumidity();
    message += " % \n\n";
  }
  else {  // error has occured
    int errorCode = th.getErrorCode();
    switch (errorCode) {
      case 1: message +="ERR: Sensor is offline.\n\n"; break;
      case 2: message +="ERR: CRC validation failed.\n\n"; break;
    }    
  }
  
 Serial.print(message);
 server.send(200, "text/plain", message);
}


void loop(void){
  server.handleClient();

}
