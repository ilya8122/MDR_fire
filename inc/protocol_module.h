#include <stdint.h>

#define question_device_adress_first_byte 0xA6 //0xA6 1 in first bit
#define answer_device_adress_first_byte 0x26 // 0 in first bit

#define device_adress_second_byte 0x72 //0x72 
#define yapst 0x07 //0x07
#define status 0x33 //0x33
#define rezerv 0x00 //0x00
#define po_version 1 //0x00

void get_byte(int byte);
uint8_t command_interpritation( );
