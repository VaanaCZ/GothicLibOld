#include "ge2_world.h"

using namespace GothicLib::Genome2;

bool gCProject::OnWrite(FileStream* file)
{
	return true;
}

bool gCProject::OnRead(FileStream* file)
{
	uint16_t worldCount = 0;
	file->Read(FILE_ARGS(worldCount));

	for (size_t i = 0; i < worldCount; i++)
	{
		std::string world;
		file->ReadString(world);
		worlds.push_back(world);
	}

	return true;
}
