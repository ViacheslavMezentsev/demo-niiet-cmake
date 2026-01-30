#include <K1921VG015.h>
#include <system_k1921vg015.h>
extern "C" {
#include <mtimer.h>
}
#include "SEGGER_RTT.h"
#include "version.h"

// SEGGER RTT: IP: localhost, PORT: 19021.
#define print(s)                        SEGGER_RTT_WriteString( 0, s )
#define println(s)                      print( s "\n" )
#define printf( format, ... )           SEGGER_RTT_printf( 0, ( const char * ) ( format ), ##__VA_ARGS__ )


int main( void )
{
    // Инициализация системы.
    SystemInit();

    // Обновление системной частоты.
    SystemCoreClockUpdate();

    // Настраиваем терминал 0 для работы в неблокирующем режиме.
    SEGGER_RTT_ConfigUpBuffer( 0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM );

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

    // Включаем глобальные прерывания.
    InterruptEnable();

    // Разрешаем тактирование GPIOC.
    RCU->CGCFGAHB_bit.GPIOCEN = 1;

    // Включаем  GPIOC.
    RCU->RSTDISAHB_bit.GPIOCEN = 1;

    GPIOC->OUTENSET = ( 1 << 0 );
    GPIOC->DATAOUTSET = ( 1 << 0 );

    // Мигаем светодиодом на плате.
    while ( 1 )
    {
        sleep( 500 ); // мс

        GPIOC->DATAOUTTGL |= ( 1 << 0 );
    }
}
