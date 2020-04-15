//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: APISetting.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "APISetting.hpp"
#include "GDebug.hpp"
#include "stvout.h"
#include "stdenc.h"
#include "stvmix.h"

#define DENC_CFG3     (DENC_BASE_ADDRESS + 0xC)
#define DENC_CFG9     (DENC_BASE_ADDRESS + 0x144)

#define DENC_CM       (DENC_BASE_ADDRESS + 0x104)
#define DENC_DAC6     (DENC_BASE_ADDRESS + 0x1AC)

#define DENC_LFCOEF0  (DENC_BASE_ADDRESS + 0x148)
#define DENC_LFCOEF1  (DENC_BASE_ADDRESS + 0x14C)
#define DENC_LFCOEF2  (DENC_BASE_ADDRESS + 0x150)
#define DENC_LFCOEF3  (DENC_BASE_ADDRESS + 0x154)
#define DENC_LFCOEF4  (DENC_BASE_ADDRESS + 0x158)
#define DENC_LFCOEF5  (DENC_BASE_ADDRESS + 0x15C)
#define DENC_LFCOEF6  (DENC_BASE_ADDRESS + 0x160)
#define DENC_LFCOEF7  (DENC_BASE_ADDRESS + 0x164)
#define DENC_LFCOEF8  (DENC_BASE_ADDRESS + 0x168)
#define DENC_LFCOEF9  (DENC_BASE_ADDRESS + 0x16C)

//////////////////////////////////////////////////////////////////////////////////////
int API_AUDGetID()
{
   for( int i = 0; i < AUD_MAX_NUMBER; i++ )
   {
      if( AUD_Handle[i] > 0 )
      {
         return i;
      }
   }
   return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDSetting_UnMute
//   Description: Enable the audio output
//////////////////////////////////////////////////////////////////////////////////////
#if defined(AUD_MAX_NUMBER)
BOOL API_AUDSetting_UnMute(U32 AUD_ID, U32 AUD_TYPE)
{
 ST_ErrorCode_t ErrCode=ST_NO_ERROR;

 /* Enable the audio */
 /* ---------------- */
 if (AUD_TYPE==AUD_ALL)
  {
#if defined(ST_5197)
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP0);
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_SPDIF0);
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_DECODER_COMPRESSED0);
#else
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP0);
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP1);
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_SPDIF0);
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_DECODER_COMPRESSED0);
#if defined(ST_7141)
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_HDMI_PCMP0);
#endif
#endif
  }
 if (AUD_TYPE==AUD_PCM)
  {
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP0);
  }
 if (AUD_TYPE==STAUD_OBJECT_OUTPUT_PCMP1)
  {
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP1);
  }
 if (AUD_TYPE==STAUD_OBJECT_OUTPUT_HDMI_PCMP0)
  {
   ErrCode|=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_HDMI_PCMP0);
  }
 if (AUD_TYPE==AUD_SPDIF)
  {
   ErrCode=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_SPDIF0);
  }
 if (AUD_TYPE==AUD_COMPRESSED)
  {
   ErrCode=STAUD_OPUnMute(AUD_Handle[AUD_ID],STAUD_OBJECT_DECODER_COMPRESSED0);
  }
 if (ErrCode!=ST_NO_ERROR)
  {
   _D_.Debug("API_AUDSetting_UnMute--> Unable to enable the audio output %d !\n",AUD_ID);
   return(FALSE);
  }

 /* Return no errors */
 /* ---------------- */
 return(TRUE);
}
#endif


//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDSetting_Mute
//   Description: Disable the audio output
//////////////////////////////////////////////////////////////////////////////////////
#if defined(AUD_MAX_NUMBER)
BOOL API_AUDSetting_Mute(U32 AUD_ID, U32 AUD_TYPE)
{
 ST_ErrorCode_t ErrCode=ST_NO_ERROR;

 /* Disable the audio */
 /* ----------------- */
 if (AUD_TYPE==AUD_ALL)
  {
#if defined(ST_5197)
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP0);
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_SPDIF0);
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_DECODER_COMPRESSED0);
#else
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP0);
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP1);
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_SPDIF0);
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_DECODER_COMPRESSED0);
#if defined(ST_7141)
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_HDMI_PCMP0);
#endif
#endif
  }
 if (AUD_TYPE==AUD_PCM)
  {
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP0);
  }
 if (AUD_TYPE==STAUD_OBJECT_OUTPUT_PCMP1)
  {
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_PCMP1);
  }
 if (AUD_TYPE==STAUD_OBJECT_OUTPUT_HDMI_PCMP0)
  {
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_HDMI_PCMP0);
  }
 if (AUD_TYPE==AUD_SPDIF)
  {
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_OUTPUT_SPDIF0);
  }
 if (AUD_TYPE==AUD_COMPRESSED)
  {
   ErrCode|=STAUD_OPMute(AUD_Handle[AUD_ID],STAUD_OBJECT_DECODER_COMPRESSED0);
  }
 if (ErrCode!=ST_NO_ERROR)
  {
     _D_.Debug("API_AUDSetting_UnMute--> Unable to disable the audio output %d !\n",AUD_ID);
     return(FALSE);
  }

 /* Return no errors */
 /* ---------------- */
 return(TRUE);
}
#endif


