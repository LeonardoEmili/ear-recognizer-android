cmake_minimum_required(VERSION 2.8.12)
PROJECT(GalleryGenerator)

FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( include ${OpenCV_INCLUDE_DIRS} )

SET(SOURCES main.cpp localization.cpp localization.hpp utility.cpp utility.hpp descriptors.hpp descriptors.cpp normalization.hpp normalization.cpp)

ADD_EXECUTABLE(GalleryGenerator ${SOURCES})
TARGET_LINK_LIBRARIES (GalleryGenerator ${OpenCV_LIBS})