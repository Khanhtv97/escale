//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: Webserver.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef WEBSERVERWIFI_H_
#define WEBSERVERWIFI_H_

#include "stapp_main.h"
#include "GDebug.hpp"
#include "GObject.hpp"
#include "GTimer.hpp"

typedef enum WebsrvNavigator_e
{
   WSN_NONE = 0,
   WSN_LOGIN,
   WSN_LOGIN_LIMIT,
   WSN_LOGIN_FAILED,
   WSN_WIFI_LIST,
   WSN_WIFI_JAP_OK,
   WSN_WIFI_JAP_NOK,
   WSN_RATE_TABLE,
   WSN_RATE_TABLE_EX,
   WSN_SETUP_SRV,
   WSN_SETUP_SAP,
   WSN_SETUP_PRT,
   WSN_SCALE_REG,
   WSN_SCALE_REG_OK,
   WSN_SCALE_REG_NOK,
   WSN_SCALE_REG_NOK_USER,
   WSN_SCALE_REG_NOK_EXIST,
   WSN_TEST_DEVICES,
   WSN_TEST_DEVICES_CONN_OK,
   WSN_TEST_DEVICES_CONN_NOK,
   WSN_SAVE_SETTINGS_OK,
   WSN_SAVE_SETTINGS_NOK,
   WSN_LOG_OUT,
}
WebsrvNavigator;

typedef enum HttpMethod_e
{
   HTTP_NONE = 0,
   HTTP_GET,
   HTTP_POST,
   HTTP_PUSH,
   HTTP_HEAD
}
HttpMethod;

typedef struct SessionInfo_s
{
   std::string sessionId;
   bool isVerified;
   U32 timeStamp;

   void SessionInfo()
   {
      sessionId = "";
      isVerified = false;
      timeStamp = 0;
   }
}
SessionInfo;

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////
class Webserver
{
public:
   static Webserver& getInstance();

   bool setRequest(const std::string& clientRequest);
   bool getResponse(std::string& wsResponse, int& connId);
   bool isClientConnecting();

private:
   Webserver();
   ~Webserver();

   WebsrvNavigator parseRequest(const std::string& clientRequest, std::string& sessionId, int& connId);
   WebsrvNavigator reqLogin(const std::string& reqInfo, const std::string& sessionId);
   WebsrvNavigator reqLogout(const std::string& sessionId);
   WebsrvNavigator reqConnectAp(const std::string& reqInfo);
   WebsrvNavigator reqUpdateSrv(const std::string& reqInfo);
   WebsrvNavigator reqUpdateSAP(const std::string& reqInfo);
   WebsrvNavigator reqUpdatePrinter(const std::string& reqInfo);
   WebsrvNavigator reqUpdateScaleRegister(const std::string& reqInfo);
   WebsrvNavigator reqViewRateTable(const std::string& reqInfo);
   WebsrvNavigator reqSaveSettings(const std::string& sessionId);
   WebsrvNavigator reqTestPrinter(const std::string& sessionId);
   WebsrvNavigator reqTestServerConn(const std::string& sessionId);

   int getIpdConnId(const std::string& ipdContent);
   HttpMethod getIpdMethod(const std::string& ipdContent);
   bool getIpdParam(const std::string& ipdContent, const std::string property, std::string& value);

   bool isExistentSession(std::string sessionId, bool& isVerified);
   bool isThereSessionLogged(std::string& sessionId);
   bool setLoggedSession(std::string sessionId);
   void openSession(std::string& sessionId);
   void closeSession(std::string sessionId);
   void closeSessionAll();
   void decodeUrl(std::string& ioUrl);

   void onSessionTimeout(EVENT_HANDLER_PARAM);
   GTimer* m_pSessionTimeout;
   IExecutable* m_pDelegateSessionTimeout;

   std::string m_lastRequest;
   std::list<SessionInfo> m_curSession;
   int m_tmpIndex;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //WEBSERVERWIFI_H_
