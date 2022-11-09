
#include "Mode.h"
#include "MathExtras_DSP.h"


#define MODE_TORQUECURVE_BUFFERSIZE 1024
#define MODE_TORQUECURVE_DENSEZEROCROSSING_CNT 10
class Mode_TorqueCurve : public Mode {
public:
	Mode_TorqueCurve()
	{
		SetBothNames("Mode_TorqueCurve");

	}
	virtual ~Mode_TorqueCurve(){}

	//do setup things when the mode is entered
	virtual void Initialize();

	//un-setup thing in prep for next mode
	virtual void UnInitialize();

	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);

	//Called Very Fast.  Holds DSP Code
	virtual void AudioDSPFunction(float sampleTime, int bufferSwap);

	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);


	//call to tell the mode to handle the button down.
	virtual void ButtonDown(int button);

	//call to tell the mode to handle the button up.
	virtual void ButtonUp(int button);

	virtual void OnFuncCombo(int button);

	//Prints Debug Information
	virtual void DebugPrint();

	virtual void OnPostCycleAugment(int buttonIdx, int augmentIdx);

	virtual bool WriteEEPROMState(uint32_t &ee_address);

	virtual bool ReadEEPROMState(uint32_t &ee_address);



	//MathExtras::ForwardInterpBuffer<float,MODE_TORQUECURVE_BUFFERSIZE> buffer1;
	MathExtras::GeometricInterpolationBuffer<float, MODE_TORQUECURVE_BUFFERSIZE> buffer1;




	float buffer2[MODE_TORQUECURVE_BUFFERSIZE] = {0};
	MathExtras::Threshold_Trigger<float> zeroSensor;
	int zeroCrossings = 0;
	float bidx = 0;
	float bidx_1 = 0;
	float bEndingIdx = 0;
	float samplesInPeriod = 0;

	MathExtras::SinglePoleLPFilter<float> driveIndicatorLP;


	MathExtras::SinglePoleHPFilter<float> sigHP;
	float sigMax=ER_VOLTAGE_IN_MAX;
	float sigMin=ER_VOLTAGE_IN_MIN;

};
