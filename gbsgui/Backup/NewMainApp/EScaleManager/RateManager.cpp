//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: RateManager.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#include "GStringUtils.hpp"
#include "GSystem.hpp"
#include "RateManager.hpp"
#include "JsonParser.hpp"
#include "GUserSettings.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

RateManager::RateManager()
{
   m_isInternalProccessing = false;
}

//////////////////////////////////////////////////////////////////////////////////////
RateManager::~RateManager()
{
   m_isInternalProccessing = false;
}

//////////////////////////////////////////////////////////////////////////////////////
RateManager& RateManager::getInstance()
{
   static RateManager instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
bool RateManager::init()
{
}

//////////////////////////////////////////////////////////////////////////////////////
bool RateManager::deinit()
{
}

//////////////////////////////////////////////////////////////////////////////////////
void RateManager::setPendingRateTable(const std::list<std::string>& rateTableIdList)
{
   m_pendingRateTableIds.clear();
   m_pendingRateTableIds = rateTableIdList;
}

//////////////////////////////////////////////////////////////////////////////////////
void RateManager::removePendingRateTableId(const std::string& rateTableId)
{
   if( m_pendingRateTableIds.empty() )
   {
      return;
   }

   for( std::list<std::string>::iterator it = m_pendingRateTableIds.begin(); it != m_pendingRateTableIds.end(); ++it )
   {
      if( (*it).compare(rateTableId) == 0 )
      {
         it = m_pendingRateTableIds.erase(it);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool RateManager::getNextPendingRateTableId(std::string& rateTableId)
{
   rateTableId = "";

   if( m_pendingRateTableIds.empty() )
   {
      return false;
   }

   rateTableId.assign(*(m_pendingRateTableIds.begin()));
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 RateManager::setLastActiveRateTable()
{
   U32 ret = 0;
   U32 lastRateTableId = _U_.getActiveRateTableId();

   for( int i = 0; i < ESCALE_NUM_RATE_TABLE; ++i )
   {
      if( m_rateTables[i].header.id != 0 && m_rateTables[i].header.id == lastRateTableId )
      {
         m_rateTables[i].header.isActivated = true;
         ret = lastRateTableId;
      }
      else
      {
         m_rateTables[i].header.isActivated = false;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool RateManager::hasActiveRateTableReached()
{
   bool ret = false;

   if( m_isInternalProccessing )
   {
      return ret;
   }

   time_t currentTime = _S_.getCurrentTimeSecond();

   for( int i = 0; i < ESCALE_NUM_RATE_TABLE; ++i )
   {
      if( m_rateTables[i].header.isActivated && (currentTime > m_rateTables[i].header.expiredDate) )
      {
         m_rateTables[i].header.isActivated = false;
         ret = true;
         break;
      }
      else if( currentTime > m_rateTables[i].header.activatedDate )
      {
         time_t timePassed = currentTime - m_rateTables[i].header.activatedDate;
         //during 15 minutes
         if( (timePassed < 901) && (!m_rateTables[i].header.isActivated) )
         {
            ret = true;
            break;
         }
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 RateManager::lookForActiveRateTable()
{
   U32 activeRateTableId = 0;
   time_t currentTime = _S_.getCurrentTimeSecond();
   time_t timeOffsetMin = currentTime;
   RateTableObject foundRateTableObj = ESCALE_NUM_RATE_TABLE;

   if( m_isInternalProccessing )
   {
      return activeRateTableId;
   }

   for( int i = 0; i < ESCALE_NUM_RATE_TABLE; ++i )
   {
      _DTraceDebug("RATE TABLE [%d]: CurDate: %d, ActDate: %d, ExpDate: %d",  i, currentTime, m_rateTables[i].header.activatedDate, m_rateTables[i].header.expiredDate);
      if( m_rateTables[i].header.activatedDate <= currentTime &&
         (m_rateTables[i].header.expiredDate > currentTime || m_rateTables[i].header.expiredDate == 0) )
      {
         if( currentTime - m_rateTables[i].header.activatedDate < timeOffsetMin )
         {
            timeOffsetMin = currentTime - m_rateTables[i].header.activatedDate;
            foundRateTableObj = (RateTableObject)i;
         }
      }
   }

   if( foundRateTableObj < ESCALE_NUM_RATE_TABLE )
   {
      for( int i = 0; i < ESCALE_NUM_RATE_TABLE; ++i )
      {
         if( foundRateTableObj == (RateTableObject)i )
         {
            activeRateTableId = m_rateTables[i].header.id;
            m_rateTables[i].header.isActivated = true;
            _DTraceDebug("FOUND RATE TABLE [%d], RateTableID = %d", i, activeRateTableId);
         }
         else
         {
            m_rateTables[i].header.isActivated = false;
         }
      }
   }

   _DTraceDebug("RATE TABLE [%d], RateTableID = %d", foundRateTableObj, activeRateTableId);

   return activeRateTableId;
}

//////////////////////////////////////////////////////////////////////////////////////
void RateManager::getActivatedRateTable(RateTable* pRateTable)
{
   if( pRateTable == NULL || m_isInternalProccessing )
   {
      _DTraceDebug("MEMORY ERROR");
      return;
   }

   for( int i = 0; i < ESCALE_NUM_RATE_TABLE; ++i )
   {
      if( m_rateTables[i].header.isActivated )
      {
         pRateTable->header.id = m_rateTables[i].header.id;
         pRateTable->header.activatedDate = m_rateTables[i].header.activatedDate;
         pRateTable->header.expiredDate = m_rateTables[i].header.expiredDate;
         pRateTable->header.isActivated = m_rateTables[i].header.isActivated;
         pRateTable->header.totalProduct = m_rateTables[i].header.totalProduct;

         if( pRateTable->header.totalProduct > PRODUCT_NUM_MAX )
         {
            pRateTable->header.totalProduct = PRODUCT_NUM_MAX;
         }

         for( int j = 0; j < pRateTable->header.totalProduct; ++j )
         {
            pRateTable->products[j].id = m_rateTables[i].products[j].id;
            memcpy(pRateTable->products[j].code, m_rateTables[i].products[j].code, PRODUCT_CODE_LEN);
            memcpy(pRateTable->products[j].name, m_rateTables[i].products[j].name, PRODUCT_NAME_LEN);
            memcpy(pRateTable->products[j].unit, m_rateTables[i].products[j].unit, PRODUCT_UNIT_LEN);
            pRateTable->products[j].price = m_rateTables[i].products[j].price;
            pRateTable->products[j].expdate = m_rateTables[i].products[j].expdate;
            pRateTable->products[j].temperature = m_rateTables[i].products[j].temperature;
         }

         break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
RateTableObject RateManager::findOldestRateTable()
{
   RateTableObject rateTableId = ESCALE_NUM_RATE_TABLE;

   if( _S_.getBaseTime() == SYSTEM_TIME_BASE_INIT )
   {
      _DTraceDebug("Stop finding due to SYSTEM_TIME_BASE_INIT, rateTableId = %d", rateTableId);
      return rateTableId;
   }

   U32 theOldestActiveDate = ULONG_MAX;
   for( int i = 0; i < ESCALE_NUM_RATE_TABLE; ++i )
   {
      if( !m_rateTables[i].header.isActivated )
      {
         if( m_rateTables[i].header.expiredDate < _S_.getCurrentTimeSecond() )
         {
            //the rate is already expired
            rateTableId = (RateTableObject)i;
            _DTraceDebug("Found slot %d to be already expired", rateTableId);
            break;
         }
         else if( (U32)m_rateTables[i].header.activatedDate < theOldestActiveDate )
         {
            //find the oldest active date
            theOldestActiveDate = (U32)m_rateTables[i].header.activatedDate;
            rateTableId = (RateTableObject)i;
            _DTraceDebug("Found slot %d may be the oldest", rateTableId);
         }
      }
      else
      {
         _DTraceDebug("RateTable[%d] is activated", i);
      }
   }

   return rateTableId;
}

//////////////////////////////////////////////////////////////////////////////////////
RateTableObject RateManager::addRateTable(const RateTable* pRateTable)
{
   if( m_isInternalProccessing )
   {
      _DTraceDebug("Add rate failed: m_isInternalProccessing" );
      return ESCALE_NUM_RATE_TABLE;
   }

   RateTableObject updatedRateTableId = ESCALE_NUM_RATE_TABLE;

   m_isInternalProccessing = true;

   if( pRateTable == NULL || _S_.getBaseTime() == SYSTEM_TIME_BASE_INIT )
   {
      _DTraceDebug("Add rate failed: pRateTable NULL or SYSTEM_TIME_BASE" );
      m_isInternalProccessing = false;
      return updatedRateTableId;
   }

   if( pRateTable->header.id <= 0 || pRateTable->header.expiredDate <= _S_.getCurrentTimeSecond() )
   {
      _DTraceDebug("Add rate failed: id = %d, expiredDate = %d", pRateTable->header.id, pRateTable->header.expiredDate );
      m_isInternalProccessing = false;
      return updatedRateTableId;
   }

   updatedRateTableId = findOldestRateTable();

   if( updatedRateTableId >= ESCALE_RATE_TABLE_1 && updatedRateTableId < ESCALE_NUM_RATE_TABLE )
   {
      m_rateTables[updatedRateTableId].header.id = pRateTable->header.id;
      m_rateTables[updatedRateTableId].header.activatedDate = pRateTable->header.activatedDate;
      m_rateTables[updatedRateTableId].header.expiredDate = pRateTable->header.expiredDate;
      m_rateTables[updatedRateTableId].header.isActivated = false;
      if( pRateTable->header.totalProduct > PRODUCT_NUM_MAX )
      {
         m_rateTables[updatedRateTableId].header.totalProduct = PRODUCT_NUM_MAX;
      }
      else
      {
         m_rateTables[updatedRateTableId].header.totalProduct = pRateTable->header.totalProduct;
      }

      for( U32 i = 0; i < m_rateTables[updatedRateTableId].header.totalProduct; ++i )
      {
         m_rateTables[updatedRateTableId].products[i].id = pRateTable->products[i].id;
         memcpy(m_rateTables[updatedRateTableId].products[i].code, pRateTable->products[i].code, PRODUCT_CODE_LEN);
         memcpy(m_rateTables[updatedRateTableId].products[i].name, pRateTable->products[i].name, PRODUCT_NAME_LEN);
         memcpy(m_rateTables[updatedRateTableId].products[i].unit, pRateTable->products[i].unit, PRODUCT_UNIT_LEN);
         m_rateTables[updatedRateTableId].products[i].price = pRateTable->products[i].price;
         m_rateTables[updatedRateTableId].products[i].expdate = pRateTable->products[i].expdate;
         m_rateTables[updatedRateTableId].products[i].temperature = pRateTable->products[i].temperature;
      }
   }

   m_isInternalProccessing = false;

   return updatedRateTableId;
}

//////////////////////////////////////////////////////////////////////////////////////
bool RateManager::getProductInfo(const U32 productId, ProductInfo& oProductInfo)
{
   bool ret = false;
   int curRateTableId = -1;

   if( m_isInternalProccessing )
   {
      return ret;
   }

   for( int i = 0; i < ESCALE_NUM_RATE_TABLE; ++i )
   {
      if( m_rateTables[i].header.isActivated )
      {
         curRateTableId = i;
         break;
      }
   }

   if( curRateTableId < 0 )
   {
      return ret;
   }

   int totalProducts = m_rateTables[curRateTableId].header.totalProduct;
   if( totalProducts > PRODUCT_NUM_MAX )
   {
      totalProducts = PRODUCT_NUM_MAX;
   }

   for( int j = 0; j < totalProducts; ++j )
   {
      if( m_rateTables[curRateTableId].products[j].id == productId )
      {
         oProductInfo.id = m_rateTables[curRateTableId].products[j].id;
         memcpy(oProductInfo.code, m_rateTables[curRateTableId].products[j].code, PRODUCT_CODE_LEN);
         memcpy(oProductInfo.name, m_rateTables[curRateTableId].products[j].name, PRODUCT_NAME_LEN);
         memcpy(oProductInfo.unit, m_rateTables[curRateTableId].products[j].unit, PRODUCT_UNIT_LEN);
         oProductInfo.price = m_rateTables[curRateTableId].products[j].price;
         oProductInfo.expdate = m_rateTables[curRateTableId].products[j].expdate;
         oProductInfo.temperature = m_rateTables[curRateTableId].products[j].temperature;
         _DTraceDebug("Found product code: %s, name: %s", oProductInfo.code, oProductInfo.name);
         ret = true;
         break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool RateManager::getProductInfo(const char* productCode, ProductInfo& oProductInfo)
{
   bool ret = false;
   int curRateTableId = -1;

   if( m_isInternalProccessing )
   {
      return ret;
   }

   for( int i = 0; i < ESCALE_NUM_RATE_TABLE; ++i )
   {
      if( m_rateTables[i].header.isActivated )
      {
         curRateTableId = i;
         break;
      }
   }

   if( curRateTableId < 0 )
   {
      return ret;
   }

   int totalProducts = m_rateTables[curRateTableId].header.totalProduct;
   if( totalProducts > PRODUCT_NUM_MAX )
   {
      totalProducts = PRODUCT_NUM_MAX;
   }

   for( int j = 0; j < totalProducts; ++j )
   {
      if( strcmp(m_rateTables[curRateTableId].products[j].code, productCode) == 0 )
      {
         oProductInfo.id = m_rateTables[curRateTableId].products[j].id;
         memcpy(oProductInfo.code, m_rateTables[curRateTableId].products[j].code, PRODUCT_CODE_LEN);
         memcpy(oProductInfo.name, m_rateTables[curRateTableId].products[j].name, PRODUCT_NAME_LEN);
         memcpy(oProductInfo.unit, m_rateTables[curRateTableId].products[j].unit, PRODUCT_UNIT_LEN);
         oProductInfo.price = m_rateTables[curRateTableId].products[j].price;
         oProductInfo.expdate = m_rateTables[curRateTableId].products[j].expdate;
         oProductInfo.temperature = m_rateTables[curRateTableId].products[j].temperature;
         _DTraceDebug("Found product code: %s, name: %s", oProductInfo.code, oProductInfo.name);
         ret = true;
         break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool RateManager::getProductInfo(const std::string productCode, ProductInfo& oProductInfo)
{
   return getProductInfo(productCode.c_str(), oProductInfo);
}

//////////////////////////////////////////////////////////////////////////////////////
bool RateManager::getRateTableInfo(const int slotId, RateTableHeader& rateTableHeader)
{
   bool ret = false;

   if( slotId >= 0 && slotId < (int)ESCALE_NUM_RATE_TABLE )
   {
      rateTableHeader.id = m_rateTables[slotId].header.id;
      rateTableHeader.activatedDate = m_rateTables[slotId].header.activatedDate;
      rateTableHeader.expiredDate = m_rateTables[slotId].header.expiredDate;
      rateTableHeader.isActivated = m_rateTables[slotId].header.isActivated;
      rateTableHeader.totalProduct = m_rateTables[slotId].header.totalProduct;
      ret = true;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void RateManager::serializeData(const RateTableObject rateTable, std::string& outData)
{
   Json::Value nodeRoot(Json::objectValue);
   Json::Value nodeItems(Json::arrayValue);
   Json::FastWriter jsWriter;

   if( rateTable >= ESCALE_NUM_RATE_TABLE || m_isInternalProccessing )
   {
      outData = "";
      return;
   }

   nodeRoot[TAG_RATE_TABLE_ID] = m_rateTables[rateTable].header.id;
   U32 u32DataTypeValue = (U32)m_rateTables[rateTable].header.activatedDate;
   nodeRoot[TAG_RATE_TABLE_ACTIVEDATE] = u32DataTypeValue;
   u32DataTypeValue = (U32)m_rateTables[rateTable].header.expiredDate;
   nodeRoot[TAG_RATE_TABLE_EXPIREDATE] = u32DataTypeValue;
   nodeRoot[TAG_RATE_TABLE_ACTIVATED] = m_rateTables[rateTable].header.isActivated;
   nodeRoot[TAG_RATE_TABLE_TOTALITEM] = m_rateTables[rateTable].header.totalProduct;

   for( int i = 0; i < m_rateTables[rateTable].header.totalProduct; ++i )
   {
      Json::Value nodeItem_i(Json::objectValue);
      nodeItem_i[TAG_PRODUCT_NUMBER] = m_rateTables[rateTable].products[i].id;
      nodeItem_i[TAG_PRODUCT_CODE] = m_rateTables[rateTable].products[i].code;
      nodeItem_i[TAG_PRODUCT_NAME] = m_rateTables[rateTable].products[i].name;
      nodeItem_i[TAG_PRODUCT_UNIT] = m_rateTables[rateTable].products[i].unit;
      nodeItem_i[TAG_PRODUCT_PRICE] = (double)m_rateTables[rateTable].products[i].price;
      nodeItem_i[TAG_PRODUCT_EXPDAY] = (double)m_rateTables[rateTable].products[i].expdate;
      nodeItem_i[TAG_PRODUCT_TEMPERATURE] = (double)m_rateTables[rateTable].products[i].temperature;
      nodeItems.append(nodeItem_i);
   }
   nodeRoot[TAG_RATE_TABLE_ITEMS] = nodeItems;

   outData.assign(jsWriter.write(nodeRoot));
}

//////////////////////////////////////////////////////////////////////////////////////
bool RateManager::unserializeData(const RateTableObject rateTable, const U8* data)
{
   bool canContinue = false;

   if( data == NULL || m_isInternalProccessing )
   {
      return canContinue;
   }

   m_isInternalProccessing = true;

   JsonParser jsParser;
   Json::Value nodeRoot;
   Json::Features jsFeatures;
   Json::Reader jsReader(jsFeatures);

   if( jsReader.parse((char*)data, nodeRoot) )
   {
      if( nodeRoot.type() != Json::nullValue )
      {
         canContinue = true;
      }
   }

   if( !canContinue )
   {
      m_isInternalProccessing = false;
      return canContinue;
   }

   m_rateTables[rateTable].header.id = jsParser.getUintValueNode(nodeRoot, TAG_RATE_TABLE_ID);
   m_rateTables[rateTable].header.activatedDate = jsParser.getUintValueNode(nodeRoot, TAG_RATE_TABLE_ACTIVEDATE);
   m_rateTables[rateTable].header.expiredDate = jsParser.getUintValueNode(nodeRoot, TAG_RATE_TABLE_EXPIREDATE);
   m_rateTables[rateTable].header.isActivated = jsParser.getBooleanValueNode(nodeRoot, TAG_RATE_TABLE_ACTIVATED);
   m_rateTables[rateTable].header.totalProduct = jsParser.getUintValueNode(nodeRoot, TAG_RATE_TABLE_TOTALITEM);

   _DTraceDebug("RATE TABLE id[%d], totalProduct = %d", m_rateTables[rateTable].header.id, m_rateTables[rateTable].header.totalProduct);

   if( m_rateTables[rateTable].header.totalProduct <= 0 )
   {
      _DTraceDebug("failed, totalProduct = 0");
      m_isInternalProccessing = false;
      canContinue = false;
      return canContinue;
   }

   Json::Value defaultValue;
   Json::Value nodeItems;
   nodeItems = nodeRoot.get(TAG_RATE_TABLE_ITEMS, defaultValue);

   if( nodeItems.type() == Json::arrayValue && m_rateTables[rateTable].header.totalProduct == nodeItems.size() )
   {
      if( m_rateTables[rateTable].header.totalProduct > PRODUCT_NUM_MAX )
      {
         //truncate number of product
         m_rateTables[rateTable].header.totalProduct = PRODUCT_NUM_MAX;
      }

      for( int i = 0; i < m_rateTables[rateTable].header.totalProduct; ++i )
      {
         std::string sStringValue;
         int iLength;
         m_rateTables[rateTable].products[i].id = jsParser.getUintValueNode(nodeItems[i], TAG_PRODUCT_NUMBER);
         m_rateTables[rateTable].products[i].price = jsParser.getRealValueNode(nodeItems[i], TAG_PRODUCT_PRICE);
         m_rateTables[rateTable].products[i].expdate = jsParser.getRealValueNode(nodeItems[i], TAG_PRODUCT_EXPDAY);
         m_rateTables[rateTable].products[i].temperature = jsParser.getRealValueNode(nodeItems[i], TAG_PRODUCT_TEMPERATURE);
         sStringValue = jsParser.getStrValueNode(nodeItems[i], TAG_PRODUCT_CODE);
         iLength = PRODUCT_CODE_LEN-1;
         if( iLength > sStringValue.length() )
         {
            iLength = sStringValue.length();
         }
         memcpy(m_rateTables[rateTable].products[i].code, sStringValue.c_str(), iLength);
         m_rateTables[rateTable].products[i].code[iLength] = 0;

         sStringValue = jsParser.getStrValueNode(nodeItems[i], TAG_PRODUCT_NAME);
         iLength = PRODUCT_NAME_LEN-1;
         if( iLength > sStringValue.length() )
         {
            iLength = sStringValue.length();
         }
         memcpy(m_rateTables[rateTable].products[i].name, sStringValue.c_str(), iLength);
         m_rateTables[rateTable].products[i].name[iLength] = 0;

         sStringValue = jsParser.getStrValueNode(nodeItems[i], TAG_PRODUCT_UNIT);
         iLength = PRODUCT_UNIT_LEN-1;
         if( iLength > sStringValue.length() )
         {
            iLength = sStringValue.length();
         }
         memcpy(m_rateTables[rateTable].products[i].unit, sStringValue.c_str(), iLength);
         m_rateTables[rateTable].products[i].unit[iLength] = 0;
      }
   }

   nodeRoot.clear();
   m_isInternalProccessing = false;

   return canContinue;
}

//////////////////////////////////////////////////////////////////////////////////////
void RateManager::createDemoRateTable()
{
   for( int i = 0; i < ESCALE_NUM_RATE_TABLE; ++i )
   {
      m_rateTables[i].header.isActivated = false;
      m_rateTables[i].header.totalProduct = 0;
   }

   m_rateTables[0].header.id = 1;
   m_rateTables[0].header.isActivated = true;
   m_rateTables[0].header.totalProduct = 5;
   m_rateTables[0].header.activatedDate = 1401234567;
   m_rateTables[0].header.expiredDate = 1511234567;

   for( int i = 0; i < m_rateTables[0].header.totalProduct; ++i )
   {
      m_rateTables[0].products[i].id = i+1;
      memcpy(m_rateTables[0].products[i].code, "GBS00123456", 11);
      memcpy(m_rateTables[0].products[i].name, "Thit Nac Vai", 12);
      memcpy(m_rateTables[0].products[i].unit, "kg", 2);
      m_rateTables[0].products[i].price = 101.00;
      m_rateTables[0].products[i].expdate = 5.0;
      m_rateTables[0].products[i].temperature = 11.0;
   }
}

//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
