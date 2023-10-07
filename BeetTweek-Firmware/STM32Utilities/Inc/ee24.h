#ifndef	_EE24_H
#define	_EE24_H

#ifdef __cplusplus
extern "C" {
#endif
/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    2.2.1
  
  
  Reversion History:
  
  (2.2.1)
  Fix erase chip bug.
  
  (2.2.0)
  Add erase chip function.
  
  (2.1.0)
  Fix write bytes.
  
  (2.0.0)
  Rewrite again.

*/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ee24Config.h"

bool    ee24_isConnected(void);
bool    ee24_write(uint16_t address, uint8_t *data, size_t lenInBytes, uint32_t timeout);	
bool    ee24_write_8(uint16_t address, uint8_t data, uint32_t timeout);
bool    ee24_write_16(uint16_t address, uint16_t data, uint32_t timeout);
bool    ee24_write_32(uint16_t address, uint32_t data, uint32_t timeout);
bool    ee24_write_float(uint16_t address, float data, uint32_t timeout);
bool    ee24_write_zeros(uint16_t address, size_t lenInBytes, uint32_t timeout);
bool    ee24_write_ffff(uint16_t address, size_t lenInBytes, uint32_t timeout);



void    ee24_flush();

bool    ee24_read(uint16_t address, uint8_t* data, size_t lenInBytes, uint32_t timeout);
bool    ee24_read_8(uint16_t address, uint8_t* data, uint32_t timeout);
bool    ee24_read_16(uint16_t address, uint16_t* data, uint32_t timeout);
bool    ee24_read_32(uint16_t address, uint32_t* data, uint32_t timeout);
bool    ee24_read_float(uint16_t address, float* data, uint32_t timeout, float defaultValue);

bool    ee24_eraseChip(void);
#ifdef _EEPROM_ASFILE
bool    ee24_initFile(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
