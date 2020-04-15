//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ConnDevice.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "ConnDevice.hpp"
#include "GStringUtils.hpp"
#include "SaveDataToFlash.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////
ConnDevice::ConnDevice() : SerialPort(SPN_NONE)
{
   m_deviceId = "";
   m_RAWDATABUFFER.clear();
   m_deviceStage = CDS_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
ConnDevice::ConnDevice(SerialPortName portName) : SerialPort(portName)
{
   m_deviceId = "";
   m_RAWDATABUFFER.clear();
   m_deviceStage = CDS_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
ConnDevice::~ConnDevice()
{
   m_RAWDATABUFFER.clear();
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice::init()
{
   //Do nothing, will be implemented in derived class
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice::deinit()
{
   //Do nothing, will be implemented in derived class
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice::isReady()
{
   //Do nothing, will be implemented in derived class
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnDevice::encodeAtParams(std::string& ioParams)
{
   size_t foundPos = std::string::npos;
   //_DTraceDebug("iPrams: %s", ioParams.c_str());

   for( U8 controlChar = 0; controlChar < 32; controlChar++ )
   {
      foundPos = ioParams.find((char)controlChar);
      while( foundPos != std::string::npos )
      {
        //if( controlChar == ASCII_CR || controlChar == ASCII_LF )
         ioParams.replace(foundPos, 1, " ");  //%0x0D
         foundPos = ioParams.find((char)controlChar);
      }
   }

   //_DTraceDebug("oPrams: %s", ioParams.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice::writeCommand(std::string command, std::list<CmdParameter>& params, std::string& oResponseBuff, U32 timeOut)
{
   bool ret = false;
   std::string fullCommand = "";
   fullCommand.assign(command);

   if( params.size() > 0 )
   {
      fullCommand.append("=");

      for( std::list<CmdParameter>::iterator it = params.begin(); it != params.end(); ++it )
      {
         if( it != params.begin() )
         {
            fullCommand.append(",");
         }

         if( (*it).isNumeric )
         {
            fullCommand.append((*it).value);
         }
         else
         {
            fullCommand.append(1, 0x22);
            fullCommand.append((*it).value);
            fullCommand.append(1, 0x22);
         }
      }
   }

   encodeAtParams(fullCommand);

   _DTraceDebug(fullCommand.c_str());
   U32 numWritten = 0;
   writeBytes((U8*)(fullCommand.c_str()), fullCommand.length(), numWritten);
   U8 EndCommad[2] = {ASCII_CR, ASCII_LF};
   writeBytes(EndCommad, 2, numWritten);
   Task_Delay(10);

   timeOut += 1;

   for( U32 i = 0; i < timeOut; ++i )
   {
      U32 numRead = 0;
      U8 pReadBuff[SERIAL_BUFFER_SIZE];

      Task_Delay(ST_GetClocksPerSecond()/10);

      readBytes(pReadBuff, numRead);

      if( (numRead > 0) && (numRead < SERIAL_BUFFER_SIZE) )
      {
         _DTraceDebug((char*)pReadBuff);
         oResponseBuff.assign((char*)pReadBuff);

         if( oResponseBuff.find("\r\nOK\r\n") != std::string::npos )
         {
            ret = true;
            break;
         }
         else if( oResponseBuff.find("\r\nERROR\r\n") != std::string::npos ||
                  oResponseBuff.find("\r\nFAIL\r\n")  != std::string::npos)
         {
            break;
         }
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnDevice::readBuffer(std::string& oResponseBuff)
{
   U32 numRead = 0;
   U8 pReadBuff[SERIAL_BUFFER_SIZE];
   oResponseBuff.clear();

   readBytes(pReadBuff, numRead);

   if( (numRead > 0) && (numRead < SERIAL_BUFFER_SIZE) )
   {
      //_DTraceDebug((char*)pReadBuff);
      oResponseBuff.assign((char*)pReadBuff);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnDevice::buildHttpParams(std::string& outHttpParams, ConnHttpService httpService, ConnHttpRequestType httpRequestType, std::list<std::string>& params)
{
   std::list<std::string>::iterator it = params.end();
   outHttpParams = "";

   if( !params.empty() )
   {
      it = params.begin();
   }
   else
   {
      return;
   }

   switch( httpService )
   {
   case CHS_DEVICE_REGISTER:
      if( params.size() >= 3 )
      {
         GStringUtils::format(outHttpParams, SERVICE_DEVICE_REGISTER_PARAMS, (*it).c_str(), (*(++it)).c_str(), (*(++it)).c_str());
      }
      break;
   case CHS_SUBMIT_DATA:
      if( params.size() >= 7 )
      {
         GStringUtils::format(outHttpParams, SERVICE_SUBMIT_DATA_PARAMS, (*it).c_str(), (*(++it)).c_str(), (*(++it)).c_str(), (*(++it)).c_str(), (*(++it)).c_str(), (*(++it)).c_str(), (*(++it)).c_str());
      }
      break;
   case CHS_RATE_TABLE:
      if( params.size() >= 3 )
      {
         GStringUtils::format(outHttpParams, SERVICE_UPDATE_RATETABLE_PARAMS, (*it).c_str(), (*(++it)).c_str(), (*(++it)).c_str());
      }
      break;
   case CHS_UPDATE_DATETIME:
      if( params.size() >= 1 )
      {
         GStringUtils::format(outHttpParams, SERVICE_UPDATE_DATETIME_PARAMS, (*it).c_str());
      }
      break;
   case CHS_SUBMIT_DATA_VNPOST:
      if( params.size() >= 3 )
      {
        if(httpRequestType == HTTP_REQ_POST)
        {
            GStringUtils::format(outHttpParams, SERVICE_SUBMIT_DATA_POST_VNPOST_PARAMS, (*it).c_str(), (*(++it)).c_str(), (*(++it)).c_str());
        }
        else
        {
          GStringUtils::format(outHttpParams, SERVICE_SUBMIT_DATA_GET_VNPOST_PARAMS, (*it).c_str(), (*(++it)).c_str(), (*(++it)).c_str());
        }
      }
      break;
   default:
      break;
   }

   _DTraceDebug("%s", outHttpParams.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnDevice::getHttpServiceName(std::string& outServiceName, ConnHttpService httpService)
{
   outServiceName = "";

   switch( httpService )
   {
   case CHS_DEVICE_REGISTER:
      outServiceName.assign(SERVICE_DEVICE_REGISTER_NAME);
      break;
   case CHS_SUBMIT_DATA:
      outServiceName.assign(SERVICE_SUBMIT_DATA_NAME);
      break;
   case CHS_RATE_TABLE:
      outServiceName.assign(SERVICE_UPDATE_RATETABLE_NAME);
      break;
   case CHS_UPDATE_DATETIME:
      outServiceName.assign(SERVICE_UPDATE_DATETIME_NAME);
      break;
   case CHS_SUBMIT_DATA_VNPOST:
         outServiceName.assign(SERVICE_SUBMIT_DATA_VNPOST_NAME);
         break;
   default:
      break;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnDevice::setDeviceId(const std::string& deviceId)
{
   if( deviceId.empty() )
   {
      return;
   }
   //char* deviceIdInMD5 = NULL;
   //deviceIdInMD5 = GStringUtils::md5((U8*)deviceId.c_str(), deviceId.length());
   m_deviceId.assign(deviceId);
   //if( deviceIdInMD5 != NULL )
   //{
   //   delete deviceIdInMD5;
   //   deviceIdInMD5 = NULL;
   //}
}

//temporary
//////////////////////////////////////////////////////////////////////////////////////
void ConnDevice::appendBufferIn(std::string& sBuffer)
{
   //printf("%s\n\r", sBuffer.c_str());
   //remove data before RESP_JSON_START
   size_t pos = sBuffer.find(RESP_JSON_START);
   if( pos != std::string::npos )
   {
      sBuffer = sBuffer.substr(pos);
   }

   //remove data after RESP_JSON_END
   pos = sBuffer.find(RESP_JSON_END);
   if( pos != std::string::npos )
   {
      sBuffer = sBuffer.substr(0, pos + RESP_JSON_END_LEN);
   }

   GStringUtils::remove(sBuffer, "FAIL\r\n");
   GStringUtils::remove(sBuffer, "ERROR\r\n");
   GStringUtils::remove(sBuffer, "\r");
   GStringUtils::remove(sBuffer, "\n");

   m_RAWDATABUFFER.append(sBuffer);
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice::parseBufferIn()
{
   bool ret = false;
   JsonParser jsParser;
   Json::Value rootNode;

   /////////// remove "+IPD,4,xxxx:" ///////////
   size_t pos1 = 0;
   size_t pos2 = 0;
   std::string sIpdPattern;
   GStringUtils::format(sIpdPattern, "+IPD,%d,", MAX_CONNECTION_ID);
   pos1 = m_RAWDATABUFFER.find(sIpdPattern);
   while( pos1 != std::string::npos )
   {
      pos2 = m_RAWDATABUFFER.find(':', pos1);
      if( pos2 > pos1 )
      {
         pos2 += 1;
         m_RAWDATABUFFER.erase(pos1, pos2-pos1);
      }
      else
      {
         m_RAWDATABUFFER.erase(pos1, sIpdPattern.length()+5); //as default
      }
      pos1 = m_RAWDATABUFFER.find(sIpdPattern);
   }
   /////////// remove "+IPD,4,xxxx:" ///////////

   if( jsParser.getJsonObject(m_RAWDATABUFFER, rootNode) )
   {
      if( rootNode.isObject() )
      {
         //Try with a rate table
         RateTable* pNewRateTable = NULL;
         pNewRateTable = new RateTable();

         if( pNewRateTable == NULL )
         {
            _DTraceDebug("Parse failed due to memory...");
            clearBufferIn();
            rootNode.clear();
            return ret;
         }

         pNewRateTable->header.id = jsParser.getUintValueNode(rootNode, TAG_RATE_TABLE_ID);
         pNewRateTable->header.activatedDate = jsParser.getUintValueNode(rootNode, TAG_RATE_TABLE_ACTIVEDATE);
         pNewRateTable->header.expiredDate = jsParser.getUintValueNode(rootNode, TAG_RATE_TABLE_EXPIREDATE);
         pNewRateTable->header.totalProduct = jsParser.getUintValueNode(rootNode, TAG_RATE_TABLE_TOTALITEM);

         if( pNewRateTable->header.totalProduct <= 0 )
         {
            _DTraceDebug("Parse failed due totalProduct <= 0...");
            clearBufferIn();
            rootNode.clear();
            return ret;
         }

         Json::Value defaultValue;
         Json::Value jsNodeItems = rootNode.get(TAG_RATE_TABLE_ITEMS, defaultValue);
         if( jsNodeItems.type() == Json::arrayValue && pNewRateTable->header.totalProduct == jsNodeItems.size() )
         {
            if( pNewRateTable->header.totalProduct > PRODUCT_NUM_MAX )
            {
               //truncate number of product
               pNewRateTable->header.totalProduct = PRODUCT_NUM_MAX;
            }

            for( int i = 0; i < pNewRateTable->header.totalProduct; ++i )
            {
               std::string sStringValue;
               int iLength;
               pNewRateTable->products[i].id = jsParser.getUintValueNode(jsNodeItems[i], TAG_PRODUCT_NUMBER);
               pNewRateTable->products[i].price = jsParser.getRealValueNode(jsNodeItems[i], TAG_PRODUCT_PRICE);
               pNewRateTable->products[i].expdate = jsParser.getRealValueNode(jsNodeItems[i], TAG_PRODUCT_EXPDAY);
               pNewRateTable->products[i].temperature = jsParser.getRealValueNode(jsNodeItems[i], TAG_PRODUCT_TEMPERATURE);
               sStringValue = jsParser.getStrValueNode(jsNodeItems[i], TAG_PRODUCT_CODE);
               iLength = PRODUCT_CODE_LEN-1;
               if( iLength > sStringValue.length() )
               {
                  iLength = sStringValue.length();
               }
               memcpy(pNewRateTable->products[i].code, sStringValue.c_str(), iLength);
               pNewRateTable->products[i].code[iLength] = 0;

               sStringValue = jsParser.getStrValueNode(jsNodeItems[i], TAG_PRODUCT_NAME);
               iLength = PRODUCT_NAME_LEN-1;
               if( iLength > sStringValue.length() )
               {
                  iLength = sStringValue.length();
               }
               memcpy(pNewRateTable->products[i].name, sStringValue.c_str(), iLength);
               pNewRateTable->products[i].name[iLength] = 0;

               sStringValue = jsParser.getStrValueNode(jsNodeItems[i], TAG_PRODUCT_UNIT);
               iLength = PRODUCT_UNIT_LEN-1;
               if( iLength > sStringValue.length() )
               {
                  iLength = sStringValue.length();
               }
               memcpy(pNewRateTable->products[i].unit, sStringValue.c_str(), iLength);
               pNewRateTable->products[i].unit[iLength] = 0;
            }
         }
         else
         {
            _DTraceDebug("parse arrayValue failed...");
         }

         if( pNewRateTable->header.id > 0 &&
             pNewRateTable->header.activatedDate > 0 &&
             pNewRateTable->header.expiredDate > 0 &&
             pNewRateTable->header.totalProduct > 0 )
         {

            RateTableObject updateRateId = RateManager::getInstance().addRateTable(pNewRateTable);

            switch( updateRateId )
            {
            case ESCALE_RATE_TABLE_1:
               ret = _Sa_.serialize(SERIALIZABLE_PARTITION_2);
               Task_Delay(ST_GetClocksPerSecond()/3);
               break;
            case ESCALE_RATE_TABLE_2:
               ret = _Sa_.serialize(SERIALIZABLE_PARTITION_3);
               Task_Delay(ST_GetClocksPerSecond()/3);
               break;
            case ESCALE_RATE_TABLE_3:
               ret = _Sa_.serialize(SERIALIZABLE_PARTITION_4);
               Task_Delay(ST_GetClocksPerSecond()/3);
               break;
            default:
               break;
            }

            if( !ret )
            {
               _DTraceDebug("Saving the rate table %d failed...", updateRateId);
            }
         }
         else
         {
            _DTraceDebug("pNewRateTable Failed: id=%d, activatedDate=%d, expiredDate=%d, totalProduct=%d",
                          pNewRateTable->header.id, pNewRateTable->header.activatedDate, pNewRateTable->header.expiredDate, pNewRateTable->header.totalProduct);
         }

         if( pNewRateTable != NULL )
         {
            delete pNewRateTable;
            pNewRateTable = NULL;
         }
      }
      else
      {
         _DTraceDebug("Failed due to rootNode is not Object...");
      }
   }
   else
   {
      _DTraceDebug("Could not get Json object...");
   }

   clearBufferIn();
   rootNode.clear();

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnDevice::getBufferIn(std::string& sBuffer)
{
   sBuffer.assign(m_RAWDATABUFFER);
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnDevice::clearBufferIn()
{
   m_RAWDATABUFFER.clear();
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice::checkHttpRespDone(std::string& httpResponse)
{
   bool ret = false;
   std::string strJsonResponse;
   std::string strGbsJsonResponse;

   int pos1 = httpResponse.find('{', 0);
   int pos2 = httpResponse.find('}', 0);
   if( pos1 != std::string::npos && pos2 != std::string::npos )
   {
      strJsonResponse = httpResponse.substr(pos1, pos2-pos1+1);
   }
   else
   {
      strJsonResponse = httpResponse;
   }

   strGbsJsonResponse.assign("#GBSJSONSTART#");
   strGbsJsonResponse.append(strJsonResponse);
   strGbsJsonResponse.append("#GBSJSONEND#");

   JsonParser jsParser;
   Json::Value nodeRoot;

   if( jsParser.getJsonObject(strGbsJsonResponse, nodeRoot) )
   {
      int gbsErrorCode = jsParser.getIntValueNode(nodeRoot, "errorcode");
      std::string vnpostErrorCode = jsParser.getStrValueNode(nodeRoot, "Code");

      _DTraceDebug("Code1 is %d, Code2 is %s", gbsErrorCode, vnpostErrorCode.c_str());

      if( gbsErrorCode == 200 || vnpostErrorCode.find("00") != std::string::npos )
      {
         ret = true;
      }

   }
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
