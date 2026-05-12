#!/bin/bash

# GRUB Storm Utils - Универсальный установочный скрипт
# Поддерживает большинство дистрибутивов Linux

set -e

# цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# функции вывода
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# определение дистрибутива и пакетного менеджера
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        DISTRO_NAME=$PRETTY_NAME
    elif [ -f /etc/redhat-release ]; then
        DISTRO="rhel"
        DISTRO_NAME=$(cat /etc/redhat-release)
    elif [ -f /etc/debian_version ]; then
        DISTRO="debian"
        DISTRO_NAME="Debian $(cat /etc/debian_version)"
    else
        DISTRO="unknown"
        DISTRO_NAME="Unknown"
    fi
    
    print_info "Обнаружен дистрибутив: $DISTRO_NAME"
}

# определение пакетного менеджера
detect_package_manager() {
    if command -v apt-get >/dev/null 2>&1; then
        PKG_MANAGER="apt-get"
        PKG_INSTALL="apt-get install -y"
        PKG_UPDATE="apt-get update"
    elif command -v yum >/dev/null 2>&1; then
        PKG_MANAGER="yum"
        PKG_INSTALL="yum install -y"
        PKG_UPDATE="yum update -y"
    elif command -v dnf >/dev/null 2>&1; then
        PKG_MANAGER="dnf"
        PKG_INSTALL="dnf install -y"
        PKG_UPDATE="dnf update -y"
    elif command -v pacman >/dev/null 2>&1; then
        PKG_MANAGER="pacman"
        PKG_INSTALL="pacman -S --noconfirm"
        PKG_UPDATE="pacman -Sy --noconfirm"
    elif command -v zypper >/dev/null 2>&1; then
        PKG_MANAGER="zypper"
        PKG_INSTALL="zypper install -y"
        PKG_UPDATE="zypper update -y"
    elif command -v emerge >/dev/null 2>&1; then
        PKG_MANAGER="emerge"
        PKG_INSTALL="emerge"
        PKG_UPDATE="emerge --update"
    else
        PKG_MANAGER="unknown"
        print_warning "Неизвестный пакетный менеджер"
    fi
    
    print_info "Пакетный менеджер: $PKG_MANAGER"
}

# установка зависимостей для разных дистрибутивов
install_dependencies() {
    print_info "Установка зависимостей..."
    
    case $DISTRO in
        ubuntu|debian|linuxmint|pop)
            sudo $PKG_UPDATE
            sudo $PKG_INSTALL build-essential cmake pkg-config
            ;;
        fedora|centos|rhel|rocky|almalinux)
            sudo $PKG_UPDATE
            sudo $PKG_INSTALL gcc-c++ cmake pkg-config
            ;;
        arch|manjaro)
            sudo $PKG_UPDATE
            sudo $PKG_INSTALL base-devel cmake pkgconf
            ;;
        opensuse*)
            sudo $PKG_UPDATE
            sudo $PKG_INSTALL gcc-c++ cmake pkg-config
            ;;
        gentoo)
            sudo $PKG_UPDATE
            sudo $PKG_INSTALL cmake
            ;;
        *)
            print_warning "Неизвестный дистрибутив, попытка установки базовых зависимостей..."
            if command -v apt-get >/dev/null 2>&1; then
                sudo apt-get update && sudo apt-get install -y build-essential cmake pkg-config
            elif command -v yum >/dev/null 2>&1; then
                sudo yum install -y gcc-c++ cmake pkg-config
            elif command -v dnf >/dev/null 2>&1; then
                sudo dnf install -y gcc-c++ cmake pkg-config
            else
                print_error "Не удалось установить зависимости автоматически"
                print_info "Пожалуйста, установите gcc/g++, cmake и pkg-config вручную"
                exit 1
            fi
            ;;
    esac
    
    print_success "Зависимости установлены"
}

# проверка компилятора
check_compiler() {
    print_info "Проверка компилятора..."
    
    if command -v g++ >/dev/null 2>&1; then
        GCC_VERSION=$(g++ --version | head -n1)
        print_success "GCC найден: $GCC_VERSION"
    elif command -v clang++ >/dev/null 2>&1; then
        CLANG_VERSION=$(clang++ --version | head -n1)
        print_success "Clang найден: $CLANG_VERSION"
        export CXX=clang++
    else
        print_error "Компилятор C++ не найден"
        exit 1
    fi
}

