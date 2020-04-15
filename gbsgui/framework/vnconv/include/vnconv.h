//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: vnconv.h                                                                   //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef VNCONV_H_
#define VNCONV_H_

typedef enum VNCharset_e
{
   TCVN3,
   VIQR,
   UTF8,
   UTF8Composite,
   MAX_CHARSET_NUMBER
}
VNCharset_t;

static char* CharsetNames[MAX_CHARSET_NUMBER] =
{
   "TCVN3",
   "VIQR",
   "UTF-8"
};

typedef unsigned char    U8;
typedef unsigned short   U16;
typedef unsigned int     U32;

int vnconv(U8* inData, VNCharset_t inCode, U8* outData, VNCharset_t outCode);

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////
// !!! Dummy !!!
//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS

#endif /* VNCONV_H_ */
