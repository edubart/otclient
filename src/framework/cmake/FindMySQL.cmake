# Try to find the mysql library
#  MYSQL_FOUND - system has mysql
#  MYSQL_INCLUDE_DIR - the mysql include directory
#  MYSQL_LIBRARY - the mysql library

FIND_PATH(MYSQL_INCLUDE_DIR NAMES mysql.h PATH_SUFFIXES mysql)
IF(WIN32)
    SET(_MYSQL_STATIC_LIBS libmysql.a libmysql_r.a)
    SET(_MYSQL_SHARED_LIBS libmysql.dll.a libmysql_r.dll.a libmysql libmysql_r)
ELSE()
    SET(_MYSQL_STATIC_LIBS libmysqlclient.a libmysqlclient_r.a)
    SET(_MYSQL_SHARED_LIBS libmysqlclient.dll.a libmysqlclient_r.dll.a mysqlclient mysqlclient_r)
ENDIF()

IF(USE_STATIC_LIBS)
    FIND_LIBRARY(MYSQL_LIBRARY NAMES ${_MYSQL_STATIC_LIBS} ${_MYSQL_SHARED_LIBS})
ELSE()
    FIND_LIBRARY(MYSQL_LIBRARY NAMES ${_MYSQL_SHARED_LIBS} ${_MYSQL_STATIC_LIBS})
ENDIF()
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MySQL DEFAULT_MSG MYSQL_LIBRARY MYSQL_INCLUDE_DIR)
MARK_AS_ADVANCED(MYSQL_LIBRARY MYSQL_INCLUDE_DIR)

