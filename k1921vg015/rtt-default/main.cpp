#include <K1921VG015.h>
#include <system_k1921vg015.h>
extern "C" {
#include <mtimer.h>
}
#include "SEGGER_RTT.h"
#include "version.h"

// SEGGER RTT: IP: localhost, PORT: 19021.
#define print(s)                        SEGGER_RTT_WriteString( 0, s ); sleep(1)
#define println(s)                      print( s "\n" )
#define printf( format, ... )           SEGGER_RTT_printf( 0, ( const char * ) ( format ), ##__VA_ARGS__ ); sleep(1)


/**
 * @brief   Выполняет тестирование RTT.
 *
 */
void RTT_PrintfTest()
{
    println( "###### Testing SEGGER_printf() ######" );

    printf( "printf Test: %%c,         'S' : %c.\n", 'S' );
    printf( "printf Test: %%5c,        'E' : %5c.\n", 'E' );
    printf( "printf Test: %%-5c,       'G' : %-5c.\n", 'G' );
    printf( "printf Test: %%5.3c,      'G' : %-5c.\n", 'G' );
    printf( "printf Test: %%.3c,       'E' : %-5c.\n", 'E' );
    printf( "printf Test: %%c,         'R' : %c.\n", 'R' );

    printf( "printf Test: %%s, \"RTT\\r\\nRocks.\" : %s.\n", "RTT\nRocks." );
    printf( "printf Test: %%s,      \"RTT\" : %s.\n", "RTT" );

    printf( "printf Test: %%u,       12345 : %u.\n", 12345 );
    printf( "printf Test: %%+u,      12345 : %+u.\n", 12345 );
    printf( "printf Test: %%.3u,     12345 : %.3u.\n", 12345 );
    printf( "printf Test: %%.6u,     12345 : %.6u.\n", 12345 );
    printf( "printf Test: %%6.3u,    12345 : %6.3u.\n", 12345 );
    printf( "printf Test: %%8.6u,    12345 : %8.6u.\n", 12345 );
    printf( "printf Test: %%08u,     12345 : %08u.\n", 12345 );
    printf( "printf Test: %%08.6u,   12345 : %08.6u.\n", 12345 );
    printf( "printf Test: %%0u,      12345 : %0u.\n", 12345 );
    printf( "printf Test: %%-.6u,    12345 : %-.6u.\n", 12345 );
    printf( "printf Test: %%-6.3u,   12345 : %-6.3u.\n", 12345 );
    printf( "printf Test: %%-8.6u,   12345 : %-8.6u.\n", 12345 );
    printf( "printf Test: %%-08u,    12345 : %-08u.\n", 12345 );
    printf( "printf Test: %%-08.6u,  12345 : %-08.6u.\n", 12345 );
    printf( "printf Test: %%-0u,     12345 : %-0u.\n", 12345 );

    printf( "printf Test: %%u,      -12345 : %u.\n", -12345 );
    printf( "printf Test: %%+u,     -12345 : %+u.\n", -12345 );
    printf( "printf Test: %%.3u,    -12345 : %.3u.\n", -12345 );
    printf( "printf Test: %%.6u,    -12345 : %.6u.\n", -12345 );
    printf( "printf Test: %%6.3u,   -12345 : %6.3u.\n", -12345 );
    printf( "printf Test: %%8.6u,   -12345 : %8.6u.\n", -12345 );
    printf( "printf Test: %%08u,    -12345 : %08u.\n", -12345 );
    printf( "printf Test: %%08.6u,  -12345 : %08.6u.\n", -12345 );
    printf( "printf Test: %%0u,     -12345 : %0u.\n", -12345 );
    printf( "printf Test: %%-.6u,   -12345 : %-.6u.\n", -12345 );
    printf( "printf Test: %%-6.3u,  -12345 : %-6.3u.\n", -12345 );
    printf( "printf Test: %%-8.6u,  -12345 : %-8.6u.\n", -12345 );
    printf( "printf Test: %%-08u,   -12345 : %-08u.\n", -12345 );
    printf( "printf Test: %%-08.6u, -12345 : %-08.6u.\n", -12345 );
    printf( "printf Test: %%-0u,    -12345 : %-0u.\n", -12345 );

    printf( "printf Test: %%d,      -12345 : %d.\n", -12345 );
    printf( "printf Test: %%+d,     -12345 : %+d.\n", -12345 );
    printf( "printf Test: %%.3d,    -12345 : %.3d.\n", -12345 );
    printf( "printf Test: %%.6d,    -12345 : %.6d.\n", -12345 );
    printf( "printf Test: %%6.3d,   -12345 : %6.3d.\n", -12345 );
    printf( "printf Test: %%8.6d,   -12345 : %8.6d.\n", -12345 );
    printf( "printf Test: %%08d,    -12345 : %08d.\n", -12345 );
    printf( "printf Test: %%08.6d,  -12345 : %08.6d.\n", -12345 );
    printf( "printf Test: %%0d,     -12345 : %0d.\n", -12345 );
    printf( "printf Test: %%-.6d,   -12345 : %-.6d.\n", -12345 );
    printf( "printf Test: %%-6.3d,  -12345 : %-6.3d.\n", -12345 );
    printf( "printf Test: %%-8.6d,  -12345 : %-8.6d.\n", -12345 );
    printf( "printf Test: %%-08d,   -12345 : %-08d.\n", -12345 );
    printf( "printf Test: %%-08.6d, -12345 : %-08.6d.\n", -12345 );
    printf( "printf Test: %%-0d,    -12345 : %-0d.\n", -12345 );

    printf( "printf Test: %%x,      0x1234ABC : %x.\n", 0x1234ABC );
    printf( "printf Test: %%+x,     0x1234ABC : %+x.\n", 0x1234ABC );
    printf( "printf Test: %%.3x,    0x1234ABC : %.3x.\n", 0x1234ABC );
    printf( "printf Test: %%.6x,    0x1234ABC : %.6x.\n", 0x1234ABC );
    printf( "printf Test: %%6.3x,   0x1234ABC : %6.3x.\n", 0x1234ABC );
    printf( "printf Test: %%8.6x,   0x1234ABC : %8.6x.\n", 0x1234ABC );
    printf( "printf Test: %%08x,    0x1234ABC : %08x.\n", 0x1234ABC );
    printf( "printf Test: %%08.6x,  0x1234ABC : %08.6x.\n", 0x1234ABC );
    printf( "printf Test: %%0x,     0x1234ABC : %0x.\n", 0x1234ABC );
    printf( "printf Test: %%-.6x,   0x1234ABC : %-.6x.\n", 0x1234ABC );
    printf( "printf Test: %%-6.3x,  0x1234ABC : %-6.3x.\n", 0x1234ABC );
    printf( "printf Test: %%-8.6x,  0x1234ABC : %-8.6x.\n", 0x1234ABC );
    printf( "printf Test: %%-08x,   0x1234ABC : %-08x.\n", 0x1234ABC );
    printf( "printf Test: %%-08.6x, 0x1234ABC : %-08.6x.\n", 0x1234ABC );
    printf( "printf Test: %%-0x,    0x1234ABC : %-0x.\n", 0x1234ABC );

    println( "###### SEGGER_printf() Tests done. ######" );
}

