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


#include <MathExtras_Color.h>
#include <cstdio>



namespace MathExtras
{

unsigned Color::ToUInt() const
{
    auto r = (unsigned)ClampInclusive(((int)(r_ * 255.0f)), 0, 255);
    auto g = (unsigned)ClampInclusive(((int)(g_ * 255.0f)), 0, 255);
    auto b = (unsigned)ClampInclusive(((int)(b_ * 255.0f)), 0, 255);
    auto a = (unsigned)ClampInclusive(((int)(a_ * 255.0f)), 0, 255);
    return (a << 24u) | (b << 16u) | (g << 8u) | r;
}

Vector3 Color::ToHSL() const
{
    float min, max;
    Bounds(&min, &max, true);

    float h = Hue(min, max);
    float s = SaturationHSL(min, max);
    float l = (max + min) * 0.5f;

    return Vector3(h, s, l);
}

Vector3 Color::ToHSV() const
{
    float min, max;
    Bounds(&min, &max, true);

    float h = Hue(min, max);
    float s = SaturationHSV(min, max);
    float v = max;

    return Vector3(h, s, v);
}

void Color::FromUInt(unsigned color)
{
    a_ = ((color >> 24u) & 0xffu) / 255.0f;
    b_ = ((color >> 16u) & 0xffu) / 255.0f;
    g_ = ((color >> 8u)  & 0xffu) / 255.0f;
    r_ = ((color >> 0u)  & 0xffu) / 255.0f;
}

void Color::FromHSL(float h, float s, float l, float a)
{
    float c;
    if (l < 0.5f)
        c = (1.0f + (2.0f * l - 1.0f)) * s;
    else
        c = (1.0f - (2.0f * l - 1.0f)) * s;

    float m = l - 0.5f * c;

    FromHCM(h, c, m);

    a_ = a;
}

void Color::FromHSV(float h, float s, float v, float a)
{
    float c = v * s;
    float m = v - c;

    FromHCM(h, c, m);

    a_ = a;
}

void Color::SetValue(float v)
{
	MathExtras::Vector3 hsv = ToHSV();
	hsv.z_ = v;
	FromHSV(hsv.x_,hsv.y_,hsv.z_, a_);
}
void Color::SetLightness(float l)
{
	MathExtras::Vector3 hsl = ToHSL();
	hsl.z_ = l;
	FromHSV(hsl.x_,hsl.y_,hsl.z_,a_);
}


float Color::Chroma() const
{
    float min, max;
    Bounds(&min, &max, true);

    return max - min;
}

float Color::Hue() const
{
    float min, max;
    Bounds(&min, &max, true);

    return Hue(min, max);
}

float Color::SaturationHSL() const
{
    float min, max;
    Bounds(&min, &max, true);

    return SaturationHSL(min, max);
}

float Color::SaturationHSV() const
{
    float min, max;
    Bounds(&min, &max, true);

    return SaturationHSV(min, max);
}

float Color::Lightness() const
{
    float min, max;
    Bounds(&min, &max, true);

    return (max + min) * 0.5f;
}

void Color::Bounds(float* min, float* max, bool clipped) const
{

    if (r_ > g_)
    {
        if (g_ > b_) // r > g > b
        {
            *max = r_;
            *min = b_;
        }
        else // r > g && g <= b
        {
            *max = r_ > b_ ? r_ : b_;
            *min = g_;
        }
    }
    else
    {
        if (b_ > g_) // r <= g < b
        {
            *max = b_;
            *min = r_;
        }
        else // r <= g && b <= g
        {
            *max = g_;
            *min = r_ < b_ ? r_ : b_;
        }
    }

    if (clipped)
    {
        *max = *max > 1.0f ? 1.0f : (*max < 0.0f ? 0.0f : *max);
        *min = *min > 1.0f ? 1.0f : (*min < 0.0f ? 0.0f : *min);
    }
}

float Color::MaxRGB() const
{
    if (r_ > g_)
        return (r_ > b_) ? r_ : b_;
    else
        return (g_ > b_) ? g_ : b_;
}

float Color::MinRGB() const
{
    if (r_ < g_)
        return (r_ < b_) ? r_ : b_;
    else
        return (g_ < b_) ? g_ : b_;
}

float Color::Range() const
{
    float min, max;
    Bounds(&min, &max);
    return max - min;
}

void Color::Clip(bool clipAlpha)
{
    r_ = (r_ > 1.0f) ? 1.0f : ((r_ < 0.0f) ? 0.0f : r_);
    g_ = (g_ > 1.0f) ? 1.0f : ((g_ < 0.0f) ? 0.0f : g_);
    b_ = (b_ > 1.0f) ? 1.0f : ((b_ < 0.0f) ? 0.0f : b_);

    if (clipAlpha)
        a_ = (a_ > 1.0f) ? 1.0f : ((a_ < 0.0f) ? 0.0f : a_);
}

void Color::Invert(bool invertAlpha)
{
    r_ = 1.0f - r_;
    g_ = 1.0f - g_;
    b_ = 1.0f - b_;

    if (invertAlpha)
        a_ = 1.0f - a_;
}

Color Color::Lerp(const Color& rhs, float t) const
{
    float invT = 1.0f - t;
    return Color(
        r_ * invT + rhs.r_ * t,
        g_ * invT + rhs.g_ * t,
        b_ * invT + rhs.b_ * t,
        a_ * invT + rhs.a_ * t
    );
}



float Color::Hue(float min, float max) const
{
    float chroma = max - min;

    // If chroma equals zero, hue is undefined
    if (chroma <= M_EPSILON)
        return 0.0f;

    // Calculate and return hue
    if (MathExtras::Equals(g_, max))
        return (b_ + 2.0f * chroma - r_) / (6.0f * chroma);
    else if (MathExtras::Equals(b_, max))
        return (4.0f * chroma - g_ + r_) / (6.0f * chroma);
    else
    {
        float r = (g_ - b_) / (6.0f * chroma);
        return (r < 0.0f) ? 1.0f + r : ((r >= 1.0f) ? r - 1.0f : r);
    }
}

float Color::SaturationHSV(float min, float max) const
{
    // Avoid div-by-zero: result undefined
    if (max <= M_EPSILON)
        return 0.0f;

    // Saturation equals chroma:value ratio
    return 1.0f - (min / max);
}

float Color::SaturationHSL(float min, float max) const
{
    // Avoid div-by-zero: result undefined
    if (max <= M_EPSILON || min >= 1.0f - M_EPSILON)
        return 0.0f;

    // Chroma = max - min, lightness = (max + min) * 0.5
    float hl = (max + min);
    if (hl <= 1.0f)
        return (max - min) / hl;
    else
        return (min - max) / (hl - 2.0f);

}

void Color::FromHCM(float h, float c, float m)
{
    if (h < 0.0f || h >= 1.0f)
        h -= floorf(h);

    float hs = h * 6.0f;
    float x = c * (1.0f - MathExtras::Abs(fmodf(hs, 2.0f) - 1.0f));

    // Reconstruct r', g', b' from hue
    if (hs < 2.0f)
    {
        b_ = 0.0f;
        if (hs < 1.0f)
        {
            g_ = x;
            r_ = c;
        }
        else
        {
            g_ = c;
            r_ = x;
        }
    }
    else if (hs < 4.0f)
    {
        r_ = 0.0f;
        if (hs < 3.0f)
        {
            g_ = c;
            b_ = x;
        }
        else
        {
            g_ = x;
            b_ = c;
        }
    }
    else
    {
        g_ = 0.0f;
        if (hs < 5.0f)
        {
            r_ = x;
            b_ = c;
        }
        else
        {
            r_ = c;
            b_ = x;
        }
    }

    r_ += m;
    g_ += m;
    b_ += m;
}

unsigned Color::ToUIntArgb() const
{
    auto r = (unsigned)ClampInclusive(((int)(r_ * 255.0f)), 0, 255);
    auto g = (unsigned)ClampInclusive(((int)(g_ * 255.0f)), 0, 255);
    auto b = (unsigned)ClampInclusive(((int)(b_ * 255.0f)), 0, 255);
    auto a = (unsigned)ClampInclusive(((int)(a_ * 255.0f)), 0, 255);
    return (a << 24) | (r << 16) | (g << 8) | b;
}

const Color Color::WHITE;
const Color Color::GRAY(0.5f, 0.5f, 0.5f);
const Color Color::BLACK(0.0f, 0.0f, 0.0f);
const Color Color::RED(1.0f, 0.0f, 0.0f);
const Color Color::GREEN(0.0f, 1.0f, 0.0f);
const Color Color::BLUE(0.0f, 0.0f, 1.0f);
const Color Color::CYAN(0.0f, 1.0f, 1.0f);
const Color Color::MAGENTA(1.0f, 0.0f, 1.0f);
const Color Color::YELLOW(1.0f, 1.0f, 0.0f);
const Color Color::ORANGE(1.0f, 0.5f, 0.0f);
const Color Color::TRANSPARENT_BLACK(0.0f, 0.0f, 0.0f, 0.0f);



const float Color::RED_HUE(0.0f);
const float Color::GREEN_HUE(120/360.0f);
const float Color::BLUE_HUE(240/360.0f);
const float Color::CYAN_HUE(180/360.0f);
const float Color::MAGENTA_HUE(300/360.0f);
const float Color::YELLOW_HUE(60/360.0f);
const float Color::ORANGE_HUE(30/360.0f);





}
