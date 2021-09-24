#include <si4703.h>
 si4703 radio(3,2,4,5); // SDIO, SCLK, SEN, RST
 int f_cannel = 1019;
 
void setup() {
  radio.init();
  Serial.begin(9600); 
 
  radio.fm_start(f_cannel-760);// 101.9 МГц (1019-760 = 259)
  radio.si_volume(10);// 0...15
}
 
void loop() {
  if(Serial.available()){
    char serial = Serial.read();
    if(serial == 'u'){radio.seek_up();}
    if(serial == 'd'){radio.seek_down();}
    }  
 
Serial.print(" F = ");
Serial.print(float((radio.nom_ch()+760.00)/10),2);
Serial.print(" RDS =");
Serial.print(radio.readRDS());
Serial.print(" STC =");
Serial.print(radio.seach());
Serial.print(" RSSI =");
Serial.print(radio.rss());
Serial.print(" ST =");
Serial.println(radio.st_mn());
}
