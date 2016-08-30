//distance estimation functions
#include <math.h>

#define PI_SQ   9.86960440108F
#define TAU     6.28318530717F
#define PI      3.14159265359F
#define HALF_PI 1.57079632679F

float min(float a, float b) {
	return a < b ? a : b;
}

float max(float a, float b) {
	return a > b ? a : b;
}

float fsin(float x)
{
	x = fmod(x, TAU);
	const float sign = x > PI || x < -PI ? -1 : 1;

	x = fmod(x, PI);

	const float B =  4/PI;
	const float C = -4/(PI_SQ);

	float y = B * x + C * x * fabsf(x);

	const float P = 0.225;

	//Q = 0.775
	//Q * y + P * y * abs(y)
	y = P * (y * fabsf(y) - y) + y;

	return y * sign;
}

float fcos(float x)
{
	x += HALF_PI;
	x = fmod(x, TAU);
	const float sign = x > PI || x < -PI ? -1 : 1;

	x = fmod(x, PI);

	const float B =  4/PI;
	const float C = -4/(PI_SQ);

	float y = B * x + C * x * fabsf(x);

	const float P = 0.225;

	//Q = 0.775
	//Q * y + P * y * abs(y)
	y = P * (y * fabsf(y) - y) + y;

	return y * sign;
}

//displace a point by a wobbly sine shape
float sindisplace2(vec2 point, float orig, float freq, float amp)
{
	return amp * fsin((float)point.x / freq) * fsin((float)point.y / freq) + orig;
}

//distance from a circle
float distcircle(vec2 point, vec2 center, float radius)
{
	float o, a;
	a = point.x - center.x;
	o = point.y - center.y;
	//return root(o * o + a * a) - radius;
	return sqrt((o*o)+(a*a)) - radius;
}

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
