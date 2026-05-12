cmake_minimum_required(VERSION 3.10)
project(GRUBStormUtils VERSION 1.0.0)

# ================================================================
# grub storm utils — раздельная сборка по платформам
#
# linux-версия (gtk3): собирать в папке Linux/
#   cd Linux && mkdir build && cd build && cmake .. && make
#
# windows-версия (qt5/qt6): собирать в папке Windows/
#   cd Windows && mkdir build && cd build && cmake .. && cmake --build .
# ================================================================

# на unix собираем linux-версию
if(UNIX)
    add_subdirectory(Linux)
endif()

# на windows собираем windows-версию
if(WIN32)
    add_subdirectory(Windows)
endif()
