#ifndef MATHEXTRAS_CURVES_H_
#define MATHEXTRAS_CURVES_H_

#include "MathExtras.h"

#include "arm_math.h"

namespace MathExtras {


//K1 is slope at {x_1 , y_1}, K2 is slope at {x_2 , y_2}
template <typename T>
inline T CubicSpline_1D(T x_1, T y_1, T x_2, T y_2, T K_1, T K_2, T x)
{
	T t = (x - x_1)/(x_2 - x_1);
	T oneMinusT = (1 - t);
	T x_2MinusX_1 = (x_2-x_1);
	T y_2MinusY_1 = (y_2-y_1);
	T a = K_1*x_2MinusX_1 - y_2MinusY_1;
	T b = -K_2*x_2MinusX_1 + y_2MinusY_1;
	return oneMinusT*y_1 + t*y_2 + t*oneMinusT*(oneMinusT*a + t*b);
}




//Cos based curve "mask".
//     x=0
//      |
//      _
//_____/ \_____
//    |---|
//    width
//
inline float CosCurveFilterF(float x, float width)
{
	if(x > width*0.5f)
		return 0.0f;
	else if(x < -width*0.5f)
		return 0.0f;
	
	return (-arm_cos_f32(((M_TWO_PI)/width)*x - M_PI) + 1.0f)*0.5f;
}


//Pulse Train, duty percent and pulse width
//
//
//Pulse Starts High (1.0) at x = 0
template <typename T>
T PulseTrain1(T x, T cycleWidth, T dutyPercent)
{
	T wrapped = MathExtras::WrapMax(x, cycleWidth);
	if(wrapped/cycleWidth <= dutyPercent)
		return 1.0;
	else
		return 0.0;
}

//Pulse Train,  pulse width and spacing after pulse
//
//
//Pulse Starts High (1.0) at x = 0
template <typename T>
T PulseTrain2(T x, T pulseWidth, T spacing)
{
	T wrapped = MathExtras::WrapMax(x, pulseWidth + spacing);
	if(wrapped <= pulseWidth)
		return 1.0;
	else
		return 0.0;
}


//Ramp Train,  pulse width and spacing after pulse
//
//
//Ramp Starts Low (0.0) at x = 0 and Rises to (1.0) at x = rampWidth or inverse if dir = -1. spacing is space after each pulse to next pulse start.
template <typename T>
T RampTrain2(T x, T rampWidth, T spacing)
{
	T wrapped = MathExtras::WrapMax<T>(x, rampWidth + spacing);
	if(wrapped <= rampWidth)
	{
		return wrapped/rampWidth;
	}
	else
		return 0.0;
}




//Ramp Train,  specified by cycleWidth and dutyPercent
//
//
//Ramp Starts Low (0.0) at x = 0 and Rises to (1.0) at x = cycleWidth*dutyPercent or inverse if dir = -1. spacing is space after each pulse to next pulse start.
template <typename T>
T RampTrain1(T x, T cycleWidth, T dutyPercent)
{
	T wrapped = MathExtras::WrapMax<T>(x, cycleWidth);
	if(wrapped/cycleWidth <= dutyPercent)
	{
		return wrapped/(cycleWidth*dutyPercent);
	}
	else
		return 0.0;
}

//Ramp Train,  specified by cycleWidth and dutyPercent
//
//
//Same as RampTrain1 x = 0 is centered at the ramp center with value 0.5;
template <typename T>
T RampTrain1Centered(T x, T cycleWidth, T dutyPercent)
{
	T offset = cycleWidth*dutyPercent*0.5;
	return RampTrain1(x + offset, cycleWidth, dutyPercent);
}







}

#endif
