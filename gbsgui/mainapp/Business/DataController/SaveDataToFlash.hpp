//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: SaveDataToFlash.hpp                                                        //
//             Noi serialize cac phan UserSetting, MultiSetting de save xuong Flash //
//             doc tu Flash va set lai cac form                                     //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef SAVEDATATOFLASH_HPP_
#define SAVEDATATOFLASH_HPP_

#include "GObject.hpp"
#include "GError.hpp"
#include "GSystem.hpp"
#include "GFileSystem.hpp"
#include "GUserSettings.hpp"
#include "RateManager.hpp"

#define _Sa_ SaveDataToFlash::getInstance()

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace DataController {

typedef enum
{
   FSS_NONE,
   FSS_DELETING,
   FSS_SAVING
}
FlashSavingState_t;

typedef struct FlashStageInfo_s
{
   SerializableObject_t SerializableObject;
   FlashSavingState_t FlashSavingState;
}
FlashStageInfo;


//////////////////////////////////////////////////////////////////////////////////////
class SaveDataToFlash : public virtual GObject
{
private:
   SaveDataToFlash();
   ~SaveDataToFlash();

   Semaphore_t m_SemaphoreFlashReadWrite;
   bool m_SemaphoreFlashReadWriteNotified;

public:
   static SaveDataToFlash& getInstance();

   bool init();
   bool serialize(SerializableObject_t SerializableObject);
   bool unserialize(SerializableObject_t SerializableObject);

   void deletePartition(SerializableObject_t SerializableObject);
   void resetAllToFactory();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //DataController
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //SAVEDATATOFLASH_HPP_
