#ifndef BIQUAD_FILTER
#define BIQUAD_FILTER

#include <math.h>

#define PI	  3.14159265358979323846
#define SQRT2 1.41421356237309504880

typedef enum {
	lowpass	  = 1,
	highpass  = 2,
	bandpass  = 3,
	notch	  = 4,
	peak	  = 5,
	lowShelf  = 6,
	highShelf = 7
} BiquadType;

typedef enum {
	isActiveID	 = 1,
	filterTypeID = 2,
	FcID		 = 3,
	Qid			 = 4,
	peakGain	 = 5
} BiquadParamID;

typedef struct {
	uint8_t isActive;
	int type;
	double Fc;
	double Q;
	double peakGain;
} BiquadParams;

typedef struct {
	uint8_t isActive;
	int32_t a[3];
	int32_t b[2];
} BiquadCoeffs;

typedef struct {
	uint8_t isActive;
	double a[3];
	double b[2];
} BiquadCoeffsDouble;

typedef struct {
	int32_t x[2];
	int32_t y[2];
	uint32_t err;
} BiquadStates;

typedef struct {
	double x[2];
	double y[2];
} BiquadStatesDouble;


int32_t doubleToFixed31(double x)
{
	if (x >= 1)
	{
		return INT32_MAX;
	}
	else if (x < -1)
	{
		return INT32_MIN;
	}

	return (int32_t)(x * (double)(1LL << 31));
}

ALWAYS_INLINE void initBiquadStatesDouble(BiquadStatesDouble *biquadStates)
{
	biquadStates->x[0] = 0;
	biquadStates->x[1] = 0;
	biquadStates->y[0] = 0;
	biquadStates->y[1] = 0;
}

ALWAYS_INLINE void initBiquadStates(BiquadStates *biquadStates)
{
	biquadStates->x[0] = 0;
	biquadStates->x[1] = 0;
	biquadStates->y[0] = 0;
	biquadStates->y[1] = 0;
	biquadStates->err = 0;
}

ALWAYS_INLINE void initBiquadParams(BiquadParams *biquadParams)
{
	biquadParams->isActive = 0;
	biquadParams->type = 1;
	biquadParams->Fc = 0;
	biquadParams->Q = 0;
	biquadParams->peakGain;
}

ALWAYS_INLINE void setBiquadParam(BiquadParams *biquadParams, const uint16_t id, const double value)
{
	switch (id)
	{
	case isActiveID:
		biquadParams->isActive = (int)value;
		break;

	case filterTypeID:
		biquadParams->type = (int)value;
		break;

	case FcID:
		biquadParams->Fc = value;
		break;

	case Qid:
		biquadParams->Q = value;
		break;

	case peakGain:
		biquadParams->peakGain = value;
		break;
	}
}

ALWAYS_INLINE void calcBiquadCoeffs(const BiquadParams *biquadParams, const int sampleRate, BiquadCoeffs *biquadCoeffs, BiquadCoeffsDouble *biquadCoeffsDouble)
{
	biquadCoeffs->isActive = biquadParams->isActive;
	biquadCoeffsDouble->isActive = biquadParams->isActive;

	double norm;
	double V = pow(10, fabs(biquadParams->peakGain) / 20);
	double K = tan(PI * biquadParams->Fc / sampleRate);
	double Kpow2 = K * K;

	switch (biquadParams->type)
	{
	case lowpass:
		norm = 1 / (1 + K / biquadParams->Q + Kpow2);
		biquadCoeffsDouble->a[0] = Kpow2 * norm;
		biquadCoeffsDouble->a[1] = 2 * biquadCoeffsDouble->a[0];
		biquadCoeffsDouble->a[2] = biquadCoeffsDouble->a[0];
		biquadCoeffsDouble->b[0] = 2 * (Kpow2 - 1) * norm;
		biquadCoeffsDouble->b[1] = (1 - K / biquadParams->Q + Kpow2) * norm;
		break;

	case highpass:
		norm = 1 / (1 + K / biquadParams->Q + Kpow2);
		biquadCoeffsDouble->a[0] = norm;
		biquadCoeffsDouble->a[1] = -2 * biquadCoeffsDouble->a[0];
		biquadCoeffsDouble->a[2] = biquadCoeffsDouble->a[0];
		biquadCoeffsDouble->b[0] = 2 * (Kpow2 - 1) * norm;
		biquadCoeffsDouble->b[1] = (1 - K / biquadParams->Q + Kpow2) * norm;
		break;

	case bandpass:
		norm = 1 / (1 + K / biquadParams->Q + Kpow2);
		biquadCoeffsDouble->a[0] = K / biquadParams->Q * norm;
		biquadCoeffsDouble->a[1] = 0;
		biquadCoeffsDouble->a[2] = -biquadCoeffsDouble->a[0];
		biquadCoeffsDouble->b[0] = 2 * (Kpow2 - 1) * norm;
		biquadCoeffsDouble->b[1] = (1 - K / biquadParams->Q + Kpow2) * norm;
		break;

	case notch:
		norm = 1 / (1 + K / biquadParams->Q + Kpow2);
		biquadCoeffsDouble->a[0] = (1 + Kpow2) * norm;
		biquadCoeffsDouble->a[1] = 2 * (Kpow2 - 1) * norm;
		biquadCoeffsDouble->a[2] = biquadCoeffsDouble->a[0];
		biquadCoeffsDouble->b[0] = biquadCoeffsDouble->a[1];
		biquadCoeffsDouble->b[1] = (1 - K / biquadParams->Q + Kpow2) * norm;
		break;

	case peak:
		if (biquadParams->peakGain >= 0)
		{
			norm = 1 / (1 + 1 / biquadParams->Q * K + Kpow2);
			biquadCoeffsDouble->a[0] = (1 + V / biquadParams->Q * K + Kpow2) * norm;
			biquadCoeffsDouble->a[1] = 2 * (Kpow2 - 1) * norm;
			biquadCoeffsDouble->a[2] = (1 - V / biquadParams->Q * K + Kpow2) * norm;
			biquadCoeffsDouble->b[0] = biquadCoeffsDouble->a[1];
			biquadCoeffsDouble->b[1] = (1 - 1 / biquadParams->Q * K + Kpow2) * norm;
		}
		else
		{
			norm = 1 / (1 + V / biquadParams->Q * K + Kpow2);
			biquadCoeffsDouble->a[0] = (1 + 1 / biquadParams->Q * K + Kpow2) * norm;
			biquadCoeffsDouble->a[1] = 2 * (Kpow2 - 1) * norm;
			biquadCoeffsDouble->a[2] = (1 - 1 / biquadParams->Q * K + Kpow2) * norm;
			biquadCoeffsDouble->b[0] = biquadCoeffsDouble->a[1];
			biquadCoeffsDouble->b[1] = (1 - V / biquadParams->Q * K + Kpow2) * norm;
		}
		break;

	case lowShelf:
		if (biquadParams->peakGain >= 0)
		{
			norm = 1 / (1 + SQRT2 * K + Kpow2);
			biquadCoeffsDouble->a[0] = (1 + sqrt(2 * V) * K + V * Kpow2) * norm;
			biquadCoeffsDouble->a[1] = 2 * (V * Kpow2 - 1) * norm;
			biquadCoeffsDouble->a[2] = (1 - sqrt(2 * V) * K + V * Kpow2) * norm;
			biquadCoeffsDouble->b[0] = 2 * (Kpow2 - 1) * norm;
			biquadCoeffsDouble->b[1] = (1 - SQRT2 * K + Kpow2) * norm;
		}
		else
		{
			norm = 1 / (1 + sqrt(2 * V) * K + V * Kpow2);
			biquadCoeffsDouble->a[0] = (1 + SQRT2 * K + Kpow2) * norm;
			biquadCoeffsDouble->a[1] = 2 * (Kpow2 - 1) * norm;
			biquadCoeffsDouble->a[2] = (1 - SQRT2 * K + Kpow2) * norm;
			biquadCoeffsDouble->b[0] = 2 * (V * Kpow2 - 1) * norm;
			biquadCoeffsDouble->b[1] = (1 - sqrt(2 * V) * K + V * Kpow2) * norm;
		}
		break;

	case highShelf:
		if (biquadParams->peakGain >= 0)
		{
			norm = 1 / (1 + SQRT2 * K + Kpow2);
			biquadCoeffsDouble->a[0] = (V + sqrt(2 * V) * K + Kpow2) * norm;
			biquadCoeffsDouble->a[1] = 2 * (Kpow2 - V) * norm;
			biquadCoeffsDouble->a[2] = (V - sqrt(2 * V) * K + Kpow2) * norm;
			biquadCoeffsDouble->b[0] = 2 * (Kpow2 - 1) * norm;
			biquadCoeffsDouble->b[1] = (1 - SQRT2 * K + Kpow2) * norm;
		}
		else
		{
			norm = 1 / (V + sqrt(2 * V) * K + Kpow2);
			biquadCoeffsDouble->a[0] = (1 + SQRT2 * K + Kpow2) * norm;
			biquadCoeffsDouble->a[1] = 2 * (Kpow2 - 1) * norm;
			biquadCoeffsDouble->a[2] = (1 - SQRT2 * K + Kpow2) * norm;
			biquadCoeffsDouble->b[0] = 2 * (Kpow2 - V) * norm;
			biquadCoeffsDouble->b[1] = (V - sqrt(2 * V) * K + Kpow2) * norm;
		}
	}

	biquadCoeffs->a[0] = doubleToFixed31(biquadCoeffsDouble->a[0] / 16);
	biquadCoeffs->a[1] = doubleToFixed31(biquadCoeffsDouble->a[1] / 16);
	biquadCoeffs->a[2] = doubleToFixed31(biquadCoeffsDouble->a[2] / 16);
	biquadCoeffs->b[0] = doubleToFixed31(biquadCoeffsDouble->b[0] / 16);
	biquadCoeffs->b[1] = doubleToFixed31(biquadCoeffsDouble->b[1] / 16);
}

ALWAYS_INLINE int32_t biquadDoubleFilter(const int32_t sample, const BiquadCoeffsDouble *coeffs, BiquadStatesDouble *states)
{
	if (!coeffs->isActive)
	{
		return sample;
	}

	double acc = coeffs->a[0] * sample + coeffs->a[1] * states->x[0] + coeffs->a[2] * states->x[1] - coeffs->b[0] * states->y[0] - coeffs->b[1] * states->y[1];

	states->x[1] = states->x[0];
	states->x[0] = sample;
	states->y[1] = states->y[0];
	states->y[0] = acc;

	return (int32_t)acc;
}

ALWAYS_INLINE int32_t biquadFilter(const int32_t sample, const BiquadCoeffs *coeffs, BiquadStates *states)
{
	//int64_t acc = states->err;

	//acc += (int64_t)coeffs->a[0] * sample + (int64_t)coeffs->a[1] * states->x[0] + (int64_t)coeffs->a[2] * states->x[1] - (int64_t)coeffs->b[0] * states->y[0] - (int64_t)coeffs->b[1] * states->y[1];

	//states->x[1] = states->x[0];
	//states->x[0] = sample;
	//states->y[1] = states->y[0];
	//states->y[0] = (int32_t)(acc >> 27);

	//states->err = (uint32_t)acc & 0x3fffffff;

	//return states->y[0] < 4;
}

#endif /* BIQUAD_FILTER */