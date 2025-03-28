cmake_minimum_required(VERSION 3.15)

project(PomodoroScehduleRecorder LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()

find_package(
	Qt6
	COMPONENTS Core Widgets Sql Multimedia LinguistTools
	REQUIRED
)

set(TS_FILES "${CMAKE_SOURCE_DIR}/translations/main_zh_CN.ts")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/translations")
qt6_add_translation(QM_FILES "${TS_FILES}")

add_executable(
	PomodoroScehduleRecorder WIN32 ${QM_FILES} main.cpp mainwindow.cpp mainwindow.h mainwindow.ui resources.qrc
)

add_library(HashtagCompleter hashtagcompleter.cpp hashtagcompleter.h)
add_library(Pomodoro pomodoro.cpp pomodoro.h)

target_link_libraries(HashtagCompleter PRIVATE Qt6::Widgets)
target_link_libraries(Pomodoro PRIVATE Qt6::Core Qt6::Sql)
target_link_libraries(PomodoroScehduleRecorder PRIVATE Pomodoro HashtagCompleter Qt6::Widgets Qt6::Sql Qt6::Multimedia)

install(TARGETS PomodoroScehduleRecorder RUNTIME DESTINATION bin COMPONENT binary)
install(
	FILES "${QM_FILES}"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/translations"
	COMPONENT language
)
install(
	FILES LICENSE README.md
	TYPE DESTINATION
	doc
	COMPONENT doc
)

# packing
set(CPACK_PACKAGE_NAME "PomodoroScheduleRecorder")
set(CPACK_PACKAGE_VENDOR "CJSoft")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Record Pomodoro Schedule in sqlite database")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "9")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_COMPONENTS_ALL binary language doc)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/data/icon.png")
include(CPack)
