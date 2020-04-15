##//////////////////////////////////////////////////////////////////////////////////##
## Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   ##
## This source is subject to the GBS Permissive License. Please see the License.txt ##
## file for more information.                                                       ##
##                                                                                  ##
## The makefile for gbsgui mainapp                                                  ##
##//////////////////////////////////////////////////////////////////////////////////##

include ../gbsgui.mk

SRC     = $(subst $(BAD_SLASH),$(GOOD_SLASH),$(CURDIR))$(SLASH)
ODIR 	= ..$(SLASH)$(APP_BDIR)$(SLASH)

INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(API_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(RES_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(APP_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))/UIControl
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(APP_SRC_DIR))/Business
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(APP_SRC_DIR))/Business/DataController
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(APP_SRC_DIR))/Business/UsbManager
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(APP_SRC_DIR))/Business/InstallManager
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(APP_SRC_DIR))/Business/EScaleManager

DEP_BASIC = $(SRC)mainapp.hpp \
		$(SRC)Business/DataController/GUserSettings.hpp \
		$(SRC)Business/DataController/SaveDataToFlash.hpp \
		$(SRC)Business/UsbManager/UsbManager.hpp \
		$(SRC)Business/UsbManager/UsbDevice.hpp \
		$(SRC)Business/UsbManager/UsbMassStorage.hpp \
		$(SRC)Business/InstallManager/InstallManager.hpp \
		$(SRC)Business/EScaleManager/SerialPort.hpp \
		$(SRC)Business/EScaleManager/ScaleManager.hpp \
		$(SRC)Business/EScaleManager/BarcodeReader.hpp \
		$(SRC)Business/EScaleManager/PrinterManager.hpp \
		$(SRC)Business/EScaleManager/ConnManager.hpp \
		$(SRC)Business/EScaleManager/ConnDevice.hpp \
		$(SRC)Business/EScaleManager/ConnDevice3G.hpp \
		$(SRC)Business/EScaleManager/ConnDeviceWifi.hpp \
		$(SRC)Business/EScaleManager/HtmlDocument.hpp \
		$(SRC)Business/EScaleManager/Webserver.hpp \
		$(SRC)Business/EScaleManager/RateManager.hpp \
		$(SRC)Business/EScaleManager/JsonParser.hpp \

OBJS=	$(ODIR)mainapp.o \
		$(ODIR)GUserSettings.o \
		$(ODIR)SaveDataToFlash.o \
		$(ODIR)UsbManager.o \
		$(ODIR)UsbDevice.o \
		$(ODIR)UsbMassStorage.o \
		$(ODIR)InstallManager.o \
		$(ODIR)SerialPort.o \
		$(ODIR)ScaleManager.o \
		$(ODIR)BarcodeReader.o \
		$(ODIR)PrinterManager.o \
		$(ODIR)ConnManager.o \
		$(ODIR)ConnDevice.o \
		$(ODIR)ConnDevice3G.o \
		$(ODIR)ConnDeviceWifi.o \
		$(ODIR)HtmlDocument.o \
		$(ODIR)Webserver.o \
		$(ODIR)RateManager.o \
		$(ODIR)JsonParser.o \
		#$(ODIR)others.o               
         
ifeq "$(DVD_OS)" "LINUX"
	C_CC_MAINAPP  = $(C_CC_TOOLSET) -g -O0 $(C_CC_GLOBAL)
	#C_CC_others  = $(C_CC_TOOLSET) -g -O0 $(C_CC_GLOBAL)
else 
# OS21
	C_CC_MAINAPP  = -mruntime=os21 -g -O0 $(C_CC_GLOBAL)
	#C_CC_others  = -mruntime=os21 -g -O0 $(C_CC_GLOBAL)
endif            

all : create_obj_dirs $(APP_LIB)
ifeq "$(ARCHITECTURE)" "ST20"
	@$(AR) -o $(ODIR)$(APP_LIB_NAME) $(OBJS)
endif
ifeq "$(ARCHITECTURE)" "ST40"
	@$(AR) -r $(ODIR)$(APP_LIB_NAME)  $(OBJS)
endif

create_obj_dirs:
ifeq "$(wildcard $(ODIR))" ""
	-@$(MKDIR) $(ODIR)
endif

$(APP_LIB): $(OBJS)
$(ODIR)mainapp.o	: $(SRC)mainapp.cpp $(SRC)mainapp.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)mainapp.cpp -o $@ $(C_CC_MAINAPP)
	
$(ODIR)GUserSettings.o : $(SRC)Business/DataController/GUserSettings.cpp $(SRC)Business/DataController/GUserSettings.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/DataController/GUserSettings.cpp -o $@ $(C_CC_MAINAPP)

