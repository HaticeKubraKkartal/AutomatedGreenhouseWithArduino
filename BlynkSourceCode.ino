#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <dht11.h>
char auth[] = "eef0ea91dd2a432c881cbff*********";
char ssid[] = "kafamagore";
char pass[] = "password";
#define EspSerial Serial
#define DHT11PIN 7
dht11 DHT11;
int roleLed=3;
int roleFan=2;

int deger=0;
int butondurum = 0;
int fanDeger=0;
int ledDeger=0;

int motorileriSol = 10;     // Motor sürücümüze bağladığımız pinleri tanımlıyoruz IN1
int motorgeriSol = 11;   //IN2
int motorhizSol = 5;    //ENA
int motorhizSag = 6;    //ENB
int motorileriSag =  8;  //IN3
int motorgeriSag =  9; //IN4
int yagmur_sensor=12;  //digital pin 12

#define DHTTYPE DHT11
//#include <SoftwareSerial.h>
//SoftwareSerial EspSerial(2, 3); // RX, TX
#define ESP8266_BAUD 115200
ESP8266 wifi(&EspSerial);
BlynkTimer timer;

void sicaklik()
{
  int chk=DHT11.read(DHT11PIN);
  float h = DHT11.humidity;
  float t = DHT11.temperature; // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t); 
}
void su_seviye(){
  int su_seviye=analogRead(A4);
  Serial.print("Depo Su Seviyesi:");
  Serial.println(su_seviye);
  
  Blynk.virtualWrite(V9, su_seviye);
}
void toprak_nem_olcum(){
  int toprak_nem = analogRead(A0);
  Serial.print("Toprak Nem İhtiyacı:");
  Serial.println(toprak_nem);
  
  Blynk.virtualWrite(V7, toprak_nem);
}
void yagmur_olcum(){
   float yagmur=analogRead(A3);
   Serial.print("Yağış Miktarı");
   Serial.println(yagmur);
   
   Blynk.virtualWrite(V8, yagmur);
}
BLYNK_WRITE(V2) // Led kontrol
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V2 Slider value is: ");
  Serial.println(pinValue);
  if(pinValue==0){
    digitalWrite(roleLed, LOW);
  }
  else{
      digitalWrite(roleLed, HIGH);
  } 
  ledDeger=pinValue; 
}

BLYNK_WRITE(V1) // fan kontrol
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V1 Slider value is: ");
  Serial.println(pinValue);
  
  fanDeger=pinValue;  
}

BLYNK_WRITE(V3) // Motor kontrol
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V3 Slider value is: ");
  Serial.println(pinValue);
  deger=pinValue;  
}
void setup()
{
  Serial.begin(9600);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  
  Blynk.begin(auth, wifi, ssid, pass);
  
  pinMode(yagmur_sensor,INPUT);

  pinMode(roleLed, OUTPUT);
  pinMode(roleFan, OUTPUT);

  pinMode(motorhizSol , OUTPUT); 
  pinMode(motorileriSol , OUTPUT); 
  pinMode(motorgeriSol , OUTPUT); 
  pinMode(motorhizSag , OUTPUT); 
  pinMode(motorileriSag , OUTPUT); 
  pinMode(motorgeriSag , OUTPUT); 

  timer.setInterval(1000L, sicaklik);
  timer.setInterval(1000L, toprak_nem_olcum);
  timer.setInterval(1000L, yagmur_olcum);
  timer.setInterval(1000L, su_seviye);
 // Blynk.begin(auth, wifi, ssid, pass, "188.166.206.43",8442);
  //Blynk.begin(auth, wifi, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, wifi, ssid, pass, "192.168.1.100", 8442);
}
void loop()
{
  Blynk.run();
  timer.run();

  butondurum = analogRead(A5); //butonun durumunu butondurum değişkenine atıyoruz

  cati(deger,butondurum);
  fanKontrol(ledDeger, butondurum);
  delay(10);
}
void fanKontrol(int ledDeger ,int butondurum){
  if(butondurum>=970){ // 1 durumu
    Serial.println ("Sistem Otomatik Durumda");
     fan_kontrol_otomatik();
     delay(10);
  }
  else{       // 0 durumu
    fan_kontrol_manuel();
    Serial.println ("Sistem Manuel Durumda Uzaktan Kontrol Edilebilir.");
  }
}
void fan_kontrol_otomatik(){

  int chk=DHT11.read(DHT11PIN);
  float h = DHT11.humidity;
  float t = DHT11.temperature; // or dht.readTemperature(true) for Fahrenheit

  if(h>=20 || t>=30){
    digitalWrite(roleFan, HIGH);
  }
  else{
     digitalWrite(roleFan, LOW);
  }  
}
void fan_kontrol_manuel(){
  if(fanDeger==0){
    digitalWrite(roleFan, LOW);
  }
  else{
      digitalWrite(roleFan, HIGH);
  }
}

