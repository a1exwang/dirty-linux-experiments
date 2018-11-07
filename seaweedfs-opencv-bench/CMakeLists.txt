CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(swim)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

ADD_LIBRARY(utils utils/http.cpp)
TARGET_LINK_LIBRARIES(utils curl pthread)

ADD_EXECUTABLE(swim swim2.cpp)
TARGET_INCLUDE_DIRECTORIES(swim PUBLIC concurrentqueue)
TARGET_LINK_LIBRARIES(swim opencv_core opencv_imgcodecs opencv_highgui curl pthread utils)
