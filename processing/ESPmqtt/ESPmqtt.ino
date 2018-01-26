char* ssid = "xxx"; //Wi-Fi AP Name
char* password = "xxx"; //Wi-Fi Password
char* mqtt_server = "192.168.xx.xx"; //MQTT Server IP
char* mqtt_name = "ESPSensor"; //MQTT device name
char* mqtt_topic = "esp/answer" ;//"esp1s"; //MQTT topic for communication

bool lowPower = false; //set to true if you want low power use, slower alerts but more battery
int delayTime = 2000; //ONLY FOR LOW POWER - how long motion detected should be active

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
WiFiClient mainESP;
PubSubClient MQTT(mainESP);

//long lastMsg = 0;
//char msg[50];
//int value = 0;
//int pir = 1;

const int max_length=100;
char msg[100];

char* mqtt_subtopic = "/data"; //"/answer/data"; //MQTT topic for communication
char* mqtt_maintopic = mqtt_topic;
char* mqtt_sub = "home/command/esp1s/nano";


void setup() {
  strcat(mqtt_maintopic, mqtt_subtopic);
  Serial.begin(9600);
 // msg[0] = '\0';
}

void loop() {
  if (!lowPower && WiFi.status() != WL_CONNECTED) startWiFi();
  //delay(200);
  //if (!MQTT.connected()) reconnect();
  //MQTT.publish(mqtt_topic, "TRUE");
  //MQTT.loop();
  talk_master();

  listen_master();
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
  //Serial.print(topic);
  int i=0;
  for(i=0; i<length; i++) {
      if (i<max_length) msg[i] = (char)payload[i];
  }
  set_end_mark(i);
  //Serial.println();
}

void set_end_mark(int i){
  if (max_length>i) msg[i] = '\0';
  else  msg[max_length-1] = '\0';
}


void reconnect() {
  while (!MQTT.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (MQTT.connect(mqtt_name)) {
      Serial.println("connected MQTT");
      //Serial.println(mqtt_maintopic);
      //Serial.println(mqtt_sub);
      // Once connected, publish an announcement...
      MQTT.publish(mqtt_maintopic,"hello-ESP1s");
      // ... and resubscribe
      MQTT.subscribe(mqtt_sub);
    } else {
      //Serial.print("failed, rc=");
      //Serial.println(MQTT.state());
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
  //Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  MQTT.setServer(mqtt_server, 1883);
  MQTT.setCallback(callback);
}


void listen_master()
{
    int i = 0;
    while(Serial.available() > 0) 
    {
      char a = Serial.read();
      if(a == '\0')
        continue;
      if(a != '\r' && a != '\n' && (a < 32))
        continue;
      if (i<max_length) msg[i] = a ;
      
      i++;
    }
    if (i==0) return;
    set_end_mark(i);
    String msgString((char*)msg);
    if (msgString == "received")  msg[0]= '\0';
    else {
      publish_msg((char*)msg);
      msg[0]= '\0';
    }
}

void publish_msg(char* msg_str)
{
    if ( WiFi.status() != WL_CONNECTED) startWiFi();
    if (!MQTT.connected()) reconnect();
    MQTT.publish(mqtt_topic, msg_str);
    delay(200);
}

void talk_master()
{
    if(msg[0]!='\0'){ 
      Serial.print((char*)msg);
      //msg[0] = '\0';
    }
}
