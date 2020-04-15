//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ConnDevice.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef CONNDEVICE_H_
#define CONNDEVICE_H_

#include "SerialPort.hpp"
#include "ScaleManager.hpp"
#include "GSystem.hpp"
#include "GUserSettings.hpp"
#include "GStringUtils.hpp"
#include "JsonParser.hpp"

#define RESPONSE_MAX_TIMEOUT 120 //in 100ms

#define ASCII_LF  0x0A
#define ASCII_CR  0x0D
#define ASCII_SUB 0x1A

#define MAX_CONNECTION_ID 4
#define MAX_NUM_TRYING_TO_SEND 3

#define SERVICE_SUBMIT_DATA_NAME        "/datalog"
#define SERVICE_SUBMIT_DATA_PARAMS      "timestamp=%s&deviceid=%s&packageid=%s&productid=%s&value=%s&price=%s&unit=%s"
#define SERVICE_DEVICE_REGISTER_NAME    "/deviceregister"
#define SERVICE_DEVICE_REGISTER_PARAMS  "deviceid=%s&userid=%s&regcontent=%s"
#define SERVICE_UPDATE_DATETIME_NAME    "/currentdatetime"
#define SERVICE_UPDATE_DATETIME_PARAMS  "deviceid=%s"
#define SERVICE_UPDATE_RATETABLE_NAME   "/ratetable"
#define SERVICE_UPDATE_RATETABLE_PARAMS "deviceid=%s&ratetableid=%s&updatestatus=%s"

#define SERVICE_SUBMIT_DATA_VNPOST_NAME   "/gqr1907.php"
//#define SERVICE_SUBMIT_DATA_VNPOST_NAME   "/API_HCM/InsertItemGBS"
#define SERVICE_SUBMIT_DATA_GET_VNPOST_PARAMS "device=%s&qrcode=%s&weight=%s"
#define SERVICE_SUBMIT_DATA_POST_VNPOST_PARAMS "{\"device\":\"%s\",\"qrcode\":\"%s\",\"weight\":\"%s\"}"

#define REQ_RATETABLE_UPDATE_NEW 1
#define REQ_RATETABLE_UPDATE_OK  2

#define MAX_SESSION_TIMEOUT 234

typedef enum ConnHttpService_e
{
   CHS_DEVICE_REGISTER = 0,
   CHS_SUBMIT_DATA,
   CHS_RATE_TABLE,
   CHS_UPDATE_DATETIME,
   CHS_SUBMIT_DATA_VNPOST,
}
ConnHttpService;

typedef enum ConnHttpRequestType_e
{
   HTTP_REQ_GET,
   HTTP_REQ_POST,
   HTTP_REQ_PUSH,
   HTTP_REQ_HEAD
}
ConnHttpRequestType;

typedef enum ConnHttpStatusCode_e
{
   //1xx Informational
   HTTP_100 = 100, //100 Continue,
   HTTP_101,       //101 Switching Protocols,
   HTTP_102,       //102 Processing,
   //2xx Success
   HTTP_200 = 200, //200 OK,
   HTTP_201,       //201 Created,
   HTTP_202,       //202 Accepted,
   HTTP_203,       //203 Non-Authoritative,
   HTTP_204,       //204 No Content,
   HTTP_205,       //205 Reset Content,
   HTTP_206,       //206 Partial Content,
   HTTP_207,       //207 Multi-Status,
   HTTP_208,       //208 Already Reported,
   HTTP_209,       //226 IM Used,
   //3xx Redirection
   HTTP_300 = 300, //300 Multiple Choices,
   HTTP_301,       //301 Moved Permanently,
   HTTP_302,       //302 Found,
   HTTP_303,       //303 See Other,
   HTTP_304,       //304 Not Modified,
   HTTP_305,       //305 Use Proxy,
   HTTP_306,       //306 Switch Proxy,
   HTTP_307,       //307 Temporary Redirect,
   HTTP_308,       //308 Permanent Redirect,
   //4xx Client Error
   HTTP_400 = 400, //400 Bad Request,
   HTTP_401,	   //401 Unauthorized,
   HTTP_402,       //402 Payment Required,
   HTTP_403,       //403 Forbidden,
   HTTP_404,       //404 Not Found,
   HTTP_405,       //405 Method Not Allowed,
   HTTP_406,       //406 Not Acceptable,
   HTTP_407,       //407 Proxy Authentication Required,
   HTTP_408,       //408 Request Time-out,
   HTTP_409,       //409 Conflict,
   //5xx Server Error
   HTTP_500 = 500, //500 Internal Server Error,
   HTTP_501,       //501 Not Implemented,
   HTTP_502,       //502 Bad Gateway,
   HTTP_503,       //503 Service Unavailable,
   HTTP_504,       //504 Gateway Time-out,
   HTTP_505,       //505 HTTP Version Not Supported,
   HTTP_507 = 507, //507 Insufficient Storage,
   HTTP_508 = 508, //508 Loop Detected,
   HTTP_510 = 510, //510 Not Extended,
   HTTP_511 = 511  //511 Network Authentication Required
}
ConnHttpStatusCode;

