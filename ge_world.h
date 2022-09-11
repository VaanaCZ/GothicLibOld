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

		/*
			gCLayerBase
				gCGeometryLayer
				gCDynamicLayer
		*/

		class gCLayerBase : public eCProcessibleElement
		{
		public:

			GE_DECLARE_CLASS(gCLayerBase, eCProcessibleElement);

			gCLayerBase()			{ }
			virtual ~gCLayerBase()	{ }
			
			GE_DECLARE_PROPERTY(gCLayerBase, GAME_GOTHIC3, bTPOSmartPtr<class gCSector>, SectorPtr);

		private:

		};

		class gCGeometryLayer : public gCLayerBase
		{
		public:

			GE_DECLARE_CLASS(gCGeometryLayer, gCLayerBase);

			gCGeometryLayer()			{ }
			virtual ~gCGeometryLayer()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			virtual bool DoLoadData(FileStream*);
			virtual bool DoSaveData(FileStream*);
			
			GE_DECLARE_PROPERTY(gCGeometryLayer, GAME_GOTHIC3, bCString,									OriginImportName);
			GE_DECLARE_PROPERTY(gCGeometryLayer, GAME_GOTHIC3, bTPropertyContainer<enum gEGeometryType>,	GeometryType);

		private:

		};

		class gCDynamicLayer : public gCLayerBase
		{
		public:

			GE_DECLARE_CLASS(gCDynamicLayer, gCLayerBase);

			gCDynamicLayer()			{ }
			virtual ~gCDynamicLayer()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			virtual bool DoLoadData(FileStream*);
			virtual bool DoSaveData(FileStream*);
			
			GE_DECLARE_PROPERTY(gCDynamicLayer, GAME_ALL,		bTPropertyContainer<enum gEEntityType>,	EntityType);
			GE_DECLARE_PROPERTY(gCDynamicLayer, GAME_RISEN1,	bCString,								OriginImportName);

		private:

			bool WriteData(FileStream*);
			bool ReadData(FileStream*);

		};
	};
};
