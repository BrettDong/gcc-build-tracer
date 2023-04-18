cmake_minimum_required(VERSION 3.20)
project(GCCBuildTracer)

if (NOT DEFINED GCC_PLUGINS_DIR)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=plugin
                    OUTPUT_VARIABLE GCC_PLUGINS_DIR
                    RESULT_VARIABLE GCC_PLUGINS_PROBE_RET
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (${GCC_PLUGINS_PROBE_RET} EQUAL 0)
        message(STATUS "Located GCC plugins directory ${GCC_PLUGINS_DIR}")
    else()
        message(FATAL_ERROR "Failed to locate GCC plugins directory")
    endif()
endif ()

if (NOT EXISTS ${GCC_PLUGINS_DIR}/include)
    message(FATAL_ERROR "Failed to locate GCC plugins development headers")
endif()

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp src/*.hpp)

add_library(tracer SHARED)
target_sources(tracer PRIVATE ${SRC})
target_compile_features(tracer PRIVATE cxx_std_20)
target_compile_options(tracer PRIVATE -fno-rtti)
target_compile_options(tracer PRIVATE -Wall)
target_include_directories(tracer PRIVATE ${GCC_PLUGINS_DIR}/include)
set_target_properties(tracer PROPERTIES PREFIX "")

install(TARGETS tracer DESTINATION ${GCC_PLUGINS_DIR})
