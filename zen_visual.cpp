#include "zen_visual.h"

using namespace GothicLib::ZenGin;

/*
	Visual classes
*/

/*
	zCMaterial
*/

bool zCMaterial::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Archive(archiver, game))
		return false;
	
	archiver->WriteString(ARC_ARGS(name));
	archiver->WriteEnum(ARC_ARGSEW(matGroup, "UNDEF;METAL;STONE;WOOD;EARTH;WATER;SNOW"));
	archiver->WriteColor(ARC_ARGS(color));
	archiver->WriteFloat(ARC_ARGS(smoothAngle));
	archiver->WriteString(ARC_ARGS(texture));
	archiver->WriteString("texScale", FloatToString(texScale.x) + " " + FloatToString(texScale.y));
	archiver->WriteFloat(ARC_ARGS(texAniFPS));
	archiver->WriteEnum(ARC_ARGSEW(texAniMapMode, "NONE;LINEAR"));
	archiver->WriteString("texAniMapDir", FloatToString(texAniMapDir.x) + " " + FloatToString(texAniMapDir.y));

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteBool(ARC_ARGS(noCollDet));
		archiver->WriteBool(ARC_ARGS(noLightmap));
		archiver->WriteBool(ARC_ARGS(lodDontCollapse));
		archiver->WriteString(ARC_ARGS(detailObject));
	}

	if (game >= GAME_GOTHIC2)
	{
		archiver->WriteFloat(ARC_ARGS(detailObjectScale));
		archiver->WriteBool(ARC_ARGS(forceOccluder));
		archiver->WriteBool(ARC_ARGS(environmentalMapping));
		archiver->WriteFloat(ARC_ARGS(environmentalMappingStrength));
		archiver->WriteEnum(ARC_ARGSEW(waveMode, "NONE;AMBIENT_GROUND;GROUND;AMBIENT_WALL;WALL;ENV;AMBIENT_WIND;WIND"));
		archiver->WriteEnum(ARC_ARGSEW(waveSpeed, "NONE;SLOW;NORMAL;FAST"));
		archiver->WriteFloat(ARC_ARGS(waveMaxAmplitude));
		archiver->WriteFloat(ARC_ARGS(waveGridSize));
		archiver->WriteBool(ARC_ARGS(ignoreSunLight));
		archiver->WriteEnum(ARC_ARGSEW(alphaFunc, "MAT_DEFAULT;NONE;BLEND;ADD;SUB;MUL;MUL2"));
	}

	if (game <= GAME_DEMO5)
	{
		archiver->WriteByte(ARC_ARGS(libFlag));
	}

	archiver->WriteRawFloat(ARC_ARGSF(defaultMapping));

	return true;
}

bool zCMaterial::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Unarchive(archiver, game))
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

bool zCMaterial::Save(FileStream* file, GAME game)
{
	return false;
}

bool zCMaterial::Load(FileStream* file, GAME game)
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

bool zCDecal::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCVisual::Archive(archiver, game))
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

bool zCDecal::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCVisual::Unarchive(archiver, game))
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

bool zCDecal::Save(FileStream* file, GAME game)
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

bool zCDecal::Load(FileStream* file, GAME game)
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

bool zCOBBox3D::LoadBIN(FileStream* file, GAME game)
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
				childs[i].LoadBIN(file, game);
			}
		}
	}

	return true;
}

bool zCOBBox3D::SaveBIN(FileStream* file, GAME game)
{
	file->Write(&center, sizeof(center));
	file->Write(&axis, sizeof(axis));
	file->Write(&extent, sizeof(extent));

	if (readChilds)
	{
		uint16_t childCount = childs.size();
		file->Write(&childCount, sizeof(childCount));

		for (size_t i = 0; i < childs.size(); i++)
		{
			childs[i].SaveBIN(file, game);
		}
	}

	return true;
}

inline int CalcNormalMainAxis(zTPlane plane)
{
	int axis;

	float r[3];
	r[0] = fabs(plane.normal.x);
	r[1] = fabs(plane.normal.y);
	r[2] = fabs(plane.normal.z);

	axis = (r[0] < r[1]);
	if (r[2] > r[axis])
		axis = 2;

	return axis;
};

bool zCMesh::SaveMSH(FileStream* file, GAME game)
{
	// Mesh start
	uint64_t meshStart = file->StartBinChunk(MESHCHUNK_MESHSTART);

	uint16_t version = 0;

	if (game >= GAME_GOTHIC2)
	{
		version = 0x0109;
	}
	else
	{
		version = 0x0009;
	}

	file->Write(FILE_ARGS(version));
	file->Write(FILE_ARGS(date));
	file->WriteLine(name, "\n");

	file->EndBinChunk(meshStart);

	// Bbox
	uint64_t bboxStart = file->StartBinChunk(MESHCHUNK_BBOX);

	file->Write(FILE_ARGS(bbox));
	obbox.SaveBIN(file, game);

	file->EndBinChunk(bboxStart);

	// Materials
	uint64_t materialsStart = file->StartBinChunk(MESHCHUNK_MATERIALS);

	zCArchiver archiver;
	archiver.mode = zCArchiver::ARCHIVER_MODE_BINARY;
	archiver.Write(file, true);

	if (game >= GAME_DEMO5)
	{
		archiver.WriteInt("", materials.size());
	}
	else
	{
		uint32_t materialCount = materials.size();
		file->Write(FILE_ARGS(materialCount));
	}

	for (size_t i = 0; i < materials.size(); i++)
	{
		if (game >= GAME_DEMO5)
		{
			archiver.WriteString("", materials[i].name);
			archiver.WriteObject(game, &materials[i]);
		}
		else
		{
			file->WriteLine(materials[i].name, "\r");
			materials[i].Save(file, game);
		}
	}

	if (game >= GAME_GOTHIC2)
	{
		archiver.WriteBool("", alphaTestingEnabled);
	}

	archiver.EndWrite();

	file->EndBinChunk(materialsStart);

	// Lightmaps
	if (oldLightmaps)
	{
		uint64_t lightmapsStart = file->StartBinChunk(MESHCHUNK_LIGHTMAPS_OLD);

		uint32_t textureCount = lightmaps.size();
		file->Write(FILE_ARGS(textureCount));

		for (size_t i = 0; i < lightmaps.size(); i++)
		{
			file->Write(FILE_ARGS(lightmaps[i].lmVectors));

			if (!textures[i].SaveTexture(file))
			{
				return false;
			}
		}

		file->EndBinChunk(lightmapsStart);
	}
	else
	{
		uint64_t lightmapsStart = file->StartBinChunk(MESHCHUNK_LIGHTMAPS_NEW);

		uint32_t textureCount = textures.size();
		file->Write(FILE_ARGS(textureCount));
	
		for (size_t i = 0; i < textures.size(); i++)
		{
			if (!textures[i].SaveTexture(file))
			{
				return false;
			}
		}

		// Read lightmaps
		uint32_t lightmapCount = lightmaps.size();
		file->Write(FILE_ARGS(lightmapCount));

		if (lightmaps.size() > 0)
		{
			file->Write(&lightmaps[0], sizeof(LightmapChunk) * lightmaps.size());
		}

		file->EndBinChunk(lightmapsStart);
	}

	// Verts
	uint64_t vertsStart = file->StartBinChunk(MESHCHUNK_VERTS);

	uint32_t vertCount = verts.size();
	file->Write(FILE_ARGS(vertCount));

	if (verts.size() > 0)
	{
		file->Write(&verts[0], sizeof(zVEC3) * verts.size());
	}

	file->EndBinChunk(vertsStart);

	// Feats
	uint64_t featsStart = file->StartBinChunk(MESHCHUNK_FEATS);

	uint32_t featCount = feats.size();
	file->Write(FILE_ARGS(featCount));

	if (feats.size() > 0)
	{
		file->Write(&feats[0], sizeof(Feature) * feats.size());
	}

	file->EndBinChunk(featsStart);
	
	// Polys
	uint64_t polysStart = file->StartBinChunk(MESHCHUNK_POLYS);

	uint32_t polyCount = polys.size();
	file->Write(FILE_ARGS(polyCount));

	for (size_t i = 0; i < polys.size(); i++)
	{
		file->Write(FILE_ARGS(polys[i].materialIndex));
		file->Write(FILE_ARGS(polys[i].lightmapIndex));
		file->Write(FILE_ARGS(polys[i].plane));

		if (game >= GAME_GOTHIC2)
		{
			file->Write(FILE_ARGS(polys[i].flags));
		}
		else
		{
			PolygonFlagsOld oldFlags;
			memset(&oldFlags, 0, sizeof(oldFlags));
			oldFlags.portalPoly				= polys[i].flags.portalPoly;
			oldFlags.occluder				= polys[i].flags.occluder;
			oldFlags.sectorPoly				= polys[i].flags.sectorPoly;
			oldFlags.lodFlag				= polys[i].flags.lodFlag;
			oldFlags.portalIndoorOutdoor	= polys[i].flags.portalIndoorOutdoor;
			oldFlags.ghostOccluder			= polys[i].flags.ghostOccluder;
			oldFlags.normalMainAxis			= CalcNormalMainAxis(polys[i].plane);
			oldFlags.sectorIndex			= polys[i].flags.sectorIndex;

			file->Write(FILE_ARGS(oldFlags));
		}

		uint8_t count = polys[i].verts.size();
		file->Write(FILE_ARGS(count));

		for (size_t j = 0; j < polys[i].verts.size(); j++)
		{
			int32_t vertIndex = polys[i].verts[j];
			int16_t oldvertIndex = polys[i].verts[j];
			uint32_t featIndex = polys[i].feats[j];

			if (game >= GAME_GOTHIC2)
			{
				file->Write(FILE_ARGS(vertIndex));
			}
			else
			{
				file->Write(FILE_ARGS(oldvertIndex));
			}

			file->Write(FILE_ARGS(featIndex));
		}
	}

	file->EndBinChunk(polysStart);

	// End
	uint64_t endStart = file->StartBinChunk(MESHCHUNK_MESHEND);
	file->EndBinChunk(endStart);

	return true;
}

