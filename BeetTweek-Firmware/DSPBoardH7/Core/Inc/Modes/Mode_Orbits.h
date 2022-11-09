/*
 * Mode_Orbits.h
 *
 *  Created on: Apr 11, 2022
 *      Author: casht
 */

#ifndef INC_MODES_MODE_ORBITS_H_
#define INC_MODES_MODE_ORBITS_H_




#include "Modes/Mode.h"
class Mode_Orbits : public Mode {
public:


	Mode_Orbits();
	virtual ~Mode_Orbits();

	//do setup things when the mode is entered
	virtual void Initialize();

	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);

	//Called Very Fast.  Holds DSP Code
	virtual void AudioDSPFunction(float sampleTime, int bufferSwap);

	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);



	struct Particle {
		float mass = 1.0f;
		float force = 0.0f;
		float vel = 0.0f;
		float angle = 0.0f;
		float angleLED_1 = 0.0f;
	};

	Particle particles[4];

	virtual void OnGestureRecordStart();
	virtual void OnGesturePlay();
	virtual void OnGestureLoop();
	virtual void ButtonUp(int button);
	virtual void ButtonDown(int button);

	virtual void OnButtonQuickAction(int button);


	Particle particle_guestSave;

	virtual void ReadExtraGuestureData(float *data);
	virtual void WriteExtraGuestureData(float *data);
};




#endif /* INC_MODES_MODE_ORBITS_H_ */
