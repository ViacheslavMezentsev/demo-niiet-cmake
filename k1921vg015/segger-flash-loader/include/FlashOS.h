#ifndef FLASHOS_H
#define FLASHOS_H

#include <stdint.h>

#define ONCHIP          (1)

#define MAX_NUM_SECTORS (512)
#define ALGO_VERSION    (0x0101)

typedef struct {
  uint32_t SectorSize;
  uint32_t SectorStartAddr;
} SECTOR_INFO;

typedef struct {
   uint16_t AlgoVer;
   uint8_t  Name[128];
   uint16_t Type;
   uint32_t BaseAddr;
   uint32_t TotalSize;
   uint32_t PageSize;
   uint32_t Reserved;
   uint8_t  ErasedVal;
   uint32_t TimeoutProg;
   uint32_t TimeoutErase;
   SECTOR_INFO SectorInfo[MAX_NUM_SECTORS];
} DeviceInfo;

// Макросы для размещения в спец. секциях
#define SECTION_PRGCODE __attribute__ ((section ("PrgCode")))
#define SECTION_DEVDSCR __attribute__ ((section ("DevDscr")))

#endif // FLASHOS_H
