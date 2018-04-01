
#define ACCESS_POINT_NAME  "ESP12FWeather"
#define ACCESS_POINT_PASSWORD  "12345678"
int AdminTimeOutCounter = 0;                  // Counter for Disabling the AdminMode
boolean OTAEnabled = false;
boolean AdminEnabled = true;    // Enable Admin Mode for a given Time
boolean needInitMQTT = true; 


struct strConfig {
  String ssid;
  String password;
  byte  IP[4];
  byte  MqttIP[4];
  long  MqttPort;
  byte  Netmask[4];
  byte  Gateway[4];
  boolean isAP;
}   config;

struct weatherData {
  float bmpTemp;
  int32_t bmpPres;
  int32_t bmpSealevelPres;
  float bmpAlti; // std atmosphere
  float bmpRealAlti; // std atmosphere
  float am2320Temp;
  float am2320Humi;
  String am2320Err;
}   weatherData;

static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; 
#define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )


struct  strDateTime
{
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte wday;

} ;

