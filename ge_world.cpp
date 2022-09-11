#include "ge_world.h"

using namespace GothicLib::Genome;

/*
	gCProject
*/

bool gCProject::OnWrite(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnWrite(file);
	}

	return WriteData(file);
}

bool gCProject::OnRead(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnRead(file);
	}

	return ReadData(file);
}

bool gCProject::DoLoadData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return ReadData(file);
	}

	return true;
}

bool gCProject::DoSaveData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return WriteData(file);
	}

	return true;
}

bool gCProject::WriteData(FileStream* file)
{
	uint16_t version	= 1;
	uint32_t count		= worlds.size();

	if (game >= GAME_RISEN1)
	{
		version = 200;
	}
	
	file->Write(FILE_ARGS(version));
	file->Write(FILE_ARGS(count));
	
	for (size_t i = 0; i < count; i++)
	{
		file->WriteString(worlds[i]);
	}

	return true;
}

bool gCProject::ReadData(FileStream* file)
{
	uint16_t version;
	uint32_t count;

	file->Read(FILE_ARGS(version));
	file->Read(FILE_ARGS(count));

	if (count > 0)
	{
		worlds.resize(count);

		for (size_t i = 0; i < count; i++)
		{
			file->ReadString(worlds[i]);
		}
	}

	return true;
}

/*
	gCWorld
*/

bool gCWorld::OnWrite(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnWrite(file);
	}

	return WriteData(file);
}

bool gCWorld::OnRead(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return bCObjectRefBase::OnRead(file);
	}

	return ReadData(file);
}

bool gCWorld::DoLoadData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return ReadData(file);
	}

	return true;
}

bool gCWorld::DoSaveData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return WriteData(file);
	}

	return true;
}

bool gCWorld::WriteData(FileStream* file)
{
	uint16_t version = 36;

	if (game >= GAME_RISEN1)
	{
		version = 200;
	}

	file->Write(FILE_ARGS(version));
	file->WriteString(sectorFile);

	return true;
}

bool gCWorld::ReadData(FileStream* file)
{
	uint16_t version;

	file->Read(FILE_ARGS(version));
	file->ReadString(sectorFile);

	return true;
}

/*
	gCSector
*/

bool gCSector::OnWrite(FileStream* file)
{
	uint16_t version = 27;

	if (game >= GAME_RISEN1)
	{
		version = 200;
	}

	file->Write(FILE_ARGS(version));

	if (version >= 27)
		file->Write(FILE_ARGS(enabled));

	if (game >= GAME_RISEN1)
	{
		return WriteData(file);
	}

	return true;
}

bool gCSector::OnRead(FileStream* file)
{
	uint16_t version;

	file->Read(FILE_ARGS(version));

	if (version >= 27)
		file->Read(FILE_ARGS(enabled));

	if (game >= GAME_RISEN1 &&
		version >= 200)
	{
		return ReadData(file);
	}

	return true;
}

bool gCSector::DoLoadData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		uint16_t version;

		file->Read(FILE_ARGS(version));

		return ReadData(file);
	}

	return true;
}

bool gCSector::DoSaveData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		uint16_t version = 27;

		file->Write(FILE_ARGS(version));

		return WriteData(file);
	}

	return true;
}

bool gCSector::WriteData(FileStream* file)
{
	uint32_t dynamicLayerCount	= dynamicLayers.size();
	uint32_t geometryLayerCount	= geometryLayers.size();

	file->Write(FILE_ARGS(dynamicLayerCount));
	file->Write(FILE_ARGS(geometryLayerCount));

	for (size_t i = 0; i < dynamicLayerCount; i++)
	{
		file->WriteString(dynamicLayers[i]);
	}

	if (geometryLayerCount > 0 &&
		game >= GAME_RISEN1)
	{
		LOG_WARN("Geometry layers are not supported in Risen, yet they are specified!");
		return false;
	}

	for (size_t i = 0; i < geometryLayerCount; i++)
	{
		file->WriteString(geometryLayers[i]);
	}

	return true;
}

bool gCSector::ReadData(FileStream* file)
{
	uint32_t dynamicLayerCount;
	uint32_t geometryLayerCount;

	file->Read(FILE_ARGS(dynamicLayerCount));
	file->Read(FILE_ARGS(geometryLayerCount));

	if (dynamicLayerCount > 0)
	{
		dynamicLayers.resize(dynamicLayerCount);

		for (size_t i = 0; i < dynamicLayerCount; i++)
		{
			file->ReadString(dynamicLayers[i]);
		}
	}

	if (geometryLayerCount > 0)
	{
		if (game >= GAME_RISEN1)
		{
			LOG_WARN("Geometry layers are not supported in Risen, yet they are specified!");
			return false;
		}

		geometryLayers.resize(geometryLayerCount);

		for (size_t i = 0; i < geometryLayerCount; i++)
		{
			file->ReadString(geometryLayers[i]);
		}
	}

	return true;
}

/*
	gCLayerBase
		gCGeometryLayer
		gCDynamicLayer
*/

bool gCGeometryLayer::OnWrite(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	return bCObjectBase::OnWrite(file);
}

bool gCGeometryLayer::OnRead(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	return bCObjectBase::OnRead(file);
}

bool gCGeometryLayer::DoLoadData(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	// Loading code

	return true;
}

bool gCGeometryLayer::DoSaveData(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		LOG_ERROR("Geometry layers are not supported in Risen!");
		return false;
	}

	// Saving code

	return true;
}

bool gCDynamicLayer::OnWrite(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		return WriteData(file);
	}

	return bCObjectBase::OnWrite(file);
}

bool gCDynamicLayer::OnRead(FileStream* file)
{
	if (game >= GAME_RISEN1)
	{
		return ReadData(file);
	}

	return bCObjectBase::OnRead(file);
}

bool gCDynamicLayer::DoLoadData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return ReadData(file);
	}

	return true;
}

bool gCDynamicLayer::DoSaveData(FileStream* file)
{
	if (game <= GAME_GOTHIC3)
	{
		return WriteData(file);
	}

	return true;
}

bool gCDynamicLayer::WriteData(FileStream* file)
{
	uint64_t magic = 0x4C44454D4F4E4547;
	file->Write(FILE_ARGS(magic));

	uint16_t version = 83;

	if (game >= GAME_RISEN1)
	{
		version = 200;
	}

	file->Read(FILE_ARGS(version));

	// context

	return true;
}

bool gCDynamicLayer::ReadData(FileStream* file)
{
	uint64_t magic;
	file->Read(FILE_ARGS(magic));

	if (magic != 0x4C44454D4F4E4547)
	{
		LOG_ERROR("Failed to read gCDynamicLayer, invalid magic bytes!");
		return false;
	}

	uint16_t version;
	file->Read(FILE_ARGS(version));

	// context

	return true;
}