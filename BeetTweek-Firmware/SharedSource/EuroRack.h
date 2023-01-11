/*
 * EuroRack.h
 *
 *  Created on: Dec 23, 2020
 *      Author: casht
 */

#ifndef EURORACK_H_
#define EURORACK_H_


#define TWELTH_ROOT_OF_TWO 1.059463094359f



#define ER_NOTE_FREQ_C_0 16.35
#define ER_NOTE_FREQ_CSharp_0 17.32
#define ER_NOTE_FREQ_D_0 18.35
#define ER_NOTE_FREQ_DSharp_0 19.45
#define ER_NOTE_FREQ_E_0 20.6
#define ER_NOTE_FREQ_F_0 21.83
#define ER_NOTE_FREQ_FSharp_0 23.12
#define ER_NOTE_FREQ_G_0 24.5
#define ER_NOTE_FREQ_GSharp_0 25.96
#define ER_NOTE_FREQ_A_0 27.5
#define ER_NOTE_FREQ_ASharp_0 29.14
#define ER_NOTE_FREQ_B_0 30.87
#define ER_NOTE_FREQ_C_1 32.7
#define ER_NOTE_FREQ_CSharp_1 34.65
#define ER_NOTE_FREQ_D_1 36.71
#define ER_NOTE_FREQ_DSharp_1 38.89
#define ER_NOTE_FREQ_E_1 41.2
#define ER_NOTE_FREQ_F_1 43.65
#define ER_NOTE_FREQ_FSharp_1 46.25
#define ER_NOTE_FREQ_G_1 49
#define ER_NOTE_FREQ_GSharp_1 51.91
#define ER_NOTE_FREQ_A_1 55
#define ER_NOTE_FREQ_ASharp_1 58.27
#define ER_NOTE_FREQ_B_1 61.74
#define ER_NOTE_FREQ_C_2 65.41
#define ER_NOTE_FREQ_CSharp_2 69.3
#define ER_NOTE_FREQ_D_2 73.42
#define ER_NOTE_FREQ_DSharp_2 77.78
#define ER_NOTE_FREQ_E_2 82.41
#define ER_NOTE_FREQ_F_2 87.31
#define ER_NOTE_FREQ_FSharp_2 92.5
#define ER_NOTE_FREQ_G_2 98
#define ER_NOTE_FREQ_GSharp_2 103.83
#define ER_NOTE_FREQ_A_2 110
#define ER_NOTE_FREQ_ASharp_2 116.54
#define ER_NOTE_FREQ_B_2 123.47
#define ER_NOTE_FREQ_C_3 130.81
#define ER_NOTE_FREQ_CSharp_3 138.59
#define ER_NOTE_FREQ_D_3 146.83
#define ER_NOTE_FREQ_DSharp_3 155.56
#define ER_NOTE_FREQ_E_3 164.81
#define ER_NOTE_FREQ_F_3 174.61
#define ER_NOTE_FREQ_FSharp_3 185
#define ER_NOTE_FREQ_G_3 196
#define ER_NOTE_FREQ_GSharp_3 207.65
#define ER_NOTE_FREQ_A_3 220
#define ER_NOTE_FREQ_ASharp_3 233.08
#define ER_NOTE_FREQ_B_3 246.94
#define ER_NOTE_FREQ_C_4 261.63
#define ER_NOTE_FREQ_CSharp_4 277.18
#define ER_NOTE_FREQ_D_4 293.66
#define ER_NOTE_FREQ_DSharp_4 311.13
#define ER_NOTE_FREQ_E_4 329.63
#define ER_NOTE_FREQ_F_4 349.23
#define ER_NOTE_FREQ_FSharp_4 369.99
#define ER_NOTE_FREQ_G_4 392
#define ER_NOTE_FREQ_GSharp_4 415.3
#define ER_NOTE_FREQ_A_4 440
#define ER_NOTE_FREQ_ASharp_4 466.16
#define ER_NOTE_FREQ_B_4 493.88
#define ER_NOTE_FREQ_C_5 523.25
#define ER_NOTE_FREQ_CSharp_5 554.37
#define ER_NOTE_FREQ_D_5 587.33
#define ER_NOTE_FREQ_DSharp_5 622.25
#define ER_NOTE_FREQ_E_5 659.25
#define ER_NOTE_FREQ_F_5 698.46
#define ER_NOTE_FREQ_FSharp_5 739.99
#define ER_NOTE_FREQ_G_5 783.99
#define ER_NOTE_FREQ_GSharp_5 830.61
#define ER_NOTE_FREQ_A_5 880
#define ER_NOTE_FREQ_ASharp_5 932.33
#define ER_NOTE_FREQ_B_5 987.77
#define ER_NOTE_FREQ_C_6 1046.5
#define ER_NOTE_FREQ_CSharp_6 1108.73
#define ER_NOTE_FREQ_D_6 1174.66
#define ER_NOTE_FREQ_DSharp_6 1244.51
#define ER_NOTE_FREQ_E_6 1318.51
#define ER_NOTE_FREQ_F_6 1396.91
#define ER_NOTE_FREQ_FSharp_6 1479.98
#define ER_NOTE_FREQ_G_6 1567.98
#define ER_NOTE_FREQ_GSharp_6 1661.22
#define ER_NOTE_FREQ_A_6 1760
#define ER_NOTE_FREQ_ASharp_6 1864.66
#define ER_NOTE_FREQ_B_6 1975.53
#define ER_NOTE_FREQ_C_7 2093
#define ER_NOTE_FREQ_CSharp_7 2217.46
#define ER_NOTE_FREQ_D_7 2349.32
#define ER_NOTE_FREQ_DSharp_7 2489.02
#define ER_NOTE_FREQ_E_7 2637.02
#define ER_NOTE_FREQ_F_7 2793.83
#define ER_NOTE_FREQ_FSharp_7 2959.96
#define ER_NOTE_FREQ_G_7 3135.96
#define ER_NOTE_FREQ_GSharp_7 3322.44
#define ER_NOTE_FREQ_A_7 3520
#define ER_NOTE_FREQ_ASharp_7 3729.31
#define ER_NOTE_FREQ_B_7 3951.07
#define ER_NOTE_FREQ_C_8 4186.01
#define ER_NOTE_FREQ_CSharp_8 4434.92
#define ER_NOTE_FREQ_D_8 4698.63
#define ER_NOTE_FREQ_DSharp_8 4978.03
#define ER_NOTE_FREQ_E_8 5274.04
#define ER_NOTE_FREQ_F_8 5587.65
#define ER_NOTE_FREQ_FSharp_8 5919.91
#define ER_NOTE_FREQ_G_8 6271.93
#define ER_NOTE_FREQ_GSharp_8 6644.88
#define ER_NOTE_FREQ_A_8 7040
#define ER_NOTE_FREQ_ASharp_8 7458.62
#define ER_NOTE_FREQ_B_8 7902.13



