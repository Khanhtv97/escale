//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: BarcodeReader.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#include "BarcodeReader.hpp"
#include "GStringUtils.hpp"
#include "GUserSettings.hpp"
#include "JsonParser.hpp"
#include "SaveDataToFlash.hpp"

#define ASCII_CR 0x0D
#define ASCII_LF 0x0A

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

BarcodeReader::BarcodeReader() : SerialPort(SPN_COM2)
{
}

//////////////////////////////////////////////////////////////////////////////////////
BarcodeReader::~BarcodeReader()
{
}

//////////////////////////////////////////////////////////////////////////////////////
BarcodeReader& BarcodeReader::getInstance()
{
   static BarcodeReader instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
bool BarcodeReader::init()
{
}

//////////////////////////////////////////////////////////////////////////////////////
bool BarcodeReader::deinit()
{
}

//////////////////////////////////////////////////////////////////////////////////////
bool BarcodeReader::isReady()
{
}

//////////////////////////////////////////////////////////////////////////////////////
BarcodeContentType BarcodeReader::scanReader(std::string& outContent)
{
   BarcodeContentType ret = BCT_NONE;
   U8 pBuffer[SERIAL_BUFFER_SIZE];
   U32 numRead = 0;

   outContent = "";

   readBytes(pBuffer, numRead);

   //_DTraceDebug("Read step1: %s", pBuffer);

   while(numRead > 0)
   {
      std::string strRead((char*)pBuffer);
      outContent.append(strRead);
      //Make sure that the reader's buffer is empty
      memset(pBuffer, 0, SERIAL_BUFFER_SIZE);
      numRead = 0;
      Task_Delay(ST_GetClocksPerSecond()/55);
      readBytes(pBuffer, numRead);
   }

   if( outContent.length() > 0 )
   {
      _DTraceDebug("%s", outContent.c_str());
      ret = getContentType(outContent);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
BarcodeContentType BarcodeReader::getContentType(std::string& inContent)
{
   BarcodeContentType ret = BCT_DATA;

   JsonParser jsParser;
   Json::Value nodeRoot;

   if( jsParser.getJsonObject(inContent, nodeRoot) )
   {
      std::string sId = jsParser.getStrValueNode(nodeRoot, "id");
      std::string sKey = jsParser.getStrValueNode(nodeRoot, "key");
      if( sId.find("GBSW-1078") != std::string::npos &&
          sKey.find("0797c70cdf66f252ab346354d914ae37") != std::string::npos )
      {
        UrlInfo urlInfos;
        urlInfos.url = jsParser.getStrValueNode(nodeRoot, "url");
        urlInfos.port = jsParser.getStrValueNode(nodeRoot, "port");
        urlInfos.ssl = (U8)jsParser.getUintValueNode(nodeRoot, "ssl");
        _U_.setSubmitDataUrl(urlInfos);
        _Sa_.serialize(SERIALIZABLE_PARTITION_1);
        ret = BCT_SUBMIT_DATA_URL;
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
