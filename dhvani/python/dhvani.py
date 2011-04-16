#!/usr/bin/python
# -*- coding: utf-8 -*-

  ##########################################################################################################################
  # dhvani.py                                                                                                              #
  #                                                                                                                        #
  # Copyright 2011 Vasudev Kamath <kamathvasudev@gmail.com>                                                                #
  #                                                                                                                        #
  # This program is free software; you can redistribute it and/or modify                                                   #
  # it under the terms of the GNU  General Public License as published by                                                  #
  # the Free Software Foundation; either version 3 of the License, or                                                      #
  # (at your option) any later version.                                                                                    #
  #                                                                                                                        #
  # This program is distributed in the hope that it will be useful,                                                        #
  # but WITHOUT ANY WARRANTY; without even the implied warranty of                                                         #
  # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                                          #
  # GNU General Public License for more details.                                                                           #
  #                                                                                                                        #
  # You should have received a copy of the GNU General Public License                                                      #
  # along with this program; if not, write to the Free Software                                                            #
  # Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,                                                             #
  # MA 02110-1301, USA.                                                                                                    #
  ##########################################################################################################################

import os
import codecs
from ctypes import CDLL
from ctypes import Structure,CFUNCTYPE,POINTER,byref
from ctypes import c_int,c_short,c_float,c_char_p,c_void_p

dhvani = CDLL("libdhvani.so.0")


# Define required enums

# dhvani_ERROR enum
(DHVANI_OK,DHVANI_INTERNAL_ERROR) = (0,-1)

# dhvani_output_file_format enum
(DHVANI_OGG_FORMAT,DHVANI_WAV_FORMAT) = (0,1)

# dhvani_Languages enum
(HINDI,MALAYALAM,TAMIL,KANNADA,
 ORIYA,PANJABI,GUJARATI,TELUGU,
 BENGALAI,MARATHI,PASHTO) = (1,2,3,4,5,6,7,8,9,10,11)

# Define call back types
t_dhvani_synth_callback = CFUNCTYPE(c_int,c_int)
t_dhvani_audio_callback = CFUNCTYPE(c_int,POINTER(c_short),c_int)

# Unused structure to match original implementation
class dhvani_VOICE(Structure):
    pass

# dhvani_option structure mapping class
class dhvani_options(Structure):
    _fields_ = [("voice",POINTER(dhvani_VOICE)),
                ("pitch",c_float),
                ("tempo",c_float),
                ("rate",c_int),
                ("language",c_int),
                ("output_file_format",c_int),
                ("isPhonetic",c_int),
                ("speech_to_file",c_int),
                ("output_file_name",c_char_p),
                ("synth_callback_fn",POINTER(t_dhvani_synth_callback)),
                ("audio_callback_fn",(t_dhvani_audio_callback))]


# Define dhvani speech function
dhvani_say = dhvani.dhvani_say
dhvani_say.restype = c_int
dhvani_say.argtypes = [c_char_p,POINTER(dhvani_options)]

# dhvani_speak_file function
dhvani_speak_file = dhvani.dhvani_speak_file
dhvani_speak_file.restype = c_int
dhvani_speak_file.argtypes = [c_void_p,POINTER(dhvani_options)]

# fdopen function not related to dhvani but a C library function
# in stdio.h this is used to
fileopen = dhvani.fdopen
fileopen.restype = c_void_p
fileopen.argtypes = [c_int,c_char_p]

def dhvani_init():
    option = dhvani_options()
    option.language = -1
    option.isPhonetic = 0
    option.speech_to_file = 0
    option.pitch = 0.0
    option.tempo = 0
    option.rate = 16000
    #option.synth_callback_fn = None
    #option.audio_callback_fn = None
    dhvani.start_synthesizer()
    return option

def dhvani_close():
    dhvani.stop_synthesizer()
    

def text_to_speech(text,pitch=0, speed=16000):
    dh = dhvani_init()
    dh.rate = c_int(int(speed))
    dh.pitch = c_float(float(pitch))
    dh.speech_to_file = 0
    return_type = dhvani_say(c_char_p(text.encode("utf-8")),byref(dh))
    dhvani_close()
    return return_type 

def text_to_speech_to_file(text, outputfile , pitch=0, speed=16000, format="ogg"):
    dh = dhvani_init()
    dh.rate = c_int(int(speed))
    dh.pitch = c_float(float(pitch))
    if format == "ogg":
        dh.output_file_format = DHVANI_OGG_FORMAT
    else:
        dh.output_file_format = DHVANI_WAV_FORMAT
    dh.speech_to_file = 1
    dh.output_file_name = c_char_p(outputfile)
    return_type = dhvani_say(c_char_p(text.encode("utf-8")),byref(dh))
    dhvani_close()
    return return_type 


def text_to_speech_callback(text, outputfile , pitch=0, speed=16000, format="ogg"):
    dh = dhvani_init()
    dh.rate = c_int(int(speed))
    dh.pitch = c_float(float(pitch))
    if format == "ogg":
        dh.output_file_format = DHVANI_OGG_FORMAT
    else:
        dh.output_file_format = DHVANI_WAV_FORMAT
    dh.speech_to_file = 1
    dh.audio_callback_fn = t_dhvani_audio_callback(callback)
    dh.output_file_name = c_char_p(outputfile)
    return_type = dhvani_say(c_char_p(text.encode("utf-8")),byref(dh))
    dhvani_close()
    return return_type 


def callback(data, length):
    d = [length]
    if (length > 0):
        if (data != None):
            print("callback!", length)
            for i in range(length):
				print data[i]
            outputfile_fd.write(d)
    return 0

"""
if __name__ == "__main__":
    text = codecs.open("input.txt",encoding="utf-8").read()
    #text_to_speech(text)
    text_to_speech_to_file(text, "out1.ogg")
    text_to_speech_to_file(text, "out2.ogg")
    #text_to_speech_callback(text, "out2.ogg", format="ogg")
    
"""    
        
