cmake_minimum_required(VERSION 3.5)

project(CPP3_BrickGame_v3.0-1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(JSON-C REQUIRED json-c)

include_directories(${CURL_INCLUDE_DIRECTIORIES} ${JSON-C_INCLUDE_DIRECTORIES})

set(PROJECT_SOURCES
        ../s21_desktop.cpp
        ../s21_desktop.h
        ../s21_menu.cpp
        ../s21_menu.h
        ../resources/resources.qrc

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(s21_desktop
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ../s21_main_desktop.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Pacman APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(s21_desktop SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(s21_desktop
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(s21_desktop PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(s21_desktop PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(s21_desktop PRIVATE ${CMAKE_SOURCE_DIR}/../s21_client_library.a ${CURL_LIBRARIES} json-c)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.snake)
endif()
set_target_properties(s21_desktop PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS s21_desktop
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(s21_desktop)
endif()
