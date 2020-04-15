//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: crc32.cpp                                                                  //
//                                                                                  //
// $Id: crc32.c,v 1.2 2006/01/02 18:24:04 rasc Exp $                                //
// DVBSNOOP                                                                         //
// a dvb sniffer  and mpeg2 stream analyzer tool http://dvbsnoop.sourceforge.net/   //
// (c) 2001-2006   Rainer.Scherg@gmx.de (rasc)                                      //
// - Code Module CRC32  taken von linuxtv.or                                        //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __CRC32_H
#define __CRC32_H

#include <string>

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
class CRC32
{
private:
   static CRC32*_pinstance;

public:
   CRC32();
   ~CRC32();
   unsigned long checkCRC(unsigned char* data, int len);
   static CRC32* getInstance();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif

