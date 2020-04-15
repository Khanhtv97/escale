//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: APIPower.cpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#include "APIPower.hpp"
#include "GEventManager.hpp"

// Define the Profile data
#if defined(POWER_MAX_NUMBER)

// Store the Profile name corresponding to Profile ID
static POWERi_ProfileInfo_t POWERi_ProfileInfo[POWER_PROFILE_MAX]=
{
   {POWER_PROFILE_ACTIVE,      "Active Profile"},
   {POWER_PROFILE_STANDBY,     "Active Standby Profile"},
   {POWER_PROFILE_GAMING,      "Gaming Mode Profile"},
   {POWER_PROFILE_IPRECORD,    "IP Record Profile"},
   {POWER_PROFILE_DVBRECORD,   "Broadcast Record Profile"},
   {POWER_PROFILE_HIBERNATION, "Hibernation Profile"},
};

// Store the DeviceType along with it's state in different power modes
static POWERi_DeviceStates_t POWERi_DeviceStates[POWER_DEVICE_END]=
{
   //- Device Name -//   //- Active mode -//       //- Active Standby -//   //- Gaming Mode -//      //- IP record -//        //- Broadcast record -// //- Hibernation -//       //- Init -//  //- Term -//
   {POWER_DEVICE_LMIVID ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, NULL          , NULL          },
   {POWER_DEVICE_POWER  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, NULL          , NULL          },
   {POWER_DEVICE_SYS    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, SYS_Init      , SYS_Term      }, //SYS_Init     , SYS_Term
   {POWER_DEVICE_PIO    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, PIO_Init      , PIO_Term      }, //PIO_Init     , PIO_Term
   {POWER_DEVICE_UART   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, UART_Init     , UART_Term     }, //UART_Init    , UART_Term
   {POWER_DEVICE_TBX    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, TBX_Init      , TBX_Term      }, //TBX_Init     , TBX_Term
   {POWER_DEVICE_TRACE  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, NULL          , NULL          },
   {POWER_DEVICE_EVT    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, EVT_Init      , EVT_Term      },
   {POWER_DEVICE_I2C    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, I2C_Init      , I2C_Term      }, //I2C_Init      , I2C_Term
   {POWER_DEVICE_SPI    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, SPI_Init      , SPI_Term      }, //SPI_Init      , SPI_Term
   {POWER_DEVICE_PWM    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, PWM_Init      , PWM_Term      },
   {POWER_DEVICE_FLASH  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, FLASH_Init    , FLASH_Term    }, //FLASH_Init    , FLASH_Term
   {POWER_DEVICE_PCPD   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, PCPD_Init     , PCPD_Term     },
   {POWER_DEVICE_SCART  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, SCART_Init    , SCART_Term    },
#if defined (MAGIC_PID_SUPPORT) //MAGIC_PID_SUPPORT
 #if !defined(REMOTE_TUNER_SUPPORT)
   {POWER_DEVICE_TUNER  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, /*TUNER_Init*/NULL , /*TUNER_Term*/NULL},
 #endif
#else
 #if !defined(REMOTE_TUNER_SUPPORT)
   {POWER_DEVICE_TUNER  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, TUNER_Init      , TUNER_Term    },
 #endif
#endif
   {POWER_DEVICE_DMA    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, DMA_Init      , DMA_Term      }, //DMA_Init      , DMA_Term
   {POWER_DEVICE_DEMUX  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, DEMUX_Init    , DEMUX_Term    },
   {POWER_DEVICE_TSMERGE,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, NULL          , NULL          },
   {POWER_DEVICE_INJECT ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, INJECT_Init   , INJECT_Term   }, //INJECT_Init   , INJECT_Term
   {POWER_DEVICE_CLKRV  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, CLKRV_Init    , CLKRV_Term    }, //CLKRV_Init    , CLKRV_Term
   {POWER_DEVICE_DENC   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, DENC_Init     , DENC_Term     }, //DENC_Init     , DENC_Term
   {POWER_DEVICE_VTG    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, VTG_Init      , VTG_Term      }, //VTG_Init      , VTG_Term
   {POWER_DEVICE_VOUT   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, VOUT_Init     , VOUT_Term     },
   {POWER_DEVICE_LAYER  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, LAYER_Init    , LAYER_Term    },
   {POWER_DEVICE_VBI    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, VBI_Init      , VBI_Term      }, //VBI_Init      , VBI_Term
   {POWER_DEVICE_BLIT   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, BLIT_Init     , BLIT_Term     }, //BLIT_Init     , BLIT_Term
   {POWER_DEVICE_GRAFIX ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, GRAFIX_Init   , GRAFIX_Term   },
   {POWER_DEVICE_VMIX   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, VMIX_Init     , VMIX_Term     },
   {POWER_DEVICE_VIDEO  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, VID_Init      , VID_Term      },
   {POWER_DEVICE_AUDIO  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, AUD_Init      , AUD_Term      },
   {POWER_DEVICE_BLAST  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0}, BLAST_Init    , BLAST_Term    },
   {POWER_DEVICE_USB    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, USB_Init      , USB_Term      }, //USB_Init      , USB_Term
   {POWER_DEVICE_GAMLOAD,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0}, GAMLOAD_Init  , GAMLOAD_Term  },
   {POWER_DEVICE_CRYPT  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, CRYPT_Init    , CRYPT_Term    }, //CRYPT_Init    , CRYPT_Term
   {POWER_DEVICE_ATAPI  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, NULL          , NULL          }, //ATAPI_Init    , ATAPI_Term
   {POWER_DEVICE_SMART  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, SMART_Init    , SMART_Term    }, //SMART_Init    , SMART_Term
   {POWER_DEVICE_HDMI   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, HDMI_Init     , HDMI_Term     },
   {POWER_DEVICE_KEYSCN ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, KEYSCN_Init   , KEYSCN_Term   }, //KEYSCN_Init   , KEYSCN_Term
   {POWER_DEVICE_VIN    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, VIN_Init      , VIN_Term      },
   {POWER_DEVICE_TTX    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0}, TTX_Init      , TTX_Term      }, //TTX_Init      , TTX_Term
   {POWER_DEVICE_CC     ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0}, CC_Init       , CC_Term       }, //CC_Init       , CC_Term
   {POWER_DEVICE_POD    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, POD_Init      , POD_Term      }, //POD_Init      , POD_Term
   {POWER_DEVICE_MOCA   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, MOCA_Init     , MOCA_Term     }, //MOCA_Init     , MOCA_Term
   {POWER_DEVICE_PCCRD  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, PCCRD_Init    , PCCRD_Term    }, //PCCRD_Init    , PCCRD_Term
   {POWER_DEVICE_GFX2D  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, GRAFIX_DFBInit, GRAFIX_DFBTerm},
   {POWER_DEVICE_TCPIP  ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, NULL          , NULL          },
   {POWER_DEVICE_NET    ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, NULL          , NULL          },
   {POWER_DEVICE_SUBT   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, NULL          , NULL          },
   {POWER_DEVICE_GAME   ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, NULL          , NULL          },
   {POWER_DEVICE_RECORD ,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D1, STPOWER_DEVICE_STATE_D2}, NULL          , NULL          },
   {POWER_DEVICE_NETWORK,{STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D0, STPOWER_DEVICE_STATE_D2}, NULL          , NULL          },
};
#endif

