/*
 * soundtouch4c (A wrapper for soundtouch so you can use it in C programs)
 * Copyright (c) 2006-2007 Justin Tunney and Anthony Minessale II
 *
 * Justin Tunney <jtunney@gmail.com>
 * Anthony Minessale II <anthmct@yahoo.com>
 *
 * Version: 0.4.0
 * (Versions before 0.4.0 do not include the above line)
 *
 */

#ifdef  WITH_SOUNDTOUCH
#ifndef SOUNDTOUCH4C_H
#define SOUNDTOUCH4C_H
#include <sys/types.h>

#ifdef __cplusplus

#include <stdexcept>
#include <stdio.h>
#include <SoundTouch.h>
using namespace soundtouch;
using namespace std;

extern "C" {

#else 

typedef short SAMPLETYPE;
#define SETTING_USE_AA_FILTER       0
#define SETTING_AA_FILTER_LENGTH    1
#define SETTING_USE_QUICKSEEK       2
#define SETTING_SEQUENCE_MS         3
#define SETTING_SEEKWINDOW_MS       4
#define SETTING_OVERLAP_MS          5

#endif /* #ifdef __cplusplus */

struct stouch;

struct stouch *SoundTouch_construct(void);
void SoundTouch_destruct(struct stouch *st);
void SoundTouch_setRate(struct stouch *st, float newRate);
void SoundTouch_setTempo(struct stouch *st, float newTempo);
void SoundTouch_setRateChange(struct stouch *st, float newRate);
void SoundTouch_setTempoChange(struct stouch *st, float newTempo);
void SoundTouch_setPitch(struct stouch *st, float newPitch);
void SoundTouch_setPitchOctaves(struct stouch *st, float newPitch);
void SoundTouch_setPitchSemiTonesInt(struct stouch *st, int newPitch);
void SoundTouch_setPitchSemiTonesFloat(struct stouch *st, float newPitch);
void SoundTouch_setChannels(struct stouch *st, unsigned int numChannels);
void SoundTouch_setSampleRate(struct stouch *st, unsigned int srate);
void SoundTouch_flush(struct stouch *st);
void SoundTouch_putSamples(struct stouch *st, short *samples, unsigned int numSamples);
void SoundTouch_clear(struct stouch *st);
int  SoundTouch_setSetting(struct stouch *st, unsigned int settingId, unsigned int value);
unsigned int SoundTouch_getSetting(struct stouch *st, unsigned int settingId);
unsigned int SoundTouch_numUnprocessedSamples(struct stouch *st);
unsigned int SoundTouch_receiveSamplesEx(struct stouch *st, short *output, unsigned int maxSamples);
unsigned int SoundTouch_receiveSamples(struct stouch *st, unsigned int maxSamples);
unsigned int SoundTouch_numSamples(struct stouch *st);
int  SoundTouch_isEmpty(struct stouch *st);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SOUNDTOUCH4C_H */
 #endif
/** EMACS **
 * Local variables:
 * mode: c++
 * tab-width: 4
 * indent-tabs-mode: t
 * c-basic-offset: 4
 * End:
 */