# компиляция программы
compile_program() {
    print_info "Компиляция GRUB Storm Utils..."
    
    # создание директории сборки
    mkdir -p build
    cd build
    
    # компиляция универсальной версии
    if command -v g++ >/dev/null 2>&1; then
        g++ -o GRUBStormUniversal ../src/universal_main.cpp
    else
        clang++ -o GRUBStormUniversal ../src/universal_main.cpp
    fi
    
    if [ $? -eq 0 ]; then
        print_success "Компиляция завершена успешно"
    else
        print_error "Ошибка компиляции"
        exit 1
    fi
    
    cd ..
}

# создание символьной ссылки
create_symlink() {
    print_info "Создание символьной ссылки..."
    
    if [ -w /usr/local/bin ]; then
        sudo ln -sf "$(pwd)/build/GRUBStormUniversal" /usr/local/bin/grub-storm
        print_success "Символьная ссылка создана: /usr/local/bin/grub-storm"
    else
        print_warning "Нет прав для создания ссылки в /usr/local/bin"
        print_info "Запускайте программу напрямую: ./build/GRUBStormUniversal"
    fi
}

# создание .desktop файла для графических сред
create_desktop_entry() {
    print_info "Создание .desktop файла..."
    
    DESKTOP_DIR="$HOME/.local/share/applications"
    DESKTOP_FILE="$DESKTOP_DIR/grub-storm.desktop"
    
    mkdir -p "$DESKTOP_DIR"
    
    cat > "$DESKTOP_FILE" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=GRUB Storm Utils
Comment=Утилита для управления GRUB
Exec=$(pwd)/build/GRUBStormUniversal
Icon=system-settings
Terminal=true
Categories=System;Settings;
Keywords=grub;boot;loader;system;
EOF
    
    chmod +x "$DESKTOP_FILE"
    print_success ".desktop файл создан: $DESKTOP_FILE"
}

# проверка прав доступа
check_permissions() {
    print_info "Проверка прав доступа..."
    
    if [ "$EUID" -eq 0 ]; then
        print_success "Запуск от имени root - полная функциональность"
    else
        print_warning "Запуск от имени обычного пользователя"
        print_warning "Некоторые функции могут быть недоступны"
        print_info "Для полной функциональности используйте: sudo grub-storm"
    fi
}

# функция удаления
uninstall() {
    print_info "Удаление GRUB Storm Utils..."
    
    # удаление символьной ссылки
    if [ -L /usr/local/bin/grub-storm ]; then
        sudo rm -f /usr/local/bin/grub-storm
        print_success "Символьная ссылка удалена"
    fi
    
    # удаление .desktop файла
    DESKTOP_FILE="$HOME/.local/share/applications/grub-storm.desktop"
    if [ -f "$DESKTOP_FILE" ]; then
        rm -f "$DESKTOP_FILE"
        print_success ".desktop файл удален"
    fi
    
    # удаление файлов программы
    if [ -d "build" ]; then
        rm -rf build
        print_success "Файлы программы удалены"
    fi
    
    print_success "GRUB Storm Utils полностью удален"
}

# функция тестирования
test_installation() {
    print_info "Тестирование установки..."
    
    if [ -f "build/GRUBStormUniversal" ]; then
        print_success "Исполняемый файл найден"
        
        # тестовый запуск
        echo "5" | ./build/GRUBStormUniversal >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            print_success "Программа запускается корректно"
        else
            print_warning "Возможны проблемы с запуском"
        fi
    else
        print_error "Исполняемый файл не найден"
        return 1
    fi
}

# главная функция
main() {
    echo "========================================"
    echo "    GRUB Storm Utils - Установщик"
    echo "========================================"
    echo
    
    # обработка аргументов командной строки
    case "${1:-}" in
        uninstall|remove|delete)
            uninstall
            exit 0
            ;;
        test)
            test_installation
            exit 0
            ;;
        help|--help|-h)
            echo "Использование: $0 [опция]"
            echo "Опции:"
            echo "  install     - установка программы (по умолчанию)"
            echo "  uninstall   - удаление программы"
            echo "  test        - тестирование установки"
            echo "  help        - показать эту справку"
            exit 0
            ;;
        "")
            # установка по умолчанию
            ;;
        *)
            print_error "Неизвестная опция: $1"
            echo "Используйте '$0 help' для справки"
            exit 1
            ;;
    esac
    
    # процесс установки
    detect_distro
    detect_package_manager
    check_permissions
    install_dependencies
    check_compiler
    compile_program
    create_symlink
    create_desktop_entry
    test_installation
    
    echo
    print_success "Установка GRUB Storm Utils завершена!"
    echo
    print_info "Способы запуска:"
    print_info "  1. grub-storm (если создана ссылка)"
    print_info "  2. ./build/GRUBStormUniversal"
    print_info "  3. Через меню приложений (в графических средах)"
    echo
    print_warning "Для полной функциональности используйте sudo"
    echo
}

# запуск основной функции
main "$@"
