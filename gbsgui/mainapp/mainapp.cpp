//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: mainapp.c                                                                  //
//////////////////////////////////////////////////////////////////////////////////////

#include "stapp_main.h"
#include "mainapp.hpp"
#include "GDebug.hpp"
#include "GKeyManager.hpp"
#include "APIPower.hpp"
#include "time.h"

#include "SaveDataToFlash.hpp"
#include "ScaleManager.hpp"
#include "PrinterManager.hpp"
#include "ConnManager.hpp"
#include "BarcodeReader.hpp"
#include "RateManager.hpp"

char* GBSGUI_RevisionString = "GBSGUI 1.0.447";

//namespace GBS {
//namespace STM {
//namespace Application {
//////////////////////////////////////////////////////////////////////////////////////

ST_ErrorCode_t STGUI_Splash_Init(void)
{
   return ST_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t STGUI_Init(void)
{
   ST_ErrorCode_t Error = ST_NO_ERROR;
   Error = STGUI_Run();
   return Error;
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t STGUI_Run_As_VissanApp(void);
ST_ErrorCode_t STGUI_Run_As_VNPostApp(void);
ST_ErrorCode_t STGUI_Run(void)
{
   ST_ErrorCode_t Error = ST_NO_ERROR;

   FP_SetStandbyLED(TRUE);

   _DTraceDebug("============ Electronic Scale Starts ============\r\n");

   //Init app
   _DTraceDebug("Init app...\r\n");
   _S_.init();
   _S_.setTimeZoneIndex(1);
   _Sa_.init();
   _Sa_.unserialize(SERIALIZABLE_PARTITION_1);
   _Sa_.unserialize(SERIALIZABLE_PARTITION_2);
   _Sa_.unserialize(SERIALIZABLE_PARTITION_3);
   _Sa_.unserialize(SERIALIZABLE_PARTITION_4);

   //Error = STGUI_Run_As_VissanApp();
   Error = STGUI_Run_As_VNPostApp();

   FP_SetStandbyLED(TRUE);

   _DTraceDebug("============ Electronic Scale Stops ============\r\n");
   return Error;
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t STGUI_Run_As_VissanApp(void)
{
   _DTraceDebug("============ Electronic Scale Runs As Vissan Application ============\r\n");

   RateManager::getInstance().setLastActiveRateTable();
   ScaleManager::getInstance().init();
   PrinterManager::getInstance().init();
   ConnManager::getInstance().init();

   while(0x55)
   {
      Task_Delay(ST_GetClocksPerSecond()/10);
      std::string productCode = "";
      if( BCT_DATA == BarcodeReader::getInstance().scanReader(productCode) )
      {
         ScaleManager::getInstance().setPriceLookup(productCode);
         Task_Delay(ST_GetClocksPerSecond()/10);
      }

      Task_Delay(ST_GetClocksPerSecond()/10);
      ScaleDataType foundScaleData = ScaleManager::getInstance().scanWeightTx();

      ScaleWeightData lastWeighData;
      ScaleItemData lastItemData;
      ScaleItemData itemDataDeleted;
      std::list<ScaleItemData> listItemDatas;

      switch( foundScaleData )
      {
      case SDT_FORMAT1:
         ScaleManager::getInstance().getWeightData(lastWeighData);
         ConnManager::getInstance().hasData(lastWeighData);
         PrinterManager::getInstance().hasData(lastWeighData);
         break;

      case SDT_FORMAT2:
         ScaleManager::getInstance().getLastItemData(lastItemData);
         ConnManager::getInstance().hasData(lastItemData);
         PrinterManager::getInstance().hasData(lastItemData);
         break;

      case SDT_FORMAT2_MC:
         ScaleManager::getInstance().getItemDatas(listItemDatas);
         ConnManager::getInstance().hasData(listItemDatas);
         PrinterManager::getInstance().hasData(listItemDatas);
         ScaleManager::getInstance().resetItemDatas();
         break;

      case SDT_FORMAT2_DEL:
         ScaleManager::getInstance().getItemDataDeleted(itemDataDeleted);
         ConnManager::getInstance().deleteData(itemDataDeleted);
         break;

      default:
         Task_Delay(ST_GetClocksPerSecond()/10);
         if( RateManager::getInstance().hasActiveRateTableReached() )
         {
            _DTraceDebug("hasActiveRateTableReached at Main...");
            U32 newActiveRateTableId = RateManager::getInstance().lookForActiveRateTable();
            if( (newActiveRateTableId > 0) && (newActiveRateTableId != _U_.getActiveRateTableId()) )
            {
               ScaleManager::getInstance().updateRateTable();
               _U_.setActiveRateTableId(newActiveRateTableId);
               _Sa_.serialize(SERIALIZABLE_PARTITION_1);
               Task_Delay(ST_GetClocksPerSecond()/2);
            }
         }
         break;
      }
   }

   return ST_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t STGUI_Run_As_VNPostApp(void)
{
   _DTraceDebug("============ Electronic Scale Runs As VNPost Application ============\r\n");

   GKeyManager::getInstance().startKeyManagerTask();
   ScaleManager::getInstance().init();
   ConnManager::getInstance().init();

   FP_NotifyBuzzer(3);
   FP_SetStandbyLED(FALSE);

   ScaleWeightData lastWeighData;
   ScaleItemData lastItemData;
   ScaleItemData itemDataDeleted;
   std::list<ScaleItemData> listItemDatas;
   ScaleDataType lastScaleDataFound;

   Task_Delay(ST_GetClocksPerSecond());
   ConnManager::getInstance().start();

   while(0x55)
   {
      Task_Delay(ST_GetClocksPerSecond()/10);

      ScaleDataType foundScaleData = ScaleManager::getInstance().scanWeightTx();

      switch( foundScaleData )
      {
      case SDT_FORMAT1:
      {
         ScaleWeightData tempWeightData;
         ScaleManager::getInstance().getWeightData(tempWeightData);
         if( tempWeightData.data1 > 0 )
         {
            lastScaleDataFound = SDT_FORMAT1;
            ScaleManager::getInstance().getWeightData(lastWeighData);
         }
         break;
      }

      case SDT_FORMAT2:
      {
         ScaleItemData tempItemData;
         ScaleManager::getInstance().getLastItemData(tempItemData);
         if( tempItemData.dataNet > 0 )
         {
            lastScaleDataFound = SDT_FORMAT2;
            ScaleManager::getInstance().getLastItemData(lastItemData);
            ScaleManager::getInstance().resetItemDatas();
         }
          break;
      }

      case SDT_FORMAT2_MC:
        lastScaleDataFound = SDT_FORMAT2_MC;
         ScaleManager::getInstance().resetItemDatas();
         break;

      case SDT_FORMAT2_DEL:
        lastScaleDataFound = SDT_FORMAT2_DEL;
        ScaleManager::getInstance().resetItemDatas();
         break;

      default:
         break;
      }

      Task_Delay(ST_GetClocksPerSecond()/10);
      std::string productCode = "";
      if( BCT_DATA == BarcodeReader::getInstance().scanReader(productCode) )
      {
          /////////////////////////////////////////////////////////////////////////////////////
          //Todo: If productCode is greater than 200 chars,
          // it leads a crash at ConnDevice3G::buildHttpRequest,
          // so we trim it here. Investigate later
          if( productCode.length() > 192 )
          {
             productCode = productCode.substr(0, 192);
             _DTraceDebug("!!! Content is longer than 201 characters: %s", productCode.c_str());
          }
          /////////////////////////////////////////////////////////////////////////////////////

          switch( lastScaleDataFound )
          {
          case SDT_FORMAT1:
            lastWeighData.scaleCode = productCode;
            _DTraceDebug("SDT_FORMAT1: %.2f", lastWeighData.data1);
            ConnManager::getInstance().hasData(lastWeighData);
            break;

          case SDT_FORMAT2:
            lastItemData.scaleCode = productCode;
            _DTraceDebug("SDT_FORMAT2: %.2f", lastItemData.dataNet);
            ConnManager::getInstance().hasData(lastItemData);
            break;

          default:
             break;
          }
         Task_Delay(ST_GetClocksPerSecond()/10);
      }
   }

   return ST_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t STGUI_Term(void)
{
   _DTraceDebug("\nIn mainapp.c: STGUI_Term()");
   ST_ErrorCode_t Error = ST_NO_ERROR;

   return Error;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Application
//} //STM
//} //GBS
