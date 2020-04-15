//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ScaleManager.hpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef SCALEMANAGER_H_
#define SCALEMANAGER_H_

#include "SerialPort.hpp"
#include "GUserSettings.hpp"
#include "RateManager.hpp"

typedef enum ScaleDataType_e
{
   SDT_NONE,
   SDT_FORMAT1,
   SDT_FORMAT2,
   SDT_FORMAT2_DEL,
   SDT_FORMAT2_MC,
}
ScaleDataType;

typedef enum ScaleUnit_e
{
   SUN_NONE,
   SUN_KG,  //Kilograms (kg) ie: 60 kg
   SUN_G,   //Grams (g)      ie: 60 g
   SUN_LB,  //Pounds (lb)    ie: 132.277 lb
   SUN_HKG, //I don't know this unit of weight
   SUN_LBOZ,//Pounds+Ounces (lboz) ie: 132 lb 4.438 oz
   SUN_PCS  //PCs
}
ScaleUnit;

typedef enum ScaleHeader_e
{
   SHE_NONE,
   SHE_OL,  //Over Load
   SHE_ST,  //Stable
   SHE_US,  //Unstable
   SHE_NT,  //Net Weight
   SHE_GS   //Gross Weight
}
ScaleHeader;

typedef struct ScaleWeightData_s
{
   ScaleHeader header1;
   ScaleHeader header2;
   bool sign;
   float data1;
   float data2;
   ScaleUnit unit;
   time_t timestamp;
   std::string scaleCode;

   void clear()
   {
      header1 = SHE_NONE;
      header2 = SHE_NONE;
      sign = true;
      data1 = 0;
      data2 = 0;
      unit = SUN_NONE;
      timestamp = 0;
      scaleCode = "";
   };

   void ScaleWeightData()
   {
      clear();
   };
}
ScaleWeightData;

typedef enum ScaleItemType_e
{
   SIT_NONE,
   SIT_ITEM,  //Item
   SIT_TOTAL  //Change
}
ScaleItemType;

typedef struct ScaleItemData_s
{
   ScaleItemType type;
   std::string typeName;
   ScaleUnit unit;
   float dataNet;
   float dataTare;
   float unitPrice;
   float totalPrice;
   float pay;
   float change;
   time_t timeStamp;
   std::string scaleCode;

   void clear()
   {
      type = SIT_NONE;
      typeName = "";
      unit = SUN_NONE;
      dataNet = 0;
      dataTare = 0;
      unitPrice = 0;
      totalPrice = 0;
      pay = 0;
      change = 0;
      timeStamp = 0;
      scaleCode = "";
   };

   void ScaleItemData()
   {
      clear();
   };
}
ScaleItemData;

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

class ScaleManager : public SerialPort
{
public:
   virtual bool init();
   virtual bool deinit();
   virtual bool isReady();

   static ScaleManager& getInstance();

   ScaleDataType scanWeightTx();
   ScaleDataType getDataType();

   void getWeightData(ScaleWeightData& oWeightData);
   void getItemDatas(std::list<ScaleItemData>& oItemDatas);
   void getLastItemData(ScaleItemData& oLastItemData);
   void getItemDataDeleted(ScaleItemData& oItemDataDeleted);

   void resetItemDatas();
   void updateRateTable();

   void setPriceLookup(const std::string productCode);

   std::string header2Str(ScaleHeader header);
   std::string unit2Str(ScaleUnit unit);
   std::string data2Str(float data);

   std::string genPackageId(char* pCode, time_t timeOfWeight);

private:
   ScaleManager();
   virtual ~ScaleManager();

   ScaleDataType parseWeightData(U8* pBuffer, U32 length);

   //For scaleWeightData
   ScaleHeader getHeader1(U8* pBuffer, U32 length);
   ScaleHeader getHeader2(U8* pBuffer, U32 length);
   ScaleUnit getUnit(U8* pBuffer, U32 length);
   bool isNeedMoreData(U8* pBuffer, U32 length);
   bool getSign(U8* pBuffer, U32 length);
   float getData1(U8* pBuffer, U32 length);
   float getData2(U8* pBuffer, U32 length);
   //For scaleWeightData

   //For scaleItemData
   ScaleItemType getItemType(std::list<std::string>& lstItems, std::string& typeName);
   float getItemDataNet(std::list<std::string>& lstItems, ScaleUnit& itemUnit);
   float getItemDataTare(std::list<std::string>& lstItems, ScaleUnit withUnit);
   float getItemUnitPrice(std::list<std::string>& lstItems);
   float getItemTotalPrice(std::list<std::string>& lstItems);
   float getItemPay(std::list<std::string>& lstItems);
   float getItemChange(std::list<std::string>& lstItems);
   //For scaleItemData

   bool isItemDeleted(U8* pBuffer, U32 length, std::string& itemDeleted);
   bool deleteItemData(U32 itemId);

   void updatePrice(U32 keyId, float price);

   void traceData(ScaleDataType dataType);

   ScaleDataType m_scaleDataType;
   ScaleWeightData m_scaleWeightData;
   ScaleItemData m_scaleItemDataDeleted;
   std::list<ScaleItemData> m_scaleItemDatas;
   ProductInfo m_lastProductInfo;
   bool m_isUpdatingRateTable;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //SCALEMANAGER_H_
