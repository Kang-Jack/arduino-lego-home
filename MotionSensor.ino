char* ssid = "xxx"; //Wi-Fi AP Name
char* password = "xxx"; //Wi-Fi Password
char* mqtt_server = "192.168.xxx.xx"; //MQTT Server IP
char* mqtt_name = "LegoMotionSensor"; //MQTT device name
char* mqtt_topic = "lego/motion"; //MQTT topic for communication
char* mqtt_ending = "/data"; //MQTT subsection for communication
int pirPin = 2;  //set the GPIO which you will connect the PIR sensor
int zeroPin = 0;
bool lowPower = false; //set to true if you want low power use, slower alerts but more battery
int delayTime = 2000; //ONLY FOR LOW POWER - how long motion detected should be active

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
WiFiClient mainESP;
PubSubClient MQTT(mainESP);
long lastMsg = 0;
char msg[50];
int value = 0;
int pir = 1;
boolean isRemoteOn;

char* mqtt_subtopic = "/data"; //MQTT topic for communication
char* mqtt_maintopic = mqtt_topic;
char* mqtt_sub = "home/command/lego/nano/motion";

void setup() {
  strcat(mqtt_maintopic, mqtt_subtopic);
  pinMode(pirPin, INPUT);
  pinMode(zeroPin,OUTPUT);
  Serial.begin(115200);
  isRemoteOn=false;
}

void loop() {
  if (!lowPower && WiFi.status() != WL_CONNECTED) startWiFi();
    //Serial.println(digitalRead(pirPin));
  if (isRemoteOn ==true)
  {
    //if (!MQTT.connected()) reconnect();
    //MQTT.publish(mqtt_topic, "remote high");
    digitalWrite(zeroPin,HIGH);
  }
  if (digitalRead(pirPin) == 0 & isRemoteOn ==false) {
    //if (!MQTT.connected()) reconnect();
    //MQTT.publish(mqtt_topic, "remote low");
    digitalWrite(zeroPin,LOW);
  }

  if ((digitalRead(pirPin)) == 0) 
  { 
    if (MQTT.connected() && pir == 0)
    {
      MQTT.publish(mqtt_topic, "FALSE");
      Serial.println("Message Published: FALSE");
      pir = 1;
    }
    delay(250);
  }
  else {
    if (WiFi.status() != WL_CONNECTED) startWiFi();
    MQTT.loop();
    if (!MQTT.connected()) reconnect();
    MQTT.publish(mqtt_topic, "TRUE");
    Serial.println("Message Published: TRUE");
    while (digitalRead(pirPin) == 1) {
      if (isRemoteOn==false){
         digitalWrite(zeroPin,HIGH);
         MQTT.loop();
         delay(5000);
      }
      if (!MQTT.connected()) reconnect();
      MQTT.publish(mqtt_topic, "TRUE");
      Serial.println("Message Published1: TRUE");
      delay(200);
    }
    pir = 0;
    if (!MQTT.connected()) reconnect();
    if (lowPower) delay(delayTime);
    MQTT.publish(mqtt_topic, "FALSE");
    Serial.println("Message Published: FALSE");
  }
  if (!MQTT.connected()) reconnect();
  MQTT.loop();
  delay(200);
  if (lowPower) {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic);
  int i=0;
  for(i=0; i<length; i++) {
      if (i<50) msg[i] = (char)payload[i];
  }
  if (i<50) msg[i] = '\0';
  else  msg[49] = '\0';
  if (msg[0]=='o'||msg[0]=='c')
  {
    String msgString((char*)msg); 
    if (msgString == "open") isRemoteOn=true; 
    if (msgString == "close") isRemoteOn=false; 
  }
}

void reconnect() {
  while (!MQTT.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (MQTT.connect(mqtt_name)) {
      MQTT.publish(mqtt_topic,"hello-motion1");
      delay(200);
      // ... and resubscribe
      MQTT.subscribe(mqtt_sub);
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(MQTT.state());
      Serial.println(" try again in 5 seconds");
      for (int i = 0; i < 5000; i++) {
        delay(1);
      }
    }
  }
}

void startWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
  WiFi.hostname(mqtt_name);
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  MQTT.setServer(mqtt_server, 1883);
  MQTT.setCallback(callback);
}
