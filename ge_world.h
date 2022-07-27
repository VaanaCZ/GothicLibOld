#pragma once

#include "ge_base.h"

namespace GothicLib
{
	namespace Genome
	{
		/*
			gCProject
		*/

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

		/*
			gCWorld
		*/

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

			std::string sectorFile;

		private:

			bool WriteData(FileStream*);
			bool ReadData(FileStream*);

		};
		
		/*
			gCSector
		*/

		class gCSector : public eCProcessibleElement
		{
		public:

			GE_DECLARE_CLASS(gCSector, eCProcessibleElement);

			gCSector()			{ }
			virtual ~gCSector()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			virtual bool DoLoadData(FileStream*);
			virtual bool DoSaveData(FileStream*);
			
			GE_DECLARE_PROPERTY(gCSector, GAME_GOTHIC3,	bTPOSmartPtr<class gCWorld>,			WorldPtr);
			GE_DECLARE_PROPERTY(gCSector, GAME_GOTHIC3,	bTRefPtrArray<class gCGeometryLayer *>,	GeometryLayers);
			GE_DECLARE_PROPERTY(gCSector, GAME_ALL,		bTRefPtrArray<class gCDynamicLayer *>,	EntityLayers);
			GE_DECLARE_PROPERTY(gCSector, GAME_GOTHIC3, bool,									Marked);
			GE_DECLARE_PROPERTY(gCSector, GAME_GOTHIC3, bool,									Freezed);
			GE_DECLARE_PROPERTY(gCSector, GAME_GOTHIC3, bool,									Locked);
			GE_DECLARE_PROPERTY(gCSector, GAME_GOTHIC3, float,									VisualLoDFactor);
			GE_DECLARE_PROPERTY(gCSector, GAME_GOTHIC3, float,									ObjectCullFactor);
			GE_DECLARE_PROPERTY(gCSector, GAME_RISEN1,	bool,									EnabledOnStartup);
			GE_DECLARE_PROPERTY(gCSector, GAME_RISEN1,	bool,									SaveGameRelevant);
			GE_DECLARE_PROPERTY(gCSector, GAME_RISEN1,	bool,									DemoRelevant);

			bool enabled;
			std::vector<std::string> dynamicLayers;
			std::vector<std::string> geometryLayers;

		private:

			bool WriteData(FileStream*);
			bool ReadData(FileStream*);

		};
	};
};