typedef enum ConnDeviceType_e
{
   CDT_NONE = 0,
   CDT_GPRS,
   CDT_3G,
   CDT_4G,
   CDT_WIFI,
   CDT_ETHENET
}
ConnDeviceType;

typedef enum ConnDeviceStage_e
{
   CDS_NONE = 0,
   CDS_INIT,
   CDS_SUBMIT,
   CDS_CONFIG
}
ConnDeviceStage;

typedef struct CmdParameter_s
{
   std::string value;
   bool isNumeric;
}
CmdParameter;

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////
class ConnDevice : public SerialPort
{
public:
   virtual bool init();
   virtual bool deinit();
   virtual bool isReady();

   virtual bool submitData(ScaleItemData& itemData, bool isDeleted = false) = 0;
   virtual bool submitData(ScaleWeightData& itemData) = 0;
   virtual bool submitEmail(std::string& content) = 0;
   virtual bool submitSMS(std::string& content) = 0;
   virtual bool isInitialized() = 0;
   virtual bool initSession() = 0;
   virtual bool termSession() = 0;

   virtual bool setStage(ConnDeviceStage stage) = 0;

   virtual bool reqDateTime(time_t& oDateTimeStamp) = 0;
   virtual bool reqRateTable(const std::string& rateTableId, const int rateStatus) = 0;
   virtual ConnHttpStatusCode reqRegister(const std::string& deviceId, const std::string& userId, const std::string& regContent) = 0;

   //Webserver app
   virtual bool scanDataRequest(std::string& oDataBuff) = 0;
   virtual bool respDataRequest(const int connId, const std::string& iDataBuff) = 0;
   //Webserver app

   virtual ConnDeviceType getConnDeviceType() = 0;

protected:
   ConnDevice();
   ConnDevice(SerialPortName portName);
   virtual ~ConnDevice();

   void traceBuffer();

   void encodeAtParams(std::string& ioParams);

   bool writeCommand(std::string command, std::list<CmdParameter>& params, std::string& oResponseBuff, U32 timeOut = 0);
   void readBuffer(std::string& oResponseBuff);
   void setDeviceId(const std::string& deviceId);

   void buildHttpParams(std::string& outHttpParams, ConnHttpService httpService, ConnHttpRequestType httpRequestType, std::list<std::string>& params);
   void getHttpServiceName(std::string& outServiceName, ConnHttpService httpService);

   bool checkHttpRespDone(std::string& httpResponse);

   //temporary
   void appendBufferIn(std::string& sBuffer);
   void getBufferIn(std::string& sBuffer);
   void clearBufferIn();
   bool parseBufferIn();

   std::string m_RAWDATABUFFER;
   //temporary

   std::string m_deviceId;
   ConnDeviceStage m_deviceStage;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //CONNDEVICE_H_
