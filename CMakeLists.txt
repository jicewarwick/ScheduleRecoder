cmake_minimum_required(VERSION 3.5)

project(Schedule LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Sql REQUIRED)

add_executable(Schedule
  main.cpp
  mainwindow.cpp
  mainwindow.h
  resources.qrc
)

target_link_libraries(Schedule PRIVATE Qt5::Widgets Qt5::Sql)
