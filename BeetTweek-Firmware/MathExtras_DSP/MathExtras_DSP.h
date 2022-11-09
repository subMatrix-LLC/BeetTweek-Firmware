/*
 * SignalHelpers.h
 *
 *  Created on: Feb 26, 2021
 *      Author: casht
 */

#ifndef INC_MATHEXTRAS_DSP_H_
#define INC_MATHEXTRAS_DSP_H_

#include "MathExtras_Curves.h"

namespace MathExtras {


//some cubic function.
template <class T>
T SoftClip_Cubic(T in, T max)
{
	return max*((3.0/2.0)*(in - (in*in*in)/3));
}



//soft clip where slope at 0 is 1, rolling off to a final slope of 0 at in = 1.  where the rolloff starts at rolloffStart
template <class T>
T SoftClip_Spline(T in, T rolloffStart, T amplitude)
{
	if(MathExtras::Abs(in) > rolloffStart)
	{
		return MathExtras::Sign(in)*CubicSpline_1D(rolloffStart, rolloffStart, 1.0f, amplitude, 1.0f, 0.0f, MathExtras::Abs(in));
	}
	else
	{
		return in;//linear region
	}

}



//Basic Histeresis Helper
//
//Output jumps to the input if the input moves past its last jump value plus or minus a gap amount.
//
//poll jumpedUp and jumpedDown after Process() to see if event happened.
template <class T>
class Movement_Jump
{
public:

	T mHalfGap = 0.01;
	T val_prev;
	bool firstVal = true;
	bool jumpedUp = false;
	bool jumpedDown = false;

	void SetHalfGap(T halfGap)
	{
		mHalfGap = halfGap;
	}

	inline bool JumpedDown()
	{
		return jumpedDown;
	}

	inline bool JumpedUp()
	{
		return jumpedUp;
	}

	inline bool Jumped()
	{
		return (jumpedUp || jumpedDown);
	}

	inline T Process(T in)
	{
		//clear flags
		jumpedUp = false;
		jumpedDown = false;

		if(firstVal)
		{
			val_prev = in;
			firstVal = false;
			return in;
		}

		if((in - mHalfGap) > val_prev)
		{
			val_prev = in;
			jumpedUp = true;
			return val_prev;
		}
		else if((in + mHalfGap) < val_prev)
		{
			val_prev = in;
			jumpedDown = true;
			return val_prev;
		}
		return val_prev;
	}
};




template <class T>
class Threshold_Trigger
{
public:

	bool firstVal = true;
	T thresholdVal = 0;
	T thresholdHalfGap = 0.1;
	T thresholdHigh = 0.1;
	T thresholdLow = -0.1;
	int triggerLevel = 0;
	int triggerLevel_1 = 0;
	bool triggeredDown = false;
	bool triggeredUp = false;
	T in = 0;

	inline bool Triggered()
	{
		return triggeredDown || triggeredUp;
	}

	inline bool TriggeredUp()
	{
		return triggeredUp;
	}

	inline bool TriggeredDown()
	{
		return triggeredDown;
	}
	inline int TriggerLevel()
	{
		return triggerLevel;
	}

	inline void SetThreshold(T threshold)
	{
		thresholdVal = threshold;

		thresholdHigh = thresholdVal + thresholdHalfGap;
		thresholdLow  = thresholdVal - thresholdHalfGap;
	}
	inline void SetThresholdHalfGap(T halfGap)
	{
		thresholdHalfGap = halfGap;

		thresholdHigh = thresholdVal + thresholdHalfGap;
		thresholdLow  = thresholdVal - thresholdHalfGap;
	}

	inline T Process(T in)
	{
		if(firstVal)
		{
			if(in > thresholdVal)
				triggerLevel = 1;
			else
				triggerLevel = 0;

			firstVal = false;
			this->in = in;

			return triggerLevel;
		}

		//clear trigger up/down flags.
		triggeredDown = false;
		triggeredUp   = false;

		triggerLevel_1 = triggerLevel;

		if((triggerLevel_1 == 0) && (in > thresholdHigh) )
		{
			triggeredUp = true;
			triggerLevel = 1;
		}
		else if((triggerLevel_1 == 1) && (in < thresholdLow) )
		{
			triggeredDown = true;
			triggerLevel = 0;
		}


		this->in = in;
		return T(triggerLevel);
	}


};






//Overlapping Trigger Management For region based triggering.
// for NUMREGIONS = 5:
//^^^^
// 4
//----MaxLevel
// 3
//----
// 2
//----
// 1
//----MinLevel
// 0
//vvvv
//
template < typename T, int NUMREGIONS>
class MultiLevel_Threshold_Trigger
{

public:
	//initialize the triggers
	void Initialize(T maxLevel, T minLevel, T halfGap, T minRangeValue = -1000, T maxRangeValue = 1000)
	{
		for(int i = 0; i < NUMREGIONS-1; i++)
		{
			T thresh = (i/float(NUMREGIONS-2))*(maxLevel - minLevel) + minLevel;

			triggerLevels[i].SetThreshold(thresh);
			triggerLevels[i].SetThresholdHalfGap(halfGap);
		}

		this->minRangeValue = minRangeValue;
		this->maxRangeValue = maxRangeValue;
	}
	T Process(T in)
	{
		for(int i = 0; i < NUMREGIONS-1; i++)
		{
			triggerLevels[i].Process(in);
		}
		return in;
	}

