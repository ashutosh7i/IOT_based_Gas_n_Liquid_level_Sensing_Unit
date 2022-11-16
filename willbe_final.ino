#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

//Telegram ***********************************
// Replace with your network credentials
const char* ssid = "Realme 6 Pro";
const char* password = "244466666";
// Initialize Telegram BOT
#define BOTtoken "5758127178:AAGmL8SKflly19JfCPO2k9e7MYPV1WaNl7k"  // your Bot Token (Get from Botfather)
// CHAT ID
#define CHAT_ID "1172361469"
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
//*******************************************

//output led declaration
int green_led=18;   //step1 shows heating
int red_led=19; //step2 sensors ok
int rb_led=21;    //step3 system ok

int rb_state=LOW;
//for sonar sensor
const int trigPin = 14;
const int echoPin = 12;
int a=20;    //4       //triggering distances
int b=15;    //3
int c=10;    //2
int d=5;    //1
int max_distance = 200;

//for gas sensors
int mq_02 = 4;       //pins for mq sensor analog input 
int mq_07 = 5;
int mq_135 = 34;

//Telegram Status 
int NewMsgLed=32;
int ConnectionLed=2;

//Buzzer
int Buzzer = 33;


float mqMethane, mqCarbon,mqAmmonia;
  
String GasReadings(){
  String message = "Methane: " + String(mqMethane) + "\n";
  message += "Carbon: " + String (mqCarbon) + "\n";
  message += "Ammonia/Quality " + String (mqAmmonia) + "\n";
  return message;
}


float DistInches,DistCm;
String levelfl;
  
String WaterReadings(){
  String message = "Distance=  \n" ;
  message += String (DistInches) + " inch / " + String (DistCm) + " cm\n";
  message += " Level= " + levelfl + "\n";
  return message;
}

void alarm(){
  digitalWrite(Buzzer,HIGH);delay(500);digitalWrite(Buzzer,LOW);delay(500);
  alarm();
  
}


//Telegram Config *********************************************
//Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(70);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "This is the Remote Control Panel of\n\n";
      welcome += "IOT Project Gas&water level Sensor \n\n";
      welcome += "made by Aashutosh Soni \n\n";
      welcome += "************************* \n\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/fan_on to turn the Fan ON \n";
      welcome += "/fan_off to turn the Fan OFF \n";
      welcome += "/state to request current Fan state \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/fan_on") {
      bot.sendMessage(chat_id, "Fan state set to ON", "");
      rb_state = HIGH;
      digitalWrite(rb_led, rb_state);
    }
    
    if (text == "/fan_off") {
      bot.sendMessage(chat_id, "Fan state set to OFF", "");
      rb_state = LOW;
      digitalWrite(rb_led, rb_state);
    }
    
    if (text == "/state") {
      if (digitalRead(rb_led)){
        bot.sendMessage(chat_id, "Fan is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "Fan is OFF", "");
      }
    }
  }
}
//*********************************************************************

void setup ()
{
  Serial.begin(115200);         //serial output for debugging


    //Output Led
    pinMode (green_led,OUTPUT);
    pinMode (rb_led,OUTPUT);
    pinMode (red_led,OUTPUT);
    pinMode(18, OUTPUT);    //led pins
    pinMode(19, OUTPUT);
    pinMode(21, OUTPUT);

    //Telegram led 
    pinMode(NewMsgLed,OUTPUT);
    pinMode(ConnectionLed,OUTPUT);
    pinMode(32,OUTPUT);
    pinMode(2,OUTPUT);

    //Buzzer
    pinMode(Buzzer,OUTPUT);
    pinMode(33,OUTPUT);

    // //Buzzer
    // pinMode(fanPin,OUTPUT);
    // pinMode(15,OUTPUT);
  
  pinMode(mq_02, INPUT);
  pinMode(mq_07, INPUT);
  pinMode(mq_135, INPUT);

  // pinMode(fanPin, OUTPUT);
  // digitalWrite(fanPin, fanState);

    // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(Buzzer,HIGH);digitalWrite(ConnectionLed,HIGH);
    delay(250);
    digitalWrite(Buzzer,LOW);digitalWrite(ConnectionLed,LOW);
    delay(250);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  //send that bot has started.
  digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(70);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);
  bot.sendMessage(CHAT_ID, "Bot started up,\n\nPrototype Up and Running", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(ConnectionLed,HIGH);
    delay(100);
    digitalWrite(Buzzer,LOW);digitalWrite(ConnectionLed,LOW);
    delay(100);
  delay(2000);
  Serial.print("Gas sensor Heating up!"); 
  digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(70);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);          
  bot.sendMessage(CHAT_ID, "Gas sensor Heating up!", "");    digitalWrite(Buzzer,HIGH);delay(300);digitalWrite(Buzzer,LOW);      delay(5000);    //mq sensor heating time
  Serial.println (" ");
  digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(70);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);
  Serial.println("Done!;Ready to go");              
  bot.sendMessage(CHAT_ID, "Done!;Ready to go", "");        digitalWrite(Buzzer,HIGH);delay(200);digitalWrite(Buzzer,LOW);delay(200);digitalWrite(Buzzer,HIGH);delay(200);digitalWrite(Buzzer,LOW);    delay(2000);
    digitalWrite(Buzzer,HIGH);delay(100); digitalWrite(ConnectionLed,HIGH); delay(100); digitalWrite(Buzzer,LOW);


    digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(30);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);
    Serial.println("Sending Data to the IOT BOT");      
            
  bot.sendMessage(CHAT_ID, "Now showing Data from Device-", "");  
}


