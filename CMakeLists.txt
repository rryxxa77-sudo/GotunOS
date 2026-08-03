cmake_minimum_required(VERSION 3.21)

project(GotunOSInstaller
    VERSION 2.0.0
    DESCRIPTION "Qt6/KDE installer for GötünOS"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(GOTUNOS_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/backend/Config.h
    src/backend/I18n.h
    src/backend/InstallWorker.cpp
    src/backend/InstallWorker.h
    src/widgets/SearchableComboBox.cpp
    src/widgets/SearchableComboBox.h
    src/pages/BasePage.cpp
    src/pages/BasePage.h
    src/pages/WelcomePage.cpp
    src/pages/WelcomePage.h
    src/pages/NetworkPage.cpp
    src/pages/NetworkPage.h
    src/pages/LocalePage.cpp
    src/pages/LocalePage.h
    src/pages/UsersPage.cpp
    src/pages/UsersPage.h
    src/pages/HardwarePage.cpp
    src/pages/HardwarePage.h
    src/pages/DiskPage.cpp
    src/pages/DiskPage.h
    src/pages/SecondaryDrivesPage.cpp
    src/pages/SecondaryDrivesPage.h
    src/pages/OptionsPage.cpp
    src/pages/OptionsPage.h
    src/pages/SummaryPage.cpp
    src/pages/SummaryPage.h
    src/pages/ProgressPage.cpp
    src/pages/ProgressPage.h
    src/pages/FinishPage.cpp
    src/pages/FinishPage.h
    assets/resources.qrc
)

add_executable(gotunos-installer-gui ${GOTUNOS_SOURCES})

target_include_directories(gotunos-installer-gui PRIVATE src)
target_link_libraries(gotunos-installer-gui PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

include(GNUInstallDirs)
install(TARGETS gotunos-installer-gui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES packaging/gotunos-installer.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
