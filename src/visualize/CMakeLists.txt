cmake_minimum_required(VERSION 3.27)
project(llvm-visualize)
find_package(Matplot++ REQUIRED)


download_file(https://raw.githubusercontent.com/FlorianRappl/CmdParser/c30773174389752bc74b9eb93671aa7b14efad2d/cmdparser.hpp
              ${CMAKE_SOURCE_DIR}/packages/cmdParser/cmdparser.hpp
              MD5
              5277702f459ccb1569fed4a8dfc8b1f3)


include_directories(${CMAKE_SOURCE_DIR}/packages ${CMAKE_SOURCE_DIR}/src)

add_executable(llvm-visualize main.cpp)
target_link_libraries(llvm-visualize Matplot++::matplot)