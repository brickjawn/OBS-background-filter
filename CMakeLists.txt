cmake_minimum_required(VERSION 3.16)

project(obs-background-filter VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use pkg-config for OBS on Linux
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBOBS REQUIRED libobs)

# Find OpenCV
find_package(OpenCV REQUIRED)

# ONNX Runtime path (optional)
set(ONNXRUNTIME_ROOT_DIR "" CACHE PATH "Path to ONNX Runtime installation")

# Check if ONNX Runtime path is provided
if(ONNXRUNTIME_ROOT_DIR STREQUAL "")
    message(WARNING "ONNXRUNTIME_ROOT_DIR not set. Building without ML inference support.")
    set(HAVE_ONNXRUNTIME FALSE)
else()
    message(STATUS "ONNX Runtime path: ${ONNXRUNTIME_ROOT_DIR}")
    set(HAVE_ONNXRUNTIME TRUE)
endif()

# Add source files
add_library(obs-background-filter MODULE
    src/plugin-main.cpp
    src/background-filter.cpp
    src/background-filter.h
    src/model-inference.cpp
    src/model-inference.h
    src/security-utils.cpp
    src/security-utils.h
)

# Remove "lib" prefix on Unix
set_target_properties(obs-background-filter PROPERTIES
    PREFIX ""
    OUTPUT_NAME "obs-background-filter"
)

# Include directories
target_include_directories(obs-background-filter PRIVATE
    ${LIBOBS_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

# Link directories for OBS
target_link_directories(obs-background-filter PRIVATE
    ${LIBOBS_LIBRARY_DIRS}
)

# Link libraries
target_link_libraries(obs-background-filter PRIVATE
    ${LIBOBS_LIBRARIES}
    ${OpenCV_LIBS}
    ssl
    crypto
)

# Add ONNX Runtime if path is provided
if(HAVE_ONNXRUNTIME)
    # Add include directories
    target_include_directories(obs-background-filter PRIVATE
        ${ONNXRUNTIME_ROOT_DIR}/include
        ${ONNXRUNTIME_ROOT_DIR}/include/onnxruntime
        ${ONNXRUNTIME_ROOT_DIR}/include/onnxruntime/core/session
    )
    
    # Find the ONNX Runtime library dynamically
    find_library(ONNXRUNTIME_LIB
        NAMES onnxruntime libonnxruntime
        PATHS ${ONNXRUNTIME_ROOT_DIR}/lib
        NO_DEFAULT_PATH
    )
    
    if(ONNXRUNTIME_LIB)
        message(STATUS "Found ONNX Runtime library: ${ONNXRUNTIME_LIB}")
        target_link_libraries(obs-background-filter PRIVATE ${ONNXRUNTIME_LIB})
        target_compile_definitions(obs-background-filter PRIVATE HAVE_ONNXRUNTIME)
    else()
        message(WARNING "ONNX Runtime library not found in ${ONNXRUNTIME_ROOT_DIR}/lib")
        set(HAVE_ONNXRUNTIME FALSE)
    endif()
endif()

# Default to user installation path if not specified
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.config/obs-studio/plugins" CACHE PATH "Install path" FORCE)
    message(STATUS "Installing to user directory: ${CMAKE_INSTALL_PREFIX}")
endif()

# Installation
if(WIN32)
    install(TARGETS obs-background-filter
        LIBRARY DESTINATION "obs-plugins/64bit"
        RUNTIME DESTINATION "obs-plugins/64bit"
    )
    install(FILES data/locale/en-US.ini
        DESTINATION "data/obs-plugins/obs-background-filter/locale"
        OPTIONAL
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/models
        DESTINATION "data/obs-plugins/obs-background-filter"
        OPTIONAL
    )
elseif(APPLE)
    install(TARGETS obs-background-filter
        LIBRARY DESTINATION "obs-background-filter/bin"
    )
    install(FILES data/locale/en-US.ini
        DESTINATION "obs-background-filter/data/locale"
        OPTIONAL
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/models
        DESTINATION "obs-background-filter/data"
        OPTIONAL
    )
else() # Linux
    install(TARGETS obs-background-filter
        LIBRARY DESTINATION "obs-background-filter/bin"
    )
    install(FILES data/locale/en-US.ini
        DESTINATION "obs-background-filter/data/locale"
        OPTIONAL
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/models
        DESTINATION "obs-background-filter/data"
        OPTIONAL
    )
endif()

# Print build configuration summary
message(STATUS "")
message(STATUS "=== Build Configuration Summary ===")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "OBS Libraries: ${LIBOBS_LIBRARIES}")
message(STATUS "OpenCV Version: ${OpenCV_VERSION}")
message(STATUS "ONNX Runtime: ${HAVE_ONNXRUNTIME}")
if(HAVE_ONNXRUNTIME)
    message(STATUS "ONNX Runtime Library: ${ONNXRUNTIME_LIB}")
endif()
message(STATUS "===================================")
message(STATUS "")

