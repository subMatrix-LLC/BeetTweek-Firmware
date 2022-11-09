/*
 * MotorAngleState.h
 *
 *  Created on: Jun 2, 2020
 *      Author: casht
 */

#ifndef INC_MOTORANGLESTATE_H_
#define INC_MOTORANGLESTATE_H_



//angle tracking to support tracking angles
typedef struct MotorAngleState
{
	float currentAngle;
	float lastAngle;
	int revolutionAccumulator; //revolution offset
	int revolutionAccumulatorLast; //revolution offset


	float lastAngleFiltered;
	float currentAngleFiltered;


	//derivatives
	float currentAngularVel; //rev   per sec
	float lastAngularVel; //rev   per sec

	float currentAngularVelFiltered; //rev   per sec
	float currentAngularVelFiltered2; //rev   per sec
	float lastAngularVelFiltered; //rev   per sec

	float currentAngularAcc; //rev per sec^2

	float currentAngularAccFiltered; //rev per sec^2

} MotorAngleState;


//Initialize a Motor Angle State
void MotorAngleStateInit(MotorAngleState* state);

//frees memory associated with the state
void MotorAngleStateDestroy(MotorAngleState* state);


void MotorAngleStateCopyTo(MotorAngleState* targetState);


// Reset Motor Angle To 0 and all other stats to 0
void MotorAngleStateReset(MotorAngleState* state);

// Reset Accumulation Data to 0 only, keep local angle.
void MotorAngleStateResetAccumulation(MotorAngleState* state);

// Update Motor angle with new local angle. ensure successive angles are spaced no more than 180 degrees (0.5) apart.
void MotorAngleStateUpdateLocalf(MotorAngleState* state, float newAngle, float deltaTimeS);
// Update Motor angle with new Accumulated angle.
void MotorAngleStateUpdateAccumulatedd(MotorAngleState* state, double newAngle, float sampleTimeS);
// Get Current Angle from motor state - angle is always from 0 to 1.
float MotorAngleStateGetCurrentAnglef(MotorAngleState* state);

// Get Current Accumulated Angle from Motor state.  Note: could be large number with less precision in the fractional part.
//                                                        use MotorAngleGetCurrentAnglef for more precision.
float MotorAngleStateGetCurrentAccumulatedAnglef(MotorAngleState* state);
double MotorAngleStateGetCurrentAccumulatedAngled(MotorAngleState* state);


void MotorAngleStateUpdateFiltering(MotorAngleState* state, float sampleTimeS);

// Get Current Angle Difference that is < 1. (360)
float MotorAngleStateGetAngleDeltaf(MotorAngleState* state);



#endif /* INC_MOTORANGLESTATE_H_ */
