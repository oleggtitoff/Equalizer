#define _CRT_SECURE_NO_WARNINGS

#include "FilesOperations.h"
#include "BiquadFilter.h"

#define INPUT_FILE_NAME "Sine_-12_1s.wav"
#define OUTPUT_FILE_NAME "Output.wav"

#define BYTES_PER_SAMPLE 4
#define DATA_BUFF_SIZE 1000
#define SAMPLE_RATE 48000
#define CHANNELS 2

// FILTER TYPES
// lowpass = 1,
// highpass = 2,
// bandpass = 3,
// notch = 4,
// peak = 5,
// lowShelf = 6,
// highShelf = 7

// BAND 1
#define B1_IS_ACTIVE	1
#define	B1_FILTER_TYPE	2
#define B1_FC			2200
#define B1_Q_VALUE		0.707
#define B1_PEAK_GAIN	6

// BAND 2
#define B2_IS_ACTIVE	1
#define	B2_FILTER_TYPE	5
#define B2_FC			4380
#define B2_Q_VALUE		3
#define B2_PEAK_GAIN	9

// BAND 3
#define B3_IS_ACTIVE	1
#define	B3_FILTER_TYPE	5
#define B3_FC			6560
#define B3_Q_VALUE		3
#define B3_PEAK_GAIN	6

// BAND 4
#define B4_IS_ACTIVE	1
#define	B4_FILTER_TYPE	5
#define B4_FC			8740
#define B4_Q_VALUE		3
#define B4_PEAK_GAIN	2

// BAND 5
#define B5_IS_ACTIVE	1
#define	B5_FILTER_TYPE	5
#define B5_FC			10920
#define B5_Q_VALUE		3
#define B5_PEAK_GAIN	6

// BAND 6
#define B6_IS_ACTIVE	1
#define	B6_FILTER_TYPE	5
#define B6_FC			13100
#define B6_Q_VALUE		3
#define B6_PEAK_GAIN	6

// BAND 7
#define B7_IS_ACTIVE	1
#define	B7_FILTER_TYPE	5
#define B7_FC			15280
#define B7_Q_VALUE		3
#define B7_PEAK_GAIN	-6

// BAND 8
#define B8_IS_ACTIVE	1
#define	B8_FILTER_TYPE	5
#define B8_FC			17460
#define B8_Q_VALUE		3
#define B8_PEAK_GAIN	-6

// BAND 9
#define B9_IS_ACTIVE	1
#define	B9_FILTER_TYPE	5
#define B9_FC			19640
#define B9_Q_VALUE		3
#define B9_PEAK_GAIN	6

// BAND 10
#define B10_IS_ACTIVE	1
#define	B10_FILTER_TYPE 7
#define B10_FC			21820
#define B10_Q_VALUE		0.707
#define B10_PEAK_GAIN	-10


typedef struct {
	int sampleRate;
	BiquadParams biquadParams[10];
} Params;

typedef struct {
	int sampleRate;
	BiquadCoeffsDouble biquadCoeffsDouble[10];
	BiquadCoeffs biquadCoeffs[10];
} Coeffs;

typedef struct {
	BiquadStatesDouble biquadStatesDouble[10];
	BiquadStates biquadStates[10];
} States;


ALWAYS_INLINE void init(Params *params, States *states);
ALWAYS_INLINE void setParams(Params *params);
ALWAYS_INLINE void calcCoeffs(const Params *params, Coeffs *coeffs);
void run(FILE *inputFilePtr, FILE *outputFilePtr, const Coeffs *coeffs, States *states);

int main()
{
	FILE *inputFilePtr = openFile(INPUT_FILE_NAME, 0);
	FILE *outputFilePtr = openFile(OUTPUT_FILE_NAME, 1);
	uint8_t headerBuff[FILE_HEADER_SIZE];
	States states;
	Params params;
	Coeffs coeffs;

	init(&params, &states);
	setParams(&params);
	calcCoeffs(&params, &coeffs);

	readHeader(headerBuff, inputFilePtr);
	writeHeader(headerBuff, outputFilePtr);
	run(inputFilePtr, outputFilePtr, &coeffs, &states);

	fclose(inputFilePtr);
	fclose(outputFilePtr);
	return 0;
}


ALWAYS_INLINE void initParams(Params *params)
{
	for (int i = 0; i < 10; i++)
	{
		initBiquadParams(&params->biquadParams[i]);
	}
}

ALWAYS_INLINE void initStates(States *states)
{
	for (int i = 0; i < 10; i++)
	{
		initBiquadStates(&states->biquadStates[i]);
		initBiquadStatesDouble(&states->biquadStatesDouble[i]);
	}
}

ALWAYS_INLINE void init(Params *params, States *states)
{
	initParams(params);
	initStates(states);
}

