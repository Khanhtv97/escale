//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GUserSettings.cpp                                                          //
//////////////////////////////////////////////////////////////////////////////////////

#include "GUserSettings.hpp"
#include "GROMManager.hpp"
#include "GStringUtils.hpp"
#include "JsonParser.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace DataController {

#define TAG_SUBMIT_SMS    "SubmitSMS"
#define TAG_SUBSCR_SMS    "SubscriberSMS"
#define TAG_SUBMIT_EMAIL  "SubmitEmail"
#define TAG_SUBMIT_DATA   "SubmitData"
#define TAG_PRINT_RECEIPT "PrintReceipt"
#define TAG_PRINT_LABEL   "PrintLabel"
#define TAG_LAST_RATE_ID  "LastRateId"

#define TAG_MAILSERVER_URL  "MailServerUrl"
#define TAG_MAILSERVER_PORT "MailServerPort"
#define TAG_MAILSERVER_USER "MailServerUser"
#define TAG_MAILSERVER_PASS "MailServerPass"
#define TAG_MAILSERVER_NAME "MailServerName"

#define TAG_SUBSCRIBER_MAIL "SubscriberMail"
#define TAG_SUBSCRIBER_NAME "SubscriberName"

#define TAG_SERVERDATA_ULR  "ServerDataUrl"
#define TAG_SERVERDATA_PORT "ServerDataPort"
#define TAG_SERVERDATA_SSL  "ServerDataSsl"
#define TAG_SERVERDATA_API  "ServerDataApi"

#define TAG_SERVERCFG_ULR   "ServerCfgUrl"
#define TAG_SERVERCFG_PORT  "ServerCfgPort"
#define TAG_SERVERCFG_SSL   "ServerCfgSsl"

#define TAG_SYS_ACC_USER    "Username"
#define TAG_SYS_ACC_PASS    "Password"

#define TAG_CONN_WIFI_MAC   "WifiMac"
#define TAG_CONN_WIFI_SSID  "WifiSsid"
#define TAG_CONN_WIFI_PASS  "WifiPass"

