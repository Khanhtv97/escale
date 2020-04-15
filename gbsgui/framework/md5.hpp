//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: md5.hpp                                                                    //
//                                                                                  //
// MD5.CC - source code for the C++/object oriented translation and                 //
//          modification of MD5.                                                    //
//                                                                                  //
// Translation and modification (c) 1995 by Mordechai T. Abzug                      //
//////////////////////////////////////////////////////////////////////////////////////

#include <cstdio>

//namespace GBS {
//namespace STM {
//namespace Framework {

// first, some types:
typedef unsigned       int  uint4; // assumes integer is 4 words long
typedef unsigned short int  uint2; // assumes short integer is 2 words long
typedef unsigned       char uint1; // assumes char is 1 word long

//////////////////////////////////////////////////////////////////////////////////////
class MD5
{

public:
   // methods for controlled operation:
   MD5(); // simple initializer
   void update(unsigned char* input, unsigned int input_length);
   void finalize();

   // constructors for special circumstances.  All these constructors finalize
   // the MD5 context.
   MD5(unsigned char* input, unsigned int length);

   // methods to acquire finalized result
   unsigned char* raw_digest ();  // digest as a 16-byte binary array
   char* hex_digest ();  // digest as a 33-byte ascii-hex string

private:
   // next, the private data:
   uint4 m_state[4];
   uint4 m_count[2];     // number of *bits*, mod 2^64
   uint1 m_buffer[64];   // input buffer
   uint1 m_digest[16];
   uint1 m_finalized;

   // last, the private methods, mostly static:
   void init();                   // called by all constructors
   void transform(uint1* buffer); // does the real update work.  Note
                                   // that length is implied to be 64.

   void encode(uint1* dest, uint4* src, uint4 length);
   void decode(uint4* dest, uint1* src, uint4 length);
   void memcpy(uint1* dest, uint1* src, uint4 length);
   void memset(uint1* start, uint1 val, uint4 length);

   uint4 rotate_left(uint4 x, uint4 n);
   uint4 F(uint4 x, uint4 y, uint4 z);
   uint4 G(uint4 x, uint4 y, uint4 z);
   uint4 H(uint4 x, uint4 y, uint4 z);
   uint4 I(uint4 x, uint4 y, uint4 z);
   void FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
   void GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
   void HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
   void II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
