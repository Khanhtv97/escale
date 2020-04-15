##//////////////////////////////////////////////////////////////////////////////////##
## Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   ##
## This source is subject to the GBS Permissive License. Please see the License.txt ##
## file for more information.                                                       ##
##                                                                                  ##
## The makefile for gbsgui                                                          ##
##//////////////////////////////////////////////////////////////////////////////////##

include gbsgui.mk

all: create_obj_dirs $(GBSGUI_LIB)

create_obj_dirs:
ifeq "$(wildcard $(GBSGUI_OBJ_DIR))" ""
	-@$(MKDIR) $(GBSGUI_OBJ_DIR)
endif	
ifeq "$(wildcard $(BDIR))" ""
	-@$(MKDIR) $(BDIR)
endif
ifeq "$(wildcard $(API_BDIR))" ""
	-@$(MKDIR) $(API_BDIR)
endif
ifeq "$(wildcard $(FW_BDIR))" ""
	-@$(MKDIR) $(FW_BDIR)
endif
ifeq "$(wildcard $(RES_BDIR))" ""
	-@$(MKDIR) $(RES_BDIR)
endif
ifeq "$(wildcard $(APP_BDIR))" ""
	-@$(MKDIR) $(APP_BDIR)
endif

print:
	@echo SDIR: "$(PWD)"
	@echo API_SRC:  "$(API_SRC_DIR)"
	@echo API_LIB:	"$(API_LIB)"
	@echo RES_SRC:  "$(RES_SRC_DIR)"
	@echo RES_LIB:	"$(RES_LIB)"
	@echo FW_SRC :  "$(FW_SRC_DIR)"
	@echo FW_LIB :	"$(FW_LIB)"
	@echo APP_SRC:  "$(APP_SRC_DIR)"
	@echo APP_LIB:	"$(APP_LIB)"
	@echo 
	@echo GBSGUI_LIB: "$(GBSGUI_LIB)"

IM_LIBS = $(API_LIB) $(RES_LIB) $(FW_LIB) $(APP_LIB)

define BUILD_LIB_IMPORTS
$(if $(wildcard $@),@$(RM) $@)
$(if $(wildcard ar.mac),@$(RM) ar.mac)
$(if $(filter %.a,$(IM_LIBS)),@echo CREATE $(call NATIVEPATH,$@) > ar.mac
@echo SAVE >> ar.mac
@echo END >> ar.mac
@$(AR) -M <ar.mac
)
$(if $(filter %.o,$(IM_LIBS)),@$(AR) -q $@ $(filter %.o,$(IM_LIBS)))
$(if $(filter %.a,$(IM_LIBS)),
@echo OPEN $@ > ar.mac
$(foreach LIB,$(filter %.a,$(IM_LIBS)),
@echo ADDLIB $(LIB) >> ar.mac
)
@echo SAVE >> ar.mac
@echo END >> ar.mac
@$(AR) -M <ar.mac
@$(RM) ar.mac
)
@$(AR) -s $@
endef

$(GBSGUI_LIB) :  Makefile $(API_LIB) $(RES_LIB) $(FW_LIB) $(APP_LIB)
	@$(ECHO) Building GBSGUI LIBRARY $(GBSGUI_LIB_NAME)...
ifeq "$(ARCHITECTURE)" "ST40"
	$(BUILD_LIB_IMPORTS)
endif
	@$(ECHO) DONE!

$(API_LIB): $(API_SRC_DIR)/Makefile $(API_SRC_DIR)/*.cpp $(API_SRC_DIR)/*.hpp 
		@$(ECHO) Building API Wraper LIBRARY $(API_LIB_NAME)...
		@make all -C $(API_SRC_DIR)

$(RES_LIB): $(RES_SRC_DIR)/Makefile $(RES_SRC_DIR)/*.cpp $(RES_SRC_DIR)/*.hpp
		@$(ECHO) Building Resources	LIBRARY $(RES_LIB_NAME)...
		@make all -C $(RES_SRC_DIR)

$(FW_LIB): $(FW_SRC_DIR)/Makefile $(FW_SRC_DIR)/*.cpp $(FW_SRC_DIR)/*.hpp \
           $(FW_SRC_DIR)/vnconv/*.c $(FW_SRC_DIR)/vnconv/include/*.h \
           $(FW_SRC_DIR)/UIControl/*.cpp $(FW_SRC_DIR)/UIControl/*.hpp \
           $(FW_SRC_DIR)/ImageDecode/*.cpp $(FW_SRC_DIR)/ImageDecode/*.h
               
	@$(ECHO) Building Kachiusa Framework LIBRARY $(FW_LIB_NAME)...
	@make all -C $(FW_SRC_DIR)

$(APP_LIB): $(APP_SRC_DIR)/Makefile $(APP_SRC_DIR)/*.cpp $(APP_SRC_DIR)/*.hpp \
	    $(APP_SRC_DIR)/Business/DataController/*.cpp $(APP_SRC_DIR)/Business/DataController/*.hpp \
	    $(APP_SRC_DIR)/Business/UsbManager/*.cpp $(APP_SRC_DIR)/Business/UsbManager/*.hpp \
	    $(APP_SRC_DIR)/Business/InstallManager/*.cpp $(APP_SRC_DIR)/Business/InstallManager/*.hpp \
	    $(APP_SRC_DIR)/Business/EScaleManager/*.cpp $(APP_SRC_DIR)/Business/EScaleManager/*.hpp
		@$(ECHO) Building Kachiusa Main App LIBRARY $(APP_LIB_NAME)...
		@make all -C $(APP_SRC_DIR)

## Clean Targets ##
	
purge_all: purge_app purge_fw purge_res purge_api purge_stgui 

purge_stgui:
	@$(ECHO) Cleaning GBSGUI LIBRARY...
ifneq "$(wildcard $(GBSGUI_OBJ_DIR))" ""
	$(FULLRM) $(GBSGUI_OBJ_DIR)
endif

purge_app:
		@$(ECHO) Cleaning Kachiusa Main App LIBRARY...
		@make purge -C $(APP_SRC_DIR)

purge_fw:
		@$(ECHO) Cleaning Kachiusa Framework LIBRARY...
		@make purge -C $(FW_SRC_DIR)
				
purge_res:
		@$(ECHO) Cleaning Resources	LIBRARY...
		@make purge -C $(RES_SRC_DIR)
		
purge_api:
		@$(ECHO) Cleaning API Wraper LIBRARY...
		@make purge -C $(API_SRC_DIR)

