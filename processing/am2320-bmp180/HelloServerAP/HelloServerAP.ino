#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <AM2320.h>
#include <Adafruit_BMP085.h>

AM2320 th;
Adafruit_BMP085 bmp;

//const char* ssid = "........";
//const char* password = "........";

//ESP8266WebServer server(80);
#include "FS.h"
#include <ArduinoJson.h>
#include <Ticker.h>

#include "datad.h"
#include "helpers.h"
#include "WebGlobal.h"

#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"


#include "PAGE_NetworkConfiguration.h"
#include "Page_Information.h"
#include "Page_AM2320Information.h"
#include "Page_BMPInformation.h"




void readBMP(){
  weatherData.bmpTemp = bmp.readTemperature();

  weatherData.bmpPres = bmp.readPressure();

  weatherData.bmpAlti = bmp.readAltitude();

  weatherData.bmpSealevelPres = bmp.readSealevelPressure();

  weatherData.bmpRealAlti = bmp.readAltitude(101500);
}
void readAM2320(){
  if (th.measure()) {
    weatherData.am2320Temp = th.getTemperature();
    weatherData.am2320Humi = th.getHumidity();
    weatherData.am2320Err = "";
  }
  else {  // error has occured
      int errorCode = th.getErrorCode();
      switch (errorCode) {
        case 1: weatherData.am2320Err ="ERR: Sensor is offline.\n\n"; break;
        case 2: weatherData.am2320Err ="ERR: CRC validation failed.\n\n"; break;
      }    
  }
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
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  delay(500);
  Serial.println("Starting ESP12F2");
  ReadConfig();
  Serial.println("config fetched"); 



  th.begin();
  if (!bmp.begin()) 
  {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77");
    while (1) {}
  }

  ConfigureWifi();
  //WiFi.disconnect(true);
  //delay(500);
  //WiFi.mode(WIFI_AP);
  //WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
  //WiFi.begin(ssid, password);
  //Serial.println("");

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

  server.on ( "/", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/admin.html", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
  server.on ( "/favicon.ico",   []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );
  
  server.on ( "/info.html", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );

  server.on ( "/config.html", send_network_configuration_html );

  server.on ( "/values", send_network_configuration_values_html );

  server.on ( "/connectionstate", send_connection_state_values_html );

  server.on ( "/infovalues", send_information_values_html );

  server.on("/bmp.html",[](){
    readBMP();
    //strBMP();
    server.send(200, "text/html", BMP_Information);
  });

  server.on ( "/bmpvalues", send_bmp_values_html);

  server.on("/th.html",[](){
    readAM2320();
    //strTH();
    server.send(200, "text/html", AM2320_Information);
  });

  server.on ( "/am2320values", send_am2320_values_html);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}
//
// FILL WITH INFOMATION
// 

void send_bmp_values_html ()
{
  readBMP();
  String values ="";
  values += "x_temp|" + (String)weatherData.bmpTemp +  "|div\n";
  values += "x_press|" +  (String)weatherData.bmpPres +  "|div\n";
  values += "x_alti|" +  (String)weatherData.bmpAlti +  "|div\n";
  values += "x_sl_press|" + (String)weatherData.bmpSealevelPres +  "|div\n";
  values += "x_real_alti|" + (String)weatherData.bmpRealAlti +  "|div\n";
  server.send ( 200, "text/plain", values);
  //Serial.println(__FUNCTION__); 

}

//
// FILL WITH INFOMATION
// 

void send_am2320_values_html ()
{
  readAM2320();
  String values ="";
  values += "x_temp|" + (String)weatherData.am2320Temp +  "|div\n";
  values += "x_humi|" +  (String)weatherData.am2320Humi +  "|div\n";
  values += "x_err|" +  weatherData.am2320Err+  "|div\n";
  server.send ( 200, "text/plain", values);
  //Serial.println(__FUNCTION__); 
}
/*
void strBMP(){
  //get_bmp_data();
  String message = "Chip = bmp180\n\n";
  message += "Temperature = ";
  message += weatherData.bmpTemp;
  message += " Celsius\n\n";
 
  message += "Pressure = ";
  message += weatherData.bmpPres;
  message += " Pascal\n\n";

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  message += "Altitude = ";
  message += weatherData.bmpRealAlti;
  message += " meters\n\n";

  message += "Pressure at sealevel (calculated) = ";
  message += weatherData.bmpSealevelPres;
  message += " Pa\n\n";

// you can get a more precise measurement of altitude
// if you know the current sea level pressure which will
// vary with weather and such. If it is 1015 millibars
// that is equal to 101500 Pascals.
  message += "Real altitude = ";
  message += weatherData.bmpRealAlti;
  message += " meters\n\n";
  Serial.print(message);
}

void strTH(){
  //get_bmp_data();
  String message = "Chip = am2320\n\n";
  if (th.measure()) {
    message += "Temperature = ";
    message += weatherData.am2320Temp;
    message += " Celsius\n\n";
  
    message += "Humidity = ";
    message += weatherData.am2320Humi;
    message += " % \n\n";
  }
  else {  // error has occured
    message += weatherData.am2320Err;
    }
  
  Serial.print(message);
}
*/


void loop(void){
  server.handleClient();

}
