//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: HtmlDocument.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#include "HtmlDocument.hpp"
#include "GStringUtils.hpp"
#include "GUserSettings.hpp"
#include "ConnDeviceWifi.hpp"

//////////////////////////////////////////////////////////////////////////////////////
HtmlDocument::HtmlDocument()
{
}

//////////////////////////////////////////////////////////////////////////////////////
HtmlDocument::~HtmlDocument()
{
}

//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildLogin(const std::string& sessionId, std::string& oPageContent)
{
   oPageContent += "<form action=\"LOG_IN\">";
   oPageContent += "<input type=\"hidden\" name=\"id\" value=\"" + sessionId + "\">";
   oPageContent += "<table cellpadding=\"3\">";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><b><label>";
   oPageContent += HTML_MASTER_TITLE;
   oPageContent += "</label></b></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td><label>Ten:</label></td>";
   oPageContent += "<td><input type=\"text\" placeholder=\"ten dang nhap\" name=\"username\"></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td><label>Mat khau:</label></td>";
   oPageContent += "<td><input type=\"password\" placeholder=\"mat khau\" name=\"password\"></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><button type=\"submit\">Dang nhap</button></td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
   oPageContent += "</form>";
}

//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildListWifi(const std::string& sessionId, std::string& oPageContent)
{
   oPageContent =  "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><b><label>";
   oPageContent += HTML_MASTER_TITLE;
   oPageContent += HTML_TITLE_SETUPWIFI;
   oPageContent += "</label></b><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\" valign=\"top\" bgcolor=\"#CCCCCC\"><br>";
   std::string menuPage = "";
   buildMenuContent(sessionId, menuPage);
   oPageContent += menuPage;
   oPageContent += "</td>";
   oPageContent += "<td>";
   oPageContent += "<form action=\"CONNECT_ACCESS_POINT\">";
   oPageContent += "<input type=\"hidden\" name=\"id\" value=\"" + sessionId + "\">";
   oPageContent += "<table>";

   AccessPointInfo selectedAP;
   _U_.getSelectedAccessPoint(selectedAP);

   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"3\" bgcolor=\"#CCCCCC\"><label>Wifi Hien tai</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\"><label>" + selectedAP.ssid + "</label><br></td>";

   std::string cnnStatus = "Khong xac dinh";
   NetworkConnStatus nwCnnStatus = ConnDeviceWifi::getInstance().getNetworkStatus();
   switch( nwCnnStatus )
   {
   case NCS_GOTIP: cnnStatus = "Dang ket noi"; break;
   case NCS_CONNECTED: cnnStatus = "Da ket noi"; break;
   case NCS_DISCONNECTED: cnnStatus = "Khong ket noi"; break;
   case NCS_CONNECTIONFAIL: cnnStatus = "Ket noi loi"; break;
   default: break;
   }
   oPageContent += "<td align=\"right\" colspan=\"2\"><label>" + cnnStatus + "</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\"><label>IP</label><br></td>";

   DeviceWifiLocalIp localIp;
   ConnDeviceWifi::getInstance().getLocalIpAddress(localIp);
   oPageContent += "<td align=\"right\" colspan=\"2\"><label>" + localIp.stIpAddr + "</label><br></td>";
   oPageContent += "</tr>";

   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"3\" bgcolor=\"#CCCCCC\"><label>Danh sach Wifi</label><br></td>";
   oPageContent += "</tr>";

   std::list<AccessPointInfo> apList;
   ConnDeviceWifi::getInstance().getListAP(apList, true);

   if( !apList.empty() )
   {
      for( std::list<AccessPointInfo>::iterator it = apList.begin(); it != apList.end(); ++it )
      {
         std::string ecn = "Open";
         switch( (*it).ecn )
         {
         case ECN_WEP: ecn = "WEP"; break;
         case ECN_WPA_PSK: ecn = "WPA+PSK"; break;
         case ECN_WPA2_PSK: ecn = "WPA2+PSK"; break;
         case ECN_WPA_WPA2_PSK: ecn = "WPA+WPA2+PSK"; break;
         default: break;
         }
         oPageContent += "<tr>";
         oPageContent += "<td align=\"left\"><input type=\"radio\" name=\"macaddr\" value=\"" + (*it).mac + "\">" + (*it).ssid + "</td>";
         oPageContent += "<td align=\"left\"><label>" + (*it).mac + "</label></td>";
         oPageContent += "<td align=\"left\"><label>" + ecn + "</label></td>";
         oPageContent += "</tr>";
      }
   }

   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"3\" bgcolor=\"#CCCCCC\"><label>Mat khau:</label><input type=\"password\" placeholder=\"mat khau\" name=\"password\"><button type=\"submit\">Ket noi</button></td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
   oPageContent += "</form>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
}

