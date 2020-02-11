#pragma once

//±³¼ö´Ô ÄÚµå

class dwColors
{
	void XYZtoRGB(float X, float Y, float Z, int& r, int& g, int& b);
	void XYZtoLab(float X, float Y, float Z, float& L, float& a, float& b);
	void LabtoXYZ(float L, float a, float b, float& X, float& Y, float& Z);
	void RGBtoXYZ(int r, int g, int b, float& X, float& Y, float& Z);

	float	SQRT3;
	float	SQRT2;
	float	SQRT6;

public:
	dwColors();
	~dwColors();

	void	RGB2HSV(int r, int g, int b, float& h, float& s, float& v);	//h(0:360), s(0:1), v(0:255)
	void	HSV2RGB(float h, float s, float v, int& r, int& g, int& b);	//h(0:360), s(0:1), v(0:255)

	void	RGB2Lab(int R, int G, int B, float& L, float& a, float& b);	//L(0:100), a(-86.117:98.184), b(-107.799:94.418)
	void	Lab2RGB(float L, float a, float b, int& R, int& G, int& B);	//L(0:100), a(-86.117:98.184), b(-107.799:94.418)

	void	RGB2L_Alpah_Beta(int r, int g, int b, float& l, float& alpha, float& beta);
	void	L_Alpah_Beta2RGB(float l, float alpha, float beta, int& r, int& g, int& b);
};