void loop() { 

if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  
  //for sensor
    //staring loop to continously update sensor value
    int mq_02_value = analogRead(mq_02);
    //int mq_05_value = analogRead(mq_05);
    int mq_07_value = analogRead(mq_07);
    int mq_135_value = analogRead(mq_135);


  mqMethane=mq_02_value;
  mqCarbon=mq_07_value;
  mqAmmonia=mq_135_value;
  
long duration, inches, cm;
pinMode(trigPin,OUTPUT);
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
pinMode(echoPin, INPUT);
duration = pulseIn(echoPin, HIGH);


//Serial.println(duration);
inches = (duration / 74) / 2;
cm = (duration / 29) / 2;

DistInches=inches;
DistCm=cm;

if (inches <max_distance){
Serial.print("Distance= ");
Serial.print(inches);
Serial.print(" inch");
Serial.print(" / ");
Serial.print(cm);
Serial.print(" cm");
Serial.println();
Serial.print("Level= ");

String Waterreadings = WaterReadings();
      bot.sendMessage(CHAT_ID, Waterreadings, "");

      digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(30);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);

}

if (inches >=a) { levelfl = " Safe Level ";
digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(30);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);
  Serial.print(" Safe Level "); digitalWrite (green_led,HIGH); digitalWrite (red_led,LOW);   Serial.println();
  Serial.print ("Methane=");  Serial.print (" ");   Serial.print (mq_02_value);  Serial.print ("\t");
  Serial.print ("Carbon=");  Serial.print (" ");   Serial.print (mq_07_value);    Serial.print ("\t");
  Serial.print ("Ammonia/Quality=");  Serial.print (" ");   Serial.print (mq_135_value);    Serial.println ("\t");
  if (mq_135_value>1000){
    bot.sendMessage(CHAT_ID, "!Warning! -> Fire Detected", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("!Warning!"); Serial.println ("Fire Detected"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH); }
  else if (mq_02_value>1500) { 
    bot.sendMessage(CHAT_ID, "!Warning! -> High Methane Level", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("High Methane Level"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH); }
  else if (mq_07_value>1000){
    bot.sendMessage(CHAT_ID, "!Warning! -> Smoke Detected", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("!Warning!"); Serial.println ("Smoke Detected"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH);}

      String Gasreadings = GasReadings();
      bot.sendMessage(CHAT_ID, Gasreadings, "");


}  
else if (inches >=b){  levelfl = " Level Rising ";
digitalWrite(Buzzer,HIGH);delay(500);digitalWrite(Buzzer,LOW);
digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(30);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);
  Serial.print(" Level Rising"); digitalWrite (green_led,HIGH); delay(500); digitalWrite (green_led,LOW); delay(500);   Serial.println();
  Serial.print ("Methane=");  Serial.print (" ");   Serial.print (mq_02_value);  Serial.print ("\t"); 
  Serial.print ("Carbon=");  Serial.print (" ");   Serial.print (mq_07_value);    Serial.print ("\t");
  Serial.print ("Ammonia/Quality=");  Serial.print (" ");   Serial.print (mq_135_value);    Serial.println ("\t");
   if (mq_135_value>1000){
    bot.sendMessage(CHAT_ID, "!Warning! -> Fire Detected", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("!Warning!"); Serial.println ("Fire Detected"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH);}
  else if (mq_02_value>1500) {
    bot.sendMessage(CHAT_ID, "!Warning! -> High Methane Level", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("High Methane Level"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH);}
  else if (mq_07_value>1000){
    bot.sendMessage(CHAT_ID, "!Warning! -> Smoke Detected", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("!Warning!"); Serial.println ("Smoke Detected"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH);}
      
        String Gasreadings = GasReadings();
      bot.sendMessage(CHAT_ID, Gasreadings, "");
 
 } 

else if (inches >=c){ levelfl = " !Warning High Water Levels! ";
digitalWrite(Buzzer,HIGH);delay(500);digitalWrite(Buzzer,LOW);
digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(30);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);
  Serial.print(" !Warning High Water Levels! "); digitalWrite (red_led,HIGH); delay(500); digitalWrite (red_led,LOW); delay(500);   Serial.println();
  Serial.print ("Methane=");  Serial.print (" ");   Serial.print (mq_02_value);  Serial.print ("\t"); 
  Serial.print ("Carbon=");  Serial.print (" ");   Serial.print (mq_07_value);    Serial.print ("\t");
  Serial.print ("Ammonia/Quality=");  Serial.print (" ");   Serial.print (mq_135_value);    Serial.println ("\t");
 if (mq_135_value>1000){
  bot.sendMessage(CHAT_ID, "!Warning! -> Fire Detected", "");
  digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
  Serial.println (" "); Serial.println ("!Warning!"); Serial.println ("Fire Detected"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH);}
  else if (mq_02_value>1500) { 
    bot.sendMessage(CHAT_ID, "!Warning! -> High Methane Level", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("High Methane Level"); Serial.println ("!Warning!"); Serial.println (" "); delay(2000); digitalWrite (rb_led,HIGH);}
  else if (mq_07_value>1000){
    bot.sendMessage(CHAT_ID, "!Warning! -> Smoke Detected", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("!Warning!"); Serial.println ("Smoke Detected"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH); }

        String Gasreadings = GasReadings();
      bot.sendMessage(CHAT_ID, Gasreadings, "");
      
  } 
else if (inches >=d){ levelfl = " !!!OverFlow!!! ";
//digitalWrite(NewMsgLed,HIGH);digitalWrite(Buzzer,HIGH);delay(30);digitalWrite(Buzzer,LOW);digitalWrite(NewMsgLed,LOW);
  Serial.print(" !!!OverFlow!!!"); digitalWrite (green_led,LOW); digitalWrite (red_led,HIGH);digitalWrite(Buzzer,HIGH);   Serial.println();
  Serial.print ("Methane=");  Serial.print (" ");   Serial.print (mq_02_value);  Serial.print ("\t"); 
  Serial.print ("Carbon=");  Serial.print (" ");   Serial.print (mq_07_value);    Serial.print ("\t");
  Serial.print ("Ammonia/Quality=");  Serial.print (" ");   Serial.print (mq_135_value);    Serial.println ("\t");

    if (mq_135_value>1000){
      bot.sendMessage(CHAT_ID, "!Warning! -> Fire Detected", "");
      digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
      Serial.println (" "); Serial.println ("!Warning!"); Serial.println ("Fire Detected"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH);}
  else if (mq_02_value>1500) {
    bot.sendMessage(CHAT_ID, "!Warning! -> High Methane Level", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("High Methane Level"); Serial.println ("!Warning!"); Serial.println (" "); delay(2000); digitalWrite (rb_led,HIGH);}
  else if (mq_07_value>1000){
    bot.sendMessage(CHAT_ID, "!Warning! -> Smoke Detected", "");
    digitalWrite(Buzzer,HIGH);digitalWrite(rb_led,HIGH);
    Serial.println (" "); Serial.println ("!Warning!"); Serial.println ("Smoke Detected"); Serial.println ("!Warning!"); Serial.println (" "); delay(1000); digitalWrite (rb_led,HIGH); }
     
alarm(); digitalWrite (rb_led,LOW);
        String Gasreadings = GasReadings();
      bot.sendMessage(CHAT_ID, Gasreadings, "");
      
  
  } 
delay(1000);
}