void cati_kontrol_otomatik(){
   bool yagmur=digitalRead(yagmur_sensor);
   int kapandi = 0;
   int acildi = 0;

  kapandi = analogRead(A1); //kapandığını kontrol eden switch
  acildi = analogRead(A2);  //açıldığını kontrol eden switch
  
  if(yagmur==false){  //Yagmur Yagıyor ise
    Serial.print ("Yagmur var=");
    Serial.println ("Çatı Kapanacak");
    int kapandi = 0; 
    kapandi = analogRead(A1);
 
    while(kapandi<900) {
       analogWrite(motorhizSol, 255);
       digitalWrite(motorileriSol, HIGH);
       digitalWrite(motorgeriSol,  LOW);
       
       analogWrite(motorhizSag, 200);
       digitalWrite(motorileriSag, HIGH);
       digitalWrite(motorgeriSag,  LOW);
       
       kapandi = analogRead(A1);
       acildi = analogRead(A2);

      
    }
    if(kapandi>=900){
       analogWrite(motorhizSol, 0);
       digitalWrite(motorileriSol, LOW);
       digitalWrite(motorgeriSol,  LOW);
       
       analogWrite(motorhizSag, 0);
       digitalWrite(motorileriSag, LOW);
       digitalWrite(motorgeriSag,  LOW);
     //---------------------------------------------
       Serial.print ("Yagmur Yagıyor");
       Serial.println ("Çatı Kapandı");

      Serial.println (kapandi);
    }
  }
  else{ //Yagmur Yagmuyor ise
    Serial.print ("Yagmur Yok=");
    Serial.println ("Çatı Açılacak");
    int acildi = 0;
    acildi = analogRead(A2);
   
    while(acildi <900) {
       Serial.println (acildi);
       analogWrite(motorhizSol, 255);
       digitalWrite(motorileriSol, LOW);
       digitalWrite(motorgeriSol,  HIGH);
  
       analogWrite(motorhizSag, 200);
       digitalWrite(motorileriSag, LOW);
       digitalWrite(motorgeriSag,  HIGH);
       
       kapandi = analogRead(A1);
       acildi = analogRead(A2);
    }
    if(acildi>=900){
       analogWrite(motorhizSol, 0);
       digitalWrite(motorileriSol, LOW);
       digitalWrite(motorgeriSol,  LOW);
       
       analogWrite(motorhizSag, 0);
       digitalWrite(motorileriSag, LOW);
       digitalWrite(motorgeriSag,  LOW);
  //---------------------------------------------
       Serial.print ("Yagmur Yok");
       Serial.println ("Çatı Açıldı");

      Serial.println (acildi);
    }
  }
}
void cati(int deger,int butondurum){
  
  if (butondurum >=970) { // Açık (1) durumu
    Serial.println ("Sistem Otomatik Durumda");
     cati_kontrol_otomatik();
     delay(10);
  }
  else{                   // Kapalı (0) durumu
     cati_control(deger);
     Serial.println ("Sistem Manuel Durumda Uzaktan Kontrol Edilebilir");
     delay(10); 
  }
}
void cati_control(int deger){

   bool yagmur=digitalRead(yagmur_sensor);
  
  if(deger==0){    
      Serial.println ("MOTOR Duruyooooooo");
    
       analogWrite(motorhizSag, 0);
       digitalWrite(motorileriSag, LOW);
       digitalWrite(motorgeriSag,  LOW);
       //---
       analogWrite(motorhizSol, 0);
       digitalWrite(motorileriSol, LOW);
       digitalWrite(motorgeriSol,  LOW);  
  }  
  
  if(deger==1){

    if(yagmur==false){
    

    int kapandi = 0; 
    kapandi = analogRead(A1);
 
    while(kapandi<900) {
             
       analogWrite(motorhizSag, 255);
       digitalWrite(motorileriSag, HIGH);
       digitalWrite(motorgeriSag,  LOW);
       //------
       analogWrite(motorhizSol, 255);
       digitalWrite(motorileriSol, HIGH);
       digitalWrite(motorgeriSol,  LOW);

       Serial.println ("MOTOR calisiyor");
       
       kapandi = analogRead(A1);
       //acildi = analogRead(A2);       
      
       if(kapandi>=900){
       
         analogWrite(motorhizSag, 0);
         digitalWrite(motorileriSag, LOW);
         digitalWrite(motorgeriSag,  LOW);
       //---------------------------------------------
         Serial.print ("Yagmur Yagıyor - ");
         Serial.println ("Çatı Kapandı");
        }
       deger=0;
       if(deger==0)
        return;
    }
  }
  if(yagmur==true){
    int acildi = 0; 
    acildi = analogRead(A2);
 
    while(acildi<900) {
             
       analogWrite(motorhizSag, 255);
       digitalWrite(motorileriSag, HIGH);
       digitalWrite(motorgeriSag,  LOW);
       //------
       analogWrite(motorhizSol, 255);
       digitalWrite(motorileriSol, HIGH);
       digitalWrite(motorgeriSol,  LOW);

       Serial.println ("MOTOR calisiyor");
       
       acildi = analogRead(A2);
       //acildi = analogRead(A2);       
      
       if(acildi>=900){
       
         analogWrite(motorhizSag, 0);
         digitalWrite(motorileriSag, LOW);
         digitalWrite(motorgeriSag,  LOW);
       //---------------------------------------------
         Serial.print ("Yagmur Yagıyor - ");
         Serial.println ("Çatı Kapandı");
        }
       deger=0;
       if(deger==0)
        return;
    }
  }
 } 
}
