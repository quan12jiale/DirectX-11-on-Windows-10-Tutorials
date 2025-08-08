#include "our_gl.h"

mat<4, 4> Viewport;
mat<4, 4> Projection;
mat<4, 4> ModelView;

/*
视口变换函数
功能: 将标准化设备坐标(NDC)转换为屏幕坐标

变换过程:
输入: NDC坐标范围 [-1, 1]
输出: 屏幕像素坐标 [x, x+w] × [y, y+h]

视口变换数学推导
对于NDC坐标(x_ndc, y_ndc)在范围[-1, 1]：
缩放: 从[-1,1]映射到[0, w]和[0, h]
平移: 移动到视口起始位置(x, y)
*/
void viewport(const int x, const int y, const int w, const int h)
{
	Viewport = { {{w / 2.0,0,0,x + w / 2.0},
		{0,h / 2.0,0,y + h / 2.0},
		{0,0,1,0},
		{0,0,0,1}} };
}

/*
投影变换函数
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

/*
lookat函数用于构建3D相机的视图变换矩阵（ModelView矩阵），将世界坐标系转换到相机坐标系。
ModelView = 旋转矩阵 * 平移矩阵
1、旋转矩阵（坐标轴对齐）
这个矩阵将世界坐标系对齐到相机坐标系
2、平移矩阵（移动到原点）
将相机位置移动到坐标原点。
*/
void lookat(const vec3 eye, const vec3 center, const vec3 up)
{
	const vec3 z = normalized(center - eye);
	const vec3 x = normalized(cross(up, z));
	const vec3 y = normalized(cross(z, x));
	ModelView = mat<4, 4>{ {{x.x,x.y,x.z,0}, 
		{y.x,y.y,y.z,0}, 
		{z.x,z.y,z.z,0}, 
		{0,0,0,1}} } 
		* 
		mat<4, 4>{ {{1, 0, 0, -eye.x},
			{ 0,1, 0,  -eye.y },
			{ 0, 0, 1, -eye.z },
			{ 0, 0, 0, 1 }} };
}