	void SetTriggerRegionFlags()
	{
		for(int i = 0; i < NUMREGIONS; i++)
		{
			if(TriggeredOutOfRegion(i))
				triggerOutRegionFlags[i] = true;

			if(TriggeredIntoRegion(i))
				triggerIntoRegionFlags[i] = true;
		}
	}
	inline void ClearTriggerRegionFlags()
	{
		for(int i = 0; i < NUMREGIONS; i++)
		{
			triggerOutRegionFlags[i] = false;
			triggerIntoRegionFlags[i] = false;
		}
	}
	inline bool AnyRegionFlags()
	{
		for(int i = 0; i < NUMREGIONS; i++)
		{
			if(triggerOutRegionFlags[i])
				return true;;
			if(triggerIntoRegionFlags[i])
				return true;
		}
		return false;
	}

	inline int TriggerRegion()
	{
		int sum = 0;
		for(int i = 0; i < NUMREGIONS-1; i++)
		{
			sum += triggerLevels[i].TriggerLevel();
		}

		sum = MathExtras::ClampInclusive(sum, 0, NUMREGIONS-1);
		return sum;
	}








	inline bool TriggeredIntoRegion(int region)
	{
		if(region == 0)
		{
			return (triggerLevels[region].TriggeredDown());
		}
		else if(region == NUMREGIONS-1)
		{
			return (triggerLevels[region-1].TriggeredUp());
		}
		else
			return (triggerLevels[region-1].TriggeredUp() && !triggerLevels[region].TriggerLevel()) || (triggerLevels[region].TriggeredDown() && triggerLevels[region-1].TriggerLevel());
	}





	inline bool TriggeredOutOfRegion(int region)
	{
		if(region == 0)
		{
			return (triggerLevels[region].TriggeredUp());
		}
		else if(region == NUMREGIONS-1)
		{
			return (triggerLevels[region-1].TriggeredDown());
		}
		else
			return (triggerLevels[region-1].TriggeredDown() || (triggerLevels[region].TriggeredUp()));
	}

	inline T RegionThresholdLower(int region)
	{
		if(region == 0)
			return minRangeValue;
		return triggerLevels[region-1].thresholdVal;
	}
	inline T RegionThresholdUpper(int region)
	{
		if(region == NUMREGIONS-1)
			return maxRangeValue;

		return triggerLevels[region].thresholdVal;
	}
	inline T RegionMid(int region)
	{
		return (RegionThresholdLower( region ) + RegionThresholdUpper( region ))*0.5;
	}

	//returns true if the given value is inside the given region.  (does not take into account the trigger halfgaps)
	inline bool ValueInRegion(T Value, int region)
	{
		if(Value < RegionThresholdUpper(region) || Value > RegionThresholdLower(region))
		{
			return true;
		}
		return false;
	}

	Threshold_Trigger<T> triggerLevels[NUMREGIONS-1];

	bool triggerOutRegionFlags[NUMREGIONS]  = {false};
	bool triggerIntoRegionFlags[NUMREGIONS] = {false};
	T minRangeValue=-1000;
	T maxRangeValue=1000;
};







//A single low pass pole filter
template <class T>
class SinglePoleLPFilter
{
public:

	T in_1 = 0.0;
	T val_filtered = 0.0;
	T filterCoeff = 1.0;

	inline T Process(T in, T sampleTime)
	{
		UpdateFilter(in, sampleTime);
		in_1 = in;
		return val_filtered;
	}

	inline void UpdateFilter(T in, T sampleTime)
	{
		val_filtered += (in - val_filtered)*sampleTime*filterCoeff;
	}
};



//A single high pass pole filter
template <class T>
class SinglePoleHPFilter
{
public:

	T in_1 = 0.0;
	T val_filtered = 0.0;
	T val_filtered_1 = 0.0;
	T filterCoeff = 0.1f;