//////////////////////////////////////////////////////////////////////////////////////
static void onHdmiHotPlugCallback(unsigned int hdmiState)
{
   _DTraceError(" HDMI VOUT_State = %d\n", hdmiState);
   CPower::getInstance().resetSTVOUT();
}

//////////////////////////////////////////////////////////////////////////////////////
static void eventCallback(STEVT_CallReason_t Reason,
                          const ST_DeviceName_t RegistrantName,
                          STEVT_EventConstant_t Event,
                          const void* EventData,
                          const void* SubscriberData_p)
{
   // Analysis of the event received

   switch(Event)
   {
      // Remote control events

#if defined(BLAST_MAX_NUMBER)

      case STBLAST_READ_DONE_EVT :
      {
         ST_ErrorCode_t ErrCode=ST_NO_ERROR;
         STBLAST_EvtReadDoneParams_t BLAST_EvtData = *((STBLAST_EvtReadDoneParams_t*)EventData);
         if( BLAST_EvtData.Result != ST_ERROR_TIMEOUT )
         {
            U32 keyCode, i;
            for( i = 0; i < POWER_MAX_NUMBER; i++ )
            {
               if( ((POWERi_Context[i].WAKEUP_REASON&STPOWER_WAKEUP_IR)==STPOWER_WAKEUP_IR)&&(POWERi_Context[i].CurrentCPUState==STPOWER_CPU_STATE_NORMAL) )
               {
                  keyCode = GKeyManager::getInstance().scanKeyPressed();

                  if( TRUE == GKeyManager::getInstance().isKeyWakeup(keyCode) )
                  {
                     Semaphore_Signal(POWERi_Context[i].WAKEUP_SIGNAL);
                     FP_SetStandbyLED(FALSE);
                     g_IsPowerWakeUp = TRUE;
                     break;
                  }
               }
            }
         }
      }
      break;

#endif

      // Unknown events
      default:
      {
         //_DTraceError("--> Illegal event received to wake-up the system, even 0x%08x is ignored !\n", Event);
      }
   }
}

const U32 k_AppStandbyDuration  = 45000; // 0.75 min
const U32 k_minMSecBeforeWakeup = 90000; // 1.5  minutes

//////////////////////////////////////////////////////////////////////////////////////
CPower::CPower()
{
   g_IsPowerWakeUp = FALSE;
   m_gPowerState = GPS_ACTIVE;
   m_cpuState = STPOWER_CPU_STATE_NORMAL;
   m_wakeUpDuration = 0;
   m_profileID = POWER_PROFILE_ACTIVE;

   initState();
   HDMI_SetHotplugCallback(onHdmiHotPlugCallback);
}

//////////////////////////////////////////////////////////////////////////////////////
CPower::~CPower()
{

}

//////////////////////////////////////////////////////////////////////////////////////
CPower& CPower::getInstance()
{
   static CPower instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t CPower::initState()
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   U32 i, j, k;
   ST_ClockInfo_t ClockInfo;

   ST_GetClockInfo(&ClockInfo);

   for( i = 0; i < POWER_MAX_NUMBER; i++ )
   {
      memset(&POWERi_Context[i], 0, sizeof(POWERi_Context_t));
      POWERi_Context[i].CurrentPowerState = 0;
      POWERi_Context[i].CurrentCPUState   = STPOWER_CPU_STATE_NORMAL;
      POWERi_Context[i].WakeupEventRecvd  = FALSE;
      POWERi_Context[i].WAKEUP_REASON     = 0;

      Semaphore_Init_Fifo(POWERi_Context[i].WAKEUP_SIGNAL, 0);

      for( j = 0; j < POWER_PROFILE_MAX; j++ )
      {
         POWERi_Context[i].ProfileID[j] = 0xFFFFFFFF;
      }

      for( j = 0; j < CPU_MAX_NUMBER; j++ )
      {
         POWERi_Context[i].CPU_Info[j].CPU_ID                = 0xFF;
         POWERi_Context[i].CPU_Info[j].CPU_Freq_Divisor      = 1;
#if defined(ST_5197)
         POWERi_Context[i].CPU_Info[j].CPU_Nominal_Frequency = ClockInfo.st40cpu_clk;
#else
         POWERi_Context[i].CPU_Info[j].CPU_Nominal_Frequency = ClockInfo.ckga.st40cpu_clk;
#endif
      }
   }

   // Register the profiles
   // Register profile 0 a active/normal state

   for( i = 0; i < POWER_MAX_NUMBER; i++ )
   {
      POWERi_Context[i].ProfileData[0].NumOfDevices = 0;

      for( j = 0; j < POWER_DEVICE_END; j++ )
      {
         ErrCode = profilePopulate(i, 0, POWERi_DeviceStates[j].DeviceType, POWERi_DeviceStates[j].DeviceState[0]);
         if( ErrCode != ST_NO_ERROR )
         {
            _DTraceError("(%d):**ERROR** !!! Error 0x%x populating active mode profile 0 !!!\n", i, ErrCode);
            return(ErrCode);
         }
      }

      // Register this new profile to STPOWER
      ErrCode = STPOWER_RegisterProfile(POWER_Handle[i], POWERi_Context[i].ProfileData[0].ProfileEntry, POWERi_Context[i].ProfileData[0].NumOfDevices,&POWERi_Context[i].ProfileID[0]);
      if( (ErrCode != ST_NO_ERROR) && (ErrCode != ST_ERROR_FEATURE_NOT_SUPPORTED) )
      {
         _DTraceError("(%d):**ERROR** !!! Error 0x%x registering active mode profile 0 !!!\n", i, ErrCode);
         return(ErrCode);
      }
   }

   // All other profiles are intended to reduce power consumption.
   // So, we will populate the device in reverse order
   for( i = 0; i < POWER_MAX_NUMBER; i++ )
   {
      for( j = 1; j < POWER_PROFILE_MAX; j++ )
      {
         POWERi_Context[i].ProfileData[j].NumOfDevices = 0;

         for( k = POWER_DEVICE_END; k > 0; k-- )
         {
            ErrCode = profilePopulate(i, j, POWERi_DeviceStates[k-1].DeviceType, POWERi_DeviceStates[k-1].DeviceState[j]);

            if( ErrCode != ST_NO_ERROR )
            {
               _DTraceError("(%d):**ERROR** !!! Error 0x%x populating mode profile %d !!!\n", i, ErrCode, j);
               return(ErrCode);
            }
         }

         // Register this new profile to STPOWER
         ErrCode = STPOWER_RegisterProfile(POWER_Handle[i], POWERi_Context[i].ProfileData[j].ProfileEntry, POWERi_Context[i].ProfileData[j].NumOfDevices,&POWERi_Context[i].ProfileID[j]);
         if( (ErrCode != ST_NO_ERROR) && (ErrCode != ST_ERROR_FEATURE_NOT_SUPPORTED) )
         {
            _DTraceError("(%d):**ERROR** !!! Error 0x%x registering mode profile %d !!!\n", i, ErrCode, j);
            return(ErrCode);
         }
      }
   }

   // Return no errors
   return(ST_NO_ERROR);
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CPower::setMode( STPOWER_CPUState_t CPU_State, U32 WakeUpTimer, S32 PROFILE_ID)
{
   ST_ErrorCode_t          ErrCode = ST_NO_ERROR;
   S16                     i = 0;
   S32                     ID, CPU_ID, CPU_FREQ_DIVISOR;
   U32                     WAKEUP_REASON, WAKEUP_IR_PARAMETER, WAKEUP_HDMI_PARAMETER, WAKEUP_TIMER_PARAMETER, WAKEUP_FRONTPANEL_PARAMETER, VALUE, WAKEUP_GPIO_PORT, WAKEUP_GPIO_PIN;
   char                    WAKEUP_ETHERNET_PARAMETER[128];
   STPOWER_WakeUpReason_t  POWER_Wakeup_Reason;
   STPOWER_Wakeup_Params_t POWER_Wakeup_Params;
   STPIO_OpenParams_t      PIO_OpenParams;
   STPIO_Compare_t         PIO_Compare;
   STPIO_Handle_t          PIO_Handle = 0x0;

   WAKEUP_GPIO_PORT = 3;
   WAKEUP_GPIO_PIN = 1;

   // dedault value as in stdebug / power.c
   ID = 0;
   CPU_ID = STTHIS_CPU;  // 0xFFFFFFFF;//
   CPU_FREQ_DIVISOR = 1; //0xFFFFFFFF;//32;//

   memset(&POWER_Wakeup_Params, 0, sizeof(STPOWER_Wakeup_Params_t));

   _DTraceError("powerSetMode %d \n", CPU_State);

   if( POWER_Handle[ID] == 0 )
   {
      _DTraceError("--> This power %d is undefined on this hardware !\n [0,1,..] !\n",ID);
      return(TRUE);
   }

   if( (CPU_State != 0xFFFFFFFF) && ((CPU_State < 0)||(CPU_State > STPOWER_CPU_STATE_STANDBY)) )
   {
      _DTraceError("--> Invalid cpu state, should be [PM_CPU_NORMAL,PM_CPU_STANDBY,PM_CPU_SLEEP] !\n");
      return(TRUE);
   }

#if defined(ST_5197)
   if( CPU_State == STPOWER_CPU_STATE_SLEEP )
   {
      _DTraceError("--> CPU Sleep mode is not supported for this device !\n");
      return(TRUE);
   }
#endif

   POWERi_Context[ID].CPU_Info[0].CPU_ID           = (U8)CPU_ID;
   POWERi_Context[ID].CPU_Info[0].CPU_Freq_Divisor = (U8)CPU_FREQ_DIVISOR;

   // Get list of wake-up reasons
   VALUE = WAKEUP_REASON = WAKEUP_IR_PARAMETER = WAKEUP_HDMI_PARAMETER = WAKEUP_TIMER_PARAMETER = WAKEUP_FRONTPANEL_PARAMETER = WAKEUP_GPIO_PORT = WAKEUP_GPIO_PIN = 0;
   WAKEUP_TIMER_PARAMETER = WakeUpTimer;
   WAKEUP_IR_PARAMETER = 0;

   if( WAKEUP_TIMER_PARAMETER != 0 )
   {
      WAKEUP_REASON = (STPOWER_WAKEUP_IR | STPOWER_WAKEUP_TIMER );
   }
   else
   {
      WAKEUP_REASON = (STPOWER_WAKEUP_IR);
   }

   // Configure wake up device
   if (CPU_State != STPOWER_CPU_STATE_NORMAL)
   {
      // Configuring Ethernet (Wake-on-LAN) as wakeup device
      if( (WAKEUP_REASON&STPOWER_WAKEUP_ETHERNET) == STPOWER_WAKEUP_ETHERNET )
      {
         strncpy(POWER_Wakeup_Params.Ethernet.PathOfDevice,WAKEUP_ETHERNET_PARAMETER,sizeof(POWER_Wakeup_Params.Ethernet.PathOfDevice)-1);

         ErrCode = STPOWER_SetWakeUpDevice(POWER_Handle[ID],(STPOWER_WakeUpReason_t)(WAKEUP_REASON&STPOWER_WAKEUP_ETHERNET),&POWER_Wakeup_Params);

         if( ErrCode != ST_NO_ERROR )
         {
            _DTraceError("--> Ethernet wake-up condition for power %d can't been set !\n",ID);
            return(TRUE);
         }
      }

      // Configuring Timer as wakeup device
      if( (WAKEUP_REASON&STPOWER_WAKEUP_TIMER) == STPOWER_WAKEUP_TIMER )
      {
         POWER_Wakeup_Params.Timer.TimeInSec = WAKEUP_TIMER_PARAMETER;
         ErrCode = STPOWER_SetWakeUpDevice(POWER_Handle[ID],(STPOWER_WakeUpReason_t) (WAKEUP_REASON&STPOWER_WAKEUP_TIMER),&POWER_Wakeup_Params);
         if( ErrCode != ST_NO_ERROR )
         {
            _DTraceError("--> Timer wake-up condition for power %d can't been set !\n",ID);
            return(TRUE);
         }
      }

      // Configuring HDMI-CEC as wakeup device
#if defined(HDMI_CEC_ON)
      if( (WAKEUP_REASON&STPOWER_WAKEUP_HDMI) == STPOWER_WAKEUP_HDMI )
      {
         ErrCode = STVOUT_CECWakeUpDevice(VOUT_Handle[2],TRUE);

         if( ErrCode != ST_NO_ERROR )
         {
            _DTraceError("--> Unable to configure HDMI-CEC as a wakeup device \n");
            return(TRUE);
         }
      }
#endif
   }

   // Set STAPI component to requested state
   ErrCode = profileChangeState(ID, PROFILE_ID);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to change power %d profile state %d !\n",ID,PROFILE_ID);

      ErrCode = profileChangeState(ID, POWER_PROFILE_ACTIVE);

      if( ErrCode != ST_NO_ERROR)
      {
         _DTraceError("--> Unable to restore power %d profile state, devices are in unknown states !\n",ID);
      }
      return(TRUE);
   }

   // Do we want to use reduced clock mode (meaning cpu is in normal state and profile is different from 0)
   if( (PROFILE_ID != POWER_PROFILE_ACTIVE)&&(CPU_State == STPOWER_CPU_STATE_NORMAL) )
   {
         POWERi_Context[ID].CurrentCPUState                          = STPOWER_CPU_STATE_NORMAL;
         POWERi_Context[ID].WAKEUP_REASON                            = WAKEUP_REASON;
         POWERi_Context[ID].WAKEUP_PARAM.WAKEUP_IR_PARAMETER         = WAKEUP_IR_PARAMETER;
         POWERi_Context[ID].WAKEUP_PARAM.WAKEUP_HDMI_PARAMETER       = WAKEUP_HDMI_PARAMETER;
         POWERi_Context[ID].WAKEUP_PARAM.WAKEUP_FRONTPANEL_PARAMETER = WAKEUP_FRONTPANEL_PARAMETER;
         POWERi_Context[ID].WAKEUP_PARAM.WAKEUP_TIMER_PARAMETER      = WAKEUP_TIMER_PARAMETER;
         strncpy(POWERi_Context[ID].WAKEUP_PARAM.WAKEUP_ETHERNET_PARAMETER,WAKEUP_ETHERNET_PARAMETER,strlen(WAKEUP_ETHERNET_PARAMETER));
         // Enable events on devices to be wake-up
         //_DTraceError("Enter STPOWER_CPU_STATE_NORMAL. Wait .......\n");
         g_IsPowerWakeUp = FALSE;

         ErrCode = eventConfigure(ID, TRUE);

         if( ErrCode != ST_NO_ERROR)
         {
            _DTraceError("--> Unable to configure the devices to wake-up in reduced clock mode, try to restore saved power state... !\n",ID);
            ErrCode = profileChangeState(ID, POWER_PROFILE_ACTIVE);
            if( ErrCode != ST_NO_ERROR )
            {
               _DTraceError("--> Unable to restore power %d profile state, devices are in unknown states !\n",ID);
            }
            return(TRUE);
         }

         DayTime_dtv_t dayTime, oldTime;
         U32 oldSecond;
         U32 keyCode;
         _S_.getCurrentTime(dayTime);
         _S_.getCurrentTime(oldTime);
         oldSecond = DayTime2Second(&oldTime);

         while( !g_IsPowerWakeUp )
         {
            keyCode = GKeyManager::getInstance().scanKeyPressed();

            if( GKeyManager::getInstance().isKeyWakeup(keyCode) )
            {
               char* displayLED = "  o n  ";
               _D_.PrintFP(displayLED);
               _D_.PrintFPLED();
               Semaphore_Signal(POWERi_Context[ID].WAKEUP_SIGNAL);
               FP_SetStandbyLED(FALSE);
               g_IsPowerWakeUp = TRUE;
            }
            else if( (POWERi_Context[ID].WAKEUP_REASON & STPOWER_WAKEUP_TIMER) == STPOWER_WAKEUP_TIMER )
            {
               if( DayTime2Second(&dayTime) >= oldSecond + WAKEUP_TIMER_PARAMETER)
               {
                  char* displayLED = "  o n  ";
                  _D_.PrintFP(displayLED);
                  _D_.PrintFPLED();
                  Semaphore_Signal(POWERi_Context[ID].WAKEUP_SIGNAL);
                  FP_SetStandbyLED(FALSE);
                  g_IsPowerWakeUp = TRUE;
               }
            }

            _S_.getCurrentTime(dayTime);
            displayClock(dayTime);
            Task_Delay(ST_GetClocksPerSecond()/10);
         }

         // Wait for a wake-up from reduced clock
         Semaphore_Wait(POWERi_Context[ID].WAKEUP_SIGNAL);
         // _DTraceError("CPU has been wake-up !\n");
         // Disable events on devices to be wake-up */

         ErrCode = eventConfigure(ID, FALSE);

         if( ErrCode != ST_NO_ERROR )
         {
            _DTraceError("--> Unable to remove the devices to be wake-up in reduced clock mode !\n",ID);
         }

         // Come back to normal state
         ErrCode = profileChangeState(ID, POWER_PROFILE_ACTIVE);

         if( ErrCode != ST_NO_ERROR)
         {
            _DTraceError("--> Unable to change power %d profile to normal state !\n",ID);
            return(TRUE);
         }

         _DTraceError("System entering normal mode \n");
   }

   // Otherwise, control CPU power if standby is ordered
   if( (PROFILE_ID != 0) && (CPU_State != 0xFFFFFFFF) && (CPU_State != STPOWER_CPU_STATE_NORMAL) )
   {
         // Enter CPU in the correct state requested
         // Remote control events
   #if defined(BLAST_MAX_NUMBER)
         if( (POWERi_Context[ID].WAKEUP_REASON & STPOWER_WAKEUP_IR) == STPOWER_WAKEUP_IR )
         {
            U32 Key;
            _DTraceError("Wait key power .... \n");
            // Purge the key already pressed
            while(BLAST_PollKey(POWERi_Context[ID].WAKEUP_PARAM.WAKEUP_IR_PARAMETER,&Key) == ST_NO_ERROR);;
         }
   #endif
         _DTraceError("System is entering in a power state mode...\n");
         Task_Delay(ST_GetClocksPerSecond()/10);
         POWERi_Context[ID].CurrentCPUState = CPU_State;
         //_DTraceError("call STPOWER_SetCPUState POWERi_Context[ID].CurrentCPUState = %d \n",POWERi_Context[ID].CurrentCPUState);
         ErrCode = STPOWER_SetCPUState(POWER_Handle[ID],(STPOWER_CPUState_t)CPU_State,&POWER_Wakeup_Reason);
         //_DTraceError("Power state 1 \n");

         if( ErrCode != ST_NO_ERROR )
         {
            _DTraceError("--> Unable to set new cpu state for power %d!\n",ID);

            ErrCode = profileChangeState(ID,0);

            if (ErrCode!=ST_NO_ERROR)
            {
               _DTraceError("--> Unable to change power %d profile to normal state !\n",ID);
               return(TRUE);
            }
            return(TRUE);
         }

         POWERi_Context[ID].CurrentCPUState = STPOWER_CPU_STATE_NORMAL;

         ErrCode = profileChangeState(ID, POWER_PROFILE_ACTIVE);

         if( ErrCode != ST_NO_ERROR)
         {
            _DTraceError("--> Unable to change power %d profile to normal state !\n",ID);
            return(TRUE);
         }

         // Check wake-up reason
         _DTraceError("CPU has been wake-up, reason is 0x%08x !\n", POWER_Wakeup_Reason);

         switch(POWER_Wakeup_Reason)
         {
         // Ir-blaster, look for power button
         case STPOWER_WAKEUP_IR :
            _DTraceError("Key press --> STPOWER_WAKEUP_IR\n");
#if defined(BLAST_MAX_NUMBER)
            {
               U32 KeyCode = 0;
               BLAST_GetKey(WAKEUP_IR_PARAMETER, &KeyCode, 0);

               if( KeyCode != BLAST_0_KEY_POWER )
               {
                  print("Wake-up reason seems to be remote control, wait for power button (on remote control) to be pressed ... !\n");

                  while( KeyCode!=BLAST_0_KEY_POWER )
                  {
                     BLAST_GetKey(WAKEUP_IR_PARAMETER,&KeyCode,0);
                  }
               }
            }
#endif
            _DTraceError("Cpu has been wake-up by remote control and power button pressed !\n");
            break;

         case STPOWER_WAKEUP_ETHERNET :
            _DTraceError("Cpu has been wake-up by ethernet !\n");
            break;
         case STPOWER_WAKEUP_TIMER :
            _DTraceError("Cpu has been wake-up by timer !\n");
            break;
         case STPOWER_WAKEUP_FRONTPANEL :
            _DTraceError("Cpu has been wake-up by frontpanel !\n");
            break;
         case STPOWER_WAKEUP_HDMI :
            _DTraceError("Cpu has been wake-up by hdmi !\n");
            break;
         case STPOWER_WAKEUP_MSG_REMOTE_CPU :
            _DTraceError("Cpu has been wake-up by remote CPU !\n");
            break;
         case STPOWER_WAKEUP_GPIO :
            _DTraceError("Cpu has been wake-up by GPIO !\n");
            if( PIO_Handle != 0 )
            {
               ErrCode = STPIO_Close(PIO_Handle);
               if (ErrCode!=ST_NO_ERROR)
               {
                  print("--> Unable to close pio which just wake up the system !\n");
                  return(TRUE);
               }
            }

            for( i = (POWERi_Context[ID].ProfileData[PROFILE_ID].NumOfDevices-1);i>=POWER_DEVICE_LMIVID;i--)
            {
               if( strcmp(POWERi_Context[ID].ProfileData[PROFILE_ID].ProfileEntry[i].DeviceName, PIO_DeviceName[WAKEUP_GPIO_PORT]) == 0)
               {
                  break;
               }
            }

            if( POWERi_Context[ID].ProfileData[PROFILE_ID].ProfileEntry[i].DeviceState == STPOWER_DEVICE_STATE_D0 )
            {
               POWERi_Context[ID].ProfileData[PROFILE_ID].ProfileEntry[i].DeviceState = STPOWER_DEVICE_STATE_D1;
               ErrCode = STPOWER_UnRegisterProfile(POWER_Handle[ID], POWERi_Context[ID].ProfileID[PROFILE_ID]);

               if( ErrCode != ST_NO_ERROR)
               {
                  print("--> Unable to unregister the modified power %d, profile %d !\n",ID,PROFILE_ID);
                  return(TRUE);
               }

               ErrCode = STPOWER_RegisterProfile(POWER_Handle[ID],POWERi_Context[ID].ProfileData[PROFILE_ID].ProfileEntry,POWERi_Context[ID].ProfileData[PROFILE_ID].NumOfDevices,&POWERi_Context[ID].ProfileID[PROFILE_ID]);
               if( ErrCode != ST_NO_ERROR )
               {
                  print("--> Unable to re-register the power %d, profile %d !\n",ID,PROFILE_ID);
                  return(TRUE);
               }
            }
            break;
         default :
            _DTraceError("Cpu has been wake-up by an unkwown reason !\n");
            break;
         } //End switch

         // Wake up done, set device component back to normal state (Power Profile 0)
         ErrCode = profileChangeState(ID, 0);

         if( ErrCode != ST_NO_ERROR )
         {
            _DTraceError("--> Unable to change power %d profile to normal state !\n",ID);
            return(TRUE);
         }

         _DTraceError("System entering normal mode \n");
   }

   // Return no errors
   return(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t CPower::profileChangeState(U32 PowerId, U32 ProfileId)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S8 i;

   // Configure the profile
   ErrCode = STPOWER_SetProfile(POWER_Handle[PowerId], POWERi_Context[PowerId].ProfileID[ProfileId]);

   if( ErrCode != ST_NO_ERROR )
   {
      STPOWER_ErrorCode_t DeviceError;
      _DTraceError("%s(%d):**ERROR** !!! Unable to set the power profile !!! %d",ProfileId);

      if( STPOWER_GetDeviceErrorInProfile(POWER_Handle[PowerId], POWERi_Context[PowerId].ProfileID[ProfileId],&DeviceError) != ST_NO_ERROR )
      {
         _DTraceError("%s(%d):**ERROR** !!! Unable to get the failed device name !!!\n");
         return(ErrCode);
      }
      else
      {
         _DTraceError("%s(%d):**ERROR** !!! Failed device is %s and error code is %d \n");
         return(ErrCode);
      }
   }

   // Update context and state
   POWERi_Context[PowerId].CurrentPowerState = ProfileId;

   // Return no errors
   return(ST_NO_ERROR);
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t CPower::eventConfigure(U32 PowerId, BOOL Enable)
{
   ST_ErrorCode_t                ErrCode = ST_NO_ERROR;
   STEVT_OpenParams_t            EVT_OpenParams;
   STEVT_DeviceSubscribeParams_t EVT_SubscribeParams;

   // Subscribe to the events
   if( (Enable==TRUE)&&(POWERi_Context[PowerId].EVT_Handle==0) )
   {
      // Open an event handler
      ErrCode = STEVT_Open(EVT_DeviceName[0],&EVT_OpenParams,&POWERi_Context[PowerId].EVT_Handle);

      if( ErrCode != ST_NO_ERROR )
      {
         _DTraceError("%s(%d):**ERROR** !!! Unable to open an event handler !!!\n",__FUNCTION__,PowerId);
         return(ErrCode);
      }

      // Subscribe to callback
      memset(&EVT_SubscribeParams,0,sizeof(STEVT_DeviceSubscribeParams_t));
      EVT_SubscribeParams.NotifyCallback = eventCallback;

      // Remote control events
#if defined(BLAST_MAX_NUMBER)
      if( (POWERi_Context[PowerId].WAKEUP_REASON & STPOWER_WAKEUP_IR) == STPOWER_WAKEUP_IR )
      {
         U32 Key;
         while(BLAST_PollKey(POWERi_Context[PowerId].WAKEUP_PARAM.WAKEUP_IR_PARAMETER,&Key) == ST_NO_ERROR);

         ErrCode = STEVT_SubscribeDeviceEvent(POWERi_Context[PowerId].EVT_Handle,BLAST_DeviceName[POWERi_Context[PowerId].WAKEUP_PARAM.WAKEUP_IR_PARAMETER],STBLAST_READ_DONE_EVT,&EVT_SubscribeParams);

         if( ErrCode != ST_NO_ERROR )
         {
            _DTraceError("%s(%d):**ERROR** !!! Unable to subscribe to remote control event for wake-up condition !!!\n",__FUNCTION__,PowerId);
            STEVT_Close(POWERi_Context[PowerId].EVT_Handle);
            POWERi_Context[PowerId].EVT_Handle=0;
            return(ErrCode);
         }
      }
#endif

   }

   // Subscribe to the events
   if( (Enable==FALSE)&&(POWERi_Context[PowerId].EVT_Handle!=0) )
   {
   // Remote control events
#if defined(BLAST_MAX_NUMBER)
      if( (POWERi_Context[PowerId].WAKEUP_REASON&STPOWER_WAKEUP_IR)==STPOWER_WAKEUP_IR )
      {
         ErrCode = STEVT_UnsubscribeDeviceEvent(POWERi_Context[PowerId].EVT_Handle,BLAST_DeviceName[POWERi_Context[PowerId].WAKEUP_PARAM.WAKEUP_IR_PARAMETER],STBLAST_READ_DONE_EVT);
         if( ErrCode != ST_NO_ERROR )
         {
            _DTraceError("%s(%d):**ERROR** !!! Unable to unsubscribe to remote control event for wake-up condition !!!\n",__FUNCTION__,PowerId);
         }
      }
#endif
   // Close event handler
      ErrCode = STEVT_Close(POWERi_Context[PowerId].EVT_Handle);
      if( ErrCode != ST_NO_ERROR )
      {
         _DTraceError("%s(%d):**ERROR** !!! Unable to close an event handler !!!\n",__FUNCTION__,PowerId);
         return(ErrCode);
      }

      POWERi_Context[PowerId].EVT_Handle = 0;
   }

   // Return no errors
   return(ST_NO_ERROR);
}

//////////////////////////////////////////////////////////////////////////////////////
void CPower::wakeUpHandlerPIO (STPIO_Handle_t PIO_Handle,STPIO_BitMask_t PIO_BitMask)
{
   ST_ErrorCode_t ErrCode=ST_NO_ERROR;
   S16 i = 0,ID = 0;

   _DTraceError("Cpu has been wake-up by GPIO !\n");
}

//////////////////////////////////////////////////////////////////////////////////////
void CPower::displayClock(DayTime_dtv_t dayTime)
{
   char str[14];
   int count1 = 0, count2 = 0;

   memset(str, 0, 4*sizeof(char));

   if( dayTime.hms.second%2 == 0 )
   {
      sprintf(str,"%d %d %d %d ",dayTime.hms.hour/10, dayTime.hms.hour%10, dayTime.hms.minute/10, dayTime.hms.minute%10);
   }
   else
   {
      sprintf(str,"%d %d.%d %d ",dayTime.hms.hour/10, dayTime.hms.hour%10, dayTime.hms.minute/10, dayTime.hms.minute%10);
   }

   if( g_IsPowerWakeUp == FALSE )
   {
      _D_.PrintFP(str);
   }
   else
   {
      char* displayLED = "  o n  ";
      _D_.PrintFP(displayLED);
   }

   _D_.PrintFPLED();
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t CPower::profilePopulate(U32 PowerId, U32 ProfileId, U32 DeviceId, STPOWER_State_t POWER_State)
{
   U32 i, NumOfDevices;

   // Check parameters
   if( PowerId >= POWER_MAX_NUMBER )
   {
      return(ST_ERROR_BAD_PARAMETER);
   }

   if( ProfileId >= POWER_PROFILE_MAX )
   {
      return(ST_ERROR_BAD_PARAMETER);
   }

   // Set device count
   NumOfDevices = POWERi_Context[PowerId].ProfileData[ProfileId].NumOfDevices;

   if( NumOfDevices >= POWER_DEVICE_MAX )
   {
      return(ST_ERROR_BAD_PARAMETER);
   }

   // Configure the correct device
   switch(DeviceId)
   {
   // Atapi devices
   #if defined(ATAPI_MAX_NUMBER)
   case POWER_DEVICE_ATAPI:
      for( i = 0; i < ATAPI_MAX_NUMBER; i++ )
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,ATAPI_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Audio devices
   #if defined(AUD_MAX_NUMBER)
   case POWER_DEVICE_AUDIO:
      for (i=0;i<AUD_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,AUD_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Blitter devices
   #if defined(BLIT_MAX_NUMBER)
   case POWER_DEVICE_BLIT:
      for (i=0;i<BLIT_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,BLIT_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Close Caption devices
   #if defined(CC_MAX_NUMBER)
   case POWER_DEVICE_CC:
      for (i=0;i<CC_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,CC_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Subtitle devices
   #if defined(SUBT_MAX_NUMBER)
   case POWER_DEVICE_SUBT:
      for (i=0;i<SUBT_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,SUBT_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Clock Recovery devices
   #if defined(CLKRV_MAX_NUMBER)
   case POWER_DEVICE_CLKRV:
      for (i=0;i<CLKRV_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,CLKRV_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Crypto devices
   #if defined(CRYPT_MAX_NUMBER)
   case POWER_DEVICE_CRYPT:
      for (i=0;i<CRYPT_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,CRYPT_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Demux devices
   #if defined(PTI_MAX_NUMBER)
   case POWER_DEVICE_DEMUX:
      for (i=0;i<PTI_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,PTI_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // TSMerge devices
   #if defined(TSMERGER_MAX_NUMBER)
   case POWER_DEVICE_TSMERGE:
      for (i=0;i<TSMERGER_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,TSMERGER_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Denc devices
   #if defined(DENC_MAX_NUMBER)
   case POWER_DEVICE_DENC:
      for (i=0;i<DENC_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,DENC_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Dma devices
   #if defined(DMA_MAX_NUMBER)
   case POWER_DEVICE_DMA:
      for (i=0;i<DMA_MAX_NUMBER;i++)
      {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,DMA_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
      }
   break;
   #endif

   // Flash devices
   #if defined(FLASH_MAX_NUMBER)
   case POWER_DEVICE_FLASH:
#if 0
      for (i=0;i<FLASH_MAX_NUMBER;i++)
      {
      ST_ErrorCode_t     ErrCode=ST_NO_ERROR;
      FLASH_Parameters_t FLASH_Params;
      ErrCode=FLASH_GetParams(i,&FLASH_Params);
      if (ErrCode==ST_NO_ERROR)
         {
         if (FLASH_Params.Size!=0)
         {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,FLASH_Params.DeviceName,MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
         }
         }
      }
#else
      for (i=0;i<FLASH_MAX_NUMBER;i++)
         {
            strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,FLASH_List[i].DeviceName,MAX_SIZE_OF_DEVICE_NAME);
            POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
            POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
            NumOfDevices++;
         }
#endif
   break;
   #endif

   // Grafix layer devices
   #if defined(GFX_LAYER_MAX_NUMBER)
   case POWER_DEVICE_GRAFIX:
      for (i=0;i<GFX_LAYER_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,GFX_LAYER_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Hdmi devices
   #if defined(HDMI_MAX_NUMBER)
   case POWER_DEVICE_HDMI:
      for (i=0;i<HDMI_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,HDMI_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // I2c devices
   #if defined(I2C_MAX_NUMBER)
   case POWER_DEVICE_I2C:
      for (i=0;i<I2C_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,I2C_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Injector devices
   #if defined(INJECT_MAX_NUMBER)
   case POWER_DEVICE_INJECT:
      for (i=0;i<INJECT_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,INJECT_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Keyscan devices
   #if defined(KEYSCN_MAX_NUMBER)
   case POWER_DEVICE_KEYSCN:
      for (i=0;i<KEYSCN_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,KEYSCN_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Layer devices
   #if defined(VID_LAYER_MAX_NUMBER)
   case POWER_DEVICE_LAYER:
      for (i=0;i<VID_LAYER_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,LAYER_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Pcpd devices
   #if defined(PCPD_MAX_NUMBER)
   case POWER_DEVICE_PCPD:
      for (i=0;i<PCPD_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,PCPD_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Pio devices
   #if defined(PIO_MAX_NUMBER)
   case POWER_DEVICE_PIO:
      for (i=0;i<PIO_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,PIO_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Pod devices
   #if defined(POD_MAX_NUMBER)
   case POWER_DEVICE_POD:
      for (i=0;i<POD_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,POD_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Power devices
   #if defined(POWER_MAX_NUMBER)
   case POWER_DEVICE_POWER:
      for (i=0;i<POWER_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,POWER_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Pwm devices
   #if defined(PWM_MAX_NUMBER)
   case POWER_DEVICE_PWM:
      for (i=0;i<PWM_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,PWM_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Smartcard devices
   #if defined(SMART_MAX_NUMBER)
   case POWER_DEVICE_SMART:
      for (i=0;i<SMART_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,SMART_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Spi devices
   #if defined(SPI_MAX_NUMBER)
   case POWER_DEVICE_SPI:
      for (i=0;i<SPI_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,SPI_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Teletext devices
   #if defined(TTX_MAX_NUMBER)
   case POWER_DEVICE_TTX:
      for (i=0;i<TTX_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,TTX_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Tuner devices
   #if defined(TUNER_MAX_NUMBER)
   case POWER_DEVICE_TUNER:
      for (i=0;i<TUNER_MAX_NUMBER;i++)
      {
      if (TUNER_Handle[i]!=0)
         {
         strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,TUNER_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
         POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
         NumOfDevices++;
         }
      }
   break;
   #endif

   // Uart devices
   #if defined(UART_MAX_NUMBER)
   case POWER_DEVICE_UART:
      for (i=0;i<UART_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,UART_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Vbi devices
   #if defined(VBI_MAX_NUMBER)
   case POWER_DEVICE_VBI:
      for (i=0;i<VBI_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,VBI_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Video devices
   #if defined(VID_MAX_NUMBER)
   case POWER_DEVICE_VIDEO:
      for (i=0;i<VID_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,VID_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Vin devices
   #if defined(VIN_MAX_NUMBER)
   case POWER_DEVICE_VIN:
      for (i=0;i<VIN_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,VIN_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Vmix devices
   #if defined(VMIX_MAX_NUMBER)
   case POWER_DEVICE_VMIX:
      for (i=0;i<VMIX_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,VMIX_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Vout devices
   #if defined(VOUT_MAX_NUMBER)
   case POWER_DEVICE_VOUT:
      for (i=0;i<VOUT_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,VOUT_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Vtg devices
   #if defined(VTG_MAX_NUMBER)
   case POWER_DEVICE_VTG:
      for (i=0;i<VTG_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,VTG_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // LMIVID device
   #if defined(ST_7108)||defined(ST_7141)
   case POWER_DEVICE_LMIVID:
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,"LMIVID",MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
   break;
   #endif

   // USB devices
   #if defined(USB)&&defined(OSPLUS)&&defined(STPOWER_SUPPORTED)
   case POWER_DEVICE_USB:
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,"USB_DEVICES",MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
   break;
   #endif

   // Ethernet devices
   #if defined(TCPIP_MAX_NUMBER)&&defined(OSPLUS)&&defined(STPOWER_SUPPORTED)
   case POWER_DEVICE_TCPIP:
      for (i=0;i<TCPIP_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,TCPIP_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   case POWER_DEVICE_NET:
      for (i=0;i<TCPIP_MAX_NUMBER;i++)
      {
      strncpy(POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceName,NET_DeviceName[i],MAX_SIZE_OF_DEVICE_NAME);
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceState = POWER_State;
      POWERi_Context[PowerId].ProfileData[ProfileId].ProfileEntry[NumOfDevices].DeviceType  = STPOWER_DEVICE;
      NumOfDevices++;
      }
   break;
   #endif

   // Device not known
   default :
      break;
   }

   // Set device count
   POWERi_Context[PowerId].ProfileData[ProfileId].NumOfDevices = NumOfDevices;

   // Return no errors
   return(ST_NO_ERROR);
}

//////////////////////////////////////////////////////////////////////////////////////
GPowerState_t CPower::getState()
{
   return m_gPowerState;
}

//////////////////////////////////////////////////////////////////////////////////////
void CPower::setState(GPowerState_t gPowerState)
{
   m_gPowerState = gPowerState;
}

//////////////////////////////////////////////////////////////////////////////////////
void CPower::enterStandby(STPOWER_CPUState_t CPU_State, U32 WakeUpTimer, S32 PROFILE_ID)
{
   _DTraceError("");

   if( m_gPowerState != GPS_ACTIVE )
   {
      _DTraceError("-> Incorrect state...");
      return;
   }

   _DTraceError("-> Go to app standby...");

   g_IsPowerWakeUp = FALSE;
   m_cpuState = CPU_State;

   // offset 1 min for App Standby
   U32 appStandbyDurationInSec = k_AppStandbyDuration/1000;
   if( WakeUpTimer > appStandbyDurationInSec )
   {
      m_wakeUpDuration = WakeUpTimer - appStandbyDurationInSec;
   }
   else
   {
      m_wakeUpDuration = 0;
   }

   m_profileID = PROFILE_ID;

   for( int i = 0; i < VOUT_MAX_NUMBER; i++ )
   {
      if( VOUT_Handle[i] != 0 )
      {
         STVOUT_Disable(VOUT_Handle[i]);
      }
   }

   m_gPowerState = GPS_STANDBY_APP;

   BOOL isGoingToSystemStandby = TRUE;
   standbyApp( k_AppStandbyDuration, isGoingToSystemStandby, TRUE);


   if( (m_wakeUpDuration > 0 && m_wakeUpDuration < k_minMSecBeforeWakeup/1000) || (WakeUpTimer == appStandbyDurationInSec) )
   {
      standbyApp( m_wakeUpDuration*1000 + k_minMSecBeforeWakeup, isGoingToSystemStandby, FALSE);
   }

   if( isGoingToSystemStandby == TRUE )
   {
      _DTraceError("-> Go to system power standby...\n");

      GEventManager::getInstance().fire(GEVENT_STOP_TIMER_CHECK_ALARM);
      Task_Delay( 5 * ST_GetClocksPerSecond() );

      m_gPowerState = GPS_STANDBY_SYS;

      for( int i = 0; i < VOUT_MAX_NUMBER; i++ )
      {
         if( VOUT_Handle[i] != 0 )
         {
            STVOUT_Enable(VOUT_Handle[i]);
         }
      }

      Task_Delay( 3 * ST_GetClocksPerSecond() );
      // Go to system power standby, and CPU is standing here
      // until wake up ...
      this->setMode(m_cpuState, m_wakeUpDuration, m_profileID);

      //...
      //wait for wakeup
      //...
      this->enterWakeUp();
   }
   else
   {
      if( m_gPowerState == GPS_STANDBY_APP )
      {
         this->enterWakeUp();
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void CPower::resetSTVOUT()
{
   _DTraceError("Reset STVOUT when unable to enable the HDMI interface");
   Task_Delay(ST_GetClocksPerSecond()/2);

   for( int i = 0; i < VOUT_MAX_NUMBER; i++ )
   {
      if( VOUT_Handle[i] != 0 )
      {
         STVOUT_Disable(VOUT_Handle[i]);
      }
   }

   Task_Delay(ST_GetClocksPerSecond()/2);

   for( int i = 0; i < VOUT_MAX_NUMBER; i++ )
   {
      if( VOUT_Handle[i] != 0 )
      {
         STVOUT_Enable(VOUT_Handle[i]);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void CPower::standbyApp(U32 durationInMS, BOOL& isGoingToSystemStandby, BOOL isRequestTune)
{
   DayTime_dtv_t dayTime;
   U32 atEntering = _S_.now();
   int count = 0;

   while( isGoingToSystemStandby == TRUE && ((_S_.now() - atEntering) < durationInMS) )
   {
      if( isRequestTune && count % 100 == 0 )
      {
         GEventManager::getInstance().fire(GEVENT_RETUNE_TUNER);
      }
      count++;

      if( m_gPowerState != GPS_STANDBY_APP )
      {
         isGoingToSystemStandby = FALSE;
      }

      U32 keyCodePressed = GKeyManager::getInstance().scanKeyPressed();

      //count > 9 means to avoid holding the key WakeUp in 1 sec
      if( (TRUE == GKeyManager::getInstance().isKeyWakeup(keyCodePressed)) && (count > 9) )
      {
         isGoingToSystemStandby = FALSE;
      }

      _S_.getCurrentTime(dayTime);
      displayClock(dayTime);

      Task_Delay(ST_GetClocksPerSecond()/10);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void CPower::updateWakeUpDuration(U32 wakeUpDuration)
{
   if( m_gPowerState == GPS_STANDBY_APP )
   {
      m_wakeUpDuration = wakeUpDuration;
      _DTraceError("-> m_wakeUpDuration updated: %ld\n", m_wakeUpDuration);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void CPower::enterWakeUp()
{
   _DTraceError("");

   g_IsPowerWakeUp = TRUE;

   char* displayLED = "  o n  ";
   _D_.PrintFP(displayLED);
   _D_.PrintFPLED();

   if( m_gPowerState == GPS_STANDBY_APP )
   {
      _DTraceError("-> Quit from app standby ...");

      for( int i = 0; i < VOUT_MAX_NUMBER; i++ )
      {
        if( VOUT_Handle[i] != 0 )
        {
           STVOUT_Enable(VOUT_Handle[i]);
        }
      }
   }
   else
   {
      _DTraceError("-> Quit from sys standby ...");
   }

   m_gPowerState = GPS_WAKEUP;

   // Flush all keys
   GKeyManager::getInstance().flushAllKey();

   GEventManager::getInstance().fire(GEVENT_EXECUTE_STANDBY);
}

//////////////////////////////////////////////////////////////////////////////////////
