//#include "pch.h"
#include <cmath>
#include "dwColors.h"

//±³¼ö´Ô ÄÚµå

dwColors::dwColors()
{
	SQRT3 = sqrt(3.f);
	SQRT2 = sqrt(2.f);
	SQRT6 = sqrt(6.f);
}


dwColors::~dwColors()
{
}


void dwColors::RGB2HSV(int r, int g, int b, float& h, float& s, float& v)	//h(0:360), s(0:1), v(0:255)
{
	float	fMin, fMax, fDelta;
	float	newH, newS, newV;

	fMin = r < g ? r : g;
	fMin = fMin  < b ? fMin : b;

	fMax = r > g ? r : g;
	fMax = fMax  > b ? fMax : b;

	newV = fMax;				// v
	fDelta = fMax - fMin;
	if (fMax > 0.0)				// NOTE: if Max is == 0, this divide would cause a crash
	{
		newS = (fDelta / fMax);	// s
	}
	else						// if max is 0, then r = g = b = 0              
	{							// s = 0, v is undefined
		newS = 0.0;
		newH = 0;				// its now undefined

		h = newH;
		s = newS;
		v = newV;

		return;
	}

	if (fDelta == 0.f)
		newH = 0;
	else
	{
		if (r >= fMax)						// > is bogus, just keeps compilor happy
			newH = (g - b) / fDelta;		// between yellow & magenta
		else if (g >= fMax)
			newH = 2.0 + (b - r) / fDelta;	// between cyan & yellow
		else
			newH = 4.0 + (r - g) / fDelta;	// between magenta & cyan
	}
	newH *= 60.0;						// degrees

	if (newH < 0.0)
		newH += 360.0;

	h = newH;
	s = newS;
	v = newV;
	return;
}

void dwColors::HSV2RGB(float h, float s, float v, int& r, int& g, int& b)	//h(0:360), s(0:1), v(0:255)
{
	float	hh, p, q, t, ff;
	int		i;

	if (s <= 0.0)			// < is bogus, just shuts up warnings
	{
		r = v;
		g = v;
		b = v;
		return;
	}

	hh = h;
	if (hh >= 360.0)
		hh = 0.0;

	hh /= 60.0;
	i = (int)hh;
	ff = hh - i;
	p = v * (1.0 - s);
	q = v * (1.0 - (s * ff));
	t = v * (1.0 - (s * (1.0 - ff)));

	switch (i) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;

	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
	default:
		r = v;
		g = p;
		b = q;
		break;
	}

	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;

	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;

	return;
}

//RGB -> XYZ -> Lab
void dwColors::RGBtoXYZ(int r, int g, int b, float& X, float& Y, float& Z){

	float var_R, var_G, var_B;

	var_R = ((float)r / 255.0f);
	var_G = ((float)g / 255.0f);
	var_B = ((float)b / 255.0f);

	if (var_R > 0.04045f) var_R = powf(((var_R + 0.055f) / 1.055f), 2.4f);
	else var_R = var_R / 12.92f;

	if (var_G > 0.04045f) var_G = powf(((var_G + 0.055f) / 1.055f), 2.4f);
	else var_G = var_G / 12.92f;

	if (var_B > 0.04045f) var_B = powf(((var_B + 0.055f) / 1.055f), 2.4f);
	else var_B = var_B / 12.92f;

	var_R = var_R * 100;
	var_G = var_G * 100;
	var_B = var_B * 100;

	X = var_R * 0.4124f + var_G * 0.3576f + var_B * 0.1805f;
	Y = var_R * 0.2126f + var_G * 0.7152f + var_B * 0.0722f;
	Z = var_R * 0.0193f + var_G * 0.1192f + var_B * 0.9505f;

}

void dwColors::XYZtoLab(float X, float Y, float Z, float& L, float& a, float& b){

	float var_X, var_Y, var_Z;
	float ref_X, ref_Y, ref_Z;

	ref_X = 95.047f; ref_Y = 100.000f; ref_Z = 108.883f;

	var_X = X / ref_X;          //ref_X =  95.047   Observer= 2¡Æ, Illuminant= D65
	var_Y = Y / ref_Y;          //ref_Y = 100.000
	var_Z = Z / ref_Z;          //ref_Z = 108.883

	if (var_X > 0.008856f) var_X = powf(var_X, 0.333f);
	else var_X = (7.787f * var_X) + (16.f / 116.f);

	if (var_Y > 0.008856f) var_Y = powf(var_Y, 0.333f);
	else var_Y = (7.787f * var_Y) + (16.f / 116.f);

	if (var_Z > 0.008856f) var_Z = powf(var_Z, 0.333f);
	else var_Z = (7.787f * var_Z) + (16.f / 116.f);

	L = (116.0f * var_Y) - 16.0f;
	a = 500.0f * (var_X - var_Y);
	b = 200.0f * (var_Y - var_Z);

}

