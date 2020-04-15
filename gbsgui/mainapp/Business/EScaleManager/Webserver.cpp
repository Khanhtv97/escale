//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: Webserver.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "Webserver.hpp"
#include "HtmlDocument.hpp"
#include "GStringUtils.hpp"
#include "GUserSettings.hpp"
#include "GSystem.hpp"
#include "ConnDeviceWifi.hpp"
#include "PrinterManager.hpp"
#include "ScaleManager.hpp"
#include "SaveDataToFlash.hpp"

#define WEBSERVER_SESSION_TIMEOUT 211112 //3,5 minutes

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////
Webserver& Webserver::getInstance()
{
   static Webserver instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
Webserver::Webserver()
{
   m_curSession.clear();
   m_pDelegateSessionTimeout = DELEGATE(Webserver, this, onSessionTimeout);
   m_pSessionTimeout = GTimer::createTimer(m_pDelegateSessionTimeout, 5000, false);
   if( m_pSessionTimeout != NULL )
   {
      m_pSessionTimeout->restart();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
Webserver::~Webserver()
{
}

//////////////////////////////////////////////////////////////////////////////////////
void Webserver::onSessionTimeout(EVENT_HANDLER_PARAM)
{
   U32 now = GSystem::now();

   if( m_curSession.empty() )
   {
      return;
   }

   for( std::list<SessionInfo>::iterator it = m_curSession.begin(); it != m_curSession.end(); ++it )
   {
      if( (now - (*it).timeStamp) > WEBSERVER_SESSION_TIMEOUT )
      {
         _DTraceDebug("Session %s timeout", (*it).sessionId.c_str());
         it = m_curSession.erase(it);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
int Webserver::getIpdConnId(const std::string& ipdContent)
{
   int ret = -1; // not found
   size_t pos = ipdContent.find(",");

   if( pos != std::string::npos )
   {
      ret = GStringUtils::str2int(GStringUtils::trimAll(ipdContent.substr(0, pos)));
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
HttpMethod Webserver::getIpdMethod(const std::string& ipdContent)
{
   HttpMethod ret = HTTP_NONE;

   if( ipdContent.find(":GET") != std::string::npos )
   {
      ret = HTTP_GET;
   }
   else if( ipdContent.find(":POST") != std::string::npos )
   {
      ret = HTTP_POST;
   }
   else if( ipdContent.find(":HEAD") != std::string::npos )
   {
      ret = HTTP_HEAD;
   }
   else if( ipdContent.find(":PUSH") != std::string::npos )
   {
      ret = HTTP_PUSH;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool Webserver::getIpdParam(const std::string& ipdContent, const std::string property, std::string& value)
{
   bool ret = false;

   std::string sKey = property + "=";
   size_t startPos = ipdContent.find(sKey);
   size_t endPos = 0;

   if( startPos != std::string::npos )
   {
      startPos += sKey.length();
      endPos = ipdContent.find("&", startPos);

      if( endPos == std::string::npos )
      {
         value = GStringUtils::trimAll(ipdContent.substr(startPos));
      }
      else
      {
         value = GStringUtils::trimAll(ipdContent.substr(startPos, endPos-startPos));
      }

      decodeUrl(value);

      _DTraceDebug("Found %s>%s", sKey.c_str(), value.c_str());
      ret = true;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool Webserver::isExistentSession(std::string sessionId, bool& isVerified)
{
   bool ret = false;

   if( m_curSession.empty() )
   {
      return ret;
   }

   for( std::list<SessionInfo>::iterator it = m_curSession.begin(); it != m_curSession.end(); ++it )
   {
      if( (*it).sessionId.compare(sessionId) == 0 )
      {
         ret = true;
         isVerified = (*it).isVerified;
         (*it).timeStamp = GSystem::now();
         break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool Webserver::isClientConnecting()
{
   //std::string sessionId;
   //return isThereSessionLogged(sessionId);
   return !m_curSession.empty();
}

//////////////////////////////////////////////////////////////////////////////////////
bool Webserver::isThereSessionLogged(std::string& sessionId)
{
   bool ret = false;
   sessionId = "";

   if( m_curSession.empty() )
   {
      return ret;
   }

   for( std::list<SessionInfo>::iterator it = m_curSession.begin(); it != m_curSession.end(); ++it )
   {
      if( (*it).isVerified )
      {
         ret = (*it).isVerified;
         sessionId = (*it).sessionId;
         break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool Webserver::setLoggedSession(std::string sessionId)
{
   bool ret = false;

   if( m_curSession.empty() )
   {
      return ret;
   }

   for( std::list<SessionInfo>::iterator it = m_curSession.begin(); it != m_curSession.end(); ++it )
   {
      if( (*it).sessionId.compare(sessionId) == 0 )
      {
         ret = true;
         (*it).isVerified = true;
         (*it).timeStamp = GSystem::now();
         break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void Webserver::openSession(std::string& sessionId)
{
   std::string randSession;
   srand(time(NULL));
   GStringUtils::format(randSession, "%d", rand());

   _DTraceDebug("random session = %s", randSession.c_str());

   char* pSessionMd5 = NULL;
   pSessionMd5 = GStringUtils::md5((U8*)randSession.c_str(), randSession.length());

   if( pSessionMd5 != NULL )
   {
      sessionId.assign(pSessionMd5);
      free(pSessionMd5);
      pSessionMd5 = NULL;
   }

   SessionInfo sesionInf;
   sesionInf.sessionId = sessionId;
   sesionInf.isVerified = false;
   sesionInf.timeStamp = GSystem::now();
   m_curSession.push_back(sesionInf);

   _DTraceDebug("new session = %s", sessionId.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////
void Webserver::closeSessionAll()
{
   m_curSession.clear();
}

//////////////////////////////////////////////////////////////////////////////////////
void Webserver::closeSession(std::string sessionId)
{
   if( !m_curSession.empty() )
   {
      for( std::list<SessionInfo>::iterator it = m_curSession.begin(); it != m_curSession.end(); ++it )
      {
         if( (*it).sessionId.compare(sessionId) == 0 )
         {
            it = m_curSession.erase(it);
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool Webserver::setRequest(const std::string& clientRequest)
{
   bool ret = false;

   if( clientRequest.find("+IPD,") != std::string::npos )
   {
      m_lastRequest = clientRequest;
      decodeUrl(m_lastRequest);
      ret = true;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void Webserver::decodeUrl(std::string& ioUrl)
{
   GStringUtils::replaceAll(ioUrl, std::string("%253A"), std::string(":")); // '%253A' is ':'
   GStringUtils::replaceAll(ioUrl, std::string("%252F"), std::string("/")); // '%252F' is '/'
   GStringUtils::replaceAll(ioUrl, std::string("%253F"), std::string("?")); // '%253F' is '?'
   GStringUtils::replaceAll(ioUrl, std::string("%253D"), std::string("=")); // '%253D' is '='
   GStringUtils::replaceAll(ioUrl, std::string("%2526"), std::string("&")); // '%2526' is '&'
   GStringUtils::replaceAll(ioUrl, std::string("%2525"), std::string("%")); // '%2525' is '%'
   GStringUtils::replaceAll(ioUrl, std::string("%252B"), std::string("+")); // '%252B' is '+'
   GStringUtils::replaceAll(ioUrl, std::string("%252C"), std::string(",")); // '%252C' is ','
   GStringUtils::replaceAll(ioUrl, std::string("%2540"), std::string("@")); // '%2540' is '@'

   GStringUtils::replaceAll(ioUrl, std::string("%3A"),   std::string(":")); // '%3A' is ':'
   GStringUtils::replaceAll(ioUrl, std::string("%2F"),   std::string("/")); // '%2C' is '/'
   GStringUtils::replaceAll(ioUrl, std::string("%3F"),   std::string("?")); // '%3F' is '?'
   GStringUtils::replaceAll(ioUrl, std::string("%3D"),   std::string("=")); // '%3D' is '='
   GStringUtils::replaceAll(ioUrl, std::string("%26"),   std::string("&")); // '%26' is '&'
   GStringUtils::replaceAll(ioUrl, std::string("%25"),   std::string("&")); // '%25' is '%'
   GStringUtils::replaceAll(ioUrl, std::string("%2B"),   std::string("+")); // '%2B' is '+'
   GStringUtils::replaceAll(ioUrl, std::string("%2C"),   std::string(",")); // '%2C' is ','
   GStringUtils::replaceAll(ioUrl, std::string("%40"),   std::string("@")); // '%40' is '@'
}

//////////////////////////////////////////////////////////////////////////////////////
bool Webserver::getResponse(std::string& wsResponse, int& connId)
{
   bool ret = false;

   HtmlDocument htmlDoc;
   std::string sHeader(HTML_HEADER_OK);
   std::string sContent;
   std::string sHtmlBody;
   std::string sSessionId;

   sContent = "<html>\r\n";
   sContent = "<head>\r\n";
   sContent += "<meta charset=\"utf-8\">\r\n";
   sContent += "<meta name=\"viewport\" content=\"width=device-width\">\r\n";
   sContent += "<meta name=\"description\" content=\"GBS-EScale\">\r\n";
   sContent += "<meta name=\"author\" content=\"GBS Software Dept\">";
   sContent += "<title>GBS-EScale</title>\r\n";
   sContent += "</head>\r\n";
   sContent += "<body>\r\n";

   WebsrvNavigator wsNavigator = parseRequest(m_lastRequest, sSessionId, connId);

   switch( wsNavigator )
   {
   case WSN_LOGIN:
      htmlDoc.buildLogin(sSessionId, sHtmlBody);
      break;
   case WSN_LOGIN_FAILED:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_LOGINFAILED);
      break;
   case WSN_LOGIN_LIMIT:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_LOGINLIMIT);
      break;
   case WSN_LOG_OUT:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_LOGGEDOUT);
      break;
   case WSN_WIFI_LIST:
      htmlDoc.buildListWifi(sSessionId, sHtmlBody);
      break;
   case WSN_WIFI_JAP_OK:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_WIFI_JAP_OK);
      break;
   case WSN_WIFI_JAP_NOK:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_WIFI_JAP_NOK);
      break;
   case WSN_RATE_TABLE:
      htmlDoc.buildRateTable(sSessionId, sHtmlBody);
      break;
   case WSN_RATE_TABLE_EX:
      htmlDoc.buildRateTable(sSessionId, sHtmlBody, m_tmpIndex);
      break;
   case WSN_SETUP_SRV:
      htmlDoc.buildSetupSrv(sSessionId, sHtmlBody);
      break;
   case WSN_SETUP_SAP:
      htmlDoc.buildSetupSAP(sSessionId, sHtmlBody);
      break;
   case WSN_SETUP_PRT:
      htmlDoc.buildSetupPrinter(sSessionId, sHtmlBody);
      break;
   case WSN_SCALE_REG:
      htmlDoc.buildScaleRegister(sSessionId, sHtmlBody);
      break;
   case WSN_SCALE_REG_OK:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_REGISTER_OK);
      break;
   case WSN_SCALE_REG_NOK:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_REGISTER_NOK);
      break;
   case WSN_SCALE_REG_NOK_USER:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_REGISTER_NOK_USER);
      break;
   case WSN_SCALE_REG_NOK_EXIST:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_REGISTER_NOK_EXIST);
      break;
   case WSN_TEST_DEVICES:
      htmlDoc.buildTestDevice(sSessionId, sHtmlBody);
      break;
   case WSN_TEST_DEVICES_CONN_OK:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_CONNECT_OK);
      break;
   case WSN_TEST_DEVICES_CONN_NOK:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_CONNECT_NOK);
      break;
   case WSN_SAVE_SETTINGS_OK:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_SAVE_SETTINGS_OK);
      break;
   case WSN_SAVE_SETTINGS_NOK:
      htmlDoc.buildErrorMessage(sSessionId, sHtmlBody, HTTP_ERR_SAVE_SETTINGS_NOK);
      break;
   default:
      break;
   }

   sContent += sHtmlBody;
   sContent += "\r\n";
   sContent += "</body>\r\n";
   sContent += "</html>";

   std::string contentLength;
   GStringUtils::format(contentLength, "Content-Length: %d\r\n\r\n", sContent.length());
   sHeader += contentLength;

   if( wsNavigator == WSN_NONE )
   {
      wsResponse = "";
   }
   else
   {
      wsResponse = sHeader + sContent;
      ret = true;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::parseRequest(const std::string& clientRequest, std::string& sessionId, int& connId)
{
   WebsrvNavigator ret = WSN_LOGIN;
   bool isVerified = false;
   std::string ipdContent = "";

   size_t startPos = clientRequest.find("+IPD,");
   size_t endPos = std::string::npos;

   if( startPos != std::string::npos )
   {
      _DTraceDebug("Found IPD");
      startPos += 5;
      endPos = clientRequest.find("HTTP/");
      if( endPos != std::string::npos && endPos > startPos )
      {
         ipdContent = GStringUtils::trimAll(clientRequest.substr(startPos, endPos-startPos));
         _DTraceDebug("Extracted IPD: %s", ipdContent.c_str());
      }
      else
      {
         _DTraceDebug("Not found HTTP -> Login again");
         return ret;
      }
   }
   else
   {
      return WSN_NONE;
   }

   if( ipdContent.empty() )
   {
      return ret;
   }

   connId = getIpdConnId(ipdContent);

   if( connId < 0 )
   {
      return ret;
   }

   if( getIpdParam(ipdContent, "id", sessionId) )
   {
      if( !isExistentSession(sessionId, isVerified) )
      {
         _DTraceDebug("not found session = %s", sessionId.c_str());
         std::string sessionIsLogging;
         if( isThereSessionLogged(sessionIsLogging) )
         {
            _DTraceDebug("The session %s is logged, please wait", sessionIsLogging.c_str());
            ret = WSN_LOGIN_LIMIT;
         }
         else
         {
            openSession(sessionId);
         }
         return ret;
      }
   }
   else
   {
      _DTraceDebug("can not get id");
      std::string sessionIsLogging;
      if( isThereSessionLogged(sessionIsLogging) )
      {
         _DTraceDebug("The session %s is logged, please wait", sessionIsLogging.c_str());
         ret = WSN_LOGIN_LIMIT;
      }
      else
      {
         openSession(sessionId);
      }
      return ret;
   }

   if( ipdContent.find("/LOG_IN?") != std::string::npos )
   {
      _DTraceDebug("LOG_IN");
      ret = reqLogin(ipdContent, sessionId);
   }
   else if( ipdContent.find("/LOG_IN_FAILED?") != std::string::npos )
   {
      _DTraceDebug("LOG_IN_FAILED");
      ret = WSN_LOGIN;
   }
   else if( ipdContent.find("/WIFI_LIST?") != std::string::npos && isVerified )
   {
      _DTraceDebug("WIFI_LIST");
      ret = WSN_WIFI_LIST;
   }
   else if( ipdContent.find("/CONNECT_ACCESS_POINT?") != std::string::npos && isVerified )
   {
      _DTraceDebug("CONNECT_ACCESS_POINT");
      ret = reqConnectAp(ipdContent);
   }
   else if( ipdContent.find("/RATE_TABLE?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("RATE_TABLE");
      m_tmpIndex = 1;
      ret = WSN_RATE_TABLE;
   }
   else if( ipdContent.find("/VIEWPAGE_RATE_TABLE?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("VIEWPAGE_RATE_TABLE");
      ret = reqViewRateTable(ipdContent);
   }
   else if( ipdContent.find("/SETUP_SERVER?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("SETUP_SERVER");
      ret = WSN_SETUP_SRV;
   }
   else if( ipdContent.find("/UPDATE_SERVER_SETTING?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("UPDATE_SERVER_SETTING");
      ret = reqUpdateSrv(ipdContent);
   }
   else if( ipdContent.find("/SETUP_SOFTAP?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("SETUP_SOFTAP");
      ret = WSN_SETUP_SAP;
   }
   else if( ipdContent.find("/UPDATE_SOFTAP_SETTING?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("UPDATE_SOFTAP_SETTING");
      ret = reqUpdateSAP(ipdContent);
   }
   else if( ipdContent.find("/SETUP_PRINTER?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("SETUP_PRINTER");
      ret = WSN_SETUP_PRT;
   }
   else if( ipdContent.find("/UPDATE_PRINTER_SETTING?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("UPDATE_SOFTAP_SETTING");
      ret = reqUpdatePrinter(ipdContent);
   }
   else if( ipdContent.find("/SCALE_REGISTER?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("SCALE_REGISTER");
      ret = WSN_SCALE_REG;
   }
   else if( ipdContent.find("/UPDATE_SCALE_REGISTER?") != std::string::npos && isVerified  )
   {
      _DTraceDebug("UPDATE_SCALE_REGISTER");
      ret = reqUpdateScaleRegister(ipdContent);
   }
   else if( ipdContent.find("/LOG_OUT?") != std::string::npos )
   {
      _DTraceDebug("LOG_OUT");
      ret = reqLogout(sessionId);
   }
   else if( ipdContent.find("/SAVE_SETTINGS?") != std::string::npos )
   {
      _DTraceDebug("SAVE_SETTINGS");
      ret = reqSaveSettings(sessionId);
   }
   else if( ipdContent.find("/TEST_DEVICE?") != std::string::npos )
   {
      _DTraceDebug("TEST_DEVICE");
      ret = WSN_TEST_DEVICES;
   }
   else if( ipdContent.find("/TEST_PRINTER?") != std::string::npos )
   {
      _DTraceDebug("TEST_PRINTER");
      ret = reqTestPrinter(sessionId);
   }
   else if( ipdContent.find("/TEST_DATA_SERVER_CONN?") != std::string::npos )
   {
      _DTraceDebug("TEST_DATA_SERVER_CONN");
      ret = reqTestServerConn(sessionId);
   }
   else if( ipdContent.find("/TEST_CONF_SERVER_CONN?") != std::string::npos )
   {
      _DTraceDebug("TEST_CONF_SERVER_CONN");
      ret = reqTestServerConn(sessionId);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqLogin(const std::string& reqInfo, const std::string& sessionId)
{
   WebsrvNavigator ret = WSN_LOGIN;

   std::string username;
   std::string password;

   if( getIpdParam(reqInfo, "username", username) && getIpdParam(reqInfo, "password", password) )
   {
      UserInfo userInfo;
      _U_.getUserConfigAP(userInfo);
      _DTraceDebug("Verify username and password...");
      if( (userInfo.username.compare(username) == 0) && (userInfo.password.compare(password) == 0) )
      {
         if( setLoggedSession(sessionId) )
         {
            ret = WSN_SETUP_SAP;
         }
      }
      else
      {
         ret = WSN_LOGIN_FAILED;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqLogout(const std::string& sessionId)
{
   closeSessionAll();
   return WSN_LOG_OUT;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqConnectAp(const std::string& reqInfo)
{
   WebsrvNavigator ret = WSN_WIFI_LIST;
   AccessPointInfo apSelected;
   bool ssidFound = false;

   if( getIpdParam(reqInfo, "macaddr", apSelected.mac) &&
       getIpdParam(reqInfo, "password", apSelected.passwd) )

   {
      std::list<AccessPointInfo> apList;
      ConnDeviceWifi::getInstance().getListAP(apList);
      if( !apList.empty() )
      {
         for( std::list<AccessPointInfo>::iterator it = apList.begin(); it != apList.end(); ++it )
         {
            if( apSelected.mac.compare((*it).mac) == 0 )
            {
               apSelected.ssid.assign((*it).ssid);
               _DTraceDebug("Found SSID: %s", apSelected.ssid.c_str());
               ssidFound = true;
               break;
            }
         }
      }

      if( ssidFound )
      {
         if( ConnDeviceWifi::getInstance().connectAP(apSelected.ssid, apSelected.passwd, apSelected.mac) )
         {
            _U_.setSelectedAccessPoint(apSelected);
            ret = WSN_WIFI_JAP_OK;
         }
         else
         {
            ret = WSN_WIFI_JAP_NOK;
         }
      }
      else
      {
         ret = WSN_WIFI_JAP_NOK;
      }

      Task_Delay(ST_GetClocksPerSecond()/2);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqUpdateSrv(const std::string& reqInfo)
{
   std::string newValue;

   if( getIpdParam(reqInfo, "submitsrv", newValue) )
   {
      if( newValue.find("on") != std::string::npos )
      {
         _U_.setSubmitData(SETTING_ON);
      }
   }
   else
   {
      _U_.setSubmitData(SETTING_OFF);
   }

   if( getIpdParam(reqInfo, "submitemail", newValue) )
   {
      if( newValue.find("on") != std::string::npos )
      {
         _U_.setSubmitEmail(SETTING_ON);
      }
   }
   else
   {
      _U_.setSubmitEmail(SETTING_OFF);
   }

   if( getIpdParam(reqInfo, "submitsms", newValue) )
   {
      if( newValue.find("on") != std::string::npos )
      {
         _U_.setSubmitSms(SETTING_ON);
      }
   }
   else
   {
      _U_.setSubmitSms(SETTING_OFF);
   }


   if( getIpdParam(reqInfo, "urlsubmitdata", newValue) )
   {
      UrlInfo serverUrl;
      serverUrl.url = newValue;
      serverUrl.port = "80";
      if( getIpdParam(reqInfo, "submitdataport", newValue) )
      {
         int iPort = GStringUtils::str2int(newValue);
         if( iPort != 0 && iPort != 80 )
         {
            serverUrl.port = newValue;
         }
      }

      serverUrl.ssl = SETTING_OFF;
      if( getIpdParam(reqInfo, "submitdatassl", newValue) )
      {
         if( newValue.find("on") != std::string::npos )
         {
            serverUrl.ssl = SETTING_ON;
         }
      }

      _U_.setSubmitDataUrl(serverUrl);
   }

   if( getIpdParam(reqInfo, "urlconfigsrv", newValue) )
   {
      UrlInfo serverUrl;
      serverUrl.url = newValue;
      serverUrl.port = "80";
      if( getIpdParam(reqInfo, "configsrvport", newValue) )
      {
         int iPort = GStringUtils::str2int(newValue);
         if( iPort != 0 && iPort != 80 )
         {
            serverUrl.port = newValue;
         }
      }

      serverUrl.ssl = SETTING_OFF;
      if( getIpdParam(reqInfo, "configsrvssl", newValue) )
      {
         if( newValue.find("on") != std::string::npos )
         {
            serverUrl.ssl = SETTING_ON;
         }
      }

      _U_.setConfigurationUrl(serverUrl);
   }

   SenderMailServerCfg mailSrvCfg;
   MailAddressInfo mailReceiver;

   if( getIpdParam(reqInfo, "mailserver", mailSrvCfg.outGoingServer) &&
       getIpdParam(reqInfo, "mailport", mailSrvCfg.outGoingPort) &&
       getIpdParam(reqInfo, "sendername", mailSrvCfg.senderName) &&
       getIpdParam(reqInfo, "username", mailSrvCfg.username) &&
       getIpdParam(reqInfo, "password", mailSrvCfg.password) )
   {
      _U_.setEmailSender(mailSrvCfg);
   }

   if( getIpdParam(reqInfo, "receivername", mailReceiver.accName) &&
       getIpdParam(reqInfo, "receivermail", mailReceiver.address) )
   {
      _U_.setEmailReceiver(mailReceiver);
   }

   if( getIpdParam(reqInfo, "subscribernum", newValue) )
   {
      _U_.setSmsNumReceiver(newValue);
   }

   return WSN_SETUP_SRV;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqUpdateSAP(const std::string& reqInfo)
{
   std::string sPasswordsap;
   std::string sSsidsap;
   UserInfo userInf;

   if( getIpdParam(reqInfo, "usernameadmin", userInf.username) &&
       getIpdParam(reqInfo, "passwordadmin", userInf.password) &&
       getIpdParam(reqInfo, "ssidsap", sSsidsap) &&
       getIpdParam(reqInfo, "passwordsap", sPasswordsap) )
   {
      if( !userInf.username.empty() && !userInf.username.empty() )
      {

         _U_.setUserConfigAP(userInf);
      }

      if( !sSsidsap.empty() && !sPasswordsap.empty() )
      {
         SoftAPInfo softApInfo;
         ConnDeviceWifi::getInstance().getSoftApCfg(softApInfo);

         if( sSsidsap.find("1078SW") == std::string::npos )
         {
            sSsidsap.insert(0, "1078SW");
         }

         if( (softApInfo.ssid.compare(sSsidsap) != 0) ||
             (sPasswordsap.length() > 7 && softApInfo.password.compare(sPasswordsap) != 0) )
         {
            softApInfo.ssid = sSsidsap;
            softApInfo.password = sPasswordsap;
            ConnDeviceWifi::getInstance().setTcpServer(SETTING_OFF);
            ConnDeviceWifi::getInstance().setSoftApCfg(softApInfo);
            ConnDeviceWifi::getInstance().setWifiDhcp(DWDHCP_SOFTAP_STATION);
            ConnDeviceWifi::getInstance().setTcpServer(SETTING_ON);
            closeSessionAll();
            //no connect anymore
            return WSN_NONE;
         }
      }
   }

   return WSN_SETUP_SAP;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqUpdatePrinter(const std::string& reqInfo)
{
   std::string newValue;

   if( getIpdParam(reqInfo, "printreceipt", newValue) )
   {
      if( newValue.find("on") != std::string::npos )
      {
         _U_.setPrintReceipt(SETTING_ON);
      }
   }
   else
   {
      _U_.setPrintReceipt(SETTING_OFF);
   }

   if( getIpdParam(reqInfo, "printlabel", newValue) )
   {
      if( newValue.find("on") != std::string::npos )
      {
         _U_.setPrintLabel(SETTING_ON);
      }
   }
   else
   {
      _U_.setPrintLabel(SETTING_OFF);
   }

   return WSN_SETUP_PRT;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqUpdateScaleRegister(const std::string& reqInfo)
{
   WebsrvNavigator ret = WSN_SCALE_REG_NOK;
   std::string deviceId;
   std::string userId;
   std::string regContent;

   if( getIpdParam(reqInfo, "deviceid", deviceId) &&
       getIpdParam(reqInfo, "userid", userId) &&
       getIpdParam(reqInfo, "regcontent", regContent) )
   {
      ConnHttpStatusCode regResult = ConnDeviceWifi::getInstance().reqRegister(deviceId, userId, regContent);
      if( regResult == HTTP_200 )
      {
         ret = WSN_SCALE_REG_OK;
      }
      else if( regResult == HTTP_404 )
      {
         ret = WSN_SCALE_REG_NOK_USER;
      }
      else if( regResult == HTTP_409 )
      {
         ret = WSN_SCALE_REG_NOK_EXIST;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqViewRateTable(const std::string& reqInfo)
{
   std::string pageId;

   if( getIpdParam(reqInfo, "viewpagenumber", pageId) )
   {
      m_tmpIndex = GStringUtils::str2int(pageId);
   }

   return WSN_RATE_TABLE_EX;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqTestPrinter(const std::string& sessionId)
{
   bool isVerified;

   if( isExistentSession(sessionId, isVerified) )
   {
      if( isVerified )
      {
         PrinterManager::getInstance().printTest();
      }
   }

   return WSN_TEST_DEVICES;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqTestServerConn(const std::string& sessionId)
{
   WebsrvNavigator ret = WSN_TEST_DEVICES_CONN_NOK;
   time_t timeStamp = SYSTEM_TIME_BASE_INIT;

   bool isVerified;

   if( isExistentSession(sessionId, isVerified) )
   {
      if( isVerified )
      {
         ConnDeviceWifi::getInstance().reqDateTime(timeStamp);
      }
   }

   if( timeStamp != SYSTEM_TIME_BASE_INIT )
   {
      if( _S_.getBaseTime() == SYSTEM_TIME_BASE_INIT )
      {
         _S_.setBaseTime(timeStamp);
      }

      ret = WSN_TEST_DEVICES_CONN_OK;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
WebsrvNavigator Webserver::reqSaveSettings(const std::string& sessionId)
{
   WebsrvNavigator ret = WSN_SAVE_SETTINGS_NOK;
   bool isVerified;

   if( isExistentSession(sessionId, isVerified) )
   {
      if( isVerified )
      {
         if( _Sa_.serialize(SERIALIZABLE_PARTITION_1) )
         {
            ret = WSN_SAVE_SETTINGS_OK;
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
