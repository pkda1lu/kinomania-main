cmake_minimum_required(VERSION 3.10)
project(kinomania)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Установка путей к SFML
set(SFML_DIR "${CMAKE_SOURCE_DIR}/libraries/SFML-2.6.2/lib/cmake/SFML")
set(SFML_ROOT "${CMAKE_SOURCE_DIR}/libraries/SFML-2.6.2")

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Include nlohmann/json
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/Database.cpp
    src/UI/MainWindow.cpp
    src/UI/ContentDetails.cpp
    src/UI/AuthWindow.cpp
)

set(HEADERS
    include/Application.hpp
    include/Database.hpp
    include/UI/MainWindow.hpp
    include/UI/ContentDetails.hpp
    include/UI/AuthWindow.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SFML_INCLUDE_DIRS}
    ${SFML_ROOT}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    nlohmann_json::nlohmann_json
)

# Копирование ресурсов
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# Настройка для Windows
if(WIN32)
    # Копирование DLL файлов SFML
    file(GLOB SFML_DLLS "${SFML_ROOT}/bin/*.dll")
    file(COPY ${SFML_DLLS} DESTINATION ${CMAKE_BINARY_DIR})

    # Настройка CPack
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_PACKAGE_NAME "Kinomania")
    set(CPACK_PACKAGE_VENDOR "Your Company")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Приложение для просмотра и управления коллекцией фильмов")
    set(CPACK_PACKAGE_VERSION "1.0.0")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Kinomania")
    
    # Включение CPack
    include(CPack)
endif()

cmake_policy(SET CMP0074 NEW) 