#pragma once
#include "CompressedMp3Content.h"
#include "lame.h"
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;

namespace SandCu_Audio
{
	public ref class LameWrapper sealed
	{
	public:
		LameWrapper();
		bool InitialLame(int InSampleRate,int OutSampleRate, int channels,int quality);
		IAsyncOperation<CompressedMp3Content^>^ EncodePcm2Mp3(IBuffer^ inPcm);
		void CloseLame();
	private:
		lame_global_flags* lame;
	};
	
}