#include "zen_visual.h"

using namespace GothicLib::ZenGin;

/*
	Visual classes
*/

/*
	zCMaterial
*/

bool zCMaterial::Archive(zCArchiver* archiver)
{
	if (!zCObject::Unarchive(archiver))
		return false;

	return true;
}

bool zCMaterial::Unarchive(zCArchiver* archiver)
{
	if (!zCObject::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(name));
	archiver->ReadEnum(ARC_ARGSE(matGroup));
	archiver->ReadColor(ARC_ARGS(color));
	archiver->ReadFloat(ARC_ARGS(smoothAngle));
	archiver->ReadString(ARC_ARGS(texture));

	std::string texScaleStr;
	archiver->ReadString("texScale", texScaleStr);
	if (texScaleStr.find(" ") != std::string::npos)
	{
		texScale.x = atof(&texScaleStr[0]);
		texScale.y = atof(&texScaleStr[texScaleStr.find(" ") + 1]);
	}
	
	archiver->ReadFloat(ARC_ARGS(texAniFPS));
	archiver->ReadEnum(ARC_ARGSE(texAniMapMode));

	std::string texAniMapDirStr;
	archiver->ReadString("texAniMapDir", texAniMapDirStr);
	if (texAniMapDirStr.find(" ") != std::string::npos)
	{
		texAniMapDir.x = atof(&texAniMapDirStr[0]);
		texAniMapDir.y = atof(&texAniMapDirStr[texAniMapDirStr.find(" ") + 1]);
	}

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadBool(ARC_ARGS(noCollDet));
		archiver->ReadBool(ARC_ARGS(noLightmap));
		archiver->ReadBool(ARC_ARGS(lodDontCollapse));
		archiver->ReadString(ARC_ARGS(detailObject));
	}

	if (game >= GAME_GOTHIC2)
	{
		archiver->ReadFloat(ARC_ARGS(detailObjectScale));
		archiver->ReadBool(ARC_ARGS(forceOccluder));
		archiver->ReadBool(ARC_ARGS(environmentalMapping));
		archiver->ReadFloat(ARC_ARGS(environmentalMappingStrength));
		archiver->ReadEnum(ARC_ARGSE(waveMode));
		archiver->ReadEnum(ARC_ARGSE(waveSpeed));
		archiver->ReadFloat(ARC_ARGS(waveMaxAmplitude));
		archiver->ReadFloat(ARC_ARGS(waveGridSize));
		archiver->ReadBool(ARC_ARGS(ignoreSunLight));
		archiver->ReadEnum(ARC_ARGSE(alphaFunc));
	}

	if (game <= GAME_DEMO5)
	{
		archiver->ReadByte(ARC_ARGS(libFlag));
	}

	archiver->ReadRawFloat(ARC_ARGSF(defaultMapping));

	return true;
}

bool zCMaterial::Save(FileStream* file)
{
	return false;
}

bool zCMaterial::Load(FileStream* file)
{
	std::string line, key, value;

	file->ReadLine(line);
	ParseFileLine(line, key, value);

	if (key != "zCMaterial")
	{
		LOG_ERROR("Specified stream does not include a valid zCMaterial instance!");
		return false;
	}

	name = value;

	bool inMat = false;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inMat = true;
		}
		else if (inMat)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inMat = false;
				return true;
			}
			else if (ParseFileLine(line, key, value))
			{
				if (key == "matGroup")
				{
					if (value == "UNDEF")		matGroup = zMAT_GROUP_UNDEF;
					else if (value == "METAL")	matGroup = zMAT_GROUP_METAL;
					else if (value == "STONE")	matGroup = zMAT_GROUP_STONE;
					else if (value == "WOOD")	matGroup = zMAT_GROUP_WOOD;
					else if (value == "EARTH")	matGroup = zMAT_GROUP_EARTH;
					else if (value == "WATER")	matGroup = zMAT_GROUP_WATER;
				}
				else if (key == "matLibFlag")
				{
					libFlag = std::stoi(value);
				}
				else if (key == "matDefaultMapping")
				{
					if (value.find(" ") != std::string::npos)
					{
						defaultMapping.x = atof(&value[0]);
						defaultMapping.y = atof(&value[value.find(" ") + 1]);
					}
				}
				else if (key == "texture")
				{
					texture = value;
				}
				else if (key == "texScale")
				{
					if (value.find(" ") != std::string::npos)
					{
						texScale.x = atof(&value[0]);
						texScale.y = atof(&value[value.find(" ") + 1]);
					}
				}
				else if (key == "texAniFPS")
				{
					texAniFPS = std::stof(value);
				}
				else if (key == "texAniMapMode")
				{
					texAniMapMode = std::stoi(value);
				}
				else if (key == "texAniMapDir")
				{
					if (value.find(" ") != std::string::npos)
					{
						texAniMapDir.x = atof(&value[0]);
						texAniMapDir.y = atof(&value[value.find(" ") + 1]);
					}
				}
				else if (key == "colorRGBA")
				{
					float r, g, b, a;

					if (sscanf_s(value.c_str(), "%f %f %f %f", &r, &g, &b, &a) != 4)
					{
						return false;
					}

					color.r = r;
					color.g = g;
					color.b = b;
					color.a = a;
				}
				else if (key == "smooth")
				{
					// ignore
					// this will be calculated automatically
					// if smoothAngle is larger that 0
				}
				else if (key == "smoothAngle")
				{
					smoothAngle = std::stof(value);
				}
			}
		}
	}

	return false;
}

