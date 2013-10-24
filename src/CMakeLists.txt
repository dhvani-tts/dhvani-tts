cmake_minimum_required(VERSION 2.8)

set(DHVANI_SRCS alsa_player.c
	debug.c
	dhvani_lib.c
	dhvani_utils.c
	# Till presence of gstreamer libraries can be found this file is not
	# compiled in)
	# gstplay.c
	languages.c
	oggencoder.c
	phonetic_synthesizer_bn.c
	phonetic_synthesizer_gu.c
	phonetic_synthesizer_hi.c
	phonetic_synthesizer_kn.c
	phonetic_synthesizer_ml.c
	phonetic_synthesizer_mr.c
	phonetic_synthesizer_or.c
	phonetic_synthesizer_pa.c
	phonetic_synthesizer_ps.c
	phonetic_synthesizer_ta.c
	phonetic_synthesizer_te.c
	soundtouch_utils.c
	synthesizer.c
	UTF8Decoder.c
	)

add_library(dhvani SHARED ${DHVANI_SRCS})

find_package(ALSA)
if (ALSA_FOUND)
message("Found ALSA - enabling ALSA support")
target_link_libraries(dhvani ${ALSA_LIBRARY})
include_directories(dhvani ${ALSA_INCLUDE_DIR})
endif (ALSA_FOUND)

# TODO Find presence of other libraries like it is done for ALSA.

target_link_libraries(dhvani gsm)