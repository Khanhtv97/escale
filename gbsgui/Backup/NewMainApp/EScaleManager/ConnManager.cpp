//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ConnManager.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "ConnManager.hpp"
#include "GStringUtils.hpp"
#include "GSystem.hpp"
#include "ConnDevice3G.hpp"
#include "ConnDeviceWifi.hpp"
#include "GUserSettings.hpp"
#include "Webserver.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

Task_t ConnManager::m_connectionMainTaskId = (Task_t)NULL;
bool ConnManager::m_isConnectionMainTaskRuning = false;

static Semaphore_t g_SemaphoreLockItemData;
static Semaphore_t g_SemaphoreLockItemDataDeleted;
static Semaphore_t g_SemaphoreLockWeighData;
static Semaphore_t g_SemaphoreLockReceiptData;
static bool g_SemaphoreLockItemDataNotified = false;
static bool g_SemaphoreLockItemDataDeletedNotified = false;
static bool g_SemaphoreLockWeighDataNotified = false;
static bool g_SemaphoreLockReceiptDataNotified = false;

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::createTask()
{
   if( ConnManager::m_isConnectionMainTaskRuning )
   {
      return;
   }

   if( ConnManager::m_connectionMainTaskId != (Task_t)NULL )
   {
      deleteTask();
   }

   ConnManager::m_connectionMainTaskId = Task_Create(ConnManager::MainTask, NULL, 32768, MIN_USER_PRIORITY, "EScaleConnectionTask", 0, ConnManager::m_connectionMainTaskId);

   if( ConnManager::m_connectionMainTaskId != (Task_t)NULL )
   {
      ConnManager::m_isConnectionMainTaskRuning = true;
      _DTraceDebug("EScaleConnectionTask created\n");
   }
   else
   {
      ConnManager::m_isConnectionMainTaskRuning = false;
      _DTraceDebug("Cannot create EScaleConnectionTask\n");
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::deleteTask()
{
    if( ConnManager::m_connectionMainTaskId != (Task_t)NULL )
    {
       Task_Wait(ConnManager::m_connectionMainTaskId);
       task_kill(ConnManager::m_connectionMainTaskId, 0, 0);
       Task_Delete(ConnManager::m_connectionMainTaskId);
    }

    ConnManager::m_connectionMainTaskId = (Task_t)NULL;
    ConnManager::m_isConnectionMainTaskRuning = false;
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::MainTask(void* Parameter)
{
   int mainTaskCounter = 0;

   while( ConnManager::m_isConnectionMainTaskRuning )
   {
      Task_Delay(ST_GetClocksPerSecond()/3);

      //The connection device should be ready
      if( !ConnManager::getInstance().isDeviceInitialized() )
      {
         if( ConnManager::getInstance().isDeviceReady() )
         {
            mainTaskCounter = 0;
            ConnManager::getInstance().initDevice();
            Task_Delay(ST_GetClocksPerSecond());
         }
         else
         {
            if( mainTaskCounter > 24 )
            {
               //can not wait after 120 seconds
               ConnManager::getInstance().deinitDevice();
               mainTaskCounter = 0;
            }
            Task_Delay(5*ST_GetClocksPerSecond());
            mainTaskCounter++;
            continue;
         }
      }

      /*
      //Test if there is a request from client
      if( ConnManager::getInstance().isDeviceInitialized() )
      {
         ConnManager::getInstance().processClientRequest();
         if( ConnManager::getInstance().isClientConnecting() )
         {
            Task_Delay(ST_GetClocksPerSecond()/3);
            continue;
         }
      }

      //Test if the system time is updated or not
      if( _S_.getBaseTime() == SYSTEM_TIME_BASE_INIT )
      {
         //update system time
         if( mainTaskCounter > 33 )
         {
            ConnManager::getInstance().updateSystemDateTime();
            Task_Delay(ST_GetClocksPerSecond());
            mainTaskCounter = 0;
         }
         mainTaskCounter++;
         continue;
      }

      //Test if there is a rate need update
      std::string sRateTableId;
      if( RateManager::getInstance().getNextPendingRateTableId(sRateTableId) )
      {
         ConnManager::getInstance().updateRateTable(sRateTableId);
         Task_Delay(ST_GetClocksPerSecond());
         continue;
      }
      */

      //Try to submit weigh data
      ConnManager::getInstance().submitWeighData();
      //Try to submit item data
      ConnManager::getInstance().submitItemData();
      //Try to submit item data deleted
      //ConnManager::getInstance().submitItemDataDeleted();
      //Try to send sms
      //ConnManager::getInstance().submitSmsData();
      //Try to send email
      //ConnManager::getInstance().submitEmailData();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
ConnManager::ConnManager()
{
   m_pConnDevice = NULL;
   Semaphore_Init_Fifo(g_SemaphoreLockItemData, 0);
   Semaphore_Init_Fifo(g_SemaphoreLockItemDataDeleted, 0);
   Semaphore_Init_Fifo(g_SemaphoreLockWeighData, 0);
   Semaphore_Init_Fifo(g_SemaphoreLockReceiptData, 0);
   g_SemaphoreLockItemDataNotified = true;
   g_SemaphoreLockItemDataDeletedNotified = true;
   g_SemaphoreLockWeighDataNotified = true;
   g_SemaphoreLockReceiptDataNotified = true;
   createTask();
}

//////////////////////////////////////////////////////////////////////////////////////
ConnManager::~ConnManager()
{
   m_pConnDevice = NULL;
   Semaphore_Delete(g_SemaphoreLockItemData);
   Semaphore_Delete(g_SemaphoreLockItemDataDeleted);
   Semaphore_Delete(g_SemaphoreLockWeighData);
   Semaphore_Delete(g_SemaphoreLockReceiptData);
   deleteTask();
}

//////////////////////////////////////////////////////////////////////////////////////
ConnManager& ConnManager::getInstance()
{
   static ConnManager instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnManager::init()
{
   //Detect what is the device is activated
   //if( ConnDeviceWifi::getInstance().init() )
   //{
   //   m_pConnDevice = (ConnDevice*)(&ConnDeviceWifi::getInstance());
   //}

   if( ConnDevice3G::getInstance().init() )
   {
      m_pConnDevice = (ConnDevice*)(&ConnDevice3G::getInstance());
   }
   else
   {
      m_pConnDevice = NULL;
   }

   return (bool)(m_pConnDevice != NULL) ;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnManager::deinit()
{
   bool ret = false;

   if( m_pConnDevice != NULL )
   {
      ret = m_pConnDevice->deinit();
      m_pConnDevice = NULL;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnManager::isDataEmpty()
{
   return (m_itemDataQueues.empty() &&
           m_itemDataDeletedQueues.empty() &&
           m_weighDataQueues.empty() &&
           m_emailDataQueues.empty() &&
           m_smsDataQueues.empty());
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnManager::isDataSending()
{
   return ConnManager::m_isConnectionMainTaskRuning;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnManager::isDeviceReady()
{
   if( m_pConnDevice != NULL )
   {
      return m_pConnDevice->isReady();
   }
   else
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnManager::isDeviceInitialized()
{
   if( m_pConnDevice != NULL )
   {
      return m_pConnDevice->isInitialized();
   }
   else
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnManager::initDevice()
{
   bool ret = false;
   if( m_pConnDevice != NULL )
   {
      ret = m_pConnDevice->init();
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnManager::deinitDevice()
{
   if( m_pConnDevice != NULL )
   {
      return m_pConnDevice->deinit();
   }
   else
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::updateSystemDateTime()
{
   if( _S_.getBaseTime() > SYSTEM_TIME_BASE_INIT )
   {
      return;
   }

   if( m_pConnDevice != NULL )
   {
      time_t timeStampInSec;
      m_pConnDevice->reqDateTime(timeStampInSec);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::updateRateTable(const std::string rateTableId)
{
   if( m_pConnDevice != NULL )
   {
      RateManager::getInstance().removePendingRateTableId(rateTableId);
      if( m_pConnDevice->reqRateTable(rateTableId, REQ_RATETABLE_UPDATE_NEW) )
      {
         m_pConnDevice->reqRateTable(rateTableId, REQ_RATETABLE_UPDATE_OK);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::submitItemData()
{
   if( m_pConnDevice != NULL && !m_itemDataQueues.empty() )
   {
      g_SemaphoreLockItemDataNotified = false;
      ScaleItemData itemData_i = m_itemDataQueues.front();
      itemData_i.numTryingToSend++;
      g_SemaphoreLockItemDataNotified = true;
      Semaphore_Signal(g_SemaphoreLockItemData);

      if( m_pConnDevice->submitData(itemData_i, false) )
      {
         g_SemaphoreLockItemDataNotified = false;
         m_itemDataQueues.pop();
         g_SemaphoreLockItemDataNotified = true;
         Semaphore_Signal(g_SemaphoreLockItemData);
      }
      else if( itemData_i.numTryingToSend > MAX_NUM_TRYING_TO_SEND )
      {
         g_SemaphoreLockItemDataNotified = false;
         itemData_i.numTryingToSend = 0;
         m_itemDataQueues.push(itemData_i);
         m_itemDataQueues.pop();
         g_SemaphoreLockItemDataNotified = true;
         Semaphore_Signal(g_SemaphoreLockItemData);
      }

      Task_Delay(ST_GetClocksPerSecond()/10);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::submitItemDataDeleted()
{
   if( m_pConnDevice != NULL && !m_itemDataDeletedQueues.empty() )
   {
      g_SemaphoreLockItemDataDeletedNotified = false;
      ScaleItemData itemDataDeleted_i = m_itemDataDeletedQueues.front();
      g_SemaphoreLockItemDataDeletedNotified = true;
      Semaphore_Signal(g_SemaphoreLockItemDataDeleted);

      if( m_pConnDevice->submitData(itemDataDeleted_i, true) )
      {
         g_SemaphoreLockItemDataDeletedNotified = false;
         m_itemDataDeletedQueues.pop();
         g_SemaphoreLockItemDataDeletedNotified = true;
         Semaphore_Signal(g_SemaphoreLockItemDataDeleted);
      }

      Task_Delay(ST_GetClocksPerSecond()/5);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::submitWeighData()
{
   if( m_pConnDevice != NULL && !m_weighDataQueues.empty() )
   {
      g_SemaphoreLockWeighDataNotified = false;
      ScaleWeightData weighData_i = m_weighDataQueues.front();
      weighData_i.numTryingToSend++;
      g_SemaphoreLockWeighDataNotified = true;
      Semaphore_Signal(g_SemaphoreLockWeighData);

      _DTraceDebug("submitWeighData...");

      if( m_pConnDevice->submitData(weighData_i) )
      {
         g_SemaphoreLockWeighDataNotified = false;
         m_weighDataQueues.pop();
         g_SemaphoreLockWeighDataNotified = true;
         Semaphore_Signal(g_SemaphoreLockWeighData);
      }
      else if( weighData_i.numTryingToSend > MAX_NUM_TRYING_TO_SEND )
      {
         g_SemaphoreLockWeighDataNotified = false;
         weighData_i = m_weighDataQueues.front();
         weighData_i.numTryingToSend = 0;
         m_weighDataQueues.push(weighData_i);
         m_weighDataQueues.pop();
         g_SemaphoreLockWeighDataNotified = true;
         Semaphore_Signal(g_SemaphoreLockWeighData);
      }

      Task_Delay(ST_GetClocksPerSecond()/10);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::submitEmailData()
{
   if( m_pConnDevice != NULL && !m_emailDataQueues.empty() )
   {
      g_SemaphoreLockReceiptDataNotified = false;
      std::string emailData_i = m_emailDataQueues.front();
      g_SemaphoreLockReceiptDataNotified = true;
      Semaphore_Signal(g_SemaphoreLockReceiptData);

      if( m_pConnDevice->submitEmail(emailData_i) )
      {
         g_SemaphoreLockReceiptDataNotified = false;
         m_emailDataQueues.pop();
         g_SemaphoreLockReceiptDataNotified = true;
         Semaphore_Signal(g_SemaphoreLockReceiptData);
      }

      Task_Delay(ST_GetClocksPerSecond()/5);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::submitSmsData()
{
   if( m_pConnDevice != NULL && !m_smsDataQueues.empty() )
   {
      g_SemaphoreLockReceiptDataNotified = false;
      std::string smsData_i = m_smsDataQueues.front();
      g_SemaphoreLockReceiptDataNotified = true;
      Semaphore_Signal(g_SemaphoreLockReceiptData);

      if( m_pConnDevice->submitSMS(smsData_i) )
      {
         g_SemaphoreLockReceiptDataNotified = false;
         m_smsDataQueues.pop();
         g_SemaphoreLockReceiptDataNotified = true;
         Semaphore_Signal(g_SemaphoreLockReceiptData);
      }

      Task_Delay(ST_GetClocksPerSecond()/5);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::deleteData(ScaleItemData& itemData)
{
   if( _U_.getSubmitData() == SETTING_OFF )
   {
      _DTraceDebug("SubmitData is SETTING_OFF");
      return;
   }

   if( !g_SemaphoreLockItemDataDeletedNotified )
   {
      Semaphore_Wait(g_SemaphoreLockItemDataDeleted);
   }

   if( m_itemDataDeletedQueues.size() > DATA_ITEM_QUEUE_MAX )
   {
      //don't care, remove the last one
      m_itemDataDeletedQueues.pop();
   }

   _DTraceDebug("Delete an item data");

   m_itemDataDeletedQueues.push(itemData);
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::hasData(ScaleItemData& itemData)
{
   if( _U_.getSubmitData() == SETTING_OFF )
   {
      _DTraceDebug("SubmitData is SETTING_OFF");
      return;
   }

   if( itemData.dataNet > 0.001 )
   {
      if( !g_SemaphoreLockItemDataNotified )
      {
         Semaphore_Wait(g_SemaphoreLockItemData);
      }

      if( m_itemDataQueues.size() > DATA_ITEM_QUEUE_MAX )
      {
         //don't care, remove the last one
         m_itemDataQueues.pop();
      }

      _DTraceDebug("Has an item data");

      m_itemDataQueues.push(itemData);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::hasData(ScaleWeightData& itemData)
{
   if( _U_.getSubmitData() == SETTING_OFF )
   {
      _DTraceDebug("SubmitData is SETTING_OFF");
      return;
   }

   if( itemData.data1 > 0.001 )
   {
      if( !g_SemaphoreLockWeighDataNotified )
      {
         Semaphore_Wait(g_SemaphoreLockWeighData);
      }

      if( m_weighDataQueues.size() > DATA_ITEM_QUEUE_MAX )
      {
         //don't care, remove the last one
         m_weighDataQueues.pop();
      }

      _DTraceDebug("Has a weight data");

      //itemData.timestamp = _S_.getCurrentTimeSecond();
      m_weighDataQueues.push(itemData);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::hasData(std::list<ScaleItemData>& receiptData)
{
   std::string sTemp;
   if( !receiptData.empty() )
   {
      //Just for demo and test only
      //they should be upgraded by server
      //and will be implemented in the real project
      std::string emailContent("Day la email tu dong tu Vissan-Test, vui long khong reply. \r\n\r\n");
      emailContent.append("CUA HANG THUC PHAM VISSAN - 25 TO HIEN THANH \r\n");
      //GStringUtils::format(sTemp, "Longtitude: %.6f   Latitude: %.6f \r\n", m_connLocation.longtitude, m_connLocation.latitude);
      //emailContent.append(sTemp);
      //std::string localTime;
      //getSystemTime(localTime);
      //GStringUtils::format(sTemp, "DateTime: %s", localTime.c_str());
      emailContent.append(sTemp);
      emailContent.append("-------------------------------- \r\n");
      emailContent.append("HH   S.Luong   D.Gia   T.Tien \r\n");
      emailContent.append("-------------------------------- \r\n");

      std::string smsContent("SMS tu dong tu Vissan-Test, vui long khong reply \r\n");
      smsContent.append("VISSAN - 25 TO HIEN THANH \r\n");
      smsContent.append("----------- \r\n");
      smsContent.append("HH   S.Luong   D.Gia   T.Tien \r\n");

      for( std::list<ScaleItemData>::iterator it = receiptData.begin(); it != receiptData.end(); ++it )
      {
         if( (*it).type == SIT_ITEM )
         {
            //email
            GStringUtils::format(sTemp, "%s   %.2f %s   %.2f   %.2f \r\n", (*it).typeName.c_str(), (*it).dataNet, ScaleManager::getInstance().unit2Str((*it).unit).c_str(), (*it).unitPrice, (*it).totalPrice);
            emailContent.append(sTemp);
            //sms
            smsContent.append(sTemp);
         }
         else if( (*it).type == SIT_TOTAL )
         {
            emailContent.append("-------------------------------- \r\n");
            smsContent.append("----------- \r\n");
            GStringUtils::format(sTemp, "Tong mat hang: %s \r\n", (*it).typeName.c_str());
            emailContent.append(sTemp);
            smsContent.append(sTemp);
            GStringUtils::format(sTemp, "Tong tien: %.2f \r\n", (*it).totalPrice);
            emailContent.append(sTemp);
            smsContent.append(sTemp);
         }
      }

      emailContent.append("-------------------------------- \r\n");
      //emailContent.append(MAIL_BODY_FOOTER);
      emailContent.append("\r\n");
      emailContent.append("Du lieu duoc cap nhat tai: http://gbshd.com.vn/scalelog.php?result \r\n");
      smsContent.append("----------- \r\n");
      //smsContent.append(SMS_BODY_FOOTER);

      if( !g_SemaphoreLockReceiptDataNotified )
      {
         Semaphore_Wait(g_SemaphoreLockReceiptData);
      }

      if( _U_.getSubmitEmail() == SETTING_ON )
      {
         if( m_emailDataQueues.size() > DATA_ITEM_QUEUE_MAX )
         {
            //don't care, remove the last one
            m_emailDataQueues.pop();
         }
         m_emailDataQueues.push(emailContent);
      }
      else
      {
          _DTraceDebug("SubmitEmail is SETTING_OFF");
      }

      if( _U_.getSubmitSms() == SETTING_ON )
      {
         if( m_smsDataQueues.size() > DATA_ITEM_QUEUE_MAX )
         {
            //don't care, remove the last one
            m_smsDataQueues.pop();
         }
         m_smsDataQueues.push(smsContent);
      }
      else
      {
         _DTraceDebug("SubmitSMS is SETTING_OFF");
      }

      _DTraceDebug("Has a receipt data");
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::setDeviceStage(ConnDeviceStage stage)
{
   if( m_pConnDevice != NULL )
   {
      m_pConnDevice->setStage(stage);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void ConnManager::processClientRequest()
{
   //process for wifi only
   if( m_pConnDevice == NULL || m_pConnDevice->getConnDeviceType() != CDT_WIFI )
   {
      return;
   }

   std::string buffer = "";
   int connId = -1;
   if( m_pConnDevice->scanDataRequest(buffer) )
   {
      if( Webserver::getInstance().setRequest(buffer) )
      {
         _DTraceDebug("Webserver is responding...");
         Webserver::getInstance().getResponse(buffer, connId);
         ConnDeviceWifi::getInstance().respDataRequest(connId, buffer);
         Task_Delay(ST_GetClocksPerSecond()/2);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool ConnManager::isClientConnecting()
{
   //process for wifi only
   if( m_pConnDevice == NULL || m_pConnDevice->getConnDeviceType() != CDT_WIFI )
   {
      return false;
   }

   return Webserver::getInstance().isClientConnecting();
}
//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