/**
 * @brief   Точка входа.
 *
 */
int main( void )
{
    // Инициализация системы.
    SystemInit();

    // Обновление системной частоты.
    SystemCoreClockUpdate();

    // Настраиваем терминал 0 для работы в неблокирующем режиме.
    SEGGER_RTT_ConfigUpBuffer( 0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM );

    println( "SEGGER Real-Time-Terminal Sample" );

    // Версия компилятора GCC.
    printf( "  Compiler: GCC %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );

    printf( "  K1921VG015 SYSCLK = %d MHz\n", ( int ) ( SystemCoreClock / 1E6 ) );

    printf( "  UID[0] = 0x%X  UID[1] = 0x%X  UID[2] = 0x%X  UID[3] = 0x%X\n",
            ( unsigned int ) PMUSYS->UID[0],
            ( unsigned int ) PMUSYS->UID[1],
            ( unsigned int ) PMUSYS->UID[2],
            ( unsigned int ) PMUSYS->UID[3] );

    // Дата и время сборки (стандартные макросы препроцессора).
    printf( "  Build Date:    %s\n", __DATE__ );
    printf( "  Build Time:    %s\n", __TIME__ );

    // Версия прошивки.
    printf( "  Version: %u.%u.%u.%u (%02u.%02u.%02u %02u:%02u:%02u)\n",
            Version.Major, Version.Minor, Version.Build, Version.Revision,
            DAY, MON, YEAR, HOUR, MIN, SEC );

    // Тестируем RTT.
    RTT_PrintfTest();

    // Разрешаем тактирование GPIOC.
    RCU->CGCFGAHB_bit.GPIOCEN = 1;

    // Включаем  GPIOC.
    RCU->RSTDISAHB_bit.GPIOCEN = 1;

    GPIOC->OUTENSET = ( 1 << 0 );
    GPIOC->DATAOUTSET = ( 1 << 0 );

    // Мигаем светодиодом на плате.
    while ( 1 )
    {
        sleep( 100 ); // мс

        GPIOC->DATAOUTTGL |= ( 1 << 0 );
    }
}
