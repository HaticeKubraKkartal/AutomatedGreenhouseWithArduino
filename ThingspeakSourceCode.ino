#define ag_ismi "kafamagore"
#define ag_sifresi "eiqu4609"
#define IP "184.106.153.149"    //thingspeak.com IP adresi
#include <dht11.h>
#define DHT11PIN 7
dht11 DHT11;
int su_seviye;
int yagmur;
int toprak;

void setup()
{
  Serial.begin(115200); //Seriport'u açıyoruz. Güncellediğimiz 
                        //ESP modülünün baudRate değeri 115200 olduğu için bizde Seriport'u 115200 şeklinde seçiyoruz
  Serial.println("AT"); //ESP modülümüz ile bağlantı kurulup kurulmadığını kontrol ediyoruz.
  
  delay(3000); //ESP ile iletişim için 3 saniye bekliyoruz.
 
  if(Serial.find("OK")){         //esp modülü ile bağlantıyı kurabilmişsek modül "AT" komutuna "OK" komutu ile geri dönüş yapıyor.
     Serial.println("AT+CWMODE=1"); //esp modülümüzün WiFi modunu STA şekline getiriyoruz. Bu mod ile modülümüz başka ağlara bağlanabilecek.
     delay(2000);
     String baglantiKomutu=String("AT+CWJAP=\"")+ag_ismi+"\",\""+ag_sifresi+"\"";
    Serial.println(baglantiKomutu);
    
     delay(5000);
 }
}

