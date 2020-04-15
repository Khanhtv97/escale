##//////////////////////////////////////////////////////////////////////////////////##
## Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   ##
## This source is subject to the GBS Permissive License. Please see the License.txt ##
## file for more information.                                                       ##
##                                                                                  ##
## The makefile for gbsgui API Wrapper                                              ##
##//////////////////////////////////////////////////////////////////////////////////##

include ../gbsgui.mk

SRC     = $(subst $(BAD_SLASH),$(GOOD_SLASH),$(CURDIR))$(SLASH)
ODIR 	= ..$(SLASH)$(API_BDIR)$(SLASH)

INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(API_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(RES_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))
#INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../../$(EXTRA_BDIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(APP_SRC_DIR))
DEP_BASIC =	$(SRC)frontpanel.hpp \
			$(SRC)TM1650.hpp \
			$(SRC)TM1668.hpp \
			$(SRC)APIBitmap.hpp \
			$(SRC)APIDrawParams.hpp \
			$(SRC)APIFlashIO.hpp \
			$(SRC)APIText.hpp \
			$(SRC)APISetting.hpp \
			$(SRC)APIPower.hpp \
			$(SRC)I2C.hpp \
			$(SRC)VFileSystem.hpp \
			$(SRC)ROMBlower.hpp \
			#$(SRC)others.hpp 

OBJS=	 	$(ODIR)frontpanel.o \
			$(ODIR)TM1650.o \
			$(ODIR)TM1668.o \
			$(ODIR)APIBitmap.o \
			$(ODIR)APIText.o \
			$(ODIR)APIFlashIO.o \
			$(ODIR)APISetting.o \
			$(ODIR)APIPower.o \
			$(ODIR)I2C.o \
			$(ODIR)VFileSystem.o \
			$(ODIR)ROMBlower.o \
			#$(ODIR)others.o               

         
ifeq "$(DVD_OS)" "LINUX"
	C_CC_APIWRAP  = $(C_CC_TOOLSET) -g -O0 $(C_CC_GLOBAL)
	#C_CC_others  = $(C_CC_TOOLSET) -g -O0 $(C_CC_GLOBAL)
else
# OS21
	C_CC_APIWRAP  = -mruntime=os21 -g -O0 $(C_CC_GLOBAL)
	#C_CC_others  = -mruntime=os21 -g -O0 $(C_CC_GLOBAL)
endif            


# Clean the lib
#--------------
clean: subdir_clean
	-$(RM) $($(ARCHITECTURE)_TARGETS)
subdir_clean:
	$(MAKE) -C $(DVD_BUILD_DIR)/teletext clean



all : create_obj_dirs $(API_LIB)
ifeq "$(ARCHITECTURE)" "ST20"
	@$(AR) -o $(ODIR)$(API_LIB_NAME) $(OBJS)
endif
ifeq "$(ARCHITECTURE)" "ST40"
	@$(AR) -r $(ODIR)$(API_LIB_NAME)  $(OBJS)
endif

create_obj_dirs:
ifeq "$(wildcard $(ODIR))" ""
	-@$(MKDIR) $(ODIR)
endif

$(API_LIB): $(OBJS)

$(ODIR)frontpanel.o	:	$(SRC)frontpanel.cpp	$(SRC)frontpanel.hpp	$(DEP_BASIC)
	$(CC_PLUS)	$(SRC)frontpanel.cpp	-o	$@		$(C_CC_APIWRAP)

$(ODIR)TM1650.o	:	$(SRC)TM1650.cpp	$(SRC)TM1650.hpp	$(DEP_BASIC)
	$(CC_PLUS)	$(SRC)TM1650.cpp	-o	$@		$(C_CC_APIWRAP)
	
$(ODIR)TM1668.o	:	$(SRC)TM1668.cpp	$(SRC)TM1668.hpp	$(DEP_BASIC)
	$(CC_PLUS)	$(SRC)TM1668.cpp	-o	$@		$(C_CC_APIWRAP)
	
$(ODIR)APIBitmap.o	:	$(SRC)APIBitmap.cpp	$(SRC)APIBitmap.hpp	$(DEP_BASIC)
	$(CC_PLUS)	$(SRC)APIBitmap.cpp	-o $@		$(C_CC_APIWRAP)

$(ODIR)APIText.o		: 	$(SRC)APIText.cpp 	$(SRC)APIText.hpp 	$(DEP_BASIC)
	$(CC_PLUS) $(SRC)APIText.cpp 		-o $@ 	$(C_CC_APIWRAP)
	
$(ODIR)APIFlashIO.o	: 	$(SRC)APIFlashIO.cpp	$(SRC)APIFlashIO.hpp	$(DEP_BASIC)
	$(CC_PLUS) $(SRC)APIFlashIO.cpp 	-o $@ 	$(C_CC_APIWRAP)
	
$(ODIR)I2C.o			: 	$(SRC)I2C.cpp			$(SRC)I2C.hpp			$(DEP_BASIC)
	$(CC_PLUS) $(SRC)I2C.cpp	 		-o $@ 	$(C_CC_APIWRAP)

$(ODIR)VFileSystem.o	: 	$(SRC)VFileSystem.cpp	$(SRC)VFileSystem.hpp	$(DEP_BASIC)
	$(CC_PLUS) $(SRC)VFileSystem.cpp	-o $@ 	$(C_CC_APIWRAP)

$(ODIR)ROMBlower.o	: 	$(SRC)ROMBlower.cpp	$(SRC)ROMBlower.hpp	$(DEP_BASIC)
	$(CC_PLUS) $(SRC)ROMBlower.cpp	-o $@ 	$(C_CC_APIWRAP)
	
$(ODIR)APISetting.o	: 	$(SRC)APISetting.cpp	$(SRC)APISetting.hpp	$(DEP_BASIC)
	$(CC_PLUS) $(SRC)APISetting.cpp 		-o $@ 		$(C_CC_APIWRAP)

$(ODIR)APIPower.o	: 	$(SRC)APIPower.cpp	$(SRC)APIPower.hpp	$(DEP_BASIC)
	$(CC_PLUS) $(SRC)APIPower.cpp 		-o $@ 		$(C_CC_APIWRAP)

purge: purge_objs purge_lib
ifneq "$(wildcard $(ODIR))" ""
	-@$(RMDIR) $(ODIR)
endif
purge_objs:
	-@$(RM) $(OBJS)
purge_lib:
	-@$(RM) $(ODIR)$(API_LIB_NAME)

