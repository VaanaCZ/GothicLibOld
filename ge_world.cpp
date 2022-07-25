#include "ge_world.h"

using namespace GothicLib::Genome;

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
	
	if (file->Error())
	{
		LOG_ERROR("Error encountered while writing gCProject class!");
		return false;
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

	if (file->Error())
	{
		LOG_ERROR("Error encountered while reading gCProject class!");
		return false;
	}

	return true;
}

bool gCWorld::OnWrite(FileStream* file)
{
	return false;
}

bool gCWorld::OnRead(FileStream* file)
{
	return false;
}

bool gCWorld::DoLoadData(FileStream* file)
{
	return false;
}

bool gCWorld::DoSaveData(FileStream* file)
{
	return false;
}
