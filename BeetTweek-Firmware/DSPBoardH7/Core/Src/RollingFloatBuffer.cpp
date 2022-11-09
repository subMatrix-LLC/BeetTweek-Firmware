#include "RollingFloatBuffer.h"
#include "us_delay.h"
#include "stdio.h"


void RollingFloatBufferInit(RollingFloatBuffer* buffer)
{
	buffer->idx = 0;
	buffer->lastReadIdx = 0;
	buffer->isFull = 0;
}
void RollingFloatBufferAdd(RollingFloatBuffer* buffer, float val)
{
	buffer->buff[buffer->idx] = val;
	buffer->lastReadIdx = buffer->idx;

	buffer->idx++;

	if(buffer->idx >= ROLLING_BUFFER_SIZE)
		buffer->idx = 0;
}
float RollingFloatBufferLatest(RollingFloatBuffer* buffer)
{
	return buffer->buff[buffer->lastReadIdx];
}
void RollingFloatBufferPrint(RollingFloatBuffer* buffers[], int numBuffers)
{
	for(int i = 0; i < ROLLING_BUFFER_SIZE; i++)
	{
		for(int b = 0; b < numBuffers; b++)
		{
			if(b == numBuffers -1)
				printf("%f", RollingFloatBufferLatest(buffers[b]));//last column
			else
				printf("%f,",RollingFloatBufferLatest(buffers[b]));

			DelayUS(100);
		}
		printf("\n");
	}
}
