#include <Arduino.h>
#include <si4703.h>

unsigned int _reg2,_reg3,_reg4,_reg5,_reg6;
 
si4703::si4703(uint8_t sdio, uint8_t sclk, uint8_t sen, uint8_t rst){
  SDIO = sdio;
  SCLK = sclk;
  SEN = sen;
  RST = rst;
  pinMode(SDIO,OUTPUT);
  pinMode(SCLK,OUTPUT);
  pinMode(SEN,OUTPUT);
  pinMode(RST,OUTPUT);
}

void si4703::init(){
  digitalWrite(RST,LOW);delay(1);
  digitalWrite(RST,HIGH);delay(1);
  pr_si_write(0x07,0x8100);// кварц
  delay(500);
  pr_si_write(0x02,_reg2|=0xC001);//|(1<<11));// включение 0x4001
  pr_si_write(0x06,_reg6|=0b11111|(1<<15)|(1<<14));// фильтр поиска
  pr_si_write(0x04,_reg4|=(1<<11)|(1<<12)|(1<<7)|(1<<10));
  pr_si_write(0x05,_reg5|=(1<<6)|(1<<4));
  delay(200);
}

int si4703::read_device_id(){return pr_si_read(0);}
int si4703::read_chip_id(){return pr_si_read(1);}
int si4703::rss(){return pr_si_read(0x0A) & 0xFF;}
int si4703::nom_ch(){return pr_si_read(0x0B) & 0x3FF;}
bool si4703::st_mn(){return (pr_si_read(0x0A)>>8) & 0x01;}
bool si4703::rds_sinh(){return (pr_si_read(0x0A)>>11) & 0x01;}
bool si4703::pr_rds_status(){return (pr_si_read(0x0A) >> 15) & 0x01;}
bool si4703::pr_rds_st_name(){return (pr_si_read(0x0D) >> 11) == 0 || (pr_si_read(0x0D) >> 11) == 1;}
bool si4703::pr_rds_byte_length(byte rds_data){return rds_data >= 32 && rds_data <= 127;}
void si4703::pr_stc(){while((pr_si_read(0x0A)>>14) & 0x01 == 0);}
bool si4703::seach(){return (pr_si_read(0x0A)>>14) & 0x01;}
void si4703::si_volume(int vol){pr_stc();pr_si_write(0x05,_reg5=(1<<6)|(1<<4)|vol);}

bool si4703::pr_rds_st_error() {
  byte _rds_error = (byte)((pr_si_read(0x0A) & 0x0600) >> 9);
  byte Dh = (pr_si_read(0x0F) & 0xFF00) >> 8;
  byte Dl = (pr_si_read(0x0F) & 0x00FF);
  return _rds_error == 0 && pr_rds_byte_length(Dh) && pr_rds_byte_length(Dl);
}

char * si4703::readRDS(){ 
   char rdsBuffer[8]; 
   char * buffer = rdsBuffer;
   boolean completed[] = {false, false, false, false};
   int completedCount = 0;  
 if(pr_rds_status()){
 while(completedCount < 4) {
  if(pr_rds_st_name() && pr_rds_st_error()){
    uint16_t b = pr_si_read(0x0d);
    int index = b & 0x3;
    completed[index] = true;
    completedCount ++;  
    byte Dh = (pr_si_read(0x0F) & 0xFF00) >> 8;
    byte Dl = (pr_si_read(0x0F) & 0x00FF);
  buffer[index * 2] = Dh;
  buffer[index * 2 + 1] = Dl;
  delay(40);}else{delay(30);}}return buffer;} 
}


void si4703::fm_start(int ch){
    delay(200);
    pr_stc();pr_si_write(0x03,_reg3|=ch);
    pr_stc();pr_si_write(0x03,_reg3|=(1<<15));
    delay(200);
}

void si4703::seek_up(){
  pr_stc();pr_si_write(0x03,_reg3 = 0);
  pr_stc();pr_si_write(0x02,_reg2 &=~(1<<8));
  pr_stc();pr_si_write(0x02,_reg2 |= (1<<8)|(1<<9));
  }

void si4703::seek_down(){
  pr_stc();pr_si_write(0x03,_reg3 = 0);
  pr_stc();pr_si_write(0x02,_reg2 &=~(1<<8));
  _reg2 |= (1<<8);_reg2 &=~(1<<9);
  pr_stc();pr_si_write(0x02,_reg2);
  }  

void si4703::pr_si_write(byte _si_reg, unsigned int _si_data){
   pinMode(SDIO,OUTPUT);
   digitalWrite(SCLK,LOW);
   digitalWrite(SEN,LOW);
   int _addr = (0b01100 << 4) | _si_reg;
       for(int i = 8; i >= 0; i--){
        digitalWrite(SCLK,LOW);
        digitalWrite(SDIO, (_addr >> i) & 0x01);
        digitalWrite(SCLK,HIGH);
        }
       for(int i = 15; i >= 0; i--){
        digitalWrite(SCLK,LOW);
        digitalWrite(SDIO, (_si_data >> i) & 0x01);
        digitalWrite(SCLK,HIGH);
        }      
        digitalWrite(SCLK,LOW); 
        digitalWrite(SEN,HIGH);
        digitalWrite(SCLK,HIGH);     
  } 

unsigned int si4703::pr_si_read(byte _si_reg){
   unsigned int _si_byte_read = 0;
   bool _si_bit_read[16];
   pinMode(SDIO,OUTPUT);
   digitalWrite(SCLK,LOW);
   digitalWrite(SEN,LOW);
   int _addr = (0b01110 << 4) | _si_reg;
       for(int i = 8; i >= 0; i--){
        digitalWrite(SCLK,LOW);
        digitalWrite(SDIO, (_addr >> i) & 0x01);
        digitalWrite(SCLK,HIGH);
        } 
        digitalWrite(SCLK,LOW);
    pinMode(SDIO,INPUT);     
       for(int i = 15; i >= 0; i--){
        digitalWrite(SCLK,HIGH);
        _si_bit_read[i] = digitalRead(SDIO);
        _si_byte_read = _si_byte_read + (_si_bit_read[i] << i);
        digitalWrite(SCLK,LOW);
        } 
    digitalWrite(SEN,HIGH);
    digitalWrite(SCLK,HIGH); 
    return _si_byte_read;  
  }  

