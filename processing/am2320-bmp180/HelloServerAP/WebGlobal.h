#ifndef WEBGLOBAL_H
#define WEBGLOBAL_H



boolean firstStart = true;                    // On firststart = true, NTP will try to get a valid time

strDateTime DateTime;                     // Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;                     // NTP Client
unsigned long UnixTimestamp = 0;                // GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
Ticker tkSecond;                        // Second - Timer for Updating Datetime Structure

byte Minute_Old = 100;        // Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)

ESP8266WebServer server(80);                  // The Webserver
/*
**
** CONFIGURATION HANDLING
**
*/

 bool saveConfig() {
  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  json["ssid"] = config.ssid ;
  json["password"] = config.password ;
  json["isAP"] = config.isAP ;

  JsonArray& array_ip = json.createNestedArray("IP");
  array_ip.add(config.IP[0]);
  array_ip.add(config.IP[1]);
  array_ip.add(config.IP[2]);
  array_ip.add(config.IP[3]);

  JsonArray& array_mask = json.createNestedArray("Netmask");
  array_mask.add(config.Netmask[0]);
  array_mask.add(config.Netmask[1]);
  array_mask.add(config.Netmask[2]);
  array_mask.add(config.Netmask[3]);
  JsonArray& array_gateway = json.createNestedArray("Gateway");
  array_gateway.add(config.Gateway[0]);
  array_gateway.add(config.Gateway[1]);
  array_gateway.add(config.Gateway[2]);
  array_gateway.add(config.Gateway[3]);

  json.prettyPrintTo(Serial);
  Serial.println();

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}


bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }
  const char* id =json["ssid"];
  config.ssid = String(id);
  const char* password =json["password"];
  config.password = String(password);
  config.isAP = json["isAP"];
  JsonArray& jIP = json["IP"];
  config.IP[0] = jIP[0];
  config.IP[1] = jIP[1];
  config.IP[2] = jIP[2];
  config.IP[3] = jIP[3];
  JsonArray& mqttIP = json["MqttIP"];
  config.MqttIP[0] = mqttIP[0];
  config.MqttIP[1] = mqttIP[1];
  config.MqttIP[2] = mqttIP[2];
  config.MqttIP[3] = mqttIP[3];
  JsonArray& gateIP = json["Gateway"];
  config.Gateway[0] = gateIP[0];
  config.Gateway[1] = gateIP[1];
  config.Gateway[2] = gateIP[2];
  config.Gateway[3] = gateIP[3];
  JsonArray& maskIP = json["Netmask"];
  config.Netmask[0] = maskIP[0];
  config.Netmask[1] = maskIP[1];
  config.Netmask[2] = maskIP[2];
  config.Netmask[3] = maskIP[3];
  // Real world application would store these values in some variables for
  // later use.

  Serial.print("Loaded IP ");
  Serial.println(config.ssid);
  Serial.println(config.password);
  Serial.println(config.IP[0]);
  Serial.println(config.IP[1]);
  Serial.println(config.IP[2]);
  Serial.println(config.IP[3]);
  Serial.println();  
  //Serial.print("Loaded accessToken: ");
  //Serial.println(accessToken);
  return true;
}


void ConfigureWifi()
{
  WiFi.disconnect();
  if (config.isAP)
  {
    Serial.println("AP enabled");
    WiFi.mode(WIFI_AP);
    WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
  }
  else
  {
    Serial.println("Station disabled");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);

  	Serial.println("Configuring Wifi");
  	WiFi.begin (config.ssid.c_str(), config.password.c_str());
      // ... Give ESP 10 seconds to connect to station.
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000){
      Serial.write('.');
      //Serial.print(WiFi.status());
      delay(500);
    }
     while (WiFi.status() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      delay(500);
      ESP.restart();
    }
  }
  Serial.println();
}


void WriteConfig()
{

	Serial.println("Writing Config");
  if(saveConfig())  Serial.println("Wrote Config done");
  else Serial.println("Wrote Config faild");

}
void ReadConfig()
{
	Serial.println("Reading Configuration");
	if (loadConfig())return ;
  else{
      Serial.println("using default Configuration");

      // DEFAULT CONFIG
      config.ssid = "link";
      config.password = "xxx";
      config.isAP = true;
      config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 233;
      config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
      config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
      WriteConfig();
      Serial.println("General config applied");
      return ;
  }
}




#endif
