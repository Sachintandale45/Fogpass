cmake_minimum_required(VERSION 3.16)

project(trial1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find Qt ---
find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

# --- Application Executable ---
qt_add_executable(apptrial1
    main.cpp
)

# --- QML Module ---
qt_add_qml_module(apptrial1
    URI trial1
    VERSION 1.0

    QML_FILES
        Main.qml
        content/button.qml
        content/DemoPage.qml
        content/WelcomePage.qml
        content/PlaceholderPage.qml
        content/SplashPage.qml
        content/MenuPage.qml
        content/IntroPage.qml
        content/UserMenu.qml
        content/TestMenuPage.qml
        content/GPSSimulationPage.qml
        content/HeaderBar.qml
        content/ModePlaceholder.qml
        content/VolumeControl.qml

    SOURCES
        src/backend/backendmanager.cpp
        src/backend/backendmanager.h
)

# --- Properties (for macOS/iOS, safe to keep) ---
set_target_properties(apptrial1 PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# --- Link Qt ---
target_link_libraries(apptrial1
    PRIVATE Qt6::Quick
)

# --- Install Rules (for Yocto deployment) ---
include(GNUInstallDirs)
install(TARGETS apptrial1
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    BUNDLE DESTINATION .
)

