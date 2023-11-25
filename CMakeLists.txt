cmake_minimum_required(VERSION 3.27)
project(llvm-practice)

function(download_file url filename hash_type hash)

if(NOT EXISTS ${filename})
  file(DOWNLOAD ${url} ${filename}
       TIMEOUT 60  # seconds
       EXPECTED_HASH ${hash_type}=${hash}
       TLS_VERIFY ON
      )
endif()

endfunction(download_file)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory (src)







