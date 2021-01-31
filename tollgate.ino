#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DateTime.h>

#include <ArduinoJson.h>
#include <Servo.h>

Servo servo;
constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above
constexpr uint8_t led = D2;
constexpr uint8_t buzz = D8; 
const char* ssid = "akhil";
const char* pass = "9154327896";


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
String tag;
float amount=0;

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  pinMode(led,OUTPUT);
  pinMode(buzz,OUTPUT);
  servo.attach(16); 
  servo.write(0);

  delay(10);
               
       Serial.println("Connecting to ");
       Serial.println(ssid); 
 
       WiFi.begin(ssid, pass); 
       while (WiFi.status() != WL_CONNECTED) 
          {
            delay(500);
            Serial.print(".");
          }
      Serial.println("");
      Serial.println("WiFi connected");
}
void loop() {
   
    
    if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    digitalWrite(led, HIGH);
    Serial.print("enter toll fee");
    while(Serial.available()==0){}
      amount=Serial.parseFloat();
    
    Serial.println(amount);
    Serial.println(tag);
    
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    //Serial.print("enter the amount of fuel needed: ");
    
    DynamicJsonDocument doc(2048);
    doc["type"] = "toll";
    doc["to"]= "National toll gate ";
    doc["address"]="National toll gate, Vijayawada";
    doc["userid"]="0x62697b036fb68B61e15746eCf8950A823a1849F4";
    doc["timestamp"]="";
    doc["amount"]=amount;
    doc["status"]="unpaid";
    doc["transactionhash"]="";
   
    String json;
    serializeJson(doc, json);
    HTTPClient http;    
    String url="http://autopayrest.herokuapp.com/transaction/"+tag;
    Serial.print(url);
    http.begin(url);  
    http.setTimeout(30000);//Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
   
    Serial.println(json);
    
      int httpCode = http.POST(json);   
    String payload = http.getString();    
     
                  
 
    Serial.println(httpCode);  
      
    Serial.print(payload);
   http.end();  //Close connection
   if(payload=="success"){
    for(int i=0;i<3;i++){
      digitalWrite(buzz,HIGH);
      delay(500);
     digitalWrite(buzz,LOW);
     delay(500);
      }
      servo.write(90);

    delay(5000);

    servo.write(0);

    //delay(10000);
    
   }
  } else {
  Serial.println("Error in WiFi connection");
  }
    delay(2000);
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    
    digitalWrite(led,LOW);
  } 
}
