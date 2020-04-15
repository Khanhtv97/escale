//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GSystem.cpp                                                                //
//////////////////////////////////////////////////////////////////////////////////////

#include "GSystem.hpp"
#include "GDebug.hpp"
#include <malloc.h>  // for mallinfo

#if defined(ST_5197)
#define WTCSR 0XFFC0000C
#define WTCNT 0XFFC00008
#define WTCSR2 0XFFC0001C
#define WD_ENABLE   0x80       // bit 7 - TME
#define WD_NOT_ITVL 0x40       // bit 6 - WT/TI
#define RST_MAN_NOT_PWRON 0x20 // bit 5 - RSTS
#endif

#if defined(ST_H205)
#define ST40_WDG_WTCNT    0xFFC00008
#define ST40_WDG_WTCSR    0xFFC0000C
#define ST40_WDG_WTCSR2   0xFFC0001C
#define STOP_VALUE_CMD    0xA545
#define START_VALUE_CMD   0xA5C5
#endif

U32 GSystem::m_diffSecondBaseTime = 0;
time_t GSystem::m_baseTime = SYSTEM_TIME_BASE_INIT;

Task_t WatchDogTaskId = NULL;

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
GSystem::GSystem()
{
   m_timeZoneIndex = 0;
   m_isNeedReboot = FALSE;
   // Init time zone
}

//////////////////////////////////////////////////////////////////////////////////////
GSystem::~GSystem()
{
}

