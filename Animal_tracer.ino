#include <LDateTime.h>
#include <LGPS.h>
#include <b64.h>
#include <HttpClient.h>
#include <LGPRS.h>
#include <LGPRSClient.h>
#include <LGPRSServer.h>
#include <LGPRSUdp.h>
#include <LBattery.h>
#define URL "api.mediatek.com"
#define DEVICEID "Dh0ALF54"
#define DEVICEKEY "7fBLrtpYI0Nq4cqF"
#define path "/mcs/v2/devices/"

int Touchpin = 2;
unsigned int rtc3;
unsigned int lrtc3;

char ubuff[256];
gpsSentenceInfoStruct info;
char buff[256];


#define UPDATEINTERVAL 30
char port[4]={0};
char connection_info[21]={0};
char ip[21]={0};             
int portnum;
int val = 0;
int ret = 0;
String tcpdata = String(DEVICEID) + "," + String(DEVICEKEY) + ",0";
//String upload_led;
String datachannel = "Set_configuration";
//String datachannel2 = "P_configuration";
int parseString = 40 + datachannel.length() + 1;
//int parseConfig = datachannel2.length() + 15;
int parseConfig = datachannel.length() + 15;
LGPRSClient globalclient;
HttpClient http(globalclient);
int test = 0;

String datapacket;

char Configdata[256];
int Config = 1;

char Counterdata[256];
int Count = 0;

char Batterydata[256];

char Touchdata[256];
int Touch = 1;

char GPSdata[512];

double latitude,lat_deg;
double longitude,long_deg;
int altitude;
double  converttodegree(double);
static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}
double converttodegree(double x)
{
    double y;
    y = x / 100;
    y = (int)y;
    x -= y * 100;
    return y + (x / 60);
}
   
void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
 
  int tmp, hour, minute, second, num ;
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    Serial.println(buff);
    
    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
   // sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
   lat_deg = converttodegree(latitude);
   long_deg = converttodegree(longitude);
   sprintf(buff, "latitude = %f, longitude = %f", lat_deg, long_deg);
    Serial.println(buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "satellites number = %d", num);
    Serial.println(buff); 
    tmp = getComma(9, GPGGAstr);
    altitude = getIntNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Altitude = %d", altitude);
    Serial.println(buff); 
  }
  else
  {
    Serial.println("Did not get data"); 
  }
}
void setup() {                                                //setup function starts
 
Serial.begin (115200);
//while (Serial.available() == 0) {} 

Serial.println("Hardware Serial Communication started");
LGPS.powerOn();
delay(1000);
pinMode(Touchpin, INPUT);
//Serial.println("Trying to attach to GPRS Network by auto-detect APN setting");
 while (!LGPRS.attachGPRS("internet", NULL, NULL))

  {
    Serial.println("Trying to activate GPRS");
    delay(1000);
  }
  delay(100);
  LGPRSClient client;
Serial.println("Connecting to mediatek cloud sandbox...");
  while (!(ret = client.connect("api.mediatek.com", 80)))
  {
     Serial.println(ret);
    Serial.println("Retrying to connect...");
    delay(1000);
  }
   Serial.println(ret);
  Serial.println("Connected!");
  globalclient = client;
}                                                                //setup function ends.

void loop() {                                                    //loop fucntion starts
  // put your main code here, to run repeatedly:
  globalclient.stop();
  while (!(ret = globalclient.connect("api.mediatek.com", 80)))
  {
     Serial.println(ret);
    Serial.println("Retrying to connect...");
    delay(1000);
  }
  Serial.println(ret);
  Serial.println("Connected!");
  GetConfig();
  Serial.print("The configuration on board is ");
  Serial.println(Config);
  Senddata();
  globalclient.stop();
  LDateTime.getRtc(&lrtc3);
  LDateTime.getRtc(&rtc3);
  while ((rtc3 - lrtc3) <= UPDATEINTERVAL)
  {
   delay(1000);
   LDateTime.getRtc(&rtc3);
  } 
    //  delay(10000);
 }     //loop function ends.
 
