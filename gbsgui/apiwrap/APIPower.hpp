//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: APIPower.hpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef APIPOWER_HPP_
#define APIPOWER_HPP_

#include "stapp_main.h"
#include "stpower.h"
#include "GError.hpp"
#include "GDebug.hpp"
#include "GSystem.hpp"
#include "GKeyManager.hpp"
#include "GUtility.hpp"
#include "frontpanel.hpp"

// List of power devices
// ---------------------
// /!\ Do not disturb order of members of below enum, it is done in initilisation order
// The device list has been inflated to include all the software devices for selective Init/Term.
// This is required for H205 SoC family boards where the entry to hibernation is different from 7108

#if defined(POWER_MAX_NUMBER)
typedef enum POWERi_DeviceList_e
{
   POWER_DEVICE_LMIVID = 0x0,
   POWER_DEVICE_POWER,
   POWER_DEVICE_SYS,
   POWER_DEVICE_PIO,
   POWER_DEVICE_UART,
   POWER_DEVICE_TBX,
   POWER_DEVICE_TRACE,
   POWER_DEVICE_EVT,
   POWER_DEVICE_I2C,
   POWER_DEVICE_SPI,
   POWER_DEVICE_PWM,
   POWER_DEVICE_FLASH,
   POWER_DEVICE_PCPD,
   POWER_DEVICE_SCART,
#if !defined(REMOTE_TUNER_SUPPORT)
   POWER_DEVICE_TUNER,
#endif
   POWER_DEVICE_DMA,
   POWER_DEVICE_DEMUX,
   POWER_DEVICE_TSMERGE,
   POWER_DEVICE_INJECT,
   POWER_DEVICE_CLKRV,
   POWER_DEVICE_DENC,
   POWER_DEVICE_VTG,
   POWER_DEVICE_VOUT,
   POWER_DEVICE_LAYER,
   POWER_DEVICE_VBI,
   POWER_DEVICE_BLIT,
   POWER_DEVICE_GRAFIX,
   POWER_DEVICE_VMIX,
   POWER_DEVICE_VIDEO,
   POWER_DEVICE_AUDIO,
   POWER_DEVICE_BLAST,
   POWER_DEVICE_USB,
   POWER_DEVICE_GAMLOAD,
   POWER_DEVICE_CRYPT,
   POWER_DEVICE_ATAPI,
   POWER_DEVICE_SMART,
   POWER_DEVICE_HDMI,
   POWER_DEVICE_KEYSCN,
   POWER_DEVICE_VIN,
   POWER_DEVICE_TTX,
   POWER_DEVICE_CC,
   POWER_DEVICE_POD,
   POWER_DEVICE_MOCA,
   POWER_DEVICE_PCCRD,
   POWER_DEVICE_GFX2D,
   POWER_DEVICE_TCPIP,
   POWER_DEVICE_NET,
   POWER_DEVICE_SUBT,
   POWER_DEVICE_GAME,
   POWER_DEVICE_RECORD,
   POWER_DEVICE_NETWORK,
   POWER_DEVICE_END,
}
POWERi_DeviceList_t;
#endif

typedef enum POWERi_SystemPowerState
{
   POWER_PROFILE_ACTIVE = 0x0,
   POWER_PROFILE_STANDBY,
   POWER_PROFILE_GAMING,
   POWER_PROFILE_IPRECORD,
   POWER_PROFILE_DVBRECORD,
   POWER_PROFILE_HIBERNATION,
   POWER_PROFILE_MAX,
}
POWERi_SystemPowerState_t;

// Maintain the profile ID and profile name relation
typedef struct POWERi_ProfileInfo
{
   POWERi_SystemPowerState_t ProfileID;       // Profile ID corresponding to device states
   U8                        ProfileName[32]; // This will store the name of the profile
}
POWERi_ProfileInfo_t;

// Maintain the device power state based on profiles
typedef struct POWERi_DeviceStates
{
   POWERi_DeviceList_t DeviceType;                     // The device type i.e LMIVID etc.
   STPOWER_State_t     DeviceState[POWER_PROFILE_MAX]; // The device is either active, or in active standby...
   ST_ErrorCode_t      (*DeviceInit)(void);            // Device Init function
   ST_ErrorCode_t      (*DeviceTerm)(void);            // Device Term function
}
POWERi_DeviceStates_t;

// Define maximum number of devices
#if defined(POWER_MAX_NUMBER)
#define POWER_DEVICE_MAX 150
#endif

