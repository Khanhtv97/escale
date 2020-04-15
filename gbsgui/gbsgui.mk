####################################################################################################
## The common makefile need to be included for gbsgui
##
## nguyenchithuc@gbs-jsc.com (gthuc.nguyen@gmail.com)
##
####################################################################################################



#########################
## Include Environment ##
#########################

ifeq "$(ARCHITECTURE)" "ST40"
ifeq "$(DERIVED_OBJS)" ""
ifeq "$(DVD_ADDRESSMODE)" "29"
DERIVED_OBJS := _29BITS
else
DERIVED_OBJS := _32BITS
endif
endif
endif

GBSGUI_OBJ_DIR = objs

ifeq "$(DVD_BACKEND_SOCNAME)" ""
#       OBJECT_NAME = $(DVD_PLATFORM)_$(DVD_BACKEND)_$(ARCHITECTURE)$(DVD_CPU)_$(DVD_OS)$(DERIVED_OBJS)
GBSGUI_PLATFORM_DIR = $(DVD_PLATFORM)_$(DVD_BACKEND)_$(ARCHITECTURE)$(DVD_CPU)_$(DVD_OS)$(DERIVED_OBJS)
else
#       OBJECT_NAME = $(DVD_PLATFORM)_$(DVD_BACKEND_SOCNAME)_$(ARCHITECTURE)$(DVD_CPU)_$(DVD_OS)$(DERIVED_OBJS)
GBSGUI_PLATFORM_DIR = $(DVD_PLATFORM)_$(DVD_BACKEND_SOCNAME)_$(ARCHITECTURE)$(DVD_CPU)_$(DVD_OS)$(DERIVED_OBJS)
endif

#GBSGUI_PLATFORM_DIR = $(DVD_PLATFORM)_$(DVD_BACKEND)_$(ARCHITECTURE)_$(DVD_OS)$(DERIVED_OBJS)
GBSGUI_LIB_NAME = $(LIB_PREFIX)GBSGUI$(LIB_SUFFIX)

BDIR = $(GBSGUI_OBJ_DIR)$(SLASH)$(GBSGUI_PLATFORM_DIR)
GBSGUI_LIB = $(BDIR)$(SLASH)$(GBSGUI_LIB_NAME)

# extra 
EX_SRC_DIR = $(GUIROOT)$(SLASH)..$(SLASH)extra
EX_BDIR = $(BDIR)$(SLASH)extra_objs
EX_LIB_NAME = $(LIB_PREFIX)Extra$(LIB_SUFFIX)
EX_LIB =$(EXBDIR)$(SLASH)$(EX_LIB_NAME)

# API Wrapper 
API_SRC_DIR = apiwrap
API_BDIR = $(BDIR)$(SLASH)api_objs
API_LIB_NAME = $(LIB_PREFIX)Api$(LIB_SUFFIX)
API_LIB =$(API_BDIR)$(SLASH)$(API_LIB_NAME)

# Resources
RES_SRC_DIR = resources
RES_BDIR = $(BDIR)$(SLASH)res_objs
RES_LIB_NAME = $(LIB_PREFIX)Res$(LIB_SUFFIX)
RES_LIB =$(RES_BDIR)$(SLASH)$(RES_LIB_NAME)

# Kachiusa Framework
FW_SRC_DIR = framework
FW_BDIR = $(BDIR)$(SLASH)fw_objs
FW_LIB_NAME = $(LIB_PREFIX)Core$(LIB_SUFFIX)
FW_LIB = $(FW_BDIR)$(SLASH)$(FW_LIB_NAME)

# Kachiusa main app
APP_SRC_DIR = mainapp
APP_BDIR = $(BDIR)$(SLASH)app_objs
APP_LIB_NAME = $(LIB_PREFIX)App$(LIB_SUFFIX)
APP_LIB = $(APP_BDIR)$(SLASH)$(APP_LIB_NAME)


include $(DVD_MAKE)/generic.mak

#########################
## Flags configure     ##
#########################

INCDIR  = -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DVD_INCLUDE))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DVD_CONFIG)/chip)
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DVD_CONFIG)/platform/$(DVD_PLATFORM)_$(DVD_BACKEND))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DVD_CONFIG)/platform/Default)
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DVD_INCLUDE)/internal)
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(STSDKROOT)/stapp/platform/$(DVD_PLATFORM)/$(DVD_BACKEND))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(STSDKROOT)/stapp/include)
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(STSDKROOT)/stapp/main)
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(STSDKROOT)/stapp)

ifeq "$(DVD_OS)" "LINUX"
 ifeq "$(KBUILD_OUTPUT)" ""
  INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(KDIR)$(SLASH)include)
 else
  INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(KBUILD_OUTPUT)$(SLASH)include)
 endif
endif

#########
## OS+ ##
#########
ifeq "$(OSPLUS)" "1"
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(OSPLUSROOT)/include)
DVD_CFLAGS += -DOSPLUS
endif
##############
# BUILD APP ##
##############
ifeq "$(PRODUCTION_TEST_VERSION)" "1"
DVD_CFLAGS += -DPRODUCTION_TEST_VERSION
endif
ifeq "$(DEMO_VERSION)" "1"
DVD_CFLAGS += -DDEMO_VERSION
endif
ifeq "$(DEBUG_VERSION)" "1"
DVD_CFLAGS += -DDEBUG_VERSION
endif
ifeq "$(EEPROM_VERSION)" "1"
DVD_CFLAGS += -DEEPROM_VERSION
endif
ifeq "$(STSTREAM)" "1"
DVD_CFLAGS += -DGUSE_STSTREAM
endif
##############
### WPG-FP ###
##############
ifeq "$(WPG_ST253_TM1650)" "1"
DVD_CFLAGS += -DWPG_ST253_TM1650
else
DVD_CFLAGS += -DTM1668
endif
##############
### CH DEG ###
##############
ifeq "$(CHANGHONG)" "1"
DVD_CFLAGS += -DCHANGHONG
endif
##############
## MULTICOM ##
#############
ifeq "$(MULTICOM)" "1"
# INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DVD_ROOT)$(SLASH)multicom$(SLASH)$(MULTICOM_VERSION)$(SLASH)include)
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(MULTICOM_ROOT)$(SLASH)include)
endif

####################
## UNIAPPLI Support  ##
####################
ifeq "$(UNIAPPLI)" "1"
DVD_CFLAGS += -DUNIAPPLI
endif

####################
## PVR Support  ##
####################
ifeq "$(PVR_SUPPORT)" "1"
DVD_CFLAGS+=-DADD_DVR_FEATURE -DUSE_DVR_INDEX_FILE
endif

####################
## ST Media Player  ##
####################
ifeq "$(HND_ENV)" "1"
 DVD_CFLAGS+=-DST_MEDIA_PLAYER
endif

## by default turn on large file support
DVD_CFLAGS += -DUSE_CURL -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE

################
## SD Display ##
################

ifneq "$(DVD_DISPLAY_SD)" ""
 ifneq "$(DVD_DISPLAY_SD)" "NONE"
  DVD_CFLAGS += -DDISPLAY_SD=$(DVD_DISPLAY_SD)
 endif
endif

ifeq "$(SHREE_SW)" "1"
DVD_CFLAGS += -DSHREE_SW
ifeq "$(HINDI)" "1"
DVD_CFLAGS += -DHINDI
endif
endif
ifeq "$(JADE_SW)" "1"
DVD_CFLAGS += -DJADE_SW
endif
ifeq "$(EDITOR_ENABLE)" "1"
DVD_CFLAGS += -DEDITOR_ENABLE
endif
ifeq "$(TELETEXTONOSD)" "1"
DVD_CFLAGS += -DTELETEXTONOSD
endif
ifeq "$(IFRAME_ENABLE)" "1"
DVD_CFLAGS += -DIFRAME_ENABLE
endif
#######################
## USB active or not ##
#######################

ifeq "$(USB)" "1"
 ifeq "$(OSPLUS)" "1"
  DVD_CFLAGS += -DUSB
 endif
endif

ifeq "$(JAVAVM)" "1"
     DVD_CFLAGS += -DST_JVM
endif

#########################
## TCPIP active or not ##
#########################

ifeq "$(TCPIP)" "1"
 ifeq "$(OSPLUS)" "1"
  INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(OSPLUSROOT)/include/nexgen)
  ifeq "$(ARCHITECTURE)" "ST20"
   DVD_CFLAGS += -DTCPIP -DNG_RTOS -DNG_CPU_ST20$(PROC_DEFINE) -DNG_COMP_ST20CC
  endif
  ifeq "$(ARCHITECTURE)" "ST40"
   DVD_CFLAGS += -DTCPIP -DNG_RTOS -DNG_CPU_ST40
  endif
 endif
endif

#####################
## MODULES is ON ? ##
#####################

ifeq "$(MODULES)" "1"
### INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),../../modules/include)
## DVD_CFLAGS += -DMODULES
endif

################
## HD Display ##
################

##ifeq "$(DVD_DISPLAY_HD)" "1"
## DVD_CFLAGS += -DDISPLAY_HD
##endif
ifneq "$(DVD_DISPLAY_HD)" ""
 ifneq "$(DVD_DISPLAY_HD)" "NONE"
  DVD_CFLAGS += -DDISPLAY_HD=$(DVD_DISPLAY_HD)
 endif
endif

#######################
## Switch 29/32 bits ##
#######################

ifneq "$(DVD_ADDRESSMODE)" ""
 DVD_CFLAGS += -DDVD_ADDRESSMODE=$(DVD_ADDRESSMODE)
endif

###################
## 8M FOOT PRINT ##
###################

ifeq "$(DTT5105_FOOTPRINT8MB)" "1"
  DVD_CFLAGS += -DDTT5105_FOOTPRINT8MB
endif

###################
## Subtitle      ##
###################

ifeq "$(STSUBT_OSD_SUPPORT)" "1"
  DVD_CFLAGS += -DSTSUBT_OSD_SUPPORT
endif

###################
## JADE_ENABLE_LOW_POWER_MODE ##
###################

ifeq "$(JADE_ENABLE_LOW_POWER_MODE)" "1"
  DVD_CFLAGS += -DJADE_ENABLE_LOW_POWER_MODE
endif

#######################
## JADE_PSI_MONITORING_ENABLED ##
#######################
ifeq "$(JADE_PSI_MONITORING_ENABLED)" "1"
  DVD_CFLAGS += -DJADE_PSI_MONITORING_ENABLED
endif

#######################
## TUNER SUPPORT ##
#######################
ifneq "$(DVD_FRONTEND_TUNER)" "NONE"
  DVD_CFLAGS += -DDVD_FRONTEND_TUNER='"$(DVD_FRONTEND_TUNER)"'
endif
ifeq "$(USE_TUNER_LITE)" "YES"
DVD_CFLAGS += -DUSE_STTUNER_LITE
endif
ifeq "$(DVD_USE_STFRONTEND)" "YES"
DVD_CFLAGS += -DUSE_STFRONTEND
endif

####################
## Run from flash ##
####################

ifeq "$(RUN_FROM_FLASH)" "1"
 DVD_CFLAGS += -DRUN_FROM_FLASH
endif

################################
##   STDIRECTFB IS ON  ?      ##
################################
ifeq "$(STDIRECTFB)" "1"
DVD_CFLAGS += -DST_DIRECTFB
endif

ifeq "$(DVR)" "1"
  DVD_CFLAGS += -DDVR
endif


ifeq "$(PIP_SUPPORT)" "1"
  DVD_CFLAGS += -DADD_PIP_SUPPORT
endif

###########################
## Common compil options ##
###########################

C_CC_GLOBAL  += -Wno-write-strings -c -DSTFAE -DST_$(DVD_BACKEND) -DST_$(DVD_OS) -D$(DVD_PLATFORM) -D$(DVD_TRANSPORT) -D$(DVD_SERVICE) -DPROCESSOR_$(PROC_DEFINE) -DARCHITECTURE_$(ARCHITECTURE) $(INCDIR) $(OPTIONAL_CFLAGS) $(DVD_CFLAGS)
ifeq "$(DVD_BACKEND)" "5105"
C_CC_GLOBAL += -finl-timeslice -DSTSYS_NO_PRAGMA
endif
ifeq "$(DVD_BACKEND)" "5188"
C_CC_GLOBAL += -finl-timeslice -DSTSYS_NO_PRAGMA
endif
ifeq "$(DVD_BACKEND)" "5162"
C_CC_GLOBAL += -finl-timeslice -DSTSYS_NO_PRAGMA
endif

ifeq "$(DVD_OS)" "LINUX"
 C_CC_GLOBAL += -DST_OSLINUX
endif

####################################################
## Special compil option according to the toolset ##
####################################################

ifeq "$(ARCHITECTURE)" "ST40"
ifeq "$(DVD_OS)" "LINUX"
##C_CC_TOOLSET = -Wall -fno-strict-aliasing
C_CC_TOOLSET =
C_CC_OSNAME  = "LINUX"
else
C_CC_TOOLSET = -mruntime=os21 -Wall -fno-strict-aliasing
C_CC_OSNAME  = "OS21"
endif
endif



##End of file ##