/*
	zCVisual
		zCDecal
		zCFlash
		zCMesh
		zCParticleFX
			zCUnderwaterPFX
		zCPolyStrip
			zCFlash::zCBolt
			zCLightning::zCBolt
		zCProgMeshProto
			zCMeshSoftSkin
		zCQuadMark
		zCVisualAnimate
			zCModel
			zCMorphMesh
*/

bool zCDecal::Archive(zCArchiver* archiver)
{
	if (!zCVisual::Archive(archiver))
		return false;

	archiver->WriteString(ARC_ARGS(name));
	archiver->WriteRawFloat(ARC_ARGSF(decalDim));
	archiver->WriteRawFloat(ARC_ARGSF(decalOffset));
	archiver->WriteBool(ARC_ARGS(decal2Sided));
	archiver->WriteEnum(ARC_ARGSEW(decalAlphaFunc, "MAT_DEFAULT;NONE;BLEND;ADD;SUB;MUL;MUL2"));
	archiver->WriteFloat(ARC_ARGS(decalTexAniFPS));

	if (game >= GAME_GOTHIC2)
	{
		archiver->WriteByte(ARC_ARGS(decalAlphaWeight));
		archiver->WriteBool(ARC_ARGS(ignoreDayLight));
	}

	return true;
}

bool zCDecal::Unarchive(zCArchiver* archiver)
{
	if (!zCVisual::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(name));
	archiver->ReadRawFloat(ARC_ARGSF(decalDim));
	archiver->ReadRawFloat(ARC_ARGSF(decalOffset));
	archiver->ReadBool(ARC_ARGS(decal2Sided));
	archiver->ReadEnum(ARC_ARGSE(decalAlphaFunc));
	archiver->ReadFloat(ARC_ARGS(decalTexAniFPS));

	if (game >= GAME_GOTHIC2)
	{
		archiver->ReadByte(ARC_ARGS(decalAlphaWeight));
		archiver->ReadBool(ARC_ARGS(ignoreDayLight));
	}

	return true;
}

bool zCDecal::Save(FileStream* file)
{
	file->WriteLine("zCDecal ()", "\n");
	file->WriteLine("{", "\n");

	char buff[256];

	sprintf_s(buff, 256, "%.8g %.8g", decalDim.x, decalDim.y);
	file->WriteLine("decalDim (" + std::string(buff) + ")", "\n");

	sprintf_s(buff, 256, "%.8g %.8g", decalOffset.x, decalOffset.y);
	file->WriteLine("decalOffset (" + std::string(buff) + ")", "\n");

	file->WriteLine("decal2Sided (" + std::to_string(decal2Sided) + ")", "\n");

	if (decalAlphaFunc == zRND_ALPHA_FUNC_MAT_DEFAULT)
		file->WriteLine("decalAlphaFunc (MAT_DEFAULT)", "\n");
	else if (decalAlphaFunc == zRND_ALPHA_FUNC_NONE)
		file->WriteLine("decalAlphaFunc (NONE)", "\n");
	else if (decalAlphaFunc == zRND_ALPHA_FUNC_BLEND)
		file->WriteLine("decalAlphaFunc (BLEND)", "\n");
	else if (decalAlphaFunc == zRND_ALPHA_FUNC_ADD)
		file->WriteLine("decalAlphaFunc (ADD)", "\n");
	else if (decalAlphaFunc == zRND_ALPHA_FUNC_SUB)
		file->WriteLine("decalAlphaFunc (SUB)", "\n");
	else if (decalAlphaFunc == zRND_ALPHA_FUNC_MUL)
		file->WriteLine("decalAlphaFunc (MUL)", "\n");

	file->WriteLine("decalTexAniFPS (" + FloatToString(decalTexAniFPS) + ")", "\n");

	file->WriteLine("}", "\n");

	return true;
}

