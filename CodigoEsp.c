#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define URL_BASE "http://tcceloccs.duckdns.org/api/RFID/"
#define SS_PIN  7
#define RST_PIN 1
#define PIN_OUT 2
#define LEDR 0
#define LEDG 10

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_DumpVersionToSerial();
  pinMode(8,OUTPUT);
  pinMode(PIN_OUT,OUTPUT);
  pinMode(LEDG,OUTPUT);
  pinMode(LEDR,OUTPUT);
  Serial.println("MFRC522 Ready"); 
  WiFi.begin("aaa", "");
  long time0 = millis();
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(8,LOW);
    delay(500);
    Serial.print('.');
    delay(500);
    digitalWrite(8,LOW);
    if((WiFi.status() == 6 || WiFi.status() == 4) && millis() > time0 + 15000){
      ESP.restart();
    }
  }
  WiFi.setAutoReconnect(true);
  //configTime(0, 0, ntpServer);
  //configTime(3600 * -3, 0, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.RSSI());

}

void loop() {
  delay(10);
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }
  String UID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    //UID = UID + String(rfid.uid.uidByte[i] < 0x10 ? ":0" : ":");
    UID = UID + String(rfid.uid.uidByte[i], HEX);
  }
  Serial.println(UID);

  rfid.PICC_HaltA();

  HTTPClient http;
  String url = URL_BASE + UID;
  http.begin(url);
  int codHttp = http.GET();
  UID = http.getString();
  http.end();
  Serial.println(String(codHttp));
  Serial.println(UID);
  if(codHttp != 200){
    long t0 = millis() + 5000;
    while(millis() <= t0){
      digitalWrite(LEDR, HIGH);
      delay(250);
      digitalWrite(LEDR, LOW);
      delay(250);
    }
  }
  if(UID[0] == 'L'){
    digitalWrite(PIN_OUT, HIGH);
    digitalWrite(LEDG, HIGH);
    delay(5000);
    digitalWrite(PIN_OUT, LOW);
    digitalWrite(LEDG, LOW);
  }
  else{
    digitalWrite(LEDR, HIGH);
    delay(3000);
    digitalWrite(LEDR, LOW);
  }
  delay(1000);
}
