cmake_minimum_required (VERSION 3.14.5)
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    if (CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12)
        cmake_policy (SET CMP0022 NEW) # INTERFACE_LINK_LIBRARIES defines the link interface
    endif ()
    if (CMAKE_VERSION VERSION_GREATER 3.0.0 OR CMAKE_VERSION VERSION_EQUAL 3.0.0)
        cmake_policy (SET CMP0026 OLD) # Disallow use of the LOCATION target property - therefore we set to OLD as we still need it
        cmake_policy (SET CMP0042 NEW) # MACOSX_RPATH is enabled by default
    endif ()
endif ()

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Set project name
project (ParticleCrashProject)
set(TARGET_NAME ParticleCrash)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (WEB)
	#add_compile_options(-fPIC)
	#add_definitions(-DHAVE_POSIX_SPAWN=0 -DHAVE_PTHREAD_H=0 -DHAVE_LIBPTHREAD=0 -DPTHREAD_IN_LIBC=0 -DHAVE_POSIX_SPAWNP=0)
endif()

include (${SDK_PATH}/share/cmake/Modules/UrhoCommon.cmake)
include (${SDK_PATH}/share/cmake/Urho3D.cmake)


if (DESKTOP)
    # Mobile/web toolchains manage runtime libraries themselves, we should not interfere.
    if (URHO3D_STATIC_RUNTIME)
        ucm_set_runtime(STATIC)
    else ()
        ucm_set_runtime(DYNAMIC)
    endif ()
endif ()

if (WEB)
	set (CMAKE_BUILD_TYPE Release)
else()
	set (CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

file(GLOB_RECURSE MY_CPP_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
file(GLOB_RECURSE MY_H_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h)

set(MY_SOURCE_FILES ${MY_CPP_FILES} ${MY_H_FILES})
add_executable(${TARGET_NAME} WIN32 ${MY_SOURCE_FILES})
target_link_libraries(${TARGET_NAME} PRIVATE Urho3D)

if (WEB OR MOBILE)
	SET(PACKAGE_TOOL ${SDK_PATH}/bin/PackageTool)
	create_pak("${CMAKE_CURRENT_SOURCE_DIR}/bin/Data"               "${CMAKE_BINARY_DIR}/bin/Data.pak")
    create_pak("${CMAKE_CURRENT_SOURCE_DIR}/bin/CoreData"           "${CMAKE_BINARY_DIR}/bin/CoreData.pak")
endif ()

if (WEB)
	# set (EMSCRIPTEN_TOTAL_MEMORY 1024MB)
    web_executable(${TARGET_NAME})
    package_resources_web(
        FILES        "${CMAKE_CURRENT_BINARY_DIR}/bin/Data.pak"
                     "${CMAKE_CURRENT_BINARY_DIR}/bin/CoreData.pak"
        RELATIVE_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin"
        OUTPUT       "Resources.js"
        INSTALL_TO   "${CMAKE_CURRENT_BINARY_DIR}/bin"
    )
    web_link_resources(${TARGET_NAME} Resources.js)
    target_link_libraries(${TARGET_NAME} PRIVATE "--shell-file ${CMAKE_SOURCE_DIR}/shell.html" "-s INITIAL_MEMORY=512MB" "-s TOTAL_MEMORY=512MB" "-s FORCE_FILESYSTEM")
endif ()