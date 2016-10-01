cmake_minimum_required (VERSION 2.8)
project (fm)


# Custom modules
set (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake/Modules" ${CMAKE_MODULE_PATH})


# Platform
include (platform)
initialize_platform ()


# Install
include (platform/install)


# Tests
include (platform/tests)
add_project_tests_main_target ()


# Documentation
include (platform/doxygen)
add_project_documentation_main_target ()


# 3rdparty
find_package (Qt4 4.7.1 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})
add_definitions (${QT_DEFINITIONS} -DQT_NO_CAST_FROM_ASCII)

find_package (EFC REQUIRED)
include_directories (${EFC_INCLUDE})

find_package (Lvfs REQUIRED)
include_directories (${LVFS_INCLUDE})

find_package (Lvfs-Core REQUIRED)
include_directories (${LVFS_CORE_INCLUDE})


# Project header
project_executable_header_default ()

# 3rdparty
list (APPEND ${PROJECT_NAME}_LIBS ${EFC_LIB})
list (APPEND ${PROJECT_NAME}_LIBS ${LVFS_LIB})
list (APPEND ${PROJECT_NAME}_LIBS ${QT_LIBRARIES})
list (APPEND ${PROJECT_NAME}_LIBS ${LVFS_CORE_LIB})

# Sources
add_subdirectory (src)
QT4_WRAP_UI (${PROJECT_NAME}_UIS_HEADERS ${${PROJECT_NAME}_UIS})
QT4_WRAP_CPP (${PROJECT_NAME}_MOC_SOURCES ${${PROJECT_NAME}_HEADERS} OPTIONS "-nw")
list (APPEND ${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_MOC_SOURCES} ${${PROJECT_NAME}_UIS_HEADERS})

# Resources
#add_subdirectory (resources)
#QT4_ADD_RESOURCES (${PROJECT_NAME}_RESOURCES_BIN ${${PROJECT_NAME}_RESOURCES})
#list (APPEND ${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_RESOURCES_BIN})

# Set some OS specific settings
if (WIN32)
    set (GUI_TYPE WIN32)
elseif (APPLE)
    set (GUI_TYPE MACOSX_BUNDLE)
endif ()

# Target - fm
add_executable (fm ${GUI_TYPE} ${${PROJECT_NAME}_SOURCES})
target_link_libraries (fm ${${PROJECT_NAME}_LIBS})

# It is because of Qt (qglobal.h)
set_target_properties (fm PROPERTIES COMPILE_FLAGS "-Wno-long-long")

# Documentation
add_documentation ("QFM" 1.0.0 "Qt File Manager")

# Install rules
install_target (fm)

# Prerequisites
#include (InstallRequiredSystemLibraries)

#set (EXECUTABLE_PATH "${CMAKE_INSTALL_PREFIX}/bin/fm${CMAKE_EXECUTABLE_SUFFIX}")
#set (MY_BUNDLE_UTILITIES_PATH "${fm_SOURCE_DIR}/cmake/MyBundleUtilities.cmake")
#configure_file("${fm_SOURCE_DIR}/cmake/FixBundle.cmake.in" "${fm_BINARY_DIR}/FixBundle.cmake" @ONLY)

#install (SCRIPT "${fm_BINARY_DIR}/FixBundle.cmake")
