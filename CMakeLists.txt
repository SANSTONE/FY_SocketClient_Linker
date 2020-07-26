
### CMake settings
cmake_minimum_required(VERSION 3.1)

include(CheckCXXCompilerFlag)

###
### Project settings
###
project(FKKK_LINKER VERSION 1.0)

###
### Project options
###
option(FDKK_LINKER_BUILD_TESTS "Enable testing" ON)
option(FDKK_LINKER_BUILD_DEPENDENT "Enable contrib stuff in library" ON)
option(FDKK_LINKER_INSTALL "Enable generation of install target" ON)

## Build options
# --> General
option(LINKER_BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
# --> Apple
if(APPLE)
    option(LINKER_APPLE_UNIVERSAL_BIN "Apple: Build universal binary" OFF)
endif()

# --> Microsoft Visual C++

if(MSVC)
    option(LINKER_MSVC_SHARED_RT "MSVC: Build with shared runtime libs (/MD)" ON)
    option(LINKER_MSVC_STHREADED_RT "MSVC: Build with single-threaded static runtime libs (/ML until VS .NET 2003)" OFF)
endif()

# function to collect all the sources from sub-directories
# into a single list
function(add_sources)
  # To get a property in a define erea
  get_property(is_defined GLOBAL PROPERTY SRCS_LIST DEFINED) 
  if(NOT is_defined)
    define_property(GLOBAL PROPERTY SRCS_LIST
      BRIEF_DOCS "List of source files"
      FULL_DOCS "List of all source files in the entire project")
  endif()
  # make absolute paths
  set(SRCS)
  foreach(s IN LISTS ARGN)
    if(NOT IS_ABSOLUTE "${s}")
      get_filename_component(s "${s}" ABSOLUTE)
    endif()
    list(APPEND SRCS "${s}")
  endforeach()
  # append to global list
  set_property(GLOBAL APPEND PROPERTY SRCS_LIST "${SRCS}")
endfunction(add_sources)

# set include file path 
set(header_directory "include/")

file(GLOB sources "src/[a-zA-Z]*.c")
file(GLOB_RECURSE public_headers "include/[a-zA-Z]*.h")

# if you need third party libaries
if(FDKK_LINKER_BUILD_DEPENDENT)
	file(GLOB contrib_sources "src/ThirdParty/[a-zA-Z]*.c")
	file(GLOB contrib_public_headers "include/ThirdParty/[a-zA-Z]*.h")
else()
	add_definitions(-FDKK_LINKER_NO_DEPENDENT)
endif()

set(library_sources
  ${sources}
  ${public_headers}
  ${contrib_sources}
  ${contrib_public_headers}
)
add_sources(${library_sources})

if(VERBOSE)
	message(STATUS "sources: ${sources}")
	message(STATUS "public_headers: ${public_headers}")
	message(STATUS "contrib_sources: ${contrib_sources}")
	message(STATUS "contrib_public_headers: ${contrib_public_headers}")
endif()

if (CMAKE_VERSION VERSION_LESS 2.8.12)
    include_directories(${FKKK_LINKER_SOURCE_DIR}/src)
    include_directories(${FKKK_LINKER_SOURCE_DIR}/include)
endif()


###
### General compilation settings
###
set(LINKER_c_flags ${CMAKE_C_FLAGS})
set(LINKER_cxx_flags ${CMAKE_CXX_FLAGS})

if(LINKER_BUILD_SHARED_LIBS)
	set(LABEL_SUFFIX "shared")
else()
	set(LABEL_SUFFIX "static")
endif()

if(APPLE)
	if(LINKER_APPLE_UNIVERSAL_BIN)
		set(CMAKE_OSX_ARCHITECTURES ppc;i386)
	endif()
endif()

if(IPHONE)
	set(CMAKE_OSX_SYSROOT "iphoneos13.2")
	set(CMAKE_OSX_ARCHITECTURES "armv6;armv6e")
endif()

if(WIN32)
	if(LINKER_APPLE_UNIVERSAL_BIN)
		add_definitions(-D${PROJECT_NAME}_DLL)	# use or build Windows DLL
	endif()
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "C:/")
	endif()
endif()

# GCC or Clang or Intel Compiler specialities
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
   (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT "x${CMAKE_CXX_SIMULATE_ID}" STREQUAL "xMSVC") OR
   CMAKE_CXX_COMPILER_ID MATCHES "Intel")

	### General stuff
	if(WIN32)
		set(CMAKE_SHARED_LIBRARY_PREFIX "")	# DLLs do not have a "lib" prefix
		set(CMAKE_IMPORT_LIBRARY_PREFIX "")	# same for DLL import libs
		set(CMAKE_LINK_DEF_FILE_FLAG "")	# CMake workaround (2.8.3)
	endif()

	### Project stuff
	if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Release)
	endif()
	#
	set(GCC_EXTRA_OPTIONS "")
	#
	if(LINKER_APPLE_UNIVERSAL_BIN)
		set(GCC_EXTRA_OPTIONS "${GCC_EXTRA_OPTIONS} -fPIC")
	endif()
	#
	set(FLAG_TESTED "-Wextra -Wshadow -Weffc++ -pedantic -pedantic-errors")
	check_cxx_compiler_flag(${FLAG_TESTED} FLAG_WEXTRA)
	if(FLAG_WEXTRA)
		set(GCC_EXTRA_OPTIONS "${GCC_EXTRA_OPTIONS} ${FLAG_TESTED}")
	endif()
	#
	set(LINKER_cxx_flags "-Wall ${GCC_EXTRA_OPTIONS} -pedantic -Wno-long-long ${LINKER_cxx_flags}")

	### Make specific
	if(${CMAKE_BUILD_TOOL} MATCHES make OR ${CMAKE_BUILD_TOOL} MATCHES gmake)
		add_custom_target(debuggable ${CMAKE_MAKE_PROGRAM} clean
			COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
			COMMENT "Adjusting settings for debug compilation"
			VERBATIM)
		add_custom_target(releasable ${CMAKE_MAKE_PROGRAM} clean
			COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
			COMMENT "Adjusting settings for release compilation"
			VERBATIM)
	endif()
