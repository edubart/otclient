# Try to find the zlib librairy
#  ZLIB_FOUND - system has zlib
#  ZLIB_INCLUDE_DIR - the zlib include directory
#  ZLIB_LIBRARY - the zlib library

FIND_PATH(ZLIB_INCLUDE_DIR NAMES zlib.h)
FIND_LIBRARY(ZLIB_LIBRARY NAMES libz.a libzlib.a zlib1.a z zlib zdll zlib1)
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZLIB DEFAULT_MSG ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
MARK_AS_ADVANCED(ZLIB_LIBRARY ZLIB_INCLUDE_DIR)

