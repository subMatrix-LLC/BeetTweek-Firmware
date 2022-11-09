/*
 * DualBoardComHanlder.h
 *
 *  Created on: Nov 25, 2020
 *      Author: casht
 */

#ifndef DUALBOARDCOMHANDLER_H_
#define DUALBOARDCOMHANDLER_H_

#include "main.h"


#ifndef KNOB_STM


#else

	#include "MotorControl.h"
	#include "MotorController.h"

	extern AS5048_Device motorEncoderDev;
	extern MotorDevice motorDevA;
	//extern CapSenseDevice knobCapSense;
	extern MotorController motorController;

#endif



#define DUALBOARDCOMHANDLER_SCHEME_PACK_INTERVAL 5000

class DualBoardComHandler {
public:
	DualBoardComHandler();
	virtual ~DualBoardComHandler();


	class PackScheme {
	public:
		enum PackScheme_ID {
			PackScheme_MotorTorqueTarget = 0,
			PackScheme_MotorAngleTarget = 1
		};
		PackScheme_ID id = PackScheme_MotorTorqueTarget;
		int numMasterOuts = 1;
		int numMasterIns =  1;
	};



	//queues setting the current pack scheme and send the pack scheme id across SPI.
	void Master_SetPackScheme(PackScheme scheme)
	{
		nextPackScheme = scheme;
	}

#ifndef KNOB_STM
	//main processing function.  outs are signals going from master to slave
	void Master_Process(uint16_t sig1Out, uint16_t sig2Out, uint16_t sig3Out, uint16_t sig4Out,
			uint16_t* sig1In, uint16_t* sig2In, uint16_t* sig3In, uint16_t* sig4In);



	uint16_t outs[4] = {0};
	uint16_t* ins[4] =  {nullptr};



#else


	void Slave_GetNextOut(uint16_t* sigOut);

	void Slave_ProcessNextIn(uint16_t in);

	//called when each word set has been received and the processing can commence..
	void Slave_ProcessSchemeFrame();

	void Slave_TransitionScheme();

	uint16_t outs[4] = {0};
	uint16_t ins[4] =  {0};

#endif


	PackScheme nextPackScheme = PackScheme();
	PackScheme currentPackScheme = PackScheme();



	uint16_t streamPackCounter = DUALBOARDCOMHANDLER_SCHEME_PACK_INTERVAL;
	uint16_t wordCounter = 0;
};

#endif /* DUALBOARDCOMHANDLER_H_ */
