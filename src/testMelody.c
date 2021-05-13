/**
  ******************************************************************************
  * @file    testMelody.c
  * @author  FIA
  * @version V1.0.0
  * @date    12/02/2021
  * @brief   Воспроизведение тестового звука 
	*0x000082C0
  ******************************************************************************
  *
  *
  */

#include "testMelody.h"
#include "led.h"
#include "player.h"
#include "w25qxx.h"
#include "dictionary.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sound_build.h"
#include "sound_alarm.h"

static uint16_t* pSound; //указатель на буфер с музыкой

static int melodyPosition = 0;
uint32_t global_num; //общее чило байт на фразу
uint32_t global_counter; //байт выдано
uint8_t word_num; //количество слов во фразе
uint8_t nxt_WORD_FL = 0; //флаг перекл след слова
uint32_t now_w = 0; //номер воспроизводимого слова
uint32_t needless_global[10];

struct word //структура в которую записывается слова информация о конкретном аудиофайле
    {
    uint32_t start_adress; //start_adress of massive word in flash
    uint32_t num_byte; //number byte of massive word in flash
    uint8_t id;
};

struct word readed_dictionary[30]; //под массив слов
uint8_t num_word_dictionary; //количество слов в словаре
struct word* mas_word[10]; //массив структур на конечную фразу 10 я думаю хватит

/**
* @brief  читает 4 байта и пишет их в 1 переменную
* @adr  адресс первой ячейки в массиве флеш
* @retval возвращает перемнную объединяющую 3 эл массива {1,2,3,4} => 1234
*/
uint32_t read_4_byte(uint32_t adr)
{
    uint8_t byte_mas[4];
    uint32_t val = 0;
    W25qxx_ReadBytes(byte_mas, adr, 4);

    /*ВОЛШЕБСТВО*/ val = *((uint32_t*)byte_mas); // 4 байта в 1 переменную

    return val;
}

/**
* @brief  считывает с флеш памяти словарь(набор аудиофайлов) и пишет в readed_dictionary
* @retval None
*/
void init_audio_dictionaty()
{
		#if (DEBUG_audio==1)
	  printf("\r\n  /////////////////////////////////////////////////////");
		#endif

    num_word_dictionary = read_4_byte(0x00000000);
	
		#if (DEBUG_audio==1)
    printf("\r\n num_word_dictionary %d \r\n", num_word_dictionary);
		#endif
	
    uint32_t start_mem_adr = 0x00000004;

    for (int i = 0; i < num_word_dictionary; i++) {
        uint32_t adr1 = read_4_byte(start_mem_adr);
				#if (DEBUG_audio==1)
        printf("\r\n adr file%d  0x%02X ", i, adr1);
				#endif
        start_mem_adr = start_mem_adr + 4;
        uint32_t razmer1 = read_4_byte(start_mem_adr);
				#if (DEBUG_audio==1)
        printf("\r\n razmer file%d  %d ", i, razmer1);
				#endif
        start_mem_adr = start_mem_adr + 4;

        readed_dictionary[i].start_adress = adr1;
        readed_dictionary[i].num_byte = razmer1;
        readed_dictionary[i].id = i;
    }
}
/**
* @brief  прерывает воспроизведение
* @retval None
*/
void testAudioStop(void)
{
    melodyPosition = 0;
    play_stop();
}

/**
* @brief  запускает подготовку к воспроизведению
* @param  needless массив айдишников требуемых слов,
* @param  num количество элементов needless
* @retval None
*/

void testAudioStart(uint8_t* needless, uint8_t num)
{
    testAudioStop();
			
    init_audio_dictionaty();
		global_num = 0;
	  global_counter = 0;
    word_num = num;
    melodyPosition = 0;
    nxt_WORD_FL = 0; //флаг перекл след слова
    now_w = 0; //номер воспроизводимого слова
	
	  for (int i = 0; i < num; i++)
		{
		needless_global[i]=needless[i];
		global_num = global_num + readed_dictionary[needless_global[i]].num_byte;
		#if (DEBUG_audio==1)							
    printf("\r\n needless_global %d ", needless_global[i]);
		#endif
		}   

		#if (DEBUG_audio==1)
    printf("\r\n word_num %d ", word_num);
		#endif		
 
		#if (DEBUG_audio==1)
    printf("\r\n  global_num %d ", global_num);
    printf("\r\n  /////////////////////////////////////////////////////");
		#endif
    //////////////////////////////////////////////////////
    melodyPosition = 0;
    play_start();
    //////////////////////////////////////////////////////
}

/**
	* @brief  play_nextBlockCallback. Запрос на следующий блок данных. Вызывается по сигналам DMA. Если данных больше нет, вернуть 0
	* 				ЧИТАЕТ ИЗ ФЛЕШ ПАМЯТИ uint8_t И УПАКОВЫВАЕТ В uint16_t ПОСЛЕ ЧЕГО ЛЕТИТ НА ЦАП
	* @param  pBuf		- буфер для записи блока
  * @param  maxLen	- Длина буфера
  * @retval число записанных данных
  */
uint16_t play_nextBlockCallback(uint16_t* pBuf, uint16_t maxLen)
{
    uint16_t val1 = 0xFFFF;
    uint32_t adr;
    uint16_t counter = 0;

    if (nxt_WORD_FL) //переход на новое слово
    {
        nxt_WORD_FL = 0;
        now_w = now_w + 1;
        melodyPosition = 0;
    }

    /*ВОЛШЕБСТВО*/ uint8_t* p1 = (uint8_t*)pBuf; // ИЗ 2хбайтного массива в 1байтный

    adr = readed_dictionary[needless_global[now_w]].start_adress; //readed_dictionary[]
    W25qxx_ReadBytes(p1, adr + (melodyPosition), maxLen * 2);

    pSound = (uint16_t*)p1;

    while (global_counter < global_num && counter < maxLen) {
        val1 = pSound[counter++];
        if (val1 > 4095) {
            val1 = 4095;
        }
        *pBuf++ = val1;
        melodyPosition = melodyPosition + 2;
        global_counter = global_counter + 2;
    }

    if (melodyPosition >= readed_dictionary[needless_global[now_w]].num_byte) //конец воспроизведения слова
    {		
      nxt_WORD_FL = 1;
    }

    if (global_counter >= global_num) //конец воспроизведения фразы
    {			
				nxt_WORD_FL = 0;
        return 0;
    }

    return counter;
}
