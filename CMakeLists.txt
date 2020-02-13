#
cmake_minimum_required (VERSION 3.8)
# For sanitizers 01
SET (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

PROJECT ( PBRT-V3 )

# OPTION 
OPTION(PBRT_FLOAT_AS_DOUBLE "Use 64-bit floats" OFF)

IF (PBRT_FLOAT_AS_DOUBLE)
  ADD_DEFINITIONS ( -D PBRT_FLOAT_AS_DOUBLE )
ENDIF()

OPTION(PBRT_SAMPLED_SPECTRUM "Use SampledSpectrum rather than RGBSpectrum" OFF)

IF (PBRT_SAMPLED_SPECTRUM)
  ADD_DEFINITIONS ( -D PBRT_SAMPLED_SPECTRUM )
ENDIF()


# Test
ENABLE_TESTING()

# build type 
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
# openEXR
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/openexr/OpenEXR")
  message(FATAL_ERROR "The OpenEXR submodule directory is missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by running \"git submodule update --init --recursive\"")
endif()


# check glog 
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/glog/cmake")
  message(FATAL_ERROR "The glog submodule directory is missing! "
    "You probably did not clone the project with --recursive, or you first checked out "
    "pbrt before it was added. It is possible to recover by running "
    "\"git submodule update --init --recursive\"")
endif()

# check ptex 
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/ptex/src")
  message(FATAL_ERROR "The ptex submodule directory is missing! "
    "You probably did not clone the project with --recursive, or you first checked out "
    "pbrt before it was added. It is possible to recover by running "
    "\"git submodule update --init --recursive\"")
endif()

# check zlib
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/zlib/doc")
  message(FATAL_ERROR "The zlib submodule directory is missing! "
    "You probably did not clone the project with --recursive, or you first checked out "
    "pbrt before it was added. It is possible to recover by running "
    "\"git submodule update --init --recursive\"")
endif()

# For sanitizers 02 
FIND_PACKAGE ( Sanitizers )
FIND_PACKAGE ( Threads )

# disable debug when release 
IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
  ADD_DEFINITIONS (-DNDEBUG)
ENDIF()

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

###########################################################################
# comper 

IF(MSVC)
  ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
ENDIF()
INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES ( alloca.h HAVE_ALLOCA_H )
IF ( HAVE_ALLOCA_H )
  ADD_DEFINITIONS ( -D PBRT_HAVE_ALLOCA_H )
ENDIF ()

CHECK_INCLUDE_FILES ( memory.h HAVE_MEMORY_H )
IF ( HAVE_MEMORY_H )
  ADD_DEFINITIONS ( -D PBRT_HAVE_MEMORY_H )
ENDIF ()
###########################################################################
# C++ 11 feature 
INCLUDE (CheckCXXSourceCompiles)
INCLUDE (CheckCXXSourceRuns)

CHECK_CXX_SOURCE_COMPILES (
  "int main() { float x = 0x1p-32f; }"
  HAVE_HEX_FP_CONSTANTS )
IF ( HAVE_HEX_FP_CONSTANTS )
  ADD_DEFINITIONS ( -D PBRT_HAVE_HEX_FP_CONSTANTS )
ENDIF ()

CHECK_CXX_SOURCE_COMPILES (
  "int main() { int x = 0b101011; }"
  HAVE_BINARY_CONSTANTS )
IF ( HAVE_BINARY_CONSTANTS )
  ADD_DEFINITIONS ( -D PBRT_HAVE_BINARY_CONSTANTS )
ENDIF ()

CHECK_CXX_SOURCE_COMPILES (
  "int main() { constexpr int x = 0; }"
  HAVE_CONSTEXPR )
IF ( HAVE_CONSTEXPR )
  ADD_DEFINITIONS ( -D PBRT_HAVE_CONSTEXPR )
  ADD_DEFINITIONS ( -D PBRT_CONSTEXPR=constexpr )
ELSE ()
  ADD_DEFINITIONS ( -D PBRT_CONSTEXPR=const )
ENDIF ()

CHECK_CXX_SOURCE_COMPILES (
  "struct alignas(32) Foo { char x; }; int main() { }"
  HAVE_ALIGNAS )
IF ( HAVE_ALIGNAS )
  ADD_DEFINITIONS ( -D PBRT_HAVE_ALIGNAS )
ENDIF ()

CHECK_CXX_SOURCE_COMPILES (
  "int main() { int x = alignof(double); }"
  HAVE_ALIGNOF )
IF ( HAVE_ALIGNOF )
  ADD_DEFINITIONS ( -D PBRT_HAVE_ALIGNOF )
ENDIF ()

CHECK_CXX_SOURCE_RUNS ( "
#include <signal.h>
#include <string.h>
#include <sys/time.h>
void ReportProfileSample(int, siginfo_t *, void *) { }
int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = ReportProfileSample;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPROF, &sa, NULL);
    static struct itimerval timer;
    return setitimer(ITIMER_PROF, &timer, NULL) == 0 ? 0 : 1;
}
" HAVE_ITIMER )
IF ( HAVE_ITIMER )
  ADD_DEFINITIONS ( -D PBRT_HAVE_ITIMER )
