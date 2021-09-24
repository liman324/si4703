// liman324@yandex.ru rcl-radio.ru


#ifndef si4703_H
#define si4703_H

#define si4703_address 0x70

#include <Arduino.h>
class si4703
{
  public:
    si4703(uint8_t, uint8_t, uint8_t, uint8_t);
    void init();
    void seek_up();
    void seek_down();
    void fm_start(int ch);
    int rss();
    int nom_ch();
    bool st_mn();
    char * readRDS();
    int read_device_id();
    int read_chip_id();
    bool rds_sinh();
    bool seach();
    void si_volume(int vol);

unsigned int si_read(byte si_reg);
 	
 private:
   uint8_t SDIO;
   uint8_t SCLK;
   uint8_t SEN;
   uint8_t RST;
   void pr_stc();
   void pr_si_write(byte _si_reg, unsigned int _si_data);
   unsigned int pr_si_read(byte _si_reg);
   bool pr_rds_byte_length(byte rds_data); 
   bool pr_rds_st_error();    
   bool pr_rds_st_name();
   bool pr_rds_status();
};
	
#endif //si4703_H
