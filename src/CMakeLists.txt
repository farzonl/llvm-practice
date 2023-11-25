
download_file(https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
              ${CMAKE_SOURCE_DIR}/packages/nlohmann/json.hpp
              SHA256
              665fa14b8af3837966949e8eb0052d583e2ac105d3438baba9951785512cf921
)

set(LLVM_PRACTICE_PLUGINS
    BasicBlockCounterPass
    CFGEdgeCounterPass
    InstructionCountPass
    )

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}
  Targets     ${LLVM_TARGETS_TO_BUILD}"
)

include_directories(${CMAKE_SOURCE_DIR}/packages ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(BasicBlockCounterPass_SOURCES BasicBlockCounterPass.cpp)
set(CFGEdgeCounterPass_SOURCES CFGEdgeCounterPass.cpp)
set(InstructionCountPass_SOURCES InstructionCountPass.cpp)
llvm_map_components_to_libnames(llvm_libs support core irreader passes)

foreach( plugin ${LLVM_PRACTICE_PLUGINS} )
      # Create a library corresponding to 'plugin'
      add_library(
        ${plugin}
        SHARED
        ${${plugin}_SOURCES}
        )
      
        target_link_libraries(${plugin} ${llvm_libs})
endforeach()

add_subdirectory (visualize)







