
#include "Modes/Mode.h"
#include "MathExtras_DSP.h"

extern ModeManager modeManager;


class Mode_Rachet : public Mode{
public:
	Mode_Rachet();
	virtual ~Mode_Rachet();


	//do setup things when the mode is entered
	virtual void Initialize();

	//un-setup thing in prep for next mode
	virtual void UnInitialize(){}

	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);

	//Called Very Fast.  Holds DSP Code
	virtual void AudioDSPFunction(float sampleTime, int bufferSwap);


	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);

	//called when gesture record starts.
	virtual void OnGestureRecordStart(){
		Mode::OnGestureRecordStart();
		ratchetAngle_gs = ratchetAngle;
	}
	//called when gesture record ends.
	virtual void OnGestureRecordStop(){
		Mode::OnGestureRecordStop();

	}
	//called when gesture playback loops
	virtual void OnGestureLoop(){
		Mode::OnGestureLoop();
		ratchetAngle = ratchetAngle_gs;
	}

	virtual void OnGesturePlay(){
		Mode::OnGesturePlay();
		ratchetAngle = ratchetAngle_gs;
	}




	virtual bool WriteEEPROMState(uint32_t &ee_address);

	virtual bool ReadEEPROMState(uint32_t &ee_address);

	virtual int EEPromBytesUsed()
	{
		return Mode::EEPromBytesUsed() + 1*sizeof(int);
	}
	virtual int EEPromAddressOffsets(int offset)
	{
		int byteOffset = 0;
		switch(offset)
		{
		case 0:
			byteOffset = 0;
			break;
		}

		int res = Mode::EEPromBytesUsed() + byteOffset;
		return res;
	}


	virtual void OnFuncCombo(int button);


	virtual void ButtonUp(int button);
	virtual void OnButtonQuickAction(int button);



	float ratchetAngle = 0.0f;
	float ratchetAngle_gs;

	int direction = 1;

	MathExtras::Threshold_Trigger<float> directionGate;
};
