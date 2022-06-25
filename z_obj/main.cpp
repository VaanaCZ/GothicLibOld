/*
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
		if (bytesPerPixel == 3)
		{
			fwrite(&buffer[i + 2], 1, sizeof(buffer[i + 2]), fp);
			fwrite(&buffer[i + 1], 1, sizeof(buffer[i + 1]), fp);
			fwrite(&buffer[i], 1, sizeof(buffer[i]), fp);

			i++;
			i++;
		}
		else
		{
			fwrite(&buffer[i], 1, sizeof(buffer[i]), fp);
		}
	}

	fflush(fp);
	fclose(fp);
}

*/



#include "zen_vdfs.h"
/*#include "space.hpp"


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

*/

int main(int argc, char* argv[])
{
	ZEN::FileSystem fs;
	
	fs.InitializeDirectory(L"D:\\Gothic\\Gothic_demo5");
	//fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic Sequel");
	//fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic");

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

	ZEN::FileStream* f = fs.OpenFile("DATA\\anims//../tEXuReS\\..//WORLDS\\/WORLD/WORLD.zen", false);
	//ZEN::FileStream* f = fs.OpenFile("errorlog.txt", true);
	
	if (!f)
	{
		std::cout << "File fucked";
	}
	else
	{
		char a[101];
		f->Read(&a, 100);
		a[100] = NULL;
		std::cout << a;
	}

	//fs.PrintDebug();

	/*

	char* palette;
	ReadPAL(L"D:\\DOS\\GAMES\\FINSTER\\DATA\\GAME.PAL", &palette);


	uint16_t x, y;
	unsigned char* buff;

#define SPACE(name, path) \
	ReadGFX(TEXT(path) TEXT(name) L".GRX", palette, &x, &y, &buff); \
	BufferToTGA(buff, x, y, 3, "vob_textures\\" name ".TGA")

	SPACE("64ALIEN1", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64ERDE01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64FELS01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64FELS02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64FELS03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64FELS04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64FELS05", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64FELS06", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64HOLZ01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64HOLZ02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64HOLZ03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64HOLZ04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64HOLZ05", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64HOLZ06", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64HOLZ07", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64MAUR01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64MAUR02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64MAUR03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64MAUR04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH05", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH06", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH07", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH08", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH09", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH10", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH11", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("64TECH12", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BACK6", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BILD001", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BILD002", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BILD003", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BILD004", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BILD01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BILD04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BILD06", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BILD07", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BILD08", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("BLASEN", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("CELTIC2", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("FELS01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("FELS02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("FELS03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("HOLZ01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("HOLZ02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("HOLZ03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("HOLZ04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("HOLZ05", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("MAUR01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("MAUR02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("MAUR03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("STEIN", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("TECH01", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("TECH02", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("TECH03", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");
	SPACE("TECH04", "D:\\DOS\\GAMES\\FINSTER\\VOB3D\\");


	//VOB3D_Vertex* verts;
	//uint16_t vertCount;
	//VOB3D_Face* faces;
	//uint16_t faceCount;

	//ReadV3D(L"D:\\DOS\\GAMES\\FINSTER\\VOB3D\\SCHWERT1.V3D", &verts, &vertCount, &faces, &faceCount);
	//ReadV3D(L"D:\\DOS\\GAMES\\FINSTER\\VOB3D\\BETT1.V3D", &verts, &vertCount, &faces, &faceCount);
	//ReadV3D(L"D:\\DOS\\GAMES\\FINSTER\\VOB3D\\AMBOSS1.V3D", &verts, &vertCount, &faces, &faceCount);


	//SaveOBJ(L"TEST.OBJ", verts, vertCount, faces, faceCount);

	char* buffer;


#define POOL(name) \
	ReadTexPool(L"D:\\DOS\\GAMES\\FINSTER\\POOL\\TEXPOOL.DAT", name, palette, &buffer); \
	BufferToTGA((unsigned char*)buffer, 64, 64, 3, "pool_textures\\" name ".TGA")


	POOL("_AGRAV_A.TEX");
	POOL("AGRAV_A2.TEX");
	POOL("AGRAV_A3.TEX");
	POOL("AGRAV_A4.TEX");
	POOL("AGRAV_A5.TEX");
	POOL("AGRAV_A6.TEX");
	POOL("AGRAV_A7.TEX");
	POOL("AGRAV_A8.TEX");
	POOL("_AGRAV_B.TEX");
	POOL("AGRAV_B2.TEX");
	POOL("__BAND_A.TEX");
	POOL("_BAND_A2.TEX");
	POOL("_BAND_A3.TEX");
	POOL("_BAND_A4.TEX");
	POOL("_BAND_A5.TEX");
	POOL("_BAND_A6.TEX");
	POOL("_BAND_A7.TEX");
	POOL("_BAND_A8.TEX");
	POOL("__BAND_B.TEX");
	POOL("_BAND_B2.TEX");
	POOL("_BAND_B3.TEX");
	POOL("_BAND_B4.TEX");
	POOL("_BAND_B5.TEX");
	POOL("_BAND_B6.TEX");
	POOL("_BAND_B7.TEX");
	POOL("_BAND_B8.TEX");
	POOL("__BAND_C.TEX");
	POOL("_BAND_C2.TEX");
	POOL("_BAND_C3.TEX");
	POOL("_BAND_C4.TEX");
	POOL("_BAND_C5.TEX");
	POOL("_BAND_C6.TEX");
	POOL("_BAND_C7.TEX");
	POOL("_BAND_C8.TEX");
	POOL("__BAND_D.TEX");
	POOL("_BAND_D2.TEX");
	POOL("_BAND_D3.TEX");
	POOL("_BAND_D4.TEX");
	POOL("_BAND_D5.TEX");
	POOL("_BAND_D6.TEX");
	POOL("_BAND_D7.TEX");
	POOL("_BAND_D8.TEX");
	POOL("_BODEN_A.TEX");
	POOL("BODEN_A2.TEX");
	POOL("BODEN_A3.TEX");
	POOL("BODEN_A4.TEX");
	POOL("_COLOR_A.TEX");
	POOL("___COM_A.TEX");
	POOL("__COM_A2.TEX");
	POOL("__COM_A3.TEX");
	POOL("__COM_A4.TEX");
	POOL("__COM_A5.TEX");
	POOL("__COM_A6.TEX");
	POOL("__COM_A7.TEX");
	POOL("__COM_A8.TEX");
	POOL("__ENRG_A.TEX");
	POOL("_ENRG_A2.TEX");
	POOL("_ENRG_A3.TEX");
	POOL("_ENRG_A4.TEX");
	POOL("_ENRG_A5.TEX");
	POOL("_ENRG_A6.TEX");
	POOL("_ENRG_A7.TEX");
	POOL("_ENRG_A8.TEX");
	POOL("__ERDE_A.TEX");
	POOL("_ERDE_A2.TEX");
	POOL("_ERDE_A3.TEX");
	POOL("_ERDE_A4.TEX");
	POOL("_ERDE_A5.TEX");
	POOL("_ERDE_A6.TEX");
	POOL("__ERDE_B.TEX");
	POOL("_ERDE_B2.TEX");
	POOL("_ERDE_B3.TEX");
	POOL("_ERDE_B4.TEX");
	POOL("_ERDE_B5.TEX");
	POOL("_ERDE_B6.TEX");
	POOL("_ERDE_B7.TEX");
	POOL("_ERDE_B8.TEX");
	POOL("_WATER_A.TEX");
	POOL("WATER_A2.TEX");
	POOL("WATER_A3.TEX");
	POOL("WATER_A4.TEX");
	POOL("__TECH_D.TEX");
	POOL("_TECH_D2.TEX");
	POOL("_TECH_D3.TEX");
	POOL("_TECH_D4.TEX");
	POOL("__GIFT_A.TEX");
	POOL("_GIFT_A2.TEX");
	POOL("_GIFT_A3.TEX");
	POOL("_GIFT_A4.TEX");
	POOL("__GITT_A.TEX");
	POOL("_GITT_A2.TEX");
	POOL("_GITT_A3.TEX");
	POOL("_GITT_A4.TEX");
	POOL("_GITT_A5.TEX");
	POOL("_GITT_A6.TEX");
	POOL("_GITT_A7.TEX");
	POOL("_GITT_A8.TEX");
	POOL("__HOLZ_A.TEX");
	POOL("_HOLZ_A2.TEX");
	POOL("_HOLZ_A3.TEX");
	POOL("_HOLZ_A4.TEX");
	POOL("_HOLZ_A5.TEX");
	POOL("_HOLZ_A6.TEX");
	POOL("_HOLZ_A7.TEX");
	POOL("_HOLZ_A8.TEX");
	POOL("__HOLZ_B.TEX");
	POOL("_HOLZ_B2.TEX");
	POOL("_HOLZ_B3.TEX");
	POOL("_HOLZ_B4.TEX");
	POOL("_HOLZ_B5.TEX");
	POOL("_HOLZ_B6.TEX");
	POOL("_HOLZ_B7.TEX");
	POOL("_HOLZ_B8.TEX");
	POOL("__HOLZ_C.TEX");
	POOL("_HOLZ_C2.TEX");
	POOL("_HOLZ_C3.TEX");
	POOL("_HOLZ_C4.TEX");
	POOL("_HOLZ_C5.TEX");
	POOL("_HOLZ_C6.TEX");
	POOL("_HOLZ_C7.TEX");
	POOL("_HOLZ_C8.TEX");
	POOL("__HOLZ_D.TEX");
	POOL("_HOLZ_D2.TEX");
	POOL("_HOLZ_D3.TEX");
	POOL("_HOLZ_D4.TEX");
	POOL("_HOLZ_D5.TEX");
	POOL("_HOLZ_D6.TEX");
	POOL("_HOLZ_D7.TEX");
	POOL("_HOLZ_D8.TEX");
	POOL("__HOLZ_E.TEX");
	POOL("_HOLZ_E2.TEX");
	POOL("_HOLZ_E3.TEX");
	POOL("_HOLZ_E4.TEX");
	POOL("_HOLZ_E5.TEX");
	POOL("_HOLZ_E6.TEX");
	POOL("_HOLZ_E7.TEX");
	POOL("_HOLZ_E8.TEX");
	POOL("__HOLZ_F.TEX");
	POOL("_HOLZ_F2.TEX");
	POOL("_HOLZ_F3.TEX");
	POOL("_HOLZ_F4.TEX");
	POOL("_HOLZ_F5.TEX");
	POOL("_HOLZ_F6.TEX");
	POOL("_HOLZ_F7.TEX");
	POOL("_HOLZ_F8.TEX");
	POOL("__LAVA_A.TEX");
	POOL("_LAVA_A2.TEX");
	POOL("_LAVA_A3.TEX");
	POOL("_LAVA_A4.TEX");
	POOL("__MET1_A.TEX");
	POOL("_MET1_A2.TEX");
	POOL("_MET1_A3.TEX");
	POOL("_MET1_A4.TEX");
	POOL("_MET1_A5.TEX");
	POOL("_MET1_A6.TEX");
	POOL("_MET1_A7.TEX");
	POOL("_MET1_A8.TEX");
	POOL("_MET1_B6.TEX");
	POOL("_MET1_B7.TEX");
	POOL("_MET1_B8.TEX");
	POOL("__MET1_C.TEX");
	POOL("_MET1_C2.TEX");
	POOL("_MET1_C3.TEX");
	POOL("_MET1_C4.TEX");
	POOL("_MET1_C5.TEX");
	POOL("_MET1_C6.TEX");
	POOL("_MET1_C7.TEX");
	POOL("_MET1_C8.TEX");
	POOL("_MET1_D4.TEX");
	POOL("_MET1_D5.TEX");
	POOL("_MET1_D6.TEX");
	POOL("_MET1_D7.TEX");
	POOL("__MET1_E.TEX");
	POOL("_MET1_E2.TEX");
	POOL("_MET1_E3.TEX");
	POOL("_MET1_E4.TEX");
	POOL("_MET1_E5.TEX");
	POOL("_MET1_E6.TEX");
	POOL("_MET1_E7.TEX");
	POOL("_MET1_E8.TEX");
	POOL("__MET1_F.TEX");
	POOL("_MET1_F2.TEX");
	POOL("_MET1_F3.TEX");
	POOL("_MET1_F4.TEX");
	POOL("_MET1_F5.TEX");
	POOL("_MET1_F6.TEX");
	POOL("_MET1_F7.TEX");
	POOL("_MET1_F8.TEX");
	POOL("__MET1_G.TEX");
	POOL("_MET1_G2.TEX");
	POOL("_MET1_G3.TEX");
	POOL("_MET1_G4.TEX");
	POOL("_MET1_G5.TEX");
	POOL("_MET1_G6.TEX");
	POOL("_MET1_G7.TEX");
	POOL("_MET1_G8.TEX");
	POOL("_MET2_D5.TEX");
	POOL("_MET2_D6.TEX");
	POOL("__MET3_A.TEX");
	POOL("_MET3_A2.TEX");
	POOL("_MET3_A3.TEX");
	POOL("_MET3_A4.TEX");
	POOL("_MET3_A5.TEX");
	POOL("_MET3_A6.TEX");
	POOL("_MET3_A7.TEX");
	POOL("_MET3_B5.TEX");
	POOL("_MET3_B6.TEX");
	POOL("_MET3_B7.TEX");
	POOL("_MET3_B8.TEX");
	POOL("__MET3_C.TEX");
	POOL("_MET3_C2.TEX");
	POOL("_MET3_C3.TEX");
	POOL("_MET3_C4.TEX");
	POOL("__MET4_A.TEX");
	POOL("_MET4_A2.TEX");
	POOL("_MET4_A3.TEX");
	POOL("_MET4_A4.TEX");
	POOL("_MET4_A5.TEX");
	POOL("_MET4_A6.TEX");
	POOL("_MET4_A7.TEX");
	POOL("_MET4_A8.TEX");
	POOL("__MET4_B.TEX");
	POOL("_MET4_B2.TEX");
	POOL("_MET4_B3.TEX");
	POOL("_MET4_B4.TEX");
	POOL("_MET4_B5.TEX");
	POOL("_MET4_B6.TEX");
	POOL("_MET4_B7.TEX");
	POOL("_MET4_B8.TEX");
	POOL("__MET4_C.TEX");
	POOL("_MET4_C2.TEX");
	POOL("_MET4_C3.TEX");
	POOL("_MET4_C4.TEX");
	POOL("_MET4_C5.TEX");
	POOL("_MET4_C6.TEX");
	POOL("_MET4_C7.TEX");
	POOL("_MET4_C8.TEX");
	POOL("__MET4_D.TEX");
	POOL("_MET4_D2.TEX");
	POOL("_MET4_D3.TEX");
	POOL("_MET4_D4.TEX");
	POOL("_MET4_D5.TEX");
	POOL("_MET4_D6.TEX");
	POOL("_MET4_D7.TEX");
	POOL("_MET4_D8.TEX");
	POOL("METAL_A4.TEX");
	POOL("METAL_A6.TEX");
	POOL("METAL_A7.TEX");
	POOL("METAL_A8.TEX");
	POOL("_ROTOR_A.TEX");
	POOL("ROTOR_A2.TEX");
	POOL("ROTOR_A3.TEX");
	POOL("ROTOR_A4.TEX");
	POOL("ROTOR_A5.TEX");
	POOL("ROTOR_A6.TEX");
	POOL("_STEIN_A.TEX");
	POOL("STEIN_A2.TEX");
	POOL("STEIN_A3.TEX");
	POOL("STEIN_A4.TEX");
	POOL("STEIN_A5.TEX");
	POOL("STEIN_A6.TEX");
	POOL("STEIN_A7.TEX");
	POOL("STEIN_A8.TEX");
	POOL("_STEIN_B.TEX");
	POOL("STEIN_B2.TEX");
	POOL("STEIN_B3.TEX");
	POOL("STEIN_B4.TEX");
	POOL("STEIN_B5.TEX");
	POOL("STEIN_B6.TEX");
	POOL("STEIN_B7.TEX");
	POOL("STEIN_B8.TEX");
	POOL("_STEIN_C.TEX");
	POOL("STEIN_C2.TEX");
	POOL("STEIN_C3.TEX");
	POOL("STEIN_C4.TEX");
	POOL("STEIN_C5.TEX");
	POOL("STEIN_C6.TEX");
	POOL("STEIN_C7.TEX");
	POOL("STEIN_C8.TEX");
	POOL("__TECH_A.TEX");
	POOL("_TECH_A2.TEX");
	POOL("_TECH_A3.TEX");
	POOL("_TECH_A4.TEX");
	POOL("_TECH_A5.TEX");
	POOL("_TECH_A6.TEX");
	POOL("_TECH_A7.TEX");
	POOL("__TECH_B.TEX");
	POOL("_TECH_B2.TEX");
	POOL("_TECH_B3.TEX");
	POOL("_TECH_B4.TEX");
	POOL("_TECH_B5.TEX");
	POOL("_TECH_B6.TEX");
	POOL("_TECH_B7.TEX");
	POOL("__TECH_C.TEX");
	POOL("_TECH_C2.TEX");
	POOL("_TECH_C3.TEX");
	POOL("_TECH_C4.TEX");
	POOL("_TECH_C5.TEX");
	POOL("_TECH_C6.TEX");
	POOL("_TECH_C7.TEX");
	POOL("_FELS_B5.TEX");
	POOL("_FELS_B6.TEX");
	POOL("_FELS_B7.TEX");
	POOL("_FELS_B8.TEX");
	POOL("__FELS_B.TEX");
	POOL("_FELS_B2.TEX");
	POOL("_FELS_B3.TEX");
	POOL("_FELS_B4.TEX");
	POOL("__MET5_A.TEX");
	POOL("_MET5_A2.TEX");
	POOL("_MET5_A3.TEX");
	POOL("_MET5_A4.TEX");
	POOL("_MET5_A5.TEX");
	POOL("_MET5_A6.TEX");
	POOL("_MET5_A7.TEX");
	POOL("_MET5_A8.TEX");
	POOL("__MET5_B.TEX");
	POOL("_MET5_B2.TEX");
	POOL("_MET5_B3.TEX");
	POOL("_MET5_B4.TEX");
	POOL("_MET5_B5.TEX");
	POOL("_MET5_B6.TEX");
	POOL("_MET5_B7.TEX");
	POOL("_MET5_B8.TEX");
	POOL("COLOR_A3.TEX");
	POOL("COLOR_A4.TEX");
	POOL("COLOR_A2.TEX");
	POOL("BODEN_A5.TEX");
	POOL("BODEN_A6.TEX");
	POOL("BODEN_A7.TEX");
	POOL("__HOLZ_G.TEX");
	POOL("_HOLZ_G2.TEX");
	POOL("_HOLZ_G3.TEX");
	POOL("_HOLZ_G4.TEX");
	*/
	return 0;
}