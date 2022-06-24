
#include "stdio.h"
#include "windows.h"

#pragma pack(push, 1)
struct TGA_HEADER
{
	char	  idLength;
	char	  colorMapType;
	char	  dataTypeCode;
	short int colorMapOrigin;
	short int colorMapLength;
	char	  colorMapDepth;
	short int xOrigin;
	short int yOrigin;
	short	  width;
	short	  height;
	char	  bitsPerPixel;
	char	  imageDescriptor;
};
#pragma pack(pop)

void BufferToTGA(unsigned char* buffer, unsigned int width, unsigned int height, unsigned int bytesPerPixel, const char* filename)
{
	FILE* fp;
		
	fopen_s(&fp, filename, "wb");

	TGA_HEADER header = { 0 };
	header.dataTypeCode = 2;
	header.width = width;
	header.height = height;
	header.bitsPerPixel = bytesPerPixel * 8;

	fwrite(&header, 1, sizeof(header), fp);

	for (size_t i = 0; i < width * height * bytesPerPixel; i++)
	{
		fwrite(&buffer[i], 1, sizeof(buffer[i]), fp);
	}

	fflush(fp);
	fclose(fp);
}





#include "vdfs.hpp"
#include "space.hpp"


void SaveOBJ(const wchar_t* filename,
	VOB3D_Vertex* verts, uint16_t vertCount,
	VOB3D_Face* faces, uint16_t faceCount)
{
	std::ofstream obj(filename);

	for (size_t i = 0; i < vertCount; i++)
	{
		obj << "v " << verts[i].x << " ";
		obj << verts[i].y * -1.0f << " ";
		obj << verts[i].z << std::endl;
	}

	for (size_t i = 0; i < faceCount; i++)
	{
		if (faces[i].v4 == -1)
		{
			obj << "f " << faces[i].v1 + 1 << " ";
			obj << faces[i].v2 + 1 << " ";
			obj << faces[i].v3 + 1 << std::endl;
		}
		else
		{
			obj << "f " << faces[i].v1 + 1 << " ";
			obj << faces[i].v2 + 1 << " ";
			obj << faces[i].v3 + 1 << std::endl;

			obj << "f " << faces[i].v1 + 1 << " ";
			obj << faces[i].v4 + 1 << " ";
			obj << faces[i].v3 + 1 << std::endl;
		}
	}
}



int main(int argc, char* argv[])
{
	/*VDFS::FileSystem fs;
	
	fs.InitializeDirectory(L"D:\\Gothic\\Gothic_demo5");
	//fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic Sequel");

	//VDFS::FileStream file;
	//
	//file.Open("D:\\Gothic\\Gothic_demo3\\config.d", 'r');
	//
	//std::string test;
	//
	//
	//while (file.ReadLine(test))
	//{
	//	std::cout << test << std::endl;
	//
	//}

	fs.PrintDebug();

	*/

	uint16_t x, y;
	unsigned char* buff;

#define SPACE(name, path) \
	ReadGFX(TEXT(path) TEXT(name) L".GRX", &x, &y, &buff, true); \
	BufferToTGA(buff, x, y, 3, "space_textures\\" name ".TGA")

	//SPACE("64ALIEN1", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64ERDE01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64FELS01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64FELS02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64FELS03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64FELS04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64FELS05", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64FELS06", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64HOLZ01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64HOLZ02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64HOLZ03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64HOLZ04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64HOLZ05", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64HOLZ06", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64HOLZ07", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64MAUR01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64MAUR02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64MAUR03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64MAUR04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH05", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH06", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH07", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH08", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH09", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH10", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH11", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("64TECH12", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BACK6", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BILD001", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BILD002", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BILD003", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BILD004", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BILD01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BILD04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BILD06", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BILD07", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BILD08", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("BLASEN", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("CELTIC2", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("FELS01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("FELS02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("FELS03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("HOLZ01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("HOLZ02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("HOLZ03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("HOLZ04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("HOLZ05", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("MAUR01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("MAUR02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("MAUR03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("STEIN", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("TECH01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("TECH02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("TECH03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	//SPACE("TECH04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");


	VOB3D_Vertex* verts;
	uint16_t vertCount;
	VOB3D_Face* faces;
	uint16_t faceCount;

	ReadV3D(L"D:\\DOS\\GAMES\\FINSTER\\VOB3D\\SCHWERT1.V3D", &verts, &vertCount, &faces, &faceCount);
	//ReadV3D(L"D:\\DOS\\GAMES\\FINSTER\\VOB3D\\BETT1.V3D", &verts, &vertCount, &faces, &faceCount);
	//ReadV3D(L"D:\\DOS\\GAMES\\FINSTER\\VOB3D\\AMBOSS1.V3D", &verts, &vertCount, &faces, &faceCount);


	SaveOBJ(L"TEST.OBJ", verts, vertCount, faces, faceCount);

	return 0;
}