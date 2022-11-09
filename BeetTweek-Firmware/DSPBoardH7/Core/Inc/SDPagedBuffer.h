/*
 * SDPagedBuffer.h
 *
 *  Created on: May 20, 2021
 *      Author: casht
 */

#ifndef INC_SDPAGEDBUFFER_H_
#define INC_SDPAGEDBUFFER_H_

#include "MathExtras.h"
#include "sdmmc.h"




//Handles random access to SD streaming.
template <class T, int32_t RAMBUFFERSIZE>
class SDBuffer
{
public:
	void SetSDStartAddress(uint32_t startAddress)
	{
		SDStartAdd = startAddress;
	}

	void Init()
	{
		//Make sure ram buffer size corresponds to SD size.
		if(RAMBUFFERSIZE%(512*2) != 0)
		{
			strcpy(errorCode, "SDBuffer Ram size error");
			Error_Handler();
		}
	}


	inline void Seek(int32_t pos)
	{

	}

	inline void sdskdjfhskdjfh()
	{


	}

	inline void WriteForward(T value)
	{
		buffer[IOHead_RAM] = value;


		IOHead_RAM_1 = IOHead_RAM;
		IOHead_RAM = MathExtras::WrapOnceMax(IOHead_RAM+1, RAMBUFFERSIZE);


		if(IOHead_RAM_1 >= RAM_B)
		{
			//wrote in B
			RAM_B_ContentSize++;

			if(IOHead_RAM < RAM_B)
			{
				//previous write operation crossed from B to A
				RAM_A_ContentStart = 0;
				RAM_A_ContentSize = 1;

				//start writing B to disk for the valid content in B
				//block until not busy
				while(HAL_SD_GetState(&hsd1) != HAL_SD_STATE_READY) {}


				HAL_StatusTypeDef status = HAL_SD_WriteBlocks_IT(&hsd1, (uint8_t*)&buffer[RAM_B], IOHead_SDBLKADDR + SDStartAdd, RAMBUFFERSIZE/(512*2));
				if(status != HAL_OK)
				{
					strcpy(errorCode, "SDPagedBuffer WriteForward");
					Error_Handler();
				}

				IOHead_SDBLKADDR_1 = IOHead_SDBLKADDR;
				IOHead_SDBLKADDR  += RAMBUFFERSIZE/(512*2);
			}
		}
		else
		{
			//wrote in A
			RAM_A_ContentSize++;


			if(IOHead_RAM >= RAM_B)
			{
				//previous write operation crossed from A to B
				RAM_B_ContentStart = 0;
				RAM_B_ContentSize  = 1;

				//start writing A to disk for the valid content in A

				//block until not busy
				while(HAL_SD_GetState(&hsd1) != HAL_SD_STATE_READY) {}

				HAL_StatusTypeDef status = HAL_SD_WriteBlocks_IT(&hsd1, (uint8_t*)&buffer[RAM_A], IOHead_SDBLKADDR + SDStartAdd, RAMBUFFERSIZE/(512*2));
				if(status != HAL_OK)
				{
					strcpy(errorCode, "SDPagedBuffer WriteForward");
					Error_Handler();
				}
				IOHead_SDBLKADDR_1 = IOHead_SDBLKADDR;
				IOHead_SDBLKADDR  += RAMBUFFERSIZE/(512*2);
			}

		}



	}

	inline void WriteBackward(T value)
	{
		buffer[IOHead_RAM] = value;

		IOHead_RAM_1 = IOHead_RAM;
		IOHead_RAM = MathExtras::WrapOnceMax(IOHead_RAM-1, RAMBUFFERSIZE);


	}


//	inline void WriteBuffer(T* value, int size)
//	{
//		if(size > RAMBUFFERSIZE)
//		{
//			Error_Handler();
//		}
//
//		if(RAMBUFFERSIZE/3 + IOHeadPageCords > RAMBUFFERSIZE)
//		{
//			//a loop is needed so 2 writes.
//			int32_t size1 = RAMBUFFERSIZE - (RAMBUFFERSIZE/3 + IOHeadPageCords);
//			int32_t size2 = size - size1;
//
//			memcpy(&buffer[RAMBUFFERSIZE/3 + IOHeadPageCords] ,
//					value,
//					RAMBUFFERSIZE - (RAMBUFFERSIZE/3 + IOHeadPageCords));
//
//			memcpy(&buffer[0] ,
//					&value[size1],
//					size2);
//
//			MID_DIRTY = 1;
//			RIGHT_DIRTY = 1;
//			LEFT_DIRTY = 1;
//		}
//		else
//		{
//			//just write.
//			memcpy(&buffer[RAMBUFFERSIZE/3 + IOHeadPageCords] , value, size);
//			MID_DIRTY = 1;
//
//			if(RAMBUFFERSIZE/3 + IOHeadPageCords >= RIGHT)
//			{
//				RIGHT_DIRTY = 1;
//			}
//		}
//
//
//		int32_t newHead = IOHead_SDBLKADDR + size;
//		newHead = MathExtras::WrapMax(newHead, TOTALSIZE);
//		Seek(newHead);
//	}
//
//
//	inline T Read()
//	{
//		return buffer[RAMBUFFERSIZE/3 + IOHeadPageCords];
//
//		int32_t newHead = IOHead_SDBLKADDR + 1;
//		newHead = MathExtras::WrapMax(newHead, TOTALSIZE);
//		Seek(newHead);
//	}
//




	int32_t IOHead_SDBLKADDR = 0;
	int32_t IOHead_SDBLKADDR_1 = 0;

	int32_t IOHead_RAM = 0;
	int32_t IOHead_RAM_1 = 0;

	int32_t RAM_A = 0;
	int32_t RAM_B = RAMBUFFERSIZE/2;

	int32_t RAM_A_ContentStart = 0;
	int32_t RAM_A_ContentSize = 0;

	int32_t RAM_B_ContentStart = 0;
	int32_t RAM_B_ContentSize = 0;


	int32_t SDStartAdd = 0;





	T buffer[RAMBUFFERSIZE] = {0};
};




































#endif /* INC_SDPAGEDBUFFER_H_ */