ENDIF()

CHECK_CXX_SOURCE_COMPILES ( "
class Bar { public: Bar() { x = 0; } float x; };
struct Foo { union { int x[10]; Bar b; }; Foo() : b() { } };
int main() { Foo f; }
" HAVE_NONPOD_IN_UNIONS )
IF ( HAVE_NONPOD_IN_UNIONS )
  ADD_DEFINITIONS ( -D PBRT_HAVE_NONPOD_IN_UNIONS )
ENDIF ()

CHECK_CXX_SOURCE_COMPILES ( "
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
int main() {
   int fd = open(\"foo\", O_RDONLY);
   struct stat s;
   fstat(fd, &s);
   size_t len = s.st_size;
   void *ptr = mmap(0, len, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
   munmap(ptr, len);   
}
" HAVE_MMAP )
if ( HAVE_MMAP )
  ADD_DEFINITIONS ( -D PBRT_HAVE_MMAP )
ENDIF ()


########################################
# noinline

CHECK_CXX_SOURCE_COMPILES (
"__declspec(noinline) void foo() { }
int main() { }"
HAVE_DECLSPEC_NOINLINE )

CHECK_CXX_SOURCE_COMPILES (
"__attribute__((noinline)) void foo() { }
int main() { }"
HAVE_ATTRIBUTE_NOINLINE )

IF ( HAVE_ATTRIBUTE_NOINLINE )
  ADD_DEFINITIONS ( -D "PBRT_NOINLINE=__attribute__\\(\\(noinline\\)\\)" )
ELSEIF ( HAVE_DECLSPEC_NOINLINE )
  ADD_DEFINITIONS ( -D "PBRT_NOINLINE=__declspec(noinline)" )
ELSE ()
  ADD_DEFINITIONS ( -D PBRT_NOINLINE )
ENDIF ()

########################################
# Aligned memory allocation

CHECK_CXX_SOURCE_COMPILES ( "
#include <malloc.h>
int main() { void * ptr = _aligned_malloc(1024, 32); }
" HAVE__ALIGNED_MALLOC )

CHECK_CXX_SOURCE_COMPILES ( "
#include <stdlib.h>
int main() {
  void *ptr;
  posix_memalign(&ptr, 32, 1024);
} " HAVE_POSIX_MEMALIGN )

CHECK_CXX_SOURCE_COMPILES ( "
#include <malloc.h>
int main() {
    void *ptr = memalign(32, 1024);
} " HAVE_MEMALIGN )

IF ( HAVE__ALIGNED_MALLOC )
  ADD_DEFINITIONS ( -D PBRT_HAVE__ALIGNED_MALLOC )
ELSEIF ( HAVE_POSIX_MEMALIGN )
  ADD_DEFINITIONS ( -D PBRT_HAVE_POSIX_MEMALIGN )
ELSEIF ( HAVE_MEMALIGN )
  ADD_DEFINITIONS ( -D PBRTHAVE_MEMALIGN )
ELSE ()
  MESSAGE ( SEND_ERROR "Unable to find a way to allocate aligned memory" )
ENDIF ()

########################################
# thread-local variables

CHECK_CXX_SOURCE_COMPILES ( "
#ifdef __CYGWIN__
// Hack to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64697
#error \"No thread_local on cygwin\"
#endif  // __CYGWIN__
thread_local int x; int main() { }
" HAVE_THREAD_LOCAL )

CHECK_CXX_SOURCE_COMPILES ( "
__declspec(thread) int x; int main() { }
" HAVE_DECLSPEC_THREAD )

CHECK_CXX_SOURCE_COMPILES ( "
__thread int x; int main() { }
" HAVE___THREAD )

IF ( HAVE_THREAD_LOCAL )
  ADD_DEFINITIONS ( -D PBRT_THREAD_LOCAL=thread_local )
ELSEIF ( HAVE___THREAD )
  ADD_DEFINITIONS ( -D PBRT_THREAD_LOCAL=__thread )
ELSEIF ( HAVE_DECLSPEC_THREAD )
  ADD_DEFINITIONS ( -D "PBRT_THREAD_LOCAL=__declspec(thread)" )
ELSE ()
  MESSAGE ( SEND_ERROR "Unable to find a way to declare a thread-local variable")
ENDIF ()



###########################################################################
# zlib

FIND_PACKAGE ( ZLIB )
IF(NOT ZLIB_FOUND)
  # Build zlib
  SET(ZLIB_BUILD_STATIC_LIBS ON CACHE BOOL " " FORCE)
  SET(ZLIB_BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
  ADD_SUBDIRECTORY(src/ext/zlib)

  SET(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/zlib")
  SET(ZLIB_LIBRARY zlibstatic)
  SET_PROPERTY(TARGET zlibstatic PROPERTY FOLDER "ext")
  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/src/ext/zlib")
ENDIF()
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})

###########################################################################
# OpenEXR

SET(ILMBASE_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
SET(OPENEXR_NAMESPACE_VERSIONING OFF CACHE BOOL " " FORCE)
SET(OPENEXR_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)
SET(ILMBASE_BUILD_SHARED_LIBS    OFF CACHE BOOL " " FORCE)

ADD_SUBDIRECTORY(src/ext/openexr)

SET_PROPERTY(TARGET IexMath eLut toFloat b44ExpLogTable dwaLookups IlmThread Half Iex Imath IlmImf PROPERTY FOLDER "ext")

INCLUDE_DIRECTORIES (
  src/ext/openexr/IlmBase/Imath
  src/ext/openexr/IlmBase/Half
  src/ext/openexr/IlmBase/Iex
  src/ext/openexr/OpenEXR/IlmImf
  ${CMAKE_BINARY_DIR}/src/ext/openexr/IlmBase/config
  ${CMAKE_BINARY_DIR}/src/ext/openexr/OpenEXR/config
)
IF(WIN32)
  SET(OPENEXR_LIBS IlmImf Imath Half ${ZLIB_LIBRARY})
ELSE()
  SET(OPENEXR_LIBS IlmImf Imath Half)
ENDIF()

###########################################################################
# glog

SET(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
IF(WIN32)
  ADD_DEFINITIONS( -D GOOGLE_GLOG_DLL_DECL= )
ENDIF()
ADD_SUBDIRECTORY(src/ext/glog)
SET_PROPERTY(TARGET glog logging_unittest demangle_unittest utilities_unittest stl_logging_unittest PROPERTY FOLDER "ext")
INCLUDE_DIRECTORIES (
  src/ext/glog/src
  ${CMAKE_BINARY_DIR}/src/ext/glog
)

###########################################################################
# ptex

# work around https://github.com/wdas/ptex/issues/28
IF ( CMAKE_BUILD_TYPE )
  STRING ( TOLOWER ${CMAKE_BUILD_TYPE} LOWER_BUILD_TYPE )
  SET ( ENV{FLAVOR} ${LOWER_BUILD_TYPE} )
ENDIF ()
SET(PTEX_BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)

SET(CMAKE_MACOSX_RPATH 1)
IF ( WIN32 )
  ADD_DEFINITIONS ( /D PTEX_STATIC)
ENDIF ()
ADD_SUBDIRECTORY(src/ext/ptex)
SET_PROPERTY(TARGET Ptex_static ptxinfo halftest ftest rtest wtest PROPERTY FOLDER "ext")
INCLUDE_DIRECTORIES ( src/ext/ptex/src/ptex )

###########################################################################
# On to pbrt...
SET ( PBRT_CORE_SOURCE
 src/main/pbrt.cpp
)

SET ( PBRT_CORE_HEADERS
  src/core/pbrt.h
)

FILE ( GLOB PBRT_SOURCE
src/ext/*
)

INCLUDE_DIRECTORIES ( src )
INCLUDE_DIRECTORIES ( src/core )

# Visual Studio source folders
SOURCE_GROUP (core REGULAR_EXPRESSION src/core/.*)
SOURCE_GROUP (ext REGULAR_EXPRESSION src/ext/.*)


###########################################################################
# pbrt libraries and executables
ADD_LIBRARY ( pbrt STATIC
  ${PBRT_YACC_LEX_SOURCE}
  ${PBRT_CORE_SOURCE}
  ${PBRT_CORE_HEADERS}
  ${PBRT_SOURCE}
  )
ADD_SANITIZERS ( pbrt )

# A non-exhaustive but pretty representative set..
SET ( PBRT_CXX11_FEATURES
  cxx_auto_type
  cxx_explicit_conversions
  cxx_lambdas
  cxx_nullptr
  cxx_range_for
  cxx_static_assert
)
TARGET_COMPILE_FEATURES ( pbrt PRIVATE ${PBRT_CXX11_FEATURES} )


# lib 
SET(ALL_PBRT_LIBS
pbrt
${OPENEXR_LIBS}
glog
Ptex_static
${ZLIB_LIBRARY}
)

# Main renderer
add_executable (pbrt_exe src/main/pbrt.cpp )
ADD_SANITIZERS ( pbrt_exe )
SET_TARGET_PROPERTIES ( pbrt_exe PROPERTIES OUTPUT_NAME pbrt )
TARGET_COMPILE_FEATURES ( pbrt_exe PRIVATE ${PBRT_CXX11_FEATURES} )
TARGET_LINK_LIBRARIES ( pbrt_exe ${ALL_PBRT_LIBS} )
