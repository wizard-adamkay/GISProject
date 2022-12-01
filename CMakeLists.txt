cmake_minimum_required(VERSION 3.23)
project(GISProject)

set(CMAKE_CXX_STANDARD 14)

add_executable(GISProject GIS.cpp GISRecord.h NameIndex.h CoordinateIndex.h BufferPool.h Logger.h SystemManager.h CommandProcessor.h)
