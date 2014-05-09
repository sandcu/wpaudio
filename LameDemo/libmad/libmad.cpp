#include "pch.h"
#include "libmad.h"
void libmad::stream_ini()
{
	mad_stream* stream=nullptr;
	mad_stream_init(stream);
}