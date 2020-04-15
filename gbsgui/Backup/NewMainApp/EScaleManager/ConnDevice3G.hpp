//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ConnDevice3G.hpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef CONNDEVICE3G_H_
#define CONNDEVICE3G_H_

#include "ConnDevice.hpp"


typedef enum NetworkCode_s
{
   NWC_NONE,
   NWC_MOBIFONE,
   NWC_VINAPHONE,
   NWC_VIETEL,
   NWC_VIETNAMMOBILE
}
NetworkCode;

typedef struct ConnDateTime_s
{
   int year;
   int month;
   int day;
   int hour;
   int minute;
   int second;

   void reset()
   {
      year = 2016;
      month = 1;
      day = 1;
      hour = 0;
      minute = 0;
      second = 1;
   }

   void ConnDateTime()
   {
      reset();
   }
}
ConnDateTime;

typedef struct ConnLocation_s
{
   float longtitude;
   float latitude;

   void reset()
   {
      longtitude = 0;
      latitude = 0;
   }

   void ConnLocation()
   {
      reset();
   }
}
ConnLocation;

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

class ConnDevice3G : public ConnDevice
{
   friend class ConnManager;

protected:
   virtual bool init();
   virtual bool deinit();
   virtual bool isReady();

   virtual bool submitData(ScaleItemData& itemData, bool isDeleted = false);
   virtual bool submitData(ScaleWeightData& itemData);
   virtual bool submitEmail(std::string& content);
   virtual bool submitSMS(std::string& content);
   virtual bool isInitialized();
   virtual bool setStage(ConnDeviceStage stage);

   virtual bool reqDateTime(time_t& oDateTimeStamp);
   virtual bool reqRateTable(const std::string& rateTableId, const int rateStatus);
   virtual ConnHttpStatusCode reqRegister(const std::string& deviceId, const std::string& userId, const std::string& regContent);

   //Webserver app
   virtual bool scanDataRequest(std::string& oDataBuff);
   virtual bool respDataRequest(const int connId, const std::string& iDataBuff);
   //Webserver app

   virtual ConnDeviceType getConnDeviceType();
   static ConnDevice3G& getInstance();

private:
   ConnDevice3G();
   virtual ~ConnDevice3G();

   void powerOff();

   int  getBearerStatus();
   bool setBearerConnection(bool switchOn=true);

   bool initHttp();
   bool termHttp();
   bool sendHttpRequest(ConnHttpRequestType httpReqType);
   bool sendHttpRead();
   bool sendSMS(std::string txtPhoneNum, std::string txtMessage);
   bool sendEmail(MailToList& receivers, std::string& subject, std::string& body);

   bool buildHttpRequest(std::string& outGetService, ConnHttpRequestType httpReqType, ConnHttpService httpService, std::list<std::string>& httpParams, int timeOut=120);

   bool getConfiguration(std::string& sName, std::string& sEmail, std::string& sNumber);
   bool lookForConfiguration();

   bool getGsmLocationAndTime(ConnLocation& connLocation, ConnDateTime& connDateTime);
   bool setLocalTime(std::string localTime);
   bool getLocalTime(std::string& oDateTime, std::string& oTimeZone);
   bool getCCID(std::string& oCcidNumber);
   bool getCUSD(std::string& oCusdInfo);
   bool getSubcriberNum(std::string& oSubcriberNum);

   ConnLocation m_connLocation;
   ConnDateTime m_connDateTime;
   NetworkCode m_networkCode;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //CONNDEVICE3G_H_
