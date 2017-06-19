############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2017
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
# CMakeLists.txt for Ueye camera

SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--add-needed")

#SOURCEs
SET(ueye_srcs src/UeyeCamera.cpp  src/UeyeInterface.cpp
	 src/UeyeDetInfoCtrlObj.cpp src/UeyeSyncCtrlObj.cpp
	 src/UeyeVideoCtrlObj.cpp)

FILE(STRINGS "VERSION" ueye_vers)
ADD_LIBRARY(limaueye SHARED ${ueye_srcs})

#INCLUDEs
TARGET_INCLUDE_DIRECTORIES(limaueye PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES(limaueye PUBLIC "${COMMON_PATH}/../include")


#LIBRARIES
FIND_LIBRARY(UEYE_LIB_API ueye_api)
IF("${UEYE_LIB_API}" STREQUAL "UEYE_LIB_API-NOTFOUND")
  MESSAGE(FATAL_ERROR "ueye : libueye_api not found, you can search it in cmake-gui or try to install it if it isn't here.")
ENDIF()

TARGET_LINK_LIBRARIES(limaueye limacore)
TARGET_LINK_LIBRARIES(limaueye ${UEYE_LIB_API})


SET_TARGET_PROPERTIES(limaueye PROPERTIES VERSION ${ueye_vers} SOVERSION "1")

INSTALL(TARGETS limaueye LIBRARY DESTINATION lib)

IF(COMPILE_SIP)
    SET(NAME "ueye")
    include(CameraRunSIP)
    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Ueye")
ENDIF()
