//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GLang_EN.hpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef G_L_EN_HPP_
#define G_L_EN_HPP_

#include <string>
using namespace std;
#include "GLanguage.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////

void GLanguage::Initialize_EN(string* l)
{
   l[_L_OK]      .assign("OK");
   l[_L_CANCEL]  .assign("Cancel");
   l[_L_CANCEL1] .assign("Cancel");
   l[_L_YES]     .assign("Yes");
   l[_L_NO]      .assign("No");
   l[_L_DENY]    .assign("Deny");
   l[_L_DENY1]   .assign("Cancel");
   l[_L_IGNORE]  .assign("Ignore");
   l[_L_DVBS]    .assign("Digital Video Broadcasting — Satellite");
   l[_L_GUIDE_BACK_TO_MENU] .assign("Previous Menu");
   l[_L_ON]      .assign("On");
   l[_L_OFF]     .assign("Off");
   l[_L_LOCK]    .assign("Lock");
   l[_L_UNLOCK]  .assign("Unlock");
   l[_L_AUTO]    .assign("Auto");
   l[_L_RESET]   .assign("Reset");
   l[_L_WATTING] .assign("Please wait a moment .");
   l[_L_SAVE]    .assign("Save");
   l[_L_SAVING]  .assign("Saving ...");
   l[_L_LOCKUNLOCK] .assign("Lock / Unlock");
   l[_L_DEFAULT] .assign("Default");
   l[_L_PRESS_OK] .assign("Press OK");

   //GUIDE
   l[_L_GUIDE_DELETE]       .assign("Delete");
   l[_L_GUIDE_DELETE_ALL]   .assign("Delete All");
   l[_L_GUIDE_SPACE]        .assign("Space");
   l[_L_GUIDE_EXIT]         .assign("Exit");
   l[_L_GUIDE_SORT]         .assign("Sort");
   l[_L_GUIDE_SORT_AZ]      .assign("Sort A-Z");
   l[_L_GUIDE_SORT_ZA]      .assign("Sort Z-A");
   l[_L_GUIDE_FIND]         .assign("Find");
   l[_L_GUIDE_ADD]          .assign("Add");
   l[_L_GUIDE_MOVE]         .assign("Move");
   l[_L_GUIDE_ADD_SYMBOL]   .assign("Add Symbol");

   //COMMON
   l[_L_CYCLE]              .assign("Cycle");
   l[_L_CYCLE_ONCE]         .assign("Once");
   l[_L_CYCLE_DAILY]        .assign("Daily");
   l[_L_CYCLE_WEEKLY]       .assign("Weekly");
   l[_L_MODE]               .assign("Mode");
   l[_L_MODE_ALARM]         .assign("Alarm");
   l[_L_MODE_STANDBY]       .assign("Standby");
   l[_L_MODE_RECORD]        .assign("Record");
   l[_L_MODE_NOT_DEFINED]   .assign("Not Defined");
   l[_L_NO_CHANNEL]         .assign("No Channel");
   l[_L_NO_SIGNAL]          .assign("No Signal");
   l[_L_NO_AUDIO]           .assign("No Audio Signal ");
   l[_L_SCRAMBLE_CHANNEL]   .assign("Scramble channel");
   l[_L_SIGNAL_NOT_GOOD]    .assign("Signal Quality Low");

   //MAIN MENU
   l[_L_MAIN_MENU_CHANNEL_MANAGER]                   .assign("Channel Manager");
   l[_L_MAIN_MENU_CHANNEL_MANAGER_CHANNEL_LIST]      .assign("Channel List");
   l[_L_MAIN_MENU_CHANNEL_MANAGER_EDIT_FAV_CHANNELS] .assign("Edit Favorite Channel");
   l[_L_MAIN_MENU_CHANNEL_MANAGER_PIP_DUAL_TV]       .assign("PIP - Dual TV");
   l[_L_MAIN_MENU_INSTALLATION]                      .assign("Installation");
   l[_L_MAIN_MENU_INSTALLATION_CHANNEL_SEARCH]       .assign("Channel Search");
   l[_L_MAIN_MENU_INSTALLATION_AUTO_SEARCH]          .assign("Auto Search");
   l[_L_MAIN_MENU_INSTALLATION_MANUAL_SEARCH]        .assign("Manual Search");
   l[_L_MAIN_MENU_INSTALLATION_TRANSPONDER_LIST]     .assign("Transponder List");
   l[_L_MAIN_MENU_INSTALLATION_FACTORY_RESET]        .assign("Factory Reset");
   l[_L_MAIN_MENU_INSTALLATION_DECODER_INFO]         .assign("Decoder Information");
   l[_L_MAIN_MENU_INSTALLATION_VIEWING_CARD]         .assign("Viewing Card");
   l[_L_MAIN_MENU_YOUR_PREFERENCES]                  .assign("Setting");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_LANGUAGE_SETTING] .assign("Language Setting");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_TV_SETTINGS]      .assign("STB Setting");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_PARENTAL_CONTROL] .assign("Parental Control");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_DISPLAY_SETTING]  .assign("Display Setting");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_TIMER_MANAGER]    .assign("Timer Manager");
   l[_L_MAIN_MENU_MAIL_BOX]                          .assign("Mail Box");
   l[_L_MAIN_MENU_UTILITIES]                         .assign("Utilities");
   l[_L_MAIN_MENU_GAMES_GSNAKE]                      .assign("Snake");

   // Utilities
   l[_L_UTILITIES_MEDIA]                         	 .assign("Movie / Music");
   l[_L_UTILITIES_IMAGE]                      		 .assign("Image Viewer");
   l[_L_IMAGE_UNABLE_OPEN_FILE]                      .assign("Unable open this file");
   l[_L_GAMES_HIGHSCORE]                             .assign("High score");
   l[_L_GAMES_SETTING]                               .assign("Settings");
   l[_L_GAMES_GUIDE_SPEED]                           .assign("Game Speed");
   l[_L_GAMES_GUIDE_MODE]                            .assign("Game Mode");
   l[_L_GAMES_GUIDE_MODE_DETAIL_1]                   .assign("Game Mode 1");
   l[_L_GAMES_GUIDE_MODE_DETAIL_2]                   .assign("Game Mode 2");
   l[_L_GAMES_SETTING_SPEED_LOW]                     .assign("Slow");
   l[_L_GAMES_SETTING_SPEED_MED]                     .assign("Normal");
   l[_L_GAMES_SETTING_SPEED_HI]                      .assign("Fast");
   l[_L_GAMES_SETTING_MODE_NONE]                     .assign("Free run");
   l[_L_GAMES_SETTING_MODE_MAPS]                     .assign("Maps");
   l[_L_GAMES_GUIDE_PAUSE]                           .assign("Pause");
   l[_L_GAMES_GUIDE_CHANGE_PAGE]                     .assign("Change page");
   l[_L_GAMES_ERR_NO_RECORD]                         .assign("No new record !");

   l[_L_GAMES_SUDOKU]                                .assign("Sudoku");
   l[_L_GAMES_LEVEL_TITLE]                           .assign("Level : ");
   l[_L_GAMES_TIMER_TITLE]                           .assign("Timer : ");
   l[_L_GAMES_NEW_PUZZLE_TITLE]                      .assign("New Puzzle");
   l[_L_GAMES_EASY]                                  .assign("Easy");
   l[_L_GAMES_MEDIUM]                                .assign("Medium");
   l[_L_GAMES_HARD]                                  .assign("Hard");
   l[_L_GAMES_CLEAR]                                 .assign("Clear");
   l[_L_GAMES_REPLAY]                                .assign("Replay");
   l[_L_GAMES_CONGRATULATION]                        .assign("Congratulation");
   l[_L_GAMES_SOLVED]                                .assign("Game is solved !");

   l[_L_GAMES_LINE]                                  .assign("Line");
   l[_L_GAMES_LINE_NEXT]                             .assign("Next ball");
   l[_L_GAMES_LINE_BALL]                             .assign("Ball :");
   l[_L_GAMES_LINE_SCORE]                            .assign("Score :");
   l[_L_GAMES_LINE_MOVE_SELECT]                      .assign("Select/Move");
   l[_L_GAMES_LINE_GAME_OVER]                        .assign("Game Over");

   //Chinese Chess game
   l[_L_GAMES_CHINESECHESS]                          .assign("Chess");
   l[_L_GAMES_COMPUTER_WIN]                          .assign("Computer Win ");
   l[_L_GAMES_HUMAN_WIN]                             .assign("Human Win ");
   l[_L_GAMES_COMPUTER_MOVE]                         .assign("Computer move ");
   l[_L_GAMES_HUMAN_MOVE]                            .assign("Human move ");

   l[_L_GAMES_SOKOBAN]                               .assign("Sokoban");
   l[_L_GAMES_SOKOBAN_PUSH]                          .assign("Push");
   l[_L_GAMES_SOKOBAN_NEXT_LEVEL]                    .assign("Next Level");
   l[_L_GAMES_SOKOBAN_UNDO]                          .assign("Undo");

   //FormDisplaySetting
   l[_L_FORM_DISPLAY_SETTING_TITLE]                  .assign("Display Setting");
   l[_L_FORM_DISPLAY_SETTING_DISPLAY_TRANSPARENCY]   .assign("Display Transparency");
   l[_L_FORM_DISPLAY_SETTING_CHANNEL_BANNER_TIMEOUT] .assign("Channel Banner Timeout");
   l[_L_FORM_DISPLAY_SETTING_TIME_ZONE]              .assign("Time Zone");
   l[_L_FORM_DISPLAY_SETTING_ALWAYS]                 .assign("Always");

   //FormLanguageSetting
   l[_L_FORM_LANGUAGE_SETTING_LANGUAGE_SETTING_TITLE]      .assign("Language Setting");
   l[_L_FORM_LANGUAGE_SETTING_MENU_LANGUAGE]               .assign("Menu Language");
   l[_L_FORM_LANGUAGE_SETTING_AUDIO_LANGUAGE]              .assign("Audio Language");
   l[_L_FORM_LANGUAGE_SETTING_SUBTITLE]                    .assign("Subtitle");
   l[_L_FORM_LANGUAGE_SETTING_SUBTITLE_LANGUAGE]           .assign("Subtitle Language");
   l[_L_FORM_LANGUAGE_SETTING_VIETNAMESE]                  .assign("Vietnamese");
   l[_L_FORM_LANGUAGE_SETTING_ENGLISH]                     .assign("English");
   l[_L_FORM_LANGUAGE_SETTING_FIRST_LANGUAGE]              .assign("First Language");
   l[_L_FORM_LANGUAGE_SETTING_SECOND_LANGUAGE]             .assign("Second Language");
   l[_L_FORM_LANGUAGE_SETTING_TELETEXT]                    .assign("Teletext");
   l[_L_FORM_LANGUAGE_SETTING_TELETEXT_LANGUAGE]           .assign("Teletext Language");

   //Form Default Setting
   l[_L_FORM_DEFAULT_SETTING_CONFIRM]               .assign("Are you sure ?");
   l[_L_FORM_DEFAULT_SETTING_WARNING]               .assign("All data settings and channels will be lost");
   l[_L_FORM_DEFAULT_SETTING_WAITING]               .assign("Resetting to factory data");

   //Form Letter Box
   l[_L_FORM_LETTER_BOX_TITLE]                      .assign("Mail Box");
   l[_L_FORM_LETTER_BOX_MESS]                       .assign("No message received.");

   //Form Parental control
   l[_L_FORM_PARENTAL_CONTROL_TITLE]                .assign("Parental Control");
   l[_L_FORM_PARENTAL_CONTROL_MORALITY_LEVEL]       .assign("Morality Level");
   l[_L_FORM_PARENTAL_CONTROL_DECODER_LOCK]         .assign("Decoder Lock");
   l[_L_FORM_PARENTAL_CONTROL_CHANNEL_LOCK]         .assign("Channel Lock");
   l[_L_FORM_PARENTAL_CONTROL_CHANGE_PARENTAL_CODE] .assign("Change PIN Code");
   l[_L_FORM_PARENTAL_CONTROL_UNDER_10_AGE]         .assign("Not recommended under 10 age");
   l[_L_FORM_PARENTAL_CONTROL_UNDER_12_AGE]         .assign("Not recommended under 12 age");
   l[_L_FORM_PARENTAL_CONTROL_UNDER_16_AGE]         .assign("Not recommended under 16 age");
   l[_L_FORM_PARENTAL_CONTROL_UNDER_18_AGE]         .assign("Not recommended under 18 age");

   //Form Parental control
   l[_L_FORM_CHANGE_PASSWORD_CREATE_PARENTAL_CODE]  .assign("New PIN");
   l[_L_FORM_CHANGE_PASSWORD_CONFIRM]               .assign("Confirm");

   //Form Notification Change Password
   l[_L_FORM_NOTIFICATION_PASSWORD_CHANGED_SUCCESS] .assign("PIN code changed.");
   l[_L_FORM_NOTIFICATION_PASSWORD_CHANGED_FAIL]    .assign("PIN code mismatched.");

   //Form Authentication of Password
   l[_L_FORM_AUTHEN_PASSWORD_ENTER_PASSWORD]        .assign("Enter Password");
   l[_L_FORM_AUTHEN_PASSWORD_RETRY]                 .assign("Retry");
   l[_L_FORM_AUTHEN_PASSWORD_TITLE_SYSTEM_SETTING]  .assign("System Setting");


   //Form SetTopBox info
   l[_L_FORM_DECODER_INFORMATION_TITLE]             .assign("Decoder Information");
   l[_L_FORM_DECODER_INFORMATION_MODEL_NAME]        .assign("Model Name");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_VERSION]  .assign("Software Version");
   l[_L_FORM_DECODER_INFORMATION_LOADER_VERSION]    .assign("Loader Version");
   l[_L_FORM_DECODER_INFORMATION_DECODER_CA_SN]     .assign("Decoder CA S/N");
   l[_L_FORM_DECODER_INFORMATION_CAK_VERSION]       .assign("CAK Version");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE]    .assign("Software State");

   l[_L_FORM_DECODER_INFORMATION_PROJECT_INFORMATION]   .assign("Project Information");
   l[_L_FORM_DECODER_INFORMATION_DECODER_SERIAL_NUMBER] .assign("Decoder Serial Number");
   l[_L_FORM_DECODER_INFORMATION_USAGE_ID]              .assign("Usage ID");
   l[_L_FORM_DECODER_INFORMATION_LAST_UPDATE]           .assign("Last Update");
   l[_L_FORM_DECODER_INFORMATION_GUIDE_UPDATE_SOFTWARE] .assign("OTA Update Software");
   l[_L_FORM_DECODER_INFORMATION_GUIDE_UPDATE_SOFTWARE_USB]     .assign("USB Update Software");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE_MANUFACTURE]    .assign("Manufacture");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE_DECLARATION]    .assign("Declaration");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE_REGISTRATION]   .assign("Registration");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE_UNKNOWN]        .assign("Unknown");


   //Form TV Settings
   l[_L_FORM_TV_SETTINGS_TITLE]             .assign("STB Settings");
   l[_L_FORM_TV_SETTINGS_TV_STANDARD]       .assign("TV Standard");
   l[_L_FORM_TV_SETTINGS_TV_RESOLUTION]     .assign("TV Resolution");
   l[_L_FORM_TV_SETTINGS_ERR_RECOVERY]      .assign("Error Recovery");
   l[_L_FORM_TV_SETTINGS_SCREEN_RATIO]      .assign("Screen Ratio");
   l[_L_FORM_TV_SETTINGS_ASPECT_CONVERSION] .assign("Wide Mode");
   l[_L_FORM_TV_SETTINGS_DIGITAL_DOLBY]     .assign("Digital Dolby");
   l[_L_FORM_TV_SETTINGS_RF_OUTPUT]         .assign("RF Output");
   l[_L_FORM_TV_SETTINGS_RF_CHANNEL]        .assign("RF Channel");
   l[_L_FORM_TV_SETTINGS_ADJUST_VIDEO]      .assign("Adjust Video");
   l[_L_FORM_TV_SETTINGS_LCN]               .assign("LCN");
   l[_L_FORM_TV_SETTINGS_ANTENPOWER]        .assign("Antenna Power 5V");
   l[_L_FORM_TV_SETTINGS_SHORT_CIRCUIT]     .assign("Failed, please check antenna!");
   l[_L_FORM_TV_SETTINGS_FREQUENCY_MODE]    .assign("Frequency Mode");
   l[_L_FORM_TV_SETTINGS_AUTOSCAN_MODE]     .assign("Auto Scan Mode");
   l[_L_FORM_TV_SETTINGS_CONTROL_VOLUME]    .assign("Intelligent Volume Control");

   l[_L_FORM_HDTV_RESIZE_LETTER_BOX]        .assign("Letter Box");
   l[_L_FORM_HDTV_RESIZE_PAN_SCAN]          .assign("Pan & Scan");
   l[_L_FORM_HDTV_RESIZE_COMBINED]          .assign("Combined");

   //VideoControl
   l[_L_VIDEO_CONTROL_TITLE]                .assign("Video Settings");
   l[_L_VIDEO_CONTROL_BRIGHTNESS]           .assign("Brightness");
   l[_L_VIDEO_CONTROL_COLOR]                .assign("Color");
   l[_L_VIDEO_CONTROL_CONTRAST]             .assign("Contrast");
   l[_L_VIDEO_CONTROL_DISPLAY_TRANSPARENCY] .assign("Display Transparency");
   l[_L_VIDEO_CONTROL_ERR_RECOVERY_HIGH]    .assign("High");
   l[_L_VIDEO_CONTROL_ERR_RECOVERY_FULL]    .assign("Full");

   //FormChannelList
   l[_L_FORM_CHANNEL_LIST_TITLE]                  .assign("Channel List");
   l[_L_FORM_CHANNEL_LIST_TV_ALL]                 .assign("TV - All");
   l[_L_FORM_CHANNEL_LIST_ENTER_NAME]             .assign("Enter name: ");
   l[_L_FORM_CHANNEL_LIST_GUIDE_FREE]             .assign("Free");
   l[_L_FORM_CHANNEL_LIST_COPY]                   .assign("Copy");
   l[_L_FORM_CHANNEL_LIST_PASTE]                  .assign("Paste");
   l[_L_FORM_CHANNEL_LIST_SORT_AND_PASTE]         .assign("Sort / Paste");
   l[_L_FORM_CHANNEL_LIST_OK_EXIT]                .assign("Save / Exit");
   l[_L_FORM_CHANNEL_LIST_EDIT]                   .assign("Rename");
   l[_L_FORM_CHANNEL_LIST_DELETE]                 .assign("Delete");
   l[_L_FORM_CHANNEL_LIST_GUIDE_SCRAMBLE]         .assign("Scramble");
   l[_L_FORM_CHANNEL_LIST_GUIDE_ALL]              .assign("All");
   l[_L_FORM_CHANNEL_LIST_GUIDE_STRING_SEARCH]    .assign("String search");
   l[_L_FORM_CHANNEL_LIST_TITLE_SORT]             .assign("Sort");
   l[_L_FORM_CHANNEL_LIST_BACKUP_RESTORE]         .assign("Export / import channel");
   l[_L_FORM_CHANNEL_LIST_BACKUP_DATA]            .assign("Export channel");
   l[_L_FORM_CHANNEL_LIST_RESTORE_DATA]           .assign("Import channel");
   l[_L_FORM_CHANNEL_LIST_RESTORE_DATA_BY_USB]    .assign("Import channel list from USB");
   l[_L_FORM_CHANNEL_LIST_BACKUP_DATA_DONE]       .assign("Export channel successful");
   l[_L_FORM_CHANNEL_LIST_RESTORE_DATA_DONE]      .assign("Import channel successful");

   l[_L_FORM_CHANNEL_LIST_EDIT_CHANNEL_NUMBER]    .assign("Edit channel number");
   l[_L_FORM_EDIT_CHANNEL_NUMBER_CURRENT]      	  .assign("Current nummber :");
   l[_L_FORM_EDIT_CHANNEL_NUMBER_NEW]    	      .assign("New nummber : ");

   //Form show error lock channel mess
   l[_L_FORM_SHOW_LOCK_CHANNEL_ERROR_MESS1] .assign("To enable this function,");
   l[_L_FORM_SHOW_LOCK_CHANNEL_ERROR_MESS2] .assign("please create your parental code");

   //FormSortChannel
   l[_L_FORM_SORT_CHANNEL_SORT_BY_DEFAULT]  .assign("Sort by default");
   l[_L_FORM_SORT_CHANNEL_SORT_BY_Aa_Zz]    .assign("Sort by Aa - Zz");
   l[_L_FORM_SORT_CHANNEL_SORT_BY_Zz_Aa]    .assign("Sort by Zz - Aa");

   //Form Full Channel List
   l[_L_FORM_FULL_CHANNEL_LIST_LOCK_ERROR_MESS]   .assign("[Channel lock] is inactive");

   //Form Fav List
   l[_L_FORM_FAVORITE_LIST_TITLE]           .assign("Favorite List");
   l[_L_FORM_FAVORITE_LIST_RENAME_ERROR]    .assign("Duplicate group name.");
   l[_L_FORM_FAVORITE_LIST_1]               .assign("Movie");
   l[_L_FORM_FAVORITE_LIST_2]               .assign("Children");
   l[_L_FORM_FAVORITE_LIST_3]               .assign("News");
   l[_L_FORM_FAVORITE_LIST_4]               .assign("Healthy");
   l[_L_FORM_FAVORITE_LIST_5]               .assign("Sport");
   l[_L_FORM_FAVORITE_LIST_6]               .assign("Music");
   l[_L_FORM_FAVORITE_LIST_7]               .assign("Economic");
   l[_L_FORM_FAVORITE_LIST_8]               .assign("Technology");

   //FORM MANAGE FAV CHANNEL
   l[_L_FORM_MANAGE_FAVORITE_CHANNEL_GUIDE_FAV_LIST]     .assign("Favorite List");
   l[_L_FORM_MANAGE_FAVORITE_CHANNEL_GUIDE_RENAME_GROUP] .assign("Rename Group");
   l[_L_FORM_MANAGE_FAVORITE_CHANNEL_ALL_CHANNEL]        .assign("TV All");
   l[_L_FORM_MANAGE_FAVORITE_CHANNEL_ALREADY]            .assign("This channel already exists in the favorite list");

   //FORM TRANSPONDER LIST
   l[_L_FORM_TRANSPONDER_LIST_TITLE]                     .assign("Transponder List");
   l[_L_FORM_TRANSPONDER_LIST_LIST_CHANNEL_TITLE]        .assign("Channel");
   l[_L_FORM_TRANSPONDER_LIST_LIST_EDIT_TRANSPONDER]     .assign("Edit Transponder");
   l[_L_FORM_TRANSPONDER_LIST_LIST_DELETE_TRANSPONDER]   .assign("Delete Transponder");
   l[_L_FORM_TRANSPONDER_LIST_LIST_NEW_TRANSPONDER]      .assign("New Transponder");

   //FORM NEW TRANSPONDER
   l[_L_FORM_NEW_TRANSPONDER_NEW_TP_TITLE]  .assign("New Transponder");
   l[_L_FORM_NEW_TRANSPONDER_EDIT_TP_TITLE] .assign("Edit Transponder");
   l[_L_FORM_NEW_TRANSPONDER_FREQUENCY]     .assign("Frequency");
   l[_L_FORM_NEW_TRANSPONDER_SYMBOL_RATE]   .assign("Symbol Rate");
   l[_L_FORM_NEW_TRANSPONDER_POLARIZATION]  .assign("Polarization");
   l[_L_FORM_NEW_TRANSPONDER_FEC]           .assign("Fec");
   l[_L_FORM_NEW_TRANSPONDER_MODULATION]    .assign("Modulation");
   l[_L_FORM_NEW_TRANSPONDER_TRANSPONDER_EXIST]    .assign("Transponder already exist");

   //FORM CHANNEL SCAN
   l[_L_FORM_CHANNEL_SCAN_TITLE]            .assign("Search");
   l[_L_FORM_CHANNEL_SCAN_TV_CHANNEL]       .assign("TV");
   l[_L_FORM_CHANNEL_SCAN_TV_RADIO]         .assign("Radio");
   l[_L_FORM_CHANNEL_SCAN_CHANNEL_NUM]      .assign("Channel Number: ");
   l[_L_FORM_CHANNEL_SCAN_SEARCH_FAIL]      .assign("No channel found");
   l[_L_FORM_CHANNEL_SCAN_SEARCH_SUCCESS]   .assign("Search Success");
   l[_L_FORM_CHANNEL_SCAN_SEARCH_CONTINUE]  .assign("Press OK to continue.");
   l[_L_FORM_CHANNEL_SCAN_SEARCH_SAVING]    .assign("Successful. Now saving...");

   //FORM CHANNEL SCAN SETTINGS
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_TITLE]            .assign("Channel Search");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_SATELLITE]        .assign("Satellite");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_LNB_FREQUENCY]    .assign("LNB Frequency");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_LNB_POWER]        .assign("LNB Power");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_22KHZ_TONE]       .assign("22 Khz Tone");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_TRANSPONDER_LIST] .assign("Transponder List");
   l[_L_FORM_MANUAL_SCAN_SETTING_TITLE]              .assign("Manual Scan");
   l[_L_FORM_MANUAL_SCAN_SETTING_CHANNEL_NO]         .assign("Channel No");
   l[_L_FORM_SCAN_DVBT_FREQUENCY]                    .assign("DVB-T Scan Frequency: %d.%d MHz, BW: %d MHz");
   l[_L_FORM_SCAN_DVBT2_FREQUENCY]                   .assign("DVB-T2 Scan Frequency: %d.%d MHz, BW: %d MHz");
   l[_L_FORM_SCAN_DVBC_FREQUENCY]                    .assign("DVB-C Scan Frequency: %d.%d MHz, BW: %d MHz");
   l[_L_FORM_MANUAL_SCAN_SETTING_SCAN_FREQUENCY]     .assign("Scan Frequency");
   l[_L_FORM_DVBC_SCAN_SETTINGS_START_FREQUENCY]     .assign("Start Frequency");
   l[_L_FORM_DVBC_SCAN_SETTINGS_END_FREQUENCY]       .assign("End Frequency");
   l[_L_FORM_DVBC_SCAN_SETTINGS_SYMBOL_RATE]         .assign("Symbol Rate");
   l[_L_FORM_DVBC_SCAN_SETTINGS_MODULATION]          .assign("Const.");
   l[_L_FORM_DVBC_SCAN_SETTINGS_NETWORK]             .assign("Network");
   l[_L_FORM_SCAN_SETTINGS_SYMBOLRATE_LIMIT]         .assign("Sym can't be smaller than %d ");

   //FORM TIMER MANAGER
   l[_L_FORM_TIMER_MANAGER_TITLE]                    .assign("Timer Manager");
   l[_L_FORM_TIMER_MANAGER_TIMER_LIST_TITLE]         .assign("Timer List");
   l[_L_FORM_TIMER_MANAGER_TIMER_NO_EVENT]           .assign("No Event");
   l[_L_FORM_TIMER_MANAGER_GUIDE_EDIT_TIMER]         .assign("Edit Timer");
   l[_L_FORM_ADD_TIMER_TITLE]                        .assign("Add Timer");
   l[_L_FORM_ADD_TIMER_CHANNEL]                      .assign("Channel");
   l[_L_FORM_ADD_TIMER_DATE]                         .assign("Date");
   l[_L_FORM_ADD_TIMER_DATE_SUN]                     .assign("Sun");
   l[_L_FORM_ADD_TIMER_DATE_MON]                     .assign("Mon");
   l[_L_FORM_ADD_TIMER_DATE_TUE]                     .assign("Tue");
   l[_L_FORM_ADD_TIMER_DATE_WED]                     .assign("Wed");
   l[_L_FORM_ADD_TIMER_DATE_THU]                     .assign("Thu");
   l[_L_FORM_ADD_TIMER_DATE_FRI]                     .assign("Fri");
   l[_L_FORM_ADD_TIMER_DATE_SAT]                     .assign("Sat");
   l[_L_FORM_ADD_TIMER_START_TIME]                   .assign("Start Time");
   l[_L_FORM_ADD_TIMER_DURATION]                     .assign("Duration");
   l[_L_FORM_ADD_TIMER_SAVE_TIMER]                   .assign("Save Timer");
   l[_L_FORM_DELETE_TIMER_MESS_DELETE_ALL]           .assign("Delete all timers ?");
   l[_L_FORM_DELETE_TIMER_MESS_DELETE_TIMER]         .assign("Delete this timer ?");
   l[_L_FORM_SHOW_TIMER_ERROR_MESS]                  .assign("Start time is invalid.");
   l[_L_FORM_SHOW_TIMER_DURATION_ERROR_MESS]         .assign("Duration is invalid.");
   l[_L_FORM_SHOW_TIMER_DATE_ERROR_MESS]             .assign("Start date is invalid.");
   l[_L_FORM_SHOW_TIMER_DAY_ERROR_MESS]              .assign("Please select a day !");
   l[_L_FORM_SHOW_TIMER_DUPLICATE_TIMER_ERROR_MESS]  .assign("Same timer already exist!");
   l[_L_FORM_SHOW_TIMER_OVERLAPPED_ERROR_MESS]       .assign("Timers are overlapped !");
   l[_L_FORM_POPUP_EXECUTE_ALARM_MODE_TIMER_MESS]    .assign("Channel change will occur in a few seconds.");
   l[_L_FORM_POPUP_EXECUTE_RECORD_MODE_TIMER_MESS]   .assign("Record is going to start in a few seconds.");
   l[_L_FORM_POPUP_EXECUTE_STANDBY_MODE_TIMER_MESS]  .assign("Standby mode will occur in a few seconds.");
   l[_L_FORM_POPUP_EXECUTE_TIMER_SELECT_BUTTON_MESS] .assign("Press cancel button to abort.");
   l[_L_FORM_POPUP_EXECUTE_TIMER_CONFLICT_FREQ_MESS] .assign("Terminate this record ?");

   // FORM UPGRADE FIRMWARE
   l[_L_FORM_UPGRADE_FIRMWARE_TITLE]                  .assign("OTA Update");
   l[_L_FORM_UPGRADE_FIRMWARE_SATELLITE]              .assign("Satellite");
   l[_L_FORM_UPGRADE_FIRMWARE_TRANSPONDER_LIST]       .assign("Transponder List");
   l[_L_FORM_UPGRADE_FIRMWARE_UPDATE_SOFTWARE]        .assign("Update Software");
   l[_L_FORM_UPGRADE_FIRMWARE_CURRENT_VERSION]        .assign("No new software");
   l[_L_FORM_UPGRADE_FIRMWARE_WAITTINGS_USB]          .assign("Power ON, USB is plugged !");
   l[_L_FORM_UPGRADE_FIRMWARE_SUCCESS]                .assign("Completed, restart device !");
   l[_L_FORM_UPGRADE_FIRMWARE_FAILED]                 .assign("Failed, please try again !");
   l[_L_FORM_UPGRADE_FIRMWARE_FAILED_FORMAT]          .assign("File format is not valid !");
   l[_L_FORM_UPGRADE_FIRMWARE_FAILED_SIGN]            .assign("Failed, incorrect signature !");

   //FORM STORAGE INFO
   l[_L_FORM_STORAGE_INFO_TITLE]                            .assign("Storage");
   l[_L_FORM_STORAGE_INFO_INFORM_FULL]                      .assign("Failed, USB is full");
   l[_L_FORM_STORAGE_INFO_INFORM_REC_ERROR_MESSAGE]         .assign("Total failed blocks: %u");
   l[_L_FORM_STORAGE_INFO_INFORM_FILE_OPERATION_INPROGRESS] .assign("File operation in progress !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_MOUNT_USB]          .assign("Failed, USB not found !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_DELETE_FILE]        .assign("Failed to delete file !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_COPY_FILE]          .assign("Failed to copy file !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_RENAME_FILE]        .assign("Failed to rename file !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_STOP_RECORDING]     .assign("Failed to stop recording !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_STOP_PLAYING]       .assign("Failed to stop playing !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_START_PLAYING]      .assign("Failed to start playback !");
   l[_L_FORM_STORAGE_INFO_INFORM_CHANNEL_LOCKED]            .assign("Failed, channel locked !");
   l[_L_FORM_STORAGE_INFO_INFORM_COPY_PLAYABLE_FILE_ONLY]   .assign("Copy playable file only !");
   l[_L_FORM_STORAGE_INFO_INFORM_COPY_FILE_NAME_NOT_VALID]  .assign("File name is not valid !");
   l[_L_FORM_STORAGE_INFO_INFORM_COPY_DONE]                 .assign("Copy done !");
   l[_L_FORM_STORAGE_INFO_INFORM_HOT_UNPLUG_USB]            .assign("USB connection lost");
   l[_L_FORM_STORAGE_INFO_INFORM_FILE_NOT_SUPPORT]          .assign("Format not support !");
   l[_L_FORM_STORAGE_INFO_INFORM_RECORDING_IN_PROGRESS]     .assign("Failed, still recording !");
   l[_L_FORM_STORAGE_INFO_INFORM_CHANNEL_NOT_EXIST]         .assign("Channel is not valid !");

   //FORM CONFIRM MESSAGE
   l[_L_FORM_CONFIRM_TITLE]                           .assign("Warning");
   l[_L_FORM_CONFIRM_EXIT_RECORD_MESSAGE]             .assign("Stop recording ?");
   l[_L_FORM_CONFIRM_EXIT_PLAYBACK_MESSAGE]           .assign("Exit from playback ?");
   l[_L_FORM_CONFIRM_DELETE_FILE_MESSAGE]             .assign("Delete this file ?");
   l[_L_FORM_CONFIRM_RENAME_FILE_MESSAGE]             .assign("Rename this file ?");
   l[_L_FORM_CONFIRM_RENAME_FILE_AGAIN_MESSAGE]       .assign("Failed, Continue rename this file ?");
   l[_L_FORM_CONFIRM_EXIT_GAME_SNAKE]                 .assign("Exit game ?");
   l[_L_FORM_CONFIRM_RESTART_GAME_SNAKE]              .assign("Continue game ?");

   //FORM INPUT TEXT
   l[_L_FORM_INPUT_TITLE]                             .assign("Text box");
   l[_L_FORM_INPUT_STATIC]                            .assign("Old name: ");
   l[_L_FORM_INPUT_DYNAMIC]                           .assign("New name: ");
   l[_L_FORM_INPUT_INFORM_TEXT_LENGTH_MAX]            .assign("Maximum of 13 characters !");

   // FORM EPG ONE CHANNEL
   l[_L_FORM_EPG_ONE_CHANNEL_TITLE]                   .assign("Program Guide");
   l[_L_FORM_EPG_ONE_CHANNEL_GUIDE_EPG_TYPE]          .assign("EPG Type Select");
   l[_L_FORM_EPG_ONE_CHANNEL_GUIDE_SAVE_TIMER]        .assign("Save Timer");
   l[_L_FORM_EPG_ONE_CHANNEL_GUIDE_NEXT_DAY]          .assign("Next Day");
   l[_L_FORM_EPG_ONE_CHANNEL_GUIDE_PREVIOUS_DAY]      .assign("Previous");
   l[_L_FORM_EPG_ONE_CHANNEL_WARNING_NO_EVENT]        .assign("No EPG data");

   // FORM EPG ONE CHANNEL
   l[_L_FORM_EPG_FULL_TITLE]                          .assign("Program Guide");

   //FORM EPG FULL_TIMER MANAGER
   l[_L_FORM_EPG_FULL_TIMER_MANAGER_TITLE]            .assign("Reserve");

   //KEYBOARD
   l[_L_KEYBOARD_TITLE]                               .assign("Keyboard");

   //FORM_REC_PLAYBACK
   l[_L_FORM_PLAYBACK_FILE_LIST_TITLE]                .assign("Playback List");

   //REBOOT
   l[_L_REBOOT]                                       .assign("Device is restarting ...");

   //FORM STARTUP
   l[_L_FORM_STARTUP_MESSSAGE]                        .assign("Great Bear Stars");
   l[_L_FORM_STARTUP_GOTO_SCAN_CHANNEL]               .assign("STB is going to scan channel automatically ...");

   //FORM INFOCHANNEL
   l[_L_FORM_INFO_CHANNEL_TITTLE]                     .assign("Channel Information");
   l[_L_FORM_INFO_CHANNEL_NAME]                       .assign("Channel Name");
   l[_L_FORM_INFO_CHANNEL_FREQ]                       .assign("Frequency");
   l[_L_FORM_INFO_CHANNEL_NUMBER]                     .assign("Channel Number");
   l[_L_FORM_INFO_CHANNEL_SERVICE_ID]                 .assign("Service ID");
   l[_L_FORM_INFO_CHANNEL_NETWORK_ID]                 .assign("Network ID");
   l[_L_FORM_INFO_CHANNEL_TUNER_STATUS]               .assign("Tuner Status");
   l[_L_FORM_INFO_CHANNEL_TUNER_NONE]                 .assign("NONE");
   l[_L_FORM_INFO_CHANNEL_TUNER_UNLOCKED]             .assign("UNLOCKED");
   l[_L_FORM_INFO_CHANNEL_TUNER_LOCKED]               .assign("LOCKED");
   l[_L_FORM_INFO_CHANNEL_TUNER_IDLE]                 .assign("IDLE");
   l[_L_FORM_INFO_CHANNEL_TUNER_STANDBY]              .assign("STANDBY");
   l[_L_FORM_INFO_CHANNEL_TUNER_SCANNING]             .assign("SCANNING");

   //FORM PIP CONTROL
   l[_L_FORM_PIP_CONTROL_TITTLE]                      .assign("PIP Control");
   l[_L_FORM_PIP_CONTROL_CHANN]                       .assign("Open PIP");
   l[_L_FORM_PIP_CONTROL_CLOSE]                       .assign("Close PIP");
   l[_L_FORM_PIP_CONTROL_OUTPUT]                      .assign("Output");
   l[_L_FORM_PIP_CONTROL_POS]                         .assign("Position");
   l[_L_FORM_PIP_CONTROL_SWAP]                        .assign("Swap view");
   l[_L_FORM_PIP_OUTPUT_HDMI]                         .assign("HDMI");
   l[_L_FORM_PIP_OUTPUT_AV]                           .assign("AV");
   l[_L_FORM_PIP_OUTPUT_ALL]                          .assign("All");
   l[_L_FORM_PIP_CONTROL_POS_TOPLEFT]                 .assign("Top - Left");
   l[_L_FORM_PIP_CONTROL_POS_TOPRIGHT]                .assign("Top - Right");
   l[_L_FORM_PIP_CONTROL_POS_BOTLEFT]                 .assign("Bottom - Left");
   l[_L_FORM_PIP_CONTROL_POS_BOTRIGHT]                .assign("Bottom - Right");
   l[_L_FORM_ERROR_PIP_IN_PROGRESS]                   .assign("Failed, PIP in progress !");
   l[_L_FORM_PIP_CONTROL_SWAP_WARNING_PLAYBACK]       .assign("Can not swap view");
   l[_L_FORM_PIP_CONTROL_OPEN_PLAYING_CHANNEL]        .assign("Channel is playing in main TV");
   l[_L_FORM_PIP_CONTROL_OPEN_PIP_CHANNEL]            .assign("Channel is playing in PIP");
   l[_L_FORM_PIP_CONTROL_NO_SIGNAL_FOUND]             .assign("No signal found!");
   l[_L_FORM_PIP_CONTROL_OPEN_AUDIO_CHANNEL]          .assign("Can't open a radio channel!");
   l[_L_FORM_PIP_CONTROL_SWAP_AUDIO_CHANNEL]          .assign("Can't swap a radio channel!");
   l[_L_FORM_PIP_CONTROL_DUAL_TV]                     .assign("Open Dual TV");
   l[_L_FORM_PIP_CONTROL_CLOSE_DUAL_TV]               .assign("Close Dual TV");
   l[_L_FORM_ERROR_DUAL_TV_IN_PROGRESS]               .assign("Failed, Dual TV in progress !");

   //FORM_CHANNEL_LOCK_CONTROL
   l[_L_FORM_CHANNEL_LOCK_CONTROL_TITTLE]             .assign("Lock Control");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_TYPE]               .assign("Lock Type");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_START]              .assign("Start");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_END]                .assign("End");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_LOCK]               .assign("Lock");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_UNLOCK]             .assign("Unlock");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_ALL]                .assign("All Time");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_EDIT]               .assign("Edit");

   //FRONT PANEL
   l[_L_FRONT_PANEL_POWER_OFF]                        .assign("On / Off");
   l[_L_FRONT_PANEL_ADJUST_VOLUME]                    .assign("Volume");
   l[_L_FRONT_PANEL_SCAN_AUTO]                        .assign("Scan Auto");

   //DVR PLAYING
   l[_L_PLAYING_SPEED_UP]                             .assign("1X");
   l[_L_PLAYING_SPEED_DOWN]                           .assign("-1X");
   l[_L_PLAYING_CHANGE_LOOP]                          .assign("Loop");
   l[_L_OPEN_PLAYBACK_LIST]                           .assign("Select file");

   //DEMO VERSION
   l[_L_DEMO_VERSION]                                 .assign("GBS-HD DEMO VERSION");
   l[_L_DEMO_VERSION_TIME_REACHED]                    .assign("Demo expired, STB is stopping!");
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* G_L_EN_HPP_ */
