#pragma once

#include "ge_base.h"

namespace GothicLib
{
	namespace Genome
	{
		class gCProject : public eCProcessibleElement
		{
		public:

			GE_DECLARE_CLASS(gCProject, eCProcessibleElement);

			gCProject()				{ }
			virtual ~gCProject()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			virtual bool DoLoadData(FileStream*);
			virtual bool DoSaveData(FileStream*);

			GE_DECLARE_PROPERTY(gCProject, GAME_GOTHIC3,	bTPOSmartPtr<class gCWorkspace>,	WorkspacePtr);
			GE_DECLARE_PROPERTY(gCProject, GAME_ALL,		bTRefPtrArray<class gCWorld *>,		Worlds);
			GE_DECLARE_PROPERTY(gCProject, GAME_ALL,		bCString,							ActiveWorld);

			std::vector<std::string> worlds; // temp

		private:

			bool WriteData(FileStream*);
			bool ReadData(FileStream*);

		};

		class gCWorld : public eCProcessibleElement
		{
		public:

			GE_DECLARE_CLASS(gCWorld, eCProcessibleElement);

			gCWorld()			{ }
			virtual ~gCWorld()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			virtual bool DoLoadData(FileStream*);
			virtual bool DoSaveData(FileStream*);

		private:

		};
	};
};
