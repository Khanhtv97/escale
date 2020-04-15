##//////////////////////////////////////////////////////////////////////////////////##
## Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   ##
## This source is subject to the GBS Permissive License. Please see the License.txt ##
## file for more information.                                                       ##
##                                                                                  ##
## The makefile for gbsgui Kachiusa Framework                                       ##
##//////////////////////////////////////////////////////////////////////////////////##

include ../gbsgui.mk

SRC     = $(subst $(BAD_SLASH),$(GOOD_SLASH),$(CURDIR))$(SLASH)
ODIR 	= ..$(SLASH)$(FW_BDIR)$(SLASH)

INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(API_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(RES_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))
#INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../../$(EXTRA_BDIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(APP_SRC_DIR))

INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))/vnconv/include
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))/UIControl
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))/ImageDecode
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))/QrEncode
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))/Json
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))/tinyxml

DEP_BASIC =	$(SRC)crc32.hpp \
			$(SRC)GDebug.hpp \
			$(SRC)GDelegate.hpp \
			$(SRC)GEvents.hpp \
			$(SRC)GEventManager.hpp \
			$(SRC)GError.hpp \
			$(SRC)GKeyManager.hpp \
			$(SRC)GKey.hpp \
			$(SRC)GLang_EN.hpp \
			$(SRC)GLang_VN.hpp \
			$(SRC)GObject.hpp \
			$(SRC)GDrawable.hpp \
			$(SRC)GLanguage.hpp \
			$(SRC)GSystem.hpp \
			$(SRC)GUtility.hpp \
			$(SRC)GStringUtils.hpp \
			$(SRC)md5.hpp \
			$(SRC)GFileSystem.hpp \
			$(SRC)GTimer.hpp \
			$(SRC)GLongTimer.hpp \
			$(SRC)GROMManager.hpp \
			$(SRC)GXml.hpp \
			$(SRC)/tinyxml/tinystr.h \
			$(SRC)/tinyxml/tinyxml.h \
			$(SRC)/vnconv/include/vnconv.h \
			$(SRC)/vnconv/include/vncharmap.h \
			$(SRC)/UIControl/GView.hpp \
			$(SRC)/UIControl/GStaticControl.hpp \
			$(SRC)/UIControl/GDynamicControl.hpp \
			$(SRC)/UIControl/GFocusable.hpp \
			$(SRC)/UIControl/GSelectable.hpp \
			$(SRC)/UIControl/GButton.hpp \
			$(SRC)/UIControl/GButtonEx.hpp \
			$(SRC)/UIControl/GCheckBox.hpp \
			$(SRC)/UIControl/GRadioButton.hpp \
			$(SRC)/UIControl/GActivity.hpp \
			$(SRC)/UIControl/GList.hpp \
			$(SRC)/UIControl/GListItemBase.hpp \
			$(SRC)/UIControl/GListItem.hpp \
			$(SRC)/UIControl/GListItemEx.hpp \
			$(SRC)/UIControl/GScrollBar.hpp \
			$(SRC)/UIControl/GInputBox.hpp \
			$(SRC)/UIControl/GNavigator.hpp \
			$(SRC)/UIControl/GProgressBar.hpp \
			$(SRC)/UIControl/GProgBar.hpp \
			$(SRC)/UIControl/GTextBox.hpp \
			$(SRC)/ImageDecode/lodepng.h \
			$(SRC)/ImageDecode/nanojpeg.h \
			$(SRC)/ImageDecode/nanobmp.h \
			$(SRC)/ImageDecode/GImageDecode.hpp \
			$(SRC)/QrEncode/bitstream.h \
			$(SRC)/QrEncode/mask.h \
			$(SRC)/QrEncode/mmask.h \
			$(SRC)/QrEncode/mqrspec.h \
			$(SRC)/QrEncode/qrencode.h \
			$(SRC)/QrEncode/qrencode_inner.h \
			$(SRC)/QrEncode/qrinput.h \
			$(SRC)/QrEncode/qrspec.h \
			$(SRC)/QrEncode/rscode.h \
			$(SRC)/QrEncode/split.h \
			$(SRC)/Json/json.h \
			$(SRC)/Json/features.h \
			$(SRC)/Json/reader.h \
			$(SRC)/Json/writer.h \
			$(SRC)/Json/value.h \
			$(SRC)/Json/json_valueiterator.inl \
			#$(SRC)others.hpp 

