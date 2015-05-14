//
// Coherent noise function over 1, 2 or 3 dimensions
// (copyright Ken Perlin)
//

#include <stdlib.h>
#include <cmath>
#include <GL/glew.h>

#define MAXB 0x100
#define N 0x1000
#define NP 12   // 2^N
#define NM 0xfff

#define s_curve(t) ( t * t * (3. - 2. * t) )
#define lerp(t, a, b) ( a + t * (b - a) )
#define setup(i, b0, b1, r0, r1)\
        t = vec[i] + N;\
        b0 = ((int)t) & BM;\
        b1 = (b0+1) & BM;\
        r0 = t - (int)t;\
        r1 = r0 - 1.;
#define at2(rx, ry) ( rx * q[0] + ry * q[1] )
#define at3(rx, ry, rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

class Noise
{



private:

	int p[MAXB + MAXB + 2];
	double g3[MAXB + MAXB + 2][3];
	double g2[MAXB + MAXB + 2][2];
	double g1[MAXB + MAXB + 2];

	int Noise3DTexSize;
	GLubyte* Noise3DTexPtr;
	GLuint noise3DTexName;

	int start;
	int B;
	int BM;

public:

	Noise()
	{
		Noise3DTexSize = 64;
		noise3DTexName = 0;
	}

	void initNoise();
	void init3DNoiseTexture(int texSize, GLubyte* texPtr);
	void make3DNoiseTexture();
	void SetNoiseFrequency(int frequency);
	void CreateNoise3D();
	double noise1(double arg);
	double noise2(double vec[2]);
	double noise3(double vec[3]);
	void normalize2(double v[2]);
	void normalize3(double v[3]);
	double PerlinNoise1D(double x,double alpha,double beta,int n);
	double PerlinNoise2D(double x, double y, double alpha, double beta, int n);
	double PerlinNoise3D(double x, double y, double z, double alpha, double beta, int n);
	void init3DNoiseTexture();


};