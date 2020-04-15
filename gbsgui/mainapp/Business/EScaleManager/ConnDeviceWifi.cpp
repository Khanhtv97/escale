//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ConnDeviceWifi.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "ConnDeviceWifi.hpp"
#include "PrinterManager.hpp"
#include "SaveDataToFlash.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
ConnDeviceWifi::ConnDeviceWifi() : ConnDevice(SPN_COM3)
{
}

//////////////////////////////////////////////////////////////////////////////////////
ConnDeviceWifi::~ConnDeviceWifi()
{
}

//////////////////////////////////////////////////////////////////////////////////////
ConnDeviceWifi& ConnDeviceWifi::getInstance()
{
   static ConnDeviceWifi instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::init()
{
   bool ret = false;
   _DTraceDebug("*** ConnDeviceWifi INIT ***");

   m_deviceId = "";

   m_deviceBuffer.reset();

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

   if( ret )
   {
      if( getWifiMode() != DWM_STATION_SOFTAP )
      {
         ret = setWifiMode(DWM_STATION_SOFTAP);
      }
   }

   viewVersion();

   if( getWifiDhcp() != DWDHCP_SOFTAP_STATION )
   {
      setWifiDhcp(DWDHCP_SOFTAP_STATION);
   }

   SoftAPInfo softApInfo;
   getSoftApCfg(softApInfo);

   if( softApInfo.ssid.find("1078SW") == std::string::npos ||
       softApInfo.ecn == ECN_OPEN ||
       softApInfo.password.length() < 8 )
   {
      softApInfo.ssid = "1078SW-WIFI";
      softApInfo.password = "12345678";
      softApInfo.ecn = ECN_WPA2_PSK;
      softApInfo.channel = 5;
      softApInfo.maxConnection = 2;
      softApInfo.ssidHidden = 0;
      setSoftApCfg(softApInfo);
   }

   DeviceWifiLocalIp localIp;
   if( getLocalIpAddress(localIp) )
   {
      setDeviceId(localIp.stMacAddr);
   }

   setTcpServer(SETTING_ON);

   time_t datetime;
   reqDateTime(datetime);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setStage(ConnDeviceStage stage)
{
   if( m_deviceStage != stage )
   {
      m_deviceStage = stage;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::deinit()
{
   m_deviceId = "";
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::isInitialized()
{
   return (bool)( !m_deviceId.empty() );;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::initSession()
{
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::termSession()
{
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::isReady()
{
   std::string responseBuff;
   std::list<CmdParameter> params;
   params.clear();
   lockScanBuffer(true);
   bool ret = writeCommand("AT", params, responseBuff, 1);
   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::reset()
{
   std::string responseBuff;
   std::list<CmdParameter> params;
   params.clear();
   lockScanBuffer(true);
   bool ret = writeCommand("AT+RST", params, responseBuff, 1);
   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::viewVersion()
{
   std::string responseBuff;
   std::list<CmdParameter> params;
   params.clear();
   lockScanBuffer(true);
   bool ret = writeCommand("AT+GMR", params, responseBuff, 1);
   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnDeviceWifi::lockScanBuffer(bool locked, int key)
{
   if( !locked )
   {
      if( key == m_deviceBuffer.ownerKey )
      {
         m_deviceBuffer.isLocked = false;
         m_deviceBuffer.ownerKey = 0;
      }
   }
   else
   {
      if( m_deviceBuffer.ownerKey == 0 && key > 0 )
      {
         m_deviceBuffer.isLocked = true;
         m_deviceBuffer.ownerKey = key;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::scanDataRequest(std::string& oDataBuff)
{
   bool ret = false;
   oDataBuff = "";

   if( !m_deviceBuffer.isLocked )
   {
      readBuffer(oDataBuff);
      if( !oDataBuff.empty() )
      {
         if( (oDataBuff.find("+IPD,0,") != std::string::npos ||
              oDataBuff.find("+IPD,1,") != std::string::npos ||
              oDataBuff.find("+IPD,2,") != std::string::npos)&&
              oDataBuff.find(":GET") != std::string::npos &&
              oDataBuff.find("HTTP")  != std::string::npos )
         {
            ret = true;
         }
      }
   }
   else
   {
      _DTraceDebug("Buffer is busy...");
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::respDataRequest(const int connId, const std::string& iDataBuff)
{
   bool ret = false;
   U32 dataLength = iDataBuff.length();
   size_t startPos = 0;

   ret = (bool)(dataLength > startPos);

   while( startPos < dataLength )
   {
      std::string inPartialBuff = "";
      std::string outPartialBuff = "";

      if( dataLength - startPos > 2044 )
      {
         inPartialBuff = iDataBuff.substr(startPos, 2044);
      }
      else
      {
         inPartialBuff = iDataBuff.substr(startPos);
      }


      ret &= sendData(connId, inPartialBuff, outPartialBuff);
      startPos += 2044;
      Task_Delay(ST_GetClocksPerSecond()/5);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::sendData(const int connId, const std::string& iDataBuff, std::string& oDataResp)
{
   bool ret = false;

   if( iDataBuff.empty() )
   {
      return ret;
   }

   lockScanBuffer(true, 2);

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   if( connId >= 0 )
   {
      GStringUtils::format(param_i.value, "%d", connId);
      param_i.isNumeric = true;
      params.push_back(param_i);
   }

   GStringUtils::format(param_i.value, "%d", iDataBuff.length());
   param_i.isNumeric = true;
   params.push_back(param_i);

   writeCommand("AT+CIPSEND", params, responseBuff, 1);
   if( responseBuff.find(">") != std::string::npos )
   {
      params.clear();
      writeCommand(iDataBuff, params, responseBuff);
      if( responseBuff.find("SEND OK") != std::string::npos )
      {
         ret = true;
      }
      else
      {
         for( int i = 0; i < RESPONSE_MAX_TIMEOUT; ++i )
         {
            Task_Delay(ST_GetClocksPerSecond()/10);
            readBuffer(responseBuff);
            if( responseBuff.find("SEND OK") != std::string::npos )
            {
               ret = true;
               break;
            }
         }
      }
   }

   oDataResp.assign(responseBuff);
   lockScanBuffer(false, 2);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setSizeOfSslBuffer(int size)
{
   bool ret = false;
   if( size < 2048 || size > 4096 )
   {
      //Esp8266 range of SSL buffer is 2048 - 4096
      return ret;
   }

   lockScanBuffer(true, 2);

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;
   params.clear();
   GStringUtils::format(param_i.value, "%d", size);
   param_i.isNumeric = true;
   params.push_back(param_i);

   ret = writeCommand("AT+CIPSSLSIZE", params, responseBuff, 1);

   lockScanBuffer(false, 2);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::startConnection(int connId, std::string type, std::string url, std::string port)
{
   bool ret = false;

   lockScanBuffer(true);
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;
   params.clear();
   GStringUtils::format(param_i.value, "%d", connId);
   param_i.isNumeric = true;
   params.push_back(param_i);
   param_i.value = type;
   param_i.isNumeric = false;
   params.push_back(param_i);
   param_i.value = url;
   param_i.isNumeric = false;
   params.push_back(param_i);
   param_i.value = port;
   param_i.isNumeric = true;
   params.push_back(param_i);

   ret = writeCommand("AT+CIPSTART", params, responseBuff, 1);

   //flush buffer temporary
   for( int i = 0; i < RESPONSE_MAX_TIMEOUT; ++i )
   {
      if( responseBuff.find("\r\nOK\r\n") != std::string::npos ||
          responseBuff.find("ALREADY CONNECTED") != std::string::npos )
      {
         ret = true;
         break;
      }
      else if( responseBuff.find("\r\nERROR\r\n") != std::string::npos ||
               responseBuff.find("\r\nFAIL\r\n")  != std::string::npos )
     {
        break;
     }

     Task_Delay(ST_GetClocksPerSecond()/10);
     readBuffer(responseBuff);
   }

   _DTraceDebug("%s", responseBuff.c_str());

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::startConnection(int connId, const UrlInfo& urlInf)
{
   bool ret = false;

   if( urlInf.ssl == SETTING_ON )
   {
      _DTraceDebug("SSL ON");
      if( setSizeOfSslBuffer(4096) )
      {
         ret = startConnection(connId, "SSL", urlInf.url, urlInf.port);
      }
      else
      {
         _DTraceDebug("Can not setSizeOfSslBuffer");
      }
   }
   else
   {
      _DTraceDebug("SSL OFF");
      ret = startConnection(connId, "TCP", urlInf.url, urlInf.port);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::closeConnection(int connId)
{
   lockScanBuffer(true);

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;
   params.clear();
   GStringUtils::format(param_i.value, "%d", connId);
   param_i.isNumeric = true;
   params.push_back(param_i);

   bool ret = writeCommand("AT+CIPCLOSE", params, responseBuff);

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setTcpServerTimeout(int timeOut)
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;
   params.clear();
   GStringUtils::format(param_i.value, "%d", timeOut);
   param_i.isNumeric = true;
   params.push_back(param_i);

   lockScanBuffer(true);
   bool ret = writeCommand("AT+CIPSTO", params, responseBuff);
   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
ConnDeviceType ConnDeviceWifi::getConnDeviceType()
{
   return CDT_WIFI;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::submitData(ScaleItemData& itemData, bool isDeleted)
{
   bool ret = false;

   lockScanBuffer(true, 3);

   UrlInfo submitingDataUrl;
   _U_.getSubmitDataUrl(submitingDataUrl);

   if( startConnection(MAX_CONNECTION_ID, submitingDataUrl) )
   {
      std::list<std::string> params;
      std::string strHttpRequest;
      std::string strParam_i;

      //time
      GStringUtils::format(strParam_i, "%d", itemData.timeStamp);
      params.push_back(strParam_i);
      //deviceid
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

      buildHttpRequest(strHttpRequest, HTTP_REQ_GET, CHS_SUBMIT_DATA, params);

      std::string responseBuff;
      std::string sessionClosed;
      GStringUtils::format(sessionClosed, "%d,CLOSED", MAX_CONNECTION_ID);

      if( sendData(MAX_CONNECTION_ID, strHttpRequest, responseBuff) )
      {
         //flush buffer temporary
         for( int i = 0; i < RESPONSE_MAX_TIMEOUT; ++i )
         {
            if( responseBuff.find(sessionClosed) != std::string::npos )
            {
               _DTraceDebug("%s", responseBuff.c_str());
               ret = true;
               break;
            }
            Task_Delay(ST_GetClocksPerSecond()/10);
            readBuffer(responseBuff);
         }
      }

      if( ret )
      {
         U32 uResult = 0;
         JsonParser jsParser;
         Json::Value nodeRoot;

         if( jsParser.getJsonObject(responseBuff, nodeRoot) )
         {
            uResult = jsParser.getUintValueNode(nodeRoot, "datalog");
            ret = (uResult == HTTP_200);
         }

         std::list<std::string> newRateTables;
         if( jsParser.lookForRateTables(nodeRoot, newRateTables) )
         {
            RateManager::getInstance().setPendingRateTable(newRateTables);
         }
      }
   }

   lockScanBuffer(false, 3);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::submitData(ScaleWeightData& itemData)
{
   bool ret = false;
   UrlInfo submitingDataUrl;
   _U_.getSubmitDataUrl(submitingDataUrl);

   lockScanBuffer(true, 3);

   if( startConnection(MAX_CONNECTION_ID, submitingDataUrl) )
   {
      std::list<std::string> params;
      std::string strHttpRequest;
      std::string strParam_i;

      //time
      GStringUtils::format(strParam_i, "%d", itemData.timestamp);
      params.push_back(strParam_i);
      //deviceid
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

      std::string responseBuff;
      std::string sessionClosed;
      GStringUtils::format(sessionClosed, "%d,CLOSED", MAX_CONNECTION_ID);

      if( sendData(MAX_CONNECTION_ID, strHttpRequest, responseBuff) )
      {
         for( int i = 0; i < RESPONSE_MAX_TIMEOUT; ++i )
         {
            //flush buffer temporary
            if( responseBuff.find(sessionClosed) != std::string::npos )
            {
               _DTraceDebug("%s", responseBuff.c_str());
               ret = true;
               break;
            }
            Task_Delay(ST_GetClocksPerSecond()/10);
            readBuffer(responseBuff);
         }
      }

      if( ret )
      {
         U32 uResult = 0;
         JsonParser jsParser;
         Json::Value nodeRoot;
         if( jsParser.getJsonObject(responseBuff, nodeRoot) )
         {
            uResult = jsParser.getUintValueNode(nodeRoot, "logdata");
            ret = (uResult == HTTP_200);
         }

         std::list<std::string> newRateTables;
         if( jsParser.lookForRateTables(nodeRoot, newRateTables) )
         {
            RateManager::getInstance().setPendingRateTable(newRateTables);
         }
      }
   }

   lockScanBuffer(false, 3);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::submitEmail(std::string& content)
{
   bool ret = true;
   //at the moment, does not support
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::submitSMS(std::string& content)
{
   bool ret = true;
   //at the moment, does not support
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::buildHttpRequest(std::string& outGetService, ConnHttpRequestType httpReqType, ConnHttpService httpService, std::list<std::string>& params)
{
   bool ret = true;
   UrlInfo serverUrl;
   std::string httpServiceName;
   std::string httpReqParams;
   _U_.getSubmitDataUrl(serverUrl);
   getHttpServiceName(httpServiceName, httpService);
   buildHttpParams(httpReqParams, httpService, httpReqType, params);

   if( httpReqType == HTTP_REQ_GET )
   {
      outGetService = "GET ";
      outGetService += httpServiceName;
      outGetService += "?";
      outGetService += httpReqParams;
      outGetService += " HTTP/1.1\r\nHost: ";
      outGetService += serverUrl.url;
      outGetService += "\r\n\r\n";
   }
   else if( httpReqType == HTTP_REQ_POST )
   {
      std::string contentLength = "";
      outGetService = "POST ";
      outGetService += httpServiceName;
      outGetService += " HTTP/1.1\r\nHost: ";
      outGetService += serverUrl.url;
      outGetService += "\r\n";
      outGetService += "Content-Type: application/x-www-form-urlencoded";
      outGetService += "\r\n";
      GStringUtils::format(contentLength, "Content-Length: %d", httpReqParams.length());
      outGetService += contentLength;
      outGetService += "\r\n\r\n";
      outGetService += httpReqParams;
      outGetService += "\r\n\r\n";
   }
   else
   {
      outGetService = "HEAD HTTP/1.1";
   }

   _DTraceDebug("HTTPREQ = %s", outGetService.c_str());

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
DeviceWifiMode ConnDeviceWifi::getWifiMode()
{
   DeviceWifiMode ret = DWM_NONE;
   std::string responseBuff;
   std::list<CmdParameter> params;
   params.clear();

   lockScanBuffer(true);

   if( writeCommand("AT+CWMODE?", params, responseBuff, 1) )
   {
      size_t pos = responseBuff.find("+CWMODE:");
      if( pos != std::string::npos )
      {
         pos = responseBuff.find(':', pos);
         if( pos != std::string::npos )
         {
            std::string strCwMode = GStringUtils::trimAll(responseBuff.substr(pos+1,1));
            _DTraceDebug("strCwMode = %s", strCwMode.c_str());
            ret = (DeviceWifiMode)GStringUtils::str2int(strCwMode);
         }
      }
   }

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setWifiMode(DeviceWifiMode cwMode)
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   GStringUtils::format(param_i.value, "%d", cwMode);
   param_i.isNumeric = true;
   params.push_back(param_i);

   //setting wifi mode and saving to device's flash
   lockScanBuffer(true);
   bool ret = writeCommand("AT+CWMODE_DEF", params, responseBuff, 1);
   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
DeviceWifiDhcp ConnDeviceWifi::getWifiDhcp()
{
   DeviceWifiDhcp ret = DWDHCP_NONE;
   std::string responseBuff;
   std::list<CmdParameter> params;
   params.clear();

   lockScanBuffer(true);

   if( writeCommand("AT+CWDHCP?", params, responseBuff, 1) )
   {
      size_t pos = responseBuff.find("+CWDHCP:");
      if( pos != std::string::npos )
      {
         pos = responseBuff.find(':', pos);
         if( pos != std::string::npos )
         {
            std::string strCwDhcp = GStringUtils::trimAll(responseBuff.substr(pos+1,1));
            _DTraceDebug("strCwDhcp = %s", strCwDhcp.c_str());
            ret = (DeviceWifiDhcp)GStringUtils::str2int(strCwDhcp);
         }
      }
   }

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setWifiDhcp(DeviceWifiDhcp cwDhcp)
{
   bool ret = false;

   std::string responseBuff;
   CmdParameter mode;
   CmdParameter isEnable;
   std::list<CmdParameter> params;

   lockScanBuffer(true, 2);

   switch(cwDhcp)
   {
   case DWDHCP_NONE:
      //dhcp off all mode
      mode.value = "2";
      mode.isNumeric = true;
      isEnable.value = "0";
      isEnable.isNumeric = true;
      break;

   case DWDHCP_SOFTAP:
      //dhcp on mode softAP only
      mode.value = "0";
      mode.isNumeric = true;
      isEnable.value = "1";
      isEnable.isNumeric = true;
      break;

   case DWDHCP_STATION:
      //dhcp on mode station only
      mode.value = "1";
      mode.isNumeric = true;
      isEnable.value = "1";
      isEnable.isNumeric = true;
      break;

   case DWDHCP_SOFTAP_STATION:
      //dhcp on mode station only
      mode.value = "2";
      mode.isNumeric = true;
      isEnable.value = "1";
      isEnable.isNumeric = true;
      break;

   default:
      break;
   }

   params.clear();
   params.push_back(mode);
   params.push_back(isEnable);

   //setting wifi dhcp and saving to device's flash
   ret = writeCommand("AT+CWDHCP_DEF", params, responseBuff, 1);

   if( ret && (cwDhcp == DWDHCP_SOFTAP || cwDhcp == DWDHCP_SOFTAP_STATION) )
   {
      //config default for softAP IP
      CmdParameter pram_i;
      params.clear();
      pram_i.value = "192.168.1.1";   //ip
      pram_i.isNumeric = false;
      params.push_back(pram_i);
      pram_i.value = "192.168.1.1";   //gateway
      pram_i.isNumeric = false;
      params.push_back(pram_i);
      pram_i.value = "255.255.255.0"; //netmask
      pram_i.isNumeric = false;
      params.push_back(pram_i);

      ret = writeCommand("AT+CIPAP_DEF", params, responseBuff, 1);

      //config IP range
      if( ret )
      {
         params.clear();
         pram_i.value = "3";           //Enable setting the IP range
         pram_i.isNumeric = true;
         params.push_back(pram_i);
         pram_i.value = "3";           //lease time in minute
         pram_i.isNumeric = true;
         params.push_back(pram_i);
         pram_i.value = "192.168.1.3"; //start IP of the IP range
         pram_i.isNumeric = true;
         params.push_back(pram_i);
         pram_i.value = "192.168.1.5"; //end IP of the IP range
         pram_i.isNumeric = true;
         params.push_back(pram_i);

         ret = writeCommand("AT+CWDHCPS_DEF", params, responseBuff, 1);
      }
   }

   lockScanBuffer(false, 2);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setStationNetworkCfg(const DeviceWifiNetworkCfg& networkCfg)
{
   bool ret = false;

   if( networkCfg.dhcp == SETTING_ON )
   {
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::getStationNetworkCfg(DeviceWifiNetworkCfg& networkCfg)
{
   bool ret = false;

   std::string responseBuff;
   std::list<CmdParameter> params;
   params.clear();

   lockScanBuffer(true);

   ret = writeCommand("AT+CIPSTA?", params, responseBuff, 1);

   if( ret )
   {
      size_t posStart = 0;
      size_t posEnd = 0;

      //+CIPSTA:ip:"192.168.0.123"
      posStart = responseBuff.find("+CIPSTA:ip:");
      if( posStart != std::string::npos )
      {
         posEnd = responseBuff.find((char)ASCII_CR, posStart);
         if( posEnd != std::string::npos )
         {
            networkCfg.ipaddr = GStringUtils::trimAll(responseBuff.substr(posStart+12, posEnd-1-(posStart+12)));
         }
      }

      //+CIPSTA:gateway:"192.168.0.2"
      posStart = responseBuff.find("+CIPSTA:gateway:");
      if( posStart != std::string::npos )
      {
         posEnd = responseBuff.find((char)ASCII_CR, posStart);
         if( posEnd != std::string::npos )
         {
            networkCfg.gateway = GStringUtils::trimAll(responseBuff.substr(posStart+17, posEnd-1-(posStart+17)));
            //default seting
            if( networkCfg.dns1.empty() || networkCfg.dns1.find("0.0.0.0") != std::string::npos )
            {
               networkCfg.dns1 = networkCfg.gateway;
            }
            if( networkCfg.dns2.empty() || networkCfg.dns2.find("0.0.0.0") != std::string::npos )
            {
               networkCfg.dns2 = "8.8.8.8";
            }
         }
      }

      //+CIPSTA:netmask:"255.255.255.0"
      posStart = responseBuff.find("+CIPSTA:netmask:");
      if( posStart != std::string::npos )
      {
         posEnd = responseBuff.find((char)ASCII_CR, posStart);
         if( posEnd != std::string::npos )
         {
            networkCfg.netmask = GStringUtils::trimAll(responseBuff.substr(posStart+17, posEnd-1-(posStart+17)));
         }
      }
   }

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::getLocalIpAddress(DeviceWifiLocalIp& localIp)
{
   bool ret = false;
   std::string responseBuff;
   std::list<CmdParameter> params;
   params.clear();

   lockScanBuffer(true);

   ret = writeCommand("AT+CIFSR", params, responseBuff, 1);

   if( ret )
   {
      size_t posStart = 0;
      size_t posEnd = 0;

      //+CIFSR:APIP,"192.168.4.1"
      posStart = responseBuff.find("+CIFSR:APIP");
      if( posStart != std::string::npos )
      {
         posEnd = responseBuff.find((char)ASCII_CR, posStart);
         if( posEnd != std::string::npos )
         {
            localIp.apIpAddr = GStringUtils::trimAll(responseBuff.substr(posStart+13, posEnd-1-(posStart+13)));
         }
      }

      //+CIFSR:APMAC,"5e:cf:7f:27:c6:a0"
      posStart = responseBuff.find("+CIFSR:APMAC");
      if( posStart != std::string::npos )
      {
         posEnd = responseBuff.find((char)ASCII_CR, posStart);
         if( posEnd != std::string::npos )
         {
            localIp.apMacAddr = GStringUtils::trimAll(responseBuff.substr(posStart+14, posEnd-1-(posStart+14)));
         }
      }

      //+CIFSR:STAIP,"192.168.0.99"
      posStart = responseBuff.find("+CIFSR:STAIP");
      if( posStart != std::string::npos )
      {
         posEnd = responseBuff.find((char)ASCII_CR, posStart);
         if( posEnd != std::string::npos )
         {
            localIp.stIpAddr = GStringUtils::trimAll(responseBuff.substr(posStart+14, posEnd-1-(posStart+14)));
         }
      }

      //+CIFSR:STAMAC,"5c:cf:7f:27:c6:a0"
      posStart = responseBuff.find("+CIFSR:STAMAC");
      if( posStart != std::string::npos )
      {
         posEnd = responseBuff.find((char)ASCII_CR, posStart);
         if( posEnd != std::string::npos )
         {
            localIp.stMacAddr = GStringUtils::trimAll(responseBuff.substr(posStart+15, posEnd-1-(posStart+15)));
         }
      }

      _DTraceDebug("Local AP MAC: %s", localIp.apMacAddr.c_str());
      _DTraceDebug("Local AP IP : %s", localIp.apIpAddr.c_str());
      _DTraceDebug("Local Station MAC: %s", localIp.stMacAddr.c_str());
      _DTraceDebug("Local Station IP : %s", localIp.stIpAddr.c_str());
   }

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::parseAP(std::string& apRawData, AccessPointInfo& apInfo)
{
   int partialId = 0;
   char* pStrPartial = strtok((char*)apRawData.c_str(), ",");

   while( pStrPartial != NULL )
   {
      partialId++;
      std::string sPartial;
      sPartial.assign(pStrPartial);

      if( partialId == 1 )
      {
         apInfo.ecn = (EncodeNetwork)(GStringUtils::str2int(GStringUtils::trimAll(sPartial)));
      }
      else if( partialId == 2 )
      {
         apInfo.ssid = GStringUtils::trimAll(sPartial);
         GStringUtils::remove(apInfo.ssid, "\"");
      }
      else if( partialId == 3 )
      {
         apInfo.signalStrength = GStringUtils::str2int(GStringUtils::trimAll(sPartial));
      }
      else if( partialId == 4 )
      {
         apInfo.mac = GStringUtils::trimAll(sPartial);
         GStringUtils::remove(apInfo.mac, "\"");
      }
      else if( partialId == 5 )
      {
         apInfo.channel = GStringUtils::str2int(GStringUtils::trimAll(sPartial));
      }
      else if( partialId == 6 )
      {
         apInfo.freqOffset = GStringUtils::str2int(GStringUtils::trimAll(sPartial));
      }
      else if( partialId == 7 )
      {
         apInfo.freqCalibration = GStringUtils::str2int(GStringUtils::trimAll(sPartial));
      }

      pStrPartial = strtok(NULL, ",");
   }

   return (bool)(apInfo.ssid.length() != 0);
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::getListAP(std::list<AccessPointInfo>& apList, bool refresh)
{
   if( !m_apList.empty() && !refresh )
   {
      apList = m_apList;
      return true;
   }

   lockScanBuffer(true);

   bool ret = false;
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;
   apList.clear();

   //config get list AP
   //AT+CWLAPOPT=1,127
   params.clear();
   param_i.value = "1";   //order by signal strength
   param_i.isNumeric = true;
   params.push_back(param_i);
   param_i.value = "127"; //show all field
   param_i.isNumeric = true;
   params.push_back(param_i);

   ret = writeCommand("AT+CWLAPOPT", params, responseBuff, 1);

   if( !ret )
   {
      return ret;
   }

   params.clear();
   ret = writeCommand("AT+CWLAP", params, responseBuff, 3);

   if( ret )
   {
      size_t posStart = 0;
      size_t posEnd = 0;
      posStart = responseBuff.find("+CWLAP:(");
      while( posStart != std::string::npos )
      {
         posStart += 8;
         posEnd = responseBuff.find(')', posStart);
         if( posEnd != std::string::npos )
         {
            std::string apRaw_i = GStringUtils::trimAll(responseBuff.substr(posStart, posEnd-posStart));
            AccessPointInfo apInfo_i;
            if( parseAP(apRaw_i, apInfo_i) )
            {
               apList.push_back(apInfo_i);
            }

            posStart = responseBuff.find("+CWLAP:(", posEnd);
         }
         else
         {
            posStart = std::string::npos;
         }
      }

      ret = !apList.empty();
   }

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
//connecting to a router to access to the internet first,
//then it is able to call the setDnsServer
//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setDnsServer(std::string dnsUrl)
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   param_i.value.assign(dnsUrl);
   param_i.isNumeric = false;
   params.push_back(param_i);

   lockScanBuffer(true);
   bool ret = writeCommand("AT+CIPDOMAIN", params, responseBuff, 1);
   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
NetworkConnStatus ConnDeviceWifi::getNetworkStatus()
{
   NetworkConnStatus ret = NCS_UNKNOWN;
   std::string responseBuff;
   std::list<CmdParameter> params;

   params.clear();

   lockScanBuffer(true);

   if( writeCommand("AT+CIPSTATUS", params, responseBuff, 1) )
   {
      size_t pos = responseBuff.find("STATUS:");
      if( pos != std::string::npos )
      {
         pos = responseBuff.find(':', pos);
         if( pos != std::string::npos )
         {
            std::string strNwStatus = GStringUtils::trimAll(responseBuff.substr(pos+1,1));
            _DTraceDebug("strNwStatus = %s", strNwStatus.c_str());
            ret = (NetworkConnStatus)GStringUtils::str2int(strNwStatus);
         }
      }
   }

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setSoftApCfg(const SoftAPInfo& softApInfo)
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   lockScanBuffer(true);

   params.clear();
   param_i.value.assign(softApInfo.ssid);
   param_i.isNumeric = false;
   params.push_back(param_i);
   param_i.value.assign(softApInfo.password);
   param_i.isNumeric = false;
   params.push_back(param_i);
   GStringUtils::format(param_i.value, "%d", softApInfo.channel);
   param_i.isNumeric = true;
   params.push_back(param_i);
   GStringUtils::format(param_i.value, "%d", softApInfo.ecn);
   param_i.isNumeric = true;
   params.push_back(param_i);
   GStringUtils::format(param_i.value, "%d", softApInfo.maxConnection);
   param_i.isNumeric = true;
   params.push_back(param_i);
   GStringUtils::format(param_i.value, "%d", softApInfo.ssidHidden);
   param_i.isNumeric = true;
   params.push_back(param_i);

   bool ret = writeCommand("AT+CWSAP_DEF", params, responseBuff, 1);

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::getSoftApCfg(SoftAPInfo& softApInfo)
{
   bool ret = false;

   std::string responseBuff;
   std::list<CmdParameter> params;

   params.clear();

   lockScanBuffer(true);

   ret = writeCommand("AT+CWSAP?", params, responseBuff, 1);

   if( ret )
   {
      size_t pos = responseBuff.find("+CWSAP:");
      if( pos != std::string::npos )
      {
         pos = responseBuff.find(':', pos);
         if( pos != std::string::npos )
         {
            std::string strCurrentSoftAp = GStringUtils::trimAll(responseBuff.substr(pos+1));
            _DTraceDebug("strCurrentSoftAp = %s", strCurrentSoftAp.c_str());

            int partialId = 0;
            char* pStrPartial = strtok((char*)strCurrentSoftAp.c_str(), ",");

            while( pStrPartial != NULL )
            {
               partialId++;
               std::string sPartial;
               sPartial.assign(pStrPartial);

               if( partialId == 1 )
               {
                  softApInfo.ssid = GStringUtils::trimAll(sPartial);
                  GStringUtils::remove(softApInfo.ssid, "\"");
               }
               else if( partialId == 2 )
               {
                  softApInfo.password = GStringUtils::trimAll(sPartial);
                  GStringUtils::remove(softApInfo.password, "\"");
               }
               else if( partialId == 3 )
               {
                  softApInfo.channel = (U8)(GStringUtils::str2int(GStringUtils::trimAll(sPartial)));
               }
               else if( partialId == 4 )
               {
                  softApInfo.ecn = (EncodeNetwork)(GStringUtils::str2int(GStringUtils::trimAll(sPartial)));
               }
               else if( partialId == 5 )
               {
                  softApInfo.maxConnection = (U8)(GStringUtils::str2int(GStringUtils::trimAll(sPartial)));
               }
               else if( partialId == 6 )
               {
                  softApInfo.ssidHidden = (U8)(GStringUtils::str2int(GStringUtils::trimAll(sPartial)));
               }

               pStrPartial = strtok(NULL, ",");
            }
         }
      }
   }

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setAutoConnectAp(U8 enable)
{
   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   GStringUtils::format(param_i.value, "%d", enable);
   param_i.isNumeric = true;
   params.push_back(param_i);

   lockScanBuffer(true);

   bool ret = writeCommand("AT+CWAUTOCONN", params, responseBuff, 1);

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::connectAP(std::string ssid, std::string passwd, std::string macaddr)
{
   bool ret = false;

   lockScanBuffer(true);

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   param_i.value.assign(ssid);
   param_i.isNumeric = false;
   params.push_back(param_i);
   param_i.value.assign(passwd);
   param_i.isNumeric = false;
   params.push_back(param_i);

   if( !macaddr.empty() )
   {
      param_i.value.assign(macaddr);
      param_i.isNumeric = false;
      params.push_back(param_i);
   }

   writeCommand("AT+CWJAP_DEF", params, responseBuff, 1);

   for( int i = 0; i < RESPONSE_MAX_TIMEOUT; ++i )
   {
      Task_Delay(ST_GetClocksPerSecond()/10);

      readBuffer(responseBuff);

      if( responseBuff.find("\r\nOK\r\n") != std::string::npos )
      {
        ret = true;
        break;
      }
      else if( responseBuff.find("\r\nERROR\r\n") != std::string::npos ||
               responseBuff.find("\r\nFAIL\r\n")  != std::string::npos)
      {
         break;
      }
   }

   _DTraceDebug("Connect AP: %s", responseBuff.c_str());

   if( ret == true )
   {
      setAutoConnectAp(SETTING_ON);
   }

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::setTcpServer(U8 mode, U16 port)
{
   bool ret = false;

   lockScanBuffer(true);

   std::string responseBuff;
   CmdParameter param_i;
   std::list<CmdParameter> params;

   params.clear();
   GStringUtils::format(param_i.value, "%d", 1);
   param_i.isNumeric = true;
   params.push_back(param_i);

   //Server can only be created when AT+CIPMUX=1
   ret = writeCommand("AT+CIPMUX", params, responseBuff, 1);

   params.clear();
   GStringUtils::format(param_i.value, "%d", mode);
   param_i.isNumeric = true;
   params.push_back(param_i);
   GStringUtils::format(param_i.value, "%d", port);
   param_i.isNumeric = true;
   params.push_back(param_i);
   ret = writeCommand("AT+CIPSERVER", params, responseBuff, 1);

   lockScanBuffer(false);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::reqDateTime(time_t& oDateTimeStamp)
{
   bool ret = false;

   lockScanBuffer(true, 3);

   UrlInfo serverUrl;
   _U_.getSubmitDataUrl(serverUrl);

   if( startConnection(MAX_CONNECTION_ID, serverUrl) )
   {
      std::list<std::string> params;
      std::string strHttpRequest;
      //deviceid
      params.push_back(m_deviceId);

      buildHttpRequest(strHttpRequest, HTTP_REQ_GET, CHS_UPDATE_DATETIME, params);

      std::string responseBuff;
      std::string sessionClosed;
      GStringUtils::format(sessionClosed, "%d,CLOSED", MAX_CONNECTION_ID);

      if( sendData(MAX_CONNECTION_ID, strHttpRequest, responseBuff) )
      {
         //flush buffer temporary
         for( int i = 0; i < RESPONSE_MAX_TIMEOUT; ++i )
         {
            if( responseBuff.find(sessionClosed) != std::string::npos )
            {
               _DTraceDebug("%s", responseBuff.c_str());
               ret = true;
               break;
            }
            Task_Delay(ST_GetClocksPerSecond()/10);
            readBuffer(responseBuff);
         }
      }

      if( ret )
      {
         U32 uResult = 0;
         JsonParser jsParser;
         Json::Value nodeRoot;
         if( jsParser.getJsonObject(responseBuff, nodeRoot) )
         {
            uResult = jsParser.getUintValueNode(nodeRoot, "currenttimestamp");
            if( uResult == HTTP_404 )
            {
               //device is not registered
               oDateTimeStamp = (time_t)(uResult + SYSTEM_TIME_BASE_INIT);
            }
            else
            {
               oDateTimeStamp = (time_t)uResult;
            }

            //Set system time
            _DTraceDebug("Time stamp: %d", oDateTimeStamp);
            _S_.setBaseTime(oDateTimeStamp);

            //Look for if there is a new rate table
            std::list<std::string> newRateTables;
            if( jsParser.lookForRateTables(nodeRoot, newRateTables) )
            {
               RateManager::getInstance().setPendingRateTable(newRateTables);
            }

            //init an active rate table
            U32 activeRateTableId = RateManager::getInstance().lookForActiveRateTable();
            if( activeRateTableId != _U_.getActiveRateTableId() )
            {
               ScaleManager::getInstance().updateRateTable();
               _U_.setActiveRateTableId(activeRateTableId);
               _Sa_.serialize(SERIALIZABLE_PARTITION_1);
            }
         }
      }
   }

   lockScanBuffer(false, 3);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
ConnHttpStatusCode ConnDeviceWifi::reqRegister(const std::string& deviceId, const std::string& userId, const std::string& regContent)
{
   ConnHttpStatusCode ret = HTTP_500;
   bool canContinue = false;
   lockScanBuffer(true, 3);

   UrlInfo serverUrl;
   _U_.getSubmitDataUrl(serverUrl);

   if( startConnection(MAX_CONNECTION_ID, serverUrl) )
   {
      std::list<std::string> params;
      std::string strHttpRequest;
      //deviceid
      params.push_back(deviceId);
      //userId
      params.push_back(userId);
      //register's content
      params.push_back(regContent);

      buildHttpRequest(strHttpRequest, HTTP_REQ_GET, CHS_DEVICE_REGISTER, params);

      std::string responseBuff;
      std::string sessionClosed;
      GStringUtils::format(sessionClosed, "%d,CLOSED", MAX_CONNECTION_ID);

      if( sendData(MAX_CONNECTION_ID, strHttpRequest, responseBuff) )
      {
         //flush buffer temporary
         for( int i = 0; i < RESPONSE_MAX_TIMEOUT; ++i )
         {
            if( responseBuff.find(sessionClosed) != std::string::npos )
            {
               _DTraceDebug("%s", responseBuff.c_str());
               canContinue = true;
               break;
            }
            Task_Delay(ST_GetClocksPerSecond()/10);
            readBuffer(responseBuff);
         }
      }
      else
      {
         _DTraceDebug("sendData FAILED: %s", strHttpRequest.c_str());
      }

      if( canContinue )
      {
         JsonParser jsParser;
         Json::Value nodeRoot;
         if( jsParser.getJsonObject(responseBuff, nodeRoot) )
         {
            ret = (ConnHttpStatusCode)jsParser.getUintValueNode(nodeRoot, "deviceregister");
         }

         std::list<std::string> newRateTables;
         if( jsParser.lookForRateTables(nodeRoot, newRateTables) )
         {
            RateManager::getInstance().setPendingRateTable(newRateTables);
         }
      }
   }
   else
   {
      _DTraceDebug("startConnection FALIED !!!");
   }

   lockScanBuffer(false, 3);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnDeviceWifi::reqRateTable(const std::string& rateTableId, const int rateStatus)
{
   bool ret = false;

   lockScanBuffer(true, 3);

   UrlInfo serverUrl;
   _U_.getSubmitDataUrl(serverUrl);

   if( startConnection(MAX_CONNECTION_ID, serverUrl) )
   {
      std::list<std::string> params;
      std::string strHttpRequest;
      //deviceid
      params.push_back(m_deviceId);
      //userId
      params.push_back(rateTableId);
      //status
      if( rateStatus == REQ_RATETABLE_UPDATE_NEW )
      {
         //Turn off SoftAP first
         setTcpServer(SETTING_OFF);

         //Print a message
         ScaleMessageData messageData;
         messageData.type = PDT_WARNING;
         messageData.value = "Bat dau cap nhat bang gia !";
         PrinterManager::getInstance().hasData(messageData);

         params.push_back("new");
      }
      else if( rateStatus == REQ_RATETABLE_UPDATE_OK )
      {
         params.push_back("done");
      }
      else
      {
         params.push_back("unknown");
      }

      buildHttpRequest(strHttpRequest, HTTP_REQ_GET, CHS_RATE_TABLE, params);

      std::string responseBuff;
      std::string sessionClosed;
      GStringUtils::format(sessionClosed, "%d,CLOSED", MAX_CONNECTION_ID);

      if( sendData(MAX_CONNECTION_ID, strHttpRequest, responseBuff) )
      {
         //flush buffer temporary
         int iTimeout;
         bool jsonBeginFound = false;
         bool jsonEndFound = false;
         bool sessionClosedFound = false;

         for( iTimeout = 0; iTimeout < RESPONSE_MAX_TIMEOUT; ++iTimeout )
         {
            if( responseBuff.find(RESP_JSON_START) != std::string::npos )
            {
               jsonBeginFound = true;
            }

            if( responseBuff.find(RESP_JSON_END) != std::string::npos )
            {
               jsonEndFound = true;
            }

            if( responseBuff.find(sessionClosed) != std::string::npos )
            {
               sessionClosedFound = true;
            }

            if( rateStatus == REQ_RATETABLE_UPDATE_NEW && jsonBeginFound )
            {
               appendBufferIn(responseBuff);
            }

            if( jsonEndFound || sessionClosedFound )
            {
               _DTraceDebug("FOUND AT %d", iTimeout);
               _DTraceDebug("%s", responseBuff.c_str());
               ret = true;
               break;
            }

            readBuffer(responseBuff);

            if( !responseBuff.empty() )
            {
               //reset internal timeout
               iTimeout = 0;
            }
         }

         if( !ret && iTimeout == RESPONSE_MAX_TIMEOUT )
         {
            _DTraceDebug("Time out -> Failed");
         }
      }

      if( rateStatus == REQ_RATETABLE_UPDATE_NEW )
      {
         if( ret )
         {
            ret = parseBufferIn();
         }
         else
         {
            clearBufferIn();
         }

         //Turn on SoftAP
         setTcpServer(SETTING_ON);

         //Print a message
         ScaleMessageData messageData;
         messageData.type = PDT_WARNING;
         if( ret )
         {
            messageData.value = "Cap nhat bang gia thanh cong  !";
         }
         else
         {
            messageData.value = "Co loi khi cap nhat bang gia  !";
         }
         PrinterManager::getInstance().hasData(messageData);
      }
   }

   lockScanBuffer(false, 3);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
