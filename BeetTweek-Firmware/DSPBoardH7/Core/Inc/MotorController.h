/*
 * MotorController.h
 *
 *  Created on: Nov 18, 2020
 *      Author: casht
 */

#ifndef MOTORCONTROLLER_H_
#define MOTORCONTROLLER_H_



#include "main.h"

#include "MotorControl.h"

#include "RollingFloatBuffer.h"
#include "PIDControl.h"
#include "ProbeTools.h"

#include "AS5048.h"

#include "MathExtras.h"
#include "arm_math.h"
#include "stdio.h"

#define ANTCOGGPOINTS (720)
#define ANTICOGGMINSAMPLES (200)

extern MotorDevice motorDevA;
extern AS5048_Device motorEncoderDev;


class MotorController {
public:
	MotorController();
	virtual ~MotorController();

	enum ControlScheme {
		ControlScheme_Free = 0,
		ControlScheme_PhysicallyModelled_Direct,     //simulate drivestrength(Torque) as a direct computation of parameters.
		ControlScheme_SpeedTarget,                   //use speed as target. controls drive strength as function of speed error
		ControlScheme_AngleTarget,                   //use angle as target - controls speed loop to get there
		ControlScheme_AngleTarget_Direct,            //use angle as target. controls drive strength as function of angle error
		ControlScheme_AngleTarget_Phase,             //use angle as target. controls drive phase as function of angle error
		ControlScheme_AngleTarget_PhaseDirectHybrid, //control phase to get angle target - but use drivestrength to get rotation in correct multiple of rotor fluxangle.
		ControlScheme_AngleTarget_PhaseIncremental,  //control phase to get angle target - adjust phase by constant amount (no pid)
		ControlScheme_TorqueTarget,					 //instantly control torque
		ControlScheme_TorqueModeling,			     //apply torque, simulate momentum
		ControlScheme_ConstantTorque,				 //apply torque 90 deg out of phase with rotor flux.  no control.
		ControlScheme_Lock,							 //apply a constant zero phase pwm to the motor. holding it at whatever mechanical angle that is.
		ControlScheme_SimpleRotation,			     //simple constant rotation with no feedback.
	};

	enum CalibrationMode {
		CalibrationMode_None = 0,
		CalibrationMode_ZeroPhase,
		CalibrationMode_AntiCogg
	};


	void UpdateMotorControl(float DeltaTime);

	//do the startup callibration
	void ZeroPhaseCalibration();
	void AntiCoggingCalibration();

	//Set the Current angle as the zero phase angle
	void SetZeroPhaseAngle();







	MotorDevice* motorDevice = &motorDevA;
	AS5048_Device* encoder = &motorEncoderDev;



	RollingFloatBuffer angleBuffer;
	FloatViewWindow utilFltViewWindow;

	PIDState speedToDrivePID;
	PIDState angleToSpeedPID;
	PIDState angleToDrivePID;
	PIDState angleToPhaseAnglePID;
	PIDState angleFollowPID;
	PIDState speedInputFilterPID;
	PIDState speedTargetPID;


	float motorAngle = 0;
	float motorAngleZeroPhase = 0;
	float phaseOffsetExtra = 0;
	float phaseOffset = 0;
	float phaseAngle = 0;
	float rotorFluxAngle = 0;
	uint32_t timerPeriodCount = 0;
	uint32_t timerPeriodLastCCR = 0;
	float rotationSpeedFiltered = 0.0f;
	float rotationAccelFiltered = 0.0f;
	float speedTarget = -0.5f;
	float angleTarget = 0.0f;
	float torqueCommand = 0.0f;
	float angleError = 0.0f;
	float angleDecrementor = 1.0f;
	float driveStrength = 0.0f;
	float driveStrengthFiltered = 0.0f;
	float driveAngle = 0.0f;

	float torqueIntegral = 0.0f;
	float momentum = 0.0f;

	float flywheelRotationalSpeed = 0.0f;
	float flywheelAngle = 0.0f;
	int enableAntiCogg = 0;
	float antiCoggAmplitude = 0.0f;



	float smallestAngleErrorPosDir = 999.0f;
	float smallestAngleErrorNegDir = 999.0f;
	float smallestAngleError = 999.0f;
	float bestPositiveDrStr = -1.0f;
	float bestNegativeDrStr = 1.0f;


	ControlScheme curControlScheme = ControlScheme_TorqueTarget;

	float coggDriveLookupCW[ANTCOGGPOINTS];
	float coggDriveCWMean = 0.0f;
	int coggDriveLookupCW_SMPLCNT[ANTCOGGPOINTS];
	bool coggDriveLookupCW_SMPLCMPL = false;//all samples aquired

	float coggDriveLookupCCW[ANTCOGGPOINTS];
	float coggDriveCCWMean = 0.0f;
	int coggDriveLookupCCW_SMPLCNT[ANTCOGGPOINTS];
	bool coggDriveLookupCCW_SMPLCMPL = false;//all samples aquired

	float meanFrictionDrive = 0.0f;

	arm_biquad_casd_df1_inst_f32 driveFilter;
	float driveFilterState[4*1];
	float driveFilterCoeff[5*1];


	CalibrationMode calibrationMode = CalibrationMode_None;

};

#endif /* MOTORCONTROLLER_H_ */









