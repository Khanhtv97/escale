/*
 * TM1668.hpp
 *
 *  Created on: Jan 21, 2015
 *      Author: trungkt
 */

#ifndef TM1668_HPP_
#define TM1668_HPP_

#include "stapp_main.h"

extern "C" {

void TM1668_WriteByte(U8 wr_data);
void TM1668_Send_Data(U8 addr_led, U8 data_led);
void TM1668_LedDisplay(U8 *Buffer_p);
void TM1668_Show_2012();
U8 TM1668_ReadByte();
void TM1668_KeyProcess(void);
int TM1668_GetKeyStatus();
}

#endif /* TM1668_HPP_ */