void GetConfig()
{
  String response = "";
  String Configvalue = "";
  bool Configread = 0; 
  globalclient.stop();
  while (!(ret = globalclient.connect("api.mediatek.com", 80)))
  {
     Serial.println(ret);
    Serial.println("Retrying to connect...");
    delay(1000);
  }
  Serial.println("Inside GetConfig function");
  globalclient.print("GET /mcs/v2/devices/");
  globalclient.print(DEVICEID);
  globalclient.println("/datachannels/Set_configuration/datapoints.csv HTTP/1.1");
  globalclient.print("Host: ");
  globalclient.println(URL);
  globalclient.print("deviceKey: ");
  globalclient.println(DEVICEKEY);
  //globalclient.println(("Content-Type: text/csv"));
  globalclient.println("Connection: close");
  globalclient.println();
  
   delay(10000);
   char d;
     HttpClient http(globalclient);
  int err = http.skipResponseHeaders();
  int bodyLen = http.contentLength();
  Serial.print("Content length is: ");
  Serial.println(bodyLen);
while (globalclient.available())
   {
      char ch;   
      int v = globalclient.read();
      if (v != -1)
      {
        ch = v;
        Serial.print(ch);
        response += (char)v;
       }
    }
   Serial.println();
    Configvalue = response.substring(parseConfig);  
        if (Configvalue.length() > 0) {
          Configread = 1;
           // Serial.println(response);
          Serial.println("After parsing Value");
        }
Serial.println();
if (Configread){
     Serial.print("Configvalue: ");
    Serial.println(Configvalue);
    int num = Configvalue.toInt();
    Config = num;
    Serial.print("Config value changed to ");
    Serial.println(num);
    }
}
void Senddata()
{
  globalclient.stop();
  while (!(ret = globalclient.connect("api.mediatek.com", 80)))
  {
     Serial.println(ret);
    Serial.println("Retrying to connect...");
    delay(1000);
  }
  if (Config == 0)
  {
  sprintf(Configdata,"P_configuration,,%d", Config);
  String filler = "\n";
  sprintf(Batterydata,"Battery_level,,%d", LBattery.level());
  datapacket = Configdata + filler + Batterydata;
  Serial.print("datapacket: ");
  Serial.println(datapacket);
  Serial.println("After datapacket");
  HTTPPOST(datapacket);
  Readresponse();
  globalclient.stop();
  return ;
  }
  else if (Config == 1 || Config == 2)
  {
  Count++;
  LGPS.getData(&info);
  Serial.println((char*)info.GPGGA); 
  parseGPGGA((const char*)info.GPGGA);
  delay(1000);
  sprintf(GPSdata,"GPS,,%f,%f,%d", lat_deg,long_deg,altitude);
  String filler = "\n";
  sprintf(Counterdata,"Counter,,%d\n", Count);
  sprintf(Configdata,"P_configuration,,%d\n", Config);
  sprintf(Batterydata,"Battery_level,,%d\n", LBattery.level() );
  if (digitalRead(Touchpin) == 1)
  {
   Touch = 0;
   sprintf(Touchdata,"Incontact,,%d", Touch);
  }
  else if (digitalRead(Touchpin) == 0)
  {
   Touch = 1;
   sprintf(Touchdata,"Incontact,,%d", Touch);
  }
  
  datapacket = GPSdata + filler + Counterdata + Configdata + Batterydata + Touchdata;
  Serial.print("datapacket: ");
  Serial.println(datapacket);
  Serial.println("After datapacket");
  HTTPPOST(datapacket);
  Readresponse();
  globalclient.stop();
  return ;
  }
}
void HTTPPOST(String datatosend)
{
String len = String(datatosend.length());
globalclient.print(F("POST "));
globalclient.print(path);
globalclient.print(DEVICEID);
globalclient.println(F("/datapoints.csv HTTP/1.1"));
globalclient.print(F("Host: "));
globalclient.println(URL);
globalclient.print(F("deviceKey: "));
globalclient.println(DEVICEKEY);
globalclient.print(F("Content-Length: "));
globalclient.println(len);
globalclient.println(F("Content-Type: text/csv"));
globalclient.println(F("Connection: close"));
globalclient.println();
globalclient.println(datatosend);
globalclient.println();
Serial.println("Data packet sent");
}
 
void Readresponse(void)
{
  delay(10000);
   char d;
while (globalclient.available()) //Reading the response from the server.
{
  d = globalclient.read();
  Serial.print(d);
}
Serial.println();
}
