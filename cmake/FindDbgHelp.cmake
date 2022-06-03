# OpenClonk, http://www.openclonk.org
#
# Copyright (c) 2011-2017, The OpenClonk Team and contributors
#
# Distributed under the terms of the ISC license; see accompanying file
# "COPYING" for details.
#
# "Clonk" is a registered trademark of Matthes Bender, used with permission.
# See accompanying file "TRADEMARK" for details.
#
# To redistribute this file separately, substitute the full license texts
# for the above references.

# - Find DbgHelp
# Find the DbgHelp library
# This module defines
#  DBGHELP_INCLUDE_DIR, where to find dbghelp.h, etc.
#  DBGHELP_LIBRARIES, the libraries needed to use DbgHelp.
#  DBGHELP_FOUND, If false, do not try to use DbgHelp.

# MSVC knows how to find the Windows SDK on its own, and replicating that
# in CMake is very tricky, especially with the SDK's layout changing
# occasionally. So instead we'll just leave it to MSVC to do correctly.
if(MSVC AND NOT DEFINED DBGHELP_INCLUDE_DIR AND NOT DEFINED DBGHELP_LIBRARY)
	set(DBGHELP_INCLUDE_DIR "")
	set(DBGHELP_LIBRARY "dbghelp")
	set(DBGHELP_FOUND TRUE)
endif()

if(NOT DBGHELP_FOUND)
	find_path(DBGHELP_INCLUDE_DIR NAMES dbghelp.h PATH_SUFFIXES include)
	set(DBGHELP_NAMES ${DBGHELP_NAMES} dbghelp)
	find_library(DBGHELP_LIBRARY NAMES ${DBGHELP_NAMES})

	include(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(DBGHELP DEFAULT_MSG DBGHELP_LIBRARY DBGHELP_INCLUDE_DIR)
endif()

if(DBGHELP_FOUND)
	set(DBGHELP_LIBRARIES ${DBGHELP_LIBRARY})
endif()

mark_as_advanced(DBGHELP_LIBRARY DBGHELP_INCLUDE_DIR)
