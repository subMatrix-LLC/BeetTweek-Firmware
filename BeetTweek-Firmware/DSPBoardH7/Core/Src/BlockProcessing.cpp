/*
 * BlockProcessing.cpp
 *
 *  Created on: May 11, 2021
 *      Author: casht
 */




#include "BlockProcessing.h"

#include "tim.h"
#include "spi.h"
#include "dma.h"
#include "sai.h"
#include "stdio.h"

#include "DACX0504.h"
#include "Modes/Mode.h"
#include "profiling.h"


extern ModeManager modeManager;


extern "C" {


	void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef* hsai)
	{

		ProcessHalfBlock(0);
	}
	void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef* hsai)
	{
		ProcessHalfBlock(1);
	}

	void ProcessHalfBlock(int bufferSwap)
	{
		if(fullyStartedUp == 0)
			return;
		//htim2.Instance->CNT = 0;//reset metrics counter
		//htim2.Instance->CR1 |= TIM_CR1_CEN_Msk;//start timer

		Mode* currentMode = modeManager.currentMode();
		currentMode->KnobDSPFunction(BLOCKPROCESSINGTIME_S);
		currentMode->knobDspTick++;

		//FastUpdate_KnobProcessTime = htim2.Instance->CNT;
		//FastUpdate_KnobProcessTime

		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);



		// do conversion on ADC
		for(int b = 0; b < 4; b++)
		{
			if(adcPlugStates[b])
			{
				if(b < 2)
				{
					adcIns[b] = ADC_TO_FLOAT(adc0DMA[b%2]);
				}
				else
				{
					adcIns[b] = ADC_TO_FLOAT(adc1DMA[b%2]);
				}

				adcVolts[b] = currentMode->ADCToVolts(b, adcIns[b]);
			}
			else
			{
				float volts = inputOutputDescriptors[b].CurAugment().defaultVoltVal;
				adcIns[b] = currentMode->VoltsToADC(b, volts);
				adcVolts[b] = volts;
			}
		}



		//FastUpdate_ADCFilterAndConversion = htim2.Instance->CNT - FastUpdate_KnobProcessTime;


		//Process
		if(!modeManager.calibrationMode)
		{
			currentMode->PreAudioDSP(BLOCKPROCESSINGTIME_S);
			currentMode->AudioDSPFunction(BLOCKPROCESSINGTIME_S, bufferSwap);
			currentMode->audioTick++;
		}
		else
		{
			channelInterpEnables[0] = 1;
			channelInterpEnables[2] = 1;
			channelInterpEnables[3] = 1;
			channelInterpEnables[4] = 1;
		}

		//FastUpdate_AudioDSP =  htim2.Instance->CNT - FastUpdate_ADCFilterAndConversion;



		float dacInterp[4];
		dacInterp[0] = dacOuts_1[0];
		dacInterp[1] = dacOuts_1[1];
		dacInterp[2] = dacOuts_1[2];
		dacInterp[3] = dacOuts_1[3];

		float dacInterpStepSizes[4];
		float interpDelta = (DACDMABUFFERSIZE/2);

		for(int i = 0; i < 4; i++)
		{
			if(dacInterpJumps[i])
				interpDelta = 1;

			dacInterpStepSizes[i] = ((dacOuts[i]) - (dacOuts_1[i]))/interpDelta;

			dacInterpJumps[i] = false;
		}



		BLOCK_PROCESS_ALL()

			for(int inter = 0; inter < 4; inter++)
			{
				dacInterp[inter] += dacInterpStepSizes[inter];
			}


			if(sampleIdxDMA%2 == 0)
			{
				if(channelInterpEnables[0])
				{
					dac0DMA[sampleIdxDMA] = int32_t(dacInterp[     sampleIdxDMA%2 ]*INT32_MAX) >> 8;
				}
				if(channelInterpEnables[2])
				{
					dac1DMA[sampleIdxDMA] = int32_t(dacInterp[ 2 + sampleIdxDMA%2 ]*INT32_MAX) >> 8;
				}
			}
			else
			{
				if(channelInterpEnables[1])
				{
					dac0DMA[sampleIdxDMA] = int32_t(dacInterp[     sampleIdxDMA%2 ]*INT32_MAX) >> 8;
				}
				if(channelInterpEnables[3])
				{
					dac1DMA[sampleIdxDMA] = int32_t(dacInterp[ 2 + sampleIdxDMA%2 ]*INT32_MAX) >> 8;
				}
			}
			//dac0DMA[sampleIdxDMA] = int32_t(dacInterp[     sampleIdxDMA%2 ]*INT32_MAX) >> 8;
			//dac1DMA[sampleIdxDMA] = int32_t(dacInterp[ 2 + sampleIdxDMA%2 ]*INT32_MAX) >> 8;
			//Pass right through test.
//			dac0DMA[sampleIdxDMA] = -adc0DMA[adcIdx];
//			dac1DMA[sampleIdxDMA] = -adc1DMA[adcIdx];
		}


		dacOuts_1[0]  = dacOuts[0];
		dacOuts_1[1]  = dacOuts[1];
		dacOuts_1[2]  = dacOuts[2];
		dacOuts_1[3]  = dacOuts[3];

		dacVolts_1[0] = dacVolts[0];
		dacVolts_1[1] = dacVolts[1];
		dacVolts_1[2] = dacVolts[2];
		dacVolts_1[3] = dacVolts[3];




		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

		//FastUpdate_DAC = htim2.Instance->CNT - FastUpdate_AudioDSP;

		//see how much time left on timer.
		//FastUpdate_TimerHeadRoom = htim2.Instance->CNT;
		//htim2.Instance->CR1 &= ~TIM_CR1_CEN_Msk;//stop timer

	}



}


