//
// space.h
//
// Copyright (c) 2021-2023 Václav Maroušek
//

#pragma once

#include <fstream>

bool ReadTexPool(const wchar_t* filename, const char* texture,
	char* palette, char** buffer)
{
	std::ifstream file(filename, std::ios::binary);

	if (file.fail())
		return false;

	file.seekg(0, std::ios::end);
	uint64_t filesize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Find the correct texture
	while (true)
	{
		unsigned char nameLength;
		file.read((char*)&nameLength, sizeof(nameLength));

		if (nameLength == 0)
			continue;

		char* name = new char[nameLength + 1];
		name[nameLength] = NULL;

		file.read(name, nameLength);

		bool found = strcmp(name, texture) == 0;

		delete[] name;

		if (found)
		{
			*buffer = new char[12288];
			file.read(*buffer, 4096);

			for (uint32_t i = 4096; i-- > 0; )
			{
				unsigned char pixel = (*buffer)[i];

				(*buffer)[i * 3]		= palette[pixel * 3];
				(*buffer)[i * 3 + 1]	= palette[pixel * 3 + 1];
				(*buffer)[i * 3 + 2]	= palette[pixel * 3 + 2];
			}

			return true;
		}

		file.ignore(4127);
	}

	return false;
}

bool ReadGFX(const wchar_t* filename, char* palette, 
	uint16_t* width, uint16_t* height, unsigned char** buffer)
{
	std::ifstream file(filename, std::ios::binary);

	if (file.fail())
		return false;

	file.seekg(0, std::ios::end);
	uint64_t filesize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Header is only 4 bytes
	uint16_t res[2];

	if (filesize < sizeof(res))
		return false;

	file.read((char*)&res, sizeof(res));

	// Read data
	uint32_t datasize = (uint32_t)res[0] * (uint32_t)res[1];

	if (filesize < sizeof(res) + datasize)
		return false;

	*buffer = new unsigned char[datasize * 3];
	file.read((char*)*buffer, datasize);

	for (uint32_t i = datasize; i-- > 0; )
	{
		unsigned char pixel = (*buffer)[i];
		
		(*buffer)[i * 3]		= palette[pixel * 3];
		(*buffer)[i * 3 + 1]	= palette[pixel * 3 + 1];
		(*buffer)[i * 3 + 2]	= palette[pixel * 3 + 2];
	}

	*width	= res[0];
	*height	= res[1];

	return true;
}

bool ReadPAL(const wchar_t* filename, char** buffer)
{
	std::ifstream file(filename, std::ios::binary);

	if (file.fail())
		return false;

	file.seekg(0, std::ios::end);
	uint64_t filesize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (filesize != 768)
		return false;

	*buffer = new char[768];
	file.read(*buffer, 768);
}

#pragma pack(push, 1)
struct VOB3D_Vertex
{
	int16_t x, y, z;
};

struct VOB3D_Face
{
	int16_t v1, v2, v3, v4; // If v4 is -1, this is a tri, otherwise its a quad

	unsigned char	texutreNameLength;
	char			textureName[8];
	char			reserved[7];
};
#pragma pack(pop)

bool ReadV3D(const wchar_t* filename,
	VOB3D_Vertex** verts, uint16_t* vertCount,
	VOB3D_Face** faces, uint16_t* faceCount)
{
	std::ifstream file(filename, std::ios::binary);

	if (file.fail())
		return false;

	file.seekg(0, std::ios::end);
	uint64_t filesize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Header
#pragma pack(push, 1)
	struct Header
	{
		unsigned char	nameLength;
		char			name[40];
		int16_t			vertexCount;
		int16_t			faceCount;
		char			reserved[34];
	};
#pragma pack(pop)

	if (filesize < sizeof(Header))
		return false;

	Header header;
	file.read((char*)&header, sizeof(header));

	if (filesize < sizeof(Header) +
		(uint64_t)header.vertexCount * 6 +
		(uint64_t)header.faceCount * 24)
		return false;

	if (header.vertexCount < 0 ||
		header.faceCount < 0)
		return false;

	// Read data
	*verts = new VOB3D_Vertex[header.vertexCount];
	file.read((char*)*verts, header.vertexCount * sizeof(VOB3D_Vertex));
	*vertCount = header.vertexCount;

	*faces = new VOB3D_Face[header.faceCount];
	file.read((char*)*faces, header.faceCount * sizeof(VOB3D_Face));
	*faceCount = header.faceCount;

	return true;
}
