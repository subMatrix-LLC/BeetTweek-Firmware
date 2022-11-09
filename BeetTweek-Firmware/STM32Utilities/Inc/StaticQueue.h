/*
 * Queue.h
 *
 *  Created on: Nov 11, 2021
 *      Author: casht
 */

#ifndef INC_STATICQUEUE_H_
#define INC_STATICQUEUE_H_


// Define the default capacity of the queue

// A class to represent a queue
template <class X, int SIZE>
class StaticQueue
{
public:
    X arr[SIZE];         // array to store queue elements
    int front = 0;      // front points to the front element in the queue (if any)
    int rear = -1;       // rear points to the last element in the queue
    int count = 0;      // current size of the queue

    StaticQueue<X,SIZE>(const StaticQueue<X,SIZE>& ref)
    {
    	for(int i = 0; i < SIZE; i++){
    		arr[i] = ref.arr[i];
    	}
    	front = ref.front;
    	rear = ref.rear;
    	count = ref.count;
    }
    StaticQueue<X,SIZE>() = default;

    inline void Dequeue()
    {
		// check for queue underflow
		if(Empty())
		{
			Error_Handler();
		}

		front = (front + 1) % SIZE;
		count--;
    }
    inline void Enqueue(X item)
    {
        // check for queue overflow
        if (Full())
        {
            Error_Handler();
        }

        rear = (rear + 1) % SIZE;
        arr[rear] = item;
        count++;
    }
    inline X Front()
    {
        if (Empty())
        {
        	Error_Handler();
        }
        return arr[front];
    }
    inline int Size(){
    	 return count;
    }
    inline bool Empty(){
    	  return (count == 0);
    }
    inline bool Full(){
        return (count == SIZE);
    }
};








#endif /* INC_STATICQUEUE_H_ */
