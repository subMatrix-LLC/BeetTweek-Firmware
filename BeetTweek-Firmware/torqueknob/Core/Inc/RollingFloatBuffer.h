#ifndef ROLLING_FLOAT_BUFFER_H
#define ROLLING_FLOAT_BUFFER_H


#define ROLLING_BUFFER_SIZE 1024
typedef struct {
	int idx;
	int lastReadIdx;
	float buff[ROLLING_BUFFER_SIZE];


	int isFull;

} RollingFloatBuffer;


void RollingFloatBufferInit(RollingFloatBuffer* buffer);
void RollingFloatBufferAdd(RollingFloatBuffer* buffer, float val);
float RollingFloatBufferLatest(RollingFloatBuffer* buffer);
void RollingFloatBufferPrint(RollingFloatBuffer* buffers[], int numBuffers);


#endif