	inline T Process(T in, T sampleTime)
	{
		UpdateFilter(in, sampleTime);
		in_1 = in;
		return val_filtered;
	}

	inline void UpdateFilter(T in, T sampleTime)
	{
		val_filtered += (in - val_filtered)*sampleTime*filterCoeff;

		T a = filterCoeff/(filterCoeff + sampleTime);

		val_filtered = a*(val_filtered_1 + in - in_1);
		val_filtered_1 = val_filtered;
	}
};






//A single pole filter, where, if the signal jumps to the opposite side on range, the filter jumps with it and the jump is sustained until it breaks the gap value dictated by the half gap percent.
template <class T>
class WrapAware_SinglePoleFilter : public SinglePoleLPFilter<T>
{
public:

	T mHalfGapPerc = 0.1;
	T mHighCeil = 1.0;
	T mLowCeil = -1.0;


	//T mMidVal = 0.0;
	T mHighGapVal;
	T mLowGapVal;
	bool mInHappyRange;
	bool mLockOut = false;
	bool mNoFilter = false;
	bool mGapTravel = false;


	inline void SetCeilings(T high, T low)
	{
		mHighCeil = high;
		mLowCeil = low;
		mHighGapVal = mHighCeil - mHalfGapPerc*(mHighCeil - mLowCeil);
		mLowGapVal  = mLowCeil  + mHalfGapPerc*(mHighCeil - mLowCeil);
		//mMidVal = (mHighCeil + mLowCeil)*0.5;
	}


	inline T Process(T in, T sampleTime)
	{
		mInHappyRange = (in < mHighGapVal && in > mLowGapVal);


		if(mInHappyRange)
		{
			mLockOut = false;
			//normal case
			if(!mNoFilter)
				SinglePoleLPFilter<T>::UpdateFilter(in, sampleTime);
			else
				SinglePoleLPFilter<T>::val_filtered = in;
		}
		else
		{
			if(in < mLowGapVal && SinglePoleLPFilter<T>::in_1 > mHighGapVal)
			{
				//jumped down
				if(!mLockOut)
					SinglePoleLPFilter<T>::val_filtered = in;//jump the filter

				mLockOut = true;
			}
			else if(in > mHighGapVal && SinglePoleLPFilter<T>::in_1 < mLowGapVal)
			{
				//jumped up
				if(!mLockOut)
					SinglePoleLPFilter<T>::val_filtered = in;//jump the filter

				mLockOut = true;
			}
			else if(in > mHighGapVal && in < SinglePoleLPFilter<T>::in_1)
			{
				//ok to go down only.
				if(mGapTravel)
				{
					if(!mNoFilter)
						SinglePoleLPFilter<T>::UpdateFilter(in, sampleTime);
					else
						SinglePoleLPFilter<T>::val_filtered = in;
				}

			}
			else if(in < mLowGapVal && in > SinglePoleLPFilter<T>::in_1)
			{
				//ok to go up only.
				if(mGapTravel)
				{
					if(!mNoFilter)
						SinglePoleLPFilter<T>::UpdateFilter(in, sampleTime);
					else
						SinglePoleLPFilter<T>::val_filtered = in;
				}
			}

		}


		SinglePoleLPFilter<T>::in_1 = in;
		return SinglePoleLPFilter<T>::val_filtered;
	}

};


//Calculates a Cycle Time From a Pulse like input. using Threshold_Trigger
template <class T>
class CycleTime_Tracker
{
public:
	T bps = 1.0;
	T bps_filtered = 1.0;
	T bps_filter_k = 1.0;
	T bps_max = 1000;
	T bps_min = 0.1;
	bool use_bps_limits = false;

	T trigTime = 0.0;
	T trigTime_1 = 0.0;




	//Outputs Current BPM Estimate based from previous samples
	inline T Process(T in, T sampleTime)
	{
		trigger.Process(in);


		trigTime_1 = trigTime;
		trigTime += sampleTime;

		//detect the rising edge
		if(trigger.TriggeredUp())
		{
			T deltaTimeSec = trigTime;
			trigTime_1 = trigTime;
			trigTime = 0.0f;


			T bpsCalc = 1.0f/deltaTimeSec;

			if(use_bps_limits && bpsCalc <= bps_max && bpsCalc >= bps_min)//only accept reasonable values..
				bps = bpsCalc;

		}
		bps_filtered += (bps - bps_filtered)*sampleTime*bps_filter_k;
		return bps;
	}




