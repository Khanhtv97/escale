//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: APISetting.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef APISETTING_HPP_
#define APISETTING_HPP_

#include "stapp_main.h"

#define VD_MIX       0
#define VD_CODEC     0
#define VD_CODEC_EX  1

#define VD_ALL       0xDEADBEEF
#define VD_MAIN      0
#define VD_AUX       1
#define VD_REMOTE    2
#define VD_NODISPLAY 0xFAFAFAFA
#define VD_VIRTUAL0  0xA0A0A0A0
#define VD_VIRTUAL1  0xA1A1A1A1

#define AUD_ALL         0xDEADBEEF

#define AUD_PCM         STAUD_OBJECT_OUTPUT_PCMP0
#define AUD_SPDIF       STAUD_OBJECT_OUTPUT_SPDIF0
#define AUD_COMPRESSED  STAUD_OBJECT_DECODER_COMPRESSED0

typedef struct
{
   S8 Contrast;
   U8 Brightness;
   U8 Saturation;
}
APISetting_BCS_t;

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDGetID
//   Return:      AUD_ID
//   Description: Get AUD ID for AUD_handle, get only one first use
//////////////////////////////////////////////////////////////////////////////////////
int API_AUDGetID();

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDSetting_UnMute
//   Params:      AUD_ID (ID get from fucntion API_AUDGetID), AUD_TYPE(Type out audio: AUD_ALL, AUD_PCM,AUD_SPDIF, AUD_COMPRESSED)
//   Description: Setup UnMute Audio
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_AUDSetting_UnMute(U32 AUD_ID, U32 AUD_TYPE);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDSetting_Mute
//   Params:      AUD_ID (ID get from fucntion API_AUDGetID), AUD_TYPE(Type out audio: AUD_ALL, AUD_PCM,AUD_SPDIF, AUD_COMPRESSED)
//   Description: Setup Mute Audio
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_AUDSetting_Mute(U32 AUD_ID, U32 AUD_TYPE);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDSetting_SetVolume
//   Params:      ID(Get AUD ID from function API_AUDGetID), VOLUME(VOLUM value (0-100%))
//   Description: Set the volume to the audio decoder
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_AUDSetting_SetVolume(U32 ID, S32 VOLUME);
BOOL API_AUDSetting_SetStereo(U32 ID, STAUD_Stereo_t STEREO_MODE);

//////////////////////////////////////////////////////////////////////////////////////
// Name:        API_AUDSetting_ConfigureChain
// Description: Configure a driver chain
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_AUDSetting_ConfigureChain(S32 ID, S32 ProfileID, S32 AudDrvIndex, S32 ControlIPAndDec);

//////////////////////////////////////////////////////////////////////////////////////
// Name:        API_AUDSetting_GetStatistics
// Description: Get Audio Statistics
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_AUDSetting_GetStatistics(U32 ID, STAUD_Object_t AUD_Object, AUD_Statistics_t& AUD_Statistics, STAUD_StreamInfo_t& AUD_StreamInfo);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_AUDSettingIVCFlatResponse
//   Description: Set the IVC flat response
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_AUDSettingIVCFlatResponse(U32 ID, STAUD_Object_t AUD_Object, U32 AUD_RefValue, BOOL AUD_IsFlashResponse);

void API_AUDPPEnableDownsampling(U32 AudID, U32 FrqHz);
void API_AUDPPDisableDownsampling(U32 AudID);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_VOUTGetID
//   Return:      VOUT_ID
//   Description: Get VOUT ID for VOUT_handle, get only one first use to setup Video Setting Output
//////////////////////////////////////////////////////////////////////////////////////
int API_VOUTGetID();
int API_DENCGetID();
int API_VMIXGetID();

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_VIDSetting_SetBCS
//   Description: Set brightness,contrast,saturation values for vout stage
//   Parameters:  VOUT_ID(Get from function API_VOUTGetID()), Contrast(0->128), staturation(0->128), brightness(0->128)
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_SetBCS(U32 VOUT_ID, APISetting_BCS_t BSC_Value);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_VIDSetting_GetBCS
//   Description: Get brightness,contrast,saturation values from vout stage
//   Return:  Contrast(0->128), staturation(0->128), brightness(0->128)
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_GetBCS(U32 VOUT_ID, APISetting_BCS_t &BSC_Value);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_VIDSetting_SetMode
//   Description: Set denc mode video
//   Params: DENC_ID(Get ID from function API_DENCGetID()), Mode(PAL,NTSC,SECAM)
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_SetMode(U32 DENC_ID, STDENC_Mode_t Mode);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_VIDSetting_GetMode
//   Description: Get denc mode video
//   Return: DENC_ID(Get ID from function API_DENCGetID()), Mode(PAL,NTSC,SECAM)
//////////////////////////////////////////////////////////////////////////////////////
STDENC_Mode_t API_VIDSetting_GetMode(U32 DENC_ID);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_VIDSetFormat
//   Description: Set Display Aspect Ratio Conversion
//   Params: videoOutId(0,1,..), STVID_DisplayAspectRatioConversion_t(PANSCAN,LETERBOX,COMBINE), layer(V_MAIN,V_AUX,V_REMOTE)
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetFormat(int videoOutId, STVID_DisplayAspectRatioConversion_t aRConversion, S32 layer);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_VIDSetting_SetAspectRatio
//   Description: Set AspectRatio video
//   Return: VMIX_ID(Get ID from function API_VMIXGetID()), AspectRatio(16:9,4:3)
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_SetAspectRatio(U32 VMIX_ID, STGXOBJ_AspectRatio_t AspectRatio);

//////////////////////////////////////////////////////////////////////////////////////
// Name:        API_VIDSetting_SetRecovery
// Description: Set the error recovery mode
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VIDSetting_SetRecovery(U32 CodecID, STVID_ErrorRecoveryMode_t ErrMode);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_DRV_CVBS_Settings, API_DRV_CVBS_SettingsEx
//   Description: Settings for CVBS output, suggested by ChangHong to pass their AV performance test
//////////////////////////////////////////////////////////////////////////////////////
void API_DRV_CVBS_SetEx();

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_VTG_SetMode
//   Description: Set Video Time Generator
//   Params: VTG_Index(0,1,..), STVTG_TimingMode_t(STVTG_TIMING_MODE_SLAVE,STVTG_TIMING_MODE_480P30000_12285,STVTG_TIMING_MODE_720P30000_37125,...)
//////////////////////////////////////////////////////////////////////////////////////
BOOL API_VTG_SetMode(U32 VTG_Index, STVTG_TimingMode_t VTG_NewMode);

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        API_VTG_GetMode
//   Description: Get vtg mode and format
//   Params: VTG_ID(0,1,..)
//////////////////////////////////////////////////////////////////////////////////////
STVTG_TimingMode_t API_VTG_GetMode(U32 VTG_ID);


#endif // APISETTING_HPP_
