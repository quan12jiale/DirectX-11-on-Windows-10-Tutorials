#include "our_gl.h"

mat<4, 4> Viewport;
mat<4, 4> Projection;

void viewport(const int x, const int y, const int w, const int h)
{
	Viewport = { {{w / 2.0,0,0,x + w / 2.0},
		{0,h / 2.0,0,y + h / 2.0},
		{0,0,1,0},
		{0,0,0,1}} };
}

/*
这是一个简化的透视投影：

保持X坐标不变
翻转Y坐标（处理坐标系差异）
Z坐标用于深度缓冲
W分量设为
-z/f
，为透视除法做准备
*/
void projection(const double coeff)
{
	Projection = { {{1,0,0,0},
		{0,-1,0,0},
		{0,0,1,0},
		{0,0,-1 / coeff,0}} };
}

void lookat(const vec3 eye, const vec3 center, const vec3 up)
{
}
