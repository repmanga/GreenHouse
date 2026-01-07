cmake_minimum_required(VERSION 3.16)
project(SmartGreenhouse VERSION 1.0.0 LANGUAGES C CXX)

# Настройки для CLion
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Автоматическое обновление при изменении файлов
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Выходные директории
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# === ОСНОВНЫЕ НАСТРОЙКИ ===

# 1. Включить заголовочные файлы
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# 2. Добавить поддиректории с заголовками
add_subdirectory(include)
add_subdirectory(src)

# 3. Искать заголовочные файлы в подкаталогах
file(GLOB_RECURSE HEADER_FILES
        include/*.h
        include/*.hpp
        src/*.h
        src/*.hpp
)

# 4. Показать найденные файлы (для отладки)
message(STATUS "Found header files:")
foreach(header ${HEADER_FILES})
    message(STATUS "  ${header}")
endforeach()

# 5. Добавить их в проект для отображения в IDE
add_custom_target(headers SOURCES ${HEADER_FILES})

# === ИСХОДНЫЕ ФАЙЛЫ ===

# Собрать все .cpp файлы из src/
file(GLOB_RECURSE SOURCE_FILES
        src/*.cpp
        src/*.c
)

# Исключить тестовые файлы (если нужно)
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*test.*")
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*mock.*")

# === СОЗДАНИЕ ИСПОЛНЯЕМОГО ФАЙЛА ===

# Для симуляции (нативный билд)
if(NOT ARDUINO_BUILD)
    add_executable(SmartGreenhouse ${SOURCE_FILES} ${HEADER_FILES})

    # Настройки компиляции
    target_compile_options(SmartGreenhouse PRIVATE
            -Wall
            -Wextra
            -Werror
            -O2
    )

    # Настройки линковки
    target_link_options(SmartGreenhouse PRIVATE
            -pthread  # если нужны потоки
    )

    # Для отладки в CLion
    set_target_properties(SmartGreenhouse PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )
endif()

# === ДОПОЛНИТЕЛЬНЫЕ ЦЕЛИ ===

# Цель для форматирования кода
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(format
            COMMAND ${CLANG_FORMAT} -i ${SOURCE_FILES} ${HEADER_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Formatting source files..."
    )
endif()

# Цель для генерации документации
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)
    add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating documentation..."
    )
endif()

# Цель для очистки билда
add_custom_target(clean-all
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
        COMMENT "Cleaning build directory..."
)