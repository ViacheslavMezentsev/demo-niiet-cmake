cmake_minimum_required(VERSION 3.19)

# Указываем файл тулчейна
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-riscv-none-elf.cmake)

# Стандарт C/C++
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)


# Имя проекта
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

project(${PROJECT_NAME} C CXX ASM)

# Глобальные настройки для всего проекта.
add_compile_options(
    -march=rv32imfc_zba_zbb_zbc_zbs_zicsr
    -mabi=ilp32f
    -nostartfiles
    -mstrict-align
    -mno-save-restore
    -ffunction-sections
    -fdata-sections
    -fno-builtin
    -Wall
)

# Настройки линковщика.
add_link_options(
    --specs=nosys.specs
    -march=rv32imfc_zba_zbb_zbc_zbs_zicsr
    -mabi=ilp32f
    -nostartfiles

    -ffunction-sections
    -fdata-sections
    -Wl,--gc-sections

    -Wl,--start-group
    -lc
    -lm
    -lgcc
    -Wl,--end-group
)

# Создание артефактов сборки.
add_executable(${PROJECT_NAME} syscalls.c main.cpp)

# Определения препроцессора
target_compile_definitions(${PROJECT_NAME} PRIVATE
    HSECLK_VAL=16000000
    SYSCLK_PLL
    CKO_PLL0
)

# Подключение библиотек.
add_subdirectory(platform)
add_subdirectory(RTT)

target_link_libraries(${PROJECT_NAME}
    NIIET::NoSys
    NIIET::Nano
    RTT::RTT
)

# Подключение директорий с заголовочными файлами.
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Конфигурационный файл линковщика.
set(LINKER_SCRIPT "k1921vg015_flash.ld")
#set(LINKER_SCRIPT "k1921vg015_ram.ld")

# Опции линковщика.
target_link_options(${PROJECT_NAME} PRIVATE
    -L${CMAKE_CURRENT_SOURCE_DIR}/platform/Device/K1921VG015/ldscripts/
    -T${LINKER_SCRIPT}
    -Wl,-Map=${PROJECT_NAME}.map,--no-warn-rwx-segments,--print-memory-usage
    # Определяем символ end как синоним _end (стандарт GCC)
    -Wl,--defsym=end=_end
)

# Артефакты сборки.
niiet_generate_binary_file(${PROJECT_NAME})
niiet_generate_hex_file(${PROJECT_NAME})
niiet_generate_lss_file(${PROJECT_NAME})
niiet_print_size_of_target(${PROJECT_NAME})