//Lab -> XYZ -> XYZ
void dwColors::LabtoXYZ(float L, float a, float b, float& X, float& Y, float& Z){

	float var_X, var_Y, var_Z;
	float ref_X, ref_Y, ref_Z;

	ref_X = 95.047f; ref_Y = 100.000f; ref_Z = 108.883f;

	var_Y = (L + 16.f) / 116.f;
	var_X = (a / 500.f) + var_Y;
	var_Z = var_Y - (b / 200.f);

	if (powf(var_Y, 3.f) > 0.008856f) var_Y = powf(var_Y, 3.f);
	else var_Y = (var_Y - 16.f / 116.f) / 7.787f;

	if (powf(var_X, 3.f) > 0.008856f) var_X = powf(var_X, 3.f);
	else var_X = (var_X - 16.f / 116.f) / 7.787f;

	if (powf(var_Z, 3.f) > 0.008856f) var_Z = powf(var_Z, 3.f);
	else var_Z = (var_Z - 16.f / 116.f) / 7.787f;

	X = ref_X * var_X;     //ref_X =  95.047     Observer= 2¡Æ, Illuminant= D65
	Y = ref_Y * var_Y;     //ref_Y = 100.000
	Z = ref_Z * var_Z;     //ref_Z = 108.883

}

void dwColors::XYZtoRGB(float X, float Y, float Z, int& r, int& g, int& b){

	float var_X, var_Y, var_Z;
	float var_R, var_G, var_B;

	var_X = X / 100.0f;        //X from 0 to  95.047      (Observer = 2¡Æ, Illuminant = D65)
	var_Y = Y / 100.0f;        //Y from 0 to 100.000
	var_Z = Z / 100.0f;        //Z from 0 to 108.883

	var_R = (var_X*3.2406f) + (var_Y*(-1.5372f)) + (var_Z*(-0.4986f));
	var_G = (var_X*(-0.9689f)) + (var_Y*(1.8758f)) + (var_Z*(0.0415f));
	var_B = (var_X*0.0557f) + (var_Y*(-0.2040f)) + (var_Z*(1.0570f));

	if (var_R > 0.0031308f) var_R = 1.055f * (powf(var_R, (1.f / 2.4f))) - 0.055f;
	else var_R = 12.92f * var_R;

	if (var_G > 0.0031308f) var_G = 1.055f * (powf(var_G, (1.f / 2.4f))) - 0.055f;
	else var_G = 12.92f * var_G;

	if (var_B > 0.0031308f) var_B = 1.055f * (powf(var_B, (1.f / 2.4f))) - 0.055f;
	else var_B = 12.92f * var_B;

	r = int(var_R * 255.f);
	g = int(var_G * 255.f);
	b = int(var_B * 255.f);

}

void dwColors::RGB2Lab(int R, int G, int B, float& L, float& a, float& b)	//L(0:100), a(-86.117:98.184), b(-107.799:94.418)
{
	float x, y, z;
	RGBtoXYZ(R, G, B, x, y, z);
	XYZtoLab(x, y, z, L, a, b);
}
void dwColors::Lab2RGB(float L, float a, float b, int& R, int& G, int& B)	//L(0:100), a(-86.117:98.184), b(-107.799:94.418)
{
	if (L > 100) L = 100.f;
	if (L < 0) L = 0.f;
	
	if (a > 98.184f) a = 98.184f;
	if (a < -86.117f) a = -86.117f;
	
	if (b > 94.418f) b = 94.418f;
	if (b < -107.799f) b = -107.799f;

	float x, y, z;
	LabtoXYZ(L, a, b, x, y, z);
	XYZtoRGB(x, y, z, R, G, B);

	if (R > 255) R = 255;
	if (R < 0) R = 0;

	if (G > 255) G = 255;
	if (G < 0) G = 0;
	
	if (B > 255) B = 255;
	if (B < 0) B = 0;
}

void dwColors::RGB2L_Alpah_Beta(int r, int g, int b, float& l, float& alpha, float& beta)
{
	float LL = 0.3811f * r + 0.5783f * g + 0.0402 * b;
	float MM = 0.1967f * r + 0.7244f * g + 0.0782 * b;
	float SS = 0.0241f * r + 0.1288f * g + 0.8444 * b;

	if (LL == 0)
		LL += 0.00000001;
	if (MM == 0)
		MM += 0.00000001;
	if (SS == 0)
		SS += 0.00000001;

	LL = log(LL);
	MM = log(MM);
	SS = log(SS);

	float LLL = LL + MM + SS;
	float MMM = LL + MM - 2 * SS;
	float SSS = LL - MM;

	l		= LLL / SQRT3;
	alpha	= MMM / SQRT6;
	beta	= SSS / SQRT2;
}

void dwColors::L_Alpah_Beta2RGB(float l, float alpha, float beta, int& r, int& g, int& b)
{
	float LL = (SQRT3 * l	) / 3.f;
	float MM = (SQRT6 * alpha) / 6.f;
	float SS = (SQRT2 * beta) / 2.f;

	float LLL = LL + MM + SS;
	float MMM = LL + MM - SS;
	float SSS = LL - 2 * MM;

	LL = pow(10, LLL);
	MM = pow(10, MMM);
	SS = pow(10, SSS);

	r =  4.4679 * LL - 3.5873 * MM + 0.1193 * SS;
	g = -1.2186 * LL + 2.3809 * MM - 0.1624 * SS;
	b =  0.0497 * LL - 0.2439 * MM + 1.2045 * SS;
}