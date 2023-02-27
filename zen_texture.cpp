//
// zen_texture.cpp
//
// Copyright (c) 2021-2023 Václav Maroušek
//

#include "zen_texture.h"
#include <cassert>

using namespace GothicLib::ZenGin;

bool zCTexture::SaveTexture(FileStream* file)
{
	// Write beginning
	uint32_t signature = 0x5845545A;
	uint32_t version = 0;

	file->Write(FILE_ARGS(signature));
	file->Write(FILE_ARGS(version));

	// Read info
	file->Write(FILE_ARGS(format));
	file->Write(FILE_ARGS(width));
	file->Write(FILE_ARGS(height));
	file->Write(FILE_ARGS(mipmapCount));
	file->Write(FILE_ARGS(refWidth));
	file->Write(FILE_ARGS(refHeight));
	file->Write(FILE_ARGS(averageColor));

	// Read palette
	if (format == TEXFORMAT_P8)
	{
		file->Write(palette, sizeof(TEXPALETTE) * 256);
	}

	// Read data
	int32_t currWidth, currHeight;

	for (int i = mipmapCount - 1; i >= 0; i--)
	{
		currWidth	= width >> i;
		currHeight	= height >> i;

		if (currWidth < 1)
			currWidth = 1;

		if (currHeight < 1)
			currHeight = 1;

		size_t readSize = currWidth * currHeight * texFormatBpps[format];

		file->Write(data[i], readSize);
	}

	return true;
}

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

	// Read palette
	if (format == TEXFORMAT_P8)
	{
		palette = new TEXPALETTE[256];
		file->Read(palette, sizeof(TEXPALETTE) * 256);
	}

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

bool zCTexture::SavePortableBinary(FileStream* file)
{
	uint16_t version = 0;
	file->Write(FILE_ARGS(version));

	// Read format
	uint32_t fmt = 0;

	switch (format)
	{
	case	TEXFORMAT_P8:		fmt = 0;	break;
	default:
		break;
	}

	file->Write(FILE_ARGS(fmt));

	file->Write(FILE_ARGS(height));
	file->Write(FILE_ARGS(width));

	// Read the data (mipmap 0)
	uint32_t readSize = width * height * texFormatBpps[format];
	file->Write(FILE_ARGS(readSize));

	file->Write(data[0], readSize);

	// Palette
	if (format == TEXFORMAT_P8)
	{
		zCOLOR* oldPalette = new zCOLOR[256];

		for (size_t i = 0; i < 256; i++)
		{
			oldPalette[i].r = palette[i].r;
			oldPalette[i].g = palette[i].g;
			oldPalette[i].b = palette[i].b;
			oldPalette[i].a = 0;
		}

		file->Write(oldPalette, sizeof(zCOLOR) * 256);

		delete[] oldPalette;
	}

	return true;
}

bool zCTexture::LoadPortableBinary(FileStream* file)
{
	uint16_t version;
	file->Read(FILE_ARGS(version));

	if (version != 0)
	{
		LOG_ERROR("Unknown version \"" + std::to_string(version) + "\" texture portable binary, expected 0!");
		return false;
	}

	// Read format
	uint32_t fmt;
	file->Read(FILE_ARGS(fmt));

	switch (fmt)
	{
	case 0:		format = TEXFORMAT_P8;	break;
	case 1:		assert(true); /* INTENSITY_8 */ break;
	default:	format = TEXFORMAT_R5G6B5; break;
	}

	file->Read(FILE_ARGS(height));
	file->Read(FILE_ARGS(width));

	refWidth	= width;
	refHeight	= height;

	averageColor.b = 0;// todo
	averageColor.r = 0;// todo
	averageColor.g = 0;// todo
	averageColor.a = 0;// todo

	// Read the data (mipmap 0)
	mipmapCount = 1;
	data = new char*[mipmapCount];

	uint32_t readSize = 0;
	file->Read(FILE_ARGS(readSize));

	data[0] = new char[readSize];
	file->Read(data[0], readSize);

	// Palette
	if (format == TEXFORMAT_P8)
	{
		palette = new TEXPALETTE[256];

		zCOLOR* oldPalette = new zCOLOR[256];
		file->Read(oldPalette, sizeof(zCOLOR) * 256);

		for (size_t i = 0; i < 256; i++)
		{
			palette[i].r = oldPalette[i].r;
			palette[i].g = oldPalette[i].g;
			palette[i].b = oldPalette[i].b;
		}

		delete[] oldPalette;
	}

	return true;
}

bool zCTexture::Convert_P8ToR5G6B5()
{
	if (!palette || !data || format != TEXFORMAT_P8)
	{
		return false;
	}

	int32_t currWidth, currHeight;

	for (int i = mipmapCount - 1; i >= 0; i--)
	{
		currWidth = width >> i;
		currHeight = height >> i;

		if (currWidth < 1)
			currWidth = 1;

		if (currHeight < 1)
			currHeight = 1;

		uint16_t* newData = new uint16_t[currWidth * currHeight];
		uint8_t* oldData = (uint8_t*)data[i];

		// Convert
		for (size_t y = 0; y < currHeight; y++)
		{
			for (size_t x = 0; x < currWidth; x++)
			{
				size_t offset = (y * currWidth) + x;

				uint8_t pal = oldData[offset];
				TEXPALETTE color = palette[pal];

				uint8_t red = color.r;
				uint8_t green = color.g;
				uint8_t blue = color.b;

				uint16_t b = (blue >> 3) & 0x1f;
				uint16_t g = ((green >> 2) & 0x3f) << 5;
				uint16_t r = ((red >> 3) & 0x1f) << 11;

				newData[offset] = (uint16_t)(r | g | b);
			}
		}

		delete[] oldData;
		data[i] = (char*)newData;
	}

	format = TEXFORMAT_R5G6B5;
	delete[] palette;

	return true;
}

void zCTexture::Destroy()
{
	if (palette)
	{
		delete[] palette;
	}

	if (data)
	{
		for (size_t i = 0; i < mipmapCount; i++)
		{
			delete[] data[i];
		}

		delete[] data;
	}
}
