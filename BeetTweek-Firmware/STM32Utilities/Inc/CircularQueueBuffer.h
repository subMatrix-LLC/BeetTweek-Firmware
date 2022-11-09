/*
 * CircularBuffer.h
 *
 *  Created on: Jul 28, 2020
 *      Author: casht
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include "main.h"

#define CIRCULARBUFFER_DEFAULT_SIZE 128


template <class T, int size>
class CircularQueueBuffer
{
public:


	enum CircularBufferCode {
		CircularBufferCode_OK = 0,
		CircularBufferCode_HeadCrossRead, //head cross read indicating data loss
		CircularBufferCode_ReadCrossHead, //read cross head indicating no more relevant data to read from buffer
		CircularBufferCode_NoMoreData
	};

	//adds more data to the buffer
	CircularBufferCode AddNewData(T data)
	{
		buffer[headIndex] = data;
		headIndex++;


		if(headIndex >= size)//handle wrap around.
		{
			headIndex = 0;
		}


		if(headIndex == readIndex)//detect crossover
		{
			return CircularBufferCode_HeadCrossRead;
		}
		return CircularBufferCode_OK;
	}

	//reads one byte from the buffer and stores in data.  Advances read index internally.
	CircularBufferCode ReadOne(T* data)
	{
		if(headIndex == readIndex)//don't allow reading over the head.
		{
			return CircularBufferCode_NoMoreData;
		}

		*data = buffer[readIndex];
		readIndex++;


		if(readIndex >= size) //handle wrap around
		{
			readIndex = 0;
		}

		if(headIndex == readIndex)//detect crossover
		{
			return CircularBufferCode_ReadCrossHead;
		}

		return CircularBufferCode_OK;
	}


	//returns how far head leads read.  for example if 1 data can be read, then this will return 1.
	int HeadLeadAmount()
	{
		if(headIndex < readIndex)
		{
			return (size - readIndex) + headIndex;
		}
		else
		{
			return headIndex - readIndex;
		}
	}



	T buffer[size] = {0};

	uint16_t headIndex = 0;
	uint16_t readIndex = 0;
};





#endif /* CIRCULARBUFFER_H_ */
