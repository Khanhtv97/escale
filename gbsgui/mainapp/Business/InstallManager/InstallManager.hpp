//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: InstallManager.hpp                                                         //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef INSTALLMANAGER_HPP_
#define INSTALLMANAGER_HPP_

#include "ROMBlower.hpp"
#include "GObject.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace InstallManager {
//////////////////////////////////////////////////////////////////////////////////////
class InstallManager : public virtual GObject
{
public:
   static InstallManager& getInstance();

   void deinit();
   bool init();

   bool startInstall();
   bool startUninstall();
   void finishInstall(GROMError result);
   void setInstallFile(const std::string fileName);
   std::string getInstallFile();

   bool isInstalling();

   void setLastError(GROMError error);
   GROMError getLastError();

   bool backupUserData();
   bool restoreUserData(const std::string fileName);
   GIMAGEType getImageType();

private:
   InstallManager();
   ~InstallManager();

   Semaphore_t m_installSemaphore;
   bool m_isInstalling;
   std::string m_installFile;
   GROMError m_lastError;
   GIMAGEType m_imageType;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //InstallManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //INSTALLMANAGER_HPP_