//"High Note" for "Audio" range Uses
#define ER_AUDIO_HIGHNOTE ER_NOTE_FREQ_C_6
#define ER_AUDIO_LOWNOTE ER_NOTE_FREQ_C_4


#define ER_VOLTAGE_OUT_MAX 8
#define ER_VOLTAGE_OUT_MIN (-8)
#define ER_VOLTAGE_IN_MAX ER_VOLTAGE_OUT_MAX
#define ER_VOLTAGE_IN_MIN ER_VOLTAGE_OUT_MIN

#define ER_CV_VOLTAGE_MAX (2.5f)
#define ER_CV_VOLTAGE_MIN (-2.5f)

#define ER_SS_VOLTAGE_MAX (ER_CV_VOLTAGE_MAX*0.1)
#define ER_SS_VOLTAGE_MIN (ER_CV_VOLTAGE_MIN*0.1)


#define ER_GATE_THRESH_VOLT (2.5f)
#define ER_GATE_HALF_HIST_VOLT (5.0f*0.05f)


namespace EuroRack
{

enum SignalType {
	SignalType_SMSIGNAL_1=0, //+- 0.25v modulations (CV*0.1)
	SignalType_LRGSIGNAL,	  //+-8v
	SignalType_CV, //+-2.5v modulations
	SignalType_GATE,      //0-5V Signal
	SignalType_Audio,    //+-5v Audio (Possibly No DC Component)
	SignalType_VPerOct	  //+-8v Volt Per Octave
};



enum SignalBehalvior {
	SignalBehalvior_NormalLinear = 0,
	SignalBehalvior_NormalLinear_Inverted
};

inline float SignalVoltMax(SignalType signalType)
{
	switch (signalType)
	{

		case SignalType_SMSIGNAL_1: return ER_CV_VOLTAGE_MAX*0.1f;
		case SignalType_LRGSIGNAL: return 8.0f;
		case SignalType_CV: return ER_CV_VOLTAGE_MAX;
		case SignalType_GATE: return 5.0f;
		case SignalType_Audio: return 2.5f;
		case SignalType_VPerOct: return 8.0f;
		default: return -1.0f;
	}
}
inline float SignalVoltMin(SignalType signalType)
{
	switch (signalType)
	{
		case SignalType_SMSIGNAL_1: return ER_CV_VOLTAGE_MIN*0.1f;
		case SignalType_LRGSIGNAL: return -8.0f;
		case SignalType_CV: return ER_CV_VOLTAGE_MIN;
		case SignalType_GATE: return 0.0f;
		case SignalType_Audio: return -2.5f;
		case SignalType_VPerOct: return -8.0f;
		default: return -1.0f;
	}
}

inline float SignalVoltMid(SignalType signalType)
{
	return (SignalVoltMax(signalType) + SignalVoltMin(signalType))*0.5f;
}

inline float SignalVoltRange(SignalType signalType)
{
	return SignalVoltMax(signalType) - SignalVoltMin(signalType);
}

enum ChordType
{
	ChordType_Majortriad = 0,
	ChordType_MajorSixth,
	ChordType_DominantSeventh,
	ChordType_MajorSenventh,
	ChordType_AugmentedTriad,
	ChordType_AugmentedSeventh,
	ChordType_MinorTriad,
	ChordType_MinorSixth,
	ChordType_MinorSeventh,
	ChordType_MinorMajorSeventh,
	ChordType_DiminishedTriad,
	ChordType_DiminishedSeventh,
	ChordType_HalfDiminishedSeventh,