OBJS=	 	$(ODIR)GDebug.o \
			$(ODIR)GDelegate.o \
			$(ODIR)GEventManager.o \
			$(ODIR)GKeyManager.o \
			$(ODIR)GObject.o \
			$(ODIR)GDrawable.o \
			$(ODIR)GLanguage.o \
			$(ODIR)GSystem.o \
			$(ODIR)vnconv.o \
			$(ODIR)GView.o \
			$(ODIR)GStaticControl.o \
			$(ODIR)GDynamicControl.o \
			$(ODIR)GFocusable.o \
			$(ODIR)GSelectable.o \
			$(ODIR)GButton.o \
			$(ODIR)GButtonEx.o \
			$(ODIR)GCheckBox.o \
			$(ODIR)GRadioButton.o \
			$(ODIR)GActivity.o \
			$(ODIR)GList.o \
			$(ODIR)GListItemBase.o \
			$(ODIR)GListItem.o \
			$(ODIR)GListItemEx.o \
			$(ODIR)GScrollBar.o \
			$(ODIR)GInputBox.o \
			$(ODIR)GNavigator.o \
			$(ODIR)GProgressBar.o \
			$(ODIR)GProgBar.o \
			$(ODIR)GTextBox.o \
			$(ODIR)GUtility.o \
			$(ODIR)GStringUtils.o \
			$(ODIR)md5.o \
			$(ODIR)GFileSystem.o \
			$(ODIR)GTimer.o \
			$(ODIR)GLongTimer.o \
			$(ODIR)GROMManager.o \
			$(ODIR)GXml.o \
			$(ODIR)tinystr.o \
			$(ODIR)tinyxml.o \
			$(ODIR)tinyxmlparser.o \
			$(ODIR)tinyxmlerror.o \
			$(ODIR)crc32.o \
			$(ODIR)lodepng.o \
			$(ODIR)nanojpeg.o \
			$(ODIR)nanobmp.o \
			$(ODIR)GImageDecode.o \
			$(ODIR)jsonreader.o \
			$(ODIR)jsonwriter.o \
			$(ODIR)jsonvalue.o \
        	$(ODIR)qrencode.o \
			$(ODIR)qrspec.o \
			$(ODIR)qrinput.o \
			$(ODIR)bitstream.o \
			$(ODIR)mask.o \
			$(ODIR)mmask.o \
			$(ODIR)mqrspec.o \
			$(ODIR)rscode.o \
			$(ODIR)split.o \
			#$(ODIR)others.o
			               
         
ifeq "$(DVD_OS)" "LINUX"
	C_CC_FRAMEWORK = $(C_CC_TOOLSET) -g -O0 $(C_CC_GLOBAL)
	#C_CC_others   = $(C_CC_TOOLSET) -g -O0 $(C_CC_GLOBAL)
else
	#OS21
	C_CC_FRAMEWORK = -mruntime=os21 -g -O0 $(C_CC_GLOBAL)
	#C_CC_others   = -mruntime=os21 -g -O0 $(C_CC_GLOBAL)
endif            

all : create_obj_dirs $(FW_LIB)
ifeq "$(ARCHITECTURE)" "ST20"
 @$(AR) -o $(ODIR)$(FW_LIB_NAME) $(OBJS) 
endif
ifeq "$(ARCHITECTURE)" "ST40"
	@$(AR) -r $(ODIR)$(FW_LIB_NAME)  $(OBJS)
endif

create_obj_dirs:
ifeq "$(wildcard $(ODIR))" ""
	-@$(MKDIR) $(ODIR)
endif

$(FW_LIB): $(OBJS) $(DEP_BASIC)

$(ODIR)GDebug.o				: 	$(SRC)GDebug.cpp 			$(SRC)GDebug.hpp 		
	$(CC_PLUS) $(SRC)GDebug.cpp				-o $@ 			$(C_CC_FRAMEWORK)

$(ODIR)GDelegate.o			: 	$(SRC)GDelegate.cpp 		$(SRC)GDelegate.hpp 	
	$(CC_PLUS) $(SRC)GDelegate.cpp 			-o $@ 			$(C_CC_FRAMEWORK)

$(ODIR)GEventManager.o		: 	$(SRC)GEventManager.cpp		$(SRC)GEventManager.hpp 	
	$(CC_PLUS) $(SRC)GEventManager.cpp		-o $@ 			$(C_CC_FRAMEWORK)
	
$(ODIR)GKeyManager.o			: 	$(SRC)GKeyManager.cpp 		$(SRC)GKeyManager.hpp 	
	$(CC_PLUS) $(SRC)GKeyManager.cpp 		-o $@ 			$(C_CC_FRAMEWORK)
	
