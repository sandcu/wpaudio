#pragma once
#include "RawPCMContent.h"
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace SandCu_Audio;

namespace SandCu_Audio
{
	public ref class LibmadWrapper sealed
	{
	public:
		LibmadWrapper(void);
		bool DecodeMp32Pcm_Init(IBuffer^ inMp3);
		RawPCMContent^ ReadSample();
		int  ReadSamplesForSampleRate();
		void CloseFile();
		int GetSampleRate();
		bool IsAlive();
	private:
		bool isAlive;
		int freeIndex;
	};
}
