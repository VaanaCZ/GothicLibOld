//
// ge2_world.cpp
//
// Copyright (c) 2021-2023 Václav Maroušek
//

#include "ge2_world.h"

using namespace GothicLib::Genome2;

bool gCProject::OnWrite(FileStream* file, GAME game)
{
	return true;
}

bool gCProject::OnRead(FileStream* file, GAME game)
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
