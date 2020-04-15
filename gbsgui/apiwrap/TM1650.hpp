/*
 * TM1650.hpp
 *
 *  Created on: Jan 21, 2015
 *      Author: trungkt
 */

#ifndef TM1650_HPP_
#define TM1650_HPP_

#include "stapp_main.h"

extern "C" {

void TM1650_WriteByte(U8 wr_data);
void TM1650_WriteData(U8 addr, U8 data);
U8 TM1650_ReadByte();
U8 TM1650_ReadData();
void TM1650_LedDisplay(U8 *Buffer_p);
void TM1650_KeyProcess(void);
int TM1650_GetKeyStatus();
void TM1650_Test();
void TM1650_Start();
void TM1650_Stop();
void TM1650_Ack();
}
#endif /* TM1650_HPP_ */
