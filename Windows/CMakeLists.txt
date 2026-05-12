cmake_minimum_required(VERSION 3.10)
project(GRUBStormUtils-Windows VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# подключаем папку с заголовочными файлами
include_directories(include)

# автоматическая обработка moc, uic, rcc для qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# сначала пробуем qt6, затем qt5 как запасной вариант
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(Qt6_FOUND)
    message(STATUS "используется qt6")
    set(QT_LIBS Qt6::Core Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    message(STATUS "используется qt5")
    set(QT_LIBS Qt5::Core Qt5::Widgets)
endif()

add_executable(GRUBStormUtils-Windows
    src/main/main_qt.cpp         # точка входа (qt-версия)
    src/core/grub_manager.cpp    # ядро управления grub
    src/ui/ui_qt.cpp             # графический интерфейс qt
    src/system/system_info.cpp    # сбор информации о системе
)

target_link_libraries(GRUBStormUtils-Windows ${QT_LIBS})

# на windows дополнительно линкуем ws2_32 (сетевые функции winapi)
if(WIN32)
    target_link_libraries(GRUBStormUtils-Windows ws2_32)
endif()

install(TARGETS GRUBStormUtils-Windows DESTINATION bin)
