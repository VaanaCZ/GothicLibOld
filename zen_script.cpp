#include "zen_script.h"

using namespace GothicLib::ZenGin;

bool zCPar_Symbol::Save(FileStream* file)
{
	return false;
}

bool zCPar_Symbol::Load(FileStream* file)
{
	zBOOL hasName;
	file->Read(FILE_ARGS(hasName));

	if (hasName)
	{
		file->ReadLine(name);
	}

	file->Read(FILE_ARGS(offset));

	file->Read(FILE_ARGS(elemProps));
	file->Read(FILE_ARGS(fileIndex));
	file->Read(FILE_ARGS(lineStart));
	file->Read(FILE_ARGS(lineCount));
	file->Read(FILE_ARGS(charStart));
	file->Read(FILE_ARGS(charCount));

	if ((elemProps & 4) == 0)
	{

	}

	return true;
}

bool zCPar_File::Save(FileStream* file)
{
	return false;
}

bool zCPar_File::Load(FileStream* file)
{
	uint8_t version;
	file->Read(FILE_ARGS(version));

	if (version != 50)
	{
		LOG_ERROR("Unknown script version \"" + std::to_string(version) + "\" expected 50");
		return false;
	}

	uint32_t symbolCount;
	file->Read(FILE_ARGS(symbolCount));

	symbols.resize(symbolCount);

	uint32_t* sortTable = new uint32_t[symbolCount];
	file->Read(sortTable, sizeof(uint32_t) * symbolCount);

	for (size_t i = 0; i < symbolCount; i++)
	{
		if (!symbols[i].Load(file))
		{
			delete[] sortTable;
			return false;
		}
	}

	delete[] sortTable;

	return true;
}
