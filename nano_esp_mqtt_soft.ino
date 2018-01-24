char* mqtt_name = "LegoNanoSensor"; //MQTT device name
char* mqtt_topic = "lego"; //MQTT topic for communication
char* mqtt_ending = "/data"; //MQTT subsection for communication
char* mqtt_sub = "home/command/lego/nano";

int delayTime = 2000; //ONLY FOR LOW POWER - how long motion detected should be active

#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>

int redPin = 3;  // Nano 3 ,R 红色LED 控制引脚 连接到Arduino的 11脚
int bluePin = 5;  //Nano5, B蓝色LED 控制引脚 连接到Arduino的 11脚 
int greenPin = 6;  //Nano6, G绿色LED 控制引脚 连接到Arduino的 9脚


char* mqtt_subtopic = "/data"; //MQTT topic for communication
char* mqtt_maintopic = mqtt_topic;

char message_buff[100];   // initialise storage buffer (i haven't tested to this capacity.)
//String msgString = "";

IPAddress server(192,168,xxx,xxx);
char ssid[] = "XXX";           // your network SSID (name)
char pass[] = "XXXX";           // your network password
int status = WL_IDLE_STATUS;   // the Wifi radio's status
int iprint = 0;
// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial soft(8,9); // RX, TX
void setup() {
  pinMode(redPin, OUTPUT); //设置redPin对应的管脚11为输出
  pinMode(greenPin, OUTPUT); //设置greenPin,对应的管脚9为输出
  pinMode(bluePin, OUTPUT); //设置bluePin对应的管脚10为输出
  
  strcat(mqtt_maintopic, mqtt_subtopic);
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  soft.begin(9600);
  startWiFi();
}

void color (unsigned char red, unsigned char green, unsigned char blue)  //颜色控制函数 
{    
  analogWrite(redPin, 255-red);   
  analogWrite(bluePin, 255-blue); 
  analogWrite(greenPin, 255-green); 
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
  //for (int i=0;i<length;i++) {
  //  Serial.print((char)payload[i]);
  //}
  int i=0;
  for(i=0; i<length; i++) {
      message_buff[i] = (char)payload[i];
      //msgString += (char)payload[i];
  }
  message_buff[i] = '\0';
  
  //msgString += String(message_buff);
  //Serial.println(msgString); 
  Serial.println();
}

void loop() {
  status= WiFi.status(); 
  
  if(message_buff[0]!='\0'){ 
  //if(msgString!=""){
    if (!client.connected()) reconnect();
    client.publish(mqtt_maintopic,"lego1-recevied");
    delay(500);
    Serial.println((char*)message_buff);
    String msgString((char*)message_buff);
    if (msgString == "red") color(255, 0, 0); // red
    if (msgString == "blue") color(0, 0, 255); // blue  
    if (msgString == "green") color(0,255, 0); //green
    if (msgString == "yellow") color(255,255,0); // yellow  
    if (msgString == "purple") color(128,0,255); // purple 
    if (msgString == "close") color(0,0,0); // close 
    message_buff[0] ='\0';
    //msgString = "";
  }
  if (message_buff[0]!='\0') color(255,255,255); // white  

  if (!client.connected()) reconnect();
  client.loop();
  
  if (status != WL_CONNECTED) iprint=1;
  if (iprint==1)Serial.println(status);
  if (status == WL_CONNECTED & iprint==1) iprint = 0 ;
  // put your main code here, to run repeatedly:
  if (!client.connected()) reconnect();
  client.loop();
  delay(200);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect(mqtt_name)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_maintopic,"hello-lego1");
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
