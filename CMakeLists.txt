# CMakeLists.txt for Ueye camera

SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--add-needed")

# CXXFLAGS -D__LINUX__ a voir
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

TARGET_LINK_LIBRARIES(limaueye limacore)
TARGET_LINK_LIBRARIES(limaueye ${UEYE_LIB_API})


SET_TARGET_PROPERTIES(limaueye PROPERTIES VERSION ${ueye_vers} SOVERSION "1")

INSTALL(TARGETS limaueye LIBRARY DESTINATION lib)

IF(COMPILE_SIP)
    SET(NAME "ueye")
    SET(INCLUDES)
    FILE(GLOB sipfiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/sip" "${CMAKE_CURRENT_SOURCE_DIR}/sip/*.sip")
    FOREACH(sipfile ${sipfiles})
        SET(INCLUDES 
        "${INCLUDES} 
        %Include ${sipfile}")
    ENDFOREACH()

    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sip/limamodules.sip.in sip/limaueye.sip)
    SET(SIP_CONCAT_PARTS 1)
    SET(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip"
        "${CMAKE_BINARY_DIR}/sip/core"
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/tasks/sip"
        "${CMAKE_SOURCE_DIR}/common/sip"
        "${CMAKE_SOURCE_DIR}/hardware/sip"
        "${CMAKE_SOURCE_DIR}/control/sip"
        "${CMAKE_SOURCE_DIR}/control/software_operation/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/sip")
    set(SIP_DISABLE_FEATURES WITH_CONFIG)
    set(SIP_CHECK_EXC ON)
    ADD_SIP_PYTHON_MODULE(limaueye ${CMAKE_CURRENT_BINARY_DIR}/sip/limaueye.sip)
    TARGET_INCLUDE_DIRECTORIES(python_module_limaueye PRIVATE
                               ${PYTHON_INCLUDE_DIRS}
                               "${CMAKE_SOURCE_DIR}/sip"
                               "${CMAKE_SOURCE_DIR}/sip/core")
    TARGET_LINK_LIBRARIES(python_module_limaueye limaueye)

    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Ueye")
ENDIF()
