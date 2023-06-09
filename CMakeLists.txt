cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Wall -fmessage-length=0 -m32 -DSILENT=1")
set (CMAKE_CXX_FLAGS_RELEASE "-s -Ofast")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
#set(CMAKE_INSTALL_PREFIX "/opt/cathook/ipc/bin")

project(cathook_ipc_server VERSION 0.0.1)
add_subdirectory(simple-ipc EXCLUDE_FROM_ALL)
file( GLOB SOURCE_FILES RELATIVE "${CMAKE_SOURCE_DIR}" src/*.cpp )
foreach( sourcefile ${SOURCE_FILES})
    string(REPLACE ".cpp" "" sourcename ${sourcefile})
    string(REPLACE "src/" "" sourcename ${sourcename})
    add_executable(${sourcename} ${sourcefile})
    target_include_directories(${sourcename} SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/simple-ipc/include/SimpleIPC")
    target_link_libraries(${sourcename} pthread rt)
    add_custom_command(TARGET ${sourcename} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${sourcename}> ${CMAKE_SOURCE_DIR}/bin/ )
    list(APPEND INSTALL_TARGETS ${sourcename})
endforeach()

install(TARGETS ${INSTALL_TARGETS} DESTINATION /opt/cathook/ipc/bin)
