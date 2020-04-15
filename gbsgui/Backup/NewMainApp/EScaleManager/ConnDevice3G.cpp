//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ConnDevice3G.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "ConnDevice3G.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
ConnDevice3G::ConnDevice3G() : ConnDevice(SPN_COM3)
{
}

//////////////////////////////////////////////////////////////////////////////////////
ConnDevice3G::~ConnDevice3G()
{
}

//////////////////////////////////////////////////////////////////////////////////////
ConnDevice3G& ConnDevice3G::getInstance()
{
   static ConnDevice3G instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::init()
{
   bool ret = false;
   m_deviceId = "";
   m_networkCode = NWC_NONE;
   m_connLocation.reset();
   m_connDateTime.reset();

   _DTraceDebug("*** INIT ***");

   //try some time
   for( int i = 0; i < 4; ++i )
   {
      ret = isReady();
      if( ret )
      {
         break;
      }
      Task_Delay(ST_GetClocksPerSecond()/2);
   }

   if( !ret )
   {
      deinit();
      return ret;
   }

   std::string ccidNum;
   if( getCCID(ccidNum) )
   {
      setDeviceId(ccidNum);
   }

   int bearerStatus = getBearerStatus();

   if( bearerStatus != 1 )
   {
      if( bearerStatus != 3 )
      {
         setBearerConnection(false);
      }

      setBearerConnection(true);
   }

   _DTraceDebug("*** INIT DONE ***");

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::deinit()
{
   _DTraceDebug("*** DEINIT ***");
   powerOff();
   m_deviceId = "";
   m_networkCode = NWC_NONE;
   m_connLocation.reset();
   m_connDateTime.reset();
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::isInitialized()
{
   return (bool)( m_networkCode != NWC_NONE && !m_deviceId.empty() );
}


//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::scanDataRequest(std::string& oDataBuff)
{
   //not defined in 3G connection
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::respDataRequest(const int connId, const std::string& iDataBuff)
{
   //not defined in 3G connection
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
ConnDeviceType ConnDevice3G::getConnDeviceType()
{
   return CDT_3G;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::setStage(ConnDeviceStage stage)
{
   if( m_deviceStage != stage )
   {
      m_deviceStage = stage;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::reqDateTime(time_t& oDateTimeStamp)
{
   //implement later
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
ConnHttpStatusCode ConnDevice3G::reqRegister(const std::string& deviceId, const std::string& userId, const std::string& regContent)
{
   //implement later
   return HTTP_200;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::reqRateTable(const std::string& rateTableId, const int rateStatus)
{
   //implement later
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnDevice3G::powerOff()
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   param_i.value = "0"; //Power off urgently
   param_i.isNumeric = true;
   params.push_back(param_i);

   writeCommand("AT+CPOWD", params, responseBuff);
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::isReady()
{
   std::string responseBuff;
   std::list<CmdParameter> params;
   params.clear();
   return writeCommand("AT", params, responseBuff, 1);
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::setBearerConnection(bool switchOn)
{
   bool ret = false;
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   if( switchOn ) //open bearer
   {
      if( m_networkCode == NWC_NONE )
      {
         std::string ccidNum;
         if( getCCID(ccidNum) )
         {
            setDeviceId(ccidNum);
         }
      }

      params.clear();
      param_i.value = "3";
      param_i.isNumeric = true;
      params.push_back(param_i);
      param_i.value = "1";
      param_i.isNumeric = true;
      params.push_back(param_i);
      param_i.value = "APN";
      param_i.isNumeric = false;
      params.push_back(param_i);
      param_i.value = "internet";
      if( m_networkCode == NWC_MOBIFONE )
      {
         param_i.value = "m-wap";
      }
      else if( m_networkCode == NWC_VINAPHONE )
      {
         param_i.value = "m3-world";
      }
      else if( m_networkCode == NWC_VIETEL )
      {
         param_i.value = "v-internet";
      }
      param_i.isNumeric = false;
      params.push_back(param_i);
      ret = writeCommand("AT+SAPBR", params, responseBuff);

      params.clear();
      param_i.value = "1";
      param_i.isNumeric = true;
      params.push_back(param_i);
      param_i.value = "1";
      param_i.isNumeric = true;
      params.push_back(param_i);
      ret &= writeCommand("AT+SAPBR", params, responseBuff, 30);
   }
   else //close bearer
   {
      params.clear();
      param_i.value = "0";
      param_i.isNumeric = true;
      params.push_back(param_i);
      param_i.value = "1";
      param_i.isNumeric = true;
      params.push_back(param_i);
      ret &= writeCommand("AT+SAPBR", params, responseBuff, 30);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
int ConnDevice3G::getBearerStatus()
{
   int ret = -1; //0-connecting; 1-connected; 2-closing; 3-closed
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   param_i.value = "2";
   param_i.isNumeric = true;
   params.push_back(param_i);
   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);

   if( writeCommand("AT+SAPBR", params, responseBuff) )
   {
      size_t pos = responseBuff.find("+SAPBR:");
      if( pos != std::string::npos )
      {
         pos = responseBuff.find(',', pos);
         if( pos != std::string::npos )
         {
            std::string strBearStatus = GStringUtils::trimAll(responseBuff.substr(pos+1,1));
            _DTraceDebug("strBearStatus = %s", strBearStatus.c_str());
            ret = GStringUtils::str2int(strBearStatus);
         }
      }
   }

   _DTraceDebug("BearerStatus = %d", ret);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::initHttp()
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   writeCommand("AT+HTTPINIT", params, responseBuff);

   params.clear();
   param_i.value = "CID";
   param_i.isNumeric = false;
   params.push_back(param_i);
   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);
   writeCommand("AT+HTTPPARA", params, responseBuff);
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::termHttp()
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;
   params.clear();
   writeCommand("AT+HTTPTERM", params, responseBuff, 1);
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::buildHttpRequest(std::string& outGetService, ConnHttpRequestType httpReqType, ConnHttpService httpService, std::list<std::string>& httpParams, int timeOut)
{
   bool ret = true;

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   UrlInfo serverUrl;
   _U_.getSubmitDataUrl(serverUrl);
   outGetService = serverUrl.url;
   if( GStringUtils::str2int(serverUrl.port) != 80 && GStringUtils::str2int(serverUrl.port) != 0 )
   {
      outGetService.append(":");
      outGetService.append(serverUrl.port);
   }

   //Using SSL or Not
   params.clear();
   if(serverUrl.ssl == 1)
   {
      param_i.value = "1";
   }
   else
   {
      param_i.value = "0";
   }
   param_i.isNumeric = true;
   params.push_back(param_i);
   ret = writeCommand("AT+HTTSSL", params, responseBuff);

   std::string serviceParams;
   std::string serviceName;
   buildHttpParams(serviceParams, httpService, httpReqType, httpParams);
   getHttpServiceName(serviceName, httpService);

   if( httpReqType == HTTP_REQ_GET )
   {
	   outGetService.append(serviceName);
	   outGetService.append("?");
	   outGetService.append(serviceParams);
   }
   else if( httpReqType == HTTP_REQ_POST )
   {
 	   outGetService.append(serviceName);
   }

   params.clear();
   param_i.value = "URL";
   param_i.isNumeric = false;
   params.push_back(param_i);
   param_i.value.assign(outGetService);
   param_i.isNumeric = false;
   params.push_back(param_i);
   ret = writeCommand("AT+HTTPPARA", params, responseBuff);

   if( httpReqType == HTTP_REQ_GET )
   {
	   //Default timeout = 120 second
	   params.clear();
	   param_i.value = "TIMEOUT";
	   param_i.isNumeric = false;
	   params.push_back(param_i);
	   if( timeOut > 120 )
	   {
	      //max timeout = 120
	      timeOut = 120;
	   }
	   GStringUtils::format(param_i.value, "%d", timeOut);
	   param_i.isNumeric = true;
	   params.push_back(param_i);

	   writeCommand("AT+HTTPPARA", params, responseBuff);
   }
   else if( httpReqType == HTTP_REQ_POST )
   {
	   //AT+HTTPPARA="CONTENT","application/json"$0D$0A
	   //AT+HTTPDATA=191,12000$0D$0A
	   //{"device":"89840480000487906630","qrcode":"Hang hoa de vo","weight":1.23}$0D$0A
	   params.clear();
	   param_i.value = "CONTENT";
	   param_i.isNumeric = false;
	   params.push_back(param_i);
	   param_i.value.assign("application/json");
	   param_i.isNumeric = false;
	   params.push_back(param_i);
	   ret = writeCommand("AT+HTTPPARA", params, responseBuff);

	   params.clear();
	   GStringUtils::format(param_i.value, "%d", serviceParams.length());
	   param_i.isNumeric = true;
	   params.push_back(param_i);
	   param_i.value = "12000";
	   param_i.isNumeric = true;
	   params.push_back(param_i);
	   ret = writeCommand("AT+HTTPDATA", params, responseBuff);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::sendHttpRequest(ConnHttpRequestType httpReqType)
{
   bool ret = false;

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   param_i.value = "0";
   switch( httpReqType )
   {
      case HTTP_REQ_GET:
    	  break;
      case HTTP_REQ_POST:
    	  param_i.value = "1";
          break;
      case HTTP_REQ_PUSH:
      	  param_i.value = "2";
          break;
      case HTTP_REQ_HEAD:
      	  param_i.value = "3";
          break;
      default:
    	  break;
   }

   param_i.isNumeric = true;
   params.push_back(param_i);

   if( writeCommand("AT+HTTPACTION", params, responseBuff, 30) )
   {
     _DTraceDebug("HTTPACTION Step1: %s", responseBuff.c_str());

      if( responseBuff.find("+HTTPACTION:") != std::string::npos )
      {
         if( responseBuff.find(",200,") != std::string::npos )
         {
           ret = true;
         }
         else if( responseBuff.find(",600,") != std::string::npos || //Not HTTP PDU
                  responseBuff.find(",601,") != std::string::npos || //Network Error
                  responseBuff.find(",602,") != std::string::npos || //No memory
                  responseBuff.find(",603,") != std::string::npos || //DNS Error
                  responseBuff.find(",604,") != std::string::npos )  //Stack Busy

         {
            deinit();
         }
      }
      else
      {
         //try to read the response RESPONSE_MAX_TIMEOUT ms
         for( int i = 0; i < RESPONSE_MAX_TIMEOUT; i++ )
         {
            Task_Delay(ST_GetClocksPerSecond()/10);
            readBuffer(responseBuff);
            if( responseBuff.find("+HTTPACTION:") == std::string::npos )
            {
               continue;
            }

            _DTraceDebug("HTTPACTION Step2: %s", responseBuff.c_str());

            if( responseBuff.find(",200,") != std::string::npos )
            {
               ret = true;
            }
            else if( responseBuff.find(",600,") != std::string::npos || //Not HTTP PDU
                     responseBuff.find(",601,") != std::string::npos || //Network Error
                     responseBuff.find(",602,") != std::string::npos || //No memory
                     responseBuff.find(",603,") != std::string::npos || //DNS Error
                     responseBuff.find(",604,") != std::string::npos )  //Stack Busy
            {
               deinit();
            }
            break;
         }
      }

   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::sendHttpRead()
{
   bool ret = false;
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;
   params.clear();
   if( writeCommand("AT+HTTPREAD", params, responseBuff, 30) )
   {
     _DTraceDebug("HTTPREAD Step1: %s", responseBuff.c_str());

      if( responseBuff.find("+HTTPREAD:") != std::string::npos )
      {
         if( responseBuff.find("\"errorcode\":200") != std::string::npos )
         {
            ret = true;
         }
      }
      else
      {
         //try to read the response in RESPONSE_MAX_TIMEOUT ms
         for( int i = 0; i < RESPONSE_MAX_TIMEOUT; i++ )
         {
            Task_Delay(ST_GetClocksPerSecond()/10);
            readBuffer(responseBuff);
            if( responseBuff.find("+HTTPREAD:") == std::string::npos )
            {
               continue;
            }

            _DTraceDebug("HTTPREAD Step2: %s", responseBuff.c_str());

            if( responseBuff.find("\"errorcode\":200") != std::string::npos )
            {
               ret = true;
            }

            break;
         }
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::getConfiguration(std::string& sName, std::string& sEmail, std::string& sNumber)
{
   bool ret = false;
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;
   params.clear();
   if( writeCommand("AT+HTTPREAD", params, responseBuff, 30) )
   {
      if( responseBuff.find("+HTTPREAD:") != std::string::npos )
      {
         if( responseBuff.find("-LOG OK-") != std::string::npos )
         {
            ret = true;
         }
      }
      else
      {
         //try to read the response in RESPONSE_MAX_TIMEOUT ms
         for( int i = 0; i < RESPONSE_MAX_TIMEOUT; i++ )
         {
            Task_Delay(ST_GetClocksPerSecond()/10);
            readBuffer(responseBuff);
            if( responseBuff.find("+HTTPREAD:") == std::string::npos )
            {
               continue;
            }

            if( responseBuff.find("-LOG OK-") != std::string::npos )
            {
               ret = true;
            }

            break;
         }
      }
   }

   if( ret )
   {
      std::size_t pos = responseBuff.find("-LOG OK-");
      responseBuff = responseBuff.substr(pos+8);
      pos = responseBuff.find("__");
      sName = responseBuff.substr(0, pos);

      responseBuff = responseBuff.substr(pos+2);
      pos = responseBuff.find("__");
      sEmail = responseBuff.substr(0, pos);
      GStringUtils::trimAll(sEmail);

      sNumber = responseBuff.substr(pos+2);
      GStringUtils::trimAll(sNumber);
      pos = sNumber.find("__");
      sNumber = sNumber.substr(0, pos);

      if( sNumber.find("0") == 0 )
      {
         sNumber = sNumber.substr(1);
      }

      GStringUtils::format(sNumber, "+84%s", sNumber.c_str());

      _DTraceDebug("%s %s %s", sName.c_str(), sEmail.c_str(), sNumber.c_str());
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::sendSMS(std::string txtPhoneNum, std::string txtMessage)
{
   bool ret = false;
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);
   writeCommand("AT+CMGF", params, responseBuff);

   params.clear();
   param_i.value = txtPhoneNum;
   param_i.isNumeric = false;
   params.push_back(param_i);
   writeCommand("AT+CMGS", params, responseBuff);

   params.clear();
   std::string strCommand = txtMessage;
   strCommand.append(1, (char)ASCII_SUB);
   writeCommand(strCommand, params, responseBuff, 15);
   if( responseBuff.find("+CMGS:") != std::string::npos )
   {
      if( responseBuff.find("OK") != std::string::npos )
      {
         ret = true;
      }
   }
   else
   {
      //try to read the response in RESPONSE_MAX_TIMEOUT ms
      for( int i = 0; i < RESPONSE_MAX_TIMEOUT; i++ )
      {
         Task_Delay(ST_GetClocksPerSecond()/10);
         readBuffer(responseBuff);
         if( responseBuff.find("+CMGS:") == std::string::npos )
         {
            continue;
         }

         if( responseBuff.find("OK") != std::string::npos )
         {
            ret = true;
         }

         break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::setLocalTime(std::string localTime)
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   //always set mode = 1
   params.clear();
   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);
   writeCommand("AT+CLTS", params, responseBuff);

   if( localTime.empty() )
   {
      //test and set time zone +28 (GMT+7)
      std::string sTime, sZone;
      if( getLocalTime(sTime, sZone) )
      {
         if( sZone.find("+28") == std::string::npos )
         {
            localTime = sTime + sZone;
         }
      }
   }

   //abort if localTime still empty
   if( !localTime.empty() )
   {
      params.clear();
      param_i.value = localTime;
      param_i.isNumeric = false;
      params.push_back(param_i);
      writeCommand("AT+CCLK", params, responseBuff);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::getLocalTime(std::string& oDateTime, std::string& oTimeZone)
{
   bool ret = false;

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();

   if( writeCommand("AT+CCLK?", params, responseBuff, 1) )
   {
      size_t pos = 0;
      while( (pos = responseBuff.find((char)ASCII_CR)) != std::string::npos )
      {
         std::string token = GStringUtils::trimAll(responseBuff.substr(0, pos));
         responseBuff.erase(0, pos + 2);
         if( token.find("+CCLK:") != std::string::npos )
         {
            size_t pos1 = std::string::npos;
            size_t pos2 = std::string::npos;
            pos1 = token.find((char)0x22); //0x22 is '"'
            if( pos1 != std::string::npos )
            {
               pos1 = pos1+1;
               pos2 = token.find((char)0x22, pos1);
            }

            if( pos2 != std::string::npos && pos1 < pos2 )
            {
               oDateTime = token.substr(pos1, pos2-pos1-3);
               oTimeZone = token.substr(pos2-pos1-3);
               ret = true;
            }

            break;
         }
      }
   }
   else
   {
      _DTraceDebug("writeCommand false");
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::getGsmLocationAndTime(ConnLocation& connLocation, ConnDateTime& connDateTime)
{
   bool ret = false;

   connLocation.reset();
   connDateTime.reset();

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);
   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);

   if( writeCommand("AT+CIPGSMLOC", params, responseBuff, 30) )
   {
      size_t pos = 0;
      while( (pos = responseBuff.find((char)ASCII_CR)) != std::string::npos )
      {
         std::string token = GStringUtils::trimAll(responseBuff.substr(0, pos));
         responseBuff.erase(0, pos + 1);
         if( token.find("+CIPGSMLOC:") != std::string::npos )
         {
            size_t pos1 = 11; //length of "+CIPGSMLOC:"
            size_t pos2 = token.find(',', pos1);
            if( pos != std::string::npos )
            {
               std::string sErrorCode = GStringUtils::trimAll(token.substr(pos1, pos2-pos1));
               int iErrorCode = GStringUtils::str2int(sErrorCode);
               _DTraceDebug(sErrorCode.c_str());

               if( iErrorCode == 0 )
               {
                  //longtitude
                  pos1 = pos2+1;
                  pos2 = token.find(',', pos1);
                  std::string sLongtitude = GStringUtils::trimAll(token.substr(pos1, pos2-pos1));
                  _DTraceDebug(sLongtitude.c_str());
                  //latitude
                  pos1 = pos2+1;
                  pos2 = token.find(',', pos1);
                  std::string sLatitude = GStringUtils::trimAll(token.substr(pos1, pos2-pos1));
                  _DTraceDebug(sLatitude.c_str());
                  //date
                  pos1 = pos2+1;
                  pos2 = token.find(',', pos1);
                  std::string sDate = GStringUtils::trimAll(token.substr(pos1, pos2-pos1));
                  _DTraceDebug(sDate.c_str());
                  //time
                  pos1 = pos2+1;
                  std::string sTime = GStringUtils::trimAll(token.substr(pos1));
                  _DTraceDebug(sTime.c_str());

                  connLocation.longtitude = GStringUtils::str2float(sLongtitude);
                  connLocation.latitude = GStringUtils::str2float(sLatitude);

                  pos1 = 0;
                  pos2 = sDate.find('/');
                  connDateTime.year = GStringUtils::str2int(sDate.substr(pos1, pos2-pos1));
                  pos1 = pos2+1;
                  pos2 = sDate.find('/', pos1);
                  connDateTime.month = GStringUtils::str2int(sDate.substr(pos1, pos2-pos1));
                  pos1 = pos2+1;
                  connDateTime.day = GStringUtils::str2int(sDate.substr(pos1));

                  pos1 = 0;
                  pos2 = sTime.find(':');
                  connDateTime.hour = GStringUtils::str2int(sTime.substr(pos1, pos2-pos1));
                  pos1 = pos2+1;
                  pos2 = sTime.find(':');
                  connDateTime.minute = GStringUtils::str2int(sTime.substr(pos1, pos2-pos1));
                  pos1 = pos2+1;
                  connDateTime.second = GStringUtils::str2int(sTime.substr(pos1));

                  ret = true;
               }
            }

            break;
         }
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::getCCID(std::string& oCcidNumber)
{
   bool ret = false;

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();

   if( writeCommand("AT+CCID", params, responseBuff, 3) )
   {
      size_t pos = 0;
      while( (pos = responseBuff.find((char)ASCII_CR)) != std::string::npos )
      {
         std::string token = GStringUtils::trimAll(responseBuff.substr(0, pos));
         responseBuff.erase(0, pos + 1);
         if( token.length() > 15 )
         {
            oCcidNumber = token;
            ret = true;
            break;
         }
      }
   }
   else
   {
      _DTraceDebug("writeCommand false");
   }

   if( oCcidNumber.length() > 15 )
   {
      std::string networkCode = oCcidNumber.substr(0, 6);
      if( networkCode.find("8401") != std::string::npos )
      {
         m_networkCode = NWC_MOBIFONE;
         _DTraceDebug("NWC_MOBIFONE");
      }
      else if( networkCode.find("8402") != std::string::npos )
      {
         m_networkCode = NWC_VINAPHONE;
         _DTraceDebug("NWC_VINAPHONE");
      }
      else if( networkCode.find("8404") != std::string::npos )
      {
         m_networkCode = NWC_VIETEL;
         _DTraceDebug("NWC_VIETEL");
      }
      else if( networkCode.find("8405") != std::string::npos )
      {
         m_networkCode = NWC_VIETNAMMOBILE;
         _DTraceDebug("NWC_VIETNAMMOBILE");
      }
      else
      {
         m_networkCode = NWC_NONE;
         _DTraceDebug("NWC_NONE");
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::getCUSD(std::string& oCusdInfo)
{
   bool ret = false;
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;
   params.clear();

   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);

   if( writeCommand("AT+CUSD", params, responseBuff) )
   {
      params.clear();
      param_i.value = "1";
      param_i.isNumeric = true;
      params.push_back(param_i);
      param_i.value = "*101#";
      param_i.isNumeric = false;
      params.push_back(param_i);

      if( writeCommand("AT+CUSD", params, responseBuff, 10) )
      {
         if( responseBuff.find("+CUSD:") != std::string::npos )
         {
            ret = true;
         }
         else
         {
            //try to read the response in RESPONSE_MAX_TIMEOUT ms
            for( int i = 0; i < RESPONSE_MAX_TIMEOUT; i++ )
            {
               Task_Delay(ST_GetClocksPerSecond()/10);
               readBuffer(responseBuff);
               if( responseBuff.find("+CUSD:") == std::string::npos )
               {
                  continue;
               }

               if( responseBuff.find("OK") != std::string::npos )
               {
                  ret = true;
               }

               _DTraceDebug(responseBuff.c_str());

               break;
            }
         }
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::getSubcriberNum(std::string& oSubcriberNum)
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();

   if( writeCommand("AT+CNUM", params, responseBuff, 3) )
   {
      return true;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
/*
bool ConnDevice3G::submitData(ScaleItemData& itemData, bool isDeleted)
{
   bool ret = false;

   int bearerStatus = getBearerStatus();

   if( bearerStatus != 1 )
   {
      if( bearerStatus != 3 )
      {
         setBearerConnection(false);
      }

      if( !setBearerConnection(true) )
      {
         return ret;
      }
   }

   std::list<std::string> params;
   std::string strHttpRequest;
   std::string strParam_i;

   initHttp();

   //time
   GStringUtils::format(strParam_i, "%d", itemData.timeStamp);
   params.push_back(strParam_i);

   //deviceid
   if( m_deviceId.length() < 1 )
   {
      std::string ccidNum;
      if( getCCID(ccidNum) )
      {
         setDeviceId(ccidNum);
      }
   }
   params.push_back(m_deviceId);

   ProductInfo productInfo;
   RateManager::getInstance().getProductInfo((U32)GStringUtils::str2int(itemData.typeName), productInfo);
   //packageid
   strParam_i = ScaleManager::getInstance().genPackageId(productInfo.code, itemData.timeStamp);
   params.push_back(strParam_i);
   //productid
   params.push_back(productInfo.code);

   //value
   strParam_i = ScaleManager::getInstance().data2Str(itemData.dataNet);
   if( isDeleted )
   {
      strParam_i.insert(0, "-");
   }
   params.push_back(strParam_i);

   //price
   strParam_i = ScaleManager::getInstance().data2Str(itemData.unitPrice);
   params.push_back(strParam_i);

   //unit
   if( SUN_PCS == itemData.unit )
   {
      strParam_i.assign(productInfo.unit);
   }
   else
   {
      strParam_i = ScaleManager::getInstance().unit2Str(itemData.unit);
   }
   params.push_back(strParam_i);

   buildHttpRequest(strHttpRequest, HTTP_REQ_GET, CHS_SUBMIT_DATA, params, 90);

   if( sendHttpRequest(HTTP_REQ_GET) )
   {
      ret = sendHttpRead();
   }

   termHttp();

   return ret;
}
*/
//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::submitData(ScaleItemData& itemData, bool isDeleted)
{
   bool ret = false;

   int bearerStatus = getBearerStatus();

   if( bearerStatus != 1 )
   {
      if( bearerStatus != 3 )
      {
         setBearerConnection(false);
      }

      if( !setBearerConnection(true) )
      {
         return ret;
      }
   }

   std::list<std::string> params;
   std::string strHttpRequest;
   std::string strParam_i;

   initHttp();

   //deviceid
   if( m_deviceId.length() < 1 )
   {
      std::string ccidNum;
      if( getCCID(ccidNum) )
      {
         setDeviceId(ccidNum);
      }
   }
   params.push_back(m_deviceId);

   //qrcode
   strParam_i = itemData.scaleCode;
   params.push_back(strParam_i);

   //value
   strParam_i = ScaleManager::getInstance().data2Str(itemData.dataNet);
   if( isDeleted )
   {
      strParam_i.insert(0, "-");
   }
   params.push_back(strParam_i);

   buildHttpRequest(strHttpRequest, HTTP_REQ_GET, CHS_SUBMIT_DATA_VNPOST, params, 90);

   if( sendHttpRequest(HTTP_REQ_GET) )
   {
      ret = sendHttpRead();
   }

   termHttp();

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
/*
bool ConnDevice3G::submitData(ScaleWeightData& itemData)
{
   bool ret = false;

   int bearerStatus = getBearerStatus();

   if( bearerStatus != 1 )
   {
      if( bearerStatus != 3 )
      {
         setBearerConnection(false);
      }

      if( !setBearerConnection(true) )
      {
         return ret;
      }
   }

   initHttp();

   std::list<std::string> params;
   std::string strHttpRequest;
   std::string strParam_i;

   //time
   GStringUtils::format(strParam_i, "%d", itemData.timestamp);
   params.push_back(strParam_i);

   //deviceid
   if( m_deviceId.length() < 1 )
   {
      std::string ccidNum;
      if( getCCID(ccidNum) )
      {
         setDeviceId(ccidNum);
      }
   }
   params.push_back(m_deviceId);

   //packageid
   params.push_back("SanPham");

   //productid
   params.push_back("SanPham");

   //value
   strParam_i = ScaleManager::getInstance().data2Str(itemData.data1);
   params.push_back(strParam_i);

   //price
   params.push_back("0.00");

   //unit
   strParam_i = ScaleManager::getInstance().unit2Str(itemData.unit);
   params.push_back(strParam_i);

   buildHttpRequest(strHttpRequest, HTTP_REQ_GET, CHS_SUBMIT_DATA, params);

   if( sendHttpRequest(HTTP_REQ_GET) )
   {
      ret = sendHttpRead();
   }

   termHttp();

   return ret;
}
*/
//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::submitData(ScaleWeightData& itemData)
{
   bool ret = false;

   int bearerStatus = getBearerStatus();

   if( bearerStatus != 1 )
   {
      if( bearerStatus != 3 )
      {
         setBearerConnection(false);
      }

      if( !setBearerConnection(true) )
      {
         return ret;
      }
   }

   initHttp();

   std::list<std::string> params;
   std::string strHttpRequest;
   std::string strParam_i;

   //deviceid
   if( m_deviceId.length() < 1 )
   {
      std::string ccidNum;
      if( getCCID(ccidNum) )
      {
         setDeviceId(ccidNum);
      }
   }
   params.push_back(m_deviceId);

   //qrcode
   strParam_i = itemData.scaleCode;
   params.push_back(strParam_i);

   //value
   strParam_i = ScaleManager::getInstance().data2Str(itemData.data1);
   params.push_back(strParam_i);

   //Todo: If QrCode is greater than 200 characters, it leads a crash. Crazy!!!
   buildHttpRequest(strHttpRequest, HTTP_REQ_GET, CHS_SUBMIT_DATA_VNPOST, params);

   _DTraceDebug("Sending... %s", strHttpRequest.c_str());

   if( sendHttpRequest(HTTP_REQ_GET) )
   {
      ret = sendHttpRead();
   }

   termHttp();

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::submitEmail(std::string& content)
{
   bool ret = false;

   int bearerStatus = getBearerStatus();

   if( bearerStatus != 1 )
   {
      if( bearerStatus != 3 )
      {
         setBearerConnection(false);
      }

      if( !setBearerConnection(true) )
      {
         return ret;
      }
   }

   std::string subject = "[Vissan-Test] Cua hang thuc pham Vissan";
   MailToList receivers;
   MailAddressInfo emailAddress;
   receivers.reset();
   _U_.getEmailReceiver(emailAddress);

   if( !emailAddress.address.empty() &&
        emailAddress.address.find("@") != std::string::npos &&
        emailAddress.address.find("disable") == std::string::npos )
   {
      //to list
      receivers.toList.push_back(emailAddress);
      //cc list
      //emailAddress.accName = EMAIL_CC_NAME;
      //emailAddress.address = EMAIL_CC_USERNAME;
      //receivers.ccList.push_back(emailAddress);
      //bcc list
      //emailAddress.accName = "tranthanhson";
      //emailAddress.address = "tranthanhson@gbs-jsc.com";
      //receivers.bccList.push_back(emailAddress);
      ret = sendEmail(receivers, subject, content);
   }
   else
   {
      //no configuration, no send email
      ret = true;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::submitSMS(std::string& content)
{
   bool ret = false;

   std::string smsNumReceiver;
   _U_.getSmsNumReceiver(smsNumReceiver);

   if( (!smsNumReceiver.empty() &&
         smsNumReceiver.find("+84") != std::string::npos &&
         smsNumReceiver.find("disable") == std::string::npos) )
   {
      ret = sendSMS(smsNumReceiver, content);
   }
   else
   {
      //no configuration, no send sms
      ret = true;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDevice3G::sendEmail(MailToList& receivers, std::string& subject, std::string& body)
{
   bool ret = false;

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);
   writeCommand("AT+EMAILCID", params, responseBuff);
   //send mail time out 120 seconds
   params.clear();
   param_i.value = "120";
   param_i.isNumeric = true;
   params.push_back(param_i);
   writeCommand("AT+EMAILTO", params, responseBuff);
   //active ssl
   params.clear();
   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);
   writeCommand("AT+EMAILSSL", params, responseBuff);

   //outgoing server
   SenderMailServerCfg senderMailSrvCfg;
   _U_.getEmailSender(senderMailSrvCfg);
   params.clear();
   param_i.value.assign(senderMailSrvCfg.outGoingServer);
   param_i.isNumeric = false;
   params.push_back(param_i);
   param_i.value.assign(senderMailSrvCfg.outGoingPort);
   param_i.isNumeric = true;
   params.push_back(param_i);
   writeCommand("AT+SMTPSRV", params, responseBuff);
   //username,pass
   params.clear();
   param_i.value = "1";
   param_i.isNumeric = true;
   params.push_back(param_i);
   param_i.value.assign(senderMailSrvCfg.username);
   param_i.isNumeric = false;
   params.push_back(param_i);
   param_i.value.assign(senderMailSrvCfg.password);
   param_i.isNumeric = false;
   params.push_back(param_i);
   writeCommand("AT+SMTPAUTH", params, responseBuff);
   //from
   params.clear();
   param_i.value.assign(senderMailSrvCfg.username);
   param_i.isNumeric = false;
   params.push_back(param_i);
   param_i.value.assign(senderMailSrvCfg.senderName);
   param_i.isNumeric = false;
   params.push_back(param_i);
   writeCommand("AT+SMTPFROM", params, responseBuff);

   //to list
   if( !receivers.toList.empty() )
   {
      int toIndex = 0;
      for( std::list<MailAddressInfo>::iterator it = receivers.toList.begin(); it != receivers.toList.end(); ++it )
      {
         std::string sToIndex;
         GStringUtils::format(sToIndex, "%d", toIndex);
         params.clear();
         param_i.value = "0";
         param_i.isNumeric = true;
         params.push_back(param_i);
         param_i.value = sToIndex;
         param_i.isNumeric = true;
         params.push_back(param_i);
         param_i.value = (*it).address;
         param_i.isNumeric = false;
         params.push_back(param_i);
         param_i.value = (*it).accName;
         param_i.isNumeric = false;
         params.push_back(param_i);
         writeCommand("AT+SMTPRCPT", params, responseBuff);
      }
   }

   //cc list
   if( !receivers.ccList.empty() )
   {
      int ccIndex = 0;
      for( std::list<MailAddressInfo>::iterator it = receivers.ccList.begin(); it != receivers.ccList.end(); ++it )
      {
         std::string sCcIndex;
         GStringUtils::format(sCcIndex, "%d", ccIndex);
         params.clear();
         param_i.value = "1";
         param_i.isNumeric = true;
         params.push_back(param_i);
         param_i.value = sCcIndex;
         param_i.isNumeric = true;
         params.push_back(param_i);
         param_i.value = (*it).address;
         param_i.isNumeric = false;
         params.push_back(param_i);
         param_i.value = (*it).accName;
         param_i.isNumeric = false;
         params.push_back(param_i);
         writeCommand("AT+SMTPRCPT", params, responseBuff);
      }
   }

   //bcc list
   if( !receivers.bccList.empty() )
   {
      int bccIndex = 0;
      for( std::list<MailAddressInfo>::iterator it = receivers.bccList.begin(); it != receivers.bccList.end(); ++it )
      {
         std::string sBccIndex;
         GStringUtils::format(sBccIndex, "%d", bccIndex);
         params.clear();
         param_i.value = "2";
         param_i.isNumeric = true;
         params.push_back(param_i);
         param_i.value = sBccIndex;
         param_i.isNumeric = true;
         params.push_back(param_i);
         param_i.value = (*it).address;
         param_i.isNumeric = false;
         params.push_back(param_i);
         param_i.value = (*it).accName;
         param_i.isNumeric = false;
         params.push_back(param_i);
         writeCommand("AT+SMTPRCPT", params, responseBuff);
      }
   }

   //Subject
   params.clear();
   param_i.value = subject;
   param_i.isNumeric = false;
   params.push_back(param_i);
   writeCommand("AT+SMTPSUB", params, responseBuff, 1);

   //body
   std::string sBodyLength;
   GStringUtils::format(sBodyLength, "%d", body.length());
   params.clear();
   param_i.value = sBodyLength;
   param_i.isNumeric = true;
   params.push_back(param_i);
   writeCommand("AT+SMTPBODY", params, responseBuff, 3);
   if( responseBuff.find("DOWNLOAD") != std::string::npos )
   {
      params.clear();
      writeCommand(body, params, responseBuff, 1);
   }
   //send
   params.clear();
   if( writeCommand("AT+SMTPSEND", params, responseBuff, 15) )
   {
      //try to read the response in 150 secs
      for( int i = 0; i < 150; i++ )
      {
         Task_Delay(ST_GetClocksPerSecond());
         readBuffer(responseBuff);
         if( responseBuff.find("+SMTPSEND:") == std::string::npos )
         {
            continue;
         }

         if( responseBuff.find("SMTPSEND: 1") != std::string::npos )
         {
            ret = true;
         }
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
