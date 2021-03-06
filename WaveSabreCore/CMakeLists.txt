add_library(WaveSabreCore
	include/WaveSabreCore/Adultery.h
	include/WaveSabreCore/AllPass.h
	include/WaveSabreCore/AllPassDelay.h
	include/WaveSabreCore/BiquadFilter.h
	include/WaveSabreCore/Cathedral.h
	include/WaveSabreCore/Chamber.h
	include/WaveSabreCore/Comb.h
	include/WaveSabreCore/Crusher.h
	include/WaveSabreCore/DelayBuffer.h
	include/WaveSabreCore/Device.h
	include/WaveSabreCore/Echo.h
	include/WaveSabreCore/Envelope.h
	include/WaveSabreCore/Falcon.h
	include/WaveSabreCore/GmDls.h
	include/WaveSabreCore/Helpers.h
	include/WaveSabreCore/Leveller.h
	include/WaveSabreCore/MxcsrFlagGuard.h
	include/WaveSabreCore/ResampleBuffer.h
	include/WaveSabreCore/SampleLoader.h
	include/WaveSabreCore/SamplePlayer.h
	include/WaveSabreCore/Scissor.h
	include/WaveSabreCore/Slaughter.h
	include/WaveSabreCore/Smasher.h
	include/WaveSabreCore/Specimen.h
	include/WaveSabreCore/StateVariableFilter.h
	include/WaveSabreCore/SynthDevice.h
	include/WaveSabreCore/Thunder.h
	include/WaveSabreCore/Twister.h
	src/Adultery.cpp
	src/AllPass.cpp
	src/AllPassDelay.cpp
	src/BiquadFilter.cpp
	src/Cathedral.cpp
	src/Chamber.cpp
	src/Comb.cpp
	src/Crusher.cpp
	src/DelayBuffer.cpp
	src/Device.cpp
	src/Echo.cpp
	src/Envelope.cpp
	src/Falcon.cpp
	src/GmDls.cpp
	src/Helpers.cpp
	src/Leveller.cpp
	src/MxcsrFlagGuard.cpp
	src/ResampleBuffer.cpp
	src/SampleLoader.cpp
	src/SamplePlayer.cpp
	src/Scissor.cpp
	src/Slaughter.cpp
	src/Smasher.cpp
	src/Specimen.cpp
	src/StateVariableFilter.cpp
	src/SynthDevice.cpp
	src/Thunder.cpp
	src/Twister.cpp
	)

if(WIN32)
	target_sources(WaveSabreCore PRIVATE
		)

	target_link_libraries(WaveSabreCore PUBLIC Msacm32.lib)
endif()

target_include_directories(WaveSabreCore PUBLIC include)

if(MSVC)
	target_compile_options(WaveSabreCore
		PUBLIC $<$<CONFIG:MinSizeRel>:/GS- /Gy /Zi /GL /GR->
		PRIVATE $<$<CONFIG:MinSizeRel>:/Oi /fp:fast>)

	set_property(TARGET WaveSabreCore APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_MINSIZEREL
		" /LTCG")

	if(MSVC_VERSION GREATER 1900)
		target_compile_options(WaveSabreCore PUBLIC
			$<$<CONFIG:MinSizeRel>:/Zc:sizedDealloc->)
	endif()
else()
	# assuming GCC or clang for now

	if(ENABLE_FFMPEG_GSM)
		message(NOTICE "Enabling FFmpeg GSM 6.10 decoding")
		target_compile_definitions(WaveSabreCore PRIVATE HAVE_FFMPEG_GSM=1)
	else()
		message(NOTICE "FFmpeg GSM 6.10 decoding disabled")
	endif()

	if(ENABLE_EXTERNAL_GMDLS)
		message(NOTICE "Enabling gm.dls support")
		target_compile_definitions(WaveSabreCore PRIVATE HAVE_EXTERNAL_GMDLS=1)
	else()
		message(NOTICE "External gm.dls loading disabled")
	endif()

	if(CMAKE_BUILD_TYPE EQUAL Debug)
		target_compile_options(WaveSabreCore PUBLIC -g -Og)
	else()
		#set_property(TARGET WaveSabreCore PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
		target_compile_options(WaveSabreCore
			PUBLIC -O2 -fno-exceptions -fno-rtti -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-threadsafe-statics
			PRIVATE -ffast-math -march=nocona -ffunction-sections -fdata-sections -Wl,--gc-sections)
	endif()
endif()
