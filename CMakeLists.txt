PROJECT("wordfilter")

cmake_policy(SET CMP0015 OLD)



ADD_DEFINITIONS(-g -Wall -Wno-unused-function -Wno-unused-variable -Werror -O0 -std=c++0x -std=gnu++0x
-D_google_perftools_ -D_DEBUG)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )
SET(CMAKE_BUILD_TYPE Debug)

AUX_SOURCE_DIRECTORY(. WORD_SRC)

ADD_EXECUTABLE(wordfilter ${WORD_SRC})
TARGET_LINK_LIBRARIES(wordfilter unwind dl)

SET(EXECUTABLE_OUTPUT_PATH ./)




