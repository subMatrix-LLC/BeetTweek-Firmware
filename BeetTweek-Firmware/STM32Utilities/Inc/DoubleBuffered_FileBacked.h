/*
 * DoubleBuffered_FileBacked.h
 *
 *  Created on: Sep 10, 2021
 *      Author: casht
 */

#ifndef INC_DOUBLEBUFFERED_FILEBACKED_H_
#define INC_DOUBLEBUFFERED_FILEBACKED_H_


//#define DOUBLEBUFFERED_FILEBACKED_DEBUG
//#define DOUBLEBUFFERED_FILEBACKED_NEVERBUSY

#include "MathExtras.h"
#include "fatfs.h"
#include "stdio.h"
#include "string.h"
#include "StaticQueue.h"


//Buffer with handling for each half
template <class T, int size>
class DoubleBuffer
{
public:

	virtual T ReadAdvance()
	{
		T value = buffer[bufferIdx];
		bufferIdx += 1;
		if(bufferIdx >= size)
		{
			bufferIdx = 0;
			bufferSide = 0;
			Flipped(0,1);
		}
		else if(bufferIdx == size/2)
		{
			bufferSide = 1;
			Flipped(1,1);
		}
		return value;
	}

	//discard rest of current buffer and set buffer idx to the next side and call flip code
	inline void skipBufferAndFlipForward()
	{
		if(bufferIdx < size/2)
		{
			bufferIdx = size/2;
			bufferSide = 1;
			Flipped(1,1);
		}
		else if(bufferIdx >= size/2)
		{
			bufferIdx = 0;
			bufferSide = 0;
			Flipped(0,1);
		}
	}
	inline void skipBufferForward()
	{
		if(bufferIdx < size/2)
		{
			bufferIdx = size/2;
			bufferSide = 1;
		}
		else if(bufferIdx >= size/2)
		{
			bufferIdx = 0;
			bufferSide = 0;
		}
	}



	inline T Value(){
		return buffer[bufferIdx];
	}

	inline int OffsetIdx(int offset)
	{
		return MathExtras::WrapOnceMax(bufferIdx, size);
	}

	//return the index in the current bufferSide
	inline int inSideIdx()
	{
		return MathExtras::WrapOnceMax(bufferIdx, size/2);
	}

	virtual void WriteAdvance(T value)
	{
		buffer[bufferIdx] = value;
		bufferIdx += 1;
		if(bufferIdx >= size)
		{
			bufferIdx = 0;
			bufferSide = 0;
			Flipped(0,1);
		}
		else if(bufferIdx == size/2)
		{
			bufferSide = 1;
			Flipped(1,1);
		}
	}


	virtual void Flipped(int buffSide, int dir)
	{

	}

	int bufferSide = 0;
	int bufferIdx = 0;
	T buffer[size] = {0};
};

enum DOUBLEBUFFER_FBACKED_READWRITEMODE
{
	READ_MODE = 0,
	WRITE_MODE = 1
};

template <class T, int size>
class DoubleBuffer_FileBacked : public DoubleBuffer<T, size>
{
public:

	DoubleBuffer_FileBacked() : DoubleBuffer<T, size>()
	{
		ResetToBeginning(READ_MODE);
	}


	struct PageAction
	{
		DOUBLEBUFFER_FBACKED_READWRITEMODE readWrite = READ_MODE;
		int page = 1;
		bool cont = false;//continue flag
	};

	virtual T ReadAdvanceSafe(bool& success)
	{
		if(AtEndOfBuffer())
		{
			success = false;
			return T();
		}

		if(Busy())
		{
			//waiting
			busyMisses++;
			success = false;
			return T();
		}


		T val;
		if(pageIdx == 0)
		{
			if(!auxLoaded)
			{
				success = false;
				return T();
			}

			val = auxBuffer[MathExtras::WrapOnceMax(DoubleBuffer<T,size>::bufferIdx, size/2)];
			DoubleBuffer<T,size>::ReadAdvance();
		}
		else
		{
			val = DoubleBuffer<T,size>::ReadAdvance();
		}

		success = true;
		return val;
	}

