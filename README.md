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

<b>For mac OSX</b>, do the following before running autogen.sh (<b>--with-gstreamer</b> is mandatory as ALSA is not available on Mac): 

Install GStreamer dev and runtime libs using the instructions on (source code http://gstreamer.freedesktop.org/src/) http://docs.gstreamer.com/display/GstSDK/Installing+on+Mac+OS+X

Install libvorbis-dev (http://forums.tigsource.com/index.php?action=printpage;topic=21951.0 and http://www.xiph.org/downloads/) and libgsm1-dev (http://www.finkproject.org/doc/bundled/install-fast.php)

```bash
export LIBTOOLIZE=glibtoolize GSTREAMER_CFLAGS=-I/Library/Frameworks/GStreamer.framework/Headers GSTREAMER_LIBS="-L/Library/Frameworks/GStreamer.framework/Libraries -lgstreamer-0.10"
```

<b>For Windows</b>, a pre-generated Makefile.win has been modified for windows compilation and since soundtouch could not be built on Windows, the Makefile.win is in src directory only (not at the top level). Several steps are needed for windows build:

* Install cygwin (https://cygwin.com/install.html). This typically installs in C:/cygwin
* Get gsm source code from http://www.quut.com/gsm/gsm-1.0.13.tar.gz and build it. Unzip the source at the same level as dhvani source. For example, if you clone this repo in C:/dev/dhvani-tts, unzip the gsm in c:/dev/gsm-1.0-pl13. This builds with a simple Makefile.
* Get GStreamer for windows from http://docs.gstreamer.com/display/GstSDK/Installing+on+Windows and install the 32 bit Runtime and Development installer files. The merge modules file is not needed. This installs in C:/gstreamer-sdk/0.10/x86/.
* Copy files from the gstreamer dir of this repo to c:/gstreamer-sdk/0.10/x86/lib. The installation files have an error and it has been fixed and made part of this repo for easier windows build.
* The gstreamer includes vorbis and we do not need to separately download it.
* <i> cd c:/dev/dhvani-tts/src </i>
* <b>make -f Makefile.win</b> (this uses make and gcc/g++ from cygwin)
* A shell script dhvani will be created which sets some env vars for running the executable found in .libs dir. This should be used to run dhvani and do not directly execute the executable dhvani.exe or lt-dhvani.exe.
* you need to set PATH to include gstreamer DLLs. You can do this either in .bashrc of c:/cygwin or in your computer's env settings. Include the following in your .bashrc:
export PATH=/cygdrive/c/gstreamer-sdk/0.10/x86/bin:/cygdrive/c/gstreamer-sdk/0.10/x86/lib:/cygdrive/c/dev/dhvani-tts/src/.libs:/cygdrive/c/dev/dhvani-tts/src/:$PATH
* From bash, run dhvani as: <b> dhvani -l hi /cygdrive/c/dev/dhvani-tts/tdemos/Hindi </b>
* From cmd: run dhvani as: <b> bash dhvani -l pa ..\tdemos\Punjabi </b>
* Finally, if you happen to clone this repo in a directory other than c:/dev/dhvani-tts, do not forget to change the DATABASEDIR in the src/Makefile.win to appropriate value before firing make.
* Happy reading out!!

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


