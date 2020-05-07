cmake_minimum_required(VERSION 3.9)

set(PLUGIN_NAME "srcqe_analysis")

# => ADD Sources here:
set(PLUGIN_SOURCES srcqe_analysis.cc
        SrcqeAnalysisProcessor.cc
        SrcqeAnalysisProcessor.h
        SrcqeRootOutput.h)


#   Most of the time you are not interested in lines below
# ==========================================================

project(${PLUGIN_NAME})

set(PLUGIN_TARGET_NAME "${PLUGIN_NAME}_plugin")
add_library(${PLUGIN_TARGET_NAME} SHARED ${PLUGIN_SOURCES})

# Expose custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(JANA REQUIRED)
find_package(EJANA REQUIRED)
find_package(ROOT)


target_include_directories(${PLUGIN_TARGET_NAME} PUBLIC ${JANA_INCLUDE_DIR} ${EJANA_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
target_link_libraries(${PLUGIN_TARGET_NAME} ${JANA_LIBRARY} ${EJANA_LIBRARY} ${EJANA_FMT_LIBRARY} ${ROOT_LIBRARIES} dl pthread)
set_target_properties(${PLUGIN_TARGET_NAME} PROPERTIES PREFIX "" OUTPUT_NAME ${PLUGIN_NAME} SUFFIX ".so")

install(TARGETS ${PLUGIN_TARGET_NAME} LIBRARY DESTINATION ./)