	//returns true if the last sample was read before the last reset.
	inline bool AtEndOfBuffer()
	{
		return LargeIndex() == endIdx;
	}

	//return true if the curren LargeIndex is less than or equal to the given idx.
	inline bool IndexWithinEnd(uint32_t idx)
	{
		return LargeIndex() <= idx;
	}

	//returns success
	virtual bool WriteAdvanceSafe(T value)
	{
		if(Busy())
		{
			busyMisses++;
			return false;
		}

		if(pageIdx == 0)
		{
			auxBuffer[MathExtras::WrapOnceMax(DoubleBuffer<T,size>::bufferIdx, size/2)] = value;
		}

		DoubleBuffer<T,size>::WriteAdvance(value);
		return true;
	}

	//
	bool Busy()
	{
#ifdef DOUBLEBUFFERED_FILEBACKED_NEVERBUSY
		return false;
#endif
		bool busy = (flipQueue.Size() != 1) && (flipQueue.Size() != 0 );

		if(busy)
			return true;
		else
			return false;
	}



	inline uint32_t LargeIndex()
	{
		return pageIdx*(size/2)+DoubleBuffer<T,size>::inSideIdx();
	}

	virtual void Flipped(int buffSide, int dir)
	{

		//take action from flipQueue and add to serviceQueue.
		PageAction act = flipQueue.Front();
		flipQueue.Dequeue();

		serviceQueue.Enqueue(act);


		pageIdx++;
	}

	//Reset to beginning of buffer
	void ResetToBeginning(DOUBLEBUFFER_FBACKED_READWRITEMODE readWrite)
	{

		PageAction act;
		if(readWrite == READ_MODE)
		{
			if(flipQueue.Size() && flipQueue.Front().readWrite == WRITE_MODE)
			{
				endIdx = LargeIndex();
				DoubleBuffer<T,size>::skipBufferForward();

				PageAction act = flipQueue.Front();
				act.cont = false;
				serviceQueue.Enqueue(act);
				flipQueue.Dequeue();
			}
			else
			{
				DoubleBuffer<T,size>::skipBufferForward();
				while(serviceQueue.Size()) serviceQueue.Dequeue();
				while(flipQueue.Size()) flipQueue.Dequeue();

			}

			act.page = 1;
			act.readWrite = readWrite;
			act.cont = true;
			serviceQueue.Enqueue(act);
		}
		else if(readWrite == WRITE_MODE)
		{
			DoubleBuffer<T,size>::skipBufferForward();

			while(serviceQueue.Size()) serviceQueue.Dequeue();
			while(flipQueue.Size()) flipQueue.Dequeue();


			endIdx = 0;
			act.page = 0;
			act.readWrite = readWrite;
			act.cont = true;

			flipQueue.Enqueue(act);
		}

		//set current page to 0 so writes/reads happen right on the AUX buffer.
		pageIdx = 0;
	}


	void ServiceFile(FIL* file)
	{
		//lock and grab service queue
		if(!auxLoaded)
			ReadIntoAux(file);

		//grab and copy the service queue, so that the ResetToBeginning doesn't change it while DiskRoutine is running.
		//(critical section).
		__disable_irq();
			serviceQueueCpy = serviceQueue;
			while(serviceQueue.Size()) serviceQueue.Dequeue();
		__enable_irq();

		while(serviceQueueCpy.Size() > 0 )
		{
			busyFlag = true;
			DiskRoutine1(file);
			serviceQueueCpy.Dequeue();
		}

		FRESULT res = f_sync(file);
		if(res != FR_OK)
		{
			strcpy(errorCode, "DoubleBuffer_FileBacked sync Error");
			Error_Handler();
		}

		busyFlag = false;



	}



