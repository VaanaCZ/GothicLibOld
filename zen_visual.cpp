#include "zen_visual.h"

using namespace GothicLib;

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

bool ZenGin::zCDecal::Archive(zCArchiver* archiver)
{
	if (!zCVisual::Archive(archiver))
		return false;

	return false;
}

bool ZenGin::zCDecal::Unarchive(zCArchiver* archiver)
{
	if (!zCVisual::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(name));
	archiver->ReadRawFloat(ARC_ARGSF(decalDim));
	archiver->ReadRawFloat(ARC_ARGSF(decalOffset));
	archiver->ReadBool(ARC_ARGS(decal2Sided));
	archiver->ReadEnum(ARC_ARGSE(decalAlphaFunc));
	archiver->ReadFloat(ARC_ARGS(decalTexAniFPS));
	// todo: decalAlphaWeight - G2
	// todo: ignoreDayLight - G2

	return true;
}

bool ZenGin::zCDecal::Save(FileStream* file)
{
	return true;
}

bool ZenGin::zCDecal::Load(FileStream* file)
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

bool GothicLib::ZenGin::zCMesh::SaveMSH(FileStream* file)
{
	return false;
}

bool GothicLib::ZenGin::zCMesh::LoadMSH(FileStream* file)
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