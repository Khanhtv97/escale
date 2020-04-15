//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GLanguage.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "GLanguage.hpp"
#include "GLang_VN.hpp"
#include "GLang_EN.hpp"
#include "vnconv.h"
#include <string.h> // for strlen
#include <stdlib.h> // for malloc

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////
GLanguage::GLanguage() : m_curLanguage(VN)
{
   this->m_ppLanguages = new std::string*[LANGUAGE_COUNT];

   for( int i = 0; i < LANGUAGE_COUNT; i++ )
   {
      this->m_ppLanguages[i] = new std::string[LANGUAGE_ITEM_COUNT];
   }
   this->Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////
void GLanguage::Initialize()
{
   Initialize_EN(m_ppLanguages[EN]);
   Initialize_VN(m_ppLanguages[VN]);

   for( int i = 0; i < LANGUAGE_ITEM_COUNT; i++ )
   {
      char* pStrConverted = NULL;
      if( VN_Convert(m_ppLanguages[VN][i].c_str(), pStrConverted) == true )
      {
         m_ppLanguages[VN][i].clear();
         m_ppLanguages[VN][i].assign(pStrConverted);
         free(pStrConverted);
         pStrConverted = NULL;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GLanguage& GLanguage::getInstance()
{
   static GLanguage instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
int GLanguage::getCurrentLanguage()
{
   return m_curLanguage;
}

//////////////////////////////////////////////////////////////////////////////////////
void GLanguage::setCurrentLanguage(int lang)
{
   if( (lang >= 0) && (lang < LANGUAGE_COUNT) )
   {
      m_curLanguage = lang;
   }
   else
   {
      _DTraceError("Language #%d not available!", lang);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
const char* GLanguage::operator[](int i)
{
   return getStr(i);
}

//////////////////////////////////////////////////////////////////////////////////////
const char* GLanguage::operator()(int lang, int i)
{
   return getStr(lang, i);
}

//////////////////////////////////////////////////////////////////////////////////////
const char* GLanguage::getStr(int i)
{
   if( ( i < 0 ) || ( i >= LANGUAGE_ITEM_COUNT) )
   {
      _DTraceError("String #%d for language #%d not available!", i, m_curLanguage);
      return "";
   }

   if( m_curLanguage == VN )
   {
      return m_ppLanguages[VN][i].c_str();
   }
   else
   {
      return m_ppLanguages[EN][i].c_str();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
const char* GLanguage::getStr(int lang, int i)
{
   if( (lang < 0) || (lang >= LANGUAGE_COUNT) )
   {
      _DTraceError("Language #%d not available!", lang);
      return "";
   }

   if( (i < 0) || (i >= LANGUAGE_ITEM_COUNT) )
   {
      _DTraceError("String #%d for language #%d not available!", i, lang);
      return "";
   }

   if( lang == VN )
   {
      return m_ppLanguages[VN][i].c_str();
   }
   else
   {
      return m_ppLanguages[EN][i].c_str();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GLanguage::~GLanguage()
{
   for( int i = 0; i < LANGUAGE_COUNT; i++ )
   {
      delete[] this->m_ppLanguages[i];
      this->m_ppLanguages[i] = NULL;
   }

   delete[] this->m_ppLanguages;
   this->m_ppLanguages = NULL;
}

#ifdef NEED_CONVERT_VIETNAMESE_STRING

//////////////////////////////////////////////////////////////////////////////////////
// Note: Plz free pOutChar after using
//////////////////////////////////////////////////////////////////////////////////////
bool GLanguage::VN_Convert(const char* pInChar, char*& pOutChar)
{
   if( pOutChar != NULL )
   {
      free(pOutChar);
   }
   pOutChar = NULL;

   if( (pInChar == NULL) || (strlen(pInChar) < 1) )
   {
      return false;
   }

   pOutChar = (char*)malloc(strlen(pInChar));

   if( pOutChar != NULL )
   {
      vnconv((U8*)pInChar, VIQR, (U8*)pOutChar, TCVN3);
   }

   return (bool)(pOutChar != NULL);
}

//////////////////////////////////////////////////////////////////////////////////////
// Note: Plz free pOutChar after using
//////////////////////////////////////////////////////////////////////////////////////
bool GLanguage::VN_ConvertUTF8(const char* pInChar, char*& pOutChar)
{
   if( pOutChar != NULL )
   {
      free(pOutChar);
   }
   pOutChar = NULL;

   if( (pInChar == NULL) || (strlen(pInChar) < 1) )
   {
      return false;
   }

   pOutChar = (char*)malloc(strlen(pInChar)+3);
   if( pOutChar != NULL )
   {
      memset(pOutChar, 0, strlen(pInChar)+3);
      vnconv((U8*)pInChar, UTF8, (U8*)pOutChar, TCVN3);
   }

   return (bool)(pOutChar != NULL);
}

#endif // of #ifdef NEED_CONVERT_VIETNAMESE_STRING

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
