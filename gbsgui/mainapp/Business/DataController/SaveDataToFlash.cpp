//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: SaveDataToFlash.cpp                                                        //
//////////////////////////////////////////////////////////////////////////////////////

#include "SaveDataToFlash.hpp"
#include "APISetting.hpp"
#include "GStringUtils.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace DataController {

//////////////////////////////////////////////////////////////////////////////////////
SaveDataToFlash& SaveDataToFlash::getInstance()
{
   static SaveDataToFlash instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
SaveDataToFlash::SaveDataToFlash()
{
   m_SemaphoreFlashReadWriteNotified = true;
   Semaphore_Init_Fifo(m_SemaphoreFlashReadWrite, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
SaveDataToFlash::~SaveDataToFlash()
{
   Semaphore_Delete(m_SemaphoreFlashReadWrite);
}

//////////////////////////////////////////////////////////////////////////////////////
bool SaveDataToFlash::init()
{
   return GFileSystem::getInstance().init();
}

//////////////////////////////////////////////////////////////////////////////////////
void SaveDataToFlash::resetAllToFactory()
{

}

//////////////////////////////////////////////////////////////////////////////////////
void SaveDataToFlash::deletePartition(SerializableObject_t SerializableObject)
{

}

//////////////////////////////////////////////////////////////////////////////////////
bool SaveDataToFlash::serialize(SerializableObject_t SerializableObject)
{
   bool canContinue = false;
   std::string dataSerializing = "";

   _DTraceDebug("...Start");

   if( !m_SemaphoreFlashReadWriteNotified )
   {
      Semaphore_Wait(m_SemaphoreFlashReadWrite);
   }
   m_SemaphoreFlashReadWriteNotified = false;

   switch( SerializableObject )
   {
   case SERIALIZABLE_PARTITION_1:
      GUserSettings::getInstance().serializeData(dataSerializing);
      canContinue = true;
      break;
   case SERIALIZABLE_PARTITION_2:
      RateManager::getInstance().serializeData(ESCALE_RATE_TABLE_1, dataSerializing);
      canContinue = true;
      break;
   case SERIALIZABLE_PARTITION_3:
      RateManager::getInstance().serializeData(ESCALE_RATE_TABLE_2, dataSerializing);
      canContinue = true;
      break;
   case SERIALIZABLE_PARTITION_4:
      RateManager::getInstance().serializeData(ESCALE_RATE_TABLE_3, dataSerializing);
      canContinue = true;
      break;
   default:
      break;
   }

   if( canContinue )
   {
      //_DTraceDebug("Partition #%d, data: %s", SerializableObject, dataSerializing.c_str());
      canContinue = GFileSystem::getInstance().writeSavePartition(SerializableObject, dataSerializing);
      Task_Delay(ST_GetClocksPerSecond()/2);
   }

   Semaphore_Signal(m_SemaphoreFlashReadWrite);
   m_SemaphoreFlashReadWriteNotified = true;

   _DTraceDebug("...Done");

   return canContinue;
}

//////////////////////////////////////////////////////////////////////////////////////
bool SaveDataToFlash::unserialize(SerializableObject_t SerializableObject)
{
   bool canContinue = false;
   U8* pUnserializeData = NULL;
   U32 dataSize = 0;

   _DTraceDebug("...Start");

   if( !m_SemaphoreFlashReadWriteNotified )
   {
      Semaphore_Wait(m_SemaphoreFlashReadWrite);
   }
   m_SemaphoreFlashReadWriteNotified = false;

   switch( SerializableObject )
   {
   case SERIALIZABLE_PARTITION_1:
   case SERIALIZABLE_PARTITION_2:
   case SERIALIZABLE_PARTITION_3:
   case SERIALIZABLE_PARTITION_4:
      canContinue = true;
      break;

   default:
      break;
   }

   if( canContinue )
   {
      canContinue = GFileSystem::getInstance().readPartitionFromFlash(SerializableObject, pUnserializeData, dataSize);
      Task_Delay(ST_GetClocksPerSecond()/5);
   }

   if( canContinue && pUnserializeData != NULL )
   {
      _DTraceDebug("Partition #%d, data: %s", SerializableObject, (char*)pUnserializeData);

      switch( SerializableObject )
      {
      case SERIALIZABLE_PARTITION_1:
         canContinue = GUserSettings::getInstance().unserializeData(pUnserializeData);
         break;
      case SERIALIZABLE_PARTITION_2:
         //canContinue = RateManager::getInstance().unserializeData(ESCALE_RATE_TABLE_1, pUnserializeData);
         break;
      case SERIALIZABLE_PARTITION_3:
         //canContinue = RateManager::getInstance().unserializeData(ESCALE_RATE_TABLE_2, pUnserializeData);
         break;
      case SERIALIZABLE_PARTITION_4:
         //canContinue = RateManager::getInstance().unserializeData(ESCALE_RATE_TABLE_3, pUnserializeData);
         break;

      default:
         break;
      }
   }

   if( pUnserializeData != NULL )
   {
      free(pUnserializeData);
      pUnserializeData = NULL;
   }

   Semaphore_Signal(m_SemaphoreFlashReadWrite);
   m_SemaphoreFlashReadWriteNotified = true;

   _DTraceDebug("...Done!");

   return canContinue;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //DataController
//} //Business
//} //Application
//} //STM
//} //GBS