//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDSetting_SetVolume
//   Params:      ID(ID AudioHandle), VOLUME(VOLUM value (0-100%))
//   Description: Set the volume to the audio decoder
//////////////////////////////////////////////////////////////////////////////////////

#if defined(AUD_MAX_NUMBER)
BOOL API_AUDSetting_SetVolume(U32 ID, S32 VOLUME)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;

   STAUD_Attenuation_t AUD_Attenuation;
   STAUD_Capability_t  AUD_Capability;
   U16 AttenuationdB;

   // Get Audio Volume
   if( (VOLUME < 0) || (VOLUME > 100) )
   {
      _DTraceError("API_AUDSetting_SetVolume--> Invalid volume value, should be [0%%..100%%] !\n");
      return(FALSE);
   }

   // Try to get the capabilities in order to get the maximum attenuation
   ErrCode = STAUD_GetCapability(AUD_DeviceName[ID], &AUD_Capability);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("API_AUDSetting_SetVolume--> Unable to set the capabilities of the audio driver !\n");
      return(FALSE);
   }

   // Set the volume now
   memset(&AUD_Attenuation,0,sizeof(STAUD_Attenuation_t));

   if( VOLUME == 0 )
   {
      AttenuationdB = AUD_Capability.PostProcCapability.MaxAttenuation;
   }
   else
   {
      AttenuationdB = 64 - ((64*VOLUME)/100);
   }

   AUD_Attenuation.Left = AttenuationdB;
   AUD_Attenuation.Right = AttenuationdB;
   AUD_Attenuation.LeftSurround = AttenuationdB;
   AUD_Attenuation.RightSurround = AttenuationdB;
   AUD_Attenuation.Center = AttenuationdB;
   AUD_Attenuation.Subwoofer = AttenuationdB;

   ErrCode = STAUD_SetAttenuation(AUD_Handle[ID], AUD_Attenuation);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to set the volume to the audio decoder !\n");
      return(FALSE);
   }

   // Return no errors
   return(TRUE);
}
#endif

//////////////////////////////////////////////////////////////////////////////////////
// Name:        API_AUDSetting_ConfigureChain
// Description: Configure a driver chain
//////////////////////////////////////////////////////////////////////////////////////
#if defined(AUD_MAX_NUMBER)
BOOL API_AUDSetting_ConfigureChain(S32 ID, S32 Profile_ID, S32 Aud_Drvindex, S32 ControlIPAndDec)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   STAUD_StreamParams_t AUD_TransCodeParams;

   // Get Audio Identifier
   if( ID >= AUD_MAX_NUMBER )
   {
      _DTraceError("--> Invalid audio ID, should be [0,1,..] !\n");
      return(FALSE);
   }

   if( AUD_Handle[ID] == 0 )
   {
      _DTraceError("--> This audio %d is undefined on this hardware !\n", ID);
      return(FALSE);
   }

   // Get the audio profile AUD_DAC_PROFILE_STANDARD or AUD_DAC_PROFILE_MS11
   if( (Profile_ID != AUD_DAC_PROFILE_STANDARD) && (Profile_ID != AUD_DAC_PROFILE_MS11) )
   {
      _DTraceError("-->  Invalid profile should be [A_PROFILE_STANDARD,A_PROFILE_MS11]!\n");
      return(FALSE);
   }

   //Get the stapp audio subchain index
   // Should be A_SINGLE_DEC_AHS, A_SINGLE_DEC_AS, A_SINGLE_DEC_AH,
   //           A_SINGLE_DEC_A, A_PCM_MIX_AHS, A_PCM_MIX_AS, A_PCM_MIX_AH,
   //           A_PCM_MIX_A, A_DUAL_DEC_PCM_MIX_AHS, A_DUAL_DEC_PCM_MIX_AS,
   //           A_DUAL_DEC_PCM_MIX_AH, A_DUAL_DEC_PCM_MIX_A, A_TRANSCODE_AHS,
   //           A_TRANSCODE_AS, MS10_11_AHS, A_CANVAS_AHS, A_DUAL_DEC_NOMIX
   //           Aud_Drvindex = AUD_DUAL_DECODE_NOMIX_ANALOG_HDMI_SPDIF;

   //Set the  Stream Params as default
   AUD_TransCodeParams.SamplingFrequency = 48000;
   AUD_TransCodeParams.StreamType = STAUD_STREAM_TYPE_ES;
   //Get Transcoded output stream content (STAUD_STREAM_CONTENT_AC3, STAUD_STREAM_CONTENT_DTS, STAUD_STREAM_CONTENT_NULL)
   AUD_TransCodeParams.StreamContent = STAUD_STREAM_CONTENT_NULL;

   //configure the subchain
   ErrCode = AUD_ConfigureChain((U32)ID, (AUD_DynamicChainProfile_t)Profile_ID, (U32)Aud_Drvindex, (BOOL)ControlIPAndDec, &AUD_TransCodeParams);
   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to Configure the Audio driver Chain chain ErrCode=%d !\n",ErrCode);
      return(FALSE);
   }

   _DTraceError("Successfully");

   // Return no errors
   return(TRUE);
}
#endif

