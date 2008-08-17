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

#include <cstdio>
#include <ctime>
#include <soundtouch4c.h>

#ifdef __cplusplus
extern "C" {
#endif

struct soundtouch *SoundTouch_construct()
{
	return (struct soundtouch *)(new SoundTouch());
}

void SoundTouch_destruct(struct soundtouch *st)
{
	delete (SoundTouch *)st;
}

void SoundTouch_setRate(struct soundtouch *st, float newRate)
{
	((SoundTouch *)st)->setRate(newRate);
}

void SoundTouch_setTempo(struct soundtouch *st, float newTempo)
{
	((SoundTouch *)st)->setTempo(newTempo);
}

void SoundTouch_setRateChange(struct soundtouch *st, float newRate)
{
	((SoundTouch *)st)->setRateChange(newRate);
}

void SoundTouch_setTempoChange(struct soundtouch *st, float newTempo)
{
	((SoundTouch *)st)->setTempoChange(newTempo);
}

void SoundTouch_setPitch(struct soundtouch *st, float newPitch)
{
	((SoundTouch *)st)->setPitch(newPitch);
}

void SoundTouch_setPitchOctaves(struct soundtouch *st, float newPitch)
{
	((SoundTouch *)st)->setPitchOctaves(newPitch);
}

void SoundTouch_setPitchSemiTonesInt(struct soundtouch *st, int newPitch)
{
	((SoundTouch *)st)->setPitchSemiTones(newPitch);
}

void SoundTouch_setPitchSemiTonesFloat(struct soundtouch *st, float newPitch)
{
	((SoundTouch *)st)->setPitchSemiTones(newPitch);
}

void SoundTouch_setChannels(struct soundtouch *st, uint numChannels)
{
	((SoundTouch *)st)->setChannels(numChannels);
}

void SoundTouch_setSampleRate(struct soundtouch *st, uint srate)
{
	((SoundTouch *)st)->setSampleRate(srate);
}

void SoundTouch_flush(struct soundtouch *st)
{
	((SoundTouch *)st)->flush();
}

void SoundTouch_putSamples(struct soundtouch *st, short *samples, uint numSamples)
{
	SAMPLETYPE buf[numSamples];
	uint n;
	for (n = 0; n < numSamples; n++)
		buf[n] = (SAMPLETYPE)samples[n];
	((SoundTouch *)st)->putSamples(buf, numSamples);
}

void SoundTouch_clear(struct soundtouch *st)
{
	((SoundTouch *)st)->clear();
}

int SoundTouch_setSetting(struct soundtouch *st, uint settingId, uint value)
{
	return ((SoundTouch *)st)->setSetting(settingId, value);
}

uint SoundTouch_getSetting(struct soundtouch *st, uint settingId)
{
	return ((SoundTouch *)st)->getSetting(settingId);
}

uint SoundTouch_numUnprocessedSamples(struct soundtouch *st)
{
	return ((SoundTouch *)st)->numUnprocessedSamples();
}

uint SoundTouch_receiveSamplesEx(struct soundtouch *st, short *output, uint maxSamples)
{
	SAMPLETYPE buf[maxSamples];
	uint n;
	uint c;
	c = ((SoundTouch *)st)->receiveSamples(buf, maxSamples);
	for (n = 0; n < c; n++)
		output[n] = (short)buf[n];
	return c;
}

uint SoundTouch_receiveSamples(struct soundtouch *st, uint maxSamples)
{
	return ((SoundTouch *)st)->receiveSamples(maxSamples);
}

uint SoundTouch_numSamples(struct soundtouch *st)
{
	return ((SoundTouch *)st)->numSamples();
}

int SoundTouch_isEmpty(struct soundtouch *st)
{
	return ((SoundTouch *)st)->isEmpty();
}

#ifdef __cplusplus
}
#endif
 
/** EMACS **
 * Local variables:
 * mode: c++
 * tab-width: 4
 * indent-tabs-mode: t
 * c-basic-offset: 4
 * End:
 */
