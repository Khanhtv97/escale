//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: RateManager.hpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef RATEMANAGER_H_
#define RATEMANAGER_H_

#define ESCALE_NUM_KEY         40 //from 01 to 40
#define PRODUCT_NUM_MAX      5000

#define PRODUCT_CODE_LEN 33
#define PRODUCT_NAME_LEN 33
#define PRODUCT_UNIT_LEN 13

#define TAG_RATE_TABLE_ID         "id"
#define TAG_RATE_TABLE_ACTIVEDATE "activatedDate"
#define TAG_RATE_TABLE_EXPIREDATE "expiredDate"
#define TAG_RATE_TABLE_ACTIVATED  "isActivated"
#define TAG_RATE_TABLE_TOTALITEM  "totalItem"
#define TAG_RATE_TABLE_ITEMS      "items"

#define TAG_PRODUCT_NUMBER        "number"
#define TAG_PRODUCT_PRICE         "price"
#define TAG_PRODUCT_EXPDAY        "expDate"
#define TAG_PRODUCT_TEMPERATURE   "temperature"
#define TAG_PRODUCT_CODE          "code"
#define TAG_PRODUCT_NAME          "name"
#define TAG_PRODUCT_UNIT          "unit"

typedef enum RateTableObject_e
{
   ESCALE_RATE_TABLE_1 = 0,
   ESCALE_RATE_TABLE_2,
   ESCALE_RATE_TABLE_3,
   ESCALE_NUM_RATE_TABLE
}
RateTableObject;

typedef struct ProductInfo_s
{
   U32 id;
   char code[PRODUCT_CODE_LEN];
   char name[PRODUCT_NAME_LEN];
   char unit[PRODUCT_UNIT_LEN];
   float price;
   float expdate;
   float temperature;

   ProductInfo_s()
   {
      id = 0;
      price = 0;
      expdate = 0;
      temperature = 0;
      memset(code, 0, PRODUCT_CODE_LEN);
      memset(name, 0, PRODUCT_NAME_LEN);
      memset(unit, 0, PRODUCT_UNIT_LEN);
   }
}
ProductInfo;

typedef struct RateTableHeader_s
{
   U32 id;
   bool isActivated;
   time_t activatedDate;
   time_t expiredDate;
   U32 totalProduct;

   RateTableHeader_s()
   {
      id = 0;
      isActivated = false;
      activatedDate = 0;
      expiredDate = 0;
      totalProduct = 0;
   }
}
RateTableHeader;

typedef struct RateTable_s
{
   RateTableHeader header;
   ProductInfo products[PRODUCT_NUM_MAX];
}
RateTable;

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

class RateManager
{
   friend class SaveDataToFlash;

public:
   static RateManager& getInstance();

   RateTableObject addRateTable(const RateTable* pRateTable);

   bool init();
   bool deinit();

   bool getProductInfo(const U32 productId, ProductInfo& oProductInfo);
   bool getProductInfo(const char* productCode, ProductInfo& oProductInfo);
   bool getProductInfo(const std::string productCode, ProductInfo& oProductInfo);
   bool getRateTableInfo(const int slotId, RateTableHeader& rateTableHeader);

   bool hasActiveRateTableReached();
   U32 lookForActiveRateTable();
   U32 setLastActiveRateTable();
   void getActivatedRateTable(RateTable* pRateTable);

   void setPendingRateTable(const std::list<std::string>& rateTableIdList);
   void removePendingRateTableId(const std::string& rateTableId);
   bool getNextPendingRateTableId(std::string& rateTableId);

   //ForDebug
   void createDemoRateTable();
   //ForDebug

protected:
   void serializeData(const RateTableObject rateTable, std::string& outData);
   bool unserializeData(const RateTableObject rateTable, const U8* data);

private:
   RateManager();
   virtual ~RateManager();

   RateTableObject findOldestRateTable();
   //for test only
   void initTestRateTable();
   //for test only

   bool m_isInternalProccessing;
   std::list<std::string> m_pendingRateTableIds;
   RateTable m_rateTables[ESCALE_NUM_RATE_TABLE];
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //RATEMANAGER_H_
