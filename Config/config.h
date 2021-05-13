/*
Файл общей конфигурации проекта
*/
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//#include "stm32l1xx_hal.h"	// там в том числе определены и типы данных, поэтому включим везде этот файл
#include <stdint.h>

#define VERSION_STRING "Device: MILANDR FIRE v1.0.0"
#define VERSION_HW "HW: 00"


//#define HARDWARE_TEST		//Для тестирования выводов железа - постоянное одновременное переключение всех выводов

#define _DEBUG_
	
#define SYS_TICK		0.001		// период тика таймера (сек)
//#define PERIOD_CYCLE	0.1			// период опроса состояния и выполнения действий (в секундах) (ПРОВЕРЬТЕ ЗНАЧЕНИЕ OCR!)
#define PERIOD_CYCLE	0.2 //0.05			// v1.3 (темп кратен 0.05с!) период опроса состояния и выполнения действий (в секундах) (ПРОВЕРЬТЕ ЗНАЧЕНИЕ OCR!)
#define ONE_MS			0.001
#define MS_IN_TICK		(uint8_t)((SYS_TICK) / (ONE_MS))

#define	PWM_TIMER_CLOCK	(16000000)		// частота тактирования таймера ШИМ, Гц
#define	PWM_MAX_COUNT	(4096)		// (65536) Разрядность ШИМ, (максимальное значение) 65536 = 16бит

#define SEMAPHORE_BL_TIMER_PERIOD	(2.0)			// таймаут семафора контроля одновременного доступа с блютуз и 485
	
#define DEBUG_LEVEL_1	1
#define DEBUG_LEVEL_2	2
#define DEBUG_LEVEL_3	3
#define DEBUG_LEVEL_4	4
#define DEBUG_LEVEL_5	5
#ifdef _DEBUG_
//	void Write_debug(uint8_t lev, const char *str, ...);
//	extern const char d_file[];
	extern uint8_t debugLevel;	// уровень выводимой отладочной информации
	#define D_PRINTF(DLevel, param...)	if(debugLevel >= DLevel){putchar('\n');for(int i=0;i<DLevel;i++){putchar('\t');} printf("D:"); printf(param);}
	#define D_FPRINTF(DLevel, to, param...)	if(debugLevel >= DLevel){fputc('\n', to);for(int i=0;i<DLevel;i++){fputc('\t', to);} fprintf(to, "D:"); fprintf(to, param);}
//	#define D_PRINT_FILE(DLevel, param...) if(debugLevel >= DLevel){Write_debug(DLevel, param);}
#else
	#define D_PRINTF(DLevel, param...)		;
#endif

#define PROG_MAX_LEN	(99)		// максимальная длина программы (в состояниях)	
#define PROG_MAX_CNT	(20)		// максимальное количество программ
//#define PROG_MAX_LEN	(9)		// максимальная длина программы (в состояниях)	
//#define PROG_MAX_CNT	(3)		// максимальное количество программ

#define PWM_FREQ_HZ_MIN	(2)		// 
#define PWM_FREQ_HZ_MAX	(150)		// v1.3 (было 20)
	
#define PWM_DUTY_MIN	(5)		// %
#define PWM_DUTY_MAX	(100)		// %

#define PROG_TEMPO_S_MIN	(0.25)		// 
#define PROG_TEMPO_S_MAX	(20)		// 
	
#define PROG_TIME_MIN	(10)		// 	sec
#define PROG_TIME_MAX	(60*60)		// sec

#define BATT_SCALE_MIN	(0.0)			// %
#define BATT_SCALE_MAX	(100.0)		// %
#define BATT_INPUT_MIN	(3.0)			// Volt
#define BATT_INPUT_MAX	(20.0)		// Volt

#define BATT_WLAG_MIN	(0.01)			// 
#define BATT_WLAG_MAX	(1.0)				// 

#define PWDN_TIME_MIN	(10)		// 	sec
#define PWDN_TIME_MAX	(60*60)		// sec

//#define HEATER_TIMEOUT	(10*60) 	// Sec (test)
#define HEATER_TIMEOUT	(30*60) 	// Sec (work)

#define PROGRAM_IN_PROCEDURE_MAX_COUNT	(5)
#define PROCEDURE_IN_METHOD_MAX_COUNT		(12)
//#define METHOD_MAX_COUNT								(20)
#define METHOD_NAME_MAX_LEN							(100)

#define COMMAND_MAX_LEN									(50)	// Проверьте также параметры длины FIFO!

#define DAC_SAMPLE_RATE	(16000) //(16000)//(22050)

#ifdef __cplusplus
}
#endif


#endif /* CONFIG_H_INCLUDED */
