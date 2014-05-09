#include "pch.h"
#include "LibmadWrapper.h"
#include "mad.h"
#include <robuffer.h>
#include <ppltasks.h>
#include <IO.h>

using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace SandCu_Audio;
using namespace Platform;

#define INPUT_BUFFER_SIZE	(5*8192)
#define OUTPUT_BUFFER_SIZE	8192 /* Must be an integer multiple of 4. */
struct MP3FileHandle
{
	unsigned char * fileBytes;
	int size; 
	int fileIndex;
	mad_stream stream;
	mad_frame frame;
	mad_synth synth;
	mad_timer_t timer;
	int leftSamples;
	int offset;
	unsigned char inputBuffer[INPUT_BUFFER_SIZE];
};
static MP3FileHandle* handles[100];
bool _Decode(unsigned char* source,int length,int index);
int _Sread(unsigned char *target, int size, int count,MP3FileHandle* handle);
int _ReadSamples(short* array, int size, int channelsCount,int index);
int _ReadSamples(float* array, int size,int index);
void _CloseMad(int index);
int _GetSampleRate(int index);
//MP3FileHandle * handle;

LibmadWrapper::LibmadWrapper(void)
{
}
static int findFreeHandle( )
{
	for( int i = 0; i < 100; i++ )
	{
		if( handles[i] == 0 )
			return i;
	}
 
	return -1;
}
 
bool LibmadWrapper::DecodeMp32Pcm_Init(IBuffer^ inMp3)
{
	IUnknown* pUnk = reinterpret_cast<IUnknown*>(inMp3);
	IBufferByteAccess* pAccess = NULL;
	byte* bytes = NULL;
	HRESULT hr = pUnk->QueryInterface(__uuidof(IBufferByteAccess), (void**)&pAccess);
	if (SUCCEEDED(hr))
	{
		hr = pAccess->Buffer(&bytes);
		if (SUCCEEDED(hr))
		{
			isAlive=true;
			//RawPCMContent^ result = ref new RawPCMContent();
			int mp3Length = inMp3->Length;
			int index=findFreeHandle();
			freeIndex=index;
			bool result=_Decode(bytes,mp3Length,index);
			pAccess->Release();
			return result;
		}
		else
		{
			pAccess->Release();
			throw ref new Platform::Exception(hr, L"Couldn't get bytes from the buffer");
		}
	}
	else
	{
		throw ref new Platform::Exception(hr, L"Couldn't access the buffer");
	}
}
RawPCMContent^ LibmadWrapper::ReadSample()
{
	short* buffer=new short[64*1024];
	int count=_ReadSamples(buffer,64*1024,2,freeIndex);
	RawPCMContent^ result=ref new RawPCMContent();
	result->count=count;
	result->PCMData = ref new Platform::Array<short>(count);

	for(int i=0; i<count; i++)
	{
		(result->PCMData)->get(i) = buffer[i];
	}
	delete buffer;
	return result;
}
int LibmadWrapper::ReadSamplesForSampleRate()
{
	 short* tmpSamples =new  short[256];
	 _ReadSamples(tmpSamples,256,2,freeIndex);
	 delete[] tmpSamples;
	 return _GetSampleRate(freeIndex);
}
void LibmadWrapper::CloseFile()
{
	isAlive=false;
	_CloseMad(freeIndex);
}
int LibmadWrapper::GetSampleRate()
{
	return _GetSampleRate(freeIndex);
}
bool LibmadWrapper::IsAlive()
{
	return isAlive;
}
static inline void _CloseHandle( MP3FileHandle* handle )
{
	//delete handle->fileBytes;
	mad_synth_finish(&handle->synth);
	mad_frame_finish(&handle->frame);
	mad_stream_finish(&handle->stream);
	delete handle;
}

static inline signed short _FixedToShort(mad_fixed_t Fixed)
{
	if(Fixed>=MAD_F_ONE)
		return(SHRT_MAX);
	if(Fixed<=-MAD_F_ONE)
		return(-SHRT_MAX);

	Fixed=Fixed>>(MAD_F_FRACBITS-15);
	return((signed short)Fixed);
}

bool _Decode(unsigned char* source,int length,int index)
{
	do
	{
		MP3FileHandle* mp3Handle = new MP3FileHandle( );
		mp3Handle->fileBytes = source;
		//fseek( fileHandle, 0, SEEK_END);
		mp3Handle->size = length;
		mad_stream_init(&mp3Handle->stream);
		mad_frame_init(&mp3Handle->frame);
		mad_synth_init(&mp3Handle->synth);
		mad_timer_reset(&mp3Handle->timer);
		mad_header_decode(&mp3Handle->frame.header, &mp3Handle->stream);
		mp3Handle->fileIndex=0;
		handles[index] = mp3Handle;
		return true;
	}while (1);

	return false;
}

