//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GUserSettings.hpp                                                          //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GUSERSETTINGS_HPP_
#define GUSERSETTINGS_HPP_

#include "APISetting.hpp"
#include "GObject.hpp"
#include "GLanguage.hpp"
#include "GFont.hpp"
#include "GList.hpp"
#include "GFileSystem.hpp"

enum
{
   SETTING_OFF = 0,
   SETTING_ON
};

typedef struct UserInfo_s
{
   std::string username;
   std::string password;

   UserInfo_s()
   {
      username = "";
      password = "";
   }
}
UserInfo;

typedef struct UrlInfo_s
{
   std::string url;
   std::string port;
   std::string api;
   U8 ssl;

   UrlInfo_s()
   {
      url = "";
      port = "";
      api = "";
      ssl = 0;
   }
}
UrlInfo;

typedef struct SenderMailServerCfg_s
{
   std::string outGoingServer;
   std::string outGoingPort;
   std::string username;
   std::string password;
   std::string senderName;

   SenderMailServerCfg_s()
   {
      outGoingServer = "";
      outGoingPort = "";
      username = "";
      password = "";
      senderName = "";
   }
}
SenderMailServerCfg;

typedef struct MailAddressInfo_s
{
   std::string accName;
   std::string address;
   void reset()
   {
      accName = "";
      address = "";
   }

   void MailAddressInfo()
   {
      reset();
   }
}
MailAddressInfo;

typedef struct MailToList_s
{
   std::list<MailAddressInfo> toList;
   std::list<MailAddressInfo> ccList;
   std::list<MailAddressInfo> bccList;

   void reset()
   {
      toList.clear();
      ccList.clear();
      bccList.clear();
   }

   MailToList_s()
   {
      reset();
   }
}
MailToList;

typedef struct DeviceWifiNetworkCfg_s
{
   std::string macaddr;
   std::string ipaddr;
   std::string netmask;
   std::string broadcast;
   std::string gateway;
   std::string dns1;
   std::string dns2;
   U8 dhcp;

   DeviceWifiNetworkCfg_s()
   {
      reset();
   }

   void reset()
   {
      macaddr = "";
      ipaddr = "";
      netmask = "";
      broadcast = "";
      gateway = "";
      dns1 = "";
      dns2 = "";
      dhcp = SETTING_OFF;
   }
}
DeviceWifiNetworkCfg;

typedef enum EncodeNetwork_e
{
   ECN_OPEN = 0,
   ECN_WEP,
   ECN_WPA_PSK,
   ECN_WPA2_PSK,
   ECN_WPA_WPA2_PSK
}
EncodeNetwork;

typedef struct AccessPointInfo_s
{
   std::string mac;
   std::string ssid;
   std::string passwd;
   EncodeNetwork ecn;
   int signalStrength;
   int channel;
   int freqOffset;
   int freqCalibration;

   AccessPointInfo_s()
   {
      mac = "";
      ssid = "";
      passwd = "";
      ecn = ECN_OPEN;
      signalStrength = 0;
      channel = 0;
      freqOffset = 0;
      freqCalibration = 0;
   }
}
AccessPointInfo;

typedef struct GUserSettings_Logo_s
{
   GAMLOAD_Object_t* gamObj;
   bool isDefault;
   FS_Logo_t type;

   GUserSettings_Logo_s(FS_Logo_t inType) : gamObj(NULL), isDefault(true), type(inType) {}
}
GUserSettings_Logo_t;

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace DataController {

#define _U_ GUserSettings::getInstance()

//////////////////////////////////////////////////////////////////////////////////////
class GUserSettings : public virtual GObject
{
   friend class SaveDataToFlash;
private:
   GUserSettings();
   ~GUserSettings();

   std::list<GUserSettings_Logo_t> m_logoList;

   UserInfo m_userConfigAP;

   U8 m_printReceipt;
   U8 m_printLabel;

   U8 m_submitSms;
   U8 m_submitEmail;
   U8 m_submitData;

   U32 m_lastActiveRateTableId;

   DeviceWifiNetworkCfg m_deviceNetworkConfiguration;

   SenderMailServerCfg m_senderMailServer;
   MailAddressInfo m_subscriberEmail;
   std::string m_subscriberSms;

   AccessPointInfo m_selectedAcessPoint;
   UrlInfo m_submitDataUrl;
   UrlInfo m_configurationUrl;

   void initUserSettingsDefault();
   void loadLogosFromRom();
   void setLogoToDefault(GUserSettings_Logo_t& logo);

protected:
   void serializeData(std::string& outData);
   bool unserializeData(const U8* data);

public:
   static GUserSettings& getInstance();

   void initUserSetting();
   void resetAllToFactory(bool isGotoProductionTest = false);

   void setEmailSender(const SenderMailServerCfg& mailServerCfg);
   void getEmailSender(SenderMailServerCfg& mailServerCfg);
   void setEmailReceiver(const MailAddressInfo& subscriberEmail);
   void getEmailReceiver(MailAddressInfo& subscriberEmail);
   void setSmsNumReceiver(const std::string subscriberSms);
   void getSmsNumReceiver(std::string& subscriberSms);

   void setSubmitDataUrl(const UrlInfo url);
   void getSubmitDataUrl(UrlInfo& url);
   void setConfigurationUrl(const UrlInfo url);
   void getConfigurationUrl(UrlInfo& url);

   void setSelectedAccessPoint(const AccessPointInfo& selectedAP);
   void getSelectedAccessPoint(AccessPointInfo& selectedAP);

   void setDeviceNetworkConfiguration(const DeviceWifiNetworkCfg& deviceNwCfg);
   void getDeviceNetworkConfiguration(DeviceWifiNetworkCfg& deviceNwCfg);

   void setUserConfigAP(const UserInfo& userInf);
   void getUserConfigAP(UserInfo& userInf);

   void setPrintReceipt(U8 OnOff);
   void setPrintLabel(U8 OnOff);
   U8 getPrintReceipt();
   U8 getPrintLabel();

   void setSubmitSms(U8 OnOff);
   void setSubmitEmail(U8 OnOff);
   void setSubmitData(U8 OnOff);
   U8 getSubmitSms();
   U8 getSubmitEmail();
   U8 getSubmitData();

   void setActiveRateTableId(U32 rateTableId);
   U32 getActiveRateTableId();

   GAMLOAD_Object_t* getLogo(FS_Logo_t type);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //DataController
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //GUSERSETTINGS_HPP_

