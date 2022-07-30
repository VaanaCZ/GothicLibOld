#include "zen_visual.h"

using namespace GothicLib::ZenGin;

/*
	Visual classes
*/

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

		uint64_t skipPos = file->Tell() + chunkSize;

		// Read chunk
		switch (chunkId)
		{

		case CHUNK_MESH:
		{
			uint16_t	version;
			zDATE		date;
			std::string	name;
		}

		case CHUNK_END:
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
