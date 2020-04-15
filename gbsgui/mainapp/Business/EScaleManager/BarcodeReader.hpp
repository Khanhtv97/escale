//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: BarcodeReader.hpp                                                          //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef BARCODEREADER_H_
#define BARCODEREADER_H_

#include "SerialPort.hpp"

typedef enum BarcodeContentType_e
{
   BCT_NONE = 0,
   BCT_DATA,
   BCT_SUBMIT_DATA_URL
}
BarcodeContentType;

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

class BarcodeReader : public SerialPort
{
public:
   virtual bool init();
   virtual bool deinit();
   virtual bool isReady();

   static BarcodeReader& getInstance();
   BarcodeContentType scanReader(std::string& outContent);

private:
   BarcodeReader();
   virtual ~BarcodeReader();
   BarcodeContentType getContentType(std::string& inContent);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //BARCODEREADER_H_
