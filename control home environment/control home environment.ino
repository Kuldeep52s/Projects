#include <Servo.h>
#include<math.h>
#include <Vector.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
const char ssid[] = "Note 4";  //  SSID (name)
const char pass[] = "123456789";   //  password         
WiFiClient  client;

//---------Channel Details---------//
unsigned long counterChannelNumber = 809626;            // Channel ID
const char * myCounterReadAPIKey = "************"; // Read API Key
const char * myWriteAPIKey = "**************";      // Write API Key
const int Feild_gate=4       // The fiels for servo
const int FieldNumber1 = 1;  // The field you wish to read
const int FieldNumber2 = 2;  // The field you wish to read
const int FieldNumber3 = 3;  // The feidl for temprature reading 
//-------------------------------//
int frame_size = 10;
int mul_factor = 6;
int bound1[2];
int button=D4;
Vector<int>history;
Servo myservo;
void setup()
{
  Serial.begin(115200);
  pinMode(D5,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(button,INPUT);
  myservo.attach(D8);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}

void loop()
{
  
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println(" ....");
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
    Serial.println("Connected to Wi-Fi Succesfully.");
  }
   //-----------Servo--------------//
int y=digitalRead(button);
if(y==1)
{
  int order = ThingSpeak.readLongField(counterChannelNumber, Field_gate, myCounterReadAPIKey);
  int  statusCode = ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
     if(order==1)
  {
    analogWrite(D8,90);
  }
  else{
    analogWrite(D8,0);
  }
}else{
  analogWrite(D8,0);
}
    
  }
  else
  {
    Serial.println("Unable to read channel / No internet connection");
  }
 //-------------end of servo-----------//

  //---------------- Channel 1 ----------------//
  int temp1 = ThingSpeak.readLongField(counterChannelNumber, FieldNumber1, myCounterReadAPIKey);
int  statusCode = ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
    digitalWrite(D5,temp1);
    Serial.println(temp1);
    
  }
  else
  {
    Serial.println("Unable to read channel / No internet connection");
  }
  delay(100);
  //-------------- End of Channel 1 -------------//

  //---------------- Channel 2 ----------------//
  int temp2 = ThingSpeak.readLongField(counterChannelNumber, FieldNumber2, myCounterReadAPIKey);
  statusCode = ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
    digitalWrite(D7,temp2);
  }
  else
  {
    Serial.println("Unable to read channel / No internet connection");
  }
  delay(100);

  //---------------- Channel 3 ----------------//
  int Temp_data=analogRead(A0);
 int x = ThingSpeak.writeField(counterChannelNumber, FieldNumber3, Temp_data, myWriteAPIKey);
  if(x == 200)
  {
    Serial.println("Channel update successful.");
    bound = anamoly(history_data,frame_size,mul_factor,bound1);
    if(!bound)
    {
      int required_data_count=frame_size-history.size();
      Serial.print("not enough data to compute z-score");
      Serial.println(required_data_count);
      history.push_back(Temp_data);
      delay(10);
      continue;
    }
    if(Tem_data>bound1[0])
    {
      Serial.print("the temprature value is higher");
    }
    if(Tem_data<bound1[1])
    {
      Serial.print("the temprature value is lower");
    }
  }
  else{
    Serial.println("Problem updating channel. HTTP error code ";
  }
  
  delay(100);
  //-------------- End of Channel 3 -------------//
}

int anamoly(int history_data,int frame_size,int mul_factor,int bound1[])
{
  if(history.size()<frame_size)
  {
    return None;
  }
  if(history.size()>frame_size)
  {
    history.erase(0,history.size()-frame_size);
  }
  int mean=0;
  for(int i=0;i<history.end();i++)
  {
    mean+=history[i];
  }
  mean=mean/history.size();
  int variance=0;
  for(int i=0;i<history.size();i++)
  {
    variance=variance+pow(history[i]-mean,2);
  }
  int zn=mul_factor*(sqrt(variance/frame_size));
  bound1[0]=history[frame_size-1]+zn;
  bound1[1]=history[frame_size-1]-zn;
  return 1;
}
