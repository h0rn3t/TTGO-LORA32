#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "SSD1306.h"

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    868E6

SSD1306 display(0x3c, 4, 15);
String rssi = "RSSI --";
String packSize = "--";
String packet ;



void loraData(String packet){
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0 , 15 , "Received "+ packSize + " bytes");
    display.drawStringMaxWidth(0 , 26 , 128, packet);
    display.drawString(0, 0, rssi);
    display.display();
}

void logo(){
    display.clear();
    display.drawString(0, 0, "Loading...");
    display.display();
}

void log(String message){
    display.clear();
    display.drawString(0, 0, message);
    display.display();
}


void cbk(int packetSize) {
    log("recieved...");
    packet ="";
    packSize = String(packetSize,DEC);
    for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
    rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
    loraData(packet);
}

void setup() {
    pinMode(16,OUTPUT);
    digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
    delay(50);
    digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high、

    Serial.begin(9600);
    while (!Serial);
    Serial.println();
    Serial.println("LoRa Receiver Callback");
    SPI.begin(SCK,MISO,MOSI,SS);
    LoRa.setPins(SS,RST,DI0);
    if (!LoRa.begin(BAND)) {
        Serial.println("Starting LoRa failed!");
        log("Starting LoRa failed!");
        while (1);
    }
    //LoRa.onReceive(cbk);
    LoRa.receive();
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    log("init ok");
    delay(1500);
    //loop();
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        cbk(packetSize);
    }
    delay(10);
}