// Macro definitions
#if defined(POWER_MAX_NUMBER)
#define POWERi_TraceERR(x) TRACE_Print x
#endif

// Local definition of wake-up parameter
typedef struct POWERi_WakeupParam_s
{
   S32  WAKEUP_IR_PARAMETER;
   S32  WAKEUP_HDMI_PARAMETER;
   S32  WAKEUP_TIMER_PARAMETER;
   S32  WAKEUP_FRONTPANEL_PARAMETER;
   char WAKEUP_ETHERNET_PARAMETER[128];
}
POWERi_WakeupParam_t;

// Local definition of cpu info
typedef struct POWERi_CPUInfo_s
{
   U8  CPU_ID;
   U32 CPU_Nominal_Frequency;
   U8  CPU_Freq_Divisor;
}
POWERi_CPUInfo_t;

// Local definition of profile data type
#if defined(POWER_MAX_NUMBER)
typedef struct POWERi_ProfileData_s
{
   U32                    NumOfDevices;
   STPOWER_ProfileEntry_t ProfileEntry[POWER_DEVICE_MAX];
}
POWERi_ProfileData_t;
#endif

// Local definition of power context
#if defined(POWER_MAX_NUMBER)
typedef struct POWERi_Context_s
{
   STEVT_Handle_t       EVT_Handle;                     // Event handler used when we are in reduced clock mode
   U32                  WAKEUP_REASON;                  // Describe the wake-up reason
   POWERi_WakeupParam_t WAKEUP_PARAM;                   // Wake-up parameter
   Semaphore_t          WAKEUP_SIGNAL;                  // Signal to notify the wake-up
   POWERi_CPUInfo_t     CPU_Info[CPU_MAX_NUMBER];       // CPU Frequency Configuration
   //STPOWER_State_t    CurrentPowerState;              // Current profile power state
   U32                  CurrentPowerState;              // Current profile power state
   STPOWER_CPUState_t   CurrentCPUState;                // Current cpu state
   POWERi_ProfileData_t ProfileData[POWER_PROFILE_MAX]; // List of device state for each profile
   U32                  ProfileID[POWER_PROFILE_MAX];   // Unique identifier for each profile
   BOOL                 WakeupEventRecvd;
}
POWERi_Context_t;
#endif

// Local variables
#if defined(POWER_MAX_NUMBER)
static POWERi_Context_t POWERi_Context[POWER_MAX_NUMBER];
#endif

typedef enum GPowerState
{
   GPS_ACTIVE = 0x0,
   GPS_STANDBY_APP,
   GPS_STANDBY_SYS,
   GPS_WAKEUP
}
GPowerState_t;

static void onHdmiHotPlugCallback(unsigned int hdmiState);
static void eventCallback(STEVT_CallReason_t Reason,const ST_DeviceName_t RegistrantName,STEVT_EventConstant_t Event,const void *EventData,const void *SubscriberData_p);
static BOOL g_IsPowerWakeUp;

//////////////////////////////////////////////////////////////////////////////////////
class CPower
{
private:
   CPower();
   ~CPower();

   ST_ErrorCode_t initState();
   ST_ErrorCode_t eventConfigure(U32 PowerId,BOOL Enable);
   ST_ErrorCode_t profilePopulate(U32 PowerId,U32 ProfileId,U32 DeviceId,STPOWER_State_t POWER_State);
   ST_ErrorCode_t profileChangeState(U32 PowerId,U32 ProfileId);
   void           wakeUpHandlerPIO (STPIO_Handle_t PIO_Handle,STPIO_BitMask_t PIO_BitMask);
   void           displayClock(DayTime_dtv_t dayTime);
   BOOL           setMode(STPOWER_CPUState_t CPU_State, U32 WakeUpTimer, S32 PROFILE_ID);
   void           standbyApp(U32 durationInMS, BOOL& isGoingToSystemStandby, BOOL isRequestTune);

   GPowerState_t m_gPowerState;
   STPOWER_CPUState_t m_cpuState;
   U32 m_wakeUpDuration;
   S32 m_profileID;

public:
   static CPower& getInstance();
   void           enterStandby(STPOWER_CPUState_t CPU_State, U32 WakeUpTimer, S32 PROFILE_ID);
   void           enterWakeUp();
   void           updateWakeUpDuration(U32 wakeUpDuration);
   void           setState(GPowerState_t gPowerState);
   GPowerState_t  getState();
   void           resetSTVOUT();
};

//////////////////////////////////////////////////////////////////////////////////////
#endif // APIPOWER_HPP_
