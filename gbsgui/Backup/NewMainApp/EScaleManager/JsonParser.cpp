//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: JsonParser.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#include "JsonParser.hpp"
#include "GStringUtils.hpp"

//////////////////////////////////////////////////////////////////////////////////////
JsonParser::JsonParser()
{
}

//////////////////////////////////////////////////////////////////////////////////////
JsonParser::~JsonParser()
{
}

//////////////////////////////////////////////////////////////////////////////////////
bool JsonParser::getJsonObject(const std::string& dataBuffer, Json::Value& node)
{
   bool ret = false;

   _DTraceDebug("%s", dataBuffer.c_str());

   size_t startPos = dataBuffer.find(RESP_JSON_START);
   size_t endPos = 0;

   if( startPos != std::string::npos )
   {
      startPos += RESP_JSON_START_LEN;
      endPos = dataBuffer.find(RESP_JSON_END, startPos);

      if( endPos > startPos)
      {
         std::string sJsonBuffer = dataBuffer.substr(startPos, endPos-startPos);
         // json parse here
         Json::Features jsFeatures;
         Json::Reader jsReader(jsFeatures);
         if( jsReader.parse(sJsonBuffer.c_str(), node) )
         {
            if( node.type() != Json::nullValue )
            {
               ret = true;
            }
            else
            {
               _DTraceDebug("Json::Reader parses root => nullValue !");
            }
         }
         else
         {
            _DTraceDebug("Json::Reader parses failed, buffer length = %d !", sJsonBuffer.length());
            printf("%s", sJsonBuffer.c_str());
         }
      }
      else
      {
         _DTraceDebug("Not found %s", RESP_JSON_END);
      }
   }
   else
   {
      _DTraceDebug("Not found %s", RESP_JSON_START);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
std::string JsonParser::getStrValueNode(const Json::Value& parentNode, const char* pKeyNode)
{
   std::string ret = "";

   if( !parentNode.isObject() || !parentNode.isMember(pKeyNode) )
   {
      _DTraceDebug("Is not member %s", pKeyNode);
      return ret;
   }

   Json::Value defaultValue;
   Json::Value nodeValue = parentNode.get(pKeyNode, defaultValue);

   if( nodeValue.type() == Json::stringValue )
   {
      ret.assign(nodeValue.asString());
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool JsonParser::getBooleanValueNode(const Json::Value& parentNode, const char* pKeyNode)
{
   bool ret = false;

   if( !parentNode.isObject() || !parentNode.isMember(pKeyNode) )
   {
      _DTraceDebug("failed, there is no member %s", pKeyNode);
      return ret;
   }

   Json::Value defaultValue;
   Json::Value nodeValue = parentNode.get(pKeyNode, defaultValue);

   if( nodeValue.type() == Json::booleanValue ||
       nodeValue.type() == Json::realValue ||
       nodeValue.type() == Json::intValue ||
       nodeValue.type() == Json::uintValue )
   {
      ret = nodeValue.asBool();
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 JsonParser::getUintValueNode(const Json::Value& parentNode, const char* pKeyNode)
{
   U32 ret = 0;

   if( !parentNode.isObject() || !parentNode.isMember(pKeyNode) )
   {
      _DTraceDebug("failed, there is no member %s", pKeyNode);
      return ret;
   }

   Json::Value defaultValue;
   Json::Value nodeValue = parentNode.get(pKeyNode, defaultValue);

   if( nodeValue.type() == Json::uintValue ||
       nodeValue.type() == Json::intValue ||
       nodeValue.type() == Json::booleanValue)
   {
      ret = (U32)nodeValue.asUInt();
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
int JsonParser::getIntValueNode(const Json::Value& parentNode, const char* pKeyNode)
{
   int ret = 0;

   if( !parentNode.isObject() || !parentNode.isMember(pKeyNode) )
   {
      _DTraceDebug("failed, there is no member %s", pKeyNode);
      return ret;
   }

   Json::Value defaultValue;
   Json::Value nodeValue = parentNode.get(pKeyNode, defaultValue);

   if( nodeValue.type() == Json::intValue || nodeValue.type() == Json::booleanValue )
   {
      ret = (int)nodeValue.asInt();
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
float JsonParser::getRealValueNode(const Json::Value& parentNode, const char* pKeyNode)
{
   float ret = 0;

   if( !parentNode.isObject() || !parentNode.isMember(pKeyNode) )
   {
      _DTraceDebug("failed, there is no member %s", pKeyNode);
      return ret;
   }

   Json::Value defaultValue;
   Json::Value nodeValue = parentNode.get(pKeyNode, defaultValue);

   if( nodeValue.type() == Json::realValue ||
       nodeValue.type() == Json::intValue ||
       nodeValue.type() == Json::uintValue ||
       nodeValue.type() == Json::booleanValue)
   {
      ret = (float)nodeValue.asDouble();
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool JsonParser::lookForRateTables(const Json::Value& node, std::list<std::string>& listRateTables)
{
   Json::Value defaultValue;
   Json::Value jsNodeItems = node.get("ratetables", defaultValue);

   listRateTables.clear();

   if( jsNodeItems.type() == Json::arrayValue )
   {
      int sizeOfNode = jsNodeItems.size();
      for( int i = 0; i < sizeOfNode; ++i )
      {
         if( jsNodeItems[i].isMember("ratetabeid") )
         {
            std::string sRateTableId;
            U32 rateTableId = getUintValueNode(jsNodeItems[i], "ratetabeid");
            GStringUtils::format(sRateTableId, "%d", rateTableId);
            _DTraceDebug("Found rate table [%d], id = %s", i+1, sRateTableId.c_str());
            listRateTables.push_back(sRateTableId);
         }
      }
   }

   return !listRateTables.empty();
}

//////////////////////////////////////////////////////////////////////////////////////
bool JsonParser::traceNodeTree(Json::Value& node)
{
   bool ret = false;

   switch( node.type() )
   {
   case Json::nullValue:
      _DTraceDebug("nullValue");
      ret = true;
      break;

   case Json::intValue:
      _DTraceDebug("IntValue = %d\n", node.asInt() );
      ret = true;
      break;

   case Json::uintValue:
      _DTraceDebug("uintValue = %d\n", node.asUInt() );
      ret = true;
      break;

   case Json::realValue:
      _DTraceDebug("realValue = %d\n", node.asDouble() );
      ret = true;
      break;

   case Json::stringValue:
      _DTraceDebug("stringValue = %s\n", node.asString().c_str());
      ret = true;
      break;

   case Json::booleanValue:
      _DTraceDebug("booleanValue = %d\n", node.asBool());
      ret = true;
      break;

   case Json::arrayValue:
   {
      int sizeOfNode = node.size();
      //parse for each value[index]
      _DTraceDebug( "sizeOfNode = %d\n", sizeOfNode);

      for( int i = 0; i < sizeOfNode; ++i )
      {
         _DTraceDebug( "---> [Array] Parse node %d\n", i);
         traceNodeTree(node[i]);
      }
      ret = true;
      break;
   }

   case Json::objectValue:
   {
      _DTraceDebug("Json::objectValue");
      Json::Value::Members members(node.getMemberNames());
      for( Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it )
      {
         const std::string &name = *it;
         _DTraceDebug( "---> [Object] Parse node %s\n", name.c_str());
         traceNodeTree(node[name]);
      }
      ret = true;
   }
      break;

   default:
      break;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