ALWAYS_INLINE void setParams(Params *params)
{
	params->sampleRate = SAMPLE_RATE;

	setBiquadParam(&params->biquadParams[0], isActiveID, B1_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[0], filterTypeID, B1_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[0], FcID, B1_FC);
	setBiquadParam(&params->biquadParams[0], Qid, B1_Q_VALUE);
	setBiquadParam(&params->biquadParams[0], peakGain, B1_PEAK_GAIN);

	setBiquadParam(&params->biquadParams[1], isActiveID, B2_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[1], filterTypeID, B2_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[1], FcID, B2_FC);
	setBiquadParam(&params->biquadParams[1], Qid, B2_Q_VALUE);
	setBiquadParam(&params->biquadParams[1], peakGain, B2_PEAK_GAIN);

	setBiquadParam(&params->biquadParams[2], isActiveID, B3_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[2], filterTypeID, B3_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[2], FcID, B3_FC);
	setBiquadParam(&params->biquadParams[2], Qid, B3_Q_VALUE);
	setBiquadParam(&params->biquadParams[2], peakGain, B3_PEAK_GAIN);

	setBiquadParam(&params->biquadParams[3], isActiveID, B4_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[3], filterTypeID, B4_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[3], FcID, B4_FC);
	setBiquadParam(&params->biquadParams[3], Qid, B4_Q_VALUE);
	setBiquadParam(&params->biquadParams[3], peakGain, B4_PEAK_GAIN);

	setBiquadParam(&params->biquadParams[4], isActiveID, B5_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[4], filterTypeID, B5_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[4], FcID, B5_FC);
	setBiquadParam(&params->biquadParams[4], Qid, B5_Q_VALUE);
	setBiquadParam(&params->biquadParams[4], peakGain, B5_PEAK_GAIN);

	setBiquadParam(&params->biquadParams[5], isActiveID, B6_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[5], filterTypeID, B6_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[5], FcID, B6_FC);
	setBiquadParam(&params->biquadParams[5], Qid, B6_Q_VALUE);
	setBiquadParam(&params->biquadParams[5], peakGain, B6_PEAK_GAIN);

	setBiquadParam(&params->biquadParams[6], isActiveID, B7_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[6], filterTypeID, B7_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[6], FcID, B7_FC);
	setBiquadParam(&params->biquadParams[6], Qid, B7_Q_VALUE);
	setBiquadParam(&params->biquadParams[6], peakGain, B7_PEAK_GAIN);

	setBiquadParam(&params->biquadParams[7], isActiveID, B8_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[7], filterTypeID, B8_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[7], FcID, B8_FC);
	setBiquadParam(&params->biquadParams[7], Qid, B8_Q_VALUE);
	setBiquadParam(&params->biquadParams[7], peakGain, B8_PEAK_GAIN);

	setBiquadParam(&params->biquadParams[8], isActiveID, B9_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[8], filterTypeID, B9_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[8], FcID, B9_FC);
	setBiquadParam(&params->biquadParams[8], Qid, B9_Q_VALUE);
	setBiquadParam(&params->biquadParams[8], peakGain, B9_PEAK_GAIN);

	setBiquadParam(&params->biquadParams[9], isActiveID, B10_IS_ACTIVE);
	setBiquadParam(&params->biquadParams[9], filterTypeID, B10_FILTER_TYPE);
	setBiquadParam(&params->biquadParams[9], FcID, B10_FC);
	setBiquadParam(&params->biquadParams[9], Qid, B10_Q_VALUE);
	setBiquadParam(&params->biquadParams[9], peakGain, B10_PEAK_GAIN);
}

ALWAYS_INLINE void calcCoeffs(const Params *params, Coeffs *coeffs)
{
	coeffs->sampleRate = params->sampleRate;

	for (int i = 0; i < 10; i++)
	{
		calcBiquadCoeffs(&params->biquadParams[i], params->sampleRate, &coeffs->biquadCoeffs[i], &coeffs->biquadCoeffsDouble[i]);
	}
}

void run(FILE *inputFilePtr, FILE *outputFilePtr, const Coeffs *coeffs, States *states)
{
	int32_t dataBuff[DATA_BUFF_SIZE * CHANNELS];
	size_t samplesRead;
	uint32_t i;

	while (1)
	{
		samplesRead = fread(dataBuff, BYTES_PER_SAMPLE, DATA_BUFF_SIZE, inputFilePtr);

		if (!samplesRead)
		{
			break;
		}

		for (i = 0; i < samplesRead / CHANNELS; i++)
		{
			int32_t sample = dataBuff[i * CHANNELS + 1];
			
			for (int j = 0; j < 10; j++)
			{
				sample = biquadDoubleFilter(sample, &coeffs->biquadCoeffsDouble[j], &states->biquadStatesDouble[j]);
			}

			dataBuff[i * CHANNELS + 1] = sample;
		}

		fwrite(dataBuff, BYTES_PER_SAMPLE, samplesRead, outputFilePtr);
	}
}