$(ODIR)GObject.o				: 	$(SRC)GObject.cpp 			$(SRC)GObject.hpp 	
	$(CC_PLUS) $(SRC)GObject.cpp	 			-o $@ 			$(C_CC_FRAMEWORK)

$(ODIR)GDrawable.o			: 	$(SRC)GDrawable.cpp 		$(SRC)GDrawable.hpp  	
	$(CC_PLUS) $(SRC)GDrawable.cpp 			-o $@ 			$(C_CC_FRAMEWORK)
		
$(ODIR)GLanguage.o			: 	$(SRC)GLanguage.cpp 		$(SRC)GLanguage.hpp $(SRC)GLang_EN.hpp $(SRC)GLang_VN.hpp 	
	$(CC_PLUS) $(SRC)GLanguage.cpp 			-o $@ 			$(C_CC_FRAMEWORK)
		
$(ODIR)GSystem.o				: 	$(SRC)GSystem.cpp 			$(SRC)GSystem.hpp 	
	$(CC_PLUS) $(SRC)GSystem.cpp 				-o $@ 			$(C_CC_FRAMEWORK)
	
$(ODIR)vnconv.o				: 	$(SRC)/vnconv/vnconv.c 		$(SRC)/vnconv/include/*	
	$(CC_PLUS) $(SRC)/vnconv/vnconv.c		-o $@ 			$(C_CC_FRAMEWORK)
		
$(ODIR)GView.o					: 	$(SRC)/UIControl/GView.cpp				$(SRC)/UIControl/GView.hpp
	$(CC_PLUS) $(SRC)/UIControl/GView.cpp			-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GStaticControl.o		: 	$(SRC)/UIControl/GStaticControl.cpp		$(SRC)/UIControl/GStaticControl.hpp
	$(CC_PLUS) $(SRC)/UIControl/GStaticControl.cpp	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GDynamicControl.o	: 	$(SRC)/UIControl/GDynamicControl.cpp	$(SRC)/UIControl/GDynamicControl.hpp
	$(CC_PLUS) $(SRC)/UIControl/GDynamicControl.cpp	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GFocusable.o			: 	$(SRC)/UIControl/GFocusable.cpp			$(SRC)/UIControl/GFocusable.hpp
	$(CC_PLUS) $(SRC)/UIControl/GFocusable.cpp	 	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GSelectable.o			: 	$(SRC)/UIControl/GSelectable.cpp		$(SRC)/UIControl/GFocusable.hpp
	$(CC_PLUS) $(SRC)/UIControl/GSelectable.cpp	 	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GButton.o				: 	$(SRC)/UIControl/GButton.cpp			$(SRC)/UIControl/GButton.hpp
	$(CC_PLUS) $(SRC)/UIControl/GButton.cpp		 	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GButtonEx.o			: 	$(SRC)/UIControl/GButtonEx.cpp			$(SRC)/UIControl/GButtonEx.hpp
	$(CC_PLUS) $(SRC)/UIControl/GButtonEx.cpp			 -o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GCheckBox.o			: 	$(SRC)/UIControl/GCheckBox.cpp			$(SRC)/UIControl/GCheckBox.hpp
	$(CC_PLUS) $(SRC)/UIControl/GCheckBox.cpp		 	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GRadioButton.o		: 	$(SRC)/UIControl/GRadioButton.cpp		$(SRC)/UIControl/GRadioButton.hpp
	$(CC_PLUS) $(SRC)/UIControl/GRadioButton.cpp	 	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GActivity.o			: 	$(SRC)/UIControl/GActivity.cpp			$(SRC)/UIControl/GActivity.hpp
	$(CC_PLUS) $(SRC)/UIControl/GActivity.cpp	 		-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GList.o					: 	$(SRC)/UIControl/GList.cpp				$(SRC)/UIControl/GList.hpp
	$(CC_PLUS) $(SRC)/UIControl/GList.cpp	 			-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GListItemBase.o		: 	$(SRC)/UIControl/GListItemBase.cpp		$(SRC)/UIControl/GListItemBase.hpp
	$(CC_PLUS) $(SRC)/UIControl/GListItemBase.cpp	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GListItem.o			: 	$(SRC)/UIControl/GListItem.cpp			$(SRC)/UIControl/GListItem.hpp
	$(CC_PLUS) $(SRC)/UIControl/GListItem.cpp	 	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GListItemEx.o			: 	$(SRC)/UIControl/GListItemEx.cpp		$(SRC)/UIControl/GListItemEx.hpp
	$(CC_PLUS) $(SRC)/UIControl/GListItemEx.cpp		-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GScrollBar.o			: 	$(SRC)/UIControl/GScrollBar.cpp			$(SRC)/UIControl/GScrollBar.hpp
	$(CC_PLUS) $(SRC)/UIControl/GScrollBar.cpp	 	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GNavigator.o			: 	$(SRC)/UIControl/GNavigator.cpp			$(SRC)/UIControl/GNavigator.hpp
	$(CC_PLUS) $(SRC)/UIControl/GNavigator.cpp	 	-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GInputBox.o			: 	$(SRC)/UIControl/GInputBox.cpp			$(SRC)/UIControl/GInputBox.hpp
	$(CC_PLUS) $(SRC)/UIControl/GInputBox.cpp	 	-o $@				$(C_CC_FRAMEWORK)

$(ODIR)GProgressBar.o		: 	$(SRC)/UIControl/GProgressBar.cpp		$(SRC)/UIControl/GProgressBar.hpp
	$(CC_PLUS) $(SRC)/UIControl/GProgressBar.cpp		-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GProgBar.o				: 	$(SRC)/UIControl/GProgBar.cpp		$(SRC)/UIControl/GProgBar.hpp
	$(CC_PLUS) $(SRC)/UIControl/GProgBar.cpp			-o $@				$(C_CC_FRAMEWORK)

$(ODIR)GTextBox.o				: 	$(SRC)/UIControl/GTextBox.cpp				$(SRC)/UIControl/GTextBox.hpp
	$(CC_PLUS) $(SRC)/UIControl/GTextBox.cpp			-o $@					$(C_CC_FRAMEWORK)
	
$(ODIR)GUtility.o				:	$(SRC)GUtility.cpp						$(SRC)GUtility.hpp
	$(CC_PLUS) $(SRC)GUtility.cpp							-o $@							$(C_CC_FRAMEWORK)
		
$(ODIR)GStringUtils.o		: 	$(SRC)/GStringUtils.cpp					$(SRC)/GStringUtils.hpp
	$(CC_PLUS) $(SRC)/GStringUtils.cpp	 				-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)md5.o					: 	$(SRC)/md5.cpp							$(SRC)/md5.hpp
	$(CC_PLUS) $(SRC)/md5.cpp	 							-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GFileSystem.o			: 	$(SRC)/GFileSystem.cpp					$(SRC)/GFileSystem.hpp
	$(CC_PLUS) $(SRC)/GFileSystem.cpp	 				-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GTimer.o				: 	$(SRC)/GTimer.cpp						$(SRC)/GTimer.hpp
	$(CC_PLUS) $(SRC)/GTimer.cpp	 						-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)GLongTimer.o			: 	$(SRC)/GLongTimer.cpp				$(SRC)/GLongTimer.hpp
	$(CC_PLUS) $(SRC)/GLongTimer.cpp	 					-o $@			$(C_CC_FRAMEWORK)
	
$(ODIR)GROMManager.o			: 	$(SRC)/GROMManager.cpp			$(SRC)/GROMManager.hpp
	$(CC_PLUS) $(SRC)/GROMManager.cpp	 				-o $@			$(C_CC_FRAMEWORK)

$(ODIR)crc32.o					: 	$(SRC)/crc32.cpp						$(SRC)/crc32.hpp
	$(CC_PLUS) $(SRC)/crc32.cpp	 						-o $@				$(C_CC_FRAMEWORK)

$(ODIR)GXml.o					: 	$(SRC)/GXml.cpp						$(SRC)/GXml.hpp
	$(CC_PLUS) $(SRC)/GXml.cpp	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)tinystr.o				: 	$(SRC)/tinyxml/tinystr.cpp 		$(SRC)/tinyxml/tinystr.h	
	$(CC_PLUS) $(SRC)/tinyxml/tinystr.cpp				-o $@ 			$(C_CC_FRAMEWORK)
	
$(ODIR)tinyxml.o				: 	$(SRC)/tinyxml/tinyxml.cpp 		$(SRC)/tinyxml/tinyxml.h	
	$(CC_PLUS) $(SRC)/tinyxml/tinyxml.cpp				-o $@ 			$(C_CC_FRAMEWORK)

$(ODIR)tinyxmlparser.o			: 	$(SRC)/tinyxml/tinyxmlparser.cpp 	$(SRC)/tinyxml/tinyxml.h	
	$(CC_PLUS) $(SRC)/tinyxml/tinyxmlparser.cpp			-o $@ 			$(C_CC_FRAMEWORK)
	
$(ODIR)tinyxmlerror.o			: 	$(SRC)/tinyxml/tinyxmlerror.cpp 	$(SRC)/tinyxml/tinyxml.h	
	$(CC_PLUS) $(SRC)/tinyxml/tinyxmlerror.cpp			-o $@ 			$(C_CC_FRAMEWORK)

$(ODIR)lodepng.o					: 	$(SRC)/ImageDecode/lodepng.cpp					$(SRC)/ImageDecode/lodepng.h
	$(CC_PLUS) $(SRC)/ImageDecode/lodepng.cpp	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)nanojpeg.o					: 	$(SRC)/ImageDecode/nanojpeg.cpp					$(SRC)/ImageDecode/nanojpeg.h
	$(CC_PLUS) $(SRC)/ImageDecode/nanojpeg.cpp	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)nanobmp.o					: 	$(SRC)/ImageDecode/nanobmp.cpp					$(SRC)/ImageDecode/nanobmp.h
	$(CC_PLUS) $(SRC)/ImageDecode/nanobmp.cpp	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)GImageDecode.o				: 	$(SRC)/ImageDecode/GImageDecode.cpp				$(SRC)/ImageDecode/GImageDecode.hpp
	$(CC_PLUS) $(SRC)/ImageDecode/GImageDecode.cpp 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)bitstream.o					: 	$(SRC)/QrEncode/bitstream.c						$(SRC)/QrEncode/bitstream.h
	$(CC_PLUS) $(SRC)/QrEncode/bitstream.c	 						-o $@				$(C_CC_FRAMEWORK)

$(ODIR)mask.o					: 	$(SRC)/QrEncode/mask.c						$(SRC)/QrEncode/mask.h
	$(CC_PLUS) $(SRC)/QrEncode/mask.c	 						-o $@				$(C_CC_FRAMEWORK)
	
$(ODIR)mmask.o					: 	$(SRC)/QrEncode/mmask.c						$(SRC)/QrEncode/mmask.h
	$(CC_PLUS) $(SRC)/QrEncode/mmask.c	 						-o $@				$(C_CC_FRAMEWORK)

$(ODIR)mqrspec.o					: 	$(SRC)/QrEncode/mqrspec.c				$(SRC)/QrEncode/mqrspec.h
	$(CC_PLUS) $(SRC)/QrEncode/mqrspec.c	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)qrinput.o					: 	$(SRC)/QrEncode/qrinput.c				$(SRC)/QrEncode/qrinput.h
	$(CC_PLUS) $(SRC)/QrEncode/qrinput.c	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)qrspec.o						: 	$(SRC)/QrEncode/qrspec.c				$(SRC)/QrEncode/qrspec.h
	$(CC_PLUS) $(SRC)/QrEncode/qrspec.c	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)rscode.o						: 	$(SRC)/QrEncode/rscode.c				$(SRC)/QrEncode/rscode.h
	$(CC_PLUS) $(SRC)/QrEncode/rscode.c	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)split.o						: 	$(SRC)/QrEncode/split.c				$(SRC)/QrEncode/split.h
	$(CC_PLUS) $(SRC)/QrEncode/split.c	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)qrencode.o					: 	$(SRC)/QrEncode/qrencode.c				$(SRC)/QrEncode/qrencode.h
	$(CC_PLUS) $(SRC)/QrEncode/qrencode.c	 						-o $@			$(C_CC_FRAMEWORK)

$(ODIR)jsonreader.o					: 	$(SRC)/Json/json_reader.cpp				$(SRC)/Json/reader.h
	$(CC_PLUS) $(SRC)/Json/json_reader.cpp	 						-o $@			$(C_CC_FRAMEWORK)
	
$(ODIR)jsonwriter.o					: 	$(SRC)/Json/json_writer.cpp				$(SRC)/Json/writer.h
	$(CC_PLUS) $(SRC)/Json/json_writer.cpp	 						-o $@			$(C_CC_FRAMEWORK)
	
$(ODIR)jsonvalue.o					: 	$(SRC)/Json/json_value.cpp				$(SRC)/Json/value.h		$(SRC)/Json/json_valueiterator.inl
	$(CC_PLUS) $(SRC)/Json/json_value.cpp	 						-o $@			$(C_CC_FRAMEWORK)
		
purge: purge_objs purge_lib
ifneq "$(wildcard $(ODIR))" ""
	-@$(RMDIR) $(ODIR)
endif
purge_objs:
	-@$(RM) $(OBJS)
purge_lib:
	-@$(RM) $(ODIR)$(FW_LIB_NAME)