//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildRateTable(const std::string& sessionId, std::string& oPageContent, int page)
{
   RateTable* pCurRateTable = NULL;
   std::string strTemp = "";

   pCurRateTable = new RateTable();
   if( pCurRateTable == NULL )
   {
     return;
   }

   RateManager::getInstance().getActivatedRateTable(pCurRateTable);

   oPageContent =  "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><b><label>";
   oPageContent += HTML_MASTER_TITLE;
   oPageContent += HTML_TITLE_RATETABLE;
   oPageContent += "</label></b><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\" valign=\"top\" bgcolor=\"#CCCCCC\"><br>";
   std::string menuPage = "";
   buildMenuContent(sessionId, menuPage);
   oPageContent += menuPage;
   oPageContent += "</td>";
   oPageContent += "<td>";
   oPageContent += "<form action=\"VIEWPAGE_RATE_TABLE\">";
   oPageContent += "<input type=\"hidden\" name=\"id\" value=\"" + sessionId + "\">";
   oPageContent += "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"5\" bgcolor=\"#CCCCCC\"><label>Bang gia</label><br></td>";
   oPageContent += "</tr>";

   for( int slot_i = 0; slot_i < ESCALE_NUM_RATE_TABLE; ++slot_i )
   {
      RateTableHeader rateHeader;
      if( RateManager::getInstance().getRateTableInfo(slot_i, rateHeader) )
      {
         GStringUtils::format(strTemp, "#%d", slot_i+1);
         oPageContent += "<tr>";
         oPageContent += "<td align=\"left\" colspan=\"2\" bgcolor=\"#CCCCCC\"><label>Slot: </label><br></td>";
         oPageContent += "<td align=\"right\" colspan=\"3\" bgcolor=\"#CCCCCC\"><label>" + strTemp + "</label><br></td>";
         oPageContent += "</tr>";
         GStringUtils::format(strTemp, "%d", rateHeader.id);
         oPageContent += "<tr>";
         oPageContent += "<td align=\"left\" colspan=\"2\"><label>Ma bang gia: </label><br></td>";
         oPageContent += "<td align=\"right\" colspan=\"3\"><label>" + strTemp + "</label><br></td>";
         oPageContent += "</tr>";
         oPageContent += "<tr>";
         oPageContent += "<td align=\"left\" colspan=\"2\"><label>Trang thai: </label><br></td>";
         if( rateHeader.isActivated )
         {
            strTemp = "Dang su dung";
         }
         else
         {
            strTemp = "Khong su dung";
         }
         oPageContent += "<td align=\"right\" colspan=\"3\"><label>" + strTemp + "</label><br></td>";
         oPageContent += "</tr>";
         oPageContent += "<tr>";
         oPageContent += "<td align=\"left\" colspan=\"2\"><label>Ap dung tu: </label><br></td>";
         DayTime_dtv_t rateTableDayTime, dispDayTime;
         TimeZone_dtv_t timeZone = _S_.getTimeZone();
         Second2DayTime(rateHeader.activatedDate, &rateTableDayTime);
         ChangeDayTimeZone(rateTableDayTime, &timeZone, &dispDayTime);
         GStringUtils::format(strTemp, "%02d/%02d/%04d %02d:%02d:%02d", dispDayTime.ymdw.day, dispDayTime.ymdw.month, dispDayTime.ymdw.year,
                                                                        dispDayTime.hms.hour, dispDayTime.hms.minute, dispDayTime.hms.second);
         oPageContent += "<td align=\"right\" colspan=\"3\"><label>" + strTemp + "</label><br></td>";
         oPageContent += "</tr>";
         oPageContent += "<tr>";
         oPageContent += "<td align=\"left\" colspan=\"2\"><label>Het han vao: </label><br></td>";
         Second2DayTime(rateHeader.expiredDate, &rateTableDayTime);
         ChangeDayTimeZone(rateTableDayTime, &timeZone, &dispDayTime);
         GStringUtils::format(strTemp, "%02d/%02d/%04d %02d:%02d:%02d", dispDayTime.ymdw.day, dispDayTime.ymdw.month, dispDayTime.ymdw.year,
                                                                        dispDayTime.hms.hour, dispDayTime.hms.minute, dispDayTime.hms.second);
         oPageContent += "<td align=\"right\" colspan=\"3\"><label>" + strTemp + "</label><br></td>";
         oPageContent += "</tr>";
         oPageContent += "<tr>";
         oPageContent += "<td align=\"left\" colspan=\"2\"><label>Tong so SP: </label><br></td>";
         GStringUtils::format(strTemp, "%d", rateHeader.totalProduct);
         oPageContent += "<td align=\"right\" colspan=\"3\"><label>" + strTemp + "</label><br></td>";
         oPageContent += "</tr>";
      }
   }

   oPageContent += "<tr>";
   oPageContent += "<th align=\"center\" bgcolor=\"#CCCCCC\"><label>TT</label></th>";
   oPageContent += "<th align=\"center\" bgcolor=\"#CCCCCC\"><label>Ma</label></th>";
   oPageContent += "<th align=\"center\" bgcolor=\"#CCCCCC\"><label>Ten</label></th>";
   oPageContent += "<th align=\"center\" bgcolor=\"#CCCCCC\"><label>Don vi</label></th>";
   oPageContent += "<th align=\"center\" bgcolor=\"#CCCCCC\"><label>Gia</label></th>";
   oPageContent += "</tr>";
   //loop
   page--;
   int NumOfItemOnPage = 10;
   int iNumOfPage = pCurRateTable->header.totalProduct/NumOfItemOnPage;

   if( pCurRateTable->header.totalProduct > 0 && (pCurRateTable->header.totalProduct % NumOfItemOnPage) == 0 )
   {
      iNumOfPage--;
   }

   if( page < 0 )
   {
      page = 0;
   }
   else if( page > iNumOfPage )
   {
      page = iNumOfPage;
   }

   int iFromProduct = page*NumOfItemOnPage;
   int iToProduct = iFromProduct + NumOfItemOnPage;

   if( iToProduct > pCurRateTable->header.totalProduct )
   {
      iToProduct = pCurRateTable->header.totalProduct;
   }

   for( int i = iFromProduct; i < iToProduct; ++i )
   {
      std::string sPid;
      std::string sPrice;
      std::string sCode;
      std::string sName;
      std::string sUnit;
      GStringUtils::format(sPid, "%d", pCurRateTable->products[i].id);
      GStringUtils::format(sPrice, "%.2f", pCurRateTable->products[i].price);
      GStringUtils::format(sCode, "%s", pCurRateTable->products[i].code);
      GStringUtils::format(sName, "%s", pCurRateTable->products[i].name);
      GStringUtils::format(sUnit, "%s", pCurRateTable->products[i].unit);
      oPageContent += "<tr>";
      oPageContent += "<td align=\"center\"><label>" + sPid + "</label></td>";
      oPageContent += "<td align=\"left\"><label>" + sCode + "</label></td>";
      oPageContent += "<td align=\"left\"><label>" + sName + "</label></td>";
      oPageContent += "<td align=\"center\"><label>" + sUnit + "</label></td>";
      oPageContent += "<td align=\"right\"><label>" + sPrice + "</label></td>";
      oPageContent += "</tr>";
   }
   //loop

   page++;
   GStringUtils::format(strTemp, "Trang(tu 1-%d):", iNumOfPage+1);
   std::string strPage;
   GStringUtils::format(strPage, "%d", page);
   oPageContent += "<tr>";
   oPageContent += "<td align=\"right\" colspan=\"5\" bgcolor=\"#CCCCCC\"><label>" + strTemp + "</label><input type=\"number\" value=\"" + strPage +"\" name=\"viewpagenumber\"></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"5\" bgcolor=\"#CCCCCC\"><button type=\"submit\">Xem bang gia</button></td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
   oPageContent += "</form>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";

   if( pCurRateTable != NULL )
   {
      delete pCurRateTable;
      pCurRateTable = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildSetupSrv(const std::string& sessionId, std::string& oPageContent)
{
   oPageContent =  "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><b><label>";
   oPageContent += HTML_MASTER_TITLE;
   oPageContent += HTML_TITLE_SETUPSRV;
   oPageContent += "</label></b><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\" valign=\"top\" bgcolor=\"#CCCCCC\"><br>";
   std::string menuPage = "";
   buildMenuContent(sessionId, menuPage);
   oPageContent += menuPage;
   oPageContent += "</td>";
   oPageContent += "<td>";
   oPageContent += "<form action=\"UPDATE_SERVER_SETTING\">";
   oPageContent += "<input type=\"hidden\" name=\"id\" value=\"" + sessionId + "\">";
   oPageContent += "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><label>Server du lieu</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   std::string sChecked = "";
   if( _U_.getSubmitData() == SETTING_ON )
   {
      sChecked = " checked";
   }
   else
   {
      sChecked = "";
   }
   oPageContent += "<td align=\"left\"><input type=\"checkbox\" name=\"submitsrv\"" + sChecked + ">Cho phep gui du lieu len Server</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   UrlInfo serverUrl;
   _U_.getSubmitDataUrl(serverUrl);
   oPageContent += "<td align=\"left\">";
   oPageContent += "<label>Dia chi Server:</label><br><input type=\"text\" name=\"urlsubmitdata\" value=\"" + serverUrl.url + "\"><br>";
   oPageContent += "<label>Port:</label><br><input type=\"text\" name=\"submitdataport\" value=\"" + serverUrl.port + "\"><br>";
   if( serverUrl.ssl == SETTING_ON )
   {
      sChecked = " checked";
   }
   else
   {
      sChecked = "";
   }
   oPageContent += "<input type=\"checkbox\" name=\"submitdatassl\"" + sChecked + ">Su dung SSL";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><label>Server cai dat tu xa</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   _U_.getConfigurationUrl(serverUrl);
   oPageContent += "<td align=\"left\">";
   oPageContent += "<label>Dia chi Server:</label><br><input type=\"text\" name=\"urlconfigsrv\" value=\"" + serverUrl.url + "\"><br>";
   oPageContent += "<label>Port:</label><br><input type=\"text\" name=\"configsrvport\" value=\"" + serverUrl.port + "\"><br>";
   if( serverUrl.ssl == SETTING_ON )
   {
      sChecked = " checked";
   }
   else
   {
      sChecked = "";
   }
   oPageContent += "<input type=\"checkbox\" name=\"configsrvssl\"" + sChecked + ">Su dung SSL";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><label>Email</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   if( _U_.getSubmitEmail() == SETTING_ON )
   {
      sChecked = " checked";
   }
   else
   {
      sChecked = "";
   }
   oPageContent += "<td align=\"left\"><input type=\"checkbox\" name=\"submitemail\"" + sChecked + ">Cho phep gui du lieu qua email</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   SenderMailServerCfg mailSrvCfg;
   MailAddressInfo mailReceiver;
   _U_.getEmailSender(mailSrvCfg);
   _U_.getEmailReceiver(mailReceiver);
   oPageContent += "<td align=\"left\">";
   oPageContent += "<label>Mail Server:</label><br><input type=\"text\" name=\"mailserver\" value=\"" + mailSrvCfg.outGoingServer + "\"><br>";
   oPageContent += "<label>Port:</label><br><input type=\"text\" name=\"mailport\" value=\"" + mailSrvCfg.outGoingPort + "\"><br>";
   oPageContent += "<label>Nguoi gui:</label><br><input type=\"text\" name=\"sendername\" value=\"" + mailSrvCfg.senderName + "\"><br>";
   oPageContent += "<label>Email gui:</label><br><input type=\"text\" name=\"username\" value=\"" + mailSrvCfg.username + "\"><br>";
   oPageContent += "<label>Mat khau:</label><br><input type=\"password\" name=\"password\" value=\"" + mailSrvCfg.password + "\"><br><br>";
   oPageContent += "<label>Nguoi nhan:</label><br><input type=\"text\" name=\"receivername\" value=\"" + mailReceiver.accName + "\"><br>";
   oPageContent += "<label>Email nhan:</label><br><input type=\"text\" name=\"receivermail\" value=\"" + mailReceiver.address + "\">";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><label>SMS</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   if( _U_.getSubmitSms() == SETTING_ON )
   {
      sChecked = " checked";
   }
   else
   {
      sChecked = "";
   }
   std::string subscriberNum;
   _U_.getSmsNumReceiver(subscriberNum);
   oPageContent += "<td align=\"left\"><input type=\"checkbox\" name=\"submitsms\"" + sChecked + ">Cho phep gui du lieu qua tin nhan SMS</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\"><label>So dien thoai:</label><br><input type=\"text\" name=\"subscribernum\" value=\"" + subscriberNum + "\"></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><button type=\"submit\">Cap nhat</button></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"right\"><a href=\"SCALE_REGISTER?id=" + sessionId + "\">Dang ky thiet bi</a></td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
   oPageContent += "</form>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
}

//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildSetupSAP(const std::string& sessionId, std::string& oPageContent)
{
   SoftAPInfo softApInfo;
   UserInfo adminInfo;
   DeviceWifiLocalIp localIp;
   ConnDeviceWifi::getInstance().getSoftApCfg(softApInfo);
   _U_.getUserConfigAP(adminInfo);
   ConnDeviceWifi::getInstance().getLocalIpAddress(localIp);

   oPageContent =  "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><b><label>";
   oPageContent += HTML_MASTER_TITLE;
   oPageContent += HTML_TITLE_SETUPAP;
   oPageContent += "</label></b><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\" valign=\"top\" bgcolor=\"#CCCCCC\"><br>";
   std::string menuPage = "";
   buildMenuContent(sessionId, menuPage);
   oPageContent += menuPage;
   oPageContent += "</td>";
   oPageContent += "<td>";
   oPageContent += "<form action=\"UPDATE_SOFTAP_SETTING\">";
   oPageContent += "<input type=\"hidden\" name=\"id\" value=\"" + sessionId + "\">";
   oPageContent += "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"3\" bgcolor=\"#CCCCCC\"><label>Cai Dat Access Point</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\">";
   oPageContent += "<label>Dia chi MAC:</label><br><input type=\"text\" name=\"macaddr\" value=\"" + localIp.apMacAddr + "\" disabled><br>";
   oPageContent += "<label>Dia chi IP:</label><br><input type=\"text\" name=\"ipaddr\" value=\"" + localIp.apIpAddr + "\" disabled><br><br>";
   oPageContent += "<label>Ten (SSID): 1078SW-XYZ</label><br><input type=\"text\" name=\"ssidsap\" value=\"" + softApInfo.ssid + "\"><br>";
   oPageContent += "<label>Mat khau (8):</label><br><input type=\"password\" name=\"passwordsap\" value=\"" + softApInfo.password + "\"><br>";
   std::string strTemp = "";
   switch(softApInfo.ecn)
   {
   case ECN_OPEN: strTemp = "NONE"; break;
   case ECN_WEP: strTemp = "WEP"; break;
   case ECN_WPA_PSK: strTemp = "WPA-PSK"; break;
   case ECN_WPA2_PSK: strTemp = "WPA2-PSK"; break;
   case ECN_WPA_WPA2_PSK: strTemp = "WPA-WPA2-PSK"; break;
   default: break;
   }
   oPageContent += "<label>Ma hoa:</label><br><input type=\"text\" name=\"encnetwork\" value=\"" + strTemp + "\" disabled><br>";
   GStringUtils::format(strTemp, "%d", softApInfo.channel);
   oPageContent += "<label>Kenh:</label><br><input type=\"text\" name=\"channel\" value=\"" + strTemp + "\" disabled><br>";
   GStringUtils::format(strTemp, "%d", softApInfo.maxConnection);
   oPageContent += "<label>So ket noi (1-3):</label><br><input type=\"text\" name=\"maxconn\" value=\"" + strTemp + "\" disabled><br>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><label>Tai khoan Admin</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\">";
   oPageContent += "<label>Ten:</label><br><input type=\"text\" name=\"usernameadmin\" value=\"" + adminInfo.username + "\"><br>";
   oPageContent += "<label>Mat khau:</label><br><input type=\"password\" name=\"passwordadmin\" value=\"" + adminInfo.password + "\">";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"3\" bgcolor=\"#CCCCCC\"><button type=\"submit\">Cap Nhat</button></td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
   oPageContent += "</form>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
}

//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildSetupPrinter(const std::string& sessionId, std::string& oPageContent)
{
   oPageContent =  "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><b><label>";
   oPageContent += HTML_MASTER_TITLE;
   oPageContent += HTML_TITLE_SETUPPRT;
   oPageContent += "</label></b><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\" valign=\"top\" bgcolor=\"#CCCCCC\"><br>";
   std::string menuPage = "";
   buildMenuContent(sessionId, menuPage);
   oPageContent += menuPage;
   oPageContent += "</td>";
   oPageContent += "<td>";
   oPageContent += "<form action=\"UPDATE_PRINTER_SETTING\">";
   oPageContent += "<input type=\"hidden\" name=\"id\" value=\"" + sessionId + "\">";
   oPageContent += "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><label>Cai dat may in</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   std::string sChecked = "";
   if( _U_.getPrintReceipt() == SETTING_ON )
   {
      sChecked = " checked";
   }
   else
   {
      sChecked = "";
   }
   oPageContent += "<td align=\"left\"><input type=\"checkbox\" name=\"printreceipt\"" + sChecked + ">Cho phep in hoa don (receipt)</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   if( _U_.getPrintLabel() == SETTING_ON )
   {
      sChecked = " checked";
   }
   else
   {
      sChecked = "";
   }
   oPageContent += "<td align=\"left\"><input type=\"checkbox\" name=\"printlabel\"" + sChecked + ">Cho phep in nhan (label)</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><button type=\"submit\">Cap Nhat</button></td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
   oPageContent += "</form>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
}

//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildScaleRegister(const std::string& sessionId, std::string& oPageContent)
{
   UrlInfo serverUrl;
   _U_.getSubmitDataUrl(serverUrl);
   int srvPort = GStringUtils::str2int(serverUrl.port);
   if( srvPort != 80 && srvPort != 0 )
   {
      serverUrl.url.append(":");
      serverUrl.url.append(serverUrl.port);
   }

   oPageContent =  "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><b><label>";
   oPageContent += HTML_MASTER_TITLE;
   oPageContent += HTML_TITLE_SCALEREG;
   oPageContent += "</label></b><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\" valign=\"top\" bgcolor=\"#CCCCCC\"><br>";
   std::string menuPage = "";
   buildMenuContent(sessionId, menuPage);
   oPageContent += menuPage;
   oPageContent += "</td>";
   oPageContent += "<td>";
   oPageContent += "<form action=\"UPDATE_SCALE_REGISTER\">";
   oPageContent += "<input type=\"hidden\" name=\"id\" value=\"" + sessionId + "\">";
   oPageContent += "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><label>Dang ky can len Server</label><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\">";
   oPageContent += "<label>Dia chi Server:</label><br><input type=\"text\" name=\"serverurl\" value=\"" + serverUrl.url + "\" disabled><br>";
   oPageContent += "<label>Ma can:</label><br><input type=\"text\" name=\"deviceidview\" value=\"" + ConnDeviceWifi::getInstance().m_deviceId + "\" disabled><br>";
   oPageContent += "<input type=\"hidden\" name=\"deviceid\" value=\"" + ConnDeviceWifi::getInstance().m_deviceId + "\">";
   oPageContent += "<label>Ma nguoi dung:</label><br><input type=\"text\" name=\"userid\"><br>";
   oPageContent += "<label>Noi dung dang ky:</label><br><input type=\"text\" name=\"regcontent\"><br>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><button type=\"submit\">Dang ky</button></td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
   oPageContent += "</form>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
}


//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildTestDevice(const std::string& sessionId, std::string& oPageContent)
{
   oPageContent =  "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><b><label>";
   oPageContent += HTML_MASTER_TITLE;
   oPageContent += HTML_TITLE_TEST;
   oPageContent += "</label></b><br></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"left\" valign=\"top\" bgcolor=\"#CCCCCC\"><br>";
   std::string menuPage = "";
   buildMenuContent(sessionId, menuPage);
   oPageContent += menuPage;
   oPageContent += "</td>";
   oPageContent += "<td>";
   oPageContent += "<form action=\"TEST_DEVICE\">";
   oPageContent += "<input type=\"hidden\" name=\"id\" value=\"" + sessionId + "\">";
   oPageContent += "<table>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><label>Thu may in (Printer)</label></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"right\"><a href=\"TEST_PRINTER?id=" + sessionId + "\">In thu</a></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" bgcolor=\"#CCCCCC\"><label>Thu ket noi toi Server</label></td>";
   oPageContent += "</tr>";
   DeviceWifiNetworkCfg stNetCfg;
   ConnDeviceWifi::getInstance().getStationNetworkCfg(stNetCfg);
   _U_.setDeviceNetworkConfiguration(stNetCfg);
   oPageContent += "<tr>";
   oPageContent += "<td align=\"right\">";
   oPageContent += "<label>ip: " + stNetCfg.ipaddr + "</label><br>";
   oPageContent += "<label>gateway: " + stNetCfg.gateway + "</label><br>";
   oPageContent += "<label>netmask: " + stNetCfg.netmask + "</label><br>";
   oPageContent += "<label>dns1: " + stNetCfg.dns1 + "</label><br>";
   oPageContent += "<label>dns2: " + stNetCfg.dns2 + "</label><br>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   UrlInfo serverUrl;
   _U_.getSubmitDataUrl(serverUrl);
   int iPort = GStringUtils::str2int(serverUrl.port);
   if( iPort != 0 && iPort != 80 )
   {
      serverUrl.url.append(":");
      serverUrl.url.append(serverUrl.port);
   }
   oPageContent += "<tr>";
   oPageContent += "<td align=\"right\"><label>Data Server: " + serverUrl.url + "</label></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"right\"><a href=\"TEST_DATA_SERVER_CONN?id=" + sessionId + "\">Ket noi</a></td>";
   oPageContent += "</tr>";
   _U_.getConfigurationUrl(serverUrl);
   iPort = GStringUtils::str2int(serverUrl.port);
   if( iPort != 0 && iPort != 80 )
   {
      serverUrl.url.append(":");
      serverUrl.url.append(serverUrl.port);
   }
   oPageContent += "<tr>";
   oPageContent += "<td align=\"right\"><label>Conf Server: " + serverUrl.url + "</label></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"right\"><a href=\"TEST_CONF_SERVER_CONN?id=" + sessionId + "\">Ket noi</a></td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
   oPageContent += "</form>";
   oPageContent += "</td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
}

//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildErrorMessage(const std::string& sessionId, std::string& oPageContent, HttpErrorContent httpError)
{
   std::string formAction = "LOG_IN_FAILED";
   std::string titleMessage = "Loi: ";
   std::string mainMessage = "Khong xac dinh loi !";
   std::string buttonCaption = "Dang nhap lai";

   switch( httpError )
   {
   case HTTP_ERR_LOGINFAILED:
      formAction = "LOG_IN_FAILED";
      titleMessage = "Loi dang nhap: ";
      mainMessage = "Ten dang nhap hoac Mat khau sai !";
      buttonCaption = "Dang nhap lai";
      break;
   case HTTP_ERR_LOGINLIMIT:
      formAction = "LOG_IN_FAILED";
      titleMessage = "Chu y: ";
      mainMessage = "Da co nguoi khac dang nhap, vui long thu lai sau !";
      buttonCaption = "Dang nhap lai";
      break;
   case HTTP_ERR_LOGGEDOUT:
      formAction = "LOG_OUT_DONE";
      titleMessage = "Chu y: ";
      mainMessage = "Da thoat khoi he thong !";
      buttonCaption = "Dang nhap lai";
      break;
   case HTTP_ERR_REGISTER_OK:
      formAction = "SETUP_SERVER";
      titleMessage = "Chu y: ";
      mainMessage = "Dang ky thiet bi thanh cong !";
      buttonCaption = "Quay tro lai";
      break;
   case HTTP_ERR_WIFI_JAP_OK:
      formAction = "TEST_DEVICE";
      titleMessage = "Chu y: ";
      mainMessage = "Ket noi WIFI thanh cong !";
      buttonCaption = "Quay tro lai";
      break;
   case HTTP_ERR_WIFI_JAP_NOK:
      formAction = "WIFI_LIST";
      titleMessage = "Loi: ";
      mainMessage = "Khong the ket noi WIFI, vui long kiem tra lai !";
      buttonCaption = "Quay tro lai";
      break;
   case HTTP_ERR_REGISTER_NOK:
      formAction = "SCALE_REGISTER";
      titleMessage = "Loi: ";
      mainMessage = "Khong dang ky duoc thiet bi !";
      buttonCaption = "Quay tro lai";
      break;
   case HTTP_ERR_REGISTER_NOK_USER:
      formAction = "SCALE_REGISTER";
      titleMessage = "Loi dang ky: ";
      mainMessage = "Ma nguoi dung sai, khong dang ky duoc thiet bi !";
      buttonCaption = "Quay tro lai";
      break;
   case HTTP_ERR_REGISTER_NOK_EXIST:
      formAction = "SETUP_SERVER";
      titleMessage = "Loi dang ky: ";
      mainMessage = "Thiet bi da duoc dang ky tren he thong!";
      buttonCaption = "Quay tro lai";
      break;
   case HTTP_ERR_CONNECT_OK:
      formAction = "TEST_DEVICE";
      titleMessage = "Chu y: ";
      mainMessage = "Ket noi toi server thanh cong !";
      buttonCaption = "Quay tro lai";
      break;
   case HTTP_ERR_CONNECT_NOK:
      formAction = "TEST_DEVICE";
      titleMessage = "Loi ket noi: ";
      mainMessage = "Khong the ket noi toi server !";
      buttonCaption = "Quay tro lai";
      break;
   case HTTP_ERR_SAVE_SETTINGS_OK:
      formAction = "SETUP_SOFTAP";
      titleMessage = "Chu y: ";
      mainMessage = "Luu cac thay doi thanh cong !";
      buttonCaption = "Quay tro lai";
      break;
   case HTTP_ERR_SAVE_SETTINGS_NOK:
      formAction = "SETUP_SOFTAP";
      titleMessage = "Loi: ";
      mainMessage = "Khong the ket luu cac thay doi !";
      buttonCaption = "Quay tro lai";
      break;
   default:
      break;
   }

   oPageContent += "<form action=\"" + formAction + "\">";
   oPageContent += "<input type=\"hidden\" name=\"id\" value=\"" + sessionId + "\">";
   oPageContent += "<table cellpadding=\"3\">";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><b><label>";
   oPageContent += HTML_MASTER_TITLE;
   oPageContent += "</label></b></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td><label>" + titleMessage + "</label></td>";
   oPageContent += "<td><label><font color=\"red\">" + mainMessage + "</font></label></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"></td>";
   oPageContent += "</tr>";
   oPageContent += "<tr>";
   oPageContent += "<td align=\"center\" colspan=\"2\"><button type=\"submit\">" + buttonCaption + "</button></td>";
   oPageContent += "</tr>";
   oPageContent += "</table>";
   oPageContent += "</form>";
}

//////////////////////////////////////////////////////////////////////////////////////
void HtmlDocument::buildMenuContent(const std::string& sessionId, std::string& oPageContent)
{
   oPageContent = "<ul style=\"list-style-type:square\">";
   oPageContent += "<li><a href=\"SETUP_SOFTAP?id=" + sessionId + "\"><b>Cai dat mat khau </b></a></li><br>";
   oPageContent += "<li><a href=\"SETUP_SERVER?id=" + sessionId + "\"><b>Cai dat truyen tin </b></a></li><br>";
   oPageContent += "<li><a href=\"SETUP_PRINTER?id=" + sessionId + "\"><b>Cai dat may in </b></a></li><br>";
   oPageContent += "<li><a href=\"WIFI_LIST?id=" + sessionId + "\"><b>Cai dat Wifi</b></a></li><br>";
   oPageContent += "<li><a href=\"RATE_TABLE?id=" + sessionId + "\"><b>Bang gia</b></a></li><br>";
   oPageContent += "<li><a href=\"TEST_DEVICE?id=" + sessionId + "\"><b>Kiem tra ket noi</b></a></li><br>";
   oPageContent += "<li><a href=\"SAVE_SETTINGS?id=" + sessionId + "\"><b>Luu cac thay doi</b></a></li><br>";
   oPageContent += "<li><a href=\"LOG_OUT?id=" + sessionId + "\"><b>Thoat</b></a></li>";
   oPageContent += "</ul>";
}
//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