endif()

# Microsoft VisualC++ specialities
if(MSVC)
	set(LIB_RT_SUFFIX "md")	# CMake defaults to /MD for MSVC
	set(LIB_RT_OPTION "/MD")
	#
	if(NOT LINKER_MSVC_SHARED_RT)	# User wants to have static runtime libraries 
		if(LINKER_MSVC_STHREADED_RT)	# User wants to have old single-threaded static runtime libraries
			set(LIB_RT_SUFFIX "ml")
			set(LIB_RT_OPTION "/ML")
			if(NOT ${MSVC_VERSION} LESS 1400)
				message(FATAL_ERROR "Single-threaded static runtime libraries (/ML) only available until VS .NET 2003 (7.1).")
			endif()
		else()
			set(LIB_RT_SUFFIX "mt")
			set(LIB_RT_OPTION "/MT")
		endif()

		# correct linker options
		foreach(flag_var  CMAKE_C_FLAGS  CMAKE_CXX_FLAGS)
			foreach(config_name  ""  DEBUG  RELEASE  MINSIZEREL  RELWITHDEBINFO)
				set(var_name "${flag_var}")
				if(NOT "${config_name}" STREQUAL "")
					set(var_name "${var_name}_${config_name}")
				endif()
				string(REPLACE "/MD" "${LIB_RT_OPTION}" ${var_name} "${${var_name}}")
				set(${var_name} "${${var_name}}" CACHE STRING "" FORCE)
			endforeach()
		endforeach()
	endif()
	#
	set(LABEL_SUFFIX "${LABEL_SUFFIX} ${LIB_RT_SUFFIX}")

	# b) Change prefix for static libraries
	set(CMAKE_STATIC_LIBRARY_PREFIX "lib")	

	# c) Correct suffixes for static libraries
	if(NOT LINKER_BUILD_SHARED_LIBS)
		### General stuff
		set(LIB_TARGET_SUFFIX "${LIB_SUFFIX}${LIB_RT_SUFFIX}")
	endif()

	set(LINKER_cxx_flags "/W3 /wd4127 /wd4355 ${LINKER_cxx_flags}")
endif()

### General install settings
set(INCLUDE_INSTALL_ROOT_DIR include)

set(INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_ROOT_DIR})
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")

set(_INSTALL_DESTINATIONS
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

### Library
if(LINKER_BUILD_SHARED_LIBS)
    add_library(FDKK-linker SHARED ${library_sources})
else()
    add_library(FDKK-linker STATIC ${library_sources})
endif()

if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
    target_include_directories(FDKK-linker
        PUBLIC $<BUILD_INTERFACE:${FKKK_LINKER_SOURCE_DIR}/include>
               $<INSTALL_INTERFACE:${INCLUDE_INSTALL_ROOT_DIR}>
        PRIVATE $<BUILD_INTERFACE:${FKKK_LINKER_SOURCE_DIR}/src>)
endif()

set_target_properties(FDKK-linker PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(FDKK-linker PROPERTIES
  COMPILE_FLAGS "${LINKER_c_flags} ${LINKER_cxx_flags}"
)

set_target_properties(FDKK-linker PROPERTIES
	VERSION "${FDKK_LINKER_VERSION}"
	SOVERSION "${FDKK_LINKER_VERSION_MAJOR}.${FDKK_LINKER_VERSION_MINOR}"
	PROJECT_LABEL "FDKK-linker ${LABEL_SUFFIX}"
)

if(IPHONE)
	set_target_properties(FDKK-linker PROPERTIES
		XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "10.0"
	)
endif()

if(MSVC)
	if(NOT LINKER_BUILD_SHARED_LIBS)
		# correct library names
		set_target_properties(FDKK-linker PROPERTIES
			DEBUG_POSTFIX "${LIB_TARGET_SUFFIX}d"
			RELEASE_POSTFIX "${LIB_TARGET_SUFFIX}"
			MINSIZEREL_POSTFIX "${LIB_TARGET_SUFFIX}"
			RELWITHDEBINFO_POSTFIX "${LIB_TARGET_SUFFIX}"
		)
	endif()
endif()

if (FDKK_LINKER_INSTALL)
	install(TARGETS FDKK-linker EXPORT FDKK-linker-targets ${_INSTALL_DESTINATIONS})
	install(
		DIRECTORY ${header_directory}
		DESTINATION ${INCLUDE_INSTALL_DIR}
		FILES_MATCHING PATTERN "*.h"
	)
endif()


### if you need build TESTS
if(FDKK_LINKER_BUILD_TESTS)
	#enable_testing()
	#add_subdirectory(test)
endif()

### congradulation! if you happly ,you have a linker 
get_property(all_sources GLOBAL PROPERTY SRCS_LIST)
add_custom_target(format
	COMMAND clang-format --style=file -i ${all_sources}
	COMMENT "Running clang-format"
	VERBATIM)
