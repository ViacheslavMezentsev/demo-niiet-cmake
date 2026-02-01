/**
 * @file    loader.c
 * @brief   Алгоритм прошивки Flash-памяти для К1921ВГ015 (SEGGER Flash Loader).
 *          Основан на спецификации контроллера Flash и примере UART-загрузчика.
 */
#include "K1921VG015.h"
#include "system_k1921vg015.h"
#include "plib015_flash.h"
#include "mtimer.h"
#include "FlashOS.h"

#define __NOP() __asm__ __volatile__ ("nop")

#define __NOP5() \
    __NOP();     \
    __NOP();     \
    __NOP();     \
    __NOP();     \
    __NOP()

// -----------------------------------------------------------------------------
// Обязательные функции SEGGER Flash Loader API
// -----------------------------------------------------------------------------

/**
 * @brief  Инициализация (SEGGER_FL_Prepare).
 *         Вызывается J-Link перед началом любых операций с Flash.
 *
 * @param  Addr: Базовый адрес.
 * @param  Freq: Частота.
 * @param  Func: Тип операции (1-Erase, 2-Program, 3-Verify).
 * @return 0 - Успех.
 */
SECTION_PRGCODE int Init( uint32_t Addr, uint32_t Freq, uint32_t Func )
{
    ( void ) Addr;
    ( void ) Freq;
    ( void ) Func;

    // Инициализация системы.
    SystemInit();

    // Обновление системной частоты.
    SystemCoreClockUpdate();

    return OK;
}


/**
 * @brief  Деинициализация (SEGGER_FL_Restore).
 *         Вызывается J-Link после завершения операций.
 *
 * @return 0 - Успех.
 */
SECTION_PRGCODE int UnInit( uint32_t Func )
{
    ( void ) Func;

    return OK;
}


/**
 * @brief  Стирание сектора (SEGGER_FL_EraseSector).
 *         Стирает один сектор Flash по указанному адресу.
 *
 * @param  SectorAddr: Адрес сектора (выровнен на размер сектора).
 * @param  SectorIndex: Индекс сектора
 * @param  NumSectors: Количество секторов
 * @return 0 - Успех.
 */
SECTION_PRGCODE int EraseSector( uint32_t SectorAddr, uint32_t SectorIndex, uint32_t NumSectors )
{
    ( void ) NumSectors;
    ( void ) SectorIndex;

    // Записываем адрес сектора.
    FLASH_SetAddr( SectorAddr );

    // Отправляем команду стирания сектора.
    FLASH_SetCmd( FLASH_CMD_ERSEC_Msk, FLASH_Region_Main );

    // Даем время аппаратуре взвести флаг BUSY.
    __NOP5();

    // Ждём завершения операции.
    while ( FLASH_BusyStatus() ) {}

    return OK;
}


/**
 * @brief  Программирование страницы (SEGGER_FL_Program).
 *         Записывает данные во Flash.
 *
 * @note   Контроллер К1921ВГ015 имеет буфер записи 128 бит (16 байт).
 *         J-Link будет вызывать эту функцию с размером, кратным FlashDevice.PageSize (16).
 *
 * @param  DestAddr: Адрес назначения (выровнен на 16 байт).
 * @param  NumBytes: Количество байт (кратно 16).
 * @param  pSrcBuff: Указатель на данные.
 * @return 0 - Успех.
 */
