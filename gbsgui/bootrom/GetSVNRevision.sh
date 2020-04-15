#!/bin/bash

#rootLocation="/opt/STM/STAPI_SDK43"
#rootLocation="/opt/STM/GBSHD446"
#rootLocation="/opt/STM/GBSHD433"
rootLocation="/opt/STM/GBSHD447"

SET_SWVERSION="SDK433"
GET_BOARD_CONFIGURE="b2128"
SET_BOARD_CONFIGURE="B2128"
SET_SOCNAME="H252"

if [ "$rootLocation" == "/opt/STM/GBSHD446" ]; then
   GET_BOARD_CONFIGURE="b2155"
   SET_BOARD_CONFIGURE="B2155"
   SET_SWVERSION="SDK446"
fi

if [ "$rootLocation" == "/opt/STM/GBSHD447" ]; then
   GET_BOARD_CONFIGURE="b2155"
   SET_BOARD_CONFIGURE="B2155"
   SET_SWVERSION="SDK447"
fi

GET_REVISION=`svn info svn://192.168.0.18/stsdk43/gbsgui |grep '^Revision:' | sed -e 's/^Revision: //'`
echo "----->>> REVISION: $GET_REVISION <<<-----"
if [ "$GET_REVISION" == "" ]; then
   echo "Failed to get revision, Exit !!!"
   exit 1 
fi

echo "Read config for the DVD_PLATFORM: $DVD_PLATFORM" >&2
echo "Read config for the DVD_BACKEND_SOCNAME: $DVD_BACKEND_SOCNAME" >&2

if [[ "$DVD_PLATFORM" == "$GET_BOARD_CONFIGURE" && "$DVD_BACKEND_SOCNAME" == "h273" ]]; then
   SET_DVD_NAME="DVB-C"
   SET_SOCNAME="H273"
elif [[ "$DVD_PLATFORM" == "b2090" && "$DVD_BACKEND_SOCNAME" == "h237" ]]; then
   SET_DVD_NAME="DVB-S2"
   SET_SOCNAME="H237"
elif [[ "$DVD_PLATFORM" == "$GET_BOARD_CONFIGURE" && "$DVD_BACKEND_SOCNAME" == "h253" ]]; then
   SET_DVD_NAME="T252"
   SET_SOCNAME="H252"
else
   echo "Read configure not valid, exit !!!"
   exit 1
fi

rm -rf $rootLocation/gbsgui/mainapp/revision.hpp

DISPLAY_MODEL="GBSHD $SET_DVD_NAME"
DISPLAY_SOFTWARE_VERSION="$SET_BOARD_CONFIGURE-$SET_SOCNAME-$SET_SWVERSION"
DISPLAY_LOADED_VERSION="1.0.2 r$GET_REVISION"
DISPLAY_PROJECTINFO="DVBT2-FTA"
DISPLAY_CURRENTDATE=`date +%d-%m-%Y`
echo "//////////////////////////////////////////////////////////////////////////////////////" >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //" >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "// This source is subject to the GBS Permissive License. Please see the License.txt //" >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "// file for more information.                                                       //" >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "//                                                                                  //" >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "// file: revision.hpp                                                               //" >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "//////////////////////////////////////////////////////////////////////////////////////" >> $rootLocation/gbsgui/mainapp/revision.hpp
echo ""                                                                   >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "#ifndef REVISION_HPP_"                                              >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "#define REVISION_HPP_"                                              >> $rootLocation/gbsgui/mainapp/revision.hpp
echo ""                                                                   >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "#define IRD_INFO_MODEL_NAME          \"$DISPLAY_MODEL\""            >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "#define IRD_INFO_SOFTWARE_VERSION    \"$DISPLAY_SOFTWARE_VERSION\"" >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "#define IRD_INFO_LOADER_VERSION      \"$DISPLAY_LOADED_VERSION\""   >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "#define IRD_INFO_PROJECT_INFORMATION \"$DISPLAY_PROJECTINFO\""      >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "#define IRD_INFO_LAST_UPDATE         \"$DISPLAY_CURRENTDATE\""      >> $rootLocation/gbsgui/mainapp/revision.hpp
echo ""                                                                   >> $rootLocation/gbsgui/mainapp/revision.hpp
echo "#endif // REVISION_HPP_"                                            >> $rootLocation/gbsgui/mainapp/revision.hpp

echo "Save DONE !"
