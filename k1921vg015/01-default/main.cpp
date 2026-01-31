#include <K1921VG015.h>
#include <system_k1921vg015.h>
extern "C" {
#include <mtimer.h>
}
#include "version.h"

/**
 * @brief   Точка входа в программу.
 *
 */
int main( void )
{
    // Инициализация системы.
    SystemInit();

    // Настройка тактирования.
    SystemCoreClockUpdate();

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
