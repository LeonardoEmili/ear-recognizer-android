cmake_minimum_required(VERSION 2.8)
PROJECT(GalleryGenerator)

FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

ADD_EXECUTABLE(GalleryGenerator main.cpp)
TARGET_LINK_LIBRARIES (GalleryGenerator ${OpenCV_LIBS})