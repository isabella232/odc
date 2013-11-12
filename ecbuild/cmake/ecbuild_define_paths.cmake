# (C) Copyright 1996-2012 ECMWF.
# 
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
# In applying this licence, ECMWF does not waive the privileges and immunities 
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# define project paths

file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

# setup library building rpaths (both in build dir and then when installed)

set( CMAKE_SKIP_BUILD_RPATH              FALSE ) # use full RPATHs for the build tree
set( CMAKE_BUILD_WITH_INSTALL_RPATH      FALSE ) # when building, don't use the install RPATH already (but later on when installing)
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH   TRUE  ) # add the automatic parts to RPATH which point to dirs outside build tree

if( ENABLE_RPATHS )
    
    # set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib") # install with this RPATH
    # the RPATH to be used when installing, but only if it's not a system directory
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    if("${isSystemDir}" STREQUAL "-1")
       set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    endif()

ENDIF()

# Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
# in about every subdir

set( CMAKE_INCLUDE_CURRENT_DIR OFF)

# put the include dirs which are in the source or build tree
# before all other include dirs, so the headers in the sources
# are prefered over the already installed ones (since cmake 2.4.1)

set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)