cmake_minimum_required(VERSION 3.5)

project(QPoromodo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()

find_package(Qt5 COMPONENTS Widgets Sql Multimedia LinguistTools REQUIRED)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR})

add_executable(Schedule
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  resources.qrc
  ${QM_FILES}
)

add_library(Poromodo poromodo.cpp poromodo.h)
target_link_libraries(Poromodo PRIVATE Qt5::Core Qt5::Sql)

target_link_libraries(Schedule PRIVATE Poromodo Qt5::Widgets Qt5::Sql Qt5::Multimedia)

