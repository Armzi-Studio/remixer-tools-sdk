cmake_minimum_required(VERSION 3.20)
project(RemixerSDK VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# SDK Interface Library (Header-only target)
# ---------------------------------------------------------------------------
add_library(remixer_sdk INTERFACE)
add_library(remixer::sdk ALIAS remixer_sdk)

target_include_directories(remixer_sdk INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ---------------------------------------------------------------------------
# Optional Examples & Validation
# ---------------------------------------------------------------------------
option(REMIXER_BUILD_EXAMPLES "Build Remixer SDK reference modules and examples" ON)

if(REMIXER_BUILD_EXAMPLES)
    add_library(dummy_gain_module SHARED
        examples/dummy_gain_module/dummy_gain_module.c
    )
    target_link_libraries(dummy_gain_module PRIVATE remixer::sdk)
    set_target_properties(dummy_gain_module PROPERTIES
        PREFIX ""
        OUTPUT_NAME "dummy_gain"
    )
endif()

# ---------------------------------------------------------------------------
# Installation Rules
# ---------------------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS remixer_sdk
    EXPORT RemixerSDKTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/remixer
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
