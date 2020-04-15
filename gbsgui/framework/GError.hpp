//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GError.hpp                                                                 //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GERROR_HPP_
#define GERROR_HPP_

#include "stddefs.h"

typedef U32 GError_t;

enum
{
   GERROR_NONE = 0,
   GERROR_EXCEED_MAXIMUM,
   GERROR_ALREADY_EXISTED,
   GERROR_NOT_FOUND,

   GERROR_BAD_PARAMETER,
   GERROR_NO_MEMORY,
   GERROR_UNKNOWN_DEVICE,
   GERROR_ALREADY_INITIALIZED,
   GERROR_NO_FREE_HANDLES,
   GERROR_OPEN_HANDLE,
   GERROR_INVALID_HANDLE,
   GERROR_FEATURE_NOT_SUPPORTED,
   GERROR_TIMEOUT,
   GERROR_DEVICE_BUSY,
   GERROR_SUSPENDED,
   GERROR_CANNOT_GET_PMTTABLE,
   GERROR_AV_SETASPECTRATIO_FAILED,
   GERROR_HDMI_SETASPECTRATIO_FAILED,
   GERROR_AV_SETFORMAT_FAILED,
   GERROR_HDMI_SETFORMAT_FAILED,
   GERROR_SET_RESOLUTION_FAILED,

   GERROR_TYPE_SYSTEM,
};

typedef enum
{
   GDVR_ERR_NONE = 0,
   GDVR_ERR_UNKNOWN,
   GDVR_ERR_INVALID_ID,            // Invalid identifier
   GDVR_ERR_INVALID_PROGRAM_ID,    // Invalid program identifier
   GDVR_ERR_UNABLE_GET_CONNECTION, // Unable to get the connection of the stream to record
   GDVR_ERR_CONNECTION_NAME_DEMUX, // The connection name is not a valid demux source
   GDVR_ERR_NO_PTI,                // There is no PTI for this connection
   GDVR_ERR_UNABLE_GET_PAT,        // Unable to get the PAT
   GDVR_ERR_PROGRAM_NOT_EXIST,     // Sorry, but this program doesn't exist
   GDVR_ERR_UNABLE_GET_LIST,       // Unable to get the list of pids
   GDVR_ERR_PROGRAM_EMPTY,         // This program is empty
   GDVR_ERR_PROGRAM_TOO_MUCH_PID,  // This program contains too much pids to be record
   GDVR_ERR_NO_PID,                // There are 0 pids to record
   GDVR_ERR_UNABLE_INIT,           // Unable to initiate a record
   GDVR_ERR_UNABLE_START,          // Unable to start a record
   GDVR_ERR_UNABLE_PAUSE,          // Unable to pause a record
   GDVR_ERR_UNABLE_RESUME,         // Unable to resume a record
   GDVR_ERR_UNABLE_STOP,           // Unable to stop the record
   GDVR_ERR_UNABLE_SEEK,           // Unable to seek the record
   GDVR_ERR_UNABLE_CHANGE_SPEED,   // Unable to seek the record
   GDVR_ERR_UNABLE_GET_TIME,       // Unable to get time the record
   GDVR_ERR_UNABLE_GET_NAME,       // Unable to get name the record
   GDVR_ERR_UNABLE_TERMINATE,      // Unable to terminate
   GDVR_ERR_UNABLE_GET_STREAM_INF, // Unable get stream info
   GDVR_ERR_UNABLE_SET_DRM,        // Unable to set drm service
   GDVR_ERR_NO_DRM_LICENSE,        // No drm license to play
   GDVR_ERR_BAD_PARAMETER,
   GDVR_ERR_UNABLE_GET_VIDEO_INFO,
   GDVR_ERR_UNABLE_CREATE_EVENT,
   GDVR_ERR_PLAYBACK_IN_PROGRESS,  // The playback is in progress
   GDVR_ERR_FILE_FORMAT_UNSUPORTED,
   GDVR_ERR_INVALID_FILE_NAME,
   GDVR_ERR_USB_HOT_UNPLUGED,
   GDVR_ERR_UNABLE_DELETE_FILE,
   GDVR_ERR_UNABLE_COPY_FILE,
   GDVR_ERR_UNABLE_RENAME_FILE,
   GDVR_ERR_UNABLE_OPEN_FILE,

   //For recording
   GDVR_ERR_IN_PROGRESS,           // The recording is in progress
   GDVR_ERR_RECORD_UNABLE_STOP,    // Unable to stop the record
   GDVR_ERR_CHANNEL_LOCKED,        // Channel has been locked
   GDVR_ERR_MEMORY,                // Not enough memory
   GDVR_ERR_STORAGE_MISSING,       // Mass storage is missing
   GDVR_ERR_STORAGE_FULL,          // Mass storage is full
   GDVR_ERR_NO_SIGNAL,             // Channel has no signal
   GDVR_ERR_SCRAMBLE_CHANNEL,      // Channel scrambled
   GDVR_ERR_SIGNAL_NOT_GOOD,       // Signal not good

   GDVR_ERR_TRANSPONDER_EXIST,

   GDVR_ERR_CHANNEL_NOT_EXIST,

   GDVR_ERR_COUNT
}
GDVRError;

typedef enum
{
   GPIP_ERR_IN_PROGRESS = GDVR_ERR_COUNT + 1,
   GPIP_ERR_SWAP_WHILE_PLAYING_BACK,            // Can't swap while playing back
   GPIP_ERR_OPEN_PLAYING_CHANNEL,               // Can't open the playing channel
   GPIP_ERR_OPEN_PIP_CHANNEL,                   // Can't open the Pip channel
   GPIP_ERR_NO_SIGNAL_FOUND,                    // Can't find any signal
   GPIP_ERR_OPEN_AUDIO_CHANNEL,                 // Can't open a radio channel
   GPIP_ERR_SWAP_WITH_AUDIO_CHANNEL,            // Can't swap with a radio channel

   GPIP_ERR_COUNT
}
GPIPError;

typedef enum
{
   GTIMER_ERR_DATE_START = GPIP_ERR_COUNT + 1,
   GTIMER_ERR_TIME_START,
   GTIMER_ERR_TIME_DURATION,
   GTIMER_ERR_TIME_EXIST,
   GTIMER_ERR_TIME_OVERLAPPED,
   GTIMER_ERR_DAY_SELECTED,
   GTIMER_ERR_RECORD_IS_PRESENTING,

   GTIMER_ERR_COUNT
}
GTimerError;

typedef enum
{
   GTVSETTING_ERR_SHORT_CIRCUIT = GTIMER_ERR_COUNT + 1,

   GTVSETTING_ERR_COUNT
}
GTVSettingError;

typedef enum
{
   GGAME_ERR_NO_NEW_RECORD = GTVSETTING_ERR_COUNT + 1,

   GGAME_ERR_COUNT
}
GGameError;

typedef enum
{
   GDTV_ERR_IN_PROGRESS = GGAME_ERR_COUNT + 1,
   GDTV_ERR_OPEN_AUDIO_CHANNEL,
   GDTV_ERR_NO_AUDIO_CHANNEL,
   GDTV_ERR_NO_SIGNAL_FOUND,
   GDTV_ERR_COUNT
}
GDualTVError;

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////
// ! Dummy
//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GERROR_HPP_ */
