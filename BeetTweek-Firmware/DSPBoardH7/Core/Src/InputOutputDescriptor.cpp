/*
 * InputOutputDescriptor.cpp
 *
 *  Created on: Apr 16, 2021
 *      Author: casht
 */




#include "InputOutputDescriptor.h"

InputOutputDescriptor inputOutputDescriptors[8];



void InputOutputDescriptor::AddLink(int descriptorIdx)
{
	if(curLinkIdx >= 8)
		return;//fully linked already to everything else.(list full)

	//check make sure the link does not already exist
	for(int i = 0; i < curLinkIdx; i++)
	{
		if(linkedAugmentsButtons[i] == descriptorIdx)
			return;//already added.
	}

	//add the link
	numLinks++;
	linkedAugmentsButtons[curLinkIdx] = descriptorIdx;
	curLinkIdx++;


	//also add the link to the other descriptor
	inputOutputDescriptors[descriptorIdx].numAugments = numAugments;
	inputOutputDescriptors[descriptorIdx].AddLink(buttonIdx);
}