//////////////////////////////////////////////////////////////////////////////////////
GSystem& GSystem::getInstance()
{
   static GSystem instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
void GSystem::init()
{
   _DTraceError("start.");

   initTimeZone();
   std::vector<TimeZone_t> _vTimeZone = getVectorTimeZone();
   TimeZone_t _timeZone;
   memset(&_timeZone, 0, sizeof(TimeZone_t));

   if( m_timeZoneIndex > _vTimeZone.size() )
   {
      _timeZone = _vTimeZone[0];
   }
   else
   {
      _timeZone = _vTimeZone[m_timeZoneIndex];
   }

   m_timeZone = _timeZone.time;
   _DTraceError("%02d:%02d ",m_timeZone.hour,m_timeZone.minute);

   //initTimer(); //27-11-2014 trungkt move to GDVBPlay
   //initWatchDog();
   _DTraceError("end");
}

//////////////////////////////////////////////////////////////////////////////////////
void GSystem::initWatchDog()
{

#if defined(ST_5197)

   // Thuc added init watch dog timer
   STSYS_WriteRegDev16LE(WTCSR, 0xA500 + 0x00); // stop WDT
   Task_Delay(ST_GetClocksPerSecond() / 10);
   // set CKS3-0 to 1101 so that WTCNT will be overflowed in around 8s
   // set CKS bit 3 to 1
   STSYS_WriteRegDev16LE(WTCSR2, 0xAA00 + 0x01);
   Task_Delay(ST_GetClocksPerSecond() / 10);
   // set CKS bit 2-0 to 101
   STSYS_WriteRegDev16LE(WTCSR, 0XA500 + (WD_NOT_ITVL) + 0x05);
   Task_Delay(ST_GetClocksPerSecond() / 10);
   // reset counter
   STSYS_WriteRegDev16LE(WTCNT, 0X5A00+0x00);
   Task_Delay(ST_GetClocksPerSecond() / 10);
   // enable WDT
   STSYS_WriteRegDev16LE(WTCSR, 0XA500 + (WD_NOT_ITVL|WD_ENABLE|RST_MAN_NOT_PWRON) + 0x05);
   Task_Delay(ST_GetClocksPerSecond() / 10);

#endif

   WDT_ST40();
    U32 ParamId = 0;
   WatchDogTaskId = Task_Create(GSystem::WatchDogTask, (void*)ParamId, 4096, MIN_USER_PRIORITY, "WatchDogTask", 0, WatchDogTaskId);

   if( WatchDogTaskId == (Task_t)NULL)
   {
      _DTraceError("Cannot create WatchDog Task");
   }
   else
   {
      _DTraceDebug("WatchDog Task created");
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GSystem::WatchDogTask(void* Parameter)
{
   while(1)
   {
      //U8 count = STSYS_ReadRegDev8(WTCNT);
      //_D_.Debug("WatchDogTask: WTCNT = 0x%x, WTCSR2 value = 0x%x, "
      //"WTCSR value = 0x%x\n", count, STSYS_ReadRegDev8(WTCSR2), STSYS_ReadRegDev8(WTCSR));
      // reset counter

#if defined(ST_5197)
      STSYS_WriteRegDev16LE(WTCNT,0X5A00+0x00);
#endif

#if defined(ST_H205)
      SYS_WriteRegDev16LE((U32)ST40_WDG_WTCNT, (U16)0x5A00);
#endif

      Task_Delay(ST_GetClocksPerSecond()/2);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GSystem::WDT_ST40(void)
{
   volatile unsigned int st40_mask, reset_gen;

#if 0

    st40_mask= *(int*)(0xFD541188);
    st40_mask &=0xFFFFFFF7;
    *(int*)(0xFD541188) = st40_mask;

#else
    // SYSCFG_BANK3 base address: 0xFD541000
    // SYSTEM_CONFIG498 : ST40 CPU mask register
    st40_mask = STSYS_ReadRegDev16LE(0xFD541188);
    STSYS_WriteRegDev16LE(0xFD541188, st40_mask & 0xFFFFFFF7);

    // SYSTEM_CONFIG458 : Reset generator: warm or cold reset control.
    // cold boot required > 0: cold   1: warm
    reset_gen = STSYS_ReadRegDev16LE(0xFD541000 + 0x000000E8);
    STSYS_WriteRegDev16LE(0xFD541000+0x000000E8, reset_gen & 0xFFFFFFEF);

#endif

    //#define ST40_WDG_WTCNT    0xFFC00008
    //#define ST40_WDG_WTCSR    0xFFC0000C
    //#define ST40_WDG_WTCSR2   0xFFC0001C
    //#define STOP_VALUE_CMD    0xA545
    //#define START_VALUE_CMD   0xA5C5
    // Stop counting
    SYS_WriteRegDev16LE((U32)ST40_WDG_WTCSR, (U16)0xA500 | STOP_VALUE_CMD);
    // Reinit watchdog value: FF = reset immediately;
    // other value like 0x80/0x20 will wait for several seconds
    SYS_WriteRegDev16LE((U32)ST40_WDG_WTCNT, (U16)0x5A00 | 0x00ff);
    // choose the watchdog timer period
    SYS_WriteRegDev16LE((U32)ST40_WDG_WTCSR2, (U16)0xAA00 | 0x0001);  // TIMER 0x000B is bigger
    // Start counting
    SYS_WriteRegDev16LE((U32)ST40_WDG_WTCSR, (U16)START_VALUE_CMD);

}

//////////////////////////////////////////////////////////////////////////////////////
time_t GSystem::getCurrentTimeSecond()
{
   U32 second = STOS_TimeNow()/STOS_GetClocksPerSecond() - m_diffSecondBaseTime + m_baseTime;
   return (time_t)second;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GSystem::getCurrentTime(DayTime_dtv_t& currentTime)
{
   GError_t ErrorCode = GERROR_NONE;

   // Get STOS_clock time now
   U32 second = (U32)getCurrentTimeSecond();

   // Calulate daytime-dtv from second
   ErrorCode = Second2DayTime(second, &currentTime);

   return ErrorCode;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GSystem::getCurrentTimeByTimeZone(DayTime_dtv_t& currentTime)
{
   GError_t ErrorCode = GERROR_NONE;

   DayTime_dtv_t timeWithoutTimeZone;
   ErrorCode = getCurrentTime(timeWithoutTimeZone);

   if( GERROR_NONE == ErrorCode )
   {
      TimeZone_dtv_t currentTimeZone = getTimeZone();
      ErrorCode = ChangeDayTimeZone(timeWithoutTimeZone, &currentTimeZone, &currentTime);
   }

   return ErrorCode;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GSystem::now()
{
   U64 CurrentTimeInMs = 0;

   CurrentTimeInMs=((U64)Time_Now() * (U64)1000) / ((U64)ST_GetClocksPerSecond());

   // ko duoc goi Debug trong ham nay, neu ko se bi de quy ho tuong vo han
   return (U32)CurrentTimeInMs;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GSystem::cpuload()
{
   return SYS_GetCPULoad(0);
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GSystem::taskinfo()
{
#if defined(ST_OS21)

   task_t*      Task;
   task_status_t Task_Status;
   U32           Task_Number;
   U32          stack_size =0, stack_used = 0;
   // Display debug info
   _D_.Info("TASK LIST :\n");
   _D_.Info("+----+---------------------------+-------+-------+------------+------+\n");
   _D_.Info("| Id | Task name                 | Size  | Used  | Base       | Prio |\n");
   _D_.Info("+----+---------------------------+-------+-------+------------+------+\n");

   // Get the task list entry point
   Task_Number = 0;
   Task        = NULL;
   Task        = task_list_next(NULL);
   Task        = task_list_next(Task);

   // For each task
   while( Task != NULL )
   {
      // Display task infos
      Task_Number++;
      if( task_status(Task, &Task_Status, task_status_flags_stack_used) == 0 )
      {
         _D_.Info("| %02d | %-25s | %5d | %5d | 0x%08x | %03d  |\n",Task_Number,task_name(Task),Task_Status.task_stack_size,Task_Status.task_stack_used,Task_Status.task_stack_base,task_priority(Task));
      }
      stack_size += Task_Status.task_stack_size;
      stack_used += Task_Status.task_stack_used;
      // Switch to next task
      Task = task_list_next(Task);
   }

   _D_.Info("+----+---------------------------+-------+-------+------------+------+\n");
   _D_.Info("| Total (byte)                   |%5d| %5d |------------|------|\n",stack_size,stack_used);
   _D_.Info("+----+---------------------------+-------+-------+------------+------+\n");
   return Task_Number;

#endif

   return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GSystem::meminfo(int FullInfo)
{
   U32 i;
   char memType[10] = "";
   char memState[10] = "";

    // Get system partition

#if defined(ST_OS21)
   {
      partition_status_t Partition_Status;
      if( partition_status((ST_Partition_t*)SYS_Memory_GetPartition(SYS_MEMORY_CACHED|0),&Partition_Status,0) != 0 )
      {
         _D_.Info("SYSTEM PARTITION: unable to get info\n");
      }
      else
      {
         switch( Partition_Status.partition_status_type )
         {
         case partition_status_type_simple:
            strcpy(memType, "'Simple'");
            break;

         case partition_status_type_fixed:
            strcpy(memType, "'Fixed'");
            break;

         case partition_status_type_heap:
            strcpy(memType, "'Heap'");
            break;

         case partition_status_type_user:
            strcpy(memType, "'User'");
            break;

         default:
            strcpy(memType, "\'????\'");
            break;
         }

         switch( Partition_Status.partition_status_state)
         {
         case partition_status_state_invalid:
            strcpy(memState, "'Invalid'");
            break;

         case partition_status_state_valid:
            strcpy(memState, "'Valid'");
            break;

         default:
            strcpy(memState, "\'????\'");
            break;
         }

         _D_.Info("SYSTEM PARTITION (%s, %s), free: 0x%08x/0x%08x (%.2fMB/%.2fMB), largest free block:  0x%08x (%.2fMB)\n", memType, memState, Partition_Status.partition_status_free, Partition_Status.partition_status_size, (double)Partition_Status.partition_status_free/1024/1024, (double)Partition_Status.partition_status_size/1024/1024, Partition_Status.partition_status_free_largest, (double)Partition_Status.partition_status_free_largest/1024/1024);
         //print("Size  = 0x%08x\n",Partition_Status.partition_status_size);
         //print("Used  = 0x%08x\n",Partition_Status.partition_status_used);
         //print("Free  = 0x%08x - Max block = 0x%08x\n",Partition_Status.partition_status_free,Partition_Status.partition_status_free_largest);
      }
   }

#endif

    // Get ncache partition

#if defined(ST_OS21)

   {
      partition_status_t Partition_Status;

      if( partition_status((ST_Partition_t *)SYS_Memory_GetPartition(SYS_MEMORY_NCACHED|0),&Partition_Status,0) != 0 )
      {
         _D_.Info("NCACHE PARTITION: unable to get info\n");
      }
      else
      {
         switch( Partition_Status.partition_status_type )
         {
         case partition_status_type_simple:
            strcpy(memType, "'Simple'");
            break;

         case partition_status_type_fixed:
            strcpy(memType, "'Fixed'");
            break;

         case partition_status_type_heap:
            strcpy(memType, "'Heap'");
            break;

         case partition_status_type_user:
            strcpy(memType, "'User'");
            break;

         default:
            strcpy(memType, "\'????\'");
            break;
         }

         switch( Partition_Status.partition_status_state )
         {
         case partition_status_state_invalid:
            strcpy(memState, "'Invalid'");
            break;

         case partition_status_state_valid:
            strcpy(memState, "'Valid'");
            break;

         default:
            strcpy(memState, "\'????\'");
            break;
         }

         _D_.Info("NCACHE PARTITION (%s, %s), free: 0x%08x/0x%08x (%.2fMB/%.2fMB), largest free block:  0x%08x (%.2fMB)\n", memType, memState, Partition_Status.partition_status_free, Partition_Status.partition_status_size, (double)Partition_Status.partition_status_free/1024/1024, (double)Partition_Status.partition_status_size/1024/1024, Partition_Status.partition_status_free_largest, (double)Partition_Status.partition_status_free_largest/1024/1024);
      }
   }

#endif

   // Get heap partition

#if defined(VIRTUAL_PLATFORM_TLM)
#if defined(ST_OS21)

   {
      extern void*  esw_heap;
      extern void*  esw_heapsize;
      extern void** esw_stack;
      unsigned int __malloc_sbrk_base     = (unsigned int)esw_heap;
      unsigned int __malloc_max_total_mem = (unsigned int)esw_heapsize;

#if defined(PROCESSOR_SH4)

      unsigned int stackpos = 0;
      unsigned int stack = (unsigned int)esw_stack[0];

      __asm__ __volatile__ ("\tmov %%esp,%0\n" : "=g" (stackpos));

#endif

      _D_.Info("HEAP PARTITION: Base = 0x%08x (%.2fMB), Size = 0x%08x (%.2fMB)\n", __malloc_sbrk_base, (double)__malloc_sbrk_base/1024/1024, __malloc_max_total_mem, (double)__malloc_max_total_mem/1024/1024);

#if defined(PROCESSOR_SH4)

      _D_.Info("HEAP PARTITION: Stack Ptr = 0x%08x (%.2fMB), Stack Size = 0x%08x (%.2fMB)\n", (U32)stackpos, ((double)(U32)stackpos)/1024/1024, (U32)&stack-(U32)stackpos, ((double)((U32)&stack-(U32)stackpos))/1024/1024);
#endif

   }

#endif

#else

#if defined(ST_OS21)
   {
      extern unsigned int __malloc_sbrk_base;
      extern unsigned int __malloc_max_total_mem;
      struct mallinfo current_heap_status=mallinfo();

#if defined(PROCESSOR_SH4)

      extern unsigned int stack;
      unsigned int stackpos=0;
      __asm__ __volatile__ ("\tmov r15,%0\n" : "=r" (stackpos));

#endif

      _D_.Info("HEAP PARTITION: Base = 0x%08x (%.2fMB), Free = 0x%08x/0x%08x (%.2fMB/%.2fMB)\n", __malloc_sbrk_base, (double)__malloc_sbrk_base/1024/1024, __malloc_max_total_mem-current_heap_status.arena, __malloc_max_total_mem, (double)(__malloc_max_total_mem-current_heap_status.arena)/1024/1024, (double)__malloc_max_total_mem/1024/1024);

#if defined(PROCESSOR_SH4)

      _D_.Info("HEAP PARTITION: Stack Ptr = 0x%08x (%.2fMB), Stack Size = 0x%08x (%.2fMB)\n", (U32)stackpos, ((double)(U32)stackpos)/1024/1024, (U32)&stack-(U32)stackpos, ((double)((U32)&stack-(U32)stackpos))/1024/1024);

#endif

#if defined(PROCESSOR_ST231)

      _D_.Info("\n");

#endif

   }

#endif
#endif

   // Display AVMEM allocations
   if( FullInfo != 0 )
   {
      _D_.Info("AVMEM PARTITION\n");
      BOOL  AVMEM_BlockPresent = FALSE;

      for( i = 0; i < AVMEM_MAX_NUMBER; i++ )
      {
         STAVMEM_BlockInfo_t AVMEM_BlockInfo;
         U32                 AVMEM_NextBlock = 0;
         ST_ErrorCode_t      ErrCode = ST_NO_ERROR;
         ErrCode = STAVMEM_GetBlockInfo(AVMEM_PartitionHandle[i], &AVMEM_BlockInfo ,&AVMEM_NextBlock);

         if( ErrCode != ST_NO_ERROR )
         {
            _D_.Info("Unable to get avmem block information in avmem partition %d \n", i);
            return 0;
         }

         while( AVMEM_NextBlock != 0 )
         {
            if( (AVMEM_BlockInfo.AllocMode==STAVMEM_ALLOC_MODE_TOP_BOTTOM)||(AVMEM_BlockInfo.AllocMode==STAVMEM_ALLOC_MODE_BOTTOM_TOP))
            {
               if( AVMEM_BlockPresent == FALSE )
               {
                  _D_.Info("+------------+------------+-----------+---------------+------------+-----------+-------------------+------------+---------------------+\n");
                  _D_.Info("| Handle (P) | Handle (B) | AllocMode | Start Address | Size       | Alignment | Alignment Start @ | User Size  | Allocator task name |\n");
                  _D_.Info("+------------+------------+-----------+---------------+------------+-----------+-------------------+------------+---------------------+\n");
                  AVMEM_BlockPresent=TRUE;
               }
                _D_.Info("| 0x%08x | 0x%08x | %-9s | 0x%08x    | 0x%08x | %04d      | 0x%08x        | 0x%08x | %-16s    |\n",AVMEM_BlockInfo.PartitionHandle,AVMEM_BlockInfo.BlockHandle,(AVMEM_BlockInfo.AllocMode==STAVMEM_ALLOC_MODE_TOP_BOTTOM)?"TOP":"BOT",AVMEM_BlockInfo.StartAddress,AVMEM_BlockInfo.Size,AVMEM_BlockInfo.Alignment,AVMEM_BlockInfo.AlignedStartAddress,AVMEM_BlockInfo.UserSize,AVMEM_BlockInfo.AllocatorName);
            }

            ErrCode = STAVMEM_GetBlockInfo(AVMEM_PartitionHandle[i], &AVMEM_BlockInfo, &AVMEM_NextBlock);
            if( ErrCode != ST_NO_ERROR )
            {
               _D_.Info("Unable to get avmem block information in avmem partition %d \n", i);
               return(TRUE);
            }
         }
      }

      if( AVMEM_BlockPresent == TRUE )
      {
         _D_.Info("+------------+------------+-----------+---------------+------------+-----------+-------------------+------------+---------------------+\n");
      }
      else
      {
         _D_.Info("--> Nothing is allocated in the AVMEM partitions\n");
      }
   }

   for( i = 0; i < AVMEM_MAX_NUMBER; i++ )
   {
      U32 AVMEM_FreeSize;
      ST_ErrorCode_t ErrCode = ST_NO_ERROR;
      ErrCode = STAVMEM_GetPartitionFreeSize(AVMEM_PartitionHandle[i], &AVMEM_FreeSize);

      if( ErrCode == ST_NO_ERROR )
      {
         _D_.Info("AVMEM PARTITION: handle(P) = 0x%08x (%.2fMB), free: 0x%08x (%.2fMB)\n", AVMEM_PartitionHandle[i], (double)AVMEM_PartitionHandle[i]/1024/1024, AVMEM_FreeSize, (double)AVMEM_FreeSize/1024/1024);
      }
   }

   return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
void GSystem::reboot(U16 secondToDelay)
{
   _D_.Info("Rebooting...");

   //new method, causes crash to reboot by watch dog
   *((unsigned int*)0) = 0xDEAD;

   //old method, not work since enable watch dog
   //Task_Delay(ST_GetClocksPerSecond()/10);
   //SYS_WriteRegDev16LE(0xfe001124, 0x0A8C);
   //Task_Delay(ST_GetClocksPerSecond()/10);
   //SYS_WriteRegDev16LE(0xffc0000c, 0x0547);
   //Task_Delay(ST_GetClocksPerSecond()/10);
   //
   ////5a01 mean reset right away
   //SYS_WriteRegDev16LE(0xffc00008, 0x5A01 + secondToDelay);
   //Task_Delay(ST_GetClocksPerSecond()/10);
   //SYS_WriteRegDev16LE(0xffc0000c, 0xA5C7);
   //Task_Delay(ST_GetClocksPerSecond()/10);
}

//////////////////////////////////////////////////////////////////////////////////////
TimeZone_dtv_t GSystem::getTimeZone()
{
   return m_timeZone;
}

//////////////////////////////////////////////////////////////////////////////////////
void GSystem::setTimeZoneIndex(int timeIndex)
{
   m_timeZoneIndex = timeIndex;
   std::vector<TimeZone_t> _vTimeZone = getVectorTimeZone();
   TimeZone_t _timeZone;
   memset(&_timeZone, 0, sizeof(TimeZone_t));

   if( m_timeZoneIndex > _vTimeZone.size() )
   {
      _timeZone = _vTimeZone[0];
   }
   else
   {
      _timeZone = _vTimeZone[m_timeZoneIndex];
   }

   m_timeZone = _timeZone.time;
}

//////////////////////////////////////////////////////////////////////////////////////
void GSystem::setDiffSecondBaseTime(U32 _diffSecond)
{
   m_diffSecondBaseTime = _diffSecond;
}

//////////////////////////////////////////////////////////////////////////////////////
void GSystem::setBaseTime(time_t baseTime)
{
   m_baseTime = baseTime;
}

//////////////////////////////////////////////////////////////////////////////////////
time_t GSystem::getBaseTime()
{
   return m_baseTime;
}

//////////////////////////////////////////////////////////////////////////////////////
void GSystem::setReboot()
{
   m_isNeedReboot = TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GSystem::isNeedReboot()
{
   return m_isNeedReboot;
}
//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