bool zCDecal::Load(FileStream* file)
{
	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return true;
			}
			else if (ParseFileLine(line, key, value))
			{
				if (key == "decalDim")
				{
					if (sscanf_s(value.c_str(), "%f %f",
						&decalDim.x, &decalDim.y) != 2)
					{
						return false;
					}
				}
				else if (key == "decalOffset")
				{
					if (sscanf_s(value.c_str(), "%f %f",
						&decalOffset.x, &decalOffset.y) != 2)
					{
						return false;
					}
				}
				else if (key == "decal2Sided")
				{
					decal2Sided = (value == "1");
				}
				else if (key == "decalAlphaFunc")
				{
					if (value == "MAT_DEFAULT")		decalAlphaFunc = zRND_ALPHA_FUNC_MAT_DEFAULT;
					else if (value == "NONE")		decalAlphaFunc = zRND_ALPHA_FUNC_NONE;
					else if (value == "BLEND")		decalAlphaFunc = zRND_ALPHA_FUNC_BLEND;
					else if (value == "ADD")		decalAlphaFunc = zRND_ALPHA_FUNC_ADD;
					else if (value == "SUB")		decalAlphaFunc = zRND_ALPHA_FUNC_SUB;
					else if (value == "MUL")		decalAlphaFunc = zRND_ALPHA_FUNC_MUL;
				}
				else if (key == "decalTexAniFPS")
				{
					decalTexAniFPS = std::stof(value);
				}
			}
		}
	}

	return false;
}

bool zCOBBox3D::LoadBIN(FileStream* file)
{
	file->Read(&center, sizeof(center));
	file->Read(&axis, sizeof(axis));
	file->Read(&extent, sizeof(extent));

	if (readChilds)
	{
		uint16_t childCount = 0;
		file->Read(&childCount, sizeof(childCount));

		if (childCount > 0)
		{
			childs.resize(childCount);

			for (size_t i = 0; i < childCount; i++)
			{
				childs[i].LoadBIN(file);
			}
		}
	}

	return true;
}

bool zCOBBox3D::SaveBIN(FileStream* file)
{
	return false;
}

bool zCMesh::SaveMSH(FileStream* file)
{
	return false;
}

bool zCMesh::LoadMSH(FileStream* file)
{
	bool end = false;

	while (true)
	{
		uint16_t chunkId;
		uint32_t chunkSize;

		if (!file->Read(&chunkId, sizeof(chunkId)) ||
			!file->Read(&chunkSize, sizeof(chunkSize)))
		{
			LOG_ERROR("Unexpected end of file while reading mesh chunk!");
			return false;
		}

		LOG_DEBUG("Reading zCMesh chunk of id " + std::to_string(chunkId));

		uint64_t skipPos = file->Tell() + chunkSize;

		// Read chunk
		switch (chunkId)
		{

		case CHUNK_MESHSTART:
		{
			uint16_t	version;
			zDATE		date;
			std::string	name;

			file->Read(&version, sizeof(version));
			file->Read(&date, sizeof(date));
			file->ReadLine(name);

			break;
		}

		case CHUNK_BBOX:
		{
			zTBBox3D bbox;
			zCOBBox3D obbox;

			if (game <= GAME_DEMO5)
			{
				obbox.readChilds = false;
			}

			file->Read(&bbox, sizeof(bbox));
			obbox.LoadBIN(file);

			break;
		}

		case CHUNK_MATERIALS:
		{
			zCArchiver archiver;
			int materialCount = 0;

			if (game >= GAME_DEMO5)
			{
				archiver.Read(file);
				archiver.game = game;

				archiver.ReadInt("", materialCount);
			}
			else
			{
				file->Read(&materialCount, sizeof(materialCount));
			}

			if (materialCount > 0)
			{
				materials.resize(materialCount);

				for (size_t i = 0; i < materialCount; i++)
				{
					std::string materialName;
					materials[i].game = game;

					if (game >= GAME_DEMO5)
					{
						archiver.ReadString("", materialName);
						archiver.ReadObjectAs<zCMaterial*>(&materials[i]);
					}
					else
					{
						file->ReadLine(materialName);
						materials[i].Load(file);
					}
				}
			}

			zBOOL alphaTestingEnabled;
			
			if (game >= GAME_GOTHIC2)
			{
				archiver.ReadBool("", alphaTestingEnabled);
			}

			//archiver.Close();

			break;
		}

		case CHUNK_LIGHTMAPS_OLD:
		{
			break;
		}

		case CHUNK_LIGHTMAPS_NEW:
		{
			break;
		}

		case CHUNK_VERTS:
		{
			break;
		}

		case CHUNK_FEATS:
		{
			break;
		}

		case CHUNK_POLYS:
		{
			break;
		}

		case CHUNK_MESHEND:
		{
			end = true;
			break;
		}

		default:
		{
			LOG_WARN("Unknown chunk type \"" + std::to_string(chunkId) + "\" in zCMesh, skipping!");
			file->Seek(skipPos);

			break;
		}
		}

		if (end)
		{
			break;
		}
	}

	return false;
}
