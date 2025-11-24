cmake_minimum_required(VERSION 3.12)
project(RadioControlSystem)

set(CMAKE_CXX_STANDARD 17)

# Добавляем поддиректории в правильном порядке зависимостей
add_subdirectory(Protocol)     # Базовая логика
add_subdirectory(DaemonLib)    # Библиотека демона  
add_subdirectory(System)       # Система (зависит от Protocol и DaemonLib)
add_subdirectory(DaemonApp)    # Приложение демона (зависит от System)
add_subdirectory(ClientApp)    # Клиентское приложение (зависит от System)
add_subdirectory(Test)         # Тесты