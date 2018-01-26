char* ssid = "xxx"; //Wi-Fi AP Name
char* password = "xxx"; //Wi-Fi Password
char* mqtt_server = "192.168.xx.xx"; //MQTT Server IP
char* mqtt_name = "esp1s"; //MQTT device name
char* mqtt_topic = "esp1s"; //MQTT topic for communication

bool lowPower = false; //set to true if you want low power use, slower alerts but more battery
int delayTime = 2000; //ONLY FOR LOW POWER - how long motion detected should be active

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
WiFiClient mainESP;
PubSubClient MQTT(mainESP);

const int max_length=100;
char msg[max_length];

char* mqtt_subtopic = "/answer/data"; //MQTT topic for communication
char* mqtt_maintopic = mqtt_topic;
char* mqtt_sub = "home/command/esp1s/nano";

void listen()
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
//    if (msg[0]== '[')
//    {
//      set_config(&msgString);
//      msg[0]= '\0';
//      return;
//    }
    if (msgString == "received") msg[0]= '\0';
    else publish_msg((char*)msg);
}
//void set_config(char*){
//  char* str_out="";
//  if(msg_str.startsWith("[ssid]"))
//  {
//     str_out = msg_str.substring(6);
//     ssid = str_out;
//  }
//  else if(msg_str.startsWith("[psw]"))
//  {
//    str_out = msg_str.substring(5);
//    password = str_out;
//  }
//  else if(msg_str.startsWith("[m_s]"))
//  {
//    str_out = msg_str.substring(5);
//    mqtt_server = str_out;
//  }
//  else if(msg_str.startsWith("[m_n]"))
//  {
//    str_out = msg_str.substring(5);
//     mqtt_name = str_out;
//  }
//  else if(msg_str.startsWith("[m_t]"))
//  {
//    str_out = msg_str.substring(5);
//    mqtt_topic = str_out;
//  }
//  else if(msg_str.startsWith("[m_sub]"))
//  {
//    str_out = msg_str.substring(7);
//    mqtt_sub = str_out;
//  }
//  Serial.print("settings changed");
//  Serial.print(mqtt_sub);
//}
void publish_msg(char* msg_str)
{
    if ( WiFi.status() != WL_CONNECTED) startWiFi();
    if (!MQTT.connected()) reconnect();
    MQTT.publish(mqtt_topic, msg_str);
    delay(200);
}

void talk()
{
    if(msg[0]!='\0'){ 
      Serial.print((char*)msg);
      //msg[0] = '\0';
    }
 }

void setup() {
  strcat(mqtt_maintopic, mqtt_subtopic);
  msg[0] = '\0';
  Serial.begin(9600);
}

void loop() {
  if (!lowPower && WiFi.status() != WL_CONNECTED) startWiFi();
  talk();

  listen();

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
      // Once connected, publish an announcement...
      MQTT.publish(mqtt_maintopic,"hello-ESP1s");
      // ... and resubscribe
      MQTT.subscribe(mqtt_sub);
    } else {
      //Serial.print("failed, rc=");
      Serial.print(MQTT.state());
      //Serial.println(" try again in 5 seconds");
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
