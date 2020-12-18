#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <Ethernet.h>
#include <SPI.h>

#include <ArduinoJson.h>

#define SS_PIN 9
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

byte mac[] = {0xB2, 0xAD, 0xF4, 0xDE, 0xCA, 0xAF};
IPAddress myip(192, 168, 37, 200);
EthernetClient client;

int relayMagnetDoor = 7;
int buzzer = 6;
int proximity = A5;
int inputRelay = 4;
int led_red = 30;
int led_green = 31;

const char *host = "192.168.37.177";
void setup()
{
  Serial.begin(9600);
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  Ethernet.begin(mac, myip);
  
  //lcd.begin(16,2,0x00);
  lcd.begin();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  //lcd.print("Scan ID Card");

  pinMode(relayMagnetDoor, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(proximity, INPUT);
  pinMode(inputRelay, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  
  digitalWrite(led_green, HIGH);
  digitalWrite(led_red, HIGH);
  delay(100);
}

void(* resetFunc)(void) = 0;

int x = 0;
unsigned long int test = 0;
String balik[4] = {"00","00","00","00"};
String raw_content[4] = {"00","00","00","00"};
int switchState = 1;
int doorClosing;

//struct inputAllowed{
//  char nik[8];
//  char allow[8];
//};
//
//bool readResponseContent(struct inputAllowed* inputAllowed){
//  const size_t bufferSize = 200;
//  StaticJsonBuffer<bufferSize> jsonBuffer;
//  JsonObject& root = jsonBuffer.parseObject(client);
//
//  if (!root.success()) {
//    Serial.println("JSON parsing failed!");
//    return false;
//  }
//
//  strcpy(inputAllowed -> nik, root["nik"]);
//  strcpy(inputAllowed -> allow, root["allowed"]);
//
//  return true;
//  
//}
//
//void printClientData(const struct inputAllowed* inputAllowed) {
//  Serial.print("NIK: ");
//  Serial.println(inputAllowed->nik);
//  Serial.print("Allowed: ");
//  Serial.println(inputAllowed->allow);
//}

StaticJsonBuffer<200> jsonBuffer;
const char* nik;
int allow;
int ambil1;
int ambil2;

void loop()
{
  switch(switchState){
    case 1:
      digitalWrite(relayMagnetDoor, LOW);
      //Serial.println("Scan ID Card:");
      //lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Scan ID Card");
      lcd.setCursor(0,1);
      lcd.print("to get Yakult");
      if(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()){
        switchState = 2;
      }
      //Serial.println(digitalRead(proximity));
      delay(100);
      
    case 2:
      // Look for new cards
      if ( ! mfrc522.PICC_IsNewCardPresent()) 
      {
        digitalWrite(led_green, HIGH);
        while(analogRead(proximity) > 800){
          lcd.setCursor(0,0);
          lcd.print("ILLEGAL");
          lcd.setCursor(0,1);
          lcd.print("OPENING!");
          digitalWrite(buzzer, HIGH);
          digitalWrite(led_red,LOW);
          delay(100);
          digitalWrite(buzzer, LOW);
          digitalWrite(led_red, HIGH);
          delay(100);
          //Serial.println(analogRead(proximity));
          lcd.clear();
        }
        digitalWrite(led_red, HIGH);
        return;
      }
      
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial()) 
      {
        return;
      }
      //Serial.println(test);
      
      //Show UID on serial monitor
      String content= "";
      int uid_size = mfrc522.uid.size;
      String readRawContent = "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++) 
      {
         if (mfrc522.uid.uidByte[i] == 0)
         {
            //Serial.println("00");
            raw_content[i] = "00";
            balik[3-i] = "00";
         }
         else if (mfrc522.uid.uidByte[i] < 16){
            raw_content[i] = "0" + String(mfrc522.uid.uidByte[i], HEX);
            balik[3-i] = "0" + String(mfrc522.uid.uidByte[i], HEX);
         }
         else
         {
            //Serial.println(mfrc522.uid.uidByte[i], HEX);
            balik[3-i] = String(mfrc522.uid.uidByte[i], HEX);
            raw_content[i] = String(mfrc522.uid.uidByte[i], HEX);
         }
      }

      Serial.println(uid_size);
      Serial.println(mfrc522.uid.uidByte[1]);
      Serial.println(15, HEX);
      readRawContent = raw_content[0] + raw_content[1] + raw_content[2] + raw_content[3];
      readRawContent.toUpperCase();
      content = balik[0]+balik[1]+balik[2]+balik[3];
      content.toUpperCase();

     Serial.println(readRawContent);
     Serial.println(content);
     Serial.println();
    
      String n[8];
      unsigned int x[8];
      unsigned long int tmp;
      unsigned long int nomorKartu = 0;
      for (int i = 0; i<8; i++)
      {
        //Serial.print("i ");
        //Serial.println(i);
        
        n[i] = content[i];
        if (n[i] == "A")
          x[i] = 10;
        else if (n[i] == "B")
          x[i] = 11;
        else if (n[i] == "C")
          x[i] = 12;
        else if (n[i] == "D")
          x[i] = 13;
        else if (n[i] == "E")
          x[i] = 14;
        else if (n[i] == "F")
          x[i] = 15;
    //    if (n[i] == "0")
    //      x[i] = 0;
        else
          x[i] = (n[i].toInt());
    
       // Serial.print("x ");
       // Serial.println(x[i]);
    
        switch(i)
        {
          case 0: tmp = x[i]*268435456;              
                  break;
          case 1: tmp = x[i]*16777216;
                  break;
          case 2: tmp = x[i]*1048576;
                  break;
          case 3: tmp = x[i]*65536;
                  break;
          case 4: tmp = x[i] * 4096;
                  break;
          case 5: tmp = x[i]*256;
                  break;
          case 6: tmp = x[i]*16;
                  break;
          default: tmp = x[i];
                   break;
        }
        
       // Serial.print("tmp ");
       // Serial.println(tmp);
        
        nomorKartu += tmp;    
        //Serial.println(nomorKartu);
        //Serial.print("Nomor Kartu ");
        //Serial.println(nomorKartu);
      }
    
      Serial.print("Nomor Kartu: ");
      Serial.println(nomorKartu);
      //Serial.println();
      

      const int httpPort = 8989;
      if(!client.connect(host, httpPort)){
        Serial.println("Connection failed 2!");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Connection");
        lcd.setCursor(0,1);
        lcd.print("Failed!");
        digitalWrite(led_red, LOW);
        delay(500);
        digitalWrite(led_red, HIGH);
        delay(500);
        digitalWrite(led_red, LOW);
        delay(500);
        digitalWrite(led_red, HIGH);
        delay(500);
        digitalWrite(led_red, LOW);
        delay(500);
        digitalWrite(led_red, HIGH);
        delay(500);
        lcd.clear();
        resetFunc();
        //client.stop();
        return;
      }

      String url = "/write_data.php?kartu=";
      url += nomorKartu;
      url += "&raw=";
      url += readRawContent;

      Serial.print("Requesting URL: ");
      Serial.println(url);

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 1000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        digitalWrite(led_red, LOW);
        delay(500);
        digitalWrite(led_red, HIGH);
        delay(500);
        digitalWrite(led_red, LOW);
        delay(500);
        digitalWrite(led_red, HIGH);
        delay(500);
        digitalWrite(led_red, LOW);
        delay(500);
        digitalWrite(led_red, HIGH);
        delay(500);
        lcd.clear();
        resetFunc();
        return;
        }
      }

      
      String getJson;
      while (client.available()) {
        String line = client.readString();
//        Serial.print(line);
//        Serial.println(line.indexOf('{'));
//        Serial.println(line.indexOf('}'));
//        Serial.println(line.length());
        getJson = line.substring(178, 230);
        Serial.println(getJson);
      }
      
      JsonObject& root = jsonBuffer.parseObject(getJson);

      if(root.success()) {
        Serial.println("parseObject() success");
      }
      
      Serial.println();
      Serial.println("closing connection");
      //client.stop();
      nik = root["nik"];
      allow = root["allowed"];
      ambil1 = root["a1"];
      ambil2 = root["a2"];

      Serial.println(nik);
      Serial.println(allow);
      Serial.println(ambil1);
      Serial.println(ambil2);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("NIK: ");
      lcd.setCursor(5,0);
      lcd.print(nik);
      
      delay(1000);
      switchState = 3;

  case 3:
    if (nik=='\0'){
      if (nomorKartu == 2171168634){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("NIK MASTER");
      lcd.setCursor(0,1);
      lcd.print("MASTER Opening...");
      digitalWrite(led_green, LOW);
      digitalWrite(led_red, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(2000);
      digitalWrite(buzzer, LOW);
      doorClosing = digitalRead(proximity);
      digitalWrite(relayMagnetDoor, HIGH);
      while(!doorClosing){
        delay(100);
        doorClosing = digitalRead(proximity);
      }
      while (doorClosing){
        delay(100);
        doorClosing = digitalRead(proximity);
      }
      digitalWrite(relayMagnetDoor, LOW);
      digitalWrite(led_green, HIGH);
      digitalWrite(led_red, LOW);
      lcd.clear();
      resetFunc();
      return;
    }
      else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("NIK TIDAK");
        lcd.setCursor(0,1);
        lcd.print("TERDAFTAR!");
        digitalWrite(led_green, HIGH);
        digitalWrite(led_red, LOW);
        delay(2500);
        lcd.clear();
        resetFunc();
        return;
      }
    }

    
    
    if(allow == 1){
      digitalWrite(relayMagnetDoor, HIGH);
      doorClosing = digitalRead(proximity);
      digitalWrite(led_red, HIGH);
      digitalWrite(led_green, LOW);
      while (!doorClosing){
        //Serial.println("Silakan ambil 1");
        lcd.setCursor(0,1);
        lcd.print("Silakan ambil");
        digitalWrite(buzzer, HIGH);
        doorClosing = digitalRead(proximity);
        delay(350);
      }
      
      while(doorClosing){
        lcd.clear();
        
        //Serial.println("SEGERA TUTUP!");
        lcd.setCursor(0,0);
        lcd.print("SEGERA");
        lcd.setCursor(0,1);
        lcd.print("TUTUP!");
        digitalWrite(buzzer, LOW);
        delay(200);
        digitalWrite(buzzer, HIGH);
        //Serial.print(proximity);
        doorClosing = digitalRead(proximity);
      }
      digitalWrite(relayMagnetDoor, LOW);
      digitalWrite(buzzer, LOW);      
    }
    else{
      if(ambil1 == 0 && ambil2 == 0){
        lcd.setCursor(0,1);
        lcd.print("NOT ALLOWED!");
        digitalWrite(led_green, HIGH);
        digitalWrite(led_red, LOW);
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
        delay(500);
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
      }
      else{
        String ambil1String;
        String ambil2String;
        switch(ambil1){
          case 1:
            ambil1String = "SENIN";
            break;
          case 2:
            ambil1String = "SELASA";
            break;
          case 3:
            ambil1String = "RABU";
            break;
          case 4:
            ambil1String = "KAMIS";
            break;
          case 5:
            ambil1String = "JUMAT";
            break;
          case 6:
            ambil1String = "SABTU";
            break;
          case 7:
            ambil1String = "Minggu";
            break;
          default:
            ambil1String = "DEFAULT";
            break;
        }

        switch(ambil2){
          case 1:
            ambil2String = "SENIN";
            break;
          case 2:
            ambil2String = "SELASA";
            break;
          case 3:
            ambil2String = "RABU";
            break;
          case 4:
            ambil2String = "KAMIS";
            break;
          case 5:
            ambil2String = "JUMAT";
            break;
          case 6:
            ambil2String = "SABTU";
            break;
          case 7:
            ambil2String = "MINGGU";
            break;
          default:
            ambil1String = "DEFAULT";
            break;
        }

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("JATAH AMBIL:");
        lcd.setCursor(0,1);
        lcd.print(ambil1String + "  " + ambil2String);
        digitalWrite(led_green, HIGH);
        digitalWrite(led_red, LOW);
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
        delay(500);
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
        delay(500);
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
      }
    }
  }
  switchState = 1;
  delay(100);
  lcd.clear();
  resetFunc();
}
