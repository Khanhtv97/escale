//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ConnDeviceWifi.hpp                                                         //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef CONNDEVICEWIFI_H_
#define CONNDEVICEWIFI_H_

#include "ConnDevice.hpp"

typedef enum DeviceWifiMode_e
{
   DWM_NONE = 0,
   DWM_STATION,
   DWM_SOFTAP,
   DWM_STATION_SOFTAP
}
DeviceWifiMode;

typedef enum DeviceWifiDhcp_e
{
   DWDHCP_NONE = 0,
   DWDHCP_SOFTAP,
   DWDHCP_STATION,
   DWDHCP_SOFTAP_STATION
}
DeviceWifiDhcp;

typedef struct DeviceWifiLocalIp_s
{
   std::string apMacAddr;
   std::string apIpAddr;
   std::string stMacAddr;
   std::string stIpAddr;

   void DeviceWifiLocalIp()
   {
      apMacAddr = "";
      apIpAddr = "";
      stMacAddr = "";
      stIpAddr = "";
   }
}
DeviceWifiLocalIp;

typedef enum NetworkConnectionStatus_e
{
   NCS_UNKNOWN = 1,
   NCS_GOTIP,
   NCS_CONNECTED,
   NCS_DISCONNECTED,
   NCS_CONNECTIONFAIL,
}
NetworkConnStatus;

typedef struct SoftAPInfo_s
{
   std::string ssid;
   std::string password;
   EncodeNetwork ecn;
   U8 channel;
   U8 maxConnection;
   U8 ssidHidden;

   void SoftAPInfo()
   {
      ssid = "";
      password = "";
      channel = 0;
      ecn = ECN_WPA2_PSK;
      maxConnection = 1;
      ssidHidden = 0;
   }
}
SoftAPInfo;

typedef struct BufferFlag_s
{
   bool isLocked;
   int ownerKey;

   void reset()
   {
      isLocked = false;
      ownerKey = 0;
   }

   BufferFlag_s()
   {
      reset();
   }
}
BufferFlag;

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

class ConnDeviceWifi : public ConnDevice
{
   friend class HtmlDocument;
   friend class Webserver;

public:
   virtual bool init();
   virtual bool deinit();
   virtual bool isReady();

   virtual bool submitData(ScaleItemData& itemData, bool isDeleted = false);
   virtual bool submitData(ScaleWeightData& itemData);
   virtual bool submitEmail(std::string& content);
   virtual bool submitSMS(std::string& content);
   virtual bool isInitialized();
   virtual bool initSession();
   virtual bool termSession();

   virtual bool setStage(ConnDeviceStage stage);

   virtual bool reqDateTime(time_t& oDateTimeStamp);
   virtual bool reqRateTable(const std::string& rateTableId, const int rateStatus);
   virtual ConnHttpStatusCode reqRegister(const std::string& deviceId, const std::string& userId, const std::string& regContent);

   virtual ConnDeviceType getConnDeviceType();

   static ConnDeviceWifi& getInstance();

   //Webserver app
   virtual bool scanDataRequest(std::string& oDataBuff);
   virtual bool respDataRequest(const int connId, const std::string& iDataBuff);
   //Webserver app

private:
   ConnDeviceWifi();
   virtual ~ConnDeviceWifi();

   bool reset();
   bool viewVersion();

   DeviceWifiMode getWifiMode();
   bool setWifiMode(DeviceWifiMode cwMode);

   DeviceWifiDhcp getWifiDhcp();
   bool setWifiDhcp(DeviceWifiDhcp cwDhcp);

   bool getLocalIpAddress(DeviceWifiLocalIp& localIp);
   bool setSizeOfSslBuffer(int size);
   bool startConnection(int connId, std::string type, std::string url, std::string port);
   bool startConnection(int connId, const UrlInfo& urlInf);
   bool closeConnection(int connId);
   bool setTcpServerTimeout(int timeOut);
   bool sendData(const int connId, const std::string& iDataBuff, std::string& oDataResp);

   //Wifi Station
   bool setAutoConnectAp(U8 enable = SETTING_OFF);
   bool setStationNetworkCfg(const DeviceWifiNetworkCfg& networkCfg);
   bool getStationNetworkCfg(DeviceWifiNetworkCfg& networkCfg);
   bool getListAP(std::list<AccessPointInfo>& apList, bool refresh = false);
   bool parseAP(std::string& apRawData, AccessPointInfo& apInfo);
   bool connectAP(std::string ssid, std::string passwd, std::string macaddr="");
   bool setDnsServer(std::string dnsUrl);
   NetworkConnStatus getNetworkStatus();
   //Wifi Station

   //Wifi SoftAP
   bool setSoftApCfg(const SoftAPInfo& softApInfo);
   bool getSoftApCfg(SoftAPInfo& softApInfo);
   bool setTcpServer(U8 mode, U16 port = 80);
   //Wifi SoftAP

   bool buildHttpRequest(std::string& outGetService, ConnHttpRequestType httpReqType, ConnHttpService httpService, std::list<std::string>& params);

   void lockScanBuffer(bool locked = true, int key = 1);

   std::list<AccessPointInfo> m_apList;
   BufferFlag m_deviceBuffer;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //CONNDEVICEWIFI_H_
