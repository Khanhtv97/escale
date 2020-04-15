//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GLang_VN.hpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GLANG_VN_HPP_
#define GLANG_VN_HPP_

using namespace std;
#include "GLanguage.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////

void GLanguage::Initialize_VN(string* l)
{
   l[_L_OK]      .assign("DDo^`ng y'");
   l[_L_CANCEL]  .assign("Hu?y bo?");
   l[_L_CANCEL1] .assign("Tu+` cho^'i");
   l[_L_YES]     .assign("DDo^`ng y'");
   l[_L_NO]      .assign("Kho^ng");
   l[_L_DENY]    .assign("Tu+` cho^'i");
   l[_L_DENY1]   .assign("Tu+` cho^'i");
   l[_L_IGNORE]  .assign("Bo? qua");
   l[_L_DVBS]    .assign("Truye^`n Hi`nh So^' - Ve^. Tinh");
   l[_L_GUIDE_BACK_TO_MENU] .assign("Quay la.i");
   l[_L_ON]      .assign("Ba^.t");
   l[_L_OFF]     .assign("Ta('t");
   l[_L_LOCK]    .assign("Kho'a");
   l[_L_UNLOCK]  .assign("Mo+?");
   l[_L_AUTO]    .assign("Tu+. ddo^.ng");
   l[_L_RESET]   .assign("Ma(.c ddi.nh");
   l[_L_WATTING] .assign("Vui lo`ng ddo+.i trong gia^y la't .");
   l[_L_SAVE]    .assign("Lu+u la.i");
   l[_L_SAVING]  .assign("DDang ghi ...");
   l[_L_LOCKUNLOCK] .assign("Kho'a / Mo+?");
   l[_L_DEFAULT] .assign("Ma(.c ddi.nh");
   l[_L_PRESS_OK] .assign("Ba^'m phi'm OK");

   //GUIDE
   l[_L_GUIDE_DELETE]     .assign("Xo'a");
   l[_L_GUIDE_DELETE_ALL] .assign("Xo'a ta^'t ca?");
   l[_L_GUIDE_SPACE]      .assign("Da^'u ca'ch");
   l[_L_GUIDE_EXIT]       .assign("Thoa't");
   l[_L_GUIDE_SORT]       .assign("Sa('p xe^'p");
   l[_L_GUIDE_SORT_AZ]    .assign("Sa('p xe^'p A-Z");
   l[_L_GUIDE_SORT_ZA]    .assign("Sa('p xe^'p Z-A");
   l[_L_GUIDE_FIND]       .assign("Ti`m kie^'m");
   l[_L_GUIDE_ADD]        .assign("The^m");
   l[_L_GUIDE_MOVE]       .assign("Di chuye^?n");
   l[_L_GUIDE_ADD_SYMBOL] .assign("The^m bie^?u tu+o+.ng");

   //COMMON
   l[_L_CYCLE]            .assign("Chu ky`");
   l[_L_CYCLE_ONCE]       .assign("Mo^.t la^`n");
   l[_L_CYCLE_DAILY]      .assign("Ha`ng nga`y");
   l[_L_CYCLE_WEEKLY]     .assign("Ha`ng tua^`n");
   l[_L_MODE]             .assign("Che^' ddo^.");
   l[_L_MODE_ALARM]       .assign("Ba^.t");
   l[_L_MODE_STANDBY]     .assign("Ta('t");
   l[_L_MODE_RECORD]      .assign("Ghi");
   l[_L_MODE_NOT_DEFINED] .assign("Kho^ng xa'c ddi.nh");
   l[_L_NO_CHANNEL]       .assign("Kho^ng co' ke^nh");
   l[_L_NO_SIGNAL]        .assign("Kho^ng co' ti'n hie^.u");
   l[_L_NO_AUDIO]         .assign("Kho^ng co' ti'n hie^.u audio");
   l[_L_SCRAMBLE_CHANNEL] .assign("Ke^nh kho'a ma~");
   l[_L_SIGNAL_NOT_GOOD]  .assign("Cha^'t lu+o+.ng T/H tha^'p");

   //MAIN MENU
   l[_L_MAIN_MENU_CHANNEL_MANAGER]                   .assign("Qua?n ly' ke^nh");
   l[_L_MAIN_MENU_CHANNEL_MANAGER_CHANNEL_LIST]      .assign("Danh sa'ch ke^nh");
   l[_L_MAIN_MENU_CHANNEL_MANAGER_EDIT_FAV_CHANNELS] .assign("Thay ddo^?i ke^nh u+a thi'ch");
   l[_L_MAIN_MENU_CHANNEL_MANAGER_PIP_DUAL_TV]       .assign("Xem ddo^`ng tho+`i hai ke^nh");
   l[_L_MAIN_MENU_INSTALLATION]                      .assign("Ca`i dda(.t ke^nh");
   l[_L_MAIN_MENU_INSTALLATION_CHANNEL_SEARCH]       .assign("Ti`m kie^'m ke^nh");
   l[_L_MAIN_MENU_INSTALLATION_AUTO_SEARCH]          .assign("Do` ke^nh tu+. ddo^.ng");
   l[_L_MAIN_MENU_INSTALLATION_MANUAL_SEARCH]        .assign("Do` ke^nh ba(`ng tay");
   l[_L_MAIN_MENU_INSTALLATION_TRANSPONDER_LIST]     .assign("Danh sa'ch TP");
   l[_L_MAIN_MENU_INSTALLATION_FACTORY_RESET]        .assign("Xa'c la^.p ma(.c ddi.nh");
   l[_L_MAIN_MENU_INSTALLATION_DECODER_INFO]         .assign("Tho^ng tin dda^`u thu");
   l[_L_MAIN_MENU_INSTALLATION_VIEWING_CARD]         .assign("The?");
   l[_L_MAIN_MENU_YOUR_PREFERENCES]                  .assign("Ca`i dda(.t rie^ng");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_LANGUAGE_SETTING] .assign("Ca`i dda(.t ngo^n ngu+~");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_TV_SETTINGS]      .assign("Ca`i dda(.t dda^`u thu");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_PARENTAL_CONTROL] .assign("Kie^?m soa't ngu+o+`i du`ng");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_DISPLAY_SETTING]  .assign("Ca`i dda(.t hie^?n thi.");
   l[_L_MAIN_MENU_YOUR_PREFERENCES_TIMER_MANAGER]    .assign("Qua?n ly' ba'o gio+`");
   l[_L_MAIN_MENU_MAIL_BOX]                          .assign("Ho^.p thu+");
   l[_L_MAIN_MENU_UTILITIES]                         .assign("Tie^.n  i'ch");
   l[_L_MAIN_MENU_GAMES_GSNAKE]                      .assign("Ra('n sa(n mo^`i");

   // Utilities
   l[_L_UTILITIES_MEDIA]                         	 .assign("Xem phim / Nghe nha.c");
   l[_L_UTILITIES_IMAGE]                      		 .assign("Thu+ vie^.n a?nh");
   l[_L_IMAGE_UNABLE_OPEN_FILE]                      .assign("Kho^ng mo+? ddu+o+.c file na`y");
   l[_L_GAMES_HIGHSCORE]                             .assign("DDie^?m cao");
   l[_L_GAMES_SETTING]                               .assign("Ca`i dda(.t");
   l[_L_GAMES_GUIDE_SPEED]                           .assign("To^'c ddo^.");
   l[_L_GAMES_GUIDE_MODE]                            .assign("Che^' ddo^.");
   l[_L_GAMES_GUIDE_MODE_DETAIL_1]                   .assign("Che^' ddo^. 1");
   l[_L_GAMES_GUIDE_MODE_DETAIL_2]                   .assign("Che^' ddo^. 2");
   l[_L_GAMES_SETTING_SPEED_LOW]                     .assign("Cha^.m");
   l[_L_GAMES_SETTING_SPEED_MED]                     .assign("Bi`nh thu+o+`ng");
   l[_L_GAMES_SETTING_SPEED_HI]                      .assign("Nhanh");
   l[_L_GAMES_SETTING_MODE_NONE]                     .assign("Tu+. do");
   l[_L_GAMES_SETTING_MODE_MAPS]                     .assign("Ba?n ddo^`");
   l[_L_GAMES_GUIDE_PAUSE]                           .assign("Ta.m du+`ng");
   l[_L_GAMES_GUIDE_CHANGE_PAGE]                     .assign("Chuye^?n trang");
   l[_L_GAMES_ERR_NO_RECORD]                         .assign("Chu+a co' ddie^?m cao !");

   l[_L_GAMES_SUDOKU]                                .assign("Sudoku");
   l[_L_GAMES_LEVEL_TITLE]                           .assign("Ca^'p ddo^. : ");
   l[_L_GAMES_TIMER_TITLE]                           .assign("Tho+`i gian : ");
   l[_L_GAMES_NEW_PUZZLE_TITLE]                      .assign("Tro` cho+i mo+'i");
   l[_L_GAMES_EASY]                                  .assign("De^~");
   l[_L_GAMES_MEDIUM]                                .assign("Vu+`a");
   l[_L_GAMES_HARD]                                  .assign("Kho'");
   l[_L_GAMES_CLEAR]                                 .assign("Xo'a");
   l[_L_GAMES_REPLAY]                                .assign("Cho+i la.i");
   l[_L_GAMES_CONGRATULATION]                        .assign("Chu'c mu+`ng");
   l[_L_GAMES_SOLVED]                                .assign("Tro` cho+i dda~ ddu+o+.c gia?i !");

   l[_L_GAMES_LINE]                                  .assign("Line");
   l[_L_GAMES_LINE_NEXT]                             .assign("Tie^'p theo");
   l[_L_GAMES_LINE_BALL]                             .assign("Bo'ng :");
   l[_L_GAMES_LINE_SCORE]                            .assign("DDie^?m :");
   l[_L_GAMES_LINE_MOVE_SELECT]                      .assign("Cho.n/Di chuye^?n");
   l[_L_GAMES_LINE_GAME_OVER]                        .assign("Ke^'t thu'c");

   l[_L_GAMES_CHINESECHESS]                          .assign("Co+` Tu+o+'ng");
   l[_L_GAMES_COMPUTER_WIN]                          .assign("Ma'y tha('ng ");
   l[_L_GAMES_HUMAN_WIN]                             .assign("Ba.n dda~ tha('ng ");
   l[_L_GAMES_COMPUTER_MOVE]                         .assign("Ma'y di chuye^?n ");
   l[_L_GAMES_HUMAN_MOVE]                            .assign("Ba.n di chuye^?n ");

   l[_L_GAMES_SOKOBAN]                               .assign("DDa^?y ho^.p");
   l[_L_GAMES_SOKOBAN_PUSH]                          .assign("DDa^?y");
   l[_L_GAMES_SOKOBAN_NEXT_LEVEL]                    .assign("Ba`i tie^'p theo");
   l[_L_GAMES_SOKOBAN_UNDO]                          .assign("DDi la.i");

   //FormDisplaySetting
   l[_L_FORM_DISPLAY_SETTING_TITLE]                  .assign("Ca`i dda(.t hie^?n thi.");
   l[_L_FORM_DISPLAY_SETTING_DISPLAY_TRANSPARENCY]   .assign("DDo^. hie^?n thi. trong suo^'t");
   l[_L_FORM_DISPLAY_SETTING_CHANNEL_BANNER_TIMEOUT] .assign("Tho+`i gian hie^?n thi.");
   l[_L_FORM_DISPLAY_SETTING_TIME_ZONE]              .assign("Mu'i gio+`");
   l[_L_FORM_DISPLAY_SETTING_ALWAYS]                 .assign("Luo^n luo^n");

   //FormLanguageSetting
   l[_L_FORM_LANGUAGE_SETTING_LANGUAGE_SETTING_TITLE] .assign("Ca`i dda(.t ngo^n ngu+~");
   l[_L_FORM_LANGUAGE_SETTING_MENU_LANGUAGE]          .assign("Ngo^n ngu+~ hie^?n thi.");
   l[_L_FORM_LANGUAGE_SETTING_AUDIO_LANGUAGE]         .assign("Ngo^n ngu+~ tie^'ng");
   l[_L_FORM_LANGUAGE_SETTING_SUBTITLE]               .assign("Phu. dde^`");
   l[_L_FORM_LANGUAGE_SETTING_SUBTITLE_LANGUAGE]      .assign("Ngo^n ngu+~ phu. dde^`");
   l[_L_FORM_LANGUAGE_SETTING_VIETNAMESE]             .assign("Vie^.t Nam");
   l[_L_FORM_LANGUAGE_SETTING_ENGLISH]                .assign("Anh");
   l[_L_FORM_LANGUAGE_SETTING_FIRST_LANGUAGE]         .assign("Ngo^n ngu+~ thu+' nha^'t");
   l[_L_FORM_LANGUAGE_SETTING_SECOND_LANGUAGE]        .assign("Ngo^n ngu+~ thu+' hai");
   l[_L_FORM_LANGUAGE_SETTING_TELETEXT]               .assign("Va(n ba?n");
   l[_L_FORM_LANGUAGE_SETTING_TELETEXT_LANGUAGE]      .assign("Ngo^n ngu+~ va(n ba?n");

   //Form Default Setting
   l[_L_FORM_DEFAULT_SETTING_CONFIRM]                 .assign("Ba.n co' ddo^`ng y' kho^ng ?");
   l[_L_FORM_DEFAULT_SETTING_WARNING]                 .assign("Ta^'t ca? tho^ng tin ve^` ke^nh va` ca`i dda(.t cu?a ba.n se~ ma^'t");
   l[_L_FORM_DEFAULT_SETTING_WAITING]                 .assign("DDa^`u thu se~ tu+. kho+?i ddo^.ng la.i");

   //Form Letter Box
   l[_L_FORM_LETTER_BOX_TITLE]                        .assign("Ho^.p thu+");
   l[_L_FORM_LETTER_BOX_MESS]                         .assign("Kho^ng nha^.n ddu+o+.c tin nha('n na`o");

   //Form Parental control
   l[_L_FORM_PARENTAL_CONTROL_TITLE]                  .assign("Kie^?m soa't ngu+o+`i du`ng");
   l[_L_FORM_PARENTAL_CONTROL_MORALITY_LEVEL]         .assign("Khuye^'n ca'o tre? em");
   l[_L_FORM_PARENTAL_CONTROL_DECODER_LOCK]           .assign("Kho'a dda^`u thu");
   l[_L_FORM_PARENTAL_CONTROL_CHANNEL_LOCK]           .assign("Kho'a ke^nh");
   l[_L_FORM_PARENTAL_CONTROL_CHANGE_PARENTAL_CODE]   .assign("Thay ddo^?i ma~ PIN");
   l[_L_FORM_PARENTAL_CONTROL_UNDER_10_AGE]           .assign("Du+o+'i 10 tuo^?i");
   l[_L_FORM_PARENTAL_CONTROL_UNDER_12_AGE]           .assign("Du+o+'i 12 tuo^?i");
   l[_L_FORM_PARENTAL_CONTROL_UNDER_16_AGE]           .assign("Du+o+'i 16 tuo^?i");
   l[_L_FORM_PARENTAL_CONTROL_UNDER_18_AGE]           .assign("Du+o+'i 18 tuo^?i");

   //Form Parental control
   l[_L_FORM_CHANGE_PASSWORD_CREATE_PARENTAL_CODE]    .assign("Ma~ mo+'i");
   l[_L_FORM_CHANGE_PASSWORD_CONFIRM]                 .assign("Xa'c nha^.n");

   //Form Notification Change Password
   l[_L_FORM_NOTIFICATION_PASSWORD_CHANGED_SUCCESS]   .assign("Ma~ PIN dda~ thay ddo^?i");
   l[_L_FORM_NOTIFICATION_PASSWORD_CHANGED_FAIL]      .assign("Ma~ PIN nha^.p kho^ng ddu'ng.");

   //Form Notification Change Password
   l[_L_FORM_AUTHEN_PASSWORD_ENTER_PASSWORD]          .assign("Nha^.p ma^.t kha^?u");
   l[_L_FORM_AUTHEN_PASSWORD_RETRY]                   .assign("Thu+? la.i");
   l[_L_FORM_AUTHEN_PASSWORD_TITLE_SYSTEM_SETTING]    .assign("Ca`i dda(.t he^. tho^'ng");

   //Form SetTopBox info
   l[_L_FORM_DECODER_INFORMATION_TITLE]               .assign("Tho^ng tin dda^`u thu");
   l[_L_FORM_DECODER_INFORMATION_MODEL_NAME]          .assign("Te^n model");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_VERSION]    .assign("Phie^n ba?n pha^`n me^`m");
   l[_L_FORM_DECODER_INFORMATION_LOADER_VERSION]      .assign("Phie^n ba?n na^ng ca^'p");
   l[_L_FORM_DECODER_INFORMATION_DECODER_CA_SN]       .assign("Decoder CA S/N");
   l[_L_FORM_DECODER_INFORMATION_CAK_VERSION]         .assign("Phie^n ba?n CAK");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE]      .assign("Tra.ng tha'i pha^`n me^`m");
   l[_L_FORM_DECODER_INFORMATION_PROJECT_INFORMATION] .assign("Tho^ng tin du+. a'n");
   l[_L_FORM_DECODER_INFORMATION_DECODER_SERIAL_NUMBER] .assign("So^' se^ ri dda^`u thu");
   l[_L_FORM_DECODER_INFORMATION_USAGE_ID]              .assign("Usage ID");
   l[_L_FORM_DECODER_INFORMATION_LAST_UPDATE]           .assign("Na^ng ca^'p mo+'i nha^'t");
   l[_L_FORM_DECODER_INFORMATION_GUIDE_UPDATE_SOFTWARE] .assign("Na^ng ca^'p pha^`n me^`m qua OTA");
   l[_L_FORM_DECODER_INFORMATION_GUIDE_UPDATE_SOFTWARE_USB] .assign("Na^ng ca^'p pha^`n me^`m qua USB");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE_MANUFACTURE]    .assign("Manufacture");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE_DECLARATION]    .assign("Declaration");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE_REGISTRATION]   .assign("Registration");
   l[_L_FORM_DECODER_INFORMATION_SOFTWARE_STATE_UNKNOWN]        .assign("Unknown");

   //Form TV Settings
   l[_L_FORM_TV_SETTINGS_TITLE]             .assign("Ca`i dda(.t dda^`u thu");
   l[_L_FORM_TV_SETTINGS_TV_STANDARD]       .assign("Tie^u chua^?n TV");
   l[_L_FORM_TV_SETTINGS_TV_RESOLUTION]     .assign("DDo^. pha^n gia?i TV");
   l[_L_FORM_TV_SETTINGS_ERR_RECOVERY]      .assign("Gia?i ma~ ti'n hie^.u lo^~i");
   l[_L_FORM_TV_SETTINGS_SCREEN_RATIO]      .assign("Ty? le^. ma`n hi`nh");
   l[_L_FORM_TV_SETTINGS_ASPECT_CONVERSION] .assign("Che^' ddo^. ma`n hi`nh");
   l[_L_FORM_TV_SETTINGS_DIGITAL_DOLBY]     .assign("A^m thanh no^?i");
   l[_L_FORM_TV_SETTINGS_RF_OUTPUT]         .assign("Ti'n hie^.u RF ra");
   l[_L_FORM_TV_SETTINGS_RF_CHANNEL]        .assign("Ke^nh RF");
   l[_L_FORM_TV_SETTINGS_ADJUST_VIDEO]      .assign("DDie^`u chi?nh Video");
   l[_L_FORM_TV_SETTINGS_LCN]               .assign("DDa'nh so^' ke^nh theo LCN");
   l[_L_FORM_TV_SETTINGS_ANTENPOWER]        .assign("Nguo^`n 5V cho A(ngten");
   l[_L_FORM_TV_SETTINGS_SHORT_CIRCUIT]     .assign("Co' lo^~i, kie^?m tra la.i A(ngten!");
   l[_L_FORM_TV_SETTINGS_FREQUENCY_MODE]    .assign("Che^' ddo^. ta^`n so^'");
   l[_L_FORM_TV_SETTINGS_AUTOSCAN_MODE]     .assign("Che^' ddo^. do` ke^nh tu+. ddo^.ng");
   l[_L_FORM_TV_SETTINGS_CONTROL_VOLUME]    .assign("DDie^`u chi?nh a^m lu+o+.ng tho^ng minh");

   l[_L_FORM_HDTV_RESIZE_LETTER_BOX]        .assign("Ho^.p thu+");
   l[_L_FORM_HDTV_RESIZE_PAN_SCAN]          .assign("Pan & Scan");
   l[_L_FORM_HDTV_RESIZE_COMBINED]          .assign("Ke^'t ho+.p");

   //VideoControl
   l[_L_VIDEO_CONTROL_TITLE]                .assign("DDie^`u chi?nh Video");
   l[_L_VIDEO_CONTROL_BRIGHTNESS]           .assign("DDo^. sa'ng");
   l[_L_VIDEO_CONTROL_COLOR]                .assign("Ma`u sa('c");
   l[_L_VIDEO_CONTROL_CONTRAST]             .assign("DDo^. tu+o+ng pha?n");
   l[_L_VIDEO_CONTROL_DISPLAY_TRANSPARENCY] .assign("DDo^. hie^?n thi. trong suo^'t");
   l[_L_VIDEO_CONTROL_ERR_RECOVERY_HIGH]    .assign("Ba^.t");
   l[_L_VIDEO_CONTROL_ERR_RECOVERY_FULL]    .assign("Ta('t");

   //FormChannelList
   l[_L_FORM_CHANNEL_LIST_TITLE]               .assign("Danh sa'ch ke^nh");
   l[_L_FORM_CHANNEL_LIST_TV_ALL]              .assign("TV - Ta^'t ca?");
   l[_L_FORM_CHANNEL_LIST_ENTER_NAME]          .assign("Nha^.p te^n: ");
   l[_L_FORM_CHANNEL_LIST_GUIDE_FREE]          .assign("Ke^nh tu+. do");
   l[_L_FORM_CHANNEL_LIST_COPY]                .assign("Sao che'p");
   l[_L_FORM_CHANNEL_LIST_PASTE]               .assign("Da'n");
   l[_L_FORM_CHANNEL_LIST_SORT_AND_PASTE]      .assign("Sa('p xe^'p / Da'n");
   l[_L_FORM_CHANNEL_LIST_OK_EXIT]             .assign("Lu+u / Thoa't");
   l[_L_FORM_CHANNEL_LIST_EDIT]                .assign("Su+?a te^n");
   l[_L_FORM_CHANNEL_LIST_DELETE]              .assign("Xo'a file");
   l[_L_FORM_CHANNEL_LIST_GUIDE_SCRAMBLE]      .assign("Ke^nh kho'a ma~");
   l[_L_FORM_CHANNEL_LIST_GUIDE_ALL]           .assign("Ta^'t ca?");
   l[_L_FORM_CHANNEL_LIST_GUIDE_STRING_SEARCH] .assign("Ti`m theo te^n");
   l[_L_FORM_CHANNEL_LIST_TITLE_SORT]          .assign("Sa('p xe^'p");
   l[_L_FORM_CHANNEL_LIST_BACKUP_RESTORE]      .assign("Sao lu+u / phu.c ho^`i ke^nh");
   l[_L_FORM_CHANNEL_LIST_BACKUP_DATA]         .assign("Sao lu+u ke^nh");
   l[_L_FORM_CHANNEL_LIST_RESTORE_DATA]        .assign("Phu.c ho^`i ke^nh");
   l[_L_FORM_CHANNEL_LIST_RESTORE_DATA_BY_USB] .assign("Phu.c ho^`i danh sa'ch ke^nh tu+` USB");
   l[_L_FORM_CHANNEL_LIST_BACKUP_DATA_DONE]    .assign("Sao lu+u ke^nh tha`nh co^ng");
   l[_L_FORM_CHANNEL_LIST_RESTORE_DATA_DONE]   .assign("Phu.c ho^`i ke^nh tha`nh co^ng");

   l[_L_FORM_CHANNEL_LIST_EDIT_CHANNEL_NUMBER] .assign("Su+?a so^' ke^nh");
   l[_L_FORM_EDIT_CHANNEL_NUMBER_CURRENT]      .assign("So^' ke^nh hie^.n ta.i :");
   l[_L_FORM_EDIT_CHANNEL_NUMBER_NEW]    	   .assign("So^' ke^nh mo+'i : ");

   //Form show error lock channel mess
   l[_L_FORM_SHOW_LOCK_CHANNEL_ERROR_MESS1]  .assign("DDe^? ki'ch hoa.t chu+'c na(ng na`y,");
   l[_L_FORM_SHOW_LOCK_CHANNEL_ERROR_MESS2]  .assign("ba.n ca^`n dda(.t ma^.t kha^?u");

   //FormSortChannel
   l[_L_FORM_SORT_CHANNEL_SORT_BY_DEFAULT]   .assign("Sa('p xe^'p ma(.c ddi.nh");
   l[_L_FORM_SORT_CHANNEL_SORT_BY_Aa_Zz]     .assign("Sa('p xe^'p A to+'i Z");
   l[_L_FORM_SORT_CHANNEL_SORT_BY_Zz_Aa]     .assign("Sa('p xe^'p Z to+'i A");

   //Form Full Channel List
   l[_L_FORM_FULL_CHANNEL_LIST_LOCK_ERROR_MESS] .assign("[Kho'a ke^nh] chu+a ki'ch hoa.t");

   //Form Fav List
   l[_L_FORM_FAVORITE_LIST_TITLE]            .assign("Nho'm u+a thi'ch");
   l[_L_FORM_FAVORITE_LIST_RENAME_ERROR]     .assign("Tru`ng te^n nho'm kha'c.");
   l[_L_FORM_FAVORITE_LIST_1]                .assign("Phim truye^.n");
   l[_L_FORM_FAVORITE_LIST_2]                .assign("Tre? em");
   l[_L_FORM_FAVORITE_LIST_3]                .assign("Tin tu+'c");
   l[_L_FORM_FAVORITE_LIST_4]                .assign("So^'ng khoe?");
   l[_L_FORM_FAVORITE_LIST_5]                .assign("The^? thao");
   l[_L_FORM_FAVORITE_LIST_6]                .assign("A^m nha.c");
   l[_L_FORM_FAVORITE_LIST_7]                .assign("Kinh te^'");
   l[_L_FORM_FAVORITE_LIST_8]                .assign("Co^ng nghe^.");

   //FORM MANAGE FAV CHANNEL
   l[_L_FORM_MANAGE_FAVORITE_CHANNEL_GUIDE_FAV_LIST]     .assign("Danh sa'ch u+a thi'ch");
   l[_L_FORM_MANAGE_FAVORITE_CHANNEL_GUIDE_RENAME_GROUP] .assign("DDo^?i nho'm te^n");
   l[_L_FORM_MANAGE_FAVORITE_CHANNEL_ALL_CHANNEL]        .assign("Ta^'t ca? ke^nh");
   l[_L_FORM_MANAGE_FAVORITE_CHANNEL_ALREADY]            .assign("Ke^nh na`y dda~ co' trong danh sa'ch ye^u thi'ch");

   //FORM TRANSPONDER LIST
   l[_L_FORM_TRANSPONDER_LIST_TITLE]                   .assign("Danh sa'ch TP");
   l[_L_FORM_TRANSPONDER_LIST_LIST_CHANNEL_TITLE]      .assign("Ke^nh");
   l[_L_FORM_TRANSPONDER_LIST_LIST_EDIT_TRANSPONDER]   .assign("Thay ddo^?i TP");
   l[_L_FORM_TRANSPONDER_LIST_LIST_DELETE_TRANSPONDER] .assign("Xo'a TP");
   l[_L_FORM_TRANSPONDER_LIST_LIST_NEW_TRANSPONDER]    .assign("The^m mo+'i TP");

   //FORM NEW TRANSPONDER
   l[_L_FORM_NEW_TRANSPONDER_NEW_TP_TITLE]  .assign("The^m mo+'i TP");
   l[_L_FORM_NEW_TRANSPONDER_EDIT_TP_TITLE] .assign("Thay ddo^?i TP");
   l[_L_FORM_NEW_TRANSPONDER_FREQUENCY]     .assign("Ta^`n so^'");
   l[_L_FORM_NEW_TRANSPONDER_SYMBOL_RATE]   .assign("Sym");
   l[_L_FORM_NEW_TRANSPONDER_POLARIZATION]  .assign("Pha^n cu+.c");
   l[_L_FORM_NEW_TRANSPONDER_FEC]           .assign("Fec");
   l[_L_FORM_NEW_TRANSPONDER_MODULATION]    .assign("DDie^`u che^'");
   l[_L_FORM_NEW_TRANSPONDER_TRANSPONDER_EXIST]    .assign("TP dda~ to^`n ta.i");

   //FORM CHANNEL SCAN
   l[_L_FORM_CHANNEL_SCAN_TITLE]            .assign("Ti`m kie^'m ke^nh");
   l[_L_FORM_CHANNEL_SCAN_TV_CHANNEL]       .assign("Ke^nh TV");
   l[_L_FORM_CHANNEL_SCAN_TV_RADIO]         .assign("Ke^nh Radio");
   l[_L_FORM_CHANNEL_SCAN_CHANNEL_NUM]      .assign("So^' ke^nh: ");
   l[_L_FORM_CHANNEL_SCAN_SEARCH_FAIL]      .assign("Kho^ng ti`m tha^'y ke^nh");
   l[_L_FORM_CHANNEL_SCAN_SEARCH_SUCCESS]   .assign("Ti`m kie^'m tha`nh co^ng");
   l[_L_FORM_CHANNEL_SCAN_SEARCH_CONTINUE]  .assign("Nha^'n OK dde^? tie^'p tu.c.");
   l[_L_FORM_CHANNEL_SCAN_SEARCH_SAVING]    .assign("Tha`nh co^ng. DDang ghi ...");

   //FORM CHANNEL SCAN SETTINGS
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_TITLE]            .assign("Ti`m kie^'m ke^nh");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_SATELLITE]        .assign("Ve^. tinh");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_LNB_FREQUENCY]    .assign("Ta^`n so^' LNB");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_LNB_POWER]        .assign("LNB Power");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_22KHZ_TONE]       .assign("22 Khz Tone");
   l[_L_FORM_CHANNEL_SCAN_SETTINGS_TRANSPONDER_LIST] .assign("Danh sa'ch TP");
   l[_L_FORM_MANUAL_SCAN_SETTING_TITLE]              .assign("Do` ke^nh ba(`ng tay");
   l[_L_FORM_MANUAL_SCAN_SETTING_CHANNEL_NO]         .assign("Ke^nh so^'");
   l[_L_FORM_SCAN_DVBT_FREQUENCY]                    .assign("DVB-T: Ta^`n so^' %d.%d MHz, ba(ng tho^ng %d MHz");
   l[_L_FORM_SCAN_DVBT2_FREQUENCY]                   .assign("DVB-T2: Ta^`n so^' %d.%d MHz, ba(ng tho^ng %d MHz");
   l[_L_FORM_SCAN_DVBC_FREQUENCY]                    .assign("DVB-C: Ta^`n so^' %d.%d MHz, ba(ng tho^ng %d MHz");
   l[_L_FORM_MANUAL_SCAN_SETTING_SCAN_FREQUENCY]     .assign("Ta^`n so^' que't");
   l[_L_FORM_DVBC_SCAN_SETTINGS_START_FREQUENCY]     .assign("Ta^`n so^' ba('t dda^`u");
   l[_L_FORM_DVBC_SCAN_SETTINGS_END_FREQUENCY]       .assign("Ta^`n so^' ke^'t thu'c");
   l[_L_FORM_DVBC_SCAN_SETTINGS_SYMBOL_RATE]         .assign("Symbol Rate");
   l[_L_FORM_DVBC_SCAN_SETTINGS_MODULATION]          .assign("Const.");
   l[_L_FORM_DVBC_SCAN_SETTINGS_NETWORK]             .assign("Ma.ng");
   l[_L_FORM_SCAN_SETTINGS_SYMBOLRATE_LIMIT]         .assign("Sym kho^ng nho? ho+n %d !");

   //FORM TIMER MANAGER
   l[_L_FORM_TIMER_MANAGER_TITLE]                    .assign("Qua?n ly' ba'o gio+`");
   l[_L_FORM_TIMER_MANAGER_TIMER_LIST_TITLE]         .assign("Qua?n ly' tho+`i gian");
   l[_L_FORM_TIMER_MANAGER_TIMER_NO_EVENT]           .assign("Kho^ng co' chu+o+ng tri`nh");
   l[_L_FORM_TIMER_MANAGER_GUIDE_EDIT_TIMER]         .assign("Su+?a tho+`i gian");
   l[_L_FORM_ADD_TIMER_TITLE]                        .assign("The^m ba'o gio+`");
   l[_L_FORM_ADD_TIMER_CHANNEL]                      .assign("Ke^nh");
   l[_L_FORM_ADD_TIMER_DATE]                         .assign("Nga`y");
   l[_L_FORM_ADD_TIMER_DATE_SUN]                     .assign("CN");
   l[_L_FORM_ADD_TIMER_DATE_MON]                     .assign("T2");
   l[_L_FORM_ADD_TIMER_DATE_TUE]                     .assign("T3");
   l[_L_FORM_ADD_TIMER_DATE_WED]                     .assign("T4");
   l[_L_FORM_ADD_TIMER_DATE_THU]                     .assign("T5");
   l[_L_FORM_ADD_TIMER_DATE_FRI]                     .assign("T6");
   l[_L_FORM_ADD_TIMER_DATE_SAT]                     .assign("T7");
   l[_L_FORM_ADD_TIMER_START_TIME]                   .assign("Tho+`i gian ba('t dda^`u");
   l[_L_FORM_ADD_TIMER_DURATION]                     .assign("Tho+`i lu+o+.ng ghi");
   l[_L_FORM_ADD_TIMER_SAVE_TIMER]                   .assign("Lu+u ba'o gio+`");
   l[_L_FORM_DELETE_TIMER_MESS_DELETE_ALL]           .assign("Se~ xo'a toa`n bo^. dda(.t gio+` ?");
   l[_L_FORM_DELETE_TIMER_MESS_DELETE_TIMER]         .assign("Se~ xo'a mu.c dda(.t gio+` na`y ?");
   l[_L_FORM_SHOW_TIMER_ERROR_MESS]                  .assign("Tho+`i gian ba('t dda^`u co' lo^~i");
   l[_L_FORM_SHOW_TIMER_DURATION_ERROR_MESS]         .assign("Tho+`i lu+o+.ng ghi co' lo^~i");
   l[_L_FORM_SHOW_TIMER_DATE_ERROR_MESS]             .assign("Nga`y ba('t dda^`u kho^ng ddu'ng");
   l[_L_FORM_SHOW_TIMER_DAY_ERROR_MESS]              .assign("Vui lo`ng cho.n nga`y dda(.t gio+` !");
   l[_L_FORM_SHOW_TIMER_DUPLICATE_TIMER_ERROR_MESS]  .assign("DDa~ tru`ng la(.p tho+`i gian.");
   l[_L_FORM_SHOW_TIMER_OVERLAPPED_ERROR_MESS]       .assign("Tho+`i gian bi. tru`ng.");
   l[_L_FORM_POPUP_EXECUTE_ALARM_MODE_TIMER_MESS]    .assign("Sau va`i gia^y se~ thay ddo^?i ke^nh.");
   l[_L_FORM_POPUP_EXECUTE_RECORD_MODE_TIMER_MESS]   .assign("Sau va`i gia^y se~ ba('t dda^`u ghi video");
   l[_L_FORM_POPUP_EXECUTE_STANDBY_MODE_TIMER_MESS]  .assign("Sau va`i gia^y dda^`u thu se~ ddi va`o che^' ddo^. cho+`.");
   l[_L_FORM_POPUP_EXECUTE_TIMER_SELECT_BUTTON_MESS] .assign("Nha^'n phi'm tu+` cho^'i dde^? hu?y bo?");
   l[_L_FORM_POPUP_EXECUTE_TIMER_CONFLICT_FREQ_MESS] .assign("Hu?y ba?n ghi na`y ?");

   // FORM UPGRADE FIRMWARE
   l[_L_FORM_UPGRADE_FIRMWARE_TITLE]                  .assign("Na^ng ca^'p dda^`u thu");
   l[_L_FORM_UPGRADE_FIRMWARE_SATELLITE]              .assign("Ve^. tinh");
   l[_L_FORM_UPGRADE_FIRMWARE_TRANSPONDER_LIST]       .assign("Danh sa'ch TP");
   l[_L_FORM_UPGRADE_FIRMWARE_UPDATE_SOFTWARE]        .assign("Na^ng ca^'p pha^`n me^`m");
   l[_L_FORM_UPGRADE_FIRMWARE_CURRENT_VERSION]        .assign("Kho^ng co' pha^`n me^`m mo+'i");
   l[_L_FORM_UPGRADE_FIRMWARE_WAITTINGS_USB]          .assign("Kho^ng ta('t/ru't nguo^`n, USB !");
   l[_L_FORM_UPGRADE_FIRMWARE_SUCCESS]                .assign("Tha`nh co^ng, ta('t/ba^.t nguo^`n !");
   l[_L_FORM_UPGRADE_FIRMWARE_FAILED]                 .assign("Co' lo^~i, mo+`i thu+? la.i !");
   l[_L_FORM_UPGRADE_FIRMWARE_FAILED_FORMAT]          .assign("Co' lo^~i, ddi.nh da.ng file !");
   l[_L_FORM_UPGRADE_FIRMWARE_FAILED_SIGN]            .assign("Co' lo^~i, chu+~ ky' file !");

   //FORM STORAGE INFO
   l[_L_FORM_STORAGE_INFO_TITLE]                            .assign("Lu+u tru+~");
   l[_L_FORM_STORAGE_INFO_INFORM_FULL]                      .assign("Co' lo^~i, USB dda~ dda^`y");
   l[_L_FORM_STORAGE_INFO_INFORM_REC_ERROR_MESSAGE]         .assign("So^' block ghi lo^~i: %u");
   l[_L_FORM_STORAGE_INFO_INFORM_FILE_OPERATION_INPROGRESS] .assign("DDang xu+? ly' file !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_MOUNT_USB]          .assign("Co' lo^~i, kho^ng ti`m tha^'y USB");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_DELETE_FILE]        .assign("Kho^ng the^? xo'a file !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_COPY_FILE]          .assign("Kho^ng the^? copy file !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_RENAME_FILE]        .assign("Kho^ng the^? ddo^?i te^n file !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_STOP_RECORDING]     .assign("Kho^ng the^? du+`ng ghi !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_STOP_PLAYING]       .assign("Kho^ng the^? du+`ng xem video !");
   l[_L_FORM_STORAGE_INFO_INFORM_CANNOT_START_PLAYING]      .assign("Kho^ng the^? pha't la.i !");
   l[_L_FORM_STORAGE_INFO_INFORM_CHANNEL_LOCKED]            .assign("Co' lo^~i, ke^nh dda~ bi. kho'a !");
   l[_L_FORM_STORAGE_INFO_INFORM_COPY_PLAYABLE_FILE_ONLY]   .assign("Chi? che'p file co' the^? pha't la.i !");
   l[_L_FORM_STORAGE_INFO_INFORM_COPY_FILE_NAME_NOT_VALID]  .assign("Te^n file kho^ng ho+.p le^. !");
   l[_L_FORM_STORAGE_INFO_INFORM_COPY_DONE]                 .assign("Che'p file tha`nh co^ng !");
   l[_L_FORM_STORAGE_INFO_INFORM_HOT_UNPLUG_USB]            .assign("Co' lo^~i, ma^'t ke^'t no^'i USB");
   l[_L_FORM_STORAGE_INFO_INFORM_FILE_NOT_SUPPORT]          .assign("DDi.nh da.ng kho^ng ho^~ tro+. !");
   l[_L_FORM_STORAGE_INFO_INFORM_RECORDING_IN_PROGRESS]     .assign("Co' lo^~i, ddang ghi video !");
   l[_L_FORM_STORAGE_INFO_INFORM_CHANNEL_NOT_EXIST]         .assign("Ke^nh kho^ng to^`n ta.i !");

   //FORM CONFIRM MESSAGE
   l[_L_FORM_CONFIRM_TITLE]                           .assign("Chu' y'");
   l[_L_FORM_CONFIRM_EXIT_RECORD_MESSAGE]             .assign("Du+`ng ghi file ?");
   l[_L_FORM_CONFIRM_EXIT_PLAYBACK_MESSAGE]           .assign("Du+`ng xem/nghe tu+` file ?");
   l[_L_FORM_CONFIRM_DELETE_FILE_MESSAGE]             .assign("Xo'a file na`y ?");
   l[_L_FORM_CONFIRM_RENAME_FILE_MESSAGE]             .assign("DDo^?i te^n file na`y ?");
   l[_L_FORM_CONFIRM_RENAME_FILE_AGAIN_MESSAGE]       .assign("Lo^~i, Tie^'p tu.c ddo^?i te^n kha'c ?");
   l[_L_FORM_CONFIRM_EXIT_GAME_SNAKE]                 .assign("Thoa't tro` cho+i ?");
   l[_L_FORM_CONFIRM_RESTART_GAME_SNAKE]              .assign("Tie^'p tu.c cho+i ?");

   //FORM INPUT TEXT
   l[_L_FORM_INPUT_TITLE]                             .assign("Nha^.p te^n");
   l[_L_FORM_INPUT_STATIC]                            .assign("Te^n cu~: ");
   l[_L_FORM_INPUT_DYNAMIC]                           .assign("Te^n mo+'i: ");
   l[_L_FORM_INPUT_INFORM_TEXT_LENGTH_MAX]            .assign("To^'i dda 13 ky' tu+. !");

   // FORM EPG ONE CHANNEL
   l[_L_FORM_EPG_ONE_CHANNEL_TITLE]                   .assign("Li.ch pha't so'ng");
   l[_L_FORM_EPG_ONE_CHANNEL_GUIDE_EPG_TYPE]          .assign("Che^' ddo^. hie^?n thi. EPG");
   l[_L_FORM_EPG_ONE_CHANNEL_GUIDE_SAVE_TIMER]        .assign("Lu+u he.n gio+`");
   l[_L_FORM_EPG_ONE_CHANNEL_GUIDE_NEXT_DAY]          .assign("Nga`y ho^m sau");
   l[_L_FORM_EPG_ONE_CHANNEL_GUIDE_PREVIOUS_DAY]      .assign("Nga`y ho^m tru+o+'c");
   l[_L_FORM_EPG_ONE_CHANNEL_WARNING_NO_EVENT]        .assign("Kho^ng co' EPG");

   // FORM EPG ONE CHANNEL
   l[_L_FORM_EPG_FULL_TITLE]                          .assign("Li.ch pha't so'ng");

   //FORM EPG FULL_TIMER MANAGER
   l[_L_FORM_EPG_FULL_TIMER_MANAGER_TITLE]            .assign("Lu+u tru+~");

   //KEYBOARD
   l[_L_KEYBOARD_TITLE]                               .assign("Ba`n phi'm");

   //FORM_REC_PLAYBACK
   l[_L_FORM_PLAYBACK_FILE_LIST_TITLE]                .assign("Danh sa'ch file");

   //REBOOT
   l[_L_REBOOT]                                       .assign("DDang kho+?i ddo^.ng la.i ...");

   //FORM STARTUP
   l[_L_FORM_STARTUP_MESSSAGE]                        .assign("GBS");
   l[_L_FORM_STARTUP_GOTO_SCAN_CHANNEL]               .assign("DDa^`u thu se~ chuye^?n sang che^' ddo^. do` ke^nh tu+. ddo^.ng ... ");

   //FORM INFOCHANNEL
   l[_L_FORM_INFO_CHANNEL_TITTLE]                     .assign("Tho^ng tin ke^nh");
   l[_L_FORM_INFO_CHANNEL_NAME]                       .assign("Ke^nh");
   l[_L_FORM_INFO_CHANNEL_FREQ]                       .assign("Ta^`n so^'");
   l[_L_FORM_INFO_CHANNEL_NUMBER]                     .assign("So^' ke^nh");
   l[_L_FORM_INFO_CHANNEL_SERVICE_ID]                 .assign("Service ID");
   l[_L_FORM_INFO_CHANNEL_NETWORK_ID]                 .assign("Network ID");
   l[_L_FORM_INFO_CHANNEL_TUNER_STATUS]               .assign("Tra.ng tha'i Tuner");
   l[_L_FORM_INFO_CHANNEL_TUNER_NONE]                 .assign("NONE");
   l[_L_FORM_INFO_CHANNEL_TUNER_UNLOCKED]             .assign("UNLOCKED");
   l[_L_FORM_INFO_CHANNEL_TUNER_LOCKED]               .assign("LOCKED");
   l[_L_FORM_INFO_CHANNEL_TUNER_IDLE]                 .assign("IDLE");
   l[_L_FORM_INFO_CHANNEL_TUNER_STANDBY]              .assign("STANDBY");
   l[_L_FORM_INFO_CHANNEL_TUNER_SCANNING]             .assign("SCANNING");

   //FORM PIP CONTROL
   l[_L_FORM_PIP_CONTROL_TITTLE]                      .assign("PIP");
   l[_L_FORM_PIP_CONTROL_CHANN]                       .assign("Ke^nh PIP");
   l[_L_FORM_PIP_CONTROL_CLOSE]                       .assign("DDo'ng PIP");
   l[_L_FORM_PIP_CONTROL_OUTPUT]                      .assign("Co^?ng ra");
   l[_L_FORM_PIP_CONTROL_POS]                         .assign("Vi. tri'");
   l[_L_FORM_PIP_CONTROL_SWAP]                        .assign("Chuye^?n ma`n hi`nh");
   l[_L_FORM_PIP_OUTPUT_HDMI]                         .assign("HDMI");
   l[_L_FORM_PIP_OUTPUT_AV]                           .assign("AV");
   l[_L_FORM_PIP_OUTPUT_ALL]                          .assign("Ta^'t ca?");
   l[_L_FORM_PIP_CONTROL_POS_TOPLEFT]                 .assign("Tre^n - Tra'i");
   l[_L_FORM_PIP_CONTROL_POS_TOPRIGHT]                .assign("Tre^n - Pha?i");
   l[_L_FORM_PIP_CONTROL_POS_BOTLEFT]                 .assign("Du+o+'i - Tra'i");
   l[_L_FORM_PIP_CONTROL_POS_BOTRIGHT]                .assign("Du+o+'i - Pha?i");
   l[_L_FORM_ERROR_PIP_IN_PROGRESS]                   .assign("Co' lo^~i, ddang mo+? PIP !");
   l[_L_FORM_PIP_CONTROL_SWAP_WARNING_PLAYBACK]       .assign("Kho^ng the^? chuye^?n ma`n hi`nh");
   l[_L_FORM_PIP_CONTROL_OPEN_PLAYING_CHANNEL]        .assign("Ke^nh ddang ddu+o+.c mo+? tre^n TV");
   l[_L_FORM_PIP_CONTROL_OPEN_PIP_CHANNEL]            .assign("Ke^nh ddang ddu+o+.c mo+? tre^n PIP");
   l[_L_FORM_PIP_CONTROL_NO_SIGNAL_FOUND]             .assign("Kho^ng tha^'y ti'n hie^.u!");
   l[_L_FORM_PIP_CONTROL_OPEN_AUDIO_CHANNEL]          .assign("Kho^ng the^? mo+? ke^nh radio!");
   l[_L_FORM_PIP_CONTROL_SWAP_AUDIO_CHANNEL]          .assign("Kho^ng the^? chuye^?n ke^nh radio!");
   l[_L_FORM_PIP_CONTROL_DUAL_TV]                     .assign("Ke^nh Dual TV");
   l[_L_FORM_PIP_CONTROL_CLOSE_DUAL_TV]               .assign("DDo'ng Dual TV");
   l[_L_FORM_ERROR_DUAL_TV_IN_PROGRESS]               .assign("Co' lo^~i, ddang mo+? Dual TV !");

   //FORM_CHANNEL_LOCK_CONTROL
   l[_L_FORM_CHANNEL_LOCK_CONTROL_TITTLE]             .assign("Tu`y cho.n kho'a ke^nh");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_TYPE]               .assign("Tho+`i gian kho'a");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_START]              .assign("Tu+`");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_END]                .assign("DDe^'n");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_LOCK]               .assign("Kho'a");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_UNLOCK]             .assign("Mo+? kho'a");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_ALL]                .assign("Ca? nga`y");
   l[_L_FORM_CHANNEL_LOCK_CONTROL_EDIT]               .assign("Tu`y chi?nh");

   //FRONT PANEL
   l[_L_FRONT_PANEL_POWER_OFF]                        .assign("Ba^.t / Ta('t");
   l[_L_FRONT_PANEL_ADJUST_VOLUME]                    .assign("A^m lu+o+.ng");
   l[_L_FRONT_PANEL_SCAN_AUTO]                        .assign("Do` ke^nh");

   //DVR PLAYING
   l[_L_PLAYING_SPEED_UP]                             .assign("1X");
   l[_L_PLAYING_SPEED_DOWN]                           .assign("-1X");
   l[_L_PLAYING_CHANGE_LOOP]                          .assign("La(.p la.i");
   l[_L_OPEN_PLAYBACK_LIST]                           .assign("Cho.n file");

   //DEMO VERSION
   l[_L_DEMO_VERSION]                                 .assign("GBS-HD DEMO VERSION");
   l[_L_DEMO_VERSION_TIME_REACHED]                    .assign("Demo expired, STB is stopping!");
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GLANG_VN_HPP_ */
