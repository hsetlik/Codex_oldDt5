cmake_minimum_required(VERSION 3.5)

project(Codex VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
# ensure network stuff gets included
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)

set(CMAKE_AUTORCC ON)

set(PROJECT_SOURCES
    cardstructure.cpp
    cardstructure.h
    cardwidget.cpp
    cardwidget.h
    cardwidget.ui
    codexmainwindow.cpp
    codexmainwindow.h
    codexmainwindow.ui
    datastructures.cpp
    datastructures.h
    deckcreatorwidget.cpp
    deckcreatorwidget.h
    deckcreatorwidget.ui
    decklistwidget.cpp
    decklistwidget.h
    decklistwidget.ui
    deckwidget.cpp
    deckwidget.h
    deckwidget.ui
    inputform.ui
    inputwidget.cpp
    inputwidget.h
    main.cpp
    masterstackedwidget.cpp
    masterstackedwidget.h
    phraseinputform.cpp
    phraseinputform.h
    phraseinputform.ui
    stlutility.cpp
    stlutility.h
    deckmanagerwidget.h
    deckmanagerwidget.cpp
    deckmanagerwidget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Codex
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(Codex SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Codex
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Codex PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


target_link_libraries(Codex  PRIVATE Qt${QT_VERSION_MAJOR}::Network)

set_target_properties(Codex PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Codex)
endif()

# so we don't have any issues with the 'slots' and 'signals' symbols in the linker
add_definitions(-DQT_NO_KEYWORDS)
