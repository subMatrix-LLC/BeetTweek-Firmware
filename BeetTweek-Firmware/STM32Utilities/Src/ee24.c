
#include "i2c.h"
#include "ee24.h"

#ifdef _EEPROM_ASFILE
#include "fatfs.h"

FIL EEPROMFILE;
#endif

#if (_EEPROM_USE_FREERTOS == 1)
#include "cmsis_os.h"
#define ee24_delay(x)   osDelay(x)
#else
#define ee24_delay(x)   HAL_Delay(x)
#endif

#if (_EEPROM_SIZE_KBIT == 1) || (_EEPROM_SIZE_KBIT == 2)
#define _EEPROM_PSIZE     8
#elif (_EEPROM_SIZE_KBIT == 4) || (_EEPROM_SIZE_KBIT == 8) || (_EEPROM_SIZE_KBIT == 16)
#define _EEPROM_PSIZE     16
#else
#define _EEPROM_PSIZE     32
#endif

uint8_t ee24_lock = 0;
//################################################################################################################
bool ee24_isConnected(void)
{
#ifdef  _EEPROM_ASFILE
	return true;
#endif

  #if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
  #endif
  if (HAL_I2C_IsDeviceReady(&_EEPROM_I2C, _EEPROM_ADDRESS, 2, 100)==HAL_OK)
    return true;
  else
    return false;	
}

#ifdef _EEPROM_ASFILE
bool ee24_initFile(void)
{
	if(EEPROMFILE.obj.lockid)//if already opened.
		return false;

	//check if exists
	FRESULT fr;
	FILINFO fno;
	bool exists;
	fr = f_stat(_EEPROM_FILENAME, &fno);
	switch (fr) {
		case FR_OK: exists = true; break;
		default: exists = false;
	}



	FRESULT res = f_open(&EEPROMFILE, _EEPROM_FILENAME, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	if(res != FR_OK)
		Error_Handler();

	FSIZE_t size = f_size(&EEPROMFILE);
	if(!exists)
	{
		uint8_t b[(_EEPROM_SIZE_KBIT/8)*1000] = {0};
		UINT bytesw=0;
		res = f_write(&EEPROMFILE, (void*)&b, (_EEPROM_SIZE_KBIT/8)*1000, &bytesw);
		if(res != FR_OK || bytesw != (_EEPROM_SIZE_KBIT/8)*1000)
			Error_Handler();

		res = f_lseek(&EEPROMFILE, 0);
	}

	return true;
}
#endif

//################################################################################################################
bool ee24_write(uint16_t address, uint8_t *data, size_t len, uint32_t timeout)
{
#ifdef  _EEPROM_ASFILE
	/* Create and Open a new text file object with write access */
	ee24_initFile();

  /* Write data to the text file */
  FRESULT res = f_lseek(&EEPROMFILE, address);
  if(res != FR_OK)
	  Error_Handler();
  uint32_t byteswritten=0;
  res = f_write(&EEPROMFILE, (void*)data, len, (void *)&byteswritten);

  if((byteswritten == len) && (res == FR_OK))
  {

  }
  else
  {
	  Error_Handler();
  }
  return true;
#else

  if (ee24_lock == 1)
    return false;


  ee24_lock = 1; 
  uint16_t w;
  uint32_t startTime = HAL_GetTick();
  #if	(_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN,GPIO_PIN_RESET);
  #endif
  while (1)
  {
    w = _EEPROM_PSIZE - (address  % _EEPROM_PSIZE);
    if (w > len)
    {
      w = len;
    }
    #if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, data, w, timeout) == HAL_OK)
    #elif (_EEPROM_SIZE_KBIT==4)
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, timeout) == HAL_OK)
    #elif (_EEPROM_SIZE_KBIT==8)
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, timeout) == HAL_OK)
    #elif (_EEPROM_SIZE_KBIT==16)
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, timeout) == HAL_OK)
    #else
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, data, w, timeout);
    if (status == HAL_OK)
    #endif
    {
      ee24_delay(10);
      len -= w;
      data += w;
      address += w;
      if (len == 0)
      {
        #if (_EEPROM_USE_WP_PIN==1)
        HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
        #endif
        ee24_lock = 0;
        return true;
      }
      if (HAL_GetTick() - startTime >= timeout) 
      {
        ee24_lock = 0;
        Error_Handler();
        return false;
      }
    }
    else
    {
      #if (_EEPROM_USE_WP_PIN==1)
      HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
      #endif
      ee24_lock = 0;
      Error_Handler();
      return false;		
    }

  }
  Error_Handler();
  return false;
#endif
}

void ee24_flush()
{
#ifdef  _EEPROM_ASFILE
	FRESULT res =f_sync(&EEPROMFILE);
	  if(res != FR_OK)
		  Error_Handler();
#endif
}


// write zeros.
bool ee24_write_zeros(uint16_t address, size_t lenInBytes, uint32_t timeout)
{
	uint8_t* zeros = malloc(lenInBytes);


	for(int i = 0; i < lenInBytes; i++)
		zeros[i] = 0;

	bool s = ee24_write(address, zeros, lenInBytes, timeout);

	free(zeros);
	return s;
}

// write FFFF
bool ee24_write_ffff(uint16_t address, size_t lenInBytes, uint32_t timeout)
{
	uint8_t* max = malloc(lenInBytes);


	for(int i = 0; i < lenInBytes; i++)
		max[i] = 0xFF;

	bool s = ee24_write(address, max, lenInBytes, timeout);

	free(max);
	return s;
}

//################################################################################################################
bool ee24_read(uint16_t address, uint8_t *data, size_t len, uint32_t timeout)
{
#ifdef  _EEPROM_ASFILE

	ee24_initFile();

	FRESULT res = f_lseek(&EEPROMFILE, address);
	  if(res != FR_OK)
		  Error_Handler();
	uint32_t bytesread=0;
	res = f_read(&EEPROMFILE, data, len, (UINT*)&bytesread);

	if((res == FR_OK))
	{

	}
	else
	{
	  Error_Handler();
	}



	return true;
#else
  if (ee24_lock == 1)
    return false;
  ee24_lock = 1;
  #if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
  #endif
  #if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, data, len, timeout) == HAL_OK)
  #elif (_EEPROM_SIZE_KBIT == 4)
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, timeout) == HAL_OK)
  #elif (_EEPROM_SIZE_KBIT == 8)
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, timeout) == HAL_OK)
  #elif (_EEPROM_SIZE_KBIT==16)
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, timeout) == HAL_OK)
  #else
  HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, data, len, timeout);
  if (status == HAL_OK)
  #endif
  {
    ee24_lock = 0;

    return true;
  }
  else
  {
    ee24_lock = 0;
    Error_Handler();
    return false;	
  }    
#endif
}



//helpers
bool ee24_write_8(uint16_t address, uint8_t data, uint32_t timeout)
{
	uint8_t dataBuff[1];
	dataBuff[0] = (data);
	return ee24_write(address, dataBuff, 1,timeout);
}
bool ee24_write_16(uint16_t address, uint16_t data, uint32_t timeout)
{
	uint8_t dataBuff[2];
	dataBuff[0] = (data) & 0x00FF; //lsb
	dataBuff[1] = (data) >> 8;     //msb

	return ee24_write(address, dataBuff, 2,timeout);
}
bool ee24_write_32(uint16_t address, uint32_t data, uint32_t timeout)
{
	uint8_t dataBuff[4];
	dataBuff[0] = (data) & 0x000000FF; //lsb
	dataBuff[1] = (data) >> 8;
	dataBuff[2] = (data) >> 16;
	dataBuff[3] = (data) >> 24;     //msb
	return ee24_write(address, dataBuff, 4, timeout);
}
bool ee24_write_float(uint16_t address, float data, uint32_t timeout)
{
	uint32_t dataAsInt = *(uint32_t*)(&data);
	uint8_t dataBuff[4];
	dataBuff[0] = (dataAsInt) & 0x000000FF; //lsb
	dataBuff[1] = (dataAsInt) >> 8;
	dataBuff[2] = (dataAsInt) >> 16;
	dataBuff[3] = (dataAsInt) >> 24;     //msb
	return ee24_write(address, dataBuff, 4, timeout);
}

bool ee24_read_8(uint16_t address, uint8_t* data, uint32_t timeout)
{
	uint8_t dataBuff[1];
	bool s = ee24_read(address, dataBuff, 1, timeout);

	if(s)
		(*data) = (dataBuff[0]);

	return s;
}
bool ee24_read_16(uint16_t address, uint16_t* data, uint32_t timeout)
{

	uint8_t dataBuff[2];

	bool s = ee24_read(address, dataBuff, 2, timeout);

	if(s)
		(*data) = (dataBuff[1]  << 8 | (dataBuff[0]));

	return s;
}
bool ee24_read_32(uint16_t address, uint32_t* data, uint32_t timeout)
{
	uint8_t dataBuff[4];

	bool s = ee24_read(address, dataBuff, 4, timeout);

	(*data) = ((dataBuff[0]) & 0x000000FF)
			| ((dataBuff[1] << 8) & 0x0000FF00)
			| ((dataBuff[2] << 16) & 0x00FF0000)
			| ((dataBuff[3] << 24) & 0xFF000000);

	return s;
}

bool ee24_read_float(uint16_t address, float* data, uint32_t timeout, float defaultValue)
{
	uint8_t dataBuff[4];

	bool s = ee24_read(address, dataBuff, 4, timeout);

	uint32_t dataAsInt = ((dataBuff[0]) & 0x000000FF)
			| ((dataBuff[1] << 8) & 0x0000FF00)
			| ((dataBuff[2] << 16) & 0x00FF0000)
			| ((dataBuff[3] << 24) & 0xFF000000);


	*data = *(float*)(&dataAsInt);

	if(isnanf(*data))
		*data = defaultValue;

	return s;
}









//################################################################################################################
bool ee24_eraseChip(void)
{
  const uint8_t eraseData[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF\
    , 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint32_t bytes = 0;
  while ( bytes < (_EEPROM_SIZE_KBIT * 256))
  {
    if (ee24_write(bytes, (uint8_t*)eraseData, sizeof(eraseData), 100) == false)
      return false;
    bytes += sizeof(eraseData);           
  }
  return true;  
}
//################################################################################################################
