/**
  ******************************************************************************
  * File Name          : protocol_module.c
  * Date               : 31/07/2015 10:26:00
  * Description        : YAPST5
  ******************************************************************************
  *
  ******************************************************************************
  */

#include "protocol_module.h"
#include <stdio.h>
#include <stdint.h>

int pocket_buf[8];
int iterator=0;
uint8_t flag1=0;
uint8_t flag2=0;

struct command_struct
{
uint8_t first_byte_adr;
uint8_t second_byte_adr;
uint8_t command1;
uint8_t command2;
uint8_t rezerv1;
uint8_t rezerv2;	
uint8_t crc1;
uint8_t crc2;
}command;

struct answer_status_YAPST
{
uint8_t first_byte_adr;
uint8_t second_byte_adr;
uint8_t YAPST;
uint8_t YAPST_status;
uint8_t rezerv1;
uint8_t rezerv2;	
	
uint8_t problem;	
uint8_t fire;	
uint8_t Auto_off;	
uint8_t hand_start_up;	
uint8_t all_start_up;
uint8_t attention;
	
uint8_t rezerv3;
uint8_t PO_version;
	
uint8_t crc1;	
uint8_t crc2;
}status_YAPST;

void calculate_crc()
{

}


void get_byte(int byte)
{

if (flag1&&flag2)
		{
			pocket_buf[iterator]=byte;
			iterator++;
			if(iterator>7)
				{	
				flag1=0;
				flag2=0;
			command_interpritation();
				}
		}
else
		{	
			if (byte==16){flag1=1;}
			if (flag1 && byte==1){flag2=1;}
		}

}


uint8_t command_interpritation( )
{
printf(" \r\n NEW  ");

for(int i=0;i<8;i++){printf(" %x",pocket_buf[i]);}

command.first_byte_adr	=pocket_buf[0];
command.second_byte_adr	=pocket_buf[1];
command.command1				=pocket_buf[2];
command.command2				=pocket_buf[3];
command.rezerv1					=pocket_buf[4];
command.rezerv2					=pocket_buf[5];	
command.crc1						=pocket_buf[6];
command.crc2						=pocket_buf[7];

if (command.command1==yapst&&command.command2==status)
{


status_YAPST.first_byte_adr=answer_device_adress_first_byte;
status_YAPST. second_byte_adr=device_adress_second_byte;
status_YAPST. YAPST=yapst;
status_YAPST. YAPST_status=status;
status_YAPST. rezerv1=rezerv;
status_YAPST. rezerv2=rezerv;	
	
status_YAPST. problem					=0b00000000;	
status_YAPST. fire						=0b00000000;	
status_YAPST. Auto_off				=0b00000000;	
status_YAPST. hand_start_up		=0b00000000;	
status_YAPST. all_start_up		=0b00000000;
status_YAPST. attention				=0b00000000;
	
status_YAPST. rezerv3=rezerv;
status_YAPST. PO_version=po_version;
	
	calculate_crc();
	
status_YAPST. crc1;	
status_YAPST. crc2;

}
}