	Threshold_Trigger<T> trigger;
};





//keeps a cycletime from an outside source - "tap-tempo"
template <class T>
class TapTempo : public CycleTime_Tracker<T>
{
public:
	TapTempo() : CycleTime_Tracker<T>()
	{
		CycleTime_Tracker<T>::use_bps_limits = true;
	}
	T t1 = 0;
	T t2 = 0;
	T tNext = 1/CycleTime_Tracker<T>::bps;
	T nextTNext = 1/CycleTime_Tracker<T>::bps;
	T timeaccum = 0;
	int tapped = 0;
	int doATap = 0;
	int tapCount = 0;

	inline T Process(T in, T sampleTime)
	{
		if(doATap )
		{
			CycleTime_Tracker<T>::Process(CycleTime_Tracker<T>::trigger.thresholdVal + CycleTime_Tracker<T>::trigger.thresholdHalfGap*2, sampleTime);
			tapCount++;
		}
		else
			CycleTime_Tracker<T>::Process(in, sampleTime);

		tapped = 0;
		timeaccum += sampleTime;

		if(timeaccum >= tNext && tNext > 0.0)
		{
			timeaccum = 0;
			tNext = nextTNext;
			tapped = 1;
		}

		if(CycleTime_Tracker<T>::trigger.TriggeredUp())
		{

			float newTNext = 1/CycleTime_Tracker<T>::bps;

			//float percChange = MathExtras::Abs(1.0 - newTNext/tNext);
			//if(percChange > 0.01f)
				nextTNext = newTNext;


			if(doATap)
			{
				newTNext = CycleTime_Tracker<T>::trigTime_1;
				tNext =  newTNext;
				nextTNext = newTNext;
				timeaccum = 0;
				doATap = 0;
			}


			tapped = 1;
		}

		return in;
	}
	inline void TapOnNextProcess()
	{
		doATap = 1;
	}

	inline int Tapped()
	{
		return tapped;
	}

	inline int TapCount()
	{
		return tapCount;
	}

	inline void ClearTapCount()
	{
		tapCount = 0;
	}

	inline float PercToNextTap()
	{
		return timeaccum/tNext;
	}
	inline float SamplesInPeriod(float sampleTime)
	{
		return tNext/sampleTime;
	}
};







//non-generic virtual tape handling
template <class T, int SIZE>
class CircularTapeRecordPlayback_Buffer
{
public:

	inline int RecordBufferIdx(float perc)
	{
		return perc*SIZE;
	}


	inline T ReadRecordBufferAtPerc(float perc)
	{
		//TODO get interpolated.
		int idx = RecordBufferIdx(perc);
		T val1 = buffer[idx];
		T val2 = buffer[idx+1];
		return (val1 + val2)*float(0.5f);
	}

	inline void JumpToIdx(int idx)
	{
		recordIdx = idx;
	}

	inline void WriteAndAdvanceBy(T val, int sampleAdvance)
	{
		buffer[recordIdx] = val;
		AdvanceBy(sampleAdvance);
	}
	inline T ReadAndAdvanceBy(int sampleAdvance)
	{
		T readVal = buffer[recordIdx];
		AdvanceBy(sampleAdvance);
		return readVal;
	}
	inline T Read()
	{
		return buffer[recordIdx];
	}

	inline void AdvanceBy(int samples)
	{
		recordIdx   += samples;
		recordIdx_1 += samples;

		recordIdx = MathExtras::WrapMax(recordIdx, SIZE);
		recordIdx_1 = MathExtras::WrapMax(recordIdx_1, SIZE);
	}


	inline void WriteRecordBufferAtPerc(float perc, T val)
	{
		//TODO get interpolated.
		int idx = RecordBufferIdx(perc);
		buffer[RecordBufferIdx(perc)] = val;
		recordIdx_1 = idx;
	}

	inline void WriteFromLastToPerc(float perc, T val)
	{
		int idx = RecordBufferIdx(perc);

		//write values on missed values as well

		//determine direction to fill in values
		int dir = MathExtras::Sign<int>(idx - recordIdx_1);


		//correct for wrap around giving wrong dir.
		if(MathExtras::Abs(idx - recordIdx_1) > SIZE/2)
		{
			dir = -dir;
		}

		int curIdx = recordIdx_1;
		T lastRecordVal = buffer[recordIdx_1];
		while(curIdx != idx)//THIS is slow and non-deterministic..
		{
			curIdx += dir;
			if(curIdx >= SIZE)
				curIdx = 0;
			else if(curIdx < 0)
				curIdx = SIZE-1;

			//volatile float interp = MathExtras::Abs(float(curIdx - idx)/(idx - recordIdx_1));

			buffer[curIdx] = val;//*interp + lastRecordVal*(1.0f-interp);
		}

		recordIdx_1 = idx;
		recordIdx = idx + dir;

		recordIdx = MathExtras::WrapMax(recordIdx, SIZE);
		recordIdx_1 = MathExtras::WrapMax(recordIdx_1, SIZE);
	}




	inline void WriteFromLastToPerc(float perc, T* val, int size)
	{
		int idx = RecordBufferIdx(perc);

		//write values on missed values as well

		//determine direction to fill in values
		int dir = MathExtras::Sign<int>(idx - recordIdx_1);


		//correct for wrap around giving wrong dir.
		if(MathExtras::Abs(idx - recordIdx_1) > SIZE/2)
		{
			dir = -dir;
		}

		int curIdx = recordIdx_1;
		T lastRecordVal = buffer[recordIdx_1];


		while(curIdx != idx)//THIS is slow and non-deterministic..
		{
			curIdx += dir;
			if(curIdx >= SIZE)
				curIdx = 0;
			else if(curIdx < 0)
				curIdx = SIZE-1;

			buffer[curIdx] = val[int((fabs((curIdx-idx)/float(idx - recordIdx_1)))*size*4)];
		}

		recordIdx_1 = idx;
		recordIdx = idx + dir;

		recordIdx = MathExtras::WrapMax(recordIdx, SIZE);
		recordIdx_1 = MathExtras::WrapMax(recordIdx_1, SIZE);
	}


	inline void SetAll(T val)
	{
		for(int i = 0 ; i < SIZE; i++)
			buffer[i] = val;
	}

	T buffer[SIZE] = {0};
	int recordIdx_1 = 0;
	int recordIdx = 0;
};




//non-generic virtual tape handling
template <class T, class U, int SIZE>
class CircularTapeRecordPlayback_Buffer2
{
public:
	CircularTapeRecordPlayback_Buffer2()
	{
		for(int i = 0 ; i < SIZE; i++)
		{
			sampleBuffer[i] = 0.0f;
		}
		smp_filteredOut = 0.0f;
	}

	inline T Inprint(T sample, T sample_1, U wrapPerc, U wrapPerc_1, int dir, T sampleMix, T keepMix, T clipThresh)
	{
		int i_1 =  int((wrapPerc_1)*SIZE);
		int i = int((wrapPerc)*SIZE);
		int diff = i - i_1;

//		//correct the diff
		if(diff != 0 && Sign(diff) != dir)
			diff -= dir*SIZE;


		if(abs(diff) > 10){
			i_1 = WrapOnceMax(i - dir*10,SIZE);
			diff = 10*dir;
		}

		int j = i_1;


		smp_filteredIn = sample_1;
		T stride = sample - smp_filteredIn;
		while(j!=i)
		{
			j = WrapOnceMax(j+dir,SIZE);

			smp_filteredIn += stride/diff;

			//int jwrapped = WrapOnceMax(j,SIZE);
			sampleBuffer[j] = smp_filteredIn*sampleMix + sampleBuffer[j]*keepMix;
			//sampleBuffer[j] = MathExtras::SoftClip_Cubic_2(T(sampleBuffer[j]), clipThresh);

			sampleBuffer[j] = MathExtras::SoftClip_Spline<T>((sampleBuffer[j]), (0.8f), (0.8f));

		}
		return sampleBuffer[j];

	}

	inline T ReadRecordBufferAtPerc(U WrapPerc, U WrapPerc_1, int dir)
	{
		int i = int((WrapPerc)*SIZE);
		T smp = sampleBuffer[i];
//		int cnt = 0;
//		while(smp == 0 && cnt < 2)
//		{
//			i -= dir;
//			smp = sampleBuffer[i];
//			cnt++;
//		}
//

		smp_filteredOut += (smp - smp_filteredOut)*0.8f;
		smp_filteredOut = MathExtras::ClampInclusive(smp_filteredOut, -1.0f, 1.0f);
		//else
		//	sampleBuffer[i] = smp_filteredOut;
		//smp_1 = smp;
		//return smp_filteredOut;
		return smp_filteredOut;
	}


	int dir = 1;
	T sampleBuffer[SIZE] = {0};
	//short flag[SIZE] = {0};
	int headIdx_1 = 0;
	int headIdx = 0;
	T smp_1 = 0;
	T smp_filteredOut = 0;
	T smp_filteredIn = 0;
};




//non-generic virtual tape handling
template <class T, class U, int SIZE>
class CircularTapeRecordPlayback_Buffer4
{
public:
	CircularTapeRecordPlayback_Buffer4()
	{
		for(int i = 0 ; i < SIZE; i++)
		{
			sampleBuffer[i][0] = 0;
			sampleBuffer[i][1] = float(i)/SIZE;
		}
	}

	int FindSpot(U wrapPerc, int dir, int startIdx = 0)
	{
		//find the right spot.
		int idx = 0;
		if(dir > 0)
		{
			while(sampleBuffer[idx][1] < wrapPerc)
			{
				idx = MathExtras::WrapOnceMax(idx+dir, SIZE);
			}
		}
		else if(dir < 0)
		{
			while(sampleBuffer[idx][1] > wrapPerc)
			{
				idx = MathExtras::WrapOnceMax(idx+dir, SIZE);
			}
		}
		return idx;
	}

	inline T Sample(T sample, T sample_1, U wrapPerc, U wrapPerc_1, int dir)
	{
		headIdx = FindSpot(wrapPerc, dir, headIdx);
		headIdx_1 = FindSpot(wrapPerc_1, dir, headIdx);

		sampleBuffer[headIdx][0] = sample;
		sampleBuffer[headIdx][1] = wrapPerc;

		headIdx_1 = headIdx;
		headIdx = MathExtras::WrapOnceMax(headIdx+dir, SIZE);

		return sample;
	}


	inline T ReadRecordBufferAtPerc(U WrapPerc, U WrapPerc_1, int dir)
	{
		FindSpot(WrapPerc, dir);

		//interpolate
		U p = (WrapPerc - sampleBuffer[headIdx_1][1])/(sampleBuffer[headIdx][1] - sampleBuffer[headIdx_1][1]);
		return sampleBuffer[headIdx][0]*(p) + sampleBuffer[headIdx_1][0]*(1-p);
	}


	int dir = 1;
	T sampleBuffer[SIZE][2] = {0};
	//short flag[SIZE] = {0};
	int headIdx_1 = 0;
	int headIdx = 0;
	T smp_1 = 0;
	T smp_filteredOut = 0;
	T smp_filteredIn = 0;
	int firstSMP = 1;
};



















//
////non-generic virtual tape handling
//template <class T, class U, int SIZE>
//class CircularTapeRecordPlayback_Buffer3
//{
//public:
//	CircularTapeRecordPlayback_Buffer3()
//	{
//		for(int i = 0 ; i < SIZE; i++)
//		{
//			sampleBuffer[i] = 0;
//		}
//	}
//
//	inline T Sample(T sample, T sample_1, U wrapPerc, U wrapPerc_1, int dir)
//	{
//		int i_1 =  int((wrapPerc_1)*SIZE);
//		int i = int((wrapPerc)*SIZE);
//		int diff = i - i_1;
//
////		//correct the diff
//		if(diff != 0 && Sign(diff) != dir)
//			diff -= dir*SIZE;
//
//
//		if(abs(diff) > 100){
//			i_1 = WrapOnceMax(i - dir*100,SIZE);
//			diff = 100*dir;
//		}
//
//		headIdx_1 = headIdx;
//		headIdx = i;
//
//
//		sampleBuffer[i] = smp_filteredIn;
//
//
//		if(dir > 0)
//		{
//			nextSmpIdx[headIdx_1] = i;
//			prevSmpIdx[headIdx] = headIdx_1;
//		}
//		else if(dir < 0)
//		{
//			prevSmpIdx[headIdx_1] = i;
//			nextSmpIdx[headIdx] = headIdx_1;
//		}
//
//
//
//
//
//
//
//		return sample;
//
//	}
//
//	inline T ReadRecordBufferAtPerc(U WrapPerc, U WrapPerc_1, int dir)
//	{
//		int i = int((WrapPerc)*SIZE);
//		T smp = sampleBuffer[i];
//
//		int j = i;
//
//		for(;j!=i;j += dir)
//		{
//
//		}
//
//
//
////		int cnt = 0;
////		while(smp == 0 && cnt < 2)
////		{
////			i -= dir;
////			smp = sampleBuffer[i];
////			cnt++;
////		}
////
//		if(abs(smp) > 10000)
//			smp_filteredOut += (smp - smp_filteredOut)*0.8f;
//		//else
//		//	sampleBuffer[i] = smp_filteredOut;
//		//smp_1 = smp;
//		//return smp_filteredOut;
//		return smp_filteredOut;
//	}
//
//
//	//incremental update routine (patch missing buffer Indexes for faster reads)
//	inline void UpdateBufferIndexes(int numPatches)
//	{
//
//	}
//
//	int dir = 1;
//	T sampleBuffer[SIZE] = {0};
//	int nextSmpIdx[SIZE] = {0};
//	int prevSmpIdx[SIZE] = {0};
//	//short flag[SIZE] = {0};
//	int headIdx_1 = 0;
//	int headIdx = 0;
//	T smp_1 = 0;
//	T smp_filteredOut = 0;
//	T smp_filteredIn = 0;
//	int firstSMP = 1;
//};







//Tracks step like signals, without a need for a sample trigger.  outputs the latest stable step value.
template <class T>
class StepTracker
{
public:
	StepTracker<T>()
	{
		SetRateThresh(rateThresh);
	}
	bool firstVal = true;
	T in_1 = 0.0;
	T delta = 0.0;

	T curStepValue = 0.0;      //value updated after rate has come into threshold range.
	T snapShotStepValue = 0.0; //value taken after latch delay after rate has come into threshold range.
	bool snapped = false;
	bool snapshotted = false;
	T countDownTime = 0;
	T latchDelay = 0.1;

	inline T SnapShotValue();

	inline bool SnapShotted(){
		return snapshotted;
	}

	inline void SetRateThresh(T thresh)
	{
		rateThresh = thresh;
	}
	inline T Process(T in, T sampleTime)
	{
		if(firstVal)
		{
			firstVal = false;
			in_1 = in;
			return in;
		}
		snapshotted = false;//clear pollable flags

		delta = in - in_1;
		in_1 = in;


		T deltaFiltered = rateFilter.Process(delta, sampleTime);
		T deltaFilteredAbs = MathExtras::Abs(deltaFiltered);

		latchDelay = 1.0f/rateFilter.filterCoeff;

		if(deltaFilteredAbs < rateThresh)
		{
			if(countDownTime < latchDelay)
			{
				curStepValue = in;
				countDownTime += sampleTime;
			}
			else
			{
				if(snapped == false)
				{
					snapShotStepValue = in;
					snapped = true;
					snapshotted = true;
				}
			}
		}
		else
		{
			countDownTime = 0;
			snapped = false;
		}

		return snapShotStepValue;
	}

	SinglePoleLPFilter<T> rateFilter;
private:
	T rateThresh = 0.01;
};


//Buffer where any set writes that occur interpolate from the last write to the current write, going in the forward direction.
//if looping is enabled - if idx is less than the previous idx, values are still interpolated, looping around the buffer.
template <class T, int SIZE>
class ForwardInterpBuffer
{
public:
	T buffer[SIZE] = {0};
	int lastIdx = 0;
	T lastVal = 0;
	bool firstSet = true;
	bool looping = false;


	inline void Set(int idx, T value)
	{
		buffer[idx] = value;
		if(firstSet)
		{
			lastVal = value;
			lastIdx = idx;
			firstSet = false;
			return;
		}

		int span = idx - lastIdx;
		if(looping && span < 0)
			span += SIZE;


		if(span > 0)
		{


			T step = (value - lastVal)/(span);

			int it = lastIdx+1;
			T v = lastVal + step;
			while(it!=idx)
			{
				buffer[it] = v;
				v += step;
				it++;
				if(it >= SIZE)
					it = 0;
			}
		}

		lastVal = value;
		lastIdx = idx;
	}
	inline T Get(int idx)
	{
		return buffer[idx];
	}

};



//Store SIZE y values, in SIZE cells.  Each y value has associated xValue.
//xValues are in domain [0-1]
//Retrieve interpolated y values by x value.
template <class T, int SIZE>
class GeometricInterpolationBuffer
{
public:
	T xValues[SIZE] = {0};
	T yValues[SIZE] = {0};


	inline void Reset(T yValue = T(0.0))
	{
		for(int i = 0; i < SIZE; i++)
		{
			xValues[i] = T(i)/(SIZE);
			yValues[i] = yValue;
		}
	}

	//xValue is percent from 0 to 1, returns cellindx
	inline int Set(T yValue, T xValue)
	{
		xValue = MathExtras::ClampInclusive<T>(xValue, T(0.0), T(1.0));
		int idx = MathExtras::ClampInclusive<T>(xValue*(SIZE),0, SIZE-1);

		xValues[idx] = xValue;
		yValues[idx] = yValue;
		return idx;
	}


