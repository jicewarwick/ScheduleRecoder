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

set(TS_FILES ${CMAKE_SOURCE_DIR}/translations/main_zh_CN.ts)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/translations)
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "translations")
qt5_add_translation(QM_FILES ${TS_FILES})

add_executable(Schedule WIN32
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

install(TARGETS Schedule RUNTIME DESTINATION bin COMPONENT binary)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/translations COMPONENT language)
install(FILES LICENCE README.md TYPE DESTINATION doc COMPONENT doc)

# packing
set(CPACK_PACKAGE_NAME "PoromodoScheduleRecorder")
set(CPACK_PACKAGE_VENDOR "CJSoft")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Record Poromodo Schedule in sqlite database")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "5")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_COMPONENTS_ALL binary language doc)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/data/icon.png")
#set(CPACK_NSIS_MENU_LINKS "programs\\hello" "Hello"
#                          "data\\example.pdf" "documentation")
include(CPack)
