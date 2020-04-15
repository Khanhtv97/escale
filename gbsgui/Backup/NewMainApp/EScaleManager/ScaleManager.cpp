//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ScaleManager.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#include "ScaleManager.hpp"
#include "GStringUtils.hpp"
#include "GSystem.hpp"

#define BYTES_FIXED_16 16
#define BYTES_FIXED_18 18
#define BYTES_FIXED_21 21
#define NUM_READ_TRIAL  5

#define ASCII_CR 0x0D
#define ASCII_LF 0x0A

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

ScaleManager::ScaleManager() : SerialPort(SPN_COM1)
{
   m_isUpdatingRateTable = false;
}

//////////////////////////////////////////////////////////////////////////////////////
ScaleManager::~ScaleManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////
ScaleManager& ScaleManager::getInstance()
{
   static ScaleManager instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ScaleManager::init()
{
   m_scaleDataType = SDT_NONE;
   m_scaleWeightData.clear();
   m_scaleItemDatas.clear();
   m_scaleItemDataDeleted.clear();
}

//////////////////////////////////////////////////////////////////////////////////////
bool ScaleManager::deinit()
{
}

//////////////////////////////////////////////////////////////////////////////////////
bool ScaleManager::isReady()
{
}

//////////////////////////////////////////////////////////////////////////////////////
ScaleDataType ScaleManager::scanWeightTx()
{
   ScaleDataType ret = SDT_NONE;
   U8 bufferRead[SERIAL_BUFFER_SIZE];
   U32 totalRead = 0;
   U8 pBuffer_i[SERIAL_BUFFER_SIZE];
   U32 numRead_i = 0;

   if( m_isUpdatingRateTable == true )
   {
      return ret;
   }

   readBytes(pBuffer_i, numRead_i);

   if( (numRead_i > 0) && (numRead_i < SERIAL_BUFFER_SIZE) )
   {
      for( int numTrial = 0; numTrial < NUM_READ_TRIAL; ++numTrial)
      {
         if( (numRead_i > 0) && (numRead_i < SERIAL_BUFFER_SIZE) )
         {
            //_DTraceDebug("READ TRIAL %d: %s", numTrial+1, (char*)pBuffer_i);

            for( int i = 0; i < numRead_i; ++i)
            {
               bufferRead[totalRead+i] = pBuffer_i[i];
            }
            totalRead += numRead_i;
         }

         if( isNeedMoreData(bufferRead, totalRead) )
         {
            numRead_i = 0;
            Task_Delay(ST_GetClocksPerSecond()/10);
            readBytes(pBuffer_i, numRead_i);
         }
         else
         {
            break;
         }
      }

      bufferRead[totalRead] = ASCII_CR;
      bufferRead[totalRead+1] = ASCII_LF;
      bufferRead[totalRead+2] = 0;
      //_DTraceDebug("SCALE BUFFER: %s", (char*)bufferRead);
      ret = parseWeightData(bufferRead, totalRead);
   }

   //traceData(ret);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
ScaleDataType ScaleManager::getDataType()
{
   return m_scaleDataType;
}

//////////////////////////////////////////////////////////////////////////////////////
void ScaleManager::getLastItemData(ScaleItemData& oLastItemData)
{
   if( m_scaleItemDatas.size() > 0 )
   {
      oLastItemData = m_scaleItemDatas.back();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ScaleManager::getItemDataDeleted(ScaleItemData& oItemDataDeleted)
{
   oItemDataDeleted = m_scaleItemDataDeleted;
}

//////////////////////////////////////////////////////////////////////////////////////
void ScaleManager::resetItemDatas()
{
   m_scaleItemDatas.clear();
}

//////////////////////////////////////////////////////////////////////////////////////
void ScaleManager::getItemDatas(std::list<ScaleItemData>& oItemDatas)
{
   oItemDatas = m_scaleItemDatas;
}

//////////////////////////////////////////////////////////////////////////////////////
void ScaleManager::getWeightData(ScaleWeightData& oWeightData)
{
   oWeightData.clear();
   oWeightData.header1 = m_scaleWeightData.header1;
   oWeightData.header2 = m_scaleWeightData.header2;
   oWeightData.sign = m_scaleWeightData.sign;
   oWeightData.data1 = m_scaleWeightData.data1;
   oWeightData.data2 = m_scaleWeightData.data2;
   oWeightData.unit = m_scaleWeightData.unit;
}

bool ScaleManager::isNeedMoreData(U8* pBuffer, U32 length)
{
   bool ret = true;

   if( length == BYTES_FIXED_18 || length == BYTES_FIXED_21 )
   {
      if( pBuffer[length-2] == ASCII_CR && pBuffer[length-1] == ASCII_LF )
      {
         ret = false;
      }
   }
   else if( length >= ((BYTES_FIXED_16+2)*4 + BYTES_FIXED_16) )
   {
      ret = false;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
ScaleDataType ScaleManager::parseWeightData(U8* pBuffer, U32 length)
{
   ScaleDataType ret = SDT_NONE;
   if( length == BYTES_FIXED_18 || length == BYTES_FIXED_21 )
   {
      if( pBuffer[length-2] == ASCII_CR && pBuffer[length-1] == ASCII_LF )
      {
         ret = SDT_FORMAT1;
      }
   }
   else if( length > BYTES_FIXED_21 )
   {
      if( pBuffer[16] == ASCII_CR && pBuffer[17] == ASCII_LF )
      {
         ret = SDT_FORMAT2;
      }
   }

   if( ret == SDT_FORMAT1 )
   {
      //_DTraceDebug("SDT_FORMAT1");
      std::string sItemDeleted;
      if( isItemDeleted(pBuffer, length, sItemDeleted) )
      {
         //_DTraceDebug("Deleted item %s", sItemDeleted.c_str());
         U32 itemIdDeleted = (U32)GStringUtils::str2int(sItemDeleted);
         if( deleteItemData(itemIdDeleted) )
         {
            ret = SDT_FORMAT2_DEL;
         }
      }
      else
      {
         m_scaleWeightData.header1 = getHeader1(pBuffer, length);
         m_scaleWeightData.header2 = getHeader2(pBuffer, length);
         m_scaleWeightData.sign = getSign(pBuffer, length);
         m_scaleWeightData.unit = getUnit(pBuffer, length);
         m_scaleWeightData.data1 = getData1(pBuffer, length);
         m_scaleWeightData.data2 = getData2(pBuffer, length);
      }
   }
   else if( ret == SDT_FORMAT2 )
   {
      //_DTraceDebug("SDT_FORMAT2");
      size_t post1 = 0;
      size_t post2 = 0;
      std::string strBuffer((char*)pBuffer);
      std::list<std::string> lstItem_i;
      lstItem_i.clear();

      //split into lstItem_i
      for( U32 i = 0; i < length-1; ++i )
      {
         if( pBuffer[i] == ASCII_CR && pBuffer[i+1] == ASCII_LF )
         {
            post2 = i;
            lstItem_i.push_back(strBuffer.substr(post1, post2-post1));
            i = post2 + 2;
            post1 = i;
         }
      }

      if( lstItem_i.size() > 0 )
      {
         ScaleItemData itemData;
         itemData.clear();
         itemData.type = getItemType(lstItem_i, itemData.typeName);
         itemData.dataNet = getItemDataNet(lstItem_i, itemData.unit);
         itemData.dataTare = getItemDataTare(lstItem_i, itemData.unit);
         itemData.unitPrice = getItemUnitPrice(lstItem_i);
         itemData.totalPrice = getItemTotalPrice(lstItem_i);
         itemData.pay = getItemPay(lstItem_i);
         itemData.change = getItemChange(lstItem_i);

         if( itemData.type == SIT_ITEM && (U32)GStringUtils::str2int(itemData.typeName) == ESCALE_NUM_KEY )
         {
            //restore the original product ID
            if( m_lastProductInfo.id < 100 )
            {
               GStringUtils::format( itemData.typeName, "%02d", m_lastProductInfo.id);
            }
            else
            {
               GStringUtils::format( itemData.typeName, "%d", m_lastProductInfo.id);
            }
         }

         itemData.timeStamp = _S_.getCurrentTimeSecond();

         m_scaleItemDatas.push_back(itemData);

         if( itemData.type == SIT_TOTAL )
         {
            ret = SDT_FORMAT2_MC;
         }
      }
      else
      {
         ret = SDT_NONE;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ScaleManager::isItemDeleted(U8* pBuffer, U32 length, std::string& itemDeleted)
{
   bool ret = false;

   if( length == BYTES_FIXED_18 )
   {
      //DELETED
      if( pBuffer[0] == 'D' &&
          pBuffer[1] == 'E' &&
          pBuffer[2] == 'L' &&
          pBuffer[3] == 'E' &&
          pBuffer[4] == 'T' &&
          pBuffer[5] == 'E' &&
          pBuffer[6] == 'D')
      {
          std::string sName;
          sName.assign((char*)(&pBuffer[8]));
          itemDeleted.assign(GStringUtils::trimAll(sName));
          ret = true;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ScaleManager::deleteItemData(U32 itemId)
{
   bool itemFound = false;

   m_scaleItemDataDeleted.clear();

   if( m_scaleItemDatas.empty() )
   {
      return itemFound;
   }

   if( itemId >= ESCALE_NUM_KEY )
   {
      //must be using the id from barcode reader
      itemId = m_lastProductInfo.id;
   }

   for( std::list<ScaleItemData>::iterator it = m_scaleItemDatas.begin(); it != m_scaleItemDatas.end(); ++it )
   {
      if( (*it).type == SIT_ITEM )
      {
         U32 itemId_i = (U32)GStringUtils::str2int((*it).typeName);

         if( itemId == itemId_i )
         {
            m_scaleItemDataDeleted = (*it);
            m_scaleItemDatas.erase(it);
            itemFound = true;
            break;
         }
      }
   }

   return itemFound;
}

//////////////////////////////////////////////////////////////////////////////////////
ScaleHeader ScaleManager::getHeader1(U8* pBuffer, U32 length)
{
   ScaleHeader ret = SHE_NONE;
   if( length == BYTES_FIXED_18 || length == BYTES_FIXED_21 )
   {
      if( pBuffer[0] == 'O' && pBuffer[1] == 'L' )
      {
         ret = SHE_OL;
      }
      else if( pBuffer[0] == 'S' && pBuffer[1] == 'T' )
      {
         ret = SHE_ST;
      }
      else if( pBuffer[0] == 'U' && pBuffer[1] == 'S' )
      {
         ret = SHE_US;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
ScaleHeader ScaleManager::getHeader2(U8* pBuffer, U32 length)
{
   ScaleHeader ret = SHE_NONE;
   if( length == BYTES_FIXED_18 || length == BYTES_FIXED_21 )
   {
      if( pBuffer[3] == 'N' && pBuffer[4] == 'T' )
      {
         ret = SHE_NT;
      }
      else if( pBuffer[3] == 'G' && pBuffer[4] == 'S' )
      {
         ret = SHE_GS;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
ScaleUnit ScaleManager::getUnit(U8* pBuffer, U32 length)
{
   ScaleUnit ret = SUN_NONE;
   if( length == BYTES_FIXED_18 )
   {
      if( pBuffer[14] == 'l' && pBuffer[15] == 'b' )
      {
         ret = SUN_LB;
      }
      else if( pBuffer[14] == 'k' && pBuffer[15] == 'g' )
      {
         ret = SUN_KG;
      }
      else if( pBuffer[14] == ' ' && pBuffer[15] == 'g' )
      {
         ret = SUN_G;
      }
   }
   else if( length == BYTES_FIXED_21 )
   {
      if( pBuffer[15] == ' ' && pBuffer[16] == 'h' && pBuffer[17] == 'k' && pBuffer[18] == 'g' )
      {
         ret = SUN_HKG;
      }
      else if( pBuffer[15] == 'l' && pBuffer[16] == 'b' && pBuffer[17] == 'o' && pBuffer[18] == 'z' )
      {
         ret = SUN_LBOZ;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ScaleManager::getSign(U8* pBuffer, U32 length)
{
   bool ret = true;
   if( length == BYTES_FIXED_18 || length == BYTES_FIXED_21 )
   {
      if( pBuffer[6] == '-' )
      {
         ret = false;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
float ScaleManager::getData1(U8* pBuffer, U32 length)
{
   float ret = 0;
   std::string sNumber = "";

   if( length == BYTES_FIXED_18 )
   {
      sNumber.append((char*)&pBuffer[7], 7);
   }
   else if( length == BYTES_FIXED_21 )
   {
      sNumber.append((char*)&pBuffer[7], 8);
   }

   std::string sFloatNumber = GStringUtils::trimAll(sNumber);
   ret = GStringUtils::str2float(sFloatNumber);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
float ScaleManager::getData2(U8* pBuffer, U32 length)
{
   return (float)0; //TBD
}

//////////////////////////////////////////////////////////////////////////////////////
std::string ScaleManager::header2Str(ScaleHeader header)
{
   std::string ret = "";
   switch( header )
   {
   case SHE_OL:
      ret.assign("OL");
      break;
   case SHE_ST:
      ret.assign("ST");
      break;
   case SHE_US:
      ret.assign("US");
      break;
   case SHE_NT:
      ret.assign("NT");
      break;
   case SHE_GS:
      ret.assign("GS");
      break;
   default:
      ret.assign("??");
      break;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
std::string ScaleManager::unit2Str(ScaleUnit unit)
{
   std::string ret = "";

   switch( unit )
   {
   case SUN_KG:
      ret.assign("kg");
      break;
   case SUN_G:
      ret.assign("g");
      break;
   case SUN_LB:
      ret.assign("lb");
      break;
   case SUN_HKG:
      ret.assign("hkg");
      break;
   case SUN_LBOZ:
      ret.assign("lboz");
      break;
   case SUN_PCS:
      ret.assign("pcs");
      break;
   case SUN_NONE:
   default:
      ret.assign("???");
      break;
   }
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
std::string ScaleManager::data2Str(float data)
{
   std::string ret = "";
   GStringUtils::format(ret, "%.2f", data);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
std::string ScaleManager::genPackageId(char* pCode, time_t timeOfWeight)
{
   std::string ret = "";
   std::string sTimeOfWeight = "";
   GStringUtils::format(sTimeOfWeight, "%d", timeOfWeight);
   ret.assign(pCode);
   ret.resize(3, '-');
   ret.append(sTimeOfWeight);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
ScaleItemType ScaleManager::getItemType(std::list<std::string>& lstItems, std::string& typeName)
{
   ScaleItemType ret = SIT_NONE;

   for( std::list<std::string>::iterator it = lstItems.begin(); it != lstItems.end(); ++it )
   {
      if( (*it).length() < BYTES_FIXED_16 )
      {
         continue;
      }

      std::string strItem_i;
      strItem_i.assign(GStringUtils::trimAll(*it));

      if( ((strItem_i.c_str())[0]) == 'I' &&
          ((strItem_i.c_str())[1]) == 'T' &&
          ((strItem_i.c_str())[2]) == 'E' &&
          ((strItem_i.c_str())[3]) == 'M' )
      {
         std::string sName;
         sName.assign(&((strItem_i.c_str())[4]));
         typeName.assign(GStringUtils::trimAll(sName));
         ret = SIT_ITEM;
         //found
         break;
      }
      else if( ((strItem_i.c_str())[0]) == 'T' &&
               ((strItem_i.c_str())[1]) == 'O' &&
               ((strItem_i.c_str())[2]) == 'T' &&
               ((strItem_i.c_str())[3]) == 'A' &&
               ((strItem_i.c_str())[4]) == 'L' &&
               ((strItem_i.c_str())[5]) == ' ' &&
               ((strItem_i.c_str())[6]) == 'I' &&
               ((strItem_i.c_str())[7]) == 'T' &&
               ((strItem_i.c_str())[8]) == 'E' &&
               ((strItem_i.c_str())[9]) == 'M' )
      {
          std::string sName;
          sName.assign(&((strItem_i.c_str())[10]));
          typeName.assign(GStringUtils::trimAll(sName));
          ret = SIT_TOTAL;
          break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
float ScaleManager::getItemDataNet(std::list<std::string>& lstItems, ScaleUnit& itemUnit)
{
   float ret = 0;
   for( std::list<std::string>::iterator it = lstItems.begin(); it != lstItems.end(); ++it )
   {
      if( (*it).length() < BYTES_FIXED_16 )
      {
         continue;
      }

      std::string strItem_i;
      strItem_i.assign(GStringUtils::trimAll(*it));

      if( ((strItem_i.c_str())[0]) == 'N' &&
          ((strItem_i.c_str())[1]) == 'W' )
      {
         char c1 = (char)((strItem_i.c_str())[strItem_i.length()-1]);
         char c2 = (char)((strItem_i.c_str())[strItem_i.length()-2]);
         char c3 = (char)((strItem_i.c_str())[strItem_i.length()-3]);
         char c4 = (char)((strItem_i.c_str())[strItem_i.length()-4]);

         itemUnit = SUN_NONE;
         size_t post1 = 2;
         size_t post2 = 0;
         if( (c1 == 'z' || c1 == 'Z') &&
             (c2 == 'o' || c2 == 'O') &&
             (c3 == 'b' || c3 == 'B') &&
             (c4 == 'l' || c4 == 'L') )
         {
            itemUnit = SUN_LBOZ;
            post2 = strItem_i.length()-4;
         }
         else if( (c1 == 'g' || c1 == 'G') &&
                  (c2 == 'k' || c2 == 'K') &&
                  (c3 == 'h' || c3 == 'H') )
         {
            itemUnit = SUN_HKG;
            post2 = strItem_i.length()-3;
         }
         else if( (c1 == 'b' || c1 == 'B') &&
                  (c2 == 'l' || c2 == 'L') )
         {
            itemUnit = SUN_LB;
            post2 = strItem_i.length()-2;
         }
         else if( (c1 == 'g' || c1 == 'G') &&
                  (c2 == 'k' || c2 == 'K') )
         {
            itemUnit = SUN_KG;
            post2 = strItem_i.length()-2;
         }
         else if( c1 == 'g' || c1 == 'G' )
         {
            itemUnit = SUN_G;
            post2 = strItem_i.length()-1;
         }

         if( itemUnit != SUN_NONE )
         {
            std::string sValue;
            sValue.assign(GStringUtils::trimAll(strItem_i.substr(post1, post2-post1)));
            ret = GStringUtils::str2float(sValue);
            //found
            break;
         }
      }
      else if( ((strItem_i.c_str())[0]) == 'Q' &&
               ((strItem_i.c_str())[1]) == 'T' &&
               ((strItem_i.c_str())[2]) == 'Y' )
      {
         char c1 = (char)((strItem_i.c_str())[strItem_i.length()-1]);
         char c2 = (char)((strItem_i.c_str())[strItem_i.length()-2]);
         char c3 = (char)((strItem_i.c_str())[strItem_i.length()-3]);

         if( (c1 == 's' || c1 == 'S') &&
             (c2 == 'c' || c2 == 'C') &&
             (c3 == 'p' || c3 == 'P') )
             {
                itemUnit = SUN_PCS;
                std::string sValue;
                sValue.assign(GStringUtils::trimAll(strItem_i.substr(4, strItem_i.length()-3-4)));
                ret = GStringUtils::str2float(sValue);
                //found
                break;
             }
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
float ScaleManager::getItemDataTare(std::list<std::string>& lstItems, ScaleUnit withUnit)
{
   float ret = 0;
   for( std::list<std::string>::iterator it = lstItems.begin(); it != lstItems.end(); ++it )
   {
      if( (*it).length() < BYTES_FIXED_16 )
      {
         continue;
      }

      std::string strItem_i;
      strItem_i.assign(GStringUtils::trimAll(*it));

      if( ((strItem_i.c_str())[0]) == 'T' &&
          ((strItem_i.c_str())[1]) == 'W' )
      {
         size_t post1 = 2;
         size_t post2 = 2;
         if( withUnit == SUN_LBOZ )
         {
            post2 = strItem_i.length()-4;
         }
         else if( withUnit == SUN_HKG )
         {
            post2 = strItem_i.length()-3;
         }
         else if( withUnit == SUN_KG || withUnit == SUN_LB)
         {
            post2 = strItem_i.length()-2;
         }
         else if( withUnit == SUN_G )
         {
            post2 = strItem_i.length()-1;
         }

         std::string sValue;
         sValue.assign(GStringUtils::trimAll(strItem_i.substr(post1, post2-post1)));
         ret = GStringUtils::str2float(sValue);
         //found
         break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
float ScaleManager::getItemUnitPrice(std::list<std::string>& lstItems)
{
   float ret = 0;

   for( std::list<std::string>::iterator it = lstItems.begin(); it != lstItems.end(); ++it )
   {
      if( (*it).length() < BYTES_FIXED_16 )
      {
         continue;
      }

      std::string strItem_i;
      strItem_i.assign(GStringUtils::trimAll(*it));

      if( ((strItem_i.c_str())[0]) == 'U' &&
          ((strItem_i.c_str())[1]) == '/' &&
          ((strItem_i.c_str())[2]) == 'P' )
      {
          size_t post1 = 3;
          size_t post2 = 3;
          for( size_t i = strItem_i.length(); i > 3; --i )
          {
             if( ((strItem_i.c_str())[i]) == '/' )
             {
                post2 = i;
             }
          }

          std::string sValue;
          sValue.assign(GStringUtils::trimAll(strItem_i.substr(post1, post2-post1)));
          ret = GStringUtils::str2float(sValue);
          //found
          break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
float ScaleManager::getItemTotalPrice(std::list<std::string>& lstItems)
{
   float ret = 0;
   for( std::list<std::string>::iterator it = lstItems.begin(); it != lstItems.end(); ++it )
   {
      if( (*it).length() < BYTES_FIXED_16 )
      {
         continue;
      }

      std::string strItem_i;
      strItem_i.assign(GStringUtils::trimAll(*it));

      if( ((strItem_i.c_str())[0]) == 'T' &&
          ((strItem_i.c_str())[1]) == 'P' )
      {
          std::string sValue;
          sValue.assign(GStringUtils::trimAll(strItem_i.substr(2)));
          ret = GStringUtils::str2float(sValue);
          //found
          break;
      }
      else if( ((strItem_i.c_str())[0]) == 'A' &&
               ((strItem_i.c_str())[1]) == 'M' &&
               ((strItem_i.c_str())[2]) == 'O' &&
               ((strItem_i.c_str())[3]) == 'U' &&
               ((strItem_i.c_str())[4]) == 'N' &&
               ((strItem_i.c_str())[5]) == 'T' )
      {
           std::string sValue;
           sValue.assign(GStringUtils::trimAll(strItem_i.substr(6)));
           ret = GStringUtils::str2float(sValue);
           //found
           break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
float ScaleManager::getItemPay(std::list<std::string>& lstItems)
{
   float ret = 0;
   for( std::list<std::string>::iterator it = lstItems.begin(); it != lstItems.end(); ++it )
   {
      if( (*it).length() < BYTES_FIXED_16 )
      {
         continue;
      }

      std::string strItem_i;
      strItem_i.assign(GStringUtils::trimAll(*it));

      if( ((strItem_i.c_str())[0]) == 'P' &&
          ((strItem_i.c_str())[1]) == 'A' &&
          ((strItem_i.c_str())[2]) == 'Y' )
      {
          std::string sValue;
          sValue.assign(GStringUtils::trimAll(strItem_i.substr(3)));
          ret = GStringUtils::str2float(sValue);
          //found
          break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
float ScaleManager::getItemChange(std::list<std::string>& lstItems)
{
   float ret = 0;
   for( std::list<std::string>::iterator it = lstItems.begin(); it != lstItems.end(); ++it )
   {
      if( (*it).length() < BYTES_FIXED_16 )
      {
         continue;
      }

      std::string strItem_i;
      strItem_i.assign(GStringUtils::trimAll(*it));

      if( ((strItem_i.c_str())[0]) == 'C' &&
          ((strItem_i.c_str())[1]) == 'H' &&
          ((strItem_i.c_str())[2]) == 'A' &&
          ((strItem_i.c_str())[3]) == 'N' &&
          ((strItem_i.c_str())[4]) == 'G' &&
          ((strItem_i.c_str())[5]) == 'E' )
      {
          std::string sValue;
          sValue.assign(GStringUtils::trimAll(strItem_i.substr(6)));
          ret = GStringUtils::str2float(sValue);
          //found
          break;
      }
   }
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void ScaleManager::setPriceLookup(const std::string productCode)
{
   if( RateManager::getInstance().getProductInfo(productCode, m_lastProductInfo) )
   {
      //product is selected from the barcode reader, uses PLU ESCALE_NUM_KEY temporary
      _DTraceDebug("Found Product Code: %s is %s", productCode.c_str(), m_lastProductInfo.code);
      updatePrice(ESCALE_NUM_KEY, m_lastProductInfo.price);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ScaleManager::updateRateTable()
{
   m_isUpdatingRateTable = true;

   for( int i = 0; i < ESCALE_NUM_KEY; ++i )
   {
      ProductInfo productInfo;
      U32 keyId = (U32)(i+1);
      if( RateManager::getInstance().getProductInfo(keyId, productInfo) )
      {
         updatePrice(productInfo.id, productInfo.price);
      }
      else
      {
         //clear PLU at the keyID
         updatePrice(keyId, 0.0);
      }
   }

   m_isUpdatingRateTable = false;
}

//////////////////////////////////////////////////////////////////////////////////////
void ScaleManager::updatePrice(U32 keyId, float price)
{
   U8 byteArray[13];

   if( keyId < 1 || keyId > ESCALE_NUM_KEY )
   {
      //invalid keyId, do nothing
      _DTraceDebug("Invalid keyId, do nothing");
      return;
   }

   _DTraceDebug("Key = %02d, Price = %.2f", keyId, price);

   //prefix
   byteArray[0] = 0x39; //9
   byteArray[1] = 0x38; //8

   //item
   if( keyId < 10 )
   {
      byteArray[2] = 0x30; //0
   }
   else
   {
      byteArray[2] = 0x30 + (U8)(keyId/10);
   }

   byteArray[3] = 0x30 + (U8)(keyId%10);

   //price
   //the maximum price is the number has 5 digit number, including decimal point
   //get the natural parts
   U32 u32Price = (U32)(price*100);
   U32 naturalParts = (U32)(u32Price/100);
   U32 decimalParts = (U32)(u32Price%100);

   while( naturalParts >= 100000 )
   {
      //more than 5 digits, trim the lowest value
      naturalParts = naturalParts/10;
   }

   if( naturalParts >= 100000 )
   {
      //more than 5 digits, consider the wrong price
      return;
   }

   if( naturalParts >= 10000 )
   {
      byteArray[4] = 0x30 + (U8)( naturalParts / 10000);
      byteArray[5] = 0x30 + (U8)((naturalParts % 10000) / 1000);
      byteArray[6] = 0x30 + (U8)((naturalParts %  1000) /  100);
      byteArray[7] = 0x30 + (U8)((naturalParts %   100) /   10);
      byteArray[8] = 0x30 + (U8)( naturalParts %    10);
      byteArray[9] = 0x31; //there is no decimal part
   }
   else if( price >= 1000 )
   {
      byteArray[4] = 0x30 + (U8)( naturalParts / 1000);
      byteArray[5] = 0x30 + (U8)((naturalParts % 1000) / 100);
      byteArray[6] = 0x30 + (U8)((naturalParts %  100) /  10);
      byteArray[7] = 0x30 + (U8)( naturalParts %   10);
      byteArray[8] = 0x30 + (U8)( decimalParts / 10);
      byteArray[9] = 0x32; //there is 1 digit in decimal part
   }
   else
   {
      byteArray[4] = 0x30 + (U8)( naturalParts / 100);
      byteArray[5] = 0x30 + (U8)((naturalParts % 100) / 10);
      byteArray[6] = 0x30 + (U8)( naturalParts % 10 );
      byteArray[7] = 0x30 + (U8)( decimalParts / 10);
      byteArray[8] = 0x30 + (U8)( decimalParts % 10);
      byteArray[9] = 0x33; //there are 2 digits in decimal part
   }

   //suffix
   byteArray[10] = 0x39;
   byteArray[11] = 0x0D;
   byteArray[12] = 0x0A;

   U32 numWritten = 0;
   writeBytes(byteArray, 13, numWritten);
   _DTraceDebug("numWritten = %d, %s", numWritten, (char*)byteArray);
   Task_Delay(ST_GetClocksPerSecond()/20);
}

//////////////////////////////////////////////////////////////////////////////////////
void ScaleManager::traceData(ScaleDataType dataType)
{
   if( dataType == SDT_FORMAT1 )
   {
      std::string debugInfo;
      debugInfo.assign(header2Str(m_scaleWeightData.header1));
      debugInfo.append("-");
      debugInfo.append(header2Str(m_scaleWeightData.header2));
      debugInfo.append(": ");
      debugInfo.append(data2Str(m_scaleWeightData.data1));
      debugInfo.append(" ");
      debugInfo.append(unit2Str(m_scaleWeightData.unit));
      _DTraceDebug(debugInfo.c_str());
   }
   else if( dataType == SDT_FORMAT2 )
   {
      if( m_scaleItemDatas.size() > 0 )
      {
         ScaleItemData lastItem = m_scaleItemDatas.back();
         _DTraceDebug("Item   Unit Price   Quantity   Amount");
         _DTraceDebug("%s     %s/%s   %s     %s",
                      lastItem.typeName.c_str(),
                      data2Str(lastItem.unitPrice).c_str(),
                      unit2Str(lastItem.unit).c_str(),
                      data2Str(lastItem.dataNet).c_str(),
                      data2Str(lastItem.totalPrice).c_str() );
      }
   }
   else if( dataType == SDT_FORMAT2_MC )
   {
      _DTraceDebug("Item   Unit Price   Quantity   Amount");
      for( std::list<ScaleItemData>::iterator it = m_scaleItemDatas.begin(); it != m_scaleItemDatas.end(); ++it )
      {
         if( (*it).type == SIT_ITEM )
         {
            _DTraceDebug("%s     %s/%s   %s     %s",
                      (*it).typeName.c_str(),
                      data2Str((*it).unitPrice).c_str(),
                      unit2Str((*it).unit).c_str(),
                      data2Str((*it).dataNet).c_str(),
                      data2Str((*it).totalPrice).c_str() );
         }
         else if( (*it).type == SIT_TOTAL )
         {
            _DTraceDebug("----------------------");
            _DTraceDebug("Total Item:         %s", (*it).typeName.c_str());
            _DTraceDebug("Amount:             %s", data2Str((*it).totalPrice).c_str());
            if( (*it).pay > 0 )
            {
               _DTraceDebug("Pay:                %s", data2Str((*it).pay).c_str());
               _DTraceDebug("Return:             %s", data2Str((*it).change).c_str());
            }
         }
      }
   }
   else if( dataType == SDT_FORMAT2_DEL )
   {
      _DTraceDebug("DELETED AN ITEM !");
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