SECTION_PRGCODE int ProgramPage( uint32_t DestAddr, uint32_t NumBytes, uint8_t* pSrcBuff )
{
    // Цикл по 16 байт (размер аппаратного буфера записи)
    while ( NumBytes >= MEM_FLASH_BUS_WIDTH_WORDS )
    {
        // Устанавливаем адрес.
        FLASH_SetAddr( DestAddr );

        // Заполняем 4 регистра данных (DATA[0]..DATA[3]).
        // Внимание: pSrcBuff может быть не выровнен на 4 байта,
        // поэтому собираем uint32_t вручную побайтно (Little Endian).
        for ( int i = 0; i < MEM_FLASH_BUS_WIDTH_WORDS / sizeof( uint32_t ); i++ )
        {
            uint32_t val = 0;

            val |= ( uint32_t ) pSrcBuff[0] << 0;
            val |= ( uint32_t ) pSrcBuff[1] << 8;
            val |= ( uint32_t ) pSrcBuff[2] << 16;
            val |= ( uint32_t ) pSrcBuff[3] << 24;

            FLASH_SetData( i, val );

            pSrcBuff += 4;
        }

        // Отправляем команду записи.
        FLASH_SetCmd( FLASH_Cmd_Write, FLASH_Region_Main );

        // Даем время аппаратуре взвести флаг BUSY.
        __NOP5();

        // Ждём завершения операции.
        while ( FLASH_BusyStatus() ) {}

        // Переходим к следующему блоку.
        DestAddr += MEM_FLASH_BUS_WIDTH_WORDS;
        NumBytes -= MEM_FLASH_BUS_WIDTH_WORDS;
    }

    return OK;
}


/**
 * @brief  Полное стирание чипа (SEGGER_FL_EraseChip).
 *         Значительно ускоряет процесс, если нужно стереть всё.
 */
SECTION_PRGCODE int EraseChip( void )
{
    FLASH_EraseFull( FLASH_Region_Main );

    return OK;
}


// -----------------------------------------------------------------------------
// Описание устройства
// -----------------------------------------------------------------------------

/**
 * @brief  Структура описания Flash-памяти для J-Link.
 *         Располагается в секции DevDscr.
 */
SECTION_DEVDSCR const DeviceInfo FlashDevice =
{
    ALGO_VERSION,              // Версия алгоритма (0x0101)
    "K1921VG015 Internal",     // Имя устройства (отображается в J-Link)
    ONCHIP,                    // Тип (1 = On-chip Flash)
    MEM_FLASH_BASE,            // Базовый адрес: 0x80000000
    MEM_FLASH_SIZE,            // Полный размер: 1 МБ (0x100000)
    2048,                      // Размер страницы программирования
    0,                         // Зарезервировано
    0xFF,                      // Значение стертой ячейки
    20,                        // Таймаут записи страницы (мс)
    400,                       // Таймаут стирания сектора (мс)
    // Карта секторов
    {
        { MEM_FLASH_PAGE_SIZE, 0x00000000 },    // Сектор 0: 256 x 4 КБ
        { 0xFFFFFFFF, 0xFFFFFFFF }              // Маркер конца таблицы
    }
};


// -----------------------------------------------------------------------------
// Таблица функций
// -----------------------------------------------------------------------------

/**
 * @brief  Таблица точек входа для J-Link.
 *         Должна располагаться в начале секции PrgCode.
 *         Секция PrgCodeTable гарантирует это в loader.ld.
 */
__attribute__( ( section( "PrgCodeTable" ) ) ) void ( *const g_pFuncs[] )( void ) = {
    ( void ( * )( void ) ) Init,
    ( void ( * )( void ) ) UnInit,
    ( void ( * )( void ) ) EraseSector,
    ( void ( * )( void ) ) ProgramPage,
    ( void ( * )( void ) ) 0,
    ( void ( * )( void ) ) EraseChip

    // Опциональные функции не реализованы.
    // (void (*)(void))Verify,     // Optional (SEGGER_FL_Verify)
    // (void (*)(void))EraseChip,  // Optional (SEGGER_FL_EraseChip)
    // (void (*)(void))CheckBlank, // Optional (SEGGER_FL_CheckBlank)
    // (void (*)(void))Read,       // Optional (SEGGER_FL_Read)
    // (void (*)(void))CalcCRC,    // Optional (SEGGER_FL_CalcCRC)
    // (void (*)(void))Start,      // Optional (SEGGER_FL_Start)
    // (void (*)(void))GetInfo     // Optional (SEGGER_FL_GetFlashInfo)
};
