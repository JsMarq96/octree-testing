cmake_minimum_required(VERSION 3.13)

project(OCTREE_VOL)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB CPP_SOURCES "src/*.cpp")
file(GLOB CPP_SUBFOLDER_SOURCES "src/**/*.cpp")
file(GLOB C_SOURCES "src/*.c")
file(GLOB C_SUBFOLDER_SOURCES "src/**/*.c")

add_executable(OCTREE_VOL ${CPP_SOURCES} ${C_SOURCES})

add_subdirectory(glm)
target_link_libraries(OCTREE_VOL glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_link_libraries(OCTREE_VOL glfw)

set(gl3w_dir "includes/GL")
add_library("gl3w" "${gl3w_dir}/src/gl3w.c")
target_include_directories("gl3w" PRIVATE "${glw_dir}/")
target_include_directories(OCTREE_VOL PRIVATE "${gl3w_dir}/")
target_link_libraries(OCTREE_VOL "gl3w" "${CMAKE_DL_LIBS}")

set_target_properties(OCTREE_VOL PROPERTIES OUTPUT_NAME "test")

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

 if( MSVC )
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT OCTREE_VOL)
endif()


include_directories(
  "libmorton/include/"
    "includes/"
  )

add_custom_command(TARGET OCTREE_VOL PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:OCTREE_VOL>/resources)