	ChordType_NUMCHORDS
};


const int ChordOffsets [ChordType_NUMCHORDS][4] = {
		{0,4,7,0},
		{0,4,7,9},
		{0,4,7,10},
		{0,4,7,11},
		{0,4,8,0},
		{0,4,8,10},
		{0,3,7,0},
		{0,3,7,9},
		{0,3,7,10},
		{0,3,7,11},
		{0,3,6,0},
		{0,3,6,9},
		{0,3,6,10}
};


//time to hold gates open in seconds.
const float ClockGateTime = 0.01f;


inline int CHORD_INVERSION(int inversion, int halfStepOffset)
{
	if(halfStepOffset < MathExtras::Abs(inversion))
		halfStepOffset += 12*(inversion/4 + 1);

	return halfStepOffset;
}





//Convert A halfstep offset to frequency using equation.  defaults to A_4 as baseFreq.
//inline float HALFSTEPS_TO_FREQ(int halfstepOffset, float baseFreq = ER_NOTE_FREQ_A_4)
//{
//	float aToTheN = TWELTH_ROOT_OF_TWO;
//	if(halfstepOffset > 0)
//	{
//		for(int i = 0; i < halfstepOffset; i++)
//		{
//			aToTheN *= aToTheN;
//		}
//	}
//	else if(halfstepOffset < 0)
//	{
//		for(int i = 0; i > halfstepOffset; i--)
//		{
//			aToTheN *= aToTheN;
//		}
//		aToTheN = 1/aToTheN;
//	}
//	return baseFreq*aToTheN;
//}


//Convert A halfstep offset to frequency using equation.  defaults to A_4 as baseFreq.
inline float HALFSTEPS_TO_FREQ(float halfstepOffset, float baseFreq = ER_NOTE_FREQ_A_4)
{
	return baseFreq*pow(TWELTH_ROOT_OF_TWO, halfstepOffset);
}

inline float HALFSTEPS_TO_FREQ_FACTOR(float halfstepOffset)
{
	return pow(TWELTH_ROOT_OF_TWO, halfstepOffset);
}

//convert a freq direct to a color hue.
inline float FREQ_TO_HUE(float freq){
	return (freq - ER_AUDIO_LOWNOTE)/(ER_AUDIO_HIGHNOTE - ER_AUDIO_LOWNOTE);
}

//convert a freq to a color hue offset. freq of 0 will return 0,
inline float FREQ_TO_HUE_OFFSET(float freq){
	return (freq)/(ER_AUDIO_HIGHNOTE - ER_AUDIO_LOWNOTE);
}




//convert a note to a color hue.
inline float NOTE_TO_HUE(int halfstepOffset, float baseFreq = ER_NOTE_FREQ_A_4)
{
	float freq = HALFSTEPS_TO_FREQ(halfstepOffset, baseFreq);
	return FREQ_TO_HUE(freq);
}

//V/OCT
inline float VOCT_TO_FREQ(float volts, float baseFreq = ER_NOTE_FREQ_A_4)
{
	return HALFSTEPS_TO_FREQ(volts*12.0f,baseFreq);
}


inline float FREQ_TO_VOCT(float freq, float baseFreq = ER_NOTE_FREQ_A_4)
{
	return (12*log2((freq/baseFreq)))/12.0f;
}


inline float VOCT_TO_HUE(float volts)
{
	return volts;//because 1 v is 1 octave which is one hue rotation....
	//return FREQ_TO_HUE(VOLTS_TO_FREQ(volts));
}


inline float CHORD_OFFSET_TO_FREQ_OFFSET(ChordType type, int chordSemi, int inversion)
{
	int halfStepOffset = CHORD_INVERSION(inversion, ChordOffsets[static_cast<int>(type)][chordSemi]);
	return HALFSTEPS_TO_FREQ_FACTOR(halfStepOffset);
}

inline float CHORD_OFFSET_TO_VOCT(ChordType type, int chordSemi, int inversion, float baseFreq )
{
	return FREQ_TO_VOCT(CHORD_OFFSET_TO_FREQ_OFFSET(type, chordSemi, inversion)*baseFreq);
}

inline float CHORD_TO_FREQ(ChordType type, int chordSemi, int inversion, float baseFreq )
{
	return CHORD_OFFSET_TO_FREQ_OFFSET(type, chordSemi, inversion)*baseFreq;
}
inline float CHORD_TO_VOCT(ChordType type, int chordSemi, int inversion, float baseFreq )
{
	return FREQ_TO_VOCT(CHORD_TO_FREQ(type, chordSemi, inversion, baseFreq));
}






}


#endif /* EURORACK_H_ */
