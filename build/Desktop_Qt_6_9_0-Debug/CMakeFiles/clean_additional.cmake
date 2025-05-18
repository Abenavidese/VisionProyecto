# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/VisorNiftiQt_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/VisorNiftiQt_autogen.dir/ParseCache.txt"
  "VisorNiftiQt_autogen"
  )
endif()