//////////////////////////////////////////////////////////////////////////////////////
GUserSettings& GUserSettings::getInstance()
{
   static GUserSettings instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
GUserSettings::GUserSettings()
{
   initUserSettingsDefault();
}

//////////////////////////////////////////////////////////////////////////////////////
GUserSettings::~GUserSettings()
{
   for( std::list<GUserSettings_Logo_t>::iterator it = m_logoList.begin();
         it != m_logoList.end(); ++it )
   {
      if( it->gamObj != NULL && !it->isDefault )
      {
         SYS_Memory_Deallocate(SYS_MEMORY_CACHED, (U32)it->gamObj->Bitmap->Data1_Cp);
         delete[] (U8*)it->gamObj->Bitmap->Data1_NCp;
         it->gamObj->Bitmap->Data1_NCp = NULL;
         delete it->gamObj->Bitmap;
         it->gamObj->Bitmap = NULL;
         delete it->gamObj;
         it->gamObj = NULL;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::initUserSetting()
{
   return;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::initUserSettingsDefault()
{
   _DTraceError("GUserSettings::initUserSettingsDefault \n");

   m_submitSms = SETTING_OFF;
   m_submitEmail = SETTING_OFF;
   m_submitData = SETTING_ON;
   m_printReceipt = SETTING_OFF;
   m_printLabel = SETTING_ON;
   m_lastActiveRateTableId = 0;

   m_subscriberSms = "+84";

   m_senderMailServer.outGoingServer = "smtp.mail.yahoo.com";
   m_senderMailServer.outGoingPort = "465";
   m_senderMailServer.username = "gbsescale@yahoo.com";
   m_senderMailServer.password = "gbs0246813579";
   m_senderMailServer.senderName = "GBS EScale";

   m_subscriberEmail.accName = "Customers"; //Customers
   m_subscriberEmail.address = "customer@yahoo.com"; //customer@yahoo.com

   m_submitDataUrl.url = "192.168.0.137";
   m_submitDataUrl.port = "8000";
   m_submitDataUrl.api = "";
   m_submitDataUrl.ssl = 0;
   m_configurationUrl.url = "192.168.0.137";
   m_configurationUrl.port = "8000";
   m_configurationUrl.ssl = 0;

   m_userConfigAP.username = "admin";
   m_userConfigAP.password = "admin";

   m_selectedAcessPoint.mac = "c4:6e:1f:a5:f6:dc";
   m_selectedAcessPoint.ssid = "GBS504";
   m_selectedAcessPoint.passwd = "0912151399";
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::serializeData(std::string& outData)
{
   Json::Value nodeRoot(Json::objectValue);
   Json::FastWriter jsWriter;

   nodeRoot[TAG_SUBMIT_SMS] = m_submitSms;
   nodeRoot[TAG_SUBSCR_SMS] = m_subscriberSms;
   nodeRoot[TAG_SUBMIT_EMAIL] = m_submitEmail;
   nodeRoot[TAG_SUBMIT_DATA] = m_submitData;
   nodeRoot[TAG_PRINT_RECEIPT] = m_printReceipt;
   nodeRoot[TAG_PRINT_LABEL] = m_printLabel;
   nodeRoot[TAG_LAST_RATE_ID] = m_lastActiveRateTableId;

   nodeRoot[TAG_MAILSERVER_URL] = m_senderMailServer.outGoingServer;
   nodeRoot[TAG_MAILSERVER_PORT] = m_senderMailServer.outGoingPort;
   nodeRoot[TAG_MAILSERVER_USER] = m_senderMailServer.username;
   nodeRoot[TAG_MAILSERVER_PASS] = m_senderMailServer.password;
   nodeRoot[TAG_MAILSERVER_NAME] = m_senderMailServer.senderName;

   nodeRoot[TAG_SUBSCRIBER_MAIL] = m_subscriberEmail.accName;
   nodeRoot[TAG_SUBSCRIBER_NAME] = m_subscriberEmail.address;

   nodeRoot[TAG_SERVERDATA_ULR] = m_submitDataUrl.url;
   nodeRoot[TAG_SERVERDATA_PORT] = m_submitDataUrl.port;
   nodeRoot[TAG_SERVERDATA_SSL] = m_submitDataUrl.ssl;
   nodeRoot[TAG_SERVERDATA_API] = m_submitDataUrl.api;

   nodeRoot[TAG_SERVERCFG_ULR] = m_configurationUrl.url;
   nodeRoot[TAG_SERVERCFG_PORT] = m_configurationUrl.port;
   nodeRoot[TAG_SERVERCFG_SSL] = m_configurationUrl.ssl;

   nodeRoot[TAG_SYS_ACC_USER] = m_userConfigAP.username;
   nodeRoot[TAG_SYS_ACC_PASS] = m_userConfigAP.password;

   nodeRoot[TAG_CONN_WIFI_MAC] = m_selectedAcessPoint.mac;
   nodeRoot[TAG_CONN_WIFI_SSID] = m_selectedAcessPoint.ssid;
   nodeRoot[TAG_CONN_WIFI_PASS] = m_selectedAcessPoint.passwd;

   outData.assign(jsWriter.write(nodeRoot));
}

//////////////////////////////////////////////////////////////////////////////////////
bool GUserSettings::unserializeData(const U8* data)
{
   bool canContinue = false;

   if( data == NULL )
   {
      return canContinue;
   }

   JsonParser jsParser;
   Json::Value rootNode;
   Json::Features jsFeatures;
   Json::Reader jsReader(jsFeatures);

   if( jsReader.parse((char*)data, rootNode) )
   {
      canContinue = (bool)(rootNode.type() != Json::nullValue);
   }

   if( !canContinue )
   {
      return canContinue;
   }

   m_submitSms = jsParser.getIntValueNode(rootNode, TAG_SUBMIT_SMS);
   m_subscriberSms = jsParser.getStrValueNode(rootNode, TAG_SUBSCR_SMS);

   m_submitEmail = jsParser.getIntValueNode(rootNode, TAG_SUBMIT_EMAIL);
   m_submitData = jsParser.getIntValueNode(rootNode, TAG_SUBMIT_DATA);
   m_printReceipt = jsParser.getIntValueNode(rootNode, TAG_PRINT_RECEIPT);
   m_printLabel = jsParser.getIntValueNode(rootNode, TAG_PRINT_LABEL);
   m_lastActiveRateTableId = jsParser.getUintValueNode(rootNode, TAG_LAST_RATE_ID);

   m_senderMailServer.outGoingServer = jsParser.getStrValueNode(rootNode, TAG_MAILSERVER_URL);
   m_senderMailServer.outGoingPort = jsParser.getStrValueNode(rootNode, TAG_MAILSERVER_PORT);
   m_senderMailServer.username = jsParser.getStrValueNode(rootNode, TAG_MAILSERVER_USER);
   m_senderMailServer.password = jsParser.getStrValueNode(rootNode, TAG_MAILSERVER_PASS);
   m_senderMailServer.senderName = jsParser.getStrValueNode(rootNode, TAG_MAILSERVER_NAME);

   m_subscriberEmail.accName = jsParser.getStrValueNode(rootNode, TAG_SUBSCRIBER_MAIL);
   m_subscriberEmail.address = jsParser.getStrValueNode(rootNode, TAG_SUBSCRIBER_NAME);

   m_submitDataUrl.url = jsParser.getStrValueNode(rootNode, TAG_SERVERDATA_ULR);
   m_submitDataUrl.port = jsParser.getStrValueNode(rootNode, TAG_SERVERDATA_PORT);
   m_submitDataUrl.api = jsParser.getStrValueNode(rootNode, TAG_SERVERDATA_API);
   m_submitDataUrl.ssl = jsParser.getIntValueNode(rootNode, TAG_SERVERDATA_SSL);

   m_configurationUrl.url = jsParser.getStrValueNode(rootNode, TAG_SERVERCFG_ULR);
   m_configurationUrl.port = jsParser.getStrValueNode(rootNode, TAG_SERVERCFG_PORT);
   m_configurationUrl.ssl = jsParser.getIntValueNode(rootNode, TAG_SERVERCFG_SSL);

   m_userConfigAP.username = jsParser.getStrValueNode(rootNode, TAG_SYS_ACC_USER);
   m_userConfigAP.password = jsParser.getStrValueNode(rootNode, TAG_SYS_ACC_PASS);

   m_selectedAcessPoint.mac = jsParser.getStrValueNode(rootNode, TAG_CONN_WIFI_MAC);
   m_selectedAcessPoint.ssid = jsParser.getStrValueNode(rootNode, TAG_CONN_WIFI_SSID);
   m_selectedAcessPoint.passwd = jsParser.getStrValueNode(rootNode, TAG_CONN_WIFI_PASS);

   return canContinue;
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GUserSettings::getSubmitSms()
{
   return m_submitSms;
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GUserSettings::getSubmitEmail()
{
   return m_submitEmail;
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GUserSettings::getSubmitData()
{
   return m_submitData;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setSubmitSms(U8 OnOff)
{
   m_submitSms = OnOff;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setSubmitEmail(U8 OnOff)
{
   m_submitEmail = OnOff;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setSubmitData(U8 OnOff)
{
   m_submitData = OnOff;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setPrintReceipt(U8 OnOff)
{
   m_printReceipt = OnOff;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setPrintLabel(U8 OnOff)
{
   m_printLabel = OnOff;
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GUserSettings::getPrintReceipt()
{
   return m_printReceipt;
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GUserSettings::getPrintLabel()
{
   return m_printLabel;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setEmailSender(const SenderMailServerCfg& mailServerCfg)
{
   m_senderMailServer.outGoingServer = mailServerCfg.outGoingServer;
   m_senderMailServer.outGoingPort = mailServerCfg.outGoingPort;
   m_senderMailServer.senderName = mailServerCfg.senderName;
   m_senderMailServer.username = mailServerCfg.username;
   m_senderMailServer.password = mailServerCfg.password;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::getEmailSender(SenderMailServerCfg& mailServerCfg)
{
   mailServerCfg.outGoingServer = m_senderMailServer.outGoingServer;
   mailServerCfg.outGoingPort = m_senderMailServer.outGoingPort;
   mailServerCfg.senderName = m_senderMailServer.senderName;
   mailServerCfg.username = m_senderMailServer.username;
   mailServerCfg.password = m_senderMailServer.password;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setEmailReceiver(const MailAddressInfo& subscriberEmail)
{
   m_subscriberEmail.accName = subscriberEmail.accName;
   m_subscriberEmail.address = subscriberEmail.address;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::getEmailReceiver(MailAddressInfo& subscriberEmail)
{
   subscriberEmail.accName = m_subscriberEmail.accName;
   subscriberEmail.address = m_subscriberEmail.address;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setSmsNumReceiver(const std::string subscriberSms)
{
   m_subscriberSms = subscriberSms;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::getSmsNumReceiver(std::string& subscriberSms)
{
   subscriberSms = m_subscriberSms;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setSubmitDataUrl(const UrlInfo urlInf)
{
   m_submitDataUrl.url.assign(urlInf.url);
   m_submitDataUrl.port.assign(urlInf.port);
   m_submitDataUrl.api.assign(urlInf.api);
   m_submitDataUrl.ssl = urlInf.ssl;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::getSubmitDataUrl(UrlInfo& urlInf)
{
   urlInf.url.assign(m_submitDataUrl.url);
   urlInf.port.assign(m_submitDataUrl.port);
   urlInf.api.assign(m_submitDataUrl.api);
   urlInf.ssl = m_submitDataUrl.ssl;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setConfigurationUrl(const UrlInfo urlInf)
{
   m_configurationUrl.url.assign(urlInf.url);
   m_configurationUrl.port.assign(urlInf.port);
   m_configurationUrl.ssl = urlInf.ssl;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::getConfigurationUrl(UrlInfo& urlInf)
{
   urlInf.url.assign(m_configurationUrl.url);
   urlInf.port.assign(m_configurationUrl.port);
   urlInf.ssl = m_configurationUrl.ssl;
}

//////////////////////////////////////////////////////////////////////////////////////
GAMLOAD_Object_t* GUserSettings::getLogo(FS_Logo_t type)
{
   for( std::list<GUserSettings_Logo_t>::iterator it = m_logoList.begin();
         it != m_logoList.end(); ++it )
   {
      if( it->type == type )
      {
         return it->gamObj;
      }
   }

   return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::loadLogosFromRom()
{
   GUserSettings_Logo_t splash(GLOGO_SPLASH);
   GUserSettings_Logo_t info(GLOGO_INFO);
   GUserSettings_Logo_t menu(GLOGO_MENU);

   m_logoList.push_back(splash);
   m_logoList.push_back(info);
   m_logoList.push_back(menu);

   for( std::list<GUserSettings_Logo_t>::iterator it = m_logoList.begin();
         it != m_logoList.end(); ++it )
   {
      GAMLOAD_Object_t* pGamObj = NULL;
      GFileSystem::getInstance().getLogo(it->type, pGamObj);

      if( pGamObj != NULL )
      {
         it->gamObj = pGamObj;
         it->isDefault = false;
      }
      else
      {
         setLogoToDefault(*it);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setLogoToDefault(GUserSettings_Logo_t& logo)
{
   switch( logo.type )
   {
   case GLOGO_SPLASH:
      //logo.gamObj = &StartupScreen_gam;
      logo.isDefault = true;
      break;

   case GLOGO_INFO:
   case GLOGO_MENU:
      //logo.gamObj = &FormChannelInfo_LogoGBS_gam;
      logo.isDefault = true;
      break;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setSelectedAccessPoint(const AccessPointInfo& selectedAP)
{
   m_selectedAcessPoint = selectedAP;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::getSelectedAccessPoint(AccessPointInfo& selectedAP)
{
   selectedAP = m_selectedAcessPoint;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setUserConfigAP(const UserInfo& userInf)
{
   m_userConfigAP.username = userInf.username;
   m_userConfigAP.password = userInf.password;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::getUserConfigAP(UserInfo& userInf)
{
   _DTraceDebug("getUserConfigAP");
   userInf.username = m_userConfigAP.username;
   userInf.password = m_userConfigAP.password;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setDeviceNetworkConfiguration(const DeviceWifiNetworkCfg& deviceNwCfg)
{
   m_deviceNetworkConfiguration.macaddr = deviceNwCfg.macaddr;
   m_deviceNetworkConfiguration.ipaddr = deviceNwCfg.ipaddr;
   m_deviceNetworkConfiguration.netmask = deviceNwCfg.netmask;
   m_deviceNetworkConfiguration.broadcast = deviceNwCfg.broadcast;
   m_deviceNetworkConfiguration.gateway = deviceNwCfg.gateway;
   m_deviceNetworkConfiguration.dns1 = deviceNwCfg.dns1;
   m_deviceNetworkConfiguration.dns2 = deviceNwCfg.dns2;
   m_deviceNetworkConfiguration.dhcp = deviceNwCfg.dhcp;
}

//////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::getDeviceNetworkConfiguration(DeviceWifiNetworkCfg& deviceNwCfg)
{
   deviceNwCfg.macaddr = m_deviceNetworkConfiguration.macaddr;
   deviceNwCfg.ipaddr = m_deviceNetworkConfiguration.ipaddr;
   deviceNwCfg.netmask = m_deviceNetworkConfiguration.netmask;
   deviceNwCfg.broadcast = m_deviceNetworkConfiguration.broadcast;
   deviceNwCfg.gateway = m_deviceNetworkConfiguration.gateway;
   deviceNwCfg.dns1 = m_deviceNetworkConfiguration.dns1;
   deviceNwCfg.dns2 = m_deviceNetworkConfiguration.dns2;
   deviceNwCfg.dhcp = m_deviceNetworkConfiguration.dhcp;
}

/////////////////////////////////////////////////////////////////////////////////////
void GUserSettings::setActiveRateTableId(U32 rateTableId)
{
   m_lastActiveRateTableId = rateTableId;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GUserSettings::getActiveRateTableId()
{
   return m_lastActiveRateTableId;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //DataController
//} //Business
//} //Application
//} //STM
//} //GBS
