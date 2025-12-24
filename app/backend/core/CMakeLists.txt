cmake_minimum_required(VERSION 3.16)

project(fogpass-core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core DBus)
find_package(Threads REQUIRED)

# Enable automatic meta-object compilation (moc) for Q_OBJECT macros
set(CMAKE_AUTOMOC ON)

add_executable(fogpass-core
    src/main.cpp
    src/CoreState.cpp
    src/GnssReader.cpp
    src/locator.cpp
    src/alertmanager.cpp
    src/LandmarkEngine.cpp
    ipc/CoreDBusAdaptor.cpp
    # Add headers with Q_OBJECT so AUTOMOC generates the necessary meta-object code
    include/CoreState.h
    include/GnssReader.h
    include/locator.h
    include/alertmanager.h
    include/LandmarkEngine.h
    include/CoreDBusAdaptor.h
)

target_include_directories(fogpass-core
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ipc
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(fogpass-core
    PRIVATE
        Qt6::Core
        Qt6::DBus
        Threads::Threads
)

target_compile_options(fogpass-core
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

install(TARGETS fogpass-core
    RUNTIME DESTINATION bin
)

# Install systemd service file
install(FILES fogpass-core.service
    DESTINATION /lib/systemd/system
)
