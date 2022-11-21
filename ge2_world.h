#pragma once

#include "ge2_base.h"

namespace GothicLib
{
	namespace Genome2
	{
		/*
			gCProject
		*/

		class gCProjectItem : public bCObjectRefBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_RISEN2,	1	},
			};

			GE2_DECLARE_CLASS(gCProjectItem, bCObjectRefBase);

			gCProjectItem()				{ }
			virtual ~gCProjectItem()	{ }

		private:

		};

		class gCProject : public gCProjectItem
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_RISEN2,	1	},
			};

			GE2_DECLARE_CLASS(gCProject, gCProjectItem);

			gCProject()				{ }
			virtual ~gCProject()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);
			
			GE2_DECLARE_PROPERTY(gCProject, GAME_ALL, bTRefPtrArray<class gCWorld *>,	Worlds);
			GE2_DECLARE_PROPERTY(gCProject, GAME_ALL, bCString,							ActiveWorld);

			std::vector<std::string> worlds; // temp

		private:

		};
	}
}