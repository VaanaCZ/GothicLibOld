#pragma once

#include <fstream>

bool ReadGFX(const wchar_t* filename,
	uint16_t* width, uint16_t* height, unsigned char** buffer, bool convertToRGB888)
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

	*buffer = new unsigned char[datasize * (convertToRGB888 ? 3 : 1)];
	file.read((char*)*buffer, datasize);

	if (convertToRGB888)
	{
		for (uint32_t i = datasize; i-- > 0; )
		{
			char pixel = (*buffer)[i];

			// A smarter programmer would make a lookup table ;)


			struct test
			{
				unsigned char r : 3;
				unsigned char g : 3;
				unsigned char b : 2;
			};

			test t = *(test*)&pixel;

			//unsigned char r = (pixel & 0xe0) >> 5;
			//unsigned char g = (pixel & 0x1c) >> 2;
			//unsigned char b = pixel & 0x03;
			
			unsigned char r = t.r;
			unsigned char g = t.g;
			unsigned char b = t.b;

			r = ceil(255.0f / 7.0f * r);
			g = ceil(255.0f / 7.0f * g);
			b = ceil(255.0f / 3.0f * b);

			(*buffer)[i * 3]		= b;
			(*buffer)[i * 3 + 1]	= g;
			(*buffer)[i * 3 + 2]	= r;

		}
	}

	*width	= res[0];
	*height	= res[1];

	return true;
}

struct VOB3D_Vertex
{
	int16_t x, y, z;
};

struct VOB3D_Face
{
	int16_t v1, v2, v3, v4; // If v4 is -1, this is a tri, otherwise its a quad

	unsigned char	nameLength;
	char			name[15];
};

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
