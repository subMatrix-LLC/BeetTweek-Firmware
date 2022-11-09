
#include "Mode.h"

#define INDENT_MODE_INDENTMAP_SIZE 128
class Mode_Indent2 : public Mode {
public:
	Mode_Indent2();
	virtual ~Mode_Indent2() ;



	virtual void Initialize();
	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);


	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);


	virtual void MainThreadUpdateFunction(float sampleTime);


	//Prints Debug Information
	virtual void DebugPrint();

	float indentMap[INDENT_MODE_INDENTMAP_SIZE] = {0.0f};

	int wrapMap(int idx) {
		return MathExtras::WrapMax(idx, INDENT_MODE_INDENTMAP_SIZE);
	}
	void SetIndentValue(int idx, float value)
	{

		indentMap[wrapMap(idx)] = value;
	}
	float GetIndentValue(int idx)
	{
		return indentMap[wrapMap(idx)];
	}
	float GetIndentDerivativeApproximate(int idx)
	{
		const int avgSize = 8;
		float val = 0;
		for(int i = -avgSize/2; i <= avgSize/2; i++)
		{
			val +=  (indentMap[wrapMap(idx+i+1)] - indentMap[wrapMap(idx+i-1)])/(2.0f/(INDENT_MODE_INDENTMAP_SIZE));
		}
		val = val/(avgSize*2);

		return val;
	}
	float detentFunction(float x)
	{
		x = MathExtras::ClampInclusive(x, -M_PI, M_PI);
		return (1 - cos(x - M_PI))*0.5f;
	}
};


