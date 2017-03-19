/*

 Copyright 2016 Christian Hoene, Symonics GmbH

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 */

#ifndef MYSOFA_H_INCLUDED
#define MYSOFA_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** attributes */
struct MYSOFA_ATTRIBUTE {
	struct MYSOFA_ATTRIBUTE *next;
	char *name;
	char *value;
};

struct MYSOFA_ARRAY {
	float *values;
	unsigned int elements;
	struct MYSOFA_ATTRIBUTE *attributes;
};

/*
 * The HRTF structure data types
 */
struct MYSOFA_HRTF {

	/* Dimensions defined in AES69 
	 M Number of measurements; must be integer greater than zero.
	 R Number of receivers; must be integer greater than zero.
	 E Number of emitters; must be integer greater than zero.
	 N Number of data samples describing one measurement; must be integer greater than zero.
	 S Number of characters in a string; must be integer greater than zero.
	 I 1 Singleton dimension, defines a scalar value.
	 C 3 Coordinate triplet, always three; the coordinate type defines the meaning of this dimension.
	 */
	unsigned I, C, R, E, N, M;

	struct MYSOFA_ARRAY ListenerPosition;

	struct MYSOFA_ARRAY ReceiverPosition;

	struct MYSOFA_ARRAY SourcePosition;

	struct MYSOFA_ARRAY EmitterPosition;

	struct MYSOFA_ARRAY ListenerUp;

	struct MYSOFA_ARRAY ListenerView;

	/** array of filter coefficients. Sizes are filters*filter_length. */
	struct MYSOFA_ARRAY DataIR;

	/** the sampling rate used in this structure */
	struct MYSOFA_ARRAY DataSamplingRate;

	/** array of min-phase delays. Sizes are filters */
	struct MYSOFA_ARRAY DataDelay;

	/** general file attributes */
	struct MYSOFA_ATTRIBUTE *attributes;
};

/* structure for lookup HRTF filters */
struct MYSOFA_LOOKUP {
	void *kdtree;
	float radius_min, radius_max;
};

struct MYSOFA_NEIGHBORHOOD {
	int elements;
	int *index;
};

enum {
	MYSOFA_OK = 0,
	MYSOFA_INVALID_FORMAT = 10000,
	MYSOFA_UNSUPPORTED_FORMAT,
	MYSOFA_INTERNAL_ERROR,
	MYSOFA_NO_MEMORY,
	MYSOFA_READ_ERROR
};

struct MYSOFA_HRTF* mysofa_load(const char *filename, int *err);

int mysofa_check(struct MYSOFA_HRTF *hrtf);
void mysofa_tospherical(struct MYSOFA_HRTF *hrtf);
void mysofa_tocartesian(struct MYSOFA_HRTF *hrtf);
void mysofa_free(struct MYSOFA_HRTF *hrtf);

struct MYSOFA_LOOKUP* mysofa_lookup_init(struct MYSOFA_HRTF *hrtf);
int mysofa_lookup(struct MYSOFA_LOOKUP *lookup, float *coordinate);
void mysofa_lookup_free(struct MYSOFA_LOOKUP *lookup);

struct MYSOFA_NEIGHBORHOOD *mysofa_neighborhood_init(struct MYSOFA_HRTF *hrtf,
		struct MYSOFA_LOOKUP *lookup);
int* mysofa_neighborhood(struct MYSOFA_NEIGHBORHOOD *neighborhood, int pos);
void mysofa_neighborhood_free(struct MYSOFA_NEIGHBORHOOD *neighborhood);

float* mysofa_interpolate(struct MYSOFA_HRTF *hrtf, float *cordinate,
		int nearest, int *neighborhood, float *fir, float *delays);

int mysofa_resample(struct MYSOFA_HRTF *hrtf, float samplerate);
float mysofa_loudness(struct MYSOFA_HRTF *hrtf);
int mysofa_minphase(struct MYSOFA_HRTF *hrtf, float threshold);

struct MYSOFA_EASY *mysofa_cache_lookup(const char *filename, float samplerate);
struct MYSOFA_EASY *mysofa_cache_store(struct MYSOFA_EASY *, const char *filename, float samplerate);
void mysofa_cache_release(struct MYSOFA_EASY *);
void mysofa_cache_release_all(void);

void mysofa_c2s(float values[3]);
void mysofa_s2c(float values[3]);

struct MYSOFA_EASY {
	struct MYSOFA_HRTF *hrtf;
	struct MYSOFA_LOOKUP *lookup;
	struct MYSOFA_NEIGHBORHOOD *neighborhood;
};

struct MYSOFA_EASY* mysofa_open(const char *filename, float samplerate, int *filterlength, int *err);
struct MYSOFA_EASY* mysofa_open_cached(const char *filename, float samplerate, int *filterlength, int *err);
void mysofa_getfilter_short(struct MYSOFA_EASY* easy, float x, float y, float z,
		short *IRleft, short *IRright,
		int *delayLeft, int *delayRight);
void mysofa_getfilter_float(struct MYSOFA_EASY* easy, float x, float y, float z,
		float *IRleft, float *IRright,
		float *delayLeft, float *delayRight);
void mysofa_close(struct MYSOFA_EASY* easy);
void mysofa_close_cached(struct MYSOFA_EASY* easy);


#ifdef __cplusplus
}
#endif
#endif