bool zCMesh::LoadMSH(FileStream* file, GAME game)
{
	bool end = false;

	while (true)
	{
		uint16_t chunkId;
		uint32_t chunkSize;

		if (!file->Read(FILE_ARGS(chunkId)) ||
			!file->Read(FILE_ARGS(chunkSize)))
		{
			LOG_ERROR("Unexpected end of file while reading mesh chunk!");
			return false;
		}

		LOG_DEBUG("Reading zCMesh chunk of id " + std::to_string(chunkId));

		uint64_t skipPos = file->Tell() + chunkSize;

		// Read chunk
		switch (chunkId)
		{

		case MESHCHUNK_MESHSTART:
		{
			uint16_t version;

			file->Read(FILE_ARGS(version));
			file->Read(FILE_ARGS(date));
			file->ReadLine(name);

			if (game >= GAME_GOTHIC2)
			{
				if (version != 0x0109)
				{
					LOG_ERROR("Unknown zCMesh version \"" + std::to_string(version) + "\" found, expected version 0x0109!");
					return false;
				}
			}
			else
			{
				if (version != 0x0009)
				{
					LOG_ERROR("Unknown zCMesh version \"" + std::to_string(version) + "\" found, expected version 0x0009!");
					return false;
				}
			}

			break;
		}

		case MESHCHUNK_BBOX:
		{
			if (game <= GAME_DEMO5)
			{
				obbox.readChilds = false;
			}

			file->Read(FILE_ARGS(bbox));
			obbox.LoadBIN(file, game);

			break;
		}

		case MESHCHUNK_MATERIALS:
		{
			zCArchiver archiver;
			int materialCount = 0;

			if (game >= GAME_DEMO5)
			{
				archiver.Read(file);

				archiver.ReadInt("", materialCount);
			}
			else
			{
				file->Read(FILE_ARGS(materialCount));
			}

			if (materialCount > 0)
			{
				materials.resize(materialCount);

				for (size_t i = 0; i < materialCount; i++)
				{
					std::string materialName;

					if (game >= GAME_DEMO5)
					{
						archiver.ReadString("", materialName);
						archiver.ReadObjectAs<zCMaterial*>(game, &materials[i]);
					}
					else
					{
						file->ReadLine(materialName);
						materials[i].Load(file, game);
					}
				}
			}

			if (game >= GAME_GOTHIC2)
			{
				archiver.ReadBool("", alphaTestingEnabled);
			}

			//archiver.Close();

			break;
		}

		case MESHCHUNK_LIGHTMAPS_OLD:
		{
			oldLightmaps = true;

			// Read textures
 			uint32_t textureCount = 0;
			file->Read(FILE_ARGS(textureCount));
			if (textureCount > 0)
			{
				textures.resize(textureCount);
				lightmaps.resize(textureCount);

				zVEC3 lmVectors[3];

				for (size_t i = 0; i < textureCount; i++)
				{
					file->Read(FILE_ARGS(lmVectors));

					lightmaps[i].lmVectors[0] = lmVectors[0];
					lightmaps[i].lmVectors[1] = lmVectors[1];
					lightmaps[i].lmVectors[2] = lmVectors[2];
					lightmaps[i].texIndex = i;

					if (!textures[i].LoadTexture(file))
					{
						return false;
					}
				}
			}

			break;
		}
		
		case MESHCHUNK_LIGHTMAPS_NEW:
		{
			oldLightmaps = false;

			// Read textures
			uint32_t textureCount = 0;
			file->Read(FILE_ARGS(textureCount));
		
			if (textureCount > 0)
			{
				textures.resize(textureCount);

				for (size_t i = 0; i < textureCount; i++)
				{
					if (!textures[i].LoadTexture(file))
					{
						return false;
					}
				}
			}

			// Read lightmaps
			uint32_t lightmapCount = 0;
			file->Read(FILE_ARGS(lightmapCount));

			if (lightmapCount > 0)
			{
				lightmaps.resize(lightmapCount);
				file->Read(&lightmaps[0], sizeof(LightmapChunk) * lightmapCount);
			}

			break;
		}

		case MESHCHUNK_VERTS:
		{
			uint32_t vertCount = 0;
			file->Read(FILE_ARGS(vertCount));

			if (vertCount > 0)
			{
				verts.resize(vertCount);
				file->Read(&verts[0], sizeof(zVEC3) * vertCount);
			}

			break;
		}

		case MESHCHUNK_FEATS:
		{
			uint32_t featCount = 0;
			file->Read(FILE_ARGS(featCount));

			if (featCount > 0)
			{
				feats.resize(featCount);
				file->Read(&feats[0], sizeof(Feature) * featCount);
			}

			break;
		}

		case MESHCHUNK_POLYS:
		{
			uint32_t polyCount = 0;
			file->Read(FILE_ARGS(polyCount));

			if (polyCount > 0)
			{
				polys.resize(polyCount);

				for (size_t i = 0; i < polyCount; i++)
				{
					file->Read(FILE_ARGS(polys[i].materialIndex));
					file->Read(FILE_ARGS(polys[i].lightmapIndex));
					file->Read(FILE_ARGS(polys[i].plane));

					if (game >= GAME_GOTHIC2)
					{
						file->Read(FILE_ARGS(polys[i].flags));
					}
					else
					{
						PolygonFlagsOld oldFlags;
						file->Read(FILE_ARGS(oldFlags));

						polys[i].flags.portalPoly			= oldFlags.portalPoly;
						polys[i].flags.occluder				= oldFlags.occluder;
						polys[i].flags.sectorPoly			= oldFlags.sectorPoly;
						polys[i].flags.lodFlag				= oldFlags.lodFlag;
						polys[i].flags.portalIndoorOutdoor	= oldFlags.portalIndoorOutdoor;
						polys[i].flags.ghostOccluder		= oldFlags.ghostOccluder;
						polys[i].flags.sectorIndex			= oldFlags.sectorIndex;
					}

					uint8_t count;
					file->Read(FILE_ARGS(count));

					if (count > 0)
					{
						polys[i].verts.resize(count);
						polys[i].feats.resize(count);

						int32_t vertIndex;
						int16_t oldvertIndex;
						uint32_t featIndex;

						for (size_t j = 0; j < count; j++)
						{
							if (game >= GAME_GOTHIC2)
							{
								file->Read(FILE_ARGS(vertIndex));
								polys[i].verts[j] = vertIndex;
							}
							else
							{
								file->Read(FILE_ARGS(oldvertIndex));
								polys[i].verts[j] = oldvertIndex;
							}

							file->Read(FILE_ARGS(featIndex));
							polys[i].feats[j] = featIndex;
						}
					}
				}
			}

			break;
		}

		case MESHCHUNK_MESHEND:
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

	return true;
}
