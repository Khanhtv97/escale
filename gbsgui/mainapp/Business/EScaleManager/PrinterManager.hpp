//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: PrinterManager.hpp                                                         //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef PRINTERMANAGER_H_
#define PRINTERMANAGER_H_

#include "SerialPort.hpp"
#include "ScaleManager.hpp"
#include <queue>

#define ALIGMENT_LEFT   0x48
#define ALIGMENT_CENTER 0x49
#define ALIGMENT_RIGHT  0x50

typedef enum BarcodeSystem_s
{
   UPC_A = 0,
   UPC_E,
   EAN_13,
   EAN_8,
   CODE_39,
   ITF,
   CODABAR,
   CODE_93 = 72,
   CODE_128,
   CODE_128A,
   CODE_128B,
   CODE_128C
}
BarcodeSystem;

typedef enum PrintBitmapMode_s
{
   BM_8SINGLE = 0,
   BM_8DOUBLE,
   BM_24SINGLE,
   BM_24DOUBLE
}
PrintBitmapMode;

typedef enum PrinterDataType_e
{
   PDT_NONE,
   PDT_DATA,
   PDT_ERROR,
   PDT_WARNING,
   PDT_MESSAGE
}
PrinterDataType;

typedef struct ScaleMessageData_s
{
   PrinterDataType type;
   std::string value;
}
ScaleMessageData;

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

class PrinterManager : public SerialPort
{
public:
   virtual bool init();
   virtual bool deinit();
   virtual bool isReady();

   static PrinterManager& getInstance();

   void hasData(ScaleMessageData& scaleMessageData);
   void hasData(ScaleWeightData& scaleWeighData);
   void hasData(ScaleItemData& scaleItemDatas);
   void hasData(std::list<ScaleItemData>& scaleItemDatas);

   bool printTest();

protected:
   static void MainTask(void* Parameter);
   static Task_t m_printerMainTaskId;
   static bool m_isPrinterMainTaskRuning;

private:
   PrinterManager();
   virtual ~PrinterManager();
   void createTask();
   void deleteTask();

   bool initPrinter();
   bool setPrintPamrams(U8 heatPoint, U8 heatTime, U8 heatTimeInterval);

   void printTab();
   void printLF();
   void printCR();
   void printAndPaper(U8 nLength);
   void setRightCharacterSpacing(U8 nRight);
   void setPrintMode(U8 printMode);
   void setAbsolutePrintPosition(U8 nL, U8 nH);
   void setLeftSpacing(U8 nL = 0);
   void setUnderline(U8 nL);
   void setLineSpacing(U8 nPoint);
   void setBoldPrint(U8 nBold);
   void setCharSet(U8 nCharacterId);
   void setPrintAlignment(U8 alignMode);
   void printAndFeedLine(U8 nLine);
   void setCharCodeTable(U8 nCodeTable);
   void setFontSize(U8 nSize);

   void printText(std::string text, U8 nBold, U8 nAlig, U8 nFontSize = 0, U8 spacing = 0);

   void setBarcodeOnTheLeft();
   void setPrintBarcodeCharPosition(U8 nPosition);
   void setBarcodeHight(U8 nHight);
   void setBarcodeWidth(U8 nWidth);
   void setBarcodeFont(U8 fontType);
   void printBarcode(std::string data, BarcodeSystem type, U8 nWidth, U8 nHeight, U8 nCharPosition);
   void printBarcodeEAN(std::string data, BarcodeSystem type);
   void printBarcode128(std::string data, BarcodeSystem type);

   void printBitmapArray(U8* pBmpArray, U16 width, U16 height, PrintBitmapMode bmpMode);
   void mergeBitmapArray(U8* pBmpA, U16 wA, U16 hA, U8* pBmpB, U16 wB, U16 hB, U8*& pBmpArray, U16& width, U16& height);

   void printHeader();
   void printFooter();

   void printLogo();
   void printImage(U8* pImageData, U16 width, U16 height, U16 sizeOfPixel, PrintBitmapMode bmpMode);
   void printQRcode(std::string data, U8 nScale);
   void getQrcodeBitmapArray(std::string data, U8 nScale, U8*& pBitArray, U16& bmpWidth);

   void printReceipt();
   void printItemData();
   void printWeightData();
   void printMessage();

   bool print(ScaleMessageData& scaleMessageData);
   bool print(ScaleWeightData& scaleWeighData);
   bool print(ScaleItemData scaleItemDatas);
   bool print(std::list<ScaleItemData>& scaleItemDatas);

   bool isDataEmpty();
   bool deleteItemData(U32 itemId);

   std::queue<ScaleMessageData> m_messageDataQueues;
   std::queue<ScaleItemData> m_itemDataQueues;
   std::queue<ScaleWeightData> m_weighDataQueues;
   std::queue< std::list<ScaleItemData> > m_receiptDataQueues;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //PRINTERMANAGER_H_
