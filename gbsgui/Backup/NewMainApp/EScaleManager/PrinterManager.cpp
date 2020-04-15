//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: PrinterManager.cpp                                                         //
//////////////////////////////////////////////////////////////////////////////////////

#include "PrinterManager.hpp"
#include "GStringUtils.hpp"
#include "GUserSettings.hpp"
#include "RateManager.hpp"
#include "QrEncode/qrencode.h"
#include "images_extern.hpp"
#include "GSystem.hpp"

//////////////////////////////////////////////////////////////////////////////////////
// Embedded Thermal Printer DP-EH300
#define ASCII_NUL 0x00
#define ASCII_SOH 0x01
#define ASCII_STX 0x02
#define ASCII_ETX 0x03
#define ASCII_EOT 0x04
#define ASCII_ENQ 0x05
#define ASCII_ACK 0x06
#define ASCII_BEL 0x07
#define ASCII_BS  0x08
#define ASCII_HT  0x09
#define ASCII_LF  0x0A
#define ASCII_VT  0x0B
#define ASCII_FF  0x0C
#define ASCII_CR  0x0D
#define ASCII_SO  0x0E
#define ASCII_SI  0x0F
#define ASCII_DEL 0x10
#define ASCII_DC1 0x11
#define ASCII_DC2 0x12
#define ASCII_DC3 0x13
#define ASCII_DC4 0x14
#define ASCII_NAK 0x15
#define ASCII_SYN 0x16
#define ASCII_ETB 0x17
#define ASCII_CAN 0x18
#define ASCII_EM  0x19
#define ASCII_SUB 0x1A
#define ASCII_ESC 0x1B
#define ASCII_FS  0x1C
#define ASCII_GS  0x1D
#define ASCII_RS  0x1E
#define ASCII_US  0x1F
#define ASCII_SP  0x20

#define BIT0      0x00
#define BIT1      0x01

#define STR_TOTAL_ITEM    "SO SAN PHAM"
#define STR_AMOUNT		  "TONG TIEN"
#define STR_PAY		      "THANH TOAN"
#define STR_CHANGE        "TRA LAI"
#define STR_LINE_SEPARATE "--------------------------------"

#define STR_HEADER_LOGO1  "VISSAN-TEST"
#define STR_HEADER_LOGO2  "   Cua hang thuc pham Vissan    "
#define STR_HEADER_LOGO3  "     Ma so thue: 0300105356     "
#define STR_HEADER_LOGO4  "  25 To Hien Thanh, P.13, Q.10  "
#define STR_HEADER_LOGO5  "DT:083 553 3999-FAX:083 553 3939"
#define STR_HEADER_LOGO6  "     Hotline: 1900 54 55 83     "
#define STR_HEADER_LOGO7  "  Email: vissanco@vissan.com.vn "
#define STR_HEADER_LOGO8  "   Website: www.vissan.com.vn   "
#define STR_HEADER_LOGO9  "********************************"
#define STR_LINE_SOLID    "________________________________"

#define STR_HEADER_TITLE  "HOA DON BAN LE"
#define STR_HEADER_ROW    "H.HOA   D.GIA   S.LUONG   T.TIEN"
#define STR_FOOTER_THANKS "Cam on quy khach hang!"
#define STR_MESSAGE_WARN  "*** CHU Y ***"

#define STR_PRICE_NET_TITLE "D.gia:%s   S.Luong:%s"
#define STR_TOTAL_PRICE		"THANH TIEN: %.2f"
#define STR_DATE_TEMP_TITLE " Ngay sx | Han dung | Nhiet do  "

#define LENGTH_PAPER      32
#define BITMAP_LENGTH    363
#define NSCALE_QRCODE      4

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

Task_t PrinterManager::m_printerMainTaskId = (Task_t)NULL;
bool PrinterManager::m_isPrinterMainTaskRuning = false;

static Semaphore_t g_SemaphorePrinterLockMsgData;
static Semaphore_t g_SemaphorePrinterLockItemData;
static Semaphore_t g_SemaphorePrinterLockWeighData;
static Semaphore_t g_SemaphorePrinterLockReceiptData;
static bool g_SemaphorePrinterLockMsgDataNotified = true;
static bool g_SemaphorePrinterLockItemDataNotified = true;
static bool g_SemaphorePrinterLockWeighDataNotified = true;
static bool g_SemaphorePrinterLockReceiptDataNotified = true;

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::createTask()
{
   if( PrinterManager::m_isPrinterMainTaskRuning )
   {
      return;
   }

   if( PrinterManager::m_printerMainTaskId != (Task_t)NULL )
   {
      deleteTask();
   }

   PrinterManager::m_printerMainTaskId = Task_Create(PrinterManager::MainTask, NULL, 262144, MIN_USER_PRIORITY, "EScalePrinterTask", 0, PrinterManager::m_printerMainTaskId);

   if( PrinterManager::m_printerMainTaskId != (Task_t)NULL )
   {
      PrinterManager::m_isPrinterMainTaskRuning = true;
      _DTraceDebug("EScalePrinterTask created\n");
   }
   else
   {
      PrinterManager::m_isPrinterMainTaskRuning = false;
      _DTraceDebug("Cannot create EScalePrinterTask\n");
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::deleteTask()
{
    if( PrinterManager::m_printerMainTaskId != (Task_t)NULL )
    {
       Task_Wait(PrinterManager::m_printerMainTaskId);
       task_kill(PrinterManager::m_printerMainTaskId, 0, 0);
       Task_Delete(PrinterManager::m_printerMainTaskId);
    }

    PrinterManager::m_printerMainTaskId = (Task_t)NULL;
    PrinterManager::m_isPrinterMainTaskRuning = false;
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::MainTask(void* Parameter)
{
   int mainTaskCounter = 0;

   while( PrinterManager::m_isPrinterMainTaskRuning )
   {
      Task_Delay(ST_GetClocksPerSecond()/5);

      //There is no data -> Do nothing
      if( PrinterManager::getInstance().isDataEmpty() )
      {
         continue;
      }

      //Try to print receipt data
      PrinterManager::getInstance().printReceipt();

      //Try to print weigh data
      PrinterManager::getInstance().printWeightData();

      //Try to print item data
      PrinterManager::getInstance().printItemData();

      //Try to print message
      PrinterManager::getInstance().printMessage();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
PrinterManager::PrinterManager() : SerialPort(SPN_COM4)
{
   Semaphore_Init_Fifo(g_SemaphorePrinterLockItemData, 0);
   Semaphore_Init_Fifo(g_SemaphorePrinterLockWeighData, 0);
   Semaphore_Init_Fifo(g_SemaphorePrinterLockReceiptData, 0);
   Semaphore_Init_Fifo(g_SemaphorePrinterLockMsgData, 0);
   g_SemaphorePrinterLockItemDataNotified = true;
   g_SemaphorePrinterLockWeighDataNotified = true;
   g_SemaphorePrinterLockReceiptDataNotified = true;
   g_SemaphorePrinterLockMsgDataNotified = true;
   createTask();
}

//////////////////////////////////////////////////////////////////////////////////////
PrinterManager::~PrinterManager()
{
   Semaphore_Delete(g_SemaphorePrinterLockItemData);
   Semaphore_Delete(g_SemaphorePrinterLockWeighData);
   Semaphore_Delete(g_SemaphorePrinterLockReceiptData);
   Semaphore_Delete(g_SemaphorePrinterLockMsgData);
   deleteTask();
}

//////////////////////////////////////////////////////////////////////////////////////
PrinterManager& PrinterManager::getInstance()
{
   static PrinterManager instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::init()
{
   initPrinter();
   setPrintPamrams(9, 100, 9);
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::deinit()
{
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::isReady()
{
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::initPrinter()
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 0x40};
   writeBytes(commands, 2, numWritten);
   Task_Delay(ST_GetClocksPerSecond()/10);
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::setPrintPamrams(U8 heatPoint, U8 heatTime, U8 heatTimeInterval)
{
   U32 numWritten = 0;
   U8 commands[5] = {ASCII_ESC, 0x37, 0x00, 0x00, 0x00};
   commands[2] = heatPoint;
   commands[3] = heatTime;
   commands[4] = heatTimeInterval;
   writeBytes(commands, 5, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printTab()
{
   U32 numWritten = 0;
   U8 commands[1] = {ASCII_HT};
   writeBytes(commands, 1, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printLF()
{
   U32 numWritten = 0;
   U8 commands[1] = {ASCII_LF};
   writeBytes(commands, 1, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printCR()
{
   U32 numWritten = 0;
   U8 commands[1] = {ASCII_CR};
   writeBytes(commands, 1, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printAndPaper(U8 nLength)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 0x4A, 0x00};
   commands[2] = nLength;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setRightCharacterSpacing(U8 nRight)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, ASCII_SP, 0x00};
   commands[2] = nRight;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setPrintMode(U8 printMode)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 0x21, 0x00};
   commands[2] = printMode;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setAbsolutePrintPosition(U8 nL, U8 nH)
{
   U32 numWritten = 0;
   U8 commands[4] = {ASCII_ESC, 0x24, 0x00, 0x00};
   commands[2] = nL;
   commands[3] = nH;
   writeBytes(commands, 4, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setLeftSpacing(U8 nL)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 0x42, 0x00};
   commands[2] = nL;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setUnderline(U8 nL)
{
   // 0x48 : Lift the underline mode, 0x49, 0x50 : Set underline mode
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 0x2D, 0x00};
   commands[2] = nL;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setLineSpacing(U8 nPoint)
{
   //0x30 - Default
   U32 numWritten = 0;
   U8 commands1[2] = {ASCII_ESC, 0x32};
   U8 commands2[3] = {ASCII_ESC, 0x33, 0x00};

   if( nPoint == 0 )
   {
      //Select the default row spacing
      writeBytes(commands1, 2, numWritten);
   }
   else
   {
      commands2[2] = nPoint;
      writeBytes(commands2, 3, numWritten);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setBoldPrint(U8 nBold)
{
   //0 - remove, 1 - set
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 0x45, 0x00};
   if( nBold == 0 )
   {
      commands[2] = 0x00;
   }
   else
   {
      commands[2] = 0x01;
   }
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setCharSet(U8 nCharacterId)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 0x52, 0x00};
   commands[2] = nCharacterId;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setPrintAlignment(U8 alignMode)
{
   //0x48-left; 0x49-center; 0x50-right
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 'a', alignMode};
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printAndFeedLine(U8 nLine)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 0x64, 0x00};
   commands[2] = nLine;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setCharCodeTable(U8 nCodeTable)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_ESC, 0x74, 0x00};
   commands[2] = nCodeTable;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setFontSize(U8 nSize)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_GS, 0x21, 0x00};
   commands[2] = nSize;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setBarcodeOnTheLeft()
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_GS, 0x78, 0x00};
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setPrintBarcodeCharPosition(U8 nPosition)
{
   //0x48-Not Print, 0x49-In the bar code, 0x50-In the code below,
   //0x51-In the upper part and the lower part of the bar code
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_GS, 0x48, 0x00};
   commands[2] = nPosition;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setBarcodeHight(U8 nHight)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_GS, 0x68, 0x00};
   commands[2] = nHight;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setBarcodeWidth(U8 nWidth)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_GS, 0x77, 0x00};
   commands[2] = nWidth;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::setBarcodeFont(U8 fontType)
{
   U32 numWritten = 0;
   U8 commands[3] = {ASCII_GS, 0x66, 0x00};
   commands[2] = fontType;
   writeBytes(commands, 3, numWritten);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printText(std::string text, U8 nBold, U8 nAlig, U8 nFontSize, U8 spacing)
{
   U32 numWritten = 0;
   setCharCodeTable(1);
   setPrintAlignment(nAlig);
   setBoldPrint(nBold);
   setFontSize(nFontSize);
   setLineSpacing(spacing);
   _DTraceDebug("data: %s, len: %d", text.c_str(), text.length());
   writeBytes((U8*)text.c_str(), text.length(), numWritten);
   printCR();
   printLF();
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printBarcode(std::string data, BarcodeSystem type, U8 nWidth, U8 nHeight, U8 nCharPosition)
{
   printAndPaper(1);
   setBarcodeOnTheLeft();
   setPrintBarcodeCharPosition(nCharPosition);
   setBarcodeWidth(nWidth);
   setBarcodeHight(nHeight);

   switch( type )
   {
   case UPC_A:
   case UPC_E:
      //to be defined
      break;
   case EAN_13:
   case EAN_8:
      printBarcodeEAN(data, type);
      break;
   case CODE_39:
   case ITF:
   case CODABAR:
   case CODE_93:
      break;
   case CODE_128:
   case CODE_128A:
   case CODE_128B:
   case CODE_128C:
      printBarcode128(data, type);
      break;
   default:
      break;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printBarcodeEAN(std::string data, BarcodeSystem type)
{
   U32 numWritten = 0;
   U32 dataLen = data.length();
   U8 EAN_i;
   U8 commands[3] = {ASCII_GS, 0x6B, 0x00};
   commands[2] = (U8)type;

   if( dataLen == 7 || dataLen == 8 || dataLen == 12 || dataLen == 13 )
   {
      writeBytes(commands, 3, numWritten);

      for(int i = 0; i < dataLen; ++i)
      {
         EAN_i = (U8)data.at(i);
         writeBytes(&EAN_i, 1, numWritten);
      }

      EAN_i = (U8)ASCII_NUL;
      writeBytes(&EAN_i, 1, numWritten);

      printCR();
      printLF();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printBarcode128(std::string data, BarcodeSystem type)
{
   _DTraceDebug("Print: %s", data.c_str());

   U32 numWritten = 0;
   U8 dataLen = (U8)data.length();
   U8 charSet = 0x41; //0x41-CODE A, 0x42-CODE B, 0x43-CODE C
   U8 commands[6] = {ASCII_GS, 0x6B, 0x00, 0x00, 0x7B, 0x41};

   switch( type )
   {
   case CODE_128A:
      charSet = 0x41;
      break;
   case CODE_128B:
      charSet = 0x42;
      break;
   case CODE_128C:
      charSet = 0x43;
      break;
   default:
      break;
   }

   commands[2] = (U8)(CODE_128);
   commands[3] = (U8)(dataLen + 2);
   commands[5] = (U8)(charSet);

   writeBytes(commands, 6, numWritten);

   U8 code128_i;
   for(int i = 0; i < dataLen; ++i)
   {
      code128_i = (U8)data.at(i);
      writeBytes(&code128_i, 1, numWritten);
   }

   printCR();
   printLF();
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printHeader()
{
   printText(STR_HEADER_LOGO1, 1, ALIGMENT_CENTER, 0x0E);
   printText(STR_HEADER_LOGO2, 0, ALIGMENT_CENTER);
   printText(STR_HEADER_LOGO3, 0, ALIGMENT_CENTER);
   printText(STR_HEADER_LOGO4, 0, ALIGMENT_CENTER);
   printText(STR_HEADER_LOGO5, 0, ALIGMENT_CENTER);
   printText(STR_HEADER_LOGO6, 0, ALIGMENT_CENTER);
   printText(STR_HEADER_LOGO7, 0, ALIGMENT_CENTER);
   printText(STR_HEADER_LOGO8, 0, ALIGMENT_CENTER);
   printAndPaper(50);
//   printText(STR_HEADER_LOGO9, 0, ALIGMENT_CENTER);
   printText(STR_HEADER_TITLE, 1, ALIGMENT_CENTER, 0x11);
   printText(STR_LINE_SEPARATE, 0, ALIGMENT_LEFT);
   printText(STR_HEADER_ROW, 0, ALIGMENT_LEFT);
   printText(STR_LINE_SEPARATE, 0, ALIGMENT_LEFT);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printFooter()
{
   printText(STR_LINE_SEPARATE, 0, ALIGMENT_LEFT);
   printText(STR_FOOTER_THANKS, 0, ALIGMENT_CENTER);
   printAndPaper(50);
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::print(ScaleMessageData& scaleMessageData)
{
   bool ret = true;

   if( scaleMessageData.type = PDT_WARNING )
   {
      printText(STR_MESSAGE_WARN, 1, ALIGMENT_CENTER, 0x11);
      printText(STR_LINE_SEPARATE, 0, ALIGMENT_LEFT);
      printText(scaleMessageData.value, 0, ALIGMENT_LEFT);
      printFooter();
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::print(ScaleWeightData& scaleWeighData)
{
   bool ret = true;

   if( _U_.getPrintLabel() == SETTING_OFF )
   {
      return ret;
   }

   if( scaleWeighData.data1 > 0.009 )
   {
      printHeader();
      std::string itemName, itemPrice, itemUnit, itemNet, itemTotal, itemStr = "";
      itemName = ScaleManager::getInstance().header2Str(scaleWeighData.header1);
      itemPrice = "---";
      itemUnit = ScaleManager::getInstance().unit2Str(scaleWeighData.unit);
      itemNet = ScaleManager::getInstance().data2Str(scaleWeighData.data1);
      itemTotal = "---";

      itemName.resize(7, ' ');

      if( itemPrice.length() < 6 )
      {
         std::string tmpStr = "";
         tmpStr.resize(6 - itemPrice.length(), ' ');
         itemPrice = tmpStr + itemPrice;
      }

      //itemUnit.resize(3, ' ');
      //itemNet.resize(5, ' ');
      itemNet.append(itemUnit);
      itemNet.resize(8, ' ');

      if( itemTotal.length() < 8 )
      {
         std::string tmpStr = "";
         tmpStr.resize(8 - itemTotal.length(), ' ');
         itemTotal = tmpStr + itemTotal;
      }

      itemStr = itemName + " " + itemPrice + " " + itemNet + " " + itemTotal;

      printText(itemStr, 0, ALIGMENT_LEFT);

      printFooter();
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::print(ScaleItemData scaleItemDatas)
{
   bool ret = true;

   if( _U_.getPrintLabel() == SETTING_OFF )
   {
      return ret;
   }

   std::string itemName = "";
   std::string itemPrice = "";
   std::string itemUnit = "";
   std::string itemNet = "";
   std::string itemTotal = "";
   std::string itemDate = "";
   std::string itemExpire = "";
   std::string itemStr = "";
   std::string itemQrCode = "";
   std::string itemTemp = "";

   ProductInfo productInfo;
   RateManager::getInstance().getProductInfo((U32)GStringUtils::str2int(scaleItemDatas.typeName), productInfo);
   itemName = productInfo.name;
   itemPrice = ScaleManager::getInstance().data2Str(scaleItemDatas.unitPrice);
   if( SUN_PCS == scaleItemDatas.unit )
   {
      itemUnit.assign(productInfo.unit);
   }
   else
   {
      itemUnit = ScaleManager::getInstance().unit2Str(scaleItemDatas.unit);
   }
   itemNet = ScaleManager::getInstance().data2Str(scaleItemDatas.dataNet);
   itemTotal = ScaleManager::getInstance().data2Str(scaleItemDatas.totalPrice);

   if( productInfo.temperature > -1000.0 && productInfo.temperature < 1000.0)
   {
      GStringUtils::format(itemTemp, "%.1f", productInfo.temperature);
   }

   if( productInfo.expdate > 0.1 )
   {
      DayTime_dtv_t currentTime, expireTime, tmpTime;
      time_t currentSec = scaleItemDatas.timeStamp;
      time_t expireSec = currentSec + (time_t)(productInfo.expdate * 24 * 60 * 60);
      TimeZone_dtv_t timeZone = _S_.getTimeZone();
      Second2DayTime(currentSec, &tmpTime);
      ChangeDayTimeZone(tmpTime, &timeZone, &currentTime);
      Second2DayTime(expireSec, &tmpTime);
      ChangeDayTimeZone(tmpTime, &timeZone, &expireTime);
      GStringUtils::format(itemDate, "%02d/%02d/%02d", currentTime.ymdw.day, currentTime.ymdw.month, currentTime.ymdw.year % 100);
      GStringUtils::format(itemExpire, "%02d/%02d/%02d", expireTime.ymdw.day, expireTime.ymdw.month, expireTime.ymdw.year % 100);
   }

   //////////// QR Code content /////////////
   itemQrCode = ScaleManager::getInstance().genPackageId(productInfo.code, scaleItemDatas.timeStamp);
   itemQrCode.append("\r\n");
   itemQrCode.append(productInfo.code);
   itemQrCode.append("\r\n");
   itemQrCode.append(itemNet);
   itemQrCode.append("\r\n");
   itemQrCode.append(itemPrice);
   itemQrCode.append("\r\n");
   itemQrCode.append(itemUnit);
   itemQrCode.append("\r\n");
   itemQrCode.append(itemTotal);
   //////////// QR Code content /////////////

   // print Logo + Qrcode
   U8* pBitmapQrcode = NULL;
   U8* pBitmapArray = NULL;
   U16 qrWidth, bmpWidth, bmpHeight;

   getQrcodeBitmapArray(itemQrCode, NSCALE_QRCODE, pBitmapQrcode, qrWidth);
   mergeBitmapArray((U8*)g_logoData, g_logoWidth, g_logoHeight, pBitmapQrcode, qrWidth, qrWidth, pBitmapArray, bmpWidth, bmpHeight);

   if( pBitmapArray != NULL )
   {
      printBitmapArray(pBitmapArray, bmpWidth, bmpHeight, BM_24DOUBLE);
   }
   else
   {
      printLogo();
   }

   if( pBitmapQrcode != NULL )
   {
      delete[] pBitmapQrcode;
      pBitmapQrcode = NULL;
   }

   if( pBitmapArray != NULL )
   {
      delete[] pBitmapArray;
      pBitmapArray = NULL;
   }

   printAndPaper(3);
   GStringUtils::format(itemStr, STR_PRICE_NET_TITLE, itemPrice.c_str(), (itemNet + itemUnit).c_str());
   GStringUtils::format(itemTotal, STR_TOTAL_PRICE, scaleItemDatas.totalPrice);
   printText(itemName, 1, ALIGMENT_CENTER, 1, 3);
   printText(itemStr, 0, ALIGMENT_CENTER, 0, 3);
   printText(itemTotal, 1, ALIGMENT_CENTER, 0, 1);
   //printText(STR_LINE_SOLID, 0, ALIGMENT_CENTER, 0, 1);
   printAndPaper(5);
   printText(STR_DATE_TEMP_TITLE, 0, ALIGMENT_CENTER, 0, 1);
   printText(" " + itemDate + "  " + itemExpire + "     " + itemTemp, 0, ALIGMENT_LEFT, 0, 3);
   printAndPaper(50);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::print(std::list<ScaleItemData>& scaleItemDatas)
{
   bool ret = true;

   if( scaleItemDatas.size() == 0 || _U_.getPrintReceipt() == SETTING_OFF )
   {
      return ret;
   }

   bool thereIsNoItem = true;
   bool totalIsZero = false;

   for( std::list<ScaleItemData>::iterator it = scaleItemDatas.begin(); it != scaleItemDatas.end(); ++it )
   {
      if( (*it).type == SIT_TOTAL && (*it).totalPrice == 0 )
      {
         totalIsZero = true;
      }
      else if( (*it).type == SIT_ITEM )
      {
         thereIsNoItem = false;
      }
   }

   if( thereIsNoItem || totalIsZero )
   {
      _DTraceDebug("Cancel print cause either thereIsNoItem or totalIsZero");
      return ret;
   }

   printHeader();

   int itemId = 0;

   for( std::list<ScaleItemData>::iterator it = scaleItemDatas.begin(); it != scaleItemDatas.end(); ++it )
   {
      if( (*it).type == SIT_ITEM )
      {
         std::string itemName, itemPrice, itemUnit, itemNet, itemTotal, itemStr = "";
         ProductInfo productInfo;

         RateManager::getInstance().getProductInfo((U32)GStringUtils::str2int((*it).typeName), productInfo);
         itemId++;
         GStringUtils::format(itemName, "%d. %s", itemId, productInfo.name);
         itemName.resize(31, ' ');
         printText(itemName, 0, ALIGMENT_LEFT);

         itemPrice = ScaleManager::getInstance().data2Str((*it).unitPrice);
         if( itemPrice.length() < 6 )
         {
            std::string tmpStr = "";
            tmpStr.resize(6 - itemPrice.length(), ' ');
            itemPrice = tmpStr + itemPrice;
         }

         if( SUN_PCS == (*it).unit )
         {
            itemUnit.assign(productInfo.unit);
         }
         else
         {
            itemUnit = ScaleManager::getInstance().unit2Str((*it).unit);
         }

         itemNet = ScaleManager::getInstance().data2Str((*it).dataNet);
         itemNet.append(itemUnit);
         if( itemNet.length() < 8 )
         {
            std::string tmpStr = "";
            tmpStr.resize(8 - itemNet.length(), ' ');
            itemNet = tmpStr + itemNet;
         }

         itemTotal = ScaleManager::getInstance().data2Str((*it).totalPrice);
         if( itemTotal.length() < 8 )
         {
            std::string tmpStr = "";
            tmpStr.resize(8 - itemTotal.length(), ' ');
            itemTotal = tmpStr + itemTotal;
         }

         itemStr = itemPrice + " " + itemNet + " " + itemTotal;
         if( itemStr.length() < 32 )
         {
            std::string tmpStr = "";
            tmpStr.resize(32 - itemStr.length(), ' ');
            itemStr = tmpStr + itemStr;
         }
         printText(itemStr, 0, ALIGMENT_LEFT);
      }
      else if( (*it).type == SIT_TOTAL )
      {
         printText(STR_LINE_SEPARATE, 0, ALIGMENT_LEFT);

         std::string totalItem[2], amount[2], pay[2], returnStr[2];

         GStringUtils::format(totalItem[0], "%s", (const char*)STR_TOTAL_ITEM);
         totalItem[1] = (*it).typeName;
         totalItem[0].resize(LENGTH_PAPER - totalItem[1].length(), ' ');

         printText(totalItem[0].append(totalItem[1]), 0, ALIGMENT_LEFT);

         GStringUtils::format(amount[0], "%s", (const char*)STR_AMOUNT);
         GStringUtils::format(amount[1], "%.2f", (*it).totalPrice);
         amount[0].resize(LENGTH_PAPER - amount[1].length(), ' ');

         printText(amount[0].append(amount[1]), 0, ALIGMENT_LEFT);

         if( (*it).pay > 0 )
         {
            GStringUtils::format(pay[0], "%s", (const char*)STR_PAY);
            GStringUtils::format(pay[1], "%.2f", (*it).pay);
            pay[0].resize(LENGTH_PAPER - pay[1].length(), ' ');

            printText(pay[0].append(pay[1]), 0, ALIGMENT_LEFT);

            GStringUtils::format(returnStr[0], "%s", (const char*)STR_CHANGE);
            GStringUtils::format(returnStr[1], "%.2f", (*it).change);
            returnStr[0].resize(LENGTH_PAPER - returnStr[1].length(), ' ');

            printText(returnStr[0].append(returnStr[1]), 0, ALIGMENT_LEFT);
         }
      }
   }

   printFooter();

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printReceipt()
{
   if( !m_receiptDataQueues.empty() )
   {
      g_SemaphorePrinterLockReceiptDataNotified = false;
      std::list<ScaleItemData> receiptData_i = m_receiptDataQueues.front();
      g_SemaphorePrinterLockReceiptDataNotified = true;
      Semaphore_Signal(g_SemaphorePrinterLockReceiptData);

      if( print(receiptData_i) )
      {
         g_SemaphorePrinterLockReceiptDataNotified = false;
         m_receiptDataQueues.pop();
         g_SemaphorePrinterLockReceiptDataNotified = true;
         Semaphore_Signal(g_SemaphorePrinterLockReceiptData);
      }

      Task_Delay(ST_GetClocksPerSecond()/20);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printItemData()
{
   if( !m_itemDataQueues.empty() )
   {
      g_SemaphorePrinterLockItemDataNotified = false;
      ScaleItemData itemData_i = m_itemDataQueues.front();
      g_SemaphorePrinterLockItemDataNotified = true;
      Semaphore_Signal(g_SemaphorePrinterLockItemData);

      if( print(itemData_i) )
      {
         g_SemaphorePrinterLockItemDataNotified = false;
         m_itemDataQueues.pop();
         g_SemaphorePrinterLockItemDataNotified = true;
         Semaphore_Signal(g_SemaphorePrinterLockItemData);
      }

      Task_Delay(ST_GetClocksPerSecond()/20);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printWeightData()
{
   if( !m_weighDataQueues.empty() )
   {
      g_SemaphorePrinterLockWeighDataNotified = false;
      ScaleWeightData weighData_i = m_weighDataQueues.front();
      g_SemaphorePrinterLockWeighDataNotified = true;
      Semaphore_Signal(g_SemaphorePrinterLockWeighData);

      if( print(weighData_i) )
      {
         g_SemaphorePrinterLockWeighDataNotified = false;
         m_weighDataQueues.pop();
         g_SemaphorePrinterLockWeighDataNotified = true;
         Semaphore_Signal(g_SemaphorePrinterLockWeighData);
      }

      Task_Delay(ST_GetClocksPerSecond()/20);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printMessage()
{
   if( !m_messageDataQueues.empty() )
   {
      g_SemaphorePrinterLockMsgDataNotified = false;
      ScaleMessageData msgData_i = m_messageDataQueues.front();
      g_SemaphorePrinterLockMsgDataNotified = true;
      Semaphore_Signal(g_SemaphorePrinterLockMsgData);

      if( print(msgData_i) )
      {
         g_SemaphorePrinterLockMsgDataNotified = false;
         m_messageDataQueues.pop();
         g_SemaphorePrinterLockMsgDataNotified = true;
         Semaphore_Signal(g_SemaphorePrinterLockMsgData);
      }

      Task_Delay(ST_GetClocksPerSecond()/20);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::getQrcodeBitmapArray(std::string data, U8 nScale, U8*& pBitArray, U16& bmpWidth)
{
   pBitArray = NULL;
   QRcode* pQrCode = NULL;

   bmpWidth = 0;

   if( nScale < 1 || nScale > 9 )
   {
      nScale = 2;
   }

   pQrCode = QRcode_encodeString(data.c_str(), 4, QR_ECLEVEL_L, QR_MODE_8, 0);

   if( pQrCode != NULL )
   {
      _DTraceDebug("QRcode version=%d, width=%d", pQrCode->version, pQrCode->width);
      bmpWidth = nScale * pQrCode->width;
      pBitArray = new U8[bmpWidth*bmpWidth];

      if( pBitArray == NULL )
      {
         _DTraceDebug("Can not allocate memory...");
         QRcode_free(pQrCode);
         pQrCode = NULL;
         return;
      }

      for( int i = 0; i < pQrCode->width; ++i )
      {
         for(int j = 0; j < pQrCode->width; ++j)
         {
            if( pQrCode->data[i*pQrCode->width+j]%2 )
            {
               for( int scaleH = 0; scaleH < nScale; ++scaleH )
               {
                  for( int scaleW = 0; scaleW < nScale; ++scaleW )
                  {
                     pBitArray[(i*nScale + scaleH)*bmpWidth + j*nScale + scaleW] = BIT1;
                  }
               }
            }
            else
            {
               for( int scaleH = 0; scaleH < nScale; ++scaleH )
               {
                  for( int scaleW = 0; scaleW < nScale; ++scaleW )
                  {
                     pBitArray[(i*nScale + scaleH)*bmpWidth + j*nScale + scaleW] = BIT0;
                  }
               }
            }
         }
      }

      QRcode_free(pQrCode);
      pQrCode = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printQRcode(std::string data, U8 nScale)
{
   U8* pBmpArray = NULL;
   U16 bmpWidth = 0;

   getQrcodeBitmapArray(data, nScale, pBmpArray, bmpWidth);

   if( pBmpArray != NULL )
   {
      printBitmapArray(pBmpArray, bmpWidth, bmpWidth,  BM_24DOUBLE);

      if( pBmpArray != NULL )
      {
         delete[] pBmpArray;
         pBmpArray = NULL;
      }
   }
   else
   {
      _DTraceDebug("pQrCode NULL");
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printLogo()
{
   printBitmapArray((U8*)g_logoData, g_logoWidth, g_logoHeight, BM_24DOUBLE);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printImage(U8* pImageData, U16 width, U16 height, U16 sizeOfPixel, PrintBitmapMode bmpMode)
{
   //we have not support the compress bitmap yet
   if( pImageData == NULL || sizeOfPixel < 3 )
   {
      _DTraceDebug("pImageData NULL");
      return;
   }

   U8* pBitArray = NULL;
   U32 imageSize = width*height;

   pBitArray = new U8[imageSize];
   if( pBitArray == NULL )
   {
      return;
   }

   U32 sizeOfLine = width*sizeOfPixel;
   //convert to bit array
   for( U32 i = 0; i < height; ++i )
   {
      for( U32 j = 0; j < width; ++j )
      {
         U8 red   = 0;
         U8 green = 0;
         U8 blue  = 0;

         //RGB
         red   = pImageData[i*sizeOfLine + j*sizeOfPixel];
         green = pImageData[i*sizeOfLine + j*sizeOfPixel + 1];
         blue  = pImageData[i*sizeOfLine + j*sizeOfPixel + 2];

         U8 luminance = (U8)((float)(red*0.299) + (float)(green*0.587) + (float)(blue*0.114));

         if( luminance < 128 )
         {
            //black
            pBitArray[i*height + j] = BIT1;
         }
         else
         {
            //white
            pBitArray[i*height + j] = BIT0;
         }
         printf("%d, ", pBitArray[i*height + j]);
      }
      printf("\r\n");
   }

   printBitmapArray(pBitArray, width, height, bmpMode);

   if( pBitArray != NULL )
   {
      delete[] pBitArray;
      pBitArray = NULL;
   }

   printAndPaper(80);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::mergeBitmapArray(U8* pBmpA, U16 wA, U16 hA, U8* pBmpB, U16 wB, U16 hB, U8*& pBmpArray, U16& width, U16& height)
{
   pBmpArray = NULL;

   if( (wA + wB) >= BITMAP_LENGTH )
   {
      _DTraceDebug("BITMAP SO LONG ! Return !!!");
      return;
   }

   width = BITMAP_LENGTH;
   int offSet = BITMAP_LENGTH - wA - wB;

   height = hB >= hA ? hB : hA;
   U32 imageSize = width*height;
   pBmpArray = new U8[imageSize];

   if( pBmpArray == NULL )
   {
      _DTraceDebug("BITMAP SO BIG ! NOT ENOUGH MEMORY !!!");
      return;
   }

   for( U32 i = 0; i < imageSize; ++i )
   {
      pBmpArray[i] = BIT0;
   }

   for( U32 i = 0; i < height; ++i )
   {
      for( U32 j = 0; j < wA; ++j )
      {
         if( i < hA )
         {
            pBmpArray[i*width + j] = pBmpA[i*wA + j];
         }
      }

      for( U32 j = wA + offSet; j < width; ++j )
      {
         if( i < hB )
         {
            pBmpArray[i*width + j] = pBmpB[i*wB + j - wA - offSet];
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::printBitmapArray(U8* pBmpArray, U16 width, U16 height, PrintBitmapMode bmpMode)
{
   //print bitmap
   U32 numWritten = 0;
   U8 command = 0x00;

   U8 lineSpacing  = 30; //default 30 points
   U8 printBmpMode = 33; //default 24 points double density
   U8 sizeOfSlice  =  3; //default 3 byte

   switch( bmpMode )
   {
   case BM_8SINGLE:
      lineSpacing  = 8;
      printBmpMode = 0;
      sizeOfSlice  = 1;
      break;

   case BM_8DOUBLE:
      lineSpacing  = 8;
      printBmpMode = 1;
      sizeOfSlice  = 1;
      break;

   case BM_24SINGLE:
      lineSpacing  = 24;
      printBmpMode = 32;
      sizeOfSlice  =  3;
      break;

   case BM_24DOUBLE:
      lineSpacing  = 24;
      printBmpMode = 33;
      sizeOfSlice  =  3;
      break;
   }

   //Init printer
   command = ASCII_ESC;
   writeBytes(&command, 1, numWritten);
   command = 0x40; //'@'
   writeBytes(&command, 1, numWritten);

   //Set the line spacing of 8 or 24 bit
   setLineSpacing(lineSpacing);

   U32 imageSize = width*height;
   U16 offset = 0;

   while( offset < height )
   {
      //Select the bitmap mode
      command = ASCII_ESC;
      writeBytes(&command, 1, numWritten);
      command = 0x2A; //'*' bit-image mode
      writeBytes(&command, 1, numWritten);
      command = printBmpMode;
      writeBytes(&command, 1, numWritten);
      command = (U8)(width & 0xFF);        //width low byte
      writeBytes(&command, 1, numWritten);
      command = (U8)((width >> 8) & 0xFF); //width high byte
      writeBytes(&command, 1, numWritten);

      for( U16 x = 0; x < width; ++x )
      {
         for( U16 k = 0; k < sizeOfSlice; ++k )
         {
            U8 slice = 0;

            for( U16 b = 0; b < 8; ++b )
            {
               U8 bit_i = BIT0;
               U16 y = (((offset / 8) + k) * 8) + b;
               // Calculate the location of the pixel we want in the bit array. It'll be at (y * width) + x.
               U16 i = (y * width) + x;
               // If the image is shorter than sizeOfSlice*8 dots, pad with zero.
               if( i < imageSize )
               {
                  bit_i = pBmpArray[i];
               }

               slice |= (U8)( bit_i << (7 - b));
            }

            writeBytes(&slice, 1, numWritten);
         }
      }

      offset += sizeOfSlice*8;
      command = ASCII_LF;
      writeBytes(&command, 1, numWritten);
   }

   // Restore the line spacing to the default of 30 dots.
   setLineSpacing(30);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::hasData(ScaleMessageData& msgData)
{
   if( !g_SemaphorePrinterLockMsgDataNotified )
   {
      Semaphore_Wait(g_SemaphorePrinterLockMsgData);
   }

   if( m_messageDataQueues.size() > DATA_ITEM_QUEUE_MAX )
   {
      //don't care, remove the last one
      m_messageDataQueues.pop();
   }

   _DTraceDebug("Has a message data");

   m_messageDataQueues.push(msgData);
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::hasData(ScaleItemData& itemData)
{
   if( itemData.dataNet > 0.009 )
   {
      if( !g_SemaphorePrinterLockItemDataNotified )
      {
         Semaphore_Wait(g_SemaphorePrinterLockItemData);
      }

      if( m_itemDataQueues.size() > DATA_ITEM_QUEUE_MAX )
      {
         //don't care, remove the last one
         m_itemDataQueues.pop();
      }

      _DTraceDebug("Has an item data");

      m_itemDataQueues.push(itemData);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::hasData(ScaleWeightData& itemData)
{
   if( itemData.data1 > 0.009 )
   {
      if( !g_SemaphorePrinterLockWeighDataNotified )
      {
         Semaphore_Wait(g_SemaphorePrinterLockWeighData);
      }

      if( m_weighDataQueues.size() > DATA_ITEM_QUEUE_MAX )
      {
         //don't care, remove the last one
         m_weighDataQueues.pop();
      }

      _DTraceDebug("Has a weight data");

      itemData.timestamp = _S_.getCurrentTimeSecond();
      m_weighDataQueues.push(itemData);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void PrinterManager::hasData(std::list<ScaleItemData>& receiptData)
{
   if( !receiptData.empty() )
   {
      if( !g_SemaphorePrinterLockReceiptDataNotified )
      {
         Semaphore_Wait(g_SemaphorePrinterLockReceiptData);
      }

      if( m_receiptDataQueues.size() > DATA_ITEM_QUEUE_MAX )
      {
         //don't care, remove the last one
         m_receiptDataQueues.pop();
      }
      m_receiptDataQueues.push(receiptData);

      _DTraceDebug("Has a receipt data");
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::isDataEmpty()
{
   return (bool)(m_itemDataQueues.empty() &&
                 m_weighDataQueues.empty() &&
                 m_receiptDataQueues.empty() &&
                 m_messageDataQueues.empty() );
}

//////////////////////////////////////////////////////////////////////////////////////
bool PrinterManager::printTest()
{
   if( !isDataEmpty() )
   {
      return false;
   }

   ScaleMessageData messageData;
   messageData.type = PDT_WARNING;
   messageData.value = "GBSW-1078: In thu may in!";
   print(messageData);
   printQRcode("GBSW-1078 Print a test of QR code", NSCALE_QRCODE);
   printAndPaper(10);
   printBarcode("0123456789123", EAN_13, 3, 80, 2);
   printBarcode("Barcode-128.B", CODE_128B, 2, 80, 2);
   printAndPaper(30);

   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