//////////////////////////////////////////////////////////////////////////////////////
// Name:        API_AUDSetting_GetStatistics
// Description: Get Audio Statistics
//////////////////////////////////////////////////////////////////////////////////////
#if defined(AUD_MAX_NUMBER)
BOOL API_AUDSetting_GetStatistics(U32 AudID, STAUD_Object_t AUD_Object, AUD_Statistics_t& AUD_Statistics, STAUD_StreamInfo_t& AUD_StreamInfo)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   BOOL ret = FALSE;

   if( AudID >= AUD_MAX_NUMBER )
   {
      return ret;
   }

   if( AUD_Handle[AudID] == 0 )
   {
      return ret;
   }

   // Get statistics
   memset(&AUD_Statistics, 0, sizeof(AUD_Statistics_t));
   ErrCode = AUD_GetStatistics(AUD_DeviceName[AudID], &AUD_Statistics);
   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to get informations from the audio %d ErrCode=%d !\n",AudID,ErrCode);
      return ret;
   }

   // Get stream infos
#if !defined(ST_5197)
   //AUD_Object = STAUD_OBJECT_DECODER_COMPRESSED0, STAUD_OBJECT_DECODER_COMPRESSED1, STAUD_OBJECT_DECODER_COMPRESSED2
   ErrCode = STAUD_DRGetStreamInfo(AUD_Handle[AudID], AUD_Object, &AUD_StreamInfo);
   if( ErrCode != ST_NO_ERROR )
   {
      if( AUD_Object == STAUD_OBJECT_DECODER_COMPRESSED0 )
      {
         _DTraceError("--> DECODER_COMPRESSED0 Unable to get stream informations from the audio %d ErrCode=%d !\n",AudID, ErrCode);
      }
      else if( AUD_Object == STAUD_OBJECT_DECODER_COMPRESSED1 )
      {
         _DTraceError("--> DECODER_COMPRESSED1 Unable to get stream informations from the audio %d ErrCode=%d !\n",AudID, ErrCode);
      }
      return ret;
   }

   switch( AUD_StreamInfo.StreamContent )
   {
      case STAUD_STREAM_CONTENT_AC3        : ret = TRUE; _DTraceError("AC3\n");         break;
      case STAUD_STREAM_CONTENT_DTS        : ret = TRUE; _DTraceError("DTS\n");         break;
      case STAUD_STREAM_CONTENT_MPEG1      : ret = TRUE; _DTraceError("MPEG1\n");       break;
      case STAUD_STREAM_CONTENT_MPEG2      : ret = TRUE; _DTraceError("MPEG2\n");       break;
      case STAUD_STREAM_CONTENT_CDDA       : ret = TRUE; _DTraceError("CDDA\n");        break;
      case STAUD_STREAM_CONTENT_PCM        : ret = TRUE; _DTraceError("PCM\n");         break;
      case STAUD_STREAM_CONTENT_LPCM       : ret = TRUE; _DTraceError("LPCM\n");        break;
      case STAUD_STREAM_CONTENT_PINK_NOISE : ret = TRUE; _DTraceError("PINK_NOISE\n");  break;
      case STAUD_STREAM_CONTENT_MP3        : ret = TRUE; _DTraceError("MP3\n");         break;
      case STAUD_STREAM_CONTENT_MLP        : ret = TRUE; _DTraceError("MLP\n");         break;
      case STAUD_STREAM_CONTENT_BEEP_TONE  : ret = TRUE; _DTraceError("BEEP_TONE\n");   break;
      case STAUD_STREAM_CONTENT_MPEG_AAC   : ret = TRUE; _DTraceError("MPEG_AAC\n");    break;
      case STAUD_STREAM_CONTENT_WMA        : ret = TRUE; _DTraceError("WMA\n");         break;
      case STAUD_STREAM_CONTENT_DV         : ret = TRUE; _DTraceError("DV\n");          break;
      case STAUD_STREAM_CONTENT_CDDA_DTS   : ret = TRUE; _DTraceError("CDDA_DTS\n");    break;
      case STAUD_STREAM_CONTENT_LPCM_DVDA  : ret = TRUE; _DTraceError("LPCM_DVDA\n");   break;
      case STAUD_STREAM_CONTENT_HE_AAC     : ret = TRUE; _DTraceError("HEAAC\n");       break;
      case STAUD_STREAM_CONTENT_DDPLUS     : ret = TRUE; _DTraceError("DDPLUS\n");      break;
      case STAUD_STREAM_CONTENT_WMAPROLSL  : ret = TRUE; _DTraceError("WMAPROLSL\n");   break;
      case STAUD_STREAM_CONTENT_ALSA       : ret = TRUE; _DTraceError("ALSA\n");        break;
      case STAUD_STREAM_CONTENT_MP4_FILE   : ret = TRUE; _DTraceError("MP4_FILE\n");    break;
      case STAUD_STREAM_CONTENT_ADIF       : ret = TRUE; _DTraceError("ADIF\n");        break;
      case STAUD_STREAM_CONTENT_RAW_AAC    : ret = TRUE; _DTraceError("RAW_AAC\n");     break;
      case STAUD_STREAM_CONTENT_WAVE       : ret = TRUE; _DTraceError("WAVE\n");        break;
      case STAUD_STREAM_CONTENT_AIFF       : ret = TRUE; _DTraceError("AIFF\n");        break;
      case STAUD_STREAM_CONTENT_DRA        : ret = TRUE; _DTraceError("DRA\n");         break;
      case STAUD_STREAM_CONTENT_RA         : ret = TRUE; _DTraceError("REALAUDIO\n");   break;
      case STAUD_STREAM_CONTENT_VRBS       : ret = TRUE; _DTraceError("VORBIS\n");      break;
      case STAUD_STREAM_CONTENT_FLAC       : ret = TRUE; _DTraceError("FLAC\n");        break;
      case STAUD_STREAM_CONTENT_DDPULSE    : ret = TRUE; _DTraceError("DDPULSE\n");     break;
      case STAUD_STREAM_CONTENT_NULL       : ret = FALSE; _DTraceError("NULL\n");       break;
      default                              : ret = FALSE; _DTraceError("????\n");       break;
   }

   _DTraceError("Sampling frequency = %dHz\n", AUD_StreamInfo.SamplingFrequency);
