cmake_minimum_required(VERSION 3.19)

# Указываем файл тулчейна
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-riscv-none-elf.cmake)

# Стандарт C/C++
set(CMAKE_C_STANDARD 17)

# Имя проекта
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

project(${PROJECT_NAME} C ASM)

# Исходники.
add_executable(${PROJECT_NAME} src/loader.c)

add_subdirectory(platform)

# Определения препроцессора
target_compile_definitions(${PROJECT_NAME} PRIVATE
    HSECLK_VAL=16000000
    SYSCLK_PLL
    CKO_PLL0
)

# Зависимости.
target_link_libraries(${PROJECT_NAME}
    NIIET::NoSys
    NIIET::Nano
)

# Заголовочные файлы.
target_include_directories(${PROJECT_NAME} PRIVATE include src/RTT)

# Опции компиляции для Flash Loader
# Важно: -fPIC для работы в любом месте RAM
# -mno-save-restore чтобы не зависеть от библиотечных функций
target_compile_options(${PROJECT_NAME} PRIVATE
    -march=rv32imc_zba_zbb_zbc_zbs_zicsr
    -mabi=ilp32
    -fPIC
    -fno-builtin
    -mno-save-restore
    -O2
    -g0 # Отладочная информация не нужна внутри алгоритма
    -Wall
)

# Опции линковки
target_link_options(${PROJECT_NAME} PRIVATE
    -nostartfiles
    #-nodefaultlibs
    -fPIC
    -Wl,--gc-sections
    -T${CMAKE_CURRENT_SOURCE_DIR}/loader.ld
    -Wl,-Map=${PROJECT_NAME}.map,--no-warn-rwx-segments,--print-memory-usage
)

# Артефакты сборки.
niiet_generate_binary_file(${PROJECT_NAME})
