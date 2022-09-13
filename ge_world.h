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

		class eCContextBase;

		class gCLayerBase : public eCProcessibleElement
		{
		public:

			GE_DECLARE_CLASS(gCLayerBase, eCProcessibleElement);

			gCLayerBase()			{ }
			virtual ~gCLayerBase()	{ }
			
			GE_DECLARE_PROPERTY(gCLayerBase, GAME_GOTHIC3, bTPOSmartPtr<class gCSector>, SectorPtr);

			eCContextBase* context;

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

		/*
			eCContextBase
				eCGeometrySpatialContext
				eCEntityDynamicContext
		*/

		class eCContextBase : public bCObjectRefBase
		{
		public:

			GE_DECLARE_CLASS(eCContextBase, bCObjectRefBase);

			eCContextBase()				{ }
			virtual ~eCContextBase()	{ }
			
			GE_DECLARE_PROPERTY(eCContextBase, GAME_ALL, bCGuid,	ID);
			GE_DECLARE_PROPERTY(eCContextBase, GAME_ALL, bCBox,		ContextBox);

			bool	enabled;
			float	visualLoDFactor;
			float	objectCullFactor;

		private:

		};

		class eCGeometrySpatialContext : public eCContextBase
		{
		public:

			GE_DECLARE_CLASS(eCGeometrySpatialContext, eCContextBase);

			eCGeometrySpatialContext()			{ }
			virtual ~eCGeometrySpatialContext()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			virtual bool WriteNodes(FileStream*);
			virtual bool ReadNodes(FileStream*);

			bool hybridContext;

		private:

		};

		class eCEntityDynamicContext : public eCContextBase
		{
		public:

			GE_DECLARE_CLASS(eCEntityDynamicContext, eCContextBase);

			eCEntityDynamicContext()			{ }
			virtual ~eCEntityDynamicContext()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

		private:

		};

		/*
			eCEntityProxy
			eCNode
				eCEntity
					eCGeometryEntity
						eCSpatialEntity
						eCDynamicEntity
		*/

		class eCEntityProxy
		{
		public:

			eCEntityProxy()		{ }
			~eCEntityProxy()	{ }

			virtual bool Write(FileStream*);
			virtual bool Read(FileStream*);

			bCGuid id;

		private:

		};

		class eCNode : public bCObjectRefBase
		{
		public:

			GE_DECLARE_CLASS(eCNode, bCObjectRefBase);

			eCNode()			{ }
			virtual ~eCNode()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			bCGuid		id;
						
		private:

		};

		class eCEntity : public eCNode
		{
		public:

			GE_DECLARE_CLASS(eCEntity, eCNode);

			eCEntity()			{ }
			virtual ~eCEntity()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			std::string	name;
			bCMatrix	worldMatrix;
			bCBox		worldNodeBoundary;
			bCSphere	worldNodeSphere;

			bool		enabled;
			bool		renderingEnabled;
			bool		processingDisabled;			// Gothic 3
			bool		deactivationEnabled;		// Gothic 3
			uint8_t		flags;
			bool		pickingEnabled;
			bool		collisionEnabled;
			float		renderAlphaValue;			// Gothic 3
			uint16_t	insertType;
			uint8_t		lastRenderPriority;			// Gothic 3
			bool		locked;
			uint8_t		specialDepthTexPassEnabled;	// Gothic 3
			bool		ignoreProcessingRange;
			float		visualLodFactor;			// Gothic 3
			float		objectCullFactor;			// Gothic 3
			bCDateTime	dataChangedTimestamp;
			float		uniformScaling;				// Gothic 3
			bool		rangedObjectCulling;
			bool		processingRangeOutFadingEnabled;
			bool		forceOutdoor;
			bool		saveGameRelevant;
			
		private:

		};

		class eCGeometryEntity : public eCEntity
		{
		public:

			GE_DECLARE_CLASS(eCGeometryEntity, eCEntity);

			eCGeometryEntity()			{ }
			virtual ~eCGeometryEntity()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			float		renderAlphaValue;
			float		viewRange;
			float		cacheInRange;

		private:

		};

		class eCSpatialEntity : public eCGeometryEntity
		{
		public:

			GE_DECLARE_CLASS(eCSpatialEntity, eCGeometryEntity);

			eCSpatialEntity()			{ }
			virtual ~eCSpatialEntity()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);
			
		private:

		};

		class eCDynamicEntity : public eCGeometryEntity
		{
		public:

			GE_DECLARE_CLASS(eCDynamicEntity, eCGeometryEntity);

			eCDynamicEntity()			{ }
			virtual ~eCDynamicEntity()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			bCMatrix		localMatrix;
			bCBox			worldNodeBoundary;
			bCSphere		worldNodeSphere;
			bCBox			localNodeBoundary;
			eCEntityProxy	creator;
			
		private:

		};

		/*
			
		*/
	};
};