#endif

   return ret;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDSetting_SetStereo
//   Description: Set the stereo output
//////////////////////////////////////////////////////////////////////////////////////
#if defined(AUD_MAX_NUMBER)
BOOL API_AUDSetting_SetStereo(U32 ID, STAUD_Stereo_t STEREO_MODE)
{
 ST_ErrorCode_t ErrCode=ST_NO_ERROR;

 /* Get Audio Identifier */
 /* -------------------- */
 if (ID>=AUD_MAX_NUMBER)
  {
   _DTraceError("API_AUDSetting_SetStereo--> Invalid audio ID, should be [0,1,..] !\n");
   return(FALSE);
  }
 if (AUD_Handle[ID]==0)
  {
     _DTraceError("API_AUDSetting_SetStereo--> This audio %d is undefined on this hardware !\n",ID);
     return(FALSE);
  }

 /* Set the stereo output mode */
 /* -------------------------- */
 ErrCode=STAUD_SetStereoOutput(AUD_Handle[ID],STEREO_MODE);
 if (ErrCode!=ST_NO_ERROR)
  {
     _DTraceError("API_AUDSetting_SetStereo--> Unable to set the stereo output mode to the audio decoder %d !\n",ID);
     return(FALSE);
  }

 /* Return no errors */
 /* ---------------- */
 return(TRUE);
}
#endif

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDSettingIVCFlatResponse
//   Description: Set the IVC flat response
//////////////////////////////////////////////////////////////////////////////////////
#if defined(AUD_MAX_NUMBER)
BOOL API_AUDSettingIVCFlatResponse(U32 AUD_Id, STAUD_Object_t AUD_Object, U32 AUD_RefValue, BOOL AUD_IsFlashResponse)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32 DBtoMiliBell = -100;
   STAUD_PPDRCConfigParams_t AUD_PPSetDRCParams;

   // Get Audio Identifier
   if( AUD_Id >= AUD_MAX_NUMBER )
   {
      _DTraceError("--> Invalid audio ID, should be [0,1,..] !\n");
      return FALSE;
   }

   if( AUD_Handle[AUD_Id] == 0 )
   {
      _DTraceError("--> This audio %d is undefined on this hardware !\n", AUD_Id);
      return FALSE;
   }

   //Set DRC type FOR IVC Params
   memset(&AUD_PPSetDRCParams, 0, sizeof(STAUD_PPSTIVCConfigParams_t) );
   AUD_PPSetDRCParams.DRCType = STAUD_ST_IVC;

   //Get post processor id
   if( (AUD_Object != STAUD_OBJECT_POST_PROCESSOR0) &&
       (AUD_Object != STAUD_OBJECT_POST_PROCESSOR1) &&
       (AUD_Object != STAUD_OBJECT_POST_PROCESSOR2) &&
       (AUD_Object != STAUD_OBJECT_DECODER_COMPRESSED0) )
   {
      _DTraceError("--> Invalid post processor value, should be [A_PP0,A_PP1,A_PP2,A_DEC0] !\n");
      return FALSE;
   }

   // Get the IVC Flat Response
   if( AUD_IsFlashResponse == TRUE )
   {
      AUD_PPSetDRCParams.AlgoUsed.STIVCParams.Mode = (STAUD_PPSTIVCMode_t)STAUD_STIVC_LOUDNESS;
      //This function is only valid for updating the IVC loudness params
      AUD_PPSetDRCParams.AlgoUsed.STIVCParams.UpdateMode = STAUD_STIVC_LOUDNESS;
   }
   else
   {
      AUD_PPSetDRCParams.AlgoUsed.STIVCParams.Mode = (STAUD_PPSTIVCMode_t)STAUD_STIVC_BYPASS;
      //This function is only valid for updating the IVC loudness params
      AUD_PPSetDRCParams.AlgoUsed.STIVCParams.UpdateMode = STAUD_STIVC_BYPASS;
   }

   //Get the Flat response ref value for IVC
   if( (AUD_RefValue < 0) || (AUD_RefValue > 96) )
   {
      _DTraceError("--> Invalid value, should be [0,96] to set IVC ref value !\n");
      return FALSE;
   }

   AUD_PPSetDRCParams.AlgoUsed.STIVCParams.Loudness_p.RefLevel = (S16)(AUD_RefValue*DBtoMiliBell);
   AUD_PPSetDRCParams.AlgoUsed.STIVCParams.Loudness_p.SmoothTime = 2000;
   AUD_PPSetDRCParams.AlgoUsed.STIVCParams.Loudness_p.MaxGain = 2400;
   AUD_PPSetDRCParams.AlgoUsed.STIVCParams.Loudness_p.Strength = 10;
   //[0-10] can be added in a UI
   //SGP_CA CCBN
   //SDK_37_CCBN: Changed to -800 to show we can reduce the level as well
   AUD_PPSetDRCParams.AlgoUsed.STIVCParams.Loudness_p.RefLevel = -800;
   AUD_PPSetDRCParams.AlgoUsed.STIVCParams.Limiter_p.RmsMeasure = 0;
   AUD_PPSetDRCParams.AlgoUsed.STIVCParams.Limiter_p.Threshold = -200;

   //Set the IVC flat response params
   ErrCode = STAUD_PPSetDRCParams(AUD_Handle[AUD_Id], AUD_Object, &AUD_PPSetDRCParams);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to set IVC flat response params ErrCode=%d !\n",ErrCode);
      return FALSE;
   }

   //Return no errors
   return TRUE;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////
