/*
 * InputOutputDescriptor.h
 *
 *  Created on: Apr 16, 2021
 *      Author: casht
 */

#ifndef INC_INPUTOUTPUTDESCRIPTOR_H_
#define INC_INPUTOUTPUTDESCRIPTOR_H_

#include "main.h"
#include <MathExtras_Color.h>
#include "EuroRack.h"
#include "MathExtras.h"
#include "MathExtras_DSP.h"

#define MAX_AUGMENTS_PER_INPUT 10

class InputOutputDescriptor {
public:
	int buttonIdx = 0;
	int channelIdx = 0;
	float freq = 0.0f;

	bool enabled = false;
	bool funcCombo = false;//if the input has a func combo.
	bool quickAction = false;//if the input can be adjusted by a button press. (level adjust, tap temp, etc...)


	class AugmentConfig {
	public:
		EuroRack::SignalType signalType = EuroRack::SignalType_CV;
		EuroRack::SignalBehalvior signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
		float defaultVoltVal = 0.0f;
		MathExtras::Color baseColor;
		bool useCustomColorIntensity = false;
		float customColorIntensity = 1.0f;
		bool noQuickAction = false;//no quickAction on this particular augment
		MathExtras::Threshold_Trigger<float>* gateTrigger = nullptr;
		float gateOffIntensity = 0.3f;
	};

	AugmentConfig augments[MAX_AUGMENTS_PER_INPUT];
	int numAugments = 1;
	int curAugment = 0;
	int prefferedAugment = 0;

	inline AugmentConfig& CurAugment(){return augments[curAugment];}

	void AddLink(int descriptorIdx);

	int linkedAugmentsButtons[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
	int numLinks = 0;
	int curLinkIdx = 0;




};

extern InputOutputDescriptor inputOutputDescriptors[8];
#endif /* INC_INPUTOUTPUTDESCRIPTOR_H_ */
