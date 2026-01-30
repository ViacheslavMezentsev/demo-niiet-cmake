cmake_minimum_required(VERSION 3.19)

target_include_directories(${PROJECT_NAME} PUBLIC Device/K1921VG015/include)

target_sources(${PROJECT_NAME} PRIVATE
    Device/K1921VG015/source/plic.c
    #Device/K1921VG015/source/printf.c
    Device/K1921VG015/source/sys_init.c
    Device/K1921VG015/source/mtimer.c

    Device/K1921VG015/source/system_k1921vg015.c
    Device/K1921VG015/source/startup_k1921vg015.S
)