void loop(){

   int su_seviye=analogRead(A4);
   Serial.print("Depodaki Su Seviyesi : ");
   Serial.println(su_seviye);
   int yagmur=analogRead(A3);
   Serial.print("Yağış Durumu : ");
   Serial.println(yagmur);
   int toprak=analogRead(A0);
   Serial.print("Toprak Kuruluğu : ");
   Serial.println(toprak);

   Serial.println(su_seviye);
    su_seviyesi_yolla(su_seviye);
    Serial.println(yagmur);
    Serial.println(toprak);
   //float su_seviye = 9.31;  

    int chk = DHT11.read(DHT11PIN);
   Serial.print("Nem (%): ");
   Serial.println((float)DHT11.humidity, 2);//virgülden sonra 2 basamak yazdırmak için..

   Serial.print("Sicaklik (Celcius): ");
   Serial.println((float)DHT11.temperature, 2);
   delay(500);
   int nem = DHT11.humidity;
   int sicaklik=  DHT11.temperature;
   Serial.println("\n");
    
   sicaklik_olcum(sicaklik);        //Field1 
   nem_olcum(nem);                  //Field2
   toprak_nem_olcum(toprak);       //Field3
   su_seviyesi_yolla(su_seviye);   //Field4
   yagmur_yolla(yagmur);           //Field5*/ 
}
void su_seviyesi_yolla(float su_seviye){
 Serial.println(String("AT+CIPSTART=\"TCP\",\"") + IP + "\",80");  //thingspeak sunucusuna baÄŸlanmak iÃ§in bu kodu kullanÄ±yoruz. 
                                                                   //AT+CIPSTART komutu ile sunucuya baÄŸlanmak iÃ§in sunucudan izin istiyoruz. 
                                                                   //TCP burada yapacaÄŸÄ±mÄ±z baÄŸlantÄ± Ã§eÅŸidini gÃ¶steriyor. 80 ise baÄŸlanacaÄŸÄ±mÄ±z portu gÃ¶steriyor
 delay(1000);
  if(Serial.find("Error")){     //sunucuya baÄŸlanamazsak ESP modÃ¼lÃ¼ bize "Error" komutu ile dÃ¶nÃ¼yor.
   Serial.println("AT+CIPSTART Error");
    return;
  }
  
 String yollanacakkomut = "GET /update?key=Q8HH2INBTCUOCKDI&field4=";   // Burada 64T0OS3R1OEAYUML yazan kÄ±sÄ±m bizim API Key den aldÄ±ÄŸÄ±mÄ±z Key. Siz buraya kendi keyinizi yazacaksÄ±nÄ±z.
 yollanacakkomut += (int(su_seviye));                                      // Burada ise sÄ±caklÄ±ÄŸÄ±mÄ±zÄ± float deÄŸiÅŸkenine atayarak yollanacakkomut deÄŸiÅŸkenine ekliyoruz.
 yollanacakkomut += "\r\n\r\n";                                             // ESP modÃ¼lÃ¼mÃ¼z ile seri iletiÅŸim kurarken yazdÄ±ÄŸÄ±mÄ±z komutlarÄ±n modÃ¼le iletilebilmesi iÃ§in Enter komutu yani
  delay(3000);                                                                                // /r/n komutu kullanmamÄ±z gerekiyor.
 
 Serial.print("AT+CIPSEND=");                    //veri yollayacaÄŸÄ±mÄ±z zaman bu komutu kullanÄ±yoruz. Bu komut ile Ã¶nce kaÃ§ tane karakter yollayacaÄŸÄ±mÄ±zÄ± sÃ¶ylememiz gerekiyor.
 Serial.println(yollanacakkomut.length()+2);       //yollanacakkomut deÄŸiÅŸkeninin kaÃ§ karakterden oluÅŸtuÄŸunu .length() ile bulup yazÄ±rÄ±yoruz.
 
 delay(1000);
 
 if(Serial.find(">")){                           //eÄŸer sunucu ile iletiÅŸim saÄŸlayÄ±p komut uzunluÄŸunu gÃ¶nderebilmiÅŸsek ESP modÃ¼lÃ¼ bize ">" iÅŸareti ile geri dÃ¶nÃ¼yor.
                                                 // arduino da ">" iÅŸaretini gÃ¶rdÃ¼ÄŸÃ¼ anda sÄ±caklÄ±k verisini esp modÃ¼lÃ¼ ile thingspeak sunucusuna yolluyor.
 Serial.print(yollanacakkomut);
 Serial.print("\r\n");
 }
 else{
 Serial.println("AT+CIPCLOSE");
 }

  delay(2000);

}
void yagmur_yolla(int yagmur){
 Serial.println(String("AT+CIPSTART=\"TCP\",\"") + IP + "\",80");  //thingspeak sunucusuna baÄŸlanmak iÃ§in bu kodu kullanÄ±yoruz. 
                                                                   //AT+CIPSTART komutu ile sunucuya baÄŸlanmak iÃ§in sunucudan izin istiyoruz. 
                                                                   //TCP burada yapacaÄŸÄ±mÄ±z baÄŸlantÄ± Ã§eÅŸidini gÃ¶steriyor. 80 ise baÄŸlanacaÄŸÄ±mÄ±z portu gÃ¶steriyor
 delay(1000);
  if(Serial.find("Error")){     //sunucuya baÄŸlanamazsak ESP modÃ¼lÃ¼ bize "Error" komutu ile dÃ¶nÃ¼yor.
   Serial.println("AT+CIPSTART Error");
    return;
  }
 String yollanacakkomut = "GET /update?key=Q8HH2INBTCUOCKDI&field5=";   // Burada 64T0OS3R1OEAYUML yazan kÄ±sÄ±m bizim API Key den aldÄ±ÄŸÄ±mÄ±z Key. Siz buraya kendi keyinizi yazacaksÄ±nÄ±z.
 yollanacakkomut += (int(yagmur));                                      // Burada ise sÄ±caklÄ±ÄŸÄ±mÄ±zÄ± float deÄŸiÅŸkenine atayarak yollanacakkomut deÄŸiÅŸkenine ekliyoruz.
 yollanacakkomut += "\r\n\r\n";                                             // ESP modÃ¼lÃ¼mÃ¼z ile seri iletiÅŸim kurarken yazdÄ±ÄŸÄ±mÄ±z komutlarÄ±n modÃ¼le iletilebilmesi iÃ§in Enter komutu yani
  delay(3000);                                                                                // /r/n komutu kullanmamÄ±z gerekiyor.
 
 Serial.print("AT+CIPSEND=");                    //veri yollayacaÄŸÄ±mÄ±z zaman bu komutu kullanÄ±yoruz. Bu komut ile Ã¶nce kaÃ§ tane karakter yollayacaÄŸÄ±mÄ±zÄ± sÃ¶ylememiz gerekiyor.
 Serial.println(yollanacakkomut.length()+2);       //yollanacakkomut deÄŸiÅŸkeninin kaÃ§ karakterden oluÅŸtuÄŸunu .length() ile bulup yazÄ±rÄ±yoruz.
 
 delay(1000);
 
 if(Serial.find(">")){                           //eÄŸer sunucu ile iletiÅŸim saÄŸlayÄ±p komut uzunluÄŸunu gÃ¶nderebilmiÅŸsek ESP modÃ¼lÃ¼ bize ">" iÅŸareti ile geri dÃ¶nÃ¼yor.
                                                 // arduino da ">" iÅŸaretini gÃ¶rdÃ¼ÄŸÃ¼ anda sÄ±caklÄ±k verisini esp modÃ¼lÃ¼ ile thingspeak sunucusuna yolluyor.
 Serial.print(yollanacakkomut);
 Serial.print("\r\n");
 }
 else{
 Serial.println("AT+CIPCLOSE");
 }
  delay(2000); 
}
void toprak_nem_olcum(int toprak){
 Serial.println(String("AT+CIPSTART=\"TCP\",\"") + IP + "\",80");  //thingspeak sunucusuna baÄŸlanmak iÃ§in bu kodu kullanÄ±yoruz. 
                                                                   //AT+CIPSTART komutu ile sunucuya baÄŸlanmak iÃ§in sunucudan izin istiyoruz. 
                                                                   //TCP burada yapacaÄŸÄ±mÄ±z baÄŸlantÄ± Ã§eÅŸidini gÃ¶steriyor. 80 ise baÄŸlanacaÄŸÄ±mÄ±z portu gÃ¶steriyor
 delay(1000);
  if(Serial.find("Error")){     //sunucuya baÄŸlanamazsak ESP modÃ¼lÃ¼ bize "Error" komutu ile dÃ¶nÃ¼yor.
   Serial.println("AT+CIPSTART Error");
    return;
  }
  
 String yollanacakkomut = "GET /update?key=Q8HH2INBTCUOCKDI&field3=";   // Burada 64T0OS3R1OEAYUML yazan kÄ±sÄ±m bizim API Key den aldÄ±ÄŸÄ±mÄ±z Key. Siz buraya kendi keyinizi yazacaksÄ±nÄ±z.
 yollanacakkomut += (int(toprak));                                      // Burada ise sÄ±caklÄ±ÄŸÄ±mÄ±zÄ± float deÄŸiÅŸkenine atayarak yollanacakkomut deÄŸiÅŸkenine ekliyoruz.
 yollanacakkomut += "\r\n\r\n";                                             // ESP modÃ¼lÃ¼mÃ¼z ile seri iletiÅŸim kurarken yazdÄ±ÄŸÄ±mÄ±z komutlarÄ±n modÃ¼le iletilebilmesi iÃ§in Enter komutu yani
  delay(3000);                                                                                // /r/n komutu kullanmamÄ±z gerekiyor.
 
 Serial.print("AT+CIPSEND=");                    //veri yollayacaÄŸÄ±mÄ±z zaman bu komutu kullanÄ±yoruz. Bu komut ile Ã¶nce kaÃ§ tane karakter yollayacaÄŸÄ±mÄ±zÄ± sÃ¶ylememiz gerekiyor.
 Serial.println(yollanacakkomut.length()+2);       //yollanacakkomut deÄŸiÅŸkeninin kaÃ§ karakterden oluÅŸtuÄŸunu .length() ile bulup yazÄ±rÄ±yoruz.
 
 delay(1000);
 
 if(Serial.find(">")){                           //eÄŸer sunucu ile iletiÅŸim saÄŸlayÄ±p komut uzunluÄŸunu gÃ¶nderebilmiÅŸsek ESP modÃ¼lÃ¼ bize ">" iÅŸareti ile geri dÃ¶nÃ¼yor.
                                                 // arduino da ">" iÅŸaretini gÃ¶rdÃ¼ÄŸÃ¼ anda sÄ±caklÄ±k verisini esp modÃ¼lÃ¼ ile thingspeak sunucusuna yolluyor.
 Serial.print(yollanacakkomut);
 Serial.print("\r\n");
 }
 else{
 Serial.println("AT+CIPCLOSE");
 }

  delay(2000);
 
}
void sicaklik_olcum(int sicaklik){
 Serial.println(String("AT+CIPSTART=\"TCP\",\"") + IP + "\",80");  //thingspeak sunucusuna baÄŸlanmak iÃ§in bu kodu kullanÄ±yoruz. 
                                                                   //AT+CIPSTART komutu ile sunucuya baÄŸlanmak iÃ§in sunucudan izin istiyoruz. 
                                                                   //TCP burada yapacaÄŸÄ±mÄ±z baÄŸlantÄ± Ã§eÅŸidini gÃ¶steriyor. 80 ise baÄŸlanacaÄŸÄ±mÄ±z portu gÃ¶steriyor
 delay(1000);
  if(Serial.find("Error")){     //sunucuya baÄŸlanamazsak ESP modÃ¼lÃ¼ bize "Error" komutu ile dÃ¶nÃ¼yor.
   Serial.println("AT+CIPSTART Error");
    return;
  }
  
 String yollanacakkomut = "GET /update?key=Q8HH2INBTCUOCKDI&field2=";   // Burada 64T0OS3R1OEAYUML yazan kÄ±sÄ±m bizim API Key den aldÄ±ÄŸÄ±mÄ±z Key. Siz buraya kendi keyinizi yazacaksÄ±nÄ±z.
 yollanacakkomut += (int(sicaklik));                                      // Burada ise sÄ±caklÄ±ÄŸÄ±mÄ±zÄ± float deÄŸiÅŸkenine atayarak yollanacakkomut deÄŸiÅŸkenine ekliyoruz.
 yollanacakkomut += "\r\n\r\n";                                             // ESP modÃ¼lÃ¼mÃ¼z ile seri iletiÅŸim kurarken yazdÄ±ÄŸÄ±mÄ±z komutlarÄ±n modÃ¼le iletilebilmesi iÃ§in Enter komutu yani
  delay(3000);                                                                                // /r/n komutu kullanmamÄ±z gerekiyor.
 
 Serial.print("AT+CIPSEND=");                    //veri yollayacaÄŸÄ±mÄ±z zaman bu komutu kullanÄ±yoruz. Bu komut ile Ã¶nce kaÃ§ tane karakter yollayacaÄŸÄ±mÄ±zÄ± sÃ¶ylememiz gerekiyor.
 Serial.println(yollanacakkomut.length()+2);       //yollanacakkomut deÄŸiÅŸkeninin kaÃ§ karakterden oluÅŸtuÄŸunu .length() ile bulup yazÄ±rÄ±yoruz.
 
 delay(1000);
 
 if(Serial.find(">")){                           //eÄŸer sunucu ile iletiÅŸim saÄŸlayÄ±p komut uzunluÄŸunu gÃ¶nderebilmiÅŸsek ESP modÃ¼lÃ¼ bize ">" iÅŸareti ile geri dÃ¶nÃ¼yor.
                                                 // arduino da ">" iÅŸaretini gÃ¶rdÃ¼ÄŸÃ¼ anda sÄ±caklÄ±k verisini esp modÃ¼lÃ¼ ile thingspeak sunucusuna yolluyor.
 Serial.print(yollanacakkomut);
 Serial.print("\r\n");
 }
 else{
 Serial.println("AT+CIPCLOSE");
 }

  delay(2000);
 
}
void nem_olcum(int nem){
 Serial.println(String("AT+CIPSTART=\"TCP\",\"") + IP + "\",80");  //thingspeak sunucusuna baÄŸlanmak iÃ§in bu kodu kullanÄ±yoruz. 
                                                                   //AT+CIPSTART komutu ile sunucuya baÄŸlanmak iÃ§in sunucudan izin istiyoruz. 
                                                                   //TCP burada yapacaÄŸÄ±mÄ±z baÄŸlantÄ± Ã§eÅŸidini gÃ¶steriyor. 80 ise baÄŸlanacaÄŸÄ±mÄ±z portu gÃ¶steriyor
 delay(1000);
  if(Serial.find("Error")){     //sunucuya baÄŸlanamazsak ESP modÃ¼lÃ¼ bize "Error" komutu ile dÃ¶nÃ¼yor.
   Serial.println("AT+CIPSTART Error");
    return;
  }
  
 String yollanacakkomut = "GET /update?key=Q8HH2INBTCUOCKDI&field1=";   // Burada 64T0OS3R1OEAYUML yazan kÄ±sÄ±m bizim API Key den aldÄ±ÄŸÄ±mÄ±z Key. Siz buraya kendi keyinizi yazacaksÄ±nÄ±z.
 yollanacakkomut += (int(nem));                                      // Burada ise sÄ±caklÄ±ÄŸÄ±mÄ±zÄ± float deÄŸiÅŸkenine atayarak yollanacakkomut deÄŸiÅŸkenine ekliyoruz.
 yollanacakkomut += "\r\n\r\n";                                             // ESP modÃ¼lÃ¼mÃ¼z ile seri iletiÅŸim kurarken yazdÄ±ÄŸÄ±mÄ±z komutlarÄ±n modÃ¼le iletilebilmesi iÃ§in Enter komutu yani
  delay(3000);                                                                                // /r/n komutu kullanmamÄ±z gerekiyor.
 
 Serial.print("AT+CIPSEND=");                    //veri yollayacaÄŸÄ±mÄ±z zaman bu komutu kullanÄ±yoruz. Bu komut ile Ã¶nce kaÃ§ tane karakter yollayacaÄŸÄ±mÄ±zÄ± sÃ¶ylememiz gerekiyor.
 Serial.println(yollanacakkomut.length()+2);       //yollanacakkomut deÄŸiÅŸkeninin kaÃ§ karakterden oluÅŸtuÄŸunu .length() ile bulup yazÄ±rÄ±yoruz.
 
 delay(1000);
 
 if(Serial.find(">")){                           //eÄŸer sunucu ile iletiÅŸim saÄŸlayÄ±p komut uzunluÄŸunu gÃ¶nderebilmiÅŸsek ESP modÃ¼lÃ¼ bize ">" iÅŸareti ile geri dÃ¶nÃ¼yor.
                                                 // arduino da ">" iÅŸaretini gÃ¶rdÃ¼ÄŸÃ¼ anda sÄ±caklÄ±k verisini esp modÃ¼lÃ¼ ile thingspeak sunucusuna yolluyor.
 Serial.print(yollanacakkomut);
 Serial.print("\r\n");
 }
 else{
 Serial.println("AT+CIPCLOSE");
 }

  delay(2000);
 
}

