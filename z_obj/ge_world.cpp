#include "ge_world.h"

using namespace GothicLib::Genome;

bool gCProject::OnWrite(FileStream* file)
{
	return false;
}

bool gCProject::OnRead(FileStream* file)
{
	uint16_t version;
	uint32_t count;

	file->Read(FILE_ARGS(version));
	file->Read(FILE_ARGS(count));

	for (size_t i = 0; i < count; i++)
	{
		std::string worldName;
		file->ReadString(worldName);
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
