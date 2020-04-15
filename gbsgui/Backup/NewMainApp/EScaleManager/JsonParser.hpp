//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: JsonParser.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef JSONPARSER_H_
#define JSONPARSER_H_

#include "stapp_main.h"
#include "Json/json.h"
#include "GDebug.hpp"

#define RESP_JSON_START "#GBSJSONSTART#"
#define RESP_JSON_START_LEN 14
#define RESP_JSON_END   "#GBSJSONEND#"
#define RESP_JSON_END_LEN 12

//////////////////////////////////////////////////////////////////////////////////////
class JsonParser
{
public:
   JsonParser();
   ~JsonParser();

   U32 getUintValueNode(const Json::Value& parentNode, const char* pKeyNode);
   int getIntValueNode(const Json::Value& parentNode, const char* pKeyNode);
   bool getBooleanValueNode(const Json::Value& parentNode, const char* pKeyNode);
   bool traceNodeTree(Json::Value& node);
   bool getJsonObject(const std::string& dataBuffer, Json::Value& node);
   bool lookForRateTables(const Json::Value& node, std::list<std::string>& listRateTables);
   float getRealValueNode(const Json::Value& parentNode, const char* pKeyNode);
   std::string getStrValueNode(const Json::Value& parentNode, const char* pKeyNode);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //JSONPARSER_H_
