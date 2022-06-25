#pragma once

#include "zen_base.h"

#include "zen_vdfs.h"
#include "zen_world.h"


ZEN::zCObject* ZEN::zCObject::CreateObject(const char* className)
{
#define CLASS_CREATE_ATTEMPT(c)									\
	else if (strcmp(c::GetStaticClassname(), className))	\
		return new c();

	if (strcmp(className, "zCObject") == 0)
	{
		return nullptr;
	}

	CLASS_CREATE_ATTEMPT(zCWorld)
	CLASS_CREATE_ATTEMPT(oCWorld)
	
	return nullptr;
}

bool ZEN::zCArchiver::Read(FileStream* file)
{
	// Read header
	std::string line;

	/*
		ZenGin Archive
	*/
	file->ReadLine(line);

	if (line != "ZenGin Archive") 
		return false;

	/*
		ver 0 / ver 1
	*/
	file->ReadLine(line);

	if (line == "ver 0")			version = 0;
	else if (line == "ver 1")		version = 1;
	else							return false;

	/*
		zCArchiverGeneric / zCArchiverBinSafe
	*/
	if (version == 1)
	{
		file->ReadLine(line);
	}

	/*
		ASCII / BINARY / BIN_SAFE
	*/

	file->ReadLine(line);

	if (line == "ASCII")			type = ARCHIVER_TYPE_ASCII;
	else if (line == "BINARY")		type = ARCHIVER_TYPE_BINARY;
	else if (line == "BIN_SAFE")	type = ARCHIVER_TYPE_BIN_SAFE;
	else							return false;

	/*
		saveGame 0 / saveGame 1
	*/

	file->ReadLine(line);

	bool noSavegame = false;

	if (line == "saveGame 0")		savegame = false;
	else if (line == "saveGame 1")	savegame = true;
	else
	{
		savegame = false;
		noSavegame = true;
	}

	/*
		date DD.MM.YYYY HH:MM:SS
	*/

	if (!noSavegame)
	{
		file->ReadLine(line);
	}

	if (version == 0)
	{
		/*
			objects n
		*/

		file->ReadLine(line);

		uint32_t objectCount = std::stoul(line.substr(8));

		if (objectCount == 0)
			return false;

		objectList.resize(objectCount);

		/*
			csum 00000000
		*/

		if (noSavegame)
		{
			file->ReadLine(line);
		}
	}
	else if (version == 1)
	{
		/*
			user x
		*/

		file->ReadLine(line);
	}

	/*
		END
	*/

	file->ReadLine(line);

	if (line != "END")
		return false;

	if (version == 1 &&
		type != ARCHIVER_TYPE_BIN_SAFE)
	{
		/*
			objects n
		*/

		file->ReadLine(line);

		uint32_t objectCount = std::stoul(line.substr(8));

		if (objectCount == 0)
			return false;

		objectList.resize(objectCount);

		/*
			END
		*/

		file->ReadLine(line);

		if (line != "END")
			return false;

		/*
			*empty line*
		*/

		file->ReadLine(line);

		if (!line.empty())
			return false;
	}
	
	// Read objects

	return true;
}
