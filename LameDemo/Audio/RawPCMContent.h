#pragma once

namespace SandCu_Audio
{
	public ref class RawPCMContent sealed
	{
	public:
		property int count;
		RawPCMContent(void);
		property Platform::Array<short>^ PCMData;
	};
}