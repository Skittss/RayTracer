include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Using Local Env (.env.cmake): ${LOCAL_ENV}")

set(NAME raytracer)

cmake_minimum_required(VERSION 3.13.0)

message(STATUS "using ${CMAKE_GENERATOR}")
if(CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if(NOT MINGW_PATH)
        message(FATAL_ERROR ".env.cmake does not specify MINGW_PATH")
    endif()
    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER  ${MINGW_PATH}/bin/g++.exe)
endif()

project(${NAME} VERSION 1.0.0)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE LIB_SOURCES ${PROJECT_SOURCE_DIR}/lib/*.cpp)

add_executable(${PROJECT_NAME} ${LIB_SOURCES} ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11) #c++ 11

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

if(WIN32)
    message(STATUS "BUILDING FOR WINDOWS")

    if(USE_MINGW)
        target_include_directories(${PROJECT_NAME} PUBLIC
            ${MINGW_PATH}/include
        )
        target_link_directories(${PROJECT_NAME} PUBLIC
            ${MINGW_PATH}/lib
        )
    endif()

    # includes
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/lib/include
    )
    target_link_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/lib/include
    )

elseif (UNIX)
    message(STATUS "BUILDING FOR LINUX")
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/lib/include
    )
    target_link_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/lib/include
    )
endif()