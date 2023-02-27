//
// zen_script.h
//
// Copyright (c) 2021-2023 Václav Maroušek
//

#pragma once

#include "zen_base.h"

namespace GothicLib
{
	namespace ZenGin
	{
		class zCPar_Symbol
		{
		public:

			zCPar_Symbol()	{ }
			~zCPar_Symbol()	{ }

			bool Save(FileStream*);
			bool Load(FileStream*);

			std::string name;

			uint32_t offset;

			uint32_t elemProps;
			uint32_t fileIndex;
			uint32_t lineStart;
			uint32_t lineCount;
			uint32_t charStart;
			uint32_t charCount;

		private:

		};

		class zCPar_File
		{
		public:

			zCPar_File()	{ }
			~zCPar_File()	{ }

			bool Save(FileStream*);
			bool Load(FileStream*);

			std::vector<zCPar_Symbol> symbols;

		private:

		};
	};
};