$(ODIR)SaveDataToFlash.o : $(SRC)Business/DataController/SaveDataToFlash.cpp $(SRC)Business/DataController/SaveDataToFlash.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/DataController/SaveDataToFlash.cpp -o $@ $(C_CC_MAINAPP)	

$(ODIR)UsbManager.o : $(SRC)Business/UsbManager/UsbManager.cpp $(SRC)Business/UsbManager/UsbManager.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/UsbManager/UsbManager.cpp -o $@ $(C_CC_MAINAPP)

$(ODIR)UsbDevice.o : $(SRC)Business/UsbManager/UsbDevice.cpp $(SRC)Business/UsbManager/UsbDevice.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/UsbManager/UsbDevice.cpp -o $@ $(C_CC_MAINAPP)
	
$(ODIR)UsbMassStorage.o : $(SRC)Business/UsbManager/UsbMassStorage.cpp $(SRC)Business/UsbManager/UsbMassStorage.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/UsbManager/UsbMassStorage.cpp -o $@ $(C_CC_MAINAPP)

$(ODIR)InstallManager.o : $(SRC)Business/InstallManager/InstallManager.cpp $(SRC)Business/InstallManager/InstallManager.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/InstallManager/InstallManager.cpp -o $@ $(C_CC_MAINAPP)

$(ODIR)SerialPort.o		:	$(SRC)Business/EScaleManager/SerialPort.cpp		$(SRC)Business/EScaleManager/SerialPort.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/SerialPort.cpp		-o $@			$(C_CC_MAINAPP)

$(ODIR)ScaleManager.o		:	$(SRC)Business/EScaleManager/ScaleManager.cpp	$(SRC)Business/EScaleManager/ScaleManager.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/ScaleManager.cpp		-o $@		$(C_CC_MAINAPP)

$(ODIR)BarcodeReader.o		:	$(SRC)Business/EScaleManager/BarcodeReader.cpp	$(SRC)Business/EScaleManager/BarcodeReader.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/BarcodeReader.cpp		-o $@		$(C_CC_MAINAPP)
	
$(ODIR)PrinterManager.o		:	$(SRC)Business/EScaleManager/PrinterManager.cpp	$(SRC)Business/EScaleManager/PrinterManager.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/PrinterManager.cpp		-o $@		$(C_CC_MAINAPP)
	
$(ODIR)ConnManager.o		:	$(SRC)Business/EScaleManager/ConnManager.cpp	$(SRC)Business/EScaleManager/ConnManager.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/ConnManager.cpp		-o $@			$(C_CC_MAINAPP)
	
$(ODIR)ConnDevice.o			:	$(SRC)Business/EScaleManager/ConnDevice.cpp		$(SRC)Business/EScaleManager/ConnDevice.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/ConnDevice.cpp		-o $@			$(C_CC_MAINAPP)
	
$(ODIR)ConnDevice3G.o		:	$(SRC)Business/EScaleManager/ConnDevice3G.cpp	$(SRC)Business/EScaleManager/ConnDevice3G.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/ConnDevice3G.cpp		-o $@		$(C_CC_MAINAPP)
	
$(ODIR)ConnDeviceWifi.o		:	$(SRC)Business/EScaleManager/ConnDeviceWifi.cpp		$(SRC)Business/EScaleManager/ConnDeviceWifi.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/ConnDeviceWifi.cpp		-o $@			$(C_CC_MAINAPP)
	
$(ODIR)HtmlDocument.o		:	$(SRC)Business/EScaleManager/HtmlDocument.cpp	$(SRC)Business/EScaleManager/HtmlDocument.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/HtmlDocument.cpp		-o $@			$(C_CC_MAINAPP)
		
$(ODIR)Webserver.o		:	$(SRC)Business/EScaleManager/Webserver.cpp		$(SRC)Business/EScaleManager/Webserver.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/Webserver.cpp		-o $@			$(C_CC_MAINAPP)				

$(ODIR)RateManager.o		:	$(SRC)Business/EScaleManager/RateManager.cpp		$(SRC)Business/EScaleManager/RateManager.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/RateManager.cpp		-o $@			$(C_CC_MAINAPP)				
				
$(ODIR)JsonParser.o		:	$(SRC)Business/EScaleManager/JsonParser.cpp		$(SRC)Business/EScaleManager/JsonParser.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)Business/EScaleManager/JsonParser.cpp		-o $@			$(C_CC_MAINAPP)				

purge: purge_objs purge_lib
ifneq "$(wildcard $(ODIR))" ""
	-@$(RMDIR) $(ODIR)
endif
purge_objs:
	-@$(RM) $(OBJS)
purge_lib:
	-@$(RM) $(ODIR)$(APP_LIB_NAME)

