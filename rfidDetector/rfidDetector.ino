#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <Ethernet.h>
#include <SPI.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Approximate your card to the reader...");
  
  SPI.begin();
  mfrc522.PCD_Init();
}

int x = 0;
unsigned long int test = 0;
String balik[4] = {"00","00","00","00"};
void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()){
    delay(100);
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()){
    delay(100);
    return;
  }
  String content= "";
  for (byte i = 0; i < mfrc522.uid.size; i++){
    if (mfrc522.uid.uidByte[i] == 0){
      balik[3-i] = "00";
    }
    else if (mfrc522.uid.uidByte[i] < 16){
      balik[3-i] = "0" + String(mfrc522.uid.uidByte[i], HEX);
    }
    else{
      balik[3-i] = String(mfrc522.uid.uidByte[i], HEX);
    }
  }
  content = balik[0]+balik[1]+balik[2]+balik[3];
  content.toUpperCase();

  String n[8];
  unsigned int x[8];
  unsigned long int tmp;
  unsigned long int nomorKartu = 0;
  for (int i = 0; i<8; i++){
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
    else
      x[i] = (n[i].toInt());

    switch(i){
      case 0: 
        tmp = x[i]*268435456;              
        break;
      case 1: 
        tmp = x[i]*16777216;
        break;
      case 2: 
        tmp = x[i]*1048576;
        break;
      case 3: 
        tmp = x[i]*65536;
        break;
      case 4: 
        tmp = x[i] * 4096;
        break;
      case 5: 
        tmp = x[i]*256;
        break;
      case 6: 
        tmp = x[i]*16;
        break;
      default: 
        tmp = x[i];
        break;
    }
    nomorKartu += tmp;
  }

  String stringNomorKartu = (String) nomorKartu;
  while(stringNomorKartu.length() != 10){
    stringNomorKartu = "0" + stringNomorKartu;
  }
  Serial.println(stringNomorKartu);
  delay(1500);
}