	void DiskRoutine1(FIL* file)
	{

		UINT bytesWritten = 0;
		UINT bytesRead = 0;
		FRESULT res = FR_OK;

		//always act on the other side of buffer.
		int oppositeBufferStartIdx = MathExtras::WrapMax(DoubleBuffer<T,size>::bufferSide+1,2)*(size/2);

		//take a job from the action queue
		PageAction act = serviceQueueCpy.Front();

		if(act.readWrite == WRITE_MODE)
		{
			res = f_lseek(file, act.page*(size/2)*sizeof(T));
			if(res != FR_OK)
			{
				strcpy(errorCode, "DoubleBuffer_FileBacked write seek Error");
				Error_Handler();
			}
			res = f_write(file, &DoubleBuffer<T,size>::buffer[oppositeBufferStartIdx], sizeof(T)*(size/2), &bytesWritten);
			if(bytesWritten != sizeof(T)*(size/2))
			{
				strcpy(errorCode, "DoubleBuffer_FileBacked write Error");
				Error_Handler();
			}
			if(res != FR_OK)
			{
				strcpy(errorCode, "DoubleBuffer_FileBacked write Error");
				Error_Handler();
			}
//			res = f_sync(file);
//			if(res != FR_OK)
//			{
//				strcpy(errorCode, "DoubleBuffer_FileBacked write Error");
//				Error_Handler();
//			}


#ifdef DOUBLEBUFFERED_FILEBACKED_DEBUG
			printf("Current Page %d, Wrote Page %d, missedSamples %u\r\n", pageIdx, act.page, busyMisses);
#endif
		}
		else if(act.readWrite  == READ_MODE)
		{

			res = f_lseek(file, (act.page)*(size/2)*sizeof(T));
			if(res != FR_OK)
			{
				strcpy(errorCode, "DoubleBuffer_FileBacked read seek Error");
				Error_Handler();
			}

			res = f_read(file, &DoubleBuffer<T,size>::buffer[oppositeBufferStartIdx], sizeof(T)*(size/2), &bytesRead);
			if(res != FR_OK)
			{
				strcpy(errorCode, "DoubleBuffer_FileBacked read Error");
				Error_Handler();
			}

#ifdef DOUBLEBUFFERED_FILEBACKED_DEBUG
			printf("Current Page %d, Read Page %d, missedSamples %u\r\n", pageIdx, act.page, busyMisses);
#endif
		}

		if(act.cont && flipQueue.Size() < 1)
		{
			act.page++;
			flipQueue.Enqueue(act);
#ifdef DOUBLEBUFFERED_FILEBACKED_DEBUG
			printf("Added page %d to flipQueue, flipQueueSize: %d, BIDX: %d\r\n", act.page, flipQueue.Size(), MathExtras::WrapOnceMax(DoubleBuffer<T,size>::bufferIdx, size/2));
#endif
		}



	}

	void ReadIntoAux(FIL* file)
	{
		UINT bytesToRead = MathExtras::Min(endIdx*sizeof(T), (size/2)*sizeof(T));

		FRESULT res = f_lseek(file, 0*(size/2)*sizeof(T));
		if(res != FR_OK)
		{
			Error_Handler();
		}
		UINT bytesRead = 0;
		res = f_read(file, &auxBuffer[0], bytesToRead, &bytesRead);
		if(res != FR_OK)
		{
			Error_Handler();
		}
		auxLoaded = true;
	}


	int pageIdx = 0;

	StaticQueue<PageAction,10> serviceQueue;
	StaticQueue<PageAction,10> flipQueue;
	StaticQueue<PageAction,10> serviceQueueCpy;

	volatile bool lock = false;

	int bufferShift = 0;
	uint32_t endIdx = 0;

	bool busyFlag = false;
	uint32_t busyMisses = 0;

	T auxBuffer[size/2] = {0};
	bool auxLoaded = false;

};


#endif /* INC_DOUBLEBUFFERED_FILEBACKED_H_ */
