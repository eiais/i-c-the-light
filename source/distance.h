//distance estimation functions
#include <stdio.h>
//sin functions
#include <math.h>
//vectors
#include "vector.h"
//fclamp
#include "common.h"
#include "quaternion.h"
#include "logging.h"

FILE *logfile;

#ifndef DISTANCE_H
#define DISTANCE_H

//distance from point p to circle of radius r centered at c
float distcircle(vec2 p, vec2 c, float r)
{
	float o, a;
	a = p.x - c.x;
	o = p.y - c.y;
	//dist to c
	return sqrt((o * o) + (a * a)) - r;
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
	const float t = fclamp(
		dot2(pa, ba) / length,
		0, 1);
	//projection falls on the segment
	const vec2 projection = add2(a, mult2s(ba, t));
	return dist2(
		p,
		projection
	);
}

//b specifies the box dimensions
//nesting function calls for basic addition makes me feel like
//c was not the best choice for this project
float distbox(vec3 p, vec3 b)
{
	//magnitude(max(abs(p) - b, 0))
	//vec3 ret;
	//ret = sub3(
		//absvec3(p),
		//b
	//);
	//ret = maxvec3s(ret, 0.0F);
	return magn3(
		maxvec3s(
			sub3(
				absvec3(p),
				b
			),
			0.0F
		)
	);
}

//point, center, radius
float distsphere(vec3 p, vec3 c, float r)
{
	return (dist3(p, c) - r);
}

//TODO make the center param do something
float disttorus(vec3 p, vec3 c, float thickness, float radius)
{
	p = sub3(p, c);
	vec2 q;
	q.x = magn2((vec2){.x = p.x, .y = p.z}) - radius;
	q.y = p.y;
	return magn2(q) - thickness;
}

//who needs orientation?
float distground(vec3 p)
{
	return p.y;
}

//union
float opu(float a, float b)
{
	return min(a, b);
}

//subtraction
//requires signed distance functions!!!!!
float ops(float a, float b)
{
	return max(-a, b);
}

//intersection
float opi(float a, float b)
{
	return max(a, b);
}

//displace a point by a wobbly sine shape
float opwobble2(vec2 point, float orig, float freq, float amp)
{
	return amp
		* sin(point.x / freq)
		* sin(point.y / freq)
		+ orig;
}

float opwobble3(vec3 point, float orig, float freq, float amp)
{
	return amp
		* sin(point.x / freq)
		* sin(point.y / freq)
		* sin(point.z / freq)
		+ orig;
}

float oprepeat3(/*vec3 point, vec3 period*/)
{
	//this function, obviously, does nothing
	return -10000.0F;
}

float distserpenski(vec3 pos)
{
	vec3 t1 = const3( 1.0F,  1.0F,  1.0F);
	vec3 t2 = const3(-1.0F, -1.0F,  1.0F);
	vec3 t3 = const3( 1.0F, -1.0F, -1.0F);
	vec3 t4 = const3(-1.0F,  1.0F, -1.0F);
	vec3 c, tmp;
	float dist, d;
	float scale = 2.0F;
	const int Iterations = 3;
	int n;
	for(n = 0; n < Iterations; n++) {
		c = t1;
		dist = magn3(sub3(pos, t1));
		d = magn3(sub3(pos, t2));
		if(d < dist)
			c = t2, dist = d;
		d = magn3(sub3(pos, t3));
		if(d < dist)
			c = t3, dist = d;
		d = magn3(sub3(pos, t4));
		if(d < dist)
			c = t4, dist = d;
		tmp = sub3(
			mult3s(pos, scale),
			mult3s(c, scale - 1.0F)
		);
		pos = tmp;
	}
	return magn3(pos)
		* pow(scale, -(float)n);
}

float distancejulia(vec3 pos, quaternion c)
{
#define MAX_ITERATIONS 64
	float distance;
	int i;
	fprintf(logfile, "\npos:\n"
		"    %.2f\n"
		"    %.2f\n"
		"    %.2f\n",
		pos.x, pos.y, pos.z
	);
	quaternion two = constq(2.0F, 0.0F, 0.0F, 0.0F);
	quaternion q = constq(0.0F, pos.x, pos.y, pos.z);
	//q prime, the running derivative of q
	quaternion qp = constq(1.0F, 0.0F, 0.0F, 0.0F);
	quaternion tmp;
	for(i = 0; i < MAX_ITERATIONS; i++) {
		tmp = multq(q, qp);
		qp = multq(tmp, two);

		tmp = addq(sqrq(q), c);
		q = tmp;

		fprintf(logfile, "\ni = %d, q:\n", i);
		dumpquaternion(q);
		fprintf(logfile, "q':\n");
		dumpquaternion(qp);
		if(magnq(q) > 10.0F)
			break;
	}
	float qmag = magnq(q);
	//distance = 0.5F * qmag * log(qmag) / magnq(qp);
	distance = (qmag * log(qmag)) / (2.0F * magnq(qp));
	fprintf(logfile, "final distance:\n"
		"    %.2f\n", distance);
	fprintf(plotfile, "%f\n", distance);
	return distance;
}

#endif //DISTANCE_H
