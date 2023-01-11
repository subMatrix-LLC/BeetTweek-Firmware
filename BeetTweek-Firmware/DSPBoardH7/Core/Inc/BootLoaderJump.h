/*
 * BootLoaderJump.h
 *
 *  Created on: Jul 29, 2020
 *      Author: casht
 */

#ifndef INC_BOOTLOADERJUMP_H_
#define INC_BOOTLOADERJUMP_H_

/** Bootloader error codes */
enum eBootloaderErrorCodes
{
    BL_OK = 0,      /*!< No error */
    BL_NO_APP,      /*!< No application found in flash */
    BL_SIZE_ERROR,  /*!< New application is too large for flash */
    BL_CHKS_ERROR,  /*!< Application checksum error */
    BL_ERASE_ERROR, /*!< Flash erase error */
    BL_WRITE_ERROR, /*!< Flash write error */
    BL_OBP_ERROR    /*!< Flash option bytes programming error */
} typedef eBootloaderErrorCodes;

/** Flash Protection Types */
enum eFlashProtectionTypes
{
    BL_PROTECTION_NONE  = 0,   /*!< No flash protection */
    BL_PROTECTION_WRP   = 0x1, /*!< Flash write protection */
    BL_PROTECTION_RDP   = 0x2, /*!< Flash read protection */
    BL_PROTECTION_PCROP = 0x4, /*!< Flash propietary code readout protection */
} typedef eFlashProtectionTypes;


void JumpToBootloader();



#endif /* INC_BOOTLOADERJUMP_H_ */
