cmake_minimum_required(VERSION 3.23)
project(TradeNote VERSION 1.1.0 LANGUAGES CXX)

# Configure version file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Network)
include(GNUInstallDirs)

    add_executable(TradeNote
    src/main.cpp
    src/models/deal.cpp
    src/models/strategy.cpp
    src/models/journal_entry.cpp
    src/models/data_manager.cpp
    src/resources/themes.cpp
    src/resources/icons.cpp
    src/windows/main_window.cpp
    src/windows/trade_dialog.cpp
    src/windows/deals_window.cpp
    src/windows/statistics_tab.cpp
    src/windows/journal_tab.cpp
    src/windows/todo_tab.cpp
    src/windows/terminal_window.cpp
    src/windows/strategy_dlg.cpp
    src/windows/settings_dlg.cpp
)

target_link_libraries(TradeNote PRIVATE 
    Qt6::Widgets 
    Qt6::Gui 
    Qt6::Core
    Qt6::Network
)

target_include_directories(TradeNote PRIVATE 
    src
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

# Install executable
install(TARGETS TradeNote
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

# Install data files
if(WIN32)
    install(DIRECTORY app/data
        DESTINATION data
    )
else()
    # Install desktop file
    install(FILES tradenote.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )
    
    # Install icon
    install(FILES tradenote.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
    )
endif()

# CPack configuration
set(CPACK_PACKAGE_NAME "tradenote")
set(CPACK_PACKAGE_VENDOR "Anton Rubtsov")
set(CPACK_PACKAGE_CONTACT "Anton Rubtsov <arvino.work@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TradeNote - Trading Diary and Analysis Tool")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "TradeNote")
    set(CPACK_NSIS_PACKAGE_NAME "TradeNote")
    set(CPACK_NSIS_HELP_LINK "https://github.com/antonrubtsov/TradeNote")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/antonrubtsov/TradeNote")
    set(CPACK_NSIS_MODIFY_PATH ON)
else()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Anton Rubtsov <arvino.work@gmail.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6, libqt6gui6, libqt6core6, libqt6network6")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/antonrubtsov/TradeNote")
endif()

include(CPack)
