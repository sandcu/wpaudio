
#include "pch.h"
#include "LameWrapper.h"
#include "Lame.h"
#include <robuffer.h>
#include <ppltasks.h>
#include <IO.h>

using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace SandCu_Audio;
using namespace Platform;

#define BUFF_SIZE 512 
#define INBUFF_SIZE 4096 
#define MP3BUFF_SIZE (int) (1.25 * INBUFF_SIZE) + 7200
static int run=1;
using namespace Windows::Storage;

LameWrapper::LameWrapper()
{
}


bool LameWrapper::InitialLame(int InSampleRate,int OutSampleRate, int channels,int quality)
{
	do
	{
		lame= lame_init();
		lame_set_in_samplerate(lame, InSampleRate);
		lame_set_out_samplerate(lame,OutSampleRate);
		lame_set_num_channels(lame, channels);
		lame_set_quality(lame, quality);
		lame_init_params(lame);
		return true;
	}while (1);
	return false;
}
Windows::Foundation::IAsyncOperation<CompressedMp3Content^>^ LameWrapper::EncodePcm2Mp3(IBuffer^ inPcm )
{
	IUnknown* pUnk = reinterpret_cast<IUnknown*>(inPcm);
	IBufferByteAccess* pAccess = NULL;
	byte* bytes = NULL;
	HRESULT hr = pUnk->QueryInterface(__uuidof(IBufferByteAccess), (void**)&pAccess);
	if (SUCCEEDED(hr))
	{
		hr = pAccess->Buffer(&bytes);
		if (SUCCEEDED(hr))
		{
			return Concurrency::create_async([=]()->CompressedMp3Content^
			{
				CompressedMp3Content^ result = ref new CompressedMp3Content();
				int pcmLength = inPcm->Length;
				std::vector<short> inBuffer = std::vector<short>(pcmLength / 2);

				for (std::vector<short>::size_type i=0; i<inBuffer.size(); i++)
				{
					inBuffer[i] = (((short)bytes[i*2+1]) << 8) + bytes[i*2];
				}
				std::vector<short> leftBuffer=std::vector<short>(pcmLength / 4);
				std::vector<short> rightBuffer=std::vector<short>(pcmLength / 4);
				for(int i=0;i<pcmLength/4;i++)
				{ 
						rightBuffer[i]=inBuffer[2*i+1];				 
						leftBuffer[i]=inBuffer[2*i];		 
				}
				std::vector<byte> outBuffer(inPcm->Length);
				try
				{
					if(lame!=nullptr)
					{
						int size = lame_encode_buffer(lame, leftBuffer.data(), rightBuffer.data(), leftBuffer.size(), outBuffer.data(), 0);
						if (size > 0)
						{
							result->Mp3Data = ref new Platform::Array<unsigned char>(size);

							for(int i=0; i<size; i++)
							{
								(result->Mp3Data)->get(i) = outBuffer[i];
							}
						}}
				}
				catch(...)
				{}

				pAccess->Release();

				return result;
			});
		}
		else
		{
			pAccess->Release();
			throw ref new Platform::Exception(hr, L"Couldn't get bytes from the buffer");
		}
	}
	else
	{
		lame_close(lame);
		throw ref new Platform::Exception(hr, L"Couldn't access the buffer");
	}
}

void LameWrapper::CloseLame()
{
	lame_close(lame);
	lame=0;
}

