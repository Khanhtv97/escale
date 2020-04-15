//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: HtmlDocument.hpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef HTMLDOCUMENT_H_
#define HTMLDOCUMENT_H_

#include "stapp_main.h"
#include "GDebug.hpp"

#define HTML_MASTER_TITLE    "CAI DAT GBSW-1078"
#define HTML_TITLE_SETUPAP   " - ACCESS POINT"
#define HTML_TITLE_SETUPSRV  " - SERVER"
#define HTML_TITLE_SETUPPRT  " - PRINTER"
#define HTML_TITLE_SETUPWIFI " - WIFI"
#define HTML_TITLE_RATETABLE " - BANG GIA"
#define HTML_TITLE_TEST      " - TEST"
#define HTML_TITLE_SCALEREG  " - DANG KY CAN"
#define HTML_HEADER_OK "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: Closed\r\n"

typedef enum HttpErrorContent_e
{
   HTTP_ERR_NONE = 0,
   HTTP_ERR_LOGINFAILED,
   HTTP_ERR_LOGINLIMIT,
   HTTP_ERR_WIFI_JAP_OK,
   HTTP_ERR_WIFI_JAP_NOK,
   HTTP_ERR_REGISTER_OK,
   HTTP_ERR_REGISTER_NOK,
   HTTP_ERR_REGISTER_NOK_USER,
   HTTP_ERR_REGISTER_NOK_EXIST,
   HTTP_ERR_CONNECT_OK,
   HTTP_ERR_CONNECT_NOK,
   HTTP_ERR_SAVE_SETTINGS_OK,
   HTTP_ERR_SAVE_SETTINGS_NOK,
   HTTP_ERR_LOGGEDOUT
}
HttpErrorContent;

//////////////////////////////////////////////////////////////////////////////////////
class HtmlDocument
{
public:
   HtmlDocument();
   ~HtmlDocument();

   void buildLogin(const std::string& sessionId, std::string& oLoginPage);
   void buildErrorMessage(const std::string& sessionId, std::string& oPageContent, HttpErrorContent httpError);
   void buildListWifi(const std::string& sessionId, std::string& oPageContent);
   void buildRateTable(const std::string& sessionId, std::string& oPageContent, int page=1);
   void buildSetupSrv(const std::string& sessionId, std::string& oPageContent);
   void buildSetupSAP(const std::string& sessionId, std::string& oPageContent);
   void buildSetupPrinter(const std::string& sessionId, std::string& oPageContent);
   void buildScaleRegister(const std::string& sessionId, std::string& oPageContent);
   void buildTestDevice(const std::string& sessionId, std::string& oPageContent);

private:
   void buildMenuContent(const std::string& sessionId, std::string& oPageContent);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //HTMLDOCUMENT_H_