static inline int _ReadNextFrame( MP3FileHandle* mp3 )
{
	do
	{
		if( mp3->stream.buffer == 0 || mp3->stream.error == MAD_ERROR_BUFLEN )
		{
			int inputBufferSize = 0;
			if( mp3->stream.next_frame != 0 )
			{
				int leftOver = mp3->stream.bufend - mp3->stream.next_frame;
				for( int i = 0; i < leftOver; i++ )
					mp3->inputBuffer[i] = mp3->stream.next_frame[i];
				int readBytes = _Sread( mp3->inputBuffer + leftOver, 1, INPUT_BUFFER_SIZE - leftOver, mp3);
				if( readBytes == 0 )
					return 0;
				inputBufferSize = leftOver + readBytes;
			}
			else
			{
				int readBytes = _Sread( mp3->inputBuffer, 1, INPUT_BUFFER_SIZE, mp3);
				if( readBytes == 0 )
					return 0;
				inputBufferSize = readBytes;
			}

			mad_stream_buffer( &mp3->stream, mp3->inputBuffer, inputBufferSize );
			mp3->stream.error = MAD_ERROR_NONE;
		}

		if( mad_frame_decode( &mp3->frame, &mp3->stream ) )
		{
			if( mp3->stream.error == MAD_ERROR_BUFLEN ||(MAD_RECOVERABLE(mp3->stream.error)))
				continue;
			else
				return 0;
		}
		else
			break;
	} while( true );

	mad_timer_add( &mp3->timer, mp3->frame.header.duration );
	mad_synth_frame( &mp3->synth, &mp3->frame );
	mp3->leftSamples = mp3->synth.pcm.length;
	mp3->offset = 0;

	return -1;
}

int _SkipSamples(int sampleCount,int index)
{
	MP3FileHandle* mp3 = handles[index];

	int skippedSampleCount = 0;
	while (true)
	{
		int ret = _ReadNextFrame(mp3);
		skippedSampleCount += mp3->leftSamples;
		if (ret == 0)
		{
			return 0;
		}
		if (skippedSampleCount > sampleCount)
		{
			break;
		}
	}
	return skippedSampleCount - mp3->leftSamples;

}

int _ReadSamples(float* array, int size,int index)
{
	MP3FileHandle* mp3 = handles[index];
	float* target = array;

	int idx = 0;
	while( idx != size )
	{
		if( mp3->leftSamples > 0 )
		{
			for( ; idx < size && mp3->offset < mp3->synth.pcm.length; mp3->leftSamples--, mp3->offset++ )
			{
				int value = _FixedToShort(mp3->synth.pcm.samples[0][mp3->offset]);

				if( MAD_NCHANNELS(&mp3->frame.header) == 2 )
				{
					value += _FixedToShort(mp3->synth.pcm.samples[1][mp3->offset]);
					value /= 2;
				}

				target[idx++] = value / (float)SHRT_MAX;
			}
		}
		else
		{
			int result = _ReadNextFrame( mp3 );
			if( result == 0 )
				return 0;
		}

	}

	delete[] target;

	if( idx > size )
		return 0;

	return size;
}
int _ReadSamples(short* array, int size, int channelsCount,int index)
{
	MP3FileHandle* mp3 = handles[index];
	short* target =array;

	int idx = 0;
	while( idx != size )
	{
		if( mp3->leftSamples > 0 )
		{
			for( ; idx < size && mp3->offset < mp3->synth.pcm.length; mp3->leftSamples--, mp3->offset++ )
			{
				if(channelsCount == 2){
					int value = _FixedToShort(mp3->synth.pcm.samples[0][mp3->offset]);
					target[idx++] = value;
					value = _FixedToShort(mp3->synth.pcm.samples[1][mp3->offset]);
					target[idx++] = value;
				}else{
					int value = _FixedToShort(mp3->synth.pcm.samples[0][mp3->offset]);
					value += _FixedToShort(mp3->synth.pcm.samples[1][mp3->offset]);
					value /= 2;
					target[idx++] = value;
				}
			}
		}
		else
		{
			int result = _ReadNextFrame( mp3 );
			if( result == 0 )
				return 0;
		}

	}

	//delete target;

	if( idx > size )
		return 0;

	return size;
}
int _GetSampleRate(int index)
{
	int samplerate = 0;
	if( handles[index] != 0 )
	{
		MP3FileHandle* mp3 = handles[index];
		return mp3->synth.pcm.samplerate;
	}
	return -1;
}

int _GetBitRate(int index)
{
	int bitrate = 5;
	if( handles[index] != 0 )
	{
		MP3FileHandle* mp3 = handles[index];
		return mp3->frame.header.bitrate;
	}
	return bitrate;
}

void _CloseMad(int index)
{
	if( handles[index] != 0 )
	{
		_CloseHandle( handles[index] );
		handles[index] = 0;
	}
}
int _Sread(unsigned char *target, int size, int count, MP3FileHandle *handle)
{
	if((handle->size-handle->fileIndex-1)>count)
	{
		for(int i=0;i<count;i++)
		{
			target[i]=handle->fileBytes[handle->fileIndex+i];
		}
		handle->fileIndex+=count;
		return count;
	}
	else
	{
		int rest=handle->size-handle->fileIndex-1;
		for(int i=0;i<rest;i++)
		{
			target[i]=handle->fileBytes[handle->fileIndex+i];
		}
		handle->fileIndex+=(rest);
		return rest;
	}
}