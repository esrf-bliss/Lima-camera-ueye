###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
#  Copyright (C) : 2009-2017
#  European Synchrotron Radiation Facility
#  CS40220 38043 Grenoble Cedex 9 
#  FRANCE
# 
#  Contact: lima@esrf.fr
# 
#  This is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
# 
#  This software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--add-needed")

#SOURCEs
set(ueye_srcs src/UeyeCamera.cpp  src/UeyeInterface.cpp
	 src/UeyeDetInfoCtrlObj.cpp src/UeyeSyncCtrlObj.cpp
	 src/UeyeVideoCtrlObj.cpp)

file(STRINGS "VERSION" ueye_vers)
add_library(limaueye SHARED ${ueye_srcs})

#INCLUDEs
target_include_directories(limaueye PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(limaueye PUBLIC "/usr/include")


#LIBRARIES
find_library(UEYE_LIB_API ueye_api)
if("${UEYE_LIB_API}" STREQUAL "UEYE_LIB_API-NOTFOUND")
  message(FATAL_ERROR "ueye : libueye_api not found, you can search it in cmake-gui or try to install it if it isn't here.")
endif()

target_link_libraries(limaueye limacore)
target_link_libraries(limaueye ${UEYE_LIB_API})


set_target_properties(limaueye PROPERTIES VERSION ${ueye_vers} SOVERSION "1")

install(TARGETS limaueye LIBRARY DESTINATION lib)

if(COMPILE_SIP)
    set(NAME "ueye")
    include(CameraRunSIP)
    install(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Ueye")
endif()
