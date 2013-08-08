/*
 * soundtouch4c (A wrapper for soundtouch so you can use it in C programs)
 * Copyright (c) 2006-2007 Justin Tunney and Anthony Minessale II
 *
 * Justin Tunney <jtunney@gmail.com>
 * Anthony Minessale II <anthmct@yahoo.com>
 *
 * Here we are implementing a basic wrapper around the SoundTouch
 * library.  Because SoundTouch likes to be tricky and switch between
 * float and short sample types, we're going to provide an interface
 * for shorts only, and translate to the SAMPLETYPE defined by
 * STTypes.h.  This way we shouldn't get that static garbage.
 *
 */

#ifdef  WITH_SOUNDTOUCH
#include <cstdio>
#include <ctime>
#include <soundtouch4c.h>

#ifdef __cplusplus
extern "C" {
#endif

struct stouch *SoundTouch_construct()
{
	return (struct stouch *)(new SoundTouch());
}

void SoundTouch_destruct(struct stouch *st)
{
	delete (SoundTouch *)st;
}

void SoundTouch_setRate(struct stouch *st, float newRate)
{
	((SoundTouch *)st)->setRate(newRate);
}

void SoundTouch_setTempo(struct stouch *st, float newTempo)
{
	((SoundTouch *)st)->setTempo(newTempo);
}

void SoundTouch_setRateChange(struct stouch *st, float newRate)
{
	((SoundTouch *)st)->setRateChange(newRate);
}

void SoundTouch_setTempoChange(struct stouch *st, float newTempo)
{
	((SoundTouch *)st)->setTempoChange(newTempo);
}

void SoundTouch_setPitch(struct stouch *st, float newPitch)
{
	((SoundTouch *)st)->setPitch(newPitch);
}

void SoundTouch_setPitchOctaves(struct stouch *st, float newPitch)
{
	((SoundTouch *)st)->setPitchOctaves(newPitch);
}

void SoundTouch_setPitchSemiTonesInt(struct stouch *st, int newPitch)
{
	((SoundTouch *)st)->setPitchSemiTones(newPitch);
}

void SoundTouch_setPitchSemiTonesFloat(struct stouch *st, float newPitch)
{
	((SoundTouch *)st)->setPitchSemiTones(newPitch);
}

void SoundTouch_setChannels(struct stouch *st, uint numChannels)
{
	((SoundTouch *)st)->setChannels(numChannels);
}

void SoundTouch_setSampleRate(struct stouch *st, uint srate)
{
	((SoundTouch *)st)->setSampleRate(srate);
}

void SoundTouch_flush(struct stouch *st)
{
	((SoundTouch *)st)->flush();
}

void SoundTouch_putSamples(struct stouch *st, short *samples, uint numSamples)
{
	SAMPLETYPE buf[numSamples];
	uint n;
	for (n = 0; n < numSamples; n++)
		buf[n] = (SAMPLETYPE)samples[n];
	((SoundTouch *)st)->putSamples(buf, numSamples);
}

void SoundTouch_clear(struct stouch *st)
{
	((SoundTouch *)st)->clear();
}

int SoundTouch_setSetting(struct stouch *st, uint settingId, uint value)
{
	return ((SoundTouch *)st)->setSetting(settingId, value);
}

uint SoundTouch_getSetting(struct stouch *st, uint settingId)
{
	return ((SoundTouch *)st)->getSetting(settingId);
}

uint SoundTouch_numUnprocessedSamples(struct stouch *st)
{
	return ((SoundTouch *)st)->numUnprocessedSamples();
}

uint SoundTouch_receiveSamplesEx(struct stouch *st, short *output, uint maxSamples)
{
	SAMPLETYPE buf[maxSamples];
	uint n;
	uint c;
	c = ((SoundTouch *)st)->receiveSamples(buf, maxSamples);
	for (n = 0; n < c; n++)
		output[n] = (short)buf[n];
	return c;
}

uint SoundTouch_receiveSamples(struct stouch *st, uint maxSamples)
{
	return ((SoundTouch *)st)->receiveSamples(maxSamples);
}

uint SoundTouch_numSamples(struct stouch *st)
{
	return ((SoundTouch *)st)->numSamples();
}

int SoundTouch_isEmpty(struct stouch *st)
{
	return ((SoundTouch *)st)->isEmpty();
}

#ifdef __cplusplus
}
#endif
 #endif
/** EMACS **
 * Local variables:
 * mode: c++
 * tab-width: 4
 * indent-tabs-mode: t
 * c-basic-offset: 4
 * End:
 */
