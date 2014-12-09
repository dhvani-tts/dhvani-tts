dhvani-tts
==========

dhvani is a text to speech system designed for Indian Languages. The aim of this project is to ensure that literacy and knowledge of English are not essential for using a Computer. We hope that it will reduce the digital divide and will be helpful for the visually challenges users as screenreader in their mother tongue .

Check the site : http://dhvani.sourceforge.net/

Compiling
=========

Prerequisites
-------------

For compiling, you need the following dev libraries.

* GSM Sound compression library - Install `libgsm1`, `libgsm1-dev` packages using your package manager
* ALSA development libraries - For sound playback- Install `libasound-dev` package using your package manager
* Ogg vorbis dev libraries - For oggvoribis encoding of the generated speech- Install `libvorbis-dev` package using your package manager. Required only for compiling with `--with-vorbis`
* Soundtouch dev libraries - For applying pitch and tempo. Install libsoundtouch-dev using your packagemanager. Required only for compiling with `--with-soundtouch`

The default steps for building is

```bash
./autogen.sh 
make
sudo make install
```

For mac, run the following before running autogen.sh: 

export LIBTOOLIZE=glibtoolize GSTREAMER_CFLAGS=-I/Library/Frameworks/GStreamer.framework/Headers GSTREAMER_LIBS=-L/Library/Frameworks/GStreamer.framework/Libraries

Usage
=====

Pass the file name with the content to read

```bash
dhvani textfile
```

There are many other options, for them see http://dhvani.sourceforge.net/doc/usage.html


