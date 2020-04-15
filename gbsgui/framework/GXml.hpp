//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2015 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GXml.hpp                                                                   //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GXML_HPP_
#define GXML_HPP_

#include "stapp_main.h"
#include "tinyxml/tinyxml.h"

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////

class GXmlNode : private TiXmlNode
{
public:
   GXmlNode();
   virtual ~GXmlNode();
};

//////////////////////////////////////////////////////////////////////////////////////
class GXmlElement : private TiXmlElement
{
public:
   GXmlElement(const char* pValue) : TiXmlElement(pValue){};
   GXmlElement(const std::string& value) : TiXmlElement(value){};
   GXmlElement(const GXmlElement& element) : TiXmlElement( (TiXmlElement&)element){};
   virtual ~GXmlElement(){};

   const char* addAttribute(const char* name);
   const char* addAttribute(const char* name, int* pValue);
   const char* addAttribute(const char* name, double* pValue);

   int getIntAttribute(const char* name, int* pValue);
   int getUIntAttribute(const char* name, unsigned int* pValue);
   int getBoolAttribute(const char* name, bool* pValue);
   int getDoubleAttribute(const char* name, double* pValue);
   int getFloatAttribute(const char* name, float* pValue);
   int getStringAttribute(const char* name, std::string* pValue);

   void setAttribute(const char* name, int value);
   void setAttribute(const char* name, unsigned int value);
   void setAttribute(const char* name, bool value);
   void setAttribute(const char* name, double value);
   void setAttribute(const char* name, char* value);
   void setAttribute(const char* name, std::string& value);

   bool removeNode(GXmlElement* pChildNode);
   GXmlElement* addNode(GXmlElement* pChildNode);

   GXmlElement* getFirstNode();
   GXmlElement* getFirstNode(const char* name);
   GXmlElement* getLastNode();
   GXmlElement* getLastNode(const char* name);
   GXmlElement* getNextNode();
   GXmlElement* getNextNode(const char* name);
   GXmlElement* getPrevNode();
   GXmlElement* getPrevNode(const char* name);

};

//////////////////////////////////////////////////////////////////////////////////////

class GXml
{
public:
   GXml();
   virtual ~GXml();

   bool loadFile(const std::string filename);
   bool saveFile(const std::string filename, GXmlElement* pRootElement);

   GXmlElement* getRoot();

private:
   TiXmlDocument m_xmlDocument;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif //_GXML_HPP_
