//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ConnManager.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef CONNMANAGER_H_
#define CONNMANAGER_H_

#include "ConnDevice.hpp"
#include <queue>

#include "Json/json.h"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

class ConnManager
{
public:
   static ConnManager& getInstance();

   bool init();
   bool deinit();
   bool isDataEmpty();
   bool isDataSending();

   void deleteData(ScaleItemData& itemData);
   void hasData(ScaleItemData& itemData);
   void hasData(ScaleWeightData& itemData);
   void hasData(std::list<ScaleItemData>& receiptData);

protected:
   static void MainTask(void* Parameter);
   static Task_t m_connectionMainTaskId;
   static bool m_isConnectionMainTaskRuning;

   bool isDeviceReady();
   bool isDeviceInitialized();
   bool initDevice();
   bool deinitDevice();

   void submitItemData();
   void submitItemDataDeleted();
   void submitWeighData();
   void submitEmailData();
   void submitSmsData();

   void updateSystemDateTime();
   void updateRateTable(const std::string rateTableId);

   void setDeviceStage(ConnDeviceStage stage);
   void processClientRequest();
   bool isClientConnecting();

private:
   ConnManager();
   virtual ~ConnManager();
   void createTask();
   void deleteTask();

   bool traceNodeTree(Json::Value& node);

   std::queue<ScaleItemData> m_itemDataQueues;
   std::queue<ScaleItemData> m_itemDataDeletedQueues;
   std::queue<ScaleWeightData> m_weighDataQueues;
   std::queue<std::string> m_emailDataQueues;
   std::queue<std::string> m_smsDataQueues;

   std::queue<U32> m_rateTableQueues;

   ConnDevice* m_pConnDevice;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //CONNMANAGER_H_
