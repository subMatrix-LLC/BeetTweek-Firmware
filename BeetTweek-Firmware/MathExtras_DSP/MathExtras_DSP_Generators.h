/*
 * MathExtras_DSP_Generators.h
 *
 *  Created on: Nov 18, 2021
 *      Author: casht
 */

#ifndef INC_MATHEXTRAS_DSP_GENERATORS_H_
#define INC_MATHEXTRAS_DSP_GENERATORS_H_


namespace MathExtras {


template <class T>
class OscillatorSystem
{
public:
	T x = 1.0;
	T x_prime = 0;


	inline void Process(T ommega, T sampletime)
	{
		//simple spring oscillator
		T ommega_squared = ommega*ommega;//hz
		T d_x_prime_dt = -((ommega_squared)*x);

		//T halfSampleTime = sampletime*0.5;


		//Euler

		x += x_prime*sampletime;
		x_prime += d_x_prime_dt*sampletime;

		/*
		//RK4
		//Get K Derivatives at mid points
		T K1_1 = x_prime;
		T K1_2 = d_x_prime_dt;

		T K2_1 = (x_prime + K1_1*halfSampleTime);
		T K2_2=  (d_x_prime_dt + K1_2*halfSampleTime);

		T K3_1 = (x_prime + K2_1*halfSampleTime);
		T K3_2=  (d_x_prime_dt + K2_2*halfSampleTime);

		T K4_1 = (x_prime + K3_1*sampletime);
		T K4_2=  (d_x_prime_dt + K3_2*sampletime);


		//Advance X
		x += (sampletime*0.16666f)*(K1_1 + 2*K2_1 + 2*K2_1 + K4_1);
		x_prime += (sampletime*0.16666f)*(K1_2 + 2*K2_2 + 2*K2_2 + K4_2);
*/

		//clamp variables to correct for drift.
		x = MathExtras::ClampInclusive(x, static_cast<T>(-1.0),static_cast<T>(1.0));
	}
};



}


#endif /* INC_MATHEXTRAS_DSP_GENERATORS_H_ */
