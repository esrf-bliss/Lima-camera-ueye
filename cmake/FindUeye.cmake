find_path(UEYE_INCLUDE_DIRS ueye.h)
find_library(UEYE_LIBRARIES ueye_api)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ueye
  DEFAULT_MSG
  UEYE_LIBRARIES
  UEYE_INCLUDE_DIRS)
