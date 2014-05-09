#pragma once

namespace SandCu_Audio
{
	public ref class CompressedMp3Content sealed
	{
	public:
		property int count;
		CompressedMp3Content(void);
		property Platform::Array<unsigned char>^ Mp3Data;
	};
}