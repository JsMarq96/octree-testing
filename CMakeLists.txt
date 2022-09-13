cmake_minimum_required(VERSION 3.13)

project(OCTREE_VOL)

file(GLOB CPP_SOURCES "src/*.cpp")
file(GLOB CPP_SUBFOLDER_SOURCES "src/**/*.cpp")
file(GLOB C_SOURCES "src/*.c")
file(GLOB C_SUBFOLDER_SOURCES "src/**/*.c")


include_directories(OCTREE_VOL ${EMS_INCLUDES} ${EMS_INCLUDES_SUB})

add_executable(OCTREE_VOL ${CPP_SOURCES} ${C_SOURCES})


set_target_properties(OCTREE_VOL PROPERTIES OUTPUT_NAME "test")

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()


 if( MSVC )
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT phys_sandbox)
endif()

set(CMAKE_BUILD_TYPE Debug)

include_directories(
    "libmorton/include/"
  )

add_custom_command(TARGET OCTREE_VOL PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:OCTREE_VOL>/resources)
