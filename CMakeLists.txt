cmake_minimum_required(VERSION 3.29)

project(amadeus VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Multimedia LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia LinguistTools)
# find_package(fmt REQUIRED)
find_package(Boost REQUIRED)
find_package(date REQUIRED)
find_package(range-v3 REQUIRED)

set(TS_FILES amadeus_pl_PL.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        ${TS_FILES}
)


qt_add_executable(amadeus
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        tool.h tool.cpp
        workspace.h workspace.cpp
        control_bar.h control_bar.cpp
        catalog.h catalog.cpp
        playlist_widget.h playlist_widget.cpp
        catalog_tree.h catalog_tree.cpp
        catalog_table.h catalog_table.cpp
        shared/event_controller.hh
        shared/event.hh
        model/selection.h
        playlist_tree.h playlist_tree.cpp
        playlist_table.cpp
        playlist_table.h
        sqlite/field.cc sqlite/field.h

        sqlite/gzip.h
        sqlite/logger.h
        sqlite/query.cc sqlite/query.h
        sqlite/result.cc sqlite/result.h
        sqlite/row.cc sqlite/row.h
        sqlite/shared.h
        sqlite/sqlite.cpp sqlite/sqlite.h
        sqlite/stmt.cpp sqlite/stmt.h
        sqlite/types.h
        sqlite/value.cc sqlite/value.h
        model/playlist.h model/playlist.cpp
        model/song.h model/song.cpp
        line_text_edit.h line_text_edit.cpp
        progress.h progress.cpp
        model/selection.cpp

)


target_link_libraries(amadeus PRIVATE
    z
    sqlite3
    libboost_iostreams.a
    # fmt::fmt
    date::date date::date-tz
    range-v3::meta range-v3::concepts range-v3::range-v3
    Qt6::Widgets
    Qt6::Multimedia
)

set_target_properties(amadeus PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS amadeus
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(amadeus)
endif()
