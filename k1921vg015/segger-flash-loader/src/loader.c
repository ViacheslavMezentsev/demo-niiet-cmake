/**
 * @file    loader.c
 * @brief   Алгоритм прошивки Flash-памяти для К1921ВГ015 (SEGGER Flash Loader).
 *          Основан на спецификации контроллера Flash и примере UART-загрузчика.
 */
#include "K1921VG015.h"
#include "system_k1921vg015.h"
#include "mtimer.h"
#include "FlashOS.h"

/* Магический ключ для доступа к регистру CMD (см. PDF стр. 2) */
#define FLASH_CMD_KEY_ACCESS    0xC0DE

// -----------------------------------------------------------------------------
// Вспомогательные функции
// -----------------------------------------------------------------------------

/**
 * @brief  Ожидание освобождения контроллера Flash.
 *         Wait until Flash Controller is ready.
 *
 * @note   Проверяет бит BUSY (бит 0) в регистре STAT.
 */
SECTION_PRGCODE static void _WaitBusy( void )
{
    // Ожидаем, пока бит BUSY (0) установлен в 1
    while ( FLASH->STAT_bit.BUSY )
    {
        // Можно добавить сброс Watchdog, если он активен
    }
}


// -----------------------------------------------------------------------------
// Обязательные функции SEGGER Flash Loader API
// -----------------------------------------------------------------------------

/**
 * @brief  Инициализация (SEGGER_FL_Prepare).
 *         Вызывается J-Link перед началом любых операций с Flash.
 *
 * @param  Addr: Базовый адрес (0x80000000).
 * @param  Freq: Частота ядра в кГц (информативно).
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

    // Включаем тактирование контроллера Flash.
    // Обычно оно включено по умолчанию, но для надежности пропишем.
    // При работе через JTAG RCU может быть уже настроен скриптом Reset.JLinkScript,
    // но повторное включение безопасно.
    // См. main.c: RCU->CGCFGAHB_bit.XXXX (в примере Flash явно не включается,
    // подразумевается активность по сбросу).

    // В документации (PDF стр. 5) есть регистр CFGWORD (0x00001FF0) с битом FLASHWE (бит 0).
    // Он разрешает запись. При необходимости можно проверить его.

    return 0;
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

    return 0;
}


/**
 * @brief  Стирание сектора (SEGGER_FL_Erase).
 *         Выполняет стирание страницы 4 КБ.
 *
 * @param  SectorAddr: Абсолютный адрес начала сектора (например, 0x80000000).
 * @return 0 - Успех, 1 - Ошибка (таймаут).
 */
SECTION_PRGCODE int EraseSector( uint32_t SectorAddr )
{
    // 1. Ждем, пока контроллер освободится от предыдущих операций
    _WaitBusy();

    // 2. Записываем адрес сектора в регистр ADDR
    FLASH->ADDR = SectorAddr;

    // 3. Отправляем команду стирания сектора
    // CMD = KEY(0xC0DE) | ERSEC(0x4)
    FLASH->CMD = ( FLASH_CMD_KEY_ACCESS << FLASH_CMD_KEY_Pos ) | FLASH_CMD_ERSEC_Msk;

    // 4. Даем время аппаратуре взвести флаг BUSY
    //_ShortDelay();
    usleep(1);

    // 5. Ждем завершения операции
    _WaitBusy();

    // В PDF нет регистра явной ошибки стирания, полагаемся на завершение BUSY
    return 0;
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
        // 1. Ждем готовности
        _WaitBusy();

        // 2. Устанавливаем адрес
        FLASH->ADDR = DestAddr;

        // 3. Заполняем 4 регистра данных (DATA[0]..DATA[3])
        // Внимание: pSrcBuff может быть не выровнен на 4 байта,
        // поэтому собираем uint32_t вручную побайтно (Little Endian).
        for ( int i = 0; i < 4; i++ )
        {
            uint32_t val = 0;
            val |= ( uint32_t ) pSrcBuff[0] << 0;
            val |= ( uint32_t ) pSrcBuff[1] << 8;
            val |= ( uint32_t ) pSrcBuff[2] << 16;
            val |= ( uint32_t ) pSrcBuff[3] << 24;

            FLASH->DATA[i].DATA = val;

            pSrcBuff += 4;
        }

        // 4. Отправляем команду записи
        // CMD = KEY(0xC0DE) | WR(0x2)
        FLASH->CMD = ( FLASH_CMD_KEY_ACCESS << FLASH_CMD_KEY_Pos ) | FLASH_CMD_WR_Msk;

        // 5. Даем время аппаратуре взвести флаг BUSY
        //_ShortDelay();
        usleep(1);

        // 6. Ждем завершения записи блока
        _WaitBusy();

        // 7. Переходим к следующему блоку
        DestAddr += MEM_FLASH_BUS_WIDTH_WORDS;
        NumBytes -= MEM_FLASH_BUS_WIDTH_WORDS;
    }

    return 0;
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
    MEM_FLASH_PAGE_TOTAL,      // Размер страницы программирования
    0,                         // Зарезервировано
    0xFF,                      // Значение стертой ячейки
    4,                         // Таймаут записи страницы (мс)
    400,                       // Таймаут стирания сектора (мс)
    // Карта секторов
    {
        { MEM_FLASH_PAGE_SIZE, 0x00000000 },    // Сектора по 4 КБ (MEM_FLASH_PAGE_SIZE = 4096)
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
    // Опциональные функции не реализованы.
    // (void (*)(void))Verify,     // Optional (SEGGER_FL_Verify)
    // (void (*)(void))EraseChip,  // Optional (SEGGER_FL_EraseChip)
    // (void (*)(void))CheckBlank, // Optional (SEGGER_FL_CheckBlank)
    // (void (*)(void))Read,       // Optional (SEGGER_FL_Read)
    // (void (*)(void))CalcCRC,    // Optional (SEGGER_FL_CalcCRC)
    // (void (*)(void))Start,      // Optional (SEGGER_FL_Start)
    // (void (*)(void))GetInfo     // Optional (SEGGER_FL_GetFlashInfo)
};
