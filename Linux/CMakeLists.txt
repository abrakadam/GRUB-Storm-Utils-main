cmake_minimum_required(VERSION 3.10)
project(GRUBStormUtils VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# подключаем папку с заголовочными файлами
include_directories(include)

# ищем gtk3 через pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

add_executable(GRUBStormUtils
    src/main/main_gtk.cpp         # точка входа (gtk3-версия)
    src/core/grub_manager.cpp     # ядро управления grub
    src/core/grub_backup.cpp      # управление резервными копиями
    src/core/grub_theme.cpp       # управление темами
    src/core/grub_validator.cpp   # валидация конфигурации
    src/ui/ui_gtk.cpp             # графический интерфейс gtk3
    src/ui/ui_theme_editor.cpp    # редактор тем
    src/ui/ui_status_bar.cpp      # статус-бар
    src/ui/ui_console.cpp         # консольный виджет
    src/system/system_info.cpp    # сбор информации о системе
    src/system/hardware_info.cpp  # информация о железе
    src/system/network_info.cpp   # информация о сети
    src/system/process_info.cpp   # информация о процессах
)

target_include_directories(GRUBStormUtils PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(GRUBStormUtils ${GTK3_LIBRARIES})

# на unix дополнительно линкуем pthread
if(UNIX)
    target_link_libraries(GRUBStormUtils pthread)
endif()

install(TARGETS GRUBStormUtils DESTINATION bin)