	//set an xValue,yValue, re-writing the values in cells after idx_1 up to the given value,
	// based on linear interpolation values are always written in a forward manner.
	// idx_1 must be no more than SIZE/2 behind the new idx.
	inline int SetForwardInterp(T yValue, T xValue, int idx_1)
	{
		T xValue_1 = xValues[idx_1];
		T yValue_1 = yValues[idx_1];


		xValue = MathExtras::ClampInclusive<T>(xValue, T(0.0), T(1.0));
		int idx = MathExtras::ClampInclusive<T>(xValue*(SIZE), 0, SIZE-1);

		if(MathExtras::IsLikelyForwardIdxDelta(idx, idx_1, SIZE))
		{
			T xForwardDist = xValue - xValue_1;
			if(xForwardDist < 0)
				xForwardDist += 1.0;


			for(int i = MathExtras::WrapMax(idx_1+1,SIZE); i != idx; i = MathExtras::WrapMax(i+1, SIZE))
			{
				T xVal = T(i)/SIZE;
				T xValMid = xVal + 0.5/SIZE;

				T distIn = (xValMid - xValue_1);
				if(distIn < 0)
					 distIn += 1.0;

				T percIn = distIn/xForwardDist;

				T yValMid = yValue_1 + percIn*(yValue - yValue_1);

				xValues[i] = xValMid;
				yValues[i] = yValMid;

				if(yValMid >10.0f || yValMid < -10.0f)
				{
					int error = 2;
					error++;
				}
			}
		}

		xValues[idx] = xValue;
		yValues[idx] = yValue;

		if(yValue >10.0f || yValue < -10.0f)
		{
			int error = 2;
			error++;
		}

		return idx;
	}


	inline void Get(int idx, T* xValue, T* yValue)
	{
		*xValue = xValues[idx];
		*yValue = yValues[idx];
	}

	//get interpolated y value wrapping around buffer if needed.
	inline T GetInterpolated(T xValue)
	{
		xValue = MathExtras::ClampInclusive<T>(xValue, 0.0, 1.0);
		int cellIdx = xValue*(SIZE-1);

		if(xValue >= xValues[cellIdx])
		{
			int idx_plus_1 = MathExtras::WrapMinMax(cellIdx+1, 0, SIZE);

			//interp from cellIdx to cellIdx+1
			T xPercIn = (xValue - xValues[cellIdx])/(xValues[idx_plus_1] - xValues[cellIdx]);
			T yValue = yValues[cellIdx] + xPercIn*(yValues[idx_plus_1] - yValues[cellIdx]);
			return yValue;
		}
		else//lessthan
		{
			int idx_minus_1 = MathExtras::WrapMinMax(cellIdx-1, 0, SIZE);
			//interp from cellIdx-1 to cellIdx
			T xPercIn = (xValue - xValues[idx_minus_1])/(xValues[cellIdx] - xValues[idx_minus_1]);
			T yValue = yValues[idx_minus_1] + xPercIn*(yValues[cellIdx] - yValues[idx_minus_1]);
			return yValue;
		}
	}

};



template <class T>
class PID {
public:
	int outputClampHigh = 0;
	int outputClampLow  = 0;
	T outputClampHighLevel = 1.0f;
	T outputClampLowLevel = -1.0f;

	T errorPadding= 0.0f;

	T I = 0.0f;
	T IGain = 0.000001f;

	int IClampHigh= 0;
	int IClampLow = 0;

	T IClampHighLevel = 0.0f;
	T IClampLowLevel = 0.0f;


	float PGain = 0.01f;
	inline T PIControl(T target, T measured)
	{
		T error = target - measured;

		//make error seem less using errorPadding.
		if(error > 0.0f)
		{
			error += -errorPadding;
		}
		else
		{
			error += errorPadding;
		}

		T P = PGain * error;

		//Update PIDState
		I += (error * IGain);

		//clamp I term if enabled
		if(IClampHigh)
		{
			if(I > IClampHighLevel)
				I = IClampHighLevel;
		}
		if(IClampLow)
		{
			if(I < IClampLowLevel)
				I = IClampLowLevel;
		}


		T output = (P + I);

		//clamp output if enabled.
		if(outputClampHigh)
		{
			if(output > outputClampHighLevel)
				output = outputClampHighLevel;

		}
		if(outputClampLow)
		{
			if(output < outputClampLowLevel)
				output = outputClampLowLevel;

		}

		return output;
	}
};



template <class T, int SIZE>
class DelayLine {
public:
	int headIdx = 0;
	void Process(T in)
	{
		buffer[headIdx] = in;
		headIdx = MathExtras::WrapOnceMax(headIdx, SIZE);
	}
	T Tap(int sampleOffset)
	{
		return buffer[MathExtras::WrapOnceMax(headIdx - sampleOffset, SIZE)];
	}
	T TapFloat(float sampleOffset)
	{
		T t1 = Tap(int(sampleOffset));
		T t2 = Tap(MathExtras::WrapOnceMax(int(sampleOffset) + 1,SIZE));

		T fractional = MathExtras::Fract(sampleOffset);
		T interp = t1*(1.0f - fractional) + t2*(fractional);

		return interp;
	}


	T buffer[SIZE] = {0};

};



}



#endif /* INC_MATHEXTRAS_DSP_H_ */
