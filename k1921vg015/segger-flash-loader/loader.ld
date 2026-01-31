/* Linker script for SEGGER Flash Loader (K1921VG015 RISC-V) */

/* Указываем архитектуру и формат выходного файла */
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)

/* Точка входа не важна для J-Link, но полезна, чтобы линковщик не ругался */
ENTRY(Init)

/*
    Описываем память.
*/
MEMORY
{
    RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 32K
}

SECTIONS
{
    /*
       Секция PrgCode.
       ОБЯЗАТЕЛЬНО должна идти первой.
       ОБЯЗАТЕЛЬНО должна начинаться с таблицы функций.
    */
    PrgCode :
    {
        . = ALIGN(4);
        /* Сначала кладем таблицу указателей (решение проблемы конфликта типов) */
        KEEP(*(PrgCodeTable))

        /* Затем основной код алгоритма */
        KEEP(*(PrgCode))

        /* Затем весь остальной код и константы */
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.srodata .srodata.*) /* Секция для малых данных read-only в RISC-V */

        . = ALIGN(4);
    } > RAM

    /*
       Секция PrgData.
       Здесь лежат глобальные переменные.
    */
    PrgData :
    {
        . = ALIGN(4);
        /* Инициализированные данные */
        *(.data .data.*)
        *(.sdata .sdata.*) /* Секция для малых данных (gp-relative) в RISC-V */

        /* Неинициализированные данные (BSS) */
        /* J-Link сам не чистит BSS, поэтому лучше избегать статических переменных = 0,
           либо чистить их вручную в Init(), если это критично. */
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)

        . = ALIGN(4);
    } > RAM

    /*
       Секция DevDscr.
       Содержит структуру FlashDevice. Должна быть в конце.
    */
    DevDscr :
    {
        . = ALIGN(4);
        KEEP(*(DevDscr))
        . = ALIGN(4);
    } > RAM

    /*
       Отбрасываем всё лишнее, чтобы уменьшить размер файла
       и убрать ARM-специфичные артефакты, если они вдруг появятся.
    */
    /DISCARD/ :
    {
        *(.eh_frame)
        *(.riscv.attributes)
        *(.comment)
        *(.note.GNU-stack)
    }
}