void API_AUDPPEnableDownsampling(U32 AudID, U32 FrqHz)
{
   if( AUD_Handle[AudID] == 0 )
   {
      _DTraceError("--> invalid audio id = %d!\n", AudID);
      return;
   }

   STAUD_PPEnableDownsampling(AUD_Handle[AudID], STAUD_OBJECT_POST_PROCESSOR0, FrqHz);
   STAUD_PPEnableDownsampling(AUD_Handle[AudID], STAUD_OBJECT_POST_PROCESSOR1, FrqHz);
   STAUD_PPEnableDownsampling(AUD_Handle[AudID], STAUD_OBJECT_POST_PROCESSOR2, FrqHz);
}

//////////////////////////////////////////////////////////////////////////////////////
void API_AUDPPDisableDownsampling(U32 AudID)
{
   if( AUD_Handle[AudID] == 0 )
   {
      _DTraceError("--> invalid audio id = %d!\n", AudID);
      return;
   }

   STAUD_PPDisableDownsampling(AUD_Handle[AudID], STAUD_OBJECT_POST_PROCESSOR0);
   STAUD_PPDisableDownsampling(AUD_Handle[AudID], STAUD_OBJECT_POST_PROCESSOR1);
   STAUD_PPDisableDownsampling(AUD_Handle[AudID], STAUD_OBJECT_POST_PROCESSOR2);
}

//////////////////////////////////////////////////////////////////////////////////////
int API_VOUTGetID()
{
   for( int i = 0; i < VOUT_MAX_NUMBER; i++ )
   {
      if( VOUT_Handle[i] > 0 )
      {
         return i;
      }
   }
   return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
int API_DENCGetID()
{
   for( int i = 0; i < DENC_MAX_NUMBER; i++ )
   {
      if( DENC_Handle[i] > 0 )
      {
         return i;
      }
   }
   return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
int API_VMIXGetID()
{
   for( int i = 0; i < VMIX_MAX_NUMBER || VTG_MAX_NUMBER; i++ )
   {
      if( VMIX_Handle[i] > 0 && VTG_Handle[i] > 0 )
      {
         return i;
      }
   }
   return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_SetBCS(U32 VOUT_ID, APISetting_BCS_t BCS_Value)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   STVOUT_OutputParams_t VOUT_OutputParams;
   /* Get vout identifier */
   /* ------------------- */
   if( VOUT_Handle[VOUT_ID] == 0 )
   {
      _DTraceError("API_VIDSetting_SetBCS--> This vout %d is undefined on this hardware !\n", VOUT_ID);
      return(FALSE);
   }
   /* Set the level parameters */
   /* ------------------------ */
   memset(&VOUT_OutputParams, 0, sizeof(STVOUT_OutputParams_t));
   ErrCode = STVOUT_GetOutputParams(VOUT_Handle[VOUT_ID], &VOUT_OutputParams);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("API_VIDSetting_SetBCS--> Unable to get current output params of vout stage %d !\n", VOUT_ID);
      return(FALSE);
   }

   VOUT_OutputParams.Analog.StateAnalogLevel = STVOUT_PARAMS_NOT_CHANGED;
   VOUT_OutputParams.Analog.StateChrLumFilter = STVOUT_PARAMS_NOT_CHANGED;
   VOUT_OutputParams.Analog.StateBCSLevel = STVOUT_PARAMS_AFFECTED;
   VOUT_OutputParams.Analog.BCSLevel.Brightness = BCS_Value.Brightness;
   VOUT_OutputParams.Analog.BCSLevel.Contrast = BCS_Value.Contrast;
   VOUT_OutputParams.Analog.BCSLevel.Saturation = BCS_Value.Saturation;

   _DTraceError("--> API_VIDSetting_SetBCS, VOUT_ID - %d, brightness %d, contrast %d, saturation %d !\n", VOUT_ID, VOUT_OutputParams.Analog.BCSLevel.Brightness, VOUT_OutputParams.Analog.BCSLevel.Contrast, VOUT_OutputParams.Analog.BCSLevel.Saturation);

   ErrCode = STVOUT_SetOutputParams(VOUT_Handle[VOUT_ID], &VOUT_OutputParams);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("API_VIDSetting_SetBCS--> Unable to set brightness/contrast/saturations levels of vout stage %d !\n", VOUT_ID);
      return(FALSE);
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_GetBCS(U32 VOUT_ID, APISetting_BCS_t &BCS_Value)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   STVOUT_OutputParams_t VOUT_OutputParams;
   /* Get vout identifier */
   /* ------------------- */
   if( VOUT_ID >= VOUT_MAX_NUMBER )
   {
      _DTraceError("API_VIDSetting_GetBCS--> This denc %d is undefined on this hardware !\n", VOUT_ID);
      return(FALSE);
   }

   if( VOUT_Handle[VOUT_ID] == 0 )
   {
      _DTraceError("API_VIDSetting_GetBCS--> This vout %d is undefined on this hardware !\n", VOUT_ID);
      return(FALSE);
   }
   /* Set the level parameters */
   /* ------------------------ */
   memset(&VOUT_OutputParams, 0, sizeof(STVOUT_OutputParams_t));
   ErrCode = STVOUT_GetOutputParams(VOUT_Handle[VOUT_ID], &VOUT_OutputParams);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("API_VIDSetting_SetBCS--> Unable to get current output params of vout stage %d !\n", VOUT_ID);
      return(FALSE);
   }

   BCS_Value.Brightness = VOUT_OutputParams.Analog.BCSLevel.Brightness;
   BCS_Value.Contrast = VOUT_OutputParams.Analog.BCSLevel.Contrast;
   BCS_Value.Saturation = VOUT_OutputParams.Analog.BCSLevel.Saturation;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_SetMode(U32 DENC_ID, STDENC_Mode_t Mode)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;

   if( DENC_ID >= DENC_MAX_NUMBER )
   {
      _DTraceError("API_VIDSetting_SetMode--> This denc %d is undefined on this hardware !\n", DENC_ID);
      return(FALSE);
   }

   if( DENC_Handle[DENC_ID] == 0 )
   {
      _DTraceError("API_VIDSetting_SetMode--> This denc %d is undefined on this hardware !\n", DENC_ID);
      return(FALSE);
   }

   ErrCode = DENC_SetMode(DENC_ID, Mode);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("API_VIDSetting_SetMode--> Unable to set the new mode !\n");
      return(FALSE);
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
STDENC_Mode_t API_VIDSetting_GetMode(U32 DENC_ID)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   STDENC_EncodingMode_t DENC_EncodingMode;

   if( DENC_ID >= DENC_MAX_NUMBER )
   {
      _DTraceError("API_VIDSetting_GetMode--> This denc %d is undefined on this hardware !\n", DENC_ID);
      return(STDENC_MODE_NONE);
   }

   if( DENC_Handle[DENC_ID] == 0 )
   {
      _DTraceError("API_VIDSetting_GetMode--> This denc %d is undefined on this hardware !\n", DENC_ID);
      return(STDENC_MODE_NONE);
   }

   ErrCode = STDENC_GetEncodingMode(DENC_Handle[DENC_ID], &DENC_EncodingMode);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("API_VIDSetting_GetMode--> Unable to get the current mode !\n");
      return(STDENC_MODE_NONE);
   }

   return DENC_EncodingMode.Mode;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetFormat(int videoOutId, STVID_DisplayAspectRatioConversion_t aRConversion, S32 layer)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;

   if( videoOutId >= VID_MAX_NUMBER )
   {
      _DTraceError("--> Invalid video ID \n");
      return FALSE;
   }

   if( VID_Codec[videoOutId].VID_Handle == 0 )
   {
      _DTraceError("--> This video %d is not yet initialized, you need to start a video decode first !\n", videoOutId );
      return FALSE;
   }

   if( (aRConversion != STVID_DISPLAY_AR_CONVERSION_PAN_SCAN)   &&
       (aRConversion != STVID_DISPLAY_AR_CONVERSION_LETTER_BOX) &&
       (aRConversion != STVID_DISPLAY_AR_CONVERSION_COMBINED)   &&
       (aRConversion != STVID_DISPLAY_AR_CONVERSION_IGNORE)     &&
       (aRConversion != STVID_DISPLAY_AR_CONVERSION_PANORAMIC) )
   {
      _DTraceError("--> Invalid video aspect ratio !");
      return FALSE;
   }

   if( (layer != VD_ALL) && (layer >= VID_LAYER_MAX_NUMBER) )
   {
      _DTraceError("--> Invalid video layer");
      return FALSE;
   }

   if( layer != VD_ALL )
   {
      if ( VID_Codec[videoOutId].VID_ViewPortHandle[layer] == 0 )
      {
         _DTraceError("--> Viewport %d of video %d is not yet created !\n", layer, videoOutId );
         return FALSE;
      }
   }

   _DTraceError("--> STVID_SetDisplayAspectRatioConversion video=%d, layer=%d, conversion=%d,\n", videoOutId, layer, aRConversion);

   ErrCode |= STVID_SetDisplayAspectRatioConversion( VID_Codec[videoOutId].VID_ViewPortHandle[layer], aRConversion );

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to change aspect ratio of video %d ErrCode=%d !\n", videoOutId, ErrCode);
      return FALSE;
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_SetAspectRatio(U32 VMIX_ID, STGXOBJ_AspectRatio_t AspectRatio)
{
   if( (VMIX_ID >= VMIX_MAX_NUMBER) || (VMIX_ID >= VTG_MAX_NUMBER) )
   {
      _DTraceError( "--> Invalid mixer/vtg ID, should be [0,1,..] !\n");
      return FALSE;
   }

   if( VMIX_Handle[VMIX_ID] == 0 )
   {
      _DTraceError( "--> This mixer %d doesn't exist on this hardware !\n", VMIX_ID );
      return FALSE;
   }

   if( VTG_Handle[VMIX_ID] == 0 )
   {
      _DTraceError( "--> This vtg %d doesn't exist on this hardware !\n", VMIX_ID );
      return FALSE;
   }

   _DTraceError("--> VMIX_SetAspectRatio VMIX_ID=%d, AspectRatio=%d, conversion=%d,\n", VMIX_ID, AspectRatio);

   if( VMIX_SetAspectRatio( VMIX_ID, AspectRatio ) != ST_NO_ERROR)
   {
      _DTraceError( "API_VIDSetting_SetAspectRatio--> Unable to set aspect ratio for mixer %d !\n", VMIX_ID );
      return FALSE;
   }

   return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////
// Name:        API_VIDSetting_SetRecovery
// Description: Set the error recovery mode
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_SetRecovery(U32 CodecID, STVID_ErrorRecoveryMode_t ErrMode)
{
   S32 ID = (S32)CodecID;
   STVID_ErrorRecoveryMode_t MODE = (STVID_ErrorRecoveryMode_t)ErrMode;

   // Get Video Identifier
   if( VID_Codec[ID].VID_Handle == 0 )
   {
      _DTraceError("--> This video %d is not yet initialized, you need to start a video decode first !", ID);
      return FALSE;
   }

   // Get Recovery mode
   if( (MODE != STVID_ERROR_RECOVERY_FULL) && (MODE != STVID_ERROR_RECOVERY_HIGH) &&
       (MODE != STVID_ERROR_RECOVERY_PARTIAL) && (MODE != STVID_ERROR_RECOVERY_NONE) )
   {
      _DTraceError("--> Invalid recovery mode, should be [V_FULL,V_HIGH,V_PARTIAL,V_NONE] !");
      return FALSE;
   }

   // Set the video recovery mode
   if( STVID_SetErrorRecoveryMode(VID_Codec[ID].VID_Handle, MODE) != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to set the new recovery mode!");
      return FALSE;
   }

   // Return no errors
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_DRV_CVBS_SetEx
//   Description: Settings for CVBS output, suggested by ChangHong to pass their AV performance test
//////////////////////////////////////////////////////////////////////////////////////
void API_DRV_CVBS_SetEx(void)
{
#if defined(CHANGHONG)

   U32 DENC_Val32;

   _DTraceError( "\n API_DRV_CVBS_EXT_SETTINGS" );

   //GBS: DENC_CFG3 = 0xFD538000 + 0x00C
   //    [Bit 3]MAIN_DEL_EN: Enable of chroma to luma delay programming on the main 4:4:4 input
   //                 0: Disable (digital encoder automatically sets this delay)
   //                 1: Enable  (chroma to luma delay is programmmed by DEL[3:0] bits from DENC_CFG9)
   //CH: DENC_CFG3, Enable Bright Delay software control

   DENC_Val32 = STSYS_ReadRegDev32LE((void*)(DENC_CFG3));
   //DENC_Val32 = SYS_ReadRegDev32LE(DENC_CFG3);
   DENC_Val32 |= 0x08;
   STSYS_WriteRegDev32LE((void*)(DENC_CFG3), DENC_Val32);
   //SYS_SetBitsDev32LE(DENC_CFG3, DENC_Val32);

   //GBS: DENC_CFG9 = 0xFD538000 + 0x144
   //GBS: Contains the chroma path delay
   //     [Bit 0-5] 0x35 = 110101
   //     [Bit 7:4] 0011: -1.0 pixel delay
   //     [Bit 3]      0: reserved
   //     [Bit 2:1]   10: PLG_DIV_Y - When sum of coefficients = 1024
   //     [Bit 0]      1: FLT_YS - Use register DENC_LCOEF0..9 values
   //CH: DENC_CDFL_LFC Control bright colors ranging delay
   STSYS_WriteRegDev32LE((void*)(DENC_CFG9), 0x35); //Bright colors ranging delay
   //SYS_SetBitsDev32LE(DENC_CFG9, 0x35);

   //GBS: DENC_CM = 0xFD538000 + 0x104
   //     C multiplying factor and teletext
   //     [Bit 7:4] Multiplying factors on C Digital output (before D/A converter)
   //     [Bit 0]   1: Enable - Brightness, contrast and saturation controls by register...
   //CH: DENC_CM Bright gain control*//*modify to 0xa1 for ntsc display
   STSYS_WriteRegDev32LE((void*)(DENC_CM), 0x01);
   //SYS_SetBitsDev32LE(DENC_CM, 0x01);

   //GBS: DENC_DAC6 = 0xFD538000 + 0x1AC
   //     Multiplying factors - For Bar Level
   //CH: DENC_DAC6 control CVBS The entire video output amplitude, the horizontal sync amplitude, burst amplitude adjustment
   STSYS_WriteRegDev32LE((void*)(DENC_DAC6), 0x11);
   //SYS_SetBitsDev32LE(DENC_DAC6, 0x11);

   //GBS: DENC_LU_COEF0-9 Luma filter coefficients 0-9
   //     contains 10 coefficients for luma filter
   //CH: DENC_LCOEF0--DENC_LCOEF9 Controlled frequency response
   //DENC_LFCOEF0 = DENC_BASE_ADDRESS + 0x148
   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF0), 0x21); //Holding 0x23
   //SYS_SetBitsDev32LE(DENC_LFCOEF0, 0x21);

   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF1), 0x80); // Only affect the frequency response to maintain, Holding 0x81
   //SYS_SetBitsDev32LE(DENC_LFCOEF1, 0x80);

   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF2), 0xFF); //0x80~0xbf Gain for bright colors and 0~4M The response waveform influential /
   //SYS_SetBitsDev32LE(DENC_LFCOEF2, 0xFF);

   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF3), 0xFD);
   //SYS_SetBitsDev32LE(DENC_LFCOEF3, 0xFD);

   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF4), 0x1D); //Video amplitude-frequency characteristics control the entire frequency response of the lift, to gain a slight effect bright
   //SYS_SetBitsDev32LE(DENC_LFCOEF4, 0x1D);

   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF5), 0x04);
   //SYS_SetBitsDev32LE(DENC_LFCOEF5, 0x04);

   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF6), 0xAD);//<=4MB Frequency response
   //SYS_SetBitsDev32LE(DENC_LFCOEF6, 0xAD);

   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF7), 0xF8);
   //SYS_SetBitsDev32LE(DENC_LFCOEF7, 0xF8);

   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF8), 0x19);
   //SYS_SetBitsDev32LE(DENC_LFCOEF8, 0x19);

   STSYS_WriteRegDev32LE((void*)(DENC_LFCOEF9), 0x06);
   //SYS_SetBitsDev32LE(DENC_LFCOEF9, 0x06);

   //////////////////////////////////////////////////////////////////
   ////////////////// WILL BE REMOVED - FOR AV TEST /////////////////
   //////////////////////////////////////////////////////////////////
   //0x20
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x24), 0x70);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x28), 0x3F);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x2C), 0xFE);
   //0x30
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x30), 0xBE);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x34), 0x3E);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x38), 0xC3);
   //0x60
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x68), 0xEE);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x6C), 0xDE);
   //0x70
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x70), 0x69);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x74), 0xF3);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x78), 0xC5);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x7C), 0x0F);
   //0x80
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x80), 0xFF);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x84), 0x44);
   //0x90
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x9C), 0x7D);
   //0xa0
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0xA0), 0xCB);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0xA4), 0xFF);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0xA8), 0xEF);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0xAC), 0x0F);
   //0x120
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x120), 0x31);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x124), 0x27);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x128), 0x54);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x12C), 0x47);
   //0x130
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x130), 0x5F);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x134), 0x77);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x138), 0x6C);
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x13C), 0x7B);
   //0x140
   STSYS_WriteRegDev32LE((void*)(DENC_BASE_ADDRESS + 0x140), 0x80);
   //////////////////////////////////////////////////////////////////
   ////////////////// WILL BE REMOVED - FOR AV TEST /////////////////
   //////////////////////////////////////////////////////////////////
#endif

   return;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VTG_SetMode(U32 VTG_Index, STVTG_TimingMode_t VTG_NewMode)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;

   if( VTG_Index >= VTG_MAX_NUMBER )
   {
      return FALSE;
   }

   if( VTG_Handle[VTG_Index] == 0 )
   {
      return FALSE;
   }

   ErrCode = VTG_SetMode(VTG_Index, VTG_NewMode);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("API_VTG_SetMode--> Unable to set the new mode !\n");
      return FALSE;
   }

   return TRUE;
}
