char* mqtt_name = "LegoSensor"; //MQTT device name
char* mqtt_topic = "lego"; //MQTT topic for communication
char* mqtt_ending = "/data"; //MQTT subsection for communication
char* mqtt_sub = "home/command/lego/nano";

int delayTime = 2000; //ONLY FOR LOW POWER - how long motion detected should be active

#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>

char* mqtt_subtopic = "/data"; //MQTT topic for communication
char* mqtt_maintopic = mqtt_topic;

IPAddress server(192,168,31,143);
char ssid[] = "dlink624";           // your network SSID (name)
char pass[] = "123345";           // your network password
int status = WL_IDLE_STATUS;   // the Wifi radio's status

// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial soft(8,9); // RX, TX
void setup() {
  strcat(mqtt_maintopic, mqtt_subtopic);
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  soft.begin(9600);
  startWiFi();
}

void startWiFi() {
 // initialize ESP module
  WiFi.init(&soft);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  int i=0;
  while ( status != WL_CONNECTED) {
    i++;
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    if(i<6){
      status = WiFi.begin(ssid, pass);
    }
    else
    {
      WiFi.reset(); 
      i=0;
    }
    delay(1000);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //connect to MQTT server
  client.setServer(server, 1883);
  client.setCallback(callback);
}

//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) startWiFi();
  // put your main code here, to run repeatedly:
  if (!client.connected()) reconnect();
  client.loop();
  delay(500);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect(mqtt_name)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_topic,"hello world");
      // ... and resubscribe
      client.subscribe(mqtt_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
     //delay(5000);
      for (int i = 0; i < 5000; i++) delay(1);
    }
  }
}
