#include "zen_texture.h"

using namespace GothicLib::ZenGin;

bool zCTexture::LoadTexture(FileStream* file)
{
	// Read beginning
	uint32_t signature, version;
	file->Read(FILE_ARGS(signature));

	if (signature != 0x5845545A)
	{
		LOG_ERROR("Expected ZTEX signature!");
		return false;
	}

	file->Read(FILE_ARGS(version));

	if (version != 0)
	{
		LOG_ERROR("Unknown ZTEX version \"" + std::to_string(version) + "\"!");
		return false;
	}

	// Read info
	file->Read(FILE_ARGS(format));
	file->Read(FILE_ARGS(width));
	file->Read(FILE_ARGS(height));
	file->Read(FILE_ARGS(mipmapCount));
	file->Read(FILE_ARGS(refWidth));
	file->Read(FILE_ARGS(refHeight));
	file->Read(FILE_ARGS(averageColor));

	// Read data
	int32_t currWidth, currHeight;

	data = new char*[mipmapCount];

	for (int i = mipmapCount - 1; i >= 0; i--)
	{
		currWidth	= width >> i;
		currHeight	= height >> i;

		if (currWidth < 1)
			currWidth = 1;

		if (currHeight < 1)
			currHeight = 1;

		size_t readSize = currWidth * currHeight * texFormatBpps[format];

		data[i] = new char[readSize];
		file->Read(data[i], readSize);
	}

	return true;
}

void zCTexture::Destroy()
{
	if (data)
	{
		for (size_t i = 0; i < mipmapCount; i++)
		{
			delete[] data[i];
		}

		delete[] data;
	}
}
