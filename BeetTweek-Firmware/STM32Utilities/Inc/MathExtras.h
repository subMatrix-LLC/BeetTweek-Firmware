/*
 * MathExtras.h
 *
 *  Created on: Aug 6, 2020
 *      Author: casht
 */


//
// Copyright (c) 2008-2018 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#ifndef MATHEXTRAS_H_
#define MATHEXTRAS_H_

#include <cmath>
#include <type_traits>
#include <limits>

#undef M_PI
#define M_PI 3.14159265358979323846264338327950288f

#undef M_HALF_PI
#define M_HALF_PI (M_PI * 0.5f)

#undef M_TWO_PI
#define M_TWO_PI (M_PI * 2.0f)

#define SQRT_3 1.7320508075689f
#define SQRT_3_OVER_2 (SQRT_3*0.5f)

#define M_PI_OVER_3 (M_PI / 3.0f)


#define M_MIN_INT 0x80000000
#define M_MAX_INT 0x7fffffff
#define M_MIN_UNSIGNED 0x00000000
#define M_MAX_UNSIGNED 0xffffffff

#define M_EPSILON 0.000001f
#define M_LARGE_EPSILON 0.00005f
#define M_MIN_NEARCLIP 0.01f
#define M_MAX_FOV 160.0f
#define M_LARGE_VALUE 100000000.0f
#define M_INFINITY (float)HUGE_VAL
#define M_DEGTORAD (M_PI / 180.0f)
#define M_DEGTORAD_2 (M_PI / 360.0f)    // M_DEGTORAD / 2.f
#define M_RADTODEG (1.0f / M_DEGTORAD)
#define M_RADTOPERC (1.0f / M_TWO_PI)


namespace MathExtras {


	/// Check whether two floating point values are equal within accuracy.
	template <class T>
	inline bool Equals(T lhs, T rhs) { return lhs + std::numeric_limits<T>::epsilon() >= rhs && lhs - std::numeric_limits<T>::epsilon() <= rhs; }
	template <class T>
	inline bool Equals(T lhs, T rhs, T epsilon) { return lhs + epsilon >= rhs && lhs - epsilon <= rhs; }



	/// Return remainder of X/Y for float values.
	template <class T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
	inline T Mod(T x, T y) { return fmod(x, y); }

	/// Return remainder of X/Y for integer values.
	template <class T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	inline T Mod(T x, T y) { return x % y; }

	/// Return always positive remainder of X/Y.
	template <class T>
	inline T AbsMod(T x, T y)
	{
		const T result = Mod(x, y);
		return result < 0 ? result + y : result;
	}


	/* wrap x -> [0,max)  0 Included, max not included in result */
	template<typename T> inline T WrapMax(T x, T max)
	{
		if( x < 0 || x >= max)//shortcut check
		{
			/* integer math: `(max + x % max) % max` */
			return Mod(max + Mod(x, max), max);
		}
		else
			return x;
	}
	/* wrap x -> [0,max)  0 Included, max not included in result,
	 * does not account for multiple wraps so if x is more than max*2 the result will be > max
	 * likewise if x < -max, x will be still be negative
	*/
	template<typename T> inline T WrapOnceMax(T x, T max)
	{
		if(x >= max)
		{
			return x - max;
		}
		else if(x < 0)
		{
			return x + max;
		}
		else
			return x;
	}

	template<typename T> inline T WrapOnceMaxOne(T x)
	{
		if(x >= 1)
		{
			return x - 1;
		}
		else if(x < 0)
		{
			return x + 1;
		}
		else
			return x;
	}



	/* wrap x -> [min,max)  min Included, max not included in result,
	 * does not account for multiple wraps so if x is more than max*2 the result will be > max
	 * likewise if x < -min*2 x will be < min
	*/
	template<typename T> inline T WrapOnceMinMax(T x, T min, T max)
	{
		if(x >= max)
		{
			return x - max + min;
		}
		else if(x < min)
		{
			return x + max + min;
		}
		else
			return x;
	}

	/* wrap x -> [min,max)  min Included, max not included in result */
	template<typename T> inline T WrapMinMax(T x, T min, T max)
	{
	    return min + WrapMax(x - min, max - min);
	}

	///clamp x to the given range inclusive (min and max are included in result)
	template<typename T> inline T ClampInclusive(T x, T min, T max)
	{
		if(x < min)
			x = min;
		else if(x > max)
			x = max;


		return x;
	}

	///clamp x to the given range (min is included in result)
	template<typename T> inline T ClampMin(T x, T min)
	{
		if(x < min)
			x = min;
		return x;
	}

	///clamp x to the given range (max is included in result)
	template<typename T> inline T ClampMax(T x, T max)
	{
		if(x > max)
			x = max;
		return x;
	}




	/// Linear interpolation between two values.
	template <class T, class U>
	inline T Lerp(T lhs, T rhs, U t) { return lhs * (1.0 - t) + rhs * t; }

	/// Inverse linear interpolation between two values.
	template <class T>
	inline T InverseLerp(T lhs, T rhs, T x) { return (x - lhs) / (rhs - lhs); }






	/// Return the smaller of two values.
	template <class T, class U>
	inline T Min(T lhs, U rhs) { return lhs < rhs ? lhs : rhs; }

	/// Return the larger of two values.
	template <class T, class U>
	inline T Max(T lhs, U rhs) { return lhs > rhs ? lhs : rhs; }


	/// Return absolute value of a value
	template <class T>
	inline T Abs(T value) { return value >= 0.0 ? value : -value; }

	/// Return the sign of a float (-1, 0 or 1.)
	template <class T>
	inline T Sign(T value) { return value > 0.0 ? 1.0 : (value < 0.0 ? -1.0 : 0.0); }




	/// Check whether a floating point value is NaN.
	template <class T> inline bool IsNaN(T value) { return std::isnan(value); }

	/// Check whether a floating point value is positive or negative infinity
	template <class T> inline bool IsInf(T value) { return std::isinf(value); }

	//compute the local difference between 2 values (val1 - val2) assuming they are no more than half a wrap apart inside the given domain
	template <class T> inline T WrappedLocalDifference(T val1, T val2, T domain)
	{
		//convert both values to inside the range
		val1 = WrapMinMax<T>(val1, -domain/2, domain/2);
		val2 = WrapMinMax<T>(val2, -domain/2, domain/2);

		T diff = val1 - val2;
		if(diff > domain/2)
			diff -= domain;
		else if(diff < -domain/2)
			diff += domain;

		return diff;
	}



	//returns true if idx is less than prevIdx and idx is less than half buffer size and preIdx is more than half bufferSize
	//for indicating a loop around
	template <class T> inline bool ForwardLooped(T idx, T prevIdx, T bufferSize)
	{
		if(idx < bufferSize/2 && prevIdx >= bufferSize/2)
			return true;
		return false;
	}

	template <class T> inline bool IsLikelyForwardIdxDelta(T idx, T prevIdx, T bufferSize)
	{
		if((idx - prevIdx > 0) && ((idx - prevIdx) < bufferSize/2))
		{
			return true;
		}
		else if(ForwardLooped(idx, prevIdx, bufferSize))
		{
			return true;
		}
		else
			return false;
	}


	//compute the local difference between 2 angles assuming they are no more than half a rotation apart (angle1 - angle2) and the 2 angles are in range 0 to M_TWO_PI
	template <class T> inline T AngleLocalDifferenceRad(T angle1, T angle2)
	{
		return WrappedLocalDifference<T>(angle1, angle2, M_TWO_PI);
	}



	/// Return sine of an angle in degrees.
	template <class T> inline T Sin(T angle) { return sin(angle * M_DEGTORAD); }


	/// Return cosine of an angle in degrees.
	template <class T> inline T Cos(T angle) { return cos(angle * M_DEGTORAD); }

	/// Return tangent of an angle in degrees.
	template <class T> inline T Tan(T angle) { return tan(angle * M_DEGTORAD); }

	/// Return arc sine in degrees.
	template <class T> inline T Asin(T x) { return M_RADTODEG * asin(ClampInclusive(x, T(-1.0), T(1.0))); }

	/// Return arc cosine in degrees.
	template <class T> inline T Acos(T x) { return M_RADTODEG * acos(ClampInclusive(x, T(-1.0), T(1.0))); }

	/// Return arc tangent in degrees.
	template <class T> inline T Atan(T x) { return M_RADTODEG * atan(x); }

	/// Return arc tangent of y/x in degrees.
	template <class T> inline T Atan2(T y, T x) { return M_RADTODEG * atan2(y, x); }

	/// Return X in power Y.
	template <class T> inline T Pow(T x, T y) { return pow(x, y); }

	/// Return natural logarithm of X.
	template <class T> inline T Ln(T x) { return log(x); }

	/// Return square root of X.
	template <class T> inline T Sqrt(T x) { return sqrt(x); }

	/// Return floating-point remainder of X/Y.
	template <class T> inline T Mod(T x, T y) { return fmod(x, y); }

	/// Return fractional part of passed value in range [0, 1).
	template <class T> inline T Fract(T value) { return value - floor(value); }

	/// Round value down.
	template <class T> inline T Floor(T x) { return floor(x); }

	/// Round value down. Returns integer value.
	template <class T> inline int FloorToInt(T x) { return static_cast<int>(floor(x)); }

	/// Round value to nearest integer.
	template <class T> inline T Round(T x) { return round(x); }

	/// Round value to nearest integer.
	template <class T> inline int RoundToInt(T x) { return static_cast<int>(round(x)); }

	/// Round value up.
	template <class T> inline T Ceil(T x) { return ceil(x); }

	/// Round value up.
	template <class T> inline int CeilToInt(T x) { return static_cast<int>(ceil(x)); }

	/// Check whether an unsigned integer is a power of two.
	inline bool IsPowerOfTwo(unsigned value)
	{
	    return !(value & (value - 1));
	}

	/// Round up to next power of two.
	inline unsigned NextPowerOfTwo(unsigned value)
	{
	    // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
	    --value;
	    value |= value >> 1u;
	    value |= value >> 2u;
	    value |= value >> 4u;
	    value |= value >> 8u;
	    value |= value >> 16u;
	    return ++value;
	}

	/// Round up or down to the closest power of two.
	inline unsigned ClosestPowerOfTwo(unsigned value)
	{
	    unsigned next = NextPowerOfTwo(value);
	    unsigned prev = next >> (unsigned)1;
	    return (value - prev) > (next - value) ? next : prev;
	}

	/// Return log base two or the MSB position of the given value.
	inline unsigned LogBaseTwo(unsigned value)
	{
	    // http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
	    unsigned ret = 0;
	    while (value >>= 1)     // Unroll for more speed...
	        ++ret;
	    return ret;
	}

	/// Count the number of set bits in a mask.
	inline unsigned CountSetBits(unsigned value)
	{
	    // Brian Kernighan's method
	    unsigned count = 0;
	    for (count = 0; value; count++)
	        value &= value - 1;
	    return count;
	}

	/// Return a representation of the specified floating-point value as a single format bit layout.
	inline unsigned FloatToRawIntBits(float value)
	{
	    unsigned u = *((unsigned*)&value);
	    return u;
	}

	/// Update a hash with the given 8-bit value using the SDBM algorithm.
	inline constexpr unsigned SDBMHash(unsigned hash, unsigned char c) { return c + (hash << 6u) + (hash << 16u) - hash; }


	/// Convert float to half float. From https://gist.github.com/martinkallman/5049614
	inline unsigned short FloatToHalf(float value)
	{
	    unsigned inu = FloatToRawIntBits(value);
	    unsigned t1 = inu & 0x7fffffffu;         // Non-sign bits
	    unsigned t2 = inu & 0x80000000u;         // Sign bit
	    unsigned t3 = inu & 0x7f800000u;         // Exponent

	    t1 >>= 13;                              // Align mantissa on MSB
	    t2 >>= 16;                              // Shift sign bit into position

	    t1 -= 0x1c000;                          // Adjust bias

	    t1 = (t3 < 0x38800000) ? 0 : t1;        // Flush-to-zero
	    t1 = (t3 > 0x47000000) ? 0x7bff : t1;   // Clamp-to-max
	    t1 = (t3 == 0 ? 0 : t1);                // Denormals-as-zero

	    t1 |= t2;                               // Re-insert sign bit

	    return (unsigned short)t1;
	}

	/// Convert half float to float. From https://gist.github.com/martinkallman/5049614
	inline float HalfToFloat(unsigned short value)
	{
	    unsigned t1 = value & 0x7fffu;           // Non-sign bits
	    unsigned t2 = value & 0x8000u;           // Sign bit
	    unsigned t3 = value & 0x7c00u;           // Exponent

	    t1 <<= 13;                              // Align mantissa on MSB
	    t2 <<= 16;                              // Shift sign bit into position

	    t1 += 0x38000000;                       // Adjust bias

	    t1 = (t3 == 0 ? 0 : t1);                // Denormals-as-zero

	    t1 |= t2;                               // Re-insert sign bit

	    float out;
	    *((unsigned*)&out) = t1;
	    return out;
	}










    /// https://en.wikipedia.org/wiki/Fast_inverse_square_root
	/// Carmak Method
	inline float FastInverseSqrt( float number )
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y  = number;
		i  = * ( long * ) &y;                       // floating point bit level hacking
		i  = 0x5f3759df - ( i >> 1 );               // IEEE 754 Assumptions
		y  = * ( float * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
		y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

		return y;
	}




	///Interpolate Between the values of an array assuming the distance between each value is 1.  lookup is a floating point index.
	///if lookup points to the beginning or end of the array the interpolation will consider that values of the wrapped values(on opposite sizes of the array).
	///lookup of 0 corresponds to the value at array[0] and will return array[0]. lookup of 0.5 will return interp value halfway between array[0] and array[1].
	template <class T, class U>
	inline T WrappedArrayInterp(T* array, int size, U lookup)
	{
		//wrap lookup into proper range first
		U realSpace = WrapMax(lookup, U(size));//1.0
		U arraySpace = WrapMax(U(lookup + 0.5), U(size));//1.5

		U r = (realSpace - Floor(realSpace));//0

		if(r < 0.5)
		{
			return Lerp(array[int(arraySpace)], array[WrapMax(int(arraySpace) + 1, size)], r);
		}
		else
		{
			return Lerp(array[int(arraySpace)], array[WrapMax(int(arraySpace) - 1, size)], 1.0-r);
		}
	}


	// Ramp 1
	// Fast, Simple Linear Ramp With Following definition:
	///
	//
	//      0
	//      |  ________1.0
	//      |/|
	//      / |------- 0.5
	//     /| |
	//____/ | |_______ 0.0
	//    | X |
	//    |---|
	//    width
	template <class T>
	inline T LinearRamp_1(T x, T width)
	{
		if(x < -width*0.5)
			return 0.0;
		else if(x > width*0.5)
			return 1.0;
		else
		{
			return (x + 0.5*width)/(width);
		}
	}

	// Ramp 2
	// Fast, Simple Linear Ramp With Following definition:
	///
	//
	//
	//    0    ________1.0
	//    |  /|
	//    | / |
	//    |/  |
	//____/   |_______ 0.0
	//    X   |
	//    |---|
	//    width
	template <class T>
	inline T LinearRamp_2(T x, T width)
	{
		if(x < 0)
			return 0.0;
		else if(x > width)
			return 1.0;
		else
		{
			return (x)/(width);
		}
	}



}

#endif /* MATHEXTRAS_H_ */
