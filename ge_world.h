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

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	200	},
			};

			GE_DECLARE_CLASS(gCProject, eCProcessibleElement);

			gCProject()				{ }
			virtual ~gCProject()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			virtual bool DoLoadData(FileStream*, GAME);
			virtual bool DoSaveData(FileStream*, GAME);

			GE_DECLARE_PROPERTY(gCProject, GAME_GOTHIC3,	bTPOSmartPtr<class gCWorkspace>,	WorkspacePtr);
			GE_DECLARE_PROPERTY(gCProject, GAME_ALL,		bTRefPtrArray<class gCWorld *>,		Worlds);
			GE_DECLARE_PROPERTY(gCProject, GAME_ALL,		bCString,							ActiveWorld);

			std::vector<std::string> worlds; // temp

		private:

			bool WriteData(FileStream*, GAME);
			bool ReadData(FileStream*, GAME);

		};

		/*
			gCWorld
		*/

		class gCWorld : public eCProcessibleElement
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	36	},
				{ GAME_RISEN1,	200	},
			};

			GE_DECLARE_CLASS(gCWorld, eCProcessibleElement);

			gCWorld()			{ }
			virtual ~gCWorld()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			virtual bool DoLoadData(FileStream*, GAME);
			virtual bool DoSaveData(FileStream*, GAME);

			std::string sectorFile;

		private:

			bool WriteData(FileStream*, GAME);
			bool ReadData(FileStream*, GAME);

		};
		
		/*
			gCSector
		*/

		class gCSector : public eCProcessibleElement
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	27	},
				{ GAME_RISEN1,	200	},
			};

			GE_DECLARE_CLASS(gCSector, eCProcessibleElement);

			gCSector()			{ }
			virtual ~gCSector()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			virtual bool DoLoadData(FileStream*, GAME);
			virtual bool DoSaveData(FileStream*, GAME);
			
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

			bool WriteData(FileStream*, GAME);
			bool ReadData(FileStream*, GAME);

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

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	1	},
			};

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

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
			};

			GE_DECLARE_CLASS(gCGeometryLayer, gCLayerBase);

			gCGeometryLayer()			{ }
			virtual ~gCGeometryLayer()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			virtual bool DoLoadData(FileStream*, GAME);
			virtual bool DoSaveData(FileStream*, GAME);
			
			GE_DECLARE_PROPERTY(gCGeometryLayer, GAME_GOTHIC3, bCString,									OriginImportName);
			GE_DECLARE_PROPERTY(gCGeometryLayer, GAME_GOTHIC3, bTPropertyContainer<enum gEGeometryType>,	GeometryType);

		private:

		};

		class gCDynamicLayer : public gCLayerBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	83	},
				{ GAME_RISEN1,	200	},
			};

			GE_DECLARE_CLASS(gCDynamicLayer, gCLayerBase);

			gCDynamicLayer()			{ }
			virtual ~gCDynamicLayer()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			virtual bool DoLoadData(FileStream*, GAME);
			virtual bool DoSaveData(FileStream*, GAME);
			
			GE_DECLARE_PROPERTY(gCDynamicLayer, GAME_ALL,		bTPropertyContainer<enum gEEntityType>,	EntityType);
			GE_DECLARE_PROPERTY(gCDynamicLayer, GAME_RISEN1,	bCString,								OriginImportName);

		private:

			bool WriteData(FileStream*, GAME);
			bool ReadData(FileStream*, GAME);

		};

		/*
			eCContextBase
				eCGeometrySpatialContext
				eCEntityDynamicContext
		*/

		class eCEntity;

		class eCContextBase : public bCObjectRefBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	83	},
				{ GAME_RISEN1,	83	},
			};

			GE_DECLARE_CLASS(eCContextBase, bCObjectRefBase);

			eCContextBase()				{ }
			virtual ~eCContextBase()	{ }
			
			GE_DECLARE_PROPERTY(eCContextBase, GAME_ALL, bCGuid,	ID);
			GE_DECLARE_PROPERTY(eCContextBase, GAME_ALL, bCBox,		ContextBox);

			bool	enabled;
			float	visualLoDFactor;
			float	objectCullFactor;

			eCEntity* rootEntity;

		private:

		};

		class eCGeometrySpatialContext : public eCContextBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	83	},
			};

			GE_DECLARE_CLASS(eCGeometrySpatialContext, eCContextBase);

			eCGeometrySpatialContext()			{ }
			virtual ~eCGeometrySpatialContext()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			virtual bool WriteNodes(FileStream*, GAME);
			virtual bool ReadNodes(FileStream*, GAME);

			bool hybridContext;

		private:

		};

		class eCEntityDynamicContext : public eCContextBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	212	},
			};

			GE_DECLARE_CLASS(eCEntityDynamicContext, eCContextBase);

			eCEntityDynamicContext()			{ }
			virtual ~eCEntityDynamicContext()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

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

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	210	},
			};

			GE_DECLARE_CLASS(eCNode, bCObjectRefBase);

			eCNode()			{ }
			virtual ~eCNode()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			bCGuid		id;
						
		private:

		};

		class eCEntityPropertySet;

		class eCEntity : public eCNode
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	83	},
				{ GAME_RISEN1,	217	},
			};

			GE_DECLARE_CLASS(eCEntity, eCNode);

			eCEntity()			{ }
			virtual ~eCEntity()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

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
			
			std::vector<eCEntityPropertySet*> propertySets;

		private:

		};

		class eCGeometryEntity : public eCEntity
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_RISEN1,	214	},
			};

			GE_DECLARE_CLASS(eCGeometryEntity, eCEntity);

			eCGeometryEntity()			{ }
			virtual ~eCGeometryEntity()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			float		renderAlphaValue;
			float		viewRange;
			float		cacheInRange;

		private:

		};

		class eCSpatialEntity : public eCGeometryEntity
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	35	},
			};

			GE_DECLARE_CLASS(eCSpatialEntity, eCGeometryEntity);

			eCSpatialEntity()			{ }
			virtual ~eCSpatialEntity()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			bCBox			visualWorldNodeBoundary;
			bCOrientedBox	visualWorldNodeOOBoundary;
			
		private:

		};

		class eCDynamicEntity : public eCGeometryEntity
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	83	},
				{ GAME_RISEN1,	214	},
			};

			GE_DECLARE_CLASS(eCDynamicEntity, eCGeometryEntity);

			eCDynamicEntity()			{ }
			virtual ~eCDynamicEntity()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			bCMatrix		localMatrix;
			bCBox			worldNodeBoundary;
			bCSphere		worldNodeSphere;
			bCBox			localNodeBoundary;
			eCEntityProxy	creator;
			
		private:

		};

		/*
			eCEntityPropertySet
				eCCollisionShapeBase_PS
					eCCollisionShape_PS
				eCDynamicLight_PS
					eCDirectionalLight_PS
					eCHemisphere_PS
				eCVisualMeshBase_PS [GOTHIC3]
					eCVisualMeshStatic_PS [GOTHIC3]
		*/

		class eCEntityPropertySet : public bCObjectRefBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	2	},
				{ GAME_RISEN1,	2	},
			};

			GE_DECLARE_CLASS(eCEntityPropertySet, bCObjectRefBase);

			eCEntityPropertySet()			{ }
			virtual ~eCEntityPropertySet()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			bool renderingEnabled;

		private:

		};

		class eCCollisionShapeBase_PS : public eCEntityPropertySet
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	1	},
			};

			GE_DECLARE_CLASS(eCCollisionShapeBase_PS, eCEntityPropertySet);

			eCCollisionShapeBase_PS()			{ }
			virtual ~eCCollisionShapeBase_PS()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);
						
		private:

		};

		class eCCollisionShape_PS : public eCCollisionShapeBase_PS
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	63	},
				{ GAME_RISEN1,	63	},
			};

			GE_DECLARE_CLASS(eCCollisionShape_PS, eCCollisionShapeBase_PS);

			eCCollisionShape_PS()			{ }
			virtual ~eCCollisionShape_PS()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			GE_DECLARE_PROPERTY(eCCollisionShape_PS, GAME_GOTHIC3, bTPropertyContainer<enum eECollisionGroup>,	Group);
			GE_DECLARE_PROPERTY(eCCollisionShape_PS, GAME_GOTHIC3, bTPropertyContainer<enum eEPhysicRangeType>,	Range);
			GE_DECLARE_PROPERTY(eCCollisionShape_PS, GAME_GOTHIC3, bool,										DisableCollision);
			GE_DECLARE_PROPERTY(eCCollisionShape_PS, GAME_GOTHIC3, bool,										DisableResponse);
			GE_DECLARE_PROPERTY(eCCollisionShape_PS, GAME_GOTHIC3, bool,										IgnoredByTraceRay);
			
		private:

		};

		class eCDynamicLight_PS : public eCEntityPropertySet
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	1	},
			};

			GE_DECLARE_CLASS(eCDynamicLight_PS, eCEntityPropertySet);

			eCDynamicLight_PS()			{ }
			virtual ~eCDynamicLight_PS()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			GE_DECLARE_PROPERTY(eCDynamicLight_PS, GAME_RISEN1, bool, Enabled);
			
		private:

		};

		class eCDirectionalLight_PS : public eCDynamicLight_PS
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	1	},
			};

			GE_DECLARE_CLASS(eCDirectionalLight_PS, eCDynamicLight_PS);

			eCDirectionalLight_PS()				{ }
			virtual ~eCDirectionalLight_PS()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			GE_DECLARE_PROPERTY(eCDirectionalLight_PS, GAME_RISEN1, bCFloatColor,	Color);
			GE_DECLARE_PROPERTY(eCDirectionalLight_PS, GAME_RISEN1, bCFloatColor,	SpecularColor);
			GE_DECLARE_PROPERTY(eCDirectionalLight_PS, GAME_RISEN1, float,			Intensity);
			GE_DECLARE_PROPERTY(eCDirectionalLight_PS, GAME_RISEN1, bCEulerAngles,	DirectionOffset);
			
		private:

		};

		class eCHemisphere_PS : public eCDynamicLight_PS
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
				{ GAME_RISEN1,	1	},
			};

			GE_DECLARE_CLASS(eCHemisphere_PS, eCDynamicLight_PS);

			eCHemisphere_PS()			{ }
			virtual ~eCHemisphere_PS()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			GE_DECLARE_PROPERTY(eCHemisphere_PS, GAME_RISEN1, bCFloatColor,		GeneralAmbient);
			GE_DECLARE_PROPERTY(eCHemisphere_PS, GAME_RISEN1, bCFloatColor,		BackLight);
			GE_DECLARE_PROPERTY(eCHemisphere_PS, GAME_RISEN1, bCFloatColor,		GroundLight);
			GE_DECLARE_PROPERTY(eCHemisphere_PS, GAME_RISEN1, bCEulerAngles,	BackLightAxisDirectionOffset);;
			GE_DECLARE_PROPERTY(eCHemisphere_PS, GAME_RISEN1, float,			Intensity);
			GE_DECLARE_PROPERTY(eCHemisphere_PS, GAME_RISEN1, bCFloatColor,		SunLight);
			GE_DECLARE_PROPERTY(eCHemisphere_PS, GAME_RISEN1, bCEulerAngles,	SunLightAxisDirectionOffset);
			
		private:

		};

		class eCVisualMeshBase_PS : public eCEntityPropertySet
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	1	},
			};

			GE_DECLARE_CLASS(eCVisualMeshBase_PS, eCEntityPropertySet);

			eCVisualMeshBase_PS()			{ }
			virtual ~eCVisualMeshBase_PS()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			GE_DECLARE_PROPERTY(eCVisualMeshBase_PS, GAME_GOTHIC3, bCMeshResourceString,									ResourceFileName);
			GE_DECLARE_PROPERTY(eCVisualMeshBase_PS, GAME_GOTHIC3, bTPropertyContainer<enum eEStaticLighingType>,			StaticLightingType);
			GE_DECLARE_PROPERTY(eCVisualMeshBase_PS, GAME_GOTHIC3, bTPropertyContainer<enum eELightmapAmbientOcclusion>,	LightmapAmbientOcclusion);
			GE_DECLARE_PROPERTY(eCVisualMeshBase_PS, GAME_GOTHIC3, bTPropertyContainer<enum eELightmapType>,				LightmapType);
			GE_DECLARE_PROPERTY(eCVisualMeshBase_PS, GAME_GOTHIC3, float,													UnitsPerLightmapTexel);
			GE_DECLARE_PROPERTY(eCVisualMeshBase_PS, GAME_GOTHIC3, int,														MaterialSwitch);
			
		private:

		};

		class eCVisualMeshStatic_PS : public eCVisualMeshBase_PS
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_GOTHIC3,	50	},
			};

			GE_DECLARE_CLASS(eCVisualMeshStatic_PS, eCVisualMeshBase_PS);

			eCVisualMeshStatic_PS()				{ }
			virtual ~eCVisualMeshStatic_PS()	{ }

			virtual bool OnWrite(FileStream*, GAME);
			virtual bool OnRead(FileStream*, GAME);

			GE_DECLARE_PROPERTY(eCVisualMeshStatic_PS, GAME_GOTHIC3, bCMeshResourceString, ResourceFilePath);
			
		private:

		};
	};
};
