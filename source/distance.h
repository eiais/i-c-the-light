//distance estimation functions
#include <math.h>
#include "trig.h"

//I Can't Believe It's Not Sin(x)
float parabola(float x)
{
	x -= 1;
	return (-x * x) + 1;
}

float min(float a, float b)
{
	return a < b ? a : b;
}

float max(float a, float b)
{
	return a > b ? a : b;
}

float fclamp(float val, float fmin, float fmax)
{
	return min(max(val, fmin), fmax);
}

//displace a point by a wobbly sine shape
float sindisplace2(vec2 point, float orig, float freq, float amp)
{
	return amp * sin((float)point.x / freq) * sin((float)point.y / freq) + orig;
}

//distance from point p to circle of radius r centered at c
float distcircle(vec2 p, vec2 c, float r)
{
	float o, a;
	a = p.x - c.x;
	o = p.y - c.y;
	//dist to c
	return sqrt((o*o)+(a*a)) - r;
}

//distance to line ab from p
//http://stackoverflow.com/a/1501725/5719760
float distline2(vec2 p, vec2 a, vec2 b)
{
	//length = |b-a|^2 -  avoid a sqrt
	const float length = distsqr2(a, b);
	//avoid a = b case & divide by zero
	if(length == 0.0F) return dist2(p, a);
	const vec2 pa = sub2(p, a);
	const vec2 ba = sub2(b, a);
	//Consider the line extending the segment, parameterized as
	//a + t (b - a)
	//we find projection of point c onto the line.
	//It falls where t = [(p-a) . (b-a)] / |b-a|^2
	//we clamp t from [0,1] to handle points outside the segment ab
	const float t =
		fclamp(
			dot2(pa, ba) / length
		, 0, 1);
	//projection falls on the segment
	const vec2 projection =
		add2(
			a,
			mult2scalar(
				ba,
				t
			)
		);
	return dist2(
		p,
		projection
		);
}

//float distpoly2(

//union
float opu(float a, float b)
{
	return min(a, b);
}

//subtraction
float ops(float a, float b)
{
	return max(-a, b);
}

//intersection
float opi(float a, float b)
{
	return max(a, b);
}
