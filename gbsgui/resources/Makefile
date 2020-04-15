##//////////////////////////////////////////////////////////////////////////////////##
## Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   ##
## This source is subject to the GBS Permissive License. Please see the License.txt ##
## file for more information.                                                       ##
##                                                                                  ##
## The makefile for gbsgui mainapp                                                  ##
##//////////////////////////////////////////////////////////////////////////////////##


include ../gbsgui.mk

SRC     = $(subst $(BAD_SLASH),$(GOOD_SLASH),$(CURDIR))$(SLASH)
ODIR 	= ..$(SLASH)$(RES_BDIR)$(SLASH)

INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(API_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(RES_SRC_DIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(FW_SRC_DIR))
#INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../../$(EXTRA_BDIR))
INCDIR += -I$(subst $(BAD_SLASH),$(GOOD_SLASH), ../$(APP_SRC_DIR))

DEP_BASIC = $(SRC)resource.hpp \
				$(SRC)images.hpp \
			   $(SRC)images_extern.hpp \
			   #$(SRC)others.hpp 

OBJS = $(ODIR)resource.o #\
       $(ODIR)others.o               
         
ifeq "$(DVD_OS)" "LINUX"
	C_CC_RESOURCE = $(C_CC_TOOLSET) -g -O0 $(C_CC_GLOBAL)
	#C_CC_others  = $(C_CC_TOOLSET) -g -O0 $(C_CC_GLOBAL)
else
# OS21
	C_CC_RESOURCE = -mruntime=os21 -g -O0 $(C_CC_GLOBAL)
	#C_CC_others  = -mruntime=os21 -g -O0 $(C_CC_GLOBAL)
endif            

all : create_obj_dirs $(RES_LIB)
ifeq "$(ARCHITECTURE)" "ST20"
	@$(AR) -o $(ODIR)$(RES_LIB_NAME) $(OBJS)
endif
ifeq "$(ARCHITECTURE)" "ST40"
	@$(AR) -r $(ODIR)$(RES_LIB_NAME)  $(OBJS)
endif

create_obj_dirs:
ifeq "$(wildcard $(ODIR))" ""
	-@$(MKDIR) $(ODIR)
endif

$(RES_LIB): $(OBJS)

$(ODIR)resource.o : $(SRC)resource.cpp $(SRC)resource.hpp $(DEP_BASIC)
	$(CC_PLUS) $(SRC)resource.cpp -o $@ $(C_CC_RESOURCE)


purge: purge_objs purge_lib
ifneq "$(wildcard $(ODIR))" ""
	-@$(RMDIR) $(ODIR)
endif
purge_objs:
	-@$(RM) $(OBJS)
purge_lib:
	-@$(RM) $(ODIR)$(RES_LIB_NAME)

