//functions for converting colors from uint format (0x123456)
//to rgbcolor format ({.r = 0x12, .g = 0x34, .b = 0x56})

#include <math.h>

#define CHANNELS 3

typedef unsigned char byte;

struct rgbcolor {
	byte r;
	byte g;
	byte b;
} rgbcolor;

//converts an rgbcolor to a uint
unsigned int colortoint(struct rgbcolor color)
{
	return color.b + color.g * 0x100 + color.r * 0x10000;
}

//converts a uint to an rgbcolor
struct rgbcolor inttocolor(unsigned int color)
{
	struct rgbcolor ret;
	ret.b = (color)       & 0xff;
	ret.g = (color >> 8)  & 0xff;
	ret.r = (color >> 16) & 0xff;
	return ret;
}

//returns the equivalent gray for any byte value 0-0xff
struct rgbcolor graytocolor(byte gray)
{
	struct rgbcolor ret;
	ret.r = gray;
	ret.g = gray;
	ret.b = gray;
	return ret;
}

//clamps a float to 0-0xff
byte bclamp(float value)
{
	if(value < 0)
		return 0;
	else if(value > 0xff)
		return 0xff;
	return (byte)value;
}

//this function only *approximates* a hue shift
//don't use it for anything exact
//it's also not associative
struct rgbcolor shifthue(struct rgbcolor in, const float fHue)
{
	struct rgbcolor ret;
	const float cosA = cos(fHue*3.14159265f/180);
	const float sinA = sin(fHue*3.14159265f/180);
	//calculate the rotation matrix, only depends on Hue
	const float matrix[CHANNELS][CHANNELS] =
	{
		{
			     cosA + (1.0f - cosA) / 3.0f,
			1.0f/3.0f * (1.0f - cosA) - sqrtf(1.0f/3.0f) * sinA,
			1.0f/3.0f * (1.0f - cosA) + sqrtf(1.0f/3.0f) * sinA
		},
		{
			       1.0f/3.0f * (1.0f - cosA) + sqrtf(1.0f/3.0f) * sinA,
			cosA + 1.0f/3.0f * (1.0f - cosA),
			       1.0f/3.0f * (1.0f - cosA) - sqrtf(1.0f/3.0f) * sinA
		},
		{
			       1.0f/3.0f * (1.0f - cosA) - sqrtf(1.0f/3.0f) * sinA,
			       1.0f/3.0f * (1.0f - cosA) + sqrtf(1.0f/3.0f) * sinA,
			cosA + 1.0f/3.0f * (1.0f - cosA)
		}
	};

	//Use the rotation matrix to convert the RGB directly
	//this was prettier when rgbcolors were matricies and not structs
	//just substitute 0 = red etc in your head
	ret.r = bclamp(
		  in.r * matrix[0][0]
		+ in.g * matrix[0][1]
		+ in.b * matrix[0][2]
		);

	ret.g = bclamp(
		  in.r * matrix[1][0]
		+ in.g * matrix[1][1]
		+ in.b * matrix[1][2]
		);

	ret.b = bclamp(
		  in.r * matrix[2][0]
		+ in.g * matrix[2][1]
		+ in.b * matrix[2][2]
		);

	return ret;
}
