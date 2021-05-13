/*
Функции исполнительных команд (с разных интерфейсов)
Версия 2.0 (04.02.2009)
*/

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "config.h" 	
//#include "types.h" 	
#include "tools.h" 	
#include "timer.h" 	
#include "cmdLineTerminal.h" 
#include "player.h" 	
#include "testMelody.h"
#include "w25qxx.h"
#include "uart_app.h"
#include "dictionary.h"

// for command "Echo" only
#include "terminal.h"


//****************************************************************************
// именованные константы
#define VAR_TYPE_MASK		0x00000007
#define VAR_STORETYPE_MASK	0x00000008
#define VAR_READONLY_MASK	0x00000010
#define VAR_BYTE			0x00000000
#define VAR_WORD			0x00000001
#define VAR_DWORD			0x00000002
#define VAR_FLOAT			0x00000003
//#define VAR_DOUBLE			0x00000004
#define VAR_CHAR			0x00000005
#define VAR_FLASH			0x00000008
#define VAR_READONLY		0x00000010

enum{
	TERMINAL_CMD_FORMAT_ERR=1, 
	TERMINAL_CMD_UNKNOWN,
	TERMINAL_CMD_PARAM_CNT_ERR, 
	TERMINAL_CMD_PARAM_VAL_ERR, 
	TERMINAL_CMD_PARAM_ACCESS_ERR,
};
/*static const char errMsg0[]="";
static const char errMsg1[]="Cmd format ERR!\n";
static const char errMsg2[]="Cmd unknown!\n";
static const char errMsg3[]="Cmd parameters count ERR!\n";
static const char errMsg4[]="Cmd param value ERR!\n";
static const char errMsg5[]="Cmd access ERR!\n";
static const char* pErrMsg[]={
	errMsg0,
	errMsg1,
	errMsg2,
	errMsg3,
	errMsg4,
	errMsg5,
};
#define msgCnt (sizeof(pErrMsg)/sizeof(pErrMsg[0]))
*/

#define EXIST_PARAM(x)	((argc >= x+1) && (strlen(argv[x]) != 0) && (*argv[x]!='?'))	// Х = номер проверяемого параметра командной строки

static char msgErrParam[]="E: param error!\n";
//static char msgErrStartPlay[]="E: play start error!\n";
//****************************************************************************
// внешние переменные

//****************************************************************************
// глобальные переменные доступные извне

struct VAR
{
	void *p;	// указатель
	unsigned char mode;	/* режим доступа	bit0-2 = type:
								000 - byte
								001	- word
								010	- dword
								011	- float
								100	- double
								101 - char[]
									bit3 = 1 - EEPROM (FLASH)
									bit4 = 1 - read only*/
};

/*const struct VAR var_table[] =
{
	{&configInfo.priborName, VAR_CHAR | VAR_FLASH | VAR_READONLY},	// 0	
	{&configInfo.IDNumber, VAR_WORD | VAR_FLASH},					// 1
	{(void*)&SIMConnectToTerminal, VAR_BYTE},				// 2
	{(void*)&GSM_SIM.revision, VAR_CHAR},						// 3
	{(void*)&configInfo.defaultNumber, VAR_CHAR},						// 4
	{(void*)&lastErrCode, VAR_WORD | VAR_READONLY},				// 5
//	{(void*)&prnMode, VAR_BYTE},										// 6
//	{(void*)&periodicityDefVal, VAR_BYTE},								// 7
//	{&MyInfoVar.Info.processingMode, VAR_WORD | VAR_FLASH},				// 8
};*/

// ****************************************************************************
// считывание текста ошибки
/*const char* CMD_getErrStr(int16_t err)
{
	if(err<0) err=-err;
	if(err < msgCnt){
		return pErrMsg[err];
	}
	return pErrMsg[0];
}*/


// ****************************************************************************
// функции обработки команд
// Параметры -  argc, argv[], типа интерфейса и указателя на выходную строку
// Возврат: 
//	0 - ОК
// < 0 - код ошибки
//
// ФУНКЦИИ НЕ ДОЛЖНЫ НАДОЛГО ЗАДЕРЖИВАТЬ ИСПОЛНЕНИЕ!!!

// Установка уровня выводимой отладочной информации
int cmd_debug (int argc, char** argv, FILE* to)
{
	unsigned short value; 
	int ret;
	if(EXIST_PARAM(0)){
		ret = sscanf(argv[0], "%hu", &value);
		if(ret){
			debugLevel=value;
		}
		else{
			fprintf(to, "%s", msgErrParam); 
			return(0);
		}
	}
	else{	
		fprintf(to, "Debug level = %d\n", debugLevel); 
	}
	return(0);
}

// Наименование устройства	с версией и номером
int cmd_name (int argc, char** argv, FILE* to)
{
//	fprintf( to, ("Device: TOV\nVersion: 0.1\nID: 123\n")); 
	fprintf(to, "%s %s\n", VERSION_STRING, VERSION_HW);
	return(0);
}

int cmd_status (int argc, char** argv, FILE* to)
{
/*	fprintf(to, "%d, %d, %d, %d, %.2f, %d, %d, %08X, %.1f\n", 
		(getPlayerState() == PLAY)?1:0,
		configInfo.progNum,
		configInfo.freqHz,
		configInfo.duty,
		configInfo.tempo,
		getProgWorkTime(),
		getProgTimeRemaining(),
		activeProgState,
		getChargePercent(BatteryVoltage)
	);*/
	return(0);
}

// параметры измерения емкости батареи (калибровка)
/*int cmd_battClbr1 (int argc, char** argv, FILE* to)
{
	float scale; 
	float input; 
	int ret, ret1;
	if(EXIST_PARAM(0) && EXIST_PARAM(1)){
		ret = sscanf(argv[0], "%f", &scale);
		ret1 = sscanf(argv[1], "%f", &input);
		if(ret && ret1 
			&& scale >= (BATT_SCALE_MIN) && scale <= (BATT_SCALE_MAX) 
			&& input >= (BATT_INPUT_MIN) && input <= (BATT_INPUT_MAX)
			){
			configInfo.chBattery.scaledMin =scale; 
			configInfo.chBattery.inputMin =input; 
			batteryInit();
			ConfigInfoWrite();
			fprintf(to, "OK\n"); 
		}
		else{
			fprintf(to, "%s", msgErrParam); 
			return(0);
		}
	}
	else{	
		fprintf(to, "BATT CALIBR 1 sc= %.2f, in= %.2f\n", configInfo.chBattery.scaledMin, configInfo.chBattery.inputMin); 
	}
	return(0);
}
*/
int cmd_help (int argc, char** argv, FILE* to)
{
	printHelpCmd(to);
	return(0);
}
/*
int cmd_sleep (int argc, char** argv, FILE* to)
{
	gotoSleepFlag=true;
	return(0);
}
*/

// Запуск тестовой мелодии
int cmd_dac (int argc, char** argv, FILE* to)
{
	unsigned short value; 
	int ret;
	if(EXIST_PARAM(0)){
		ret = sscanf(argv[0], "%hu", &value);
		if(ret){
			if(value){
			
				uint8_t b[3]={fire,in_sector,a1};
				testAudioStart(b,3);
			}else{
				testAudioStop();
			}
		}
		else{
			fprintf(to, "%s", msgErrParam); 
			return(0);
		}
	}
	else{	
//		fprintf(to, "Debug level = %d\n", debugLevel); 
	}
	return(0);
}

// Работа с внешней Флеш
static uint8_t tmpBuf[1024];
int cmd_w25q (int argc, char** argv, FILE* to)
{
	unsigned short value; 
	uint32_t flashAddr, flashSize;
	uint32_t flashData;
	int ret, ret1, ret2, ret3;
	if(EXIST_PARAM(0)){
		ret = sscanf(argv[0], "%hu", &value);
		if(ret){
			switch(value){
				case 1:
					W25qxx_Init();
				break;
				case 2:
					if(EXIST_PARAM(1) && EXIST_PARAM(2)){
						ret1 = sscanf(argv[1], "%x", &flashAddr);
						ret2 = sscanf(argv[2], "%x", &flashSize);
						if(ret1 && ret2 && flashSize < sizeof(tmpBuf)){
							W25qxx_ReadBytes(tmpBuf, flashAddr, flashSize);
							fprintf(to, "W25qxx_ReadBytes:\n"); 
							for (int i=0; i< flashSize/16; i++){
								for (int j=0; j< 16; j++){
									fprintf(to, " %02X", tmpBuf[i*16+j]);
								}
								fprintf(to, "\n");
								waitUartTxEnd(to);
							}
						}
						else{
							fprintf(to, "%s", msgErrParam); 
							return(0);
						}
					}
					else{
						fprintf(to, "%s", msgErrParam); 
						return(0);
					}
				break;
				case 3:
					if(EXIST_PARAM(1) && EXIST_PARAM(2) && EXIST_PARAM(3)){
						ret1 = sscanf(argv[1], "%x", &flashAddr);
						ret2 = sscanf(argv[2], "%x", &flashSize);
						ret3 = sscanf(argv[3], "%x", &flashData);
						if(ret1 && ret2 && ret3 && flashSize < sizeof(tmpBuf)){
							for (int i=0; i< flashSize; i++){
								W25qxx_WriteByte(flashData, flashAddr++);
							}
							fprintf(to, "W25qxx_Write - OK\n"); 
						}
						else{
							fprintf(to, "%s", msgErrParam); 
							return(0);
						}
					}
					else{
						fprintf(to, "%s", msgErrParam); 
						return(0);
					}
				break;
				case 4:
					if(EXIST_PARAM(1)){
						ret1 = sscanf(argv[1], "%x", &flashAddr);
						if(ret1){
							W25qxx_EraseSector(flashAddr);
							fprintf(to, "W25qxx_EraseSector - OK\n"); 
						}
						else{
							fprintf(to, "%s", msgErrParam); 
							return(0);
						}
					}
					else{
						fprintf(to, "%s", msgErrParam); 
						return(0);
					}
				break;
				default:
					break;
			}
		}
		else{
			fprintf(to, "%s", msgErrParam); 
			return(0);
		}
	}
	else{	
//		fprintf(to, "Debug level = %d\n", debugLevel); 
	}
	return(0);
}

/////////////////////////////////////////////////////////////////////////////////
// подключение команд
const cmdHandlers_t cmdList[]=
{
	{"DBG", 	CMD_PRIVILEGE_LEVEL_3, cmd_debug, "[new_debug_level]"},
	{"NAME", 	CMD_PRIVILEGE_LEVEL_3, cmd_name, ""},
//	{"LED", 	CMD_PRIVILEGE_LEVEL_3, cmd_led},
	{"STATUS",		CMD_PRIVILEGE_LEVEL_3, cmd_status, "out: runStatus..."},
//	{"BATCLBR1", 		CMD_PRIVILEGE_LEVEL_3, cmd_battClbr1, "[point1: Scale, Input]"},
//	{"SLEEP", 		CMD_PRIVILEGE_LEVEL_3, cmd_sleep, ""},
	{"DAC", 	CMD_PRIVILEGE_LEVEL_3, cmd_dac, "[0, 1]"},
	{"W25Q", 	CMD_PRIVILEGE_LEVEL_3, cmd_w25q, "[1=init, 2=readBlock (addr, size), 3=writeBlock(addr, size, val), 4=eraseSect(num)]"},
	{"HELP", 	CMD_PRIVILEGE_LEVEL_3, cmd_help, ""},
};

const uint8_t cmdListCnt = sizeof(cmdList)/sizeof(cmdList[0]);
