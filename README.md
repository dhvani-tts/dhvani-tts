dhvani-tts
==========

dhvani is a text to speech system designed for Indian Languages. The aim of this project is to ensure that literacy and knowledge of English are not essential for using a Computer. We hope that it will reduce the digital divide and will be helpful for the visually challenged users as screenreader in their mother tongue .

Check the site : http://dhvani.sourceforge.net/

Compiling
=========

Prerequisites
-------------

For compiling, you need the following dev libraries.

* GSM Sound compression library - Install `libgsm1`, `libgsm1-dev` packages using your package manager (see http://www.quut.com/gsm/ for source code. When compiling the source, change line 47 of Makefile to have -fPIC flag in the CCFLAGS variable.)
* ALSA development libraries - For sound playback- Install `libasound-dev` package using your package manager
* Ogg vorbis dev libraries - For oggvoribis encoding of the generated speech- Install `libvorbis-dev` package using your package manager. Required only for compiling with `--with-vorbis`
* Soundtouch dev libraries - For applying pitch and tempo. Install libsoundtouch-dev using your packagemanager. Required only for compiling with `--with-soundtouch`

The default steps for building is

```bash
# If gsm is not installed in a standard directory (say in $HOME)
export CFLAGS="-I$HOME/gsm-1.0-pl13/inc"
export CXXFLAGS=$CFLAGS
export LDFLAGS="-L$HOME/gsm-1.0-pl13/lib"
# Now run autogen.sh
./autogen.sh [--with-vorbis --with-gstreamer --with-soundtouch --enable-debugging]
make
sudo make install
```

For mac OSX, do the following before running autogen.sh (--with-gstreamer is mandatory as ALSA is not available on Mac): 

Install GStreamer dev and runtime libs using the instructions on (source code http://gstreamer.freedesktop.org/src/) http://docs.gstreamer.com/display/GstSDK/Installing+on+Mac+OS+X

Install libvorbis-dev (http://forums.tigsource.com/index.php?action=printpage;topic=21951.0 and http://www.xiph.org/downloads/) and libgsm1-dev (http://www.finkproject.org/doc/bundled/install-fast.php)

```bash
export LIBTOOLIZE=glibtoolize GSTREAMER_CFLAGS=-I/Library/Frameworks/GStreamer.framework/Headers GSTREAMER_LIBS="-L/Library/Frameworks/GStreamer.framework/Libraries -lgstreamer-0.10"
```

Usage
=====

Pass the file name with the content to read

```bash
dhvani textfile
dhvani -l hi -f ogg tdemos/Hindi
dhvani -l hi -f ogg -s +100 tdemos/Hindi
dhvani -l hi -f ogg -s +100 -p +5 tdemos/Hindi
dhvani -l kn -f ogg -s +100 -p +15 tdemos/Kannada
```

There are many other options, for them see http://dhvani.sourceforge.net/doc/usage.html


