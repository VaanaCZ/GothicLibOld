#pragma once

#include "zen_base.h"

namespace GothicLib
{
	namespace ZenGin
	{
		/*
			Visual classes
		*/

		/*
			zCMaterial
		*/

		enum zTMat_Group
		{
			zMAT_GROUP_UNDEF,
			zMAT_GROUP_METAL,
			zMAT_GROUP_STONE,
			zMAT_GROUP_WOOD,
			zMAT_GROUP_EARTH,
			zMAT_GROUP_WATER,
			zMAT_GROUP_SNOW,
			zMAT_NUM_MAT_GROUP
		};

		enum zTWaveAniMode
		{
			zWAVEANI_NONE,
			zWAVEANI_GROUND_AMBIENT,
			zWAVEANI_GROUND,
			zWAVEANI_WALL_AMBIENT,
			zWAVEANI_WALL,
			zWAVEANI_ENV,
			zWAVEANI_WIND_AMBIENT,
			zWAVEANI_WIND
		};

		enum zTFFT
		{
			zTFFT_NONE,
			zTFFT_SLOW,
			zTFFT_NORMAL,
			zTFFT_FAST
		};

		enum zTRnd_AlphaBlendFunc
		{
			zRND_ALPHA_FUNC_MAT_DEFAULT,
			zRND_ALPHA_FUNC_NONE,
			zRND_ALPHA_FUNC_BLEND,
			zRND_ALPHA_FUNC_ADD,
			zRND_ALPHA_FUNC_SUB,
			zRND_ALPHA_FUNC_MUL,
			zRND_ALPHA_FUNC_MUL2,
			zRND_ALPHA_FUNC_TEST,
			zRND_ALPHA_FUNC_BLEND_TEST
		};

		class zCMaterial : public zCObject
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0				},
				{ GAME_SEPTEMBERDEMO,		17408			},
				{ GAME_CHRISTMASEDITION,	17408			},
				{ GAME_GOTHIC1,				17408			},
				{ GAME_GOTHICSEQUEL,		17408			},
				{ GAME_GOTHIC2,				39939			},
				{ GAME_GOTHIC2ADDON,		39939			},
			};

			ZEN_DECLARE_CLASS(zCMaterial, zCObject);
		
			zCMaterial()			{ }
			virtual ~zCMaterial()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			virtual bool Save(FileStream*);
			virtual bool Load(FileStream*);

			/*
				Properties
			*/

			std::string				name;
			zTMat_Group				matGroup						= zMAT_GROUP_UNDEF;
			zCOLOR					color;
			float					smoothAngle						= 60.0f;
			std::string				texture;
			zVEC2					texScale						= {};
			float					texAniFPS						= 0.0f;
			zBOOL					texAniMapMode					= false;
			zVEC2					texAniMapDir					= {};
			zBOOL					noCollDet						= false;
			zBOOL					noLightmap						= false;
			zBOOL					lodDontCollapse					= false;
			std::string				detailObject;
			float					detailObjectScale				= 1.0f;
			zBOOL					forceOccluder					= false;
			zBOOL					environmentalMapping			= false;
			float					environmentalMappingStrength	= 1.0f;
			zTWaveAniMode			waveMode						= zWAVEANI_NONE;
			zTFFT					waveSpeed						= zTFFT_NORMAL;
			float					waveMaxAmplitude				= 30.0f;
			float					waveGridSize					= 100.0f;
			zBOOL					ignoreSunLight					= false;
			zTRnd_AlphaBlendFunc	alphaFunc						= zRND_ALPHA_FUNC_NONE;
			unsigned char			libFlag							= 0;					// Legacy
			zVEC2					defaultMapping					= {};

		private:

		};

		/*
			zCVisual
				zCDecal
				zCFlash
				zCMesh
				zCParticleFX
					zCUnderwaterPFX
				zCPolyStrip
					zCFlash::zCBolt
					zCLightning::zCBolt
				zCProgMeshProto
					zCMeshSoftSkin
				zCQuadMark
				zCVisualAnimate
					zCModel
					zCMorphMesh
		*/

		class zCVisual : public zCObject
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	0 },
				{ GAME_GOTHIC1,				0 },
				{ GAME_GOTHICSEQUEL,		0 },
				{ GAME_GOTHIC2,				0 },
				{ GAME_GOTHIC2ADDON,		0 },
			};

			ZEN_DECLARE_CLASS_ABSTRACT(zCVisual, zCObject);
		
			zCVisual()			{ }
			virtual ~zCVisual()	{ }

		private:

		};
		
		class zCDecal : public zCVisual
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		0		},
				{ GAME_CHRISTMASEDITION,	0		},
				{ GAME_GOTHIC1,				0		},
				{ GAME_GOTHICSEQUEL,		0		},
				{ GAME_GOTHIC2,				53505	},
				{ GAME_GOTHIC2ADDON,		53505	},
			};

			ZEN_DECLARE_CLASS(zCDecal, zCVisual);

			zCDecal()				{ }
			virtual ~zCDecal()		{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			virtual bool Save(FileStream*);
			virtual bool Load(FileStream*);

			/*
				Properties
			*/

			std::string				name;
			zVEC2					decalDim			= { 25, 25 };
			zVEC2					decalOffset			= {};
			zBOOL					decal2Sided			= false;
			zTRnd_AlphaBlendFunc	decalAlphaFunc		= zRND_ALPHA_FUNC_MAT_DEFAULT;
			float					decalTexAniFPS		= 0.0f;
			unsigned char			decalAlphaWeight	= 255;							// Gothic 2
			zBOOL					ignoreDayLight		= false;						// Gothic 2

		private:

		};

		enum CHUNK_TYPE
		{
			CHUNK_MESHSTART			= 0xB000,
			CHUNK_BBOX				= 0xB010,
			CHUNK_MATERIALS			= 0xB020,
			CHUNK_LIGHTMAPS_OLD		= 0xB025,
			CHUNK_LIGHTMAPS_NEW		= 0xB026,
			CHUNK_VERTS				= 0xB030,
			CHUNK_FEATS				= 0xB040,
			CHUNK_POLYS				= 0xB050,
			CHUNK_MESHEND			= 0xB060
		};

		class zCOBBox3D
		{
		public:

			zCOBBox3D()		{}
			~zCOBBox3D()	{}

			bool LoadBIN(FileStream*);
			bool SaveBIN(FileStream*);

			zVEC3 center;
			zVEC3 axis[3];
			zVEC3 extent;

			std::vector<zCOBBox3D> childs;

			bool readChilds = true;

		};

		class zCMesh : public zCVisual
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		0		},
				{ GAME_CHRISTMASEDITION,	0		},
				{ GAME_GOTHIC1,				0		},
				{ GAME_GOTHICSEQUEL,		0		},
				{ GAME_GOTHIC2,				53505	},
				{ GAME_GOTHIC2ADDON,		53505	},
			};

			ZEN_DECLARE_CLASS(zCMesh, zCVisual);
		
			zCMesh()			{ }
			virtual ~zCMesh()	{ }

			bool SaveMSH(FileStream*);
			bool LoadMSH(FileStream*);

			std::vector<zCMaterial> materials;

		private:

		};

		class zCParticleFX : public zCVisual
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	0 },
				{ GAME_GOTHIC1,				0 },
				{ GAME_GOTHICSEQUEL,		0 },
				{ GAME_GOTHIC2,				0 },
				{ GAME_GOTHIC2ADDON,		0 },
			};

			ZEN_DECLARE_CLASS(zCParticleFX, zCVisual);

			zCParticleFX()			{ }
			virtual ~zCParticleFX()	{ }

		private:

		};

		class zCProgMeshProto : public zCVisual
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		0		},
				{ GAME_CHRISTMASEDITION,	0		},
				{ GAME_GOTHIC1,				0		},
				{ GAME_GOTHICSEQUEL,		0		},
				{ GAME_GOTHIC2,				53505	},
				{ GAME_GOTHIC2ADDON,		53505	},
			};

			ZEN_DECLARE_CLASS(zCProgMeshProto, zCVisual);

			zCProgMeshProto()			{ }
			virtual ~zCProgMeshProto()	{ }

		private:

		};

		class zCVisualAnimate : public zCVisual
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	0 },
				{ GAME_GOTHIC1,				0 },
				{ GAME_GOTHICSEQUEL,		0 },
				{ GAME_GOTHIC2,				0 },
				{ GAME_GOTHIC2ADDON,		0 },
			};

			ZEN_DECLARE_CLASS_ABSTRACT(zCVisualAnimate, zCVisual);

			zCVisualAnimate()			{ }
			virtual ~zCVisualAnimate()	{ }

		private:

		};

		class zCModel : public zCVisualAnimate
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	0 },
				{ GAME_GOTHIC1,				0 },
				{ GAME_GOTHICSEQUEL,		0 },
				{ GAME_GOTHIC2,				0 },
				{ GAME_GOTHIC2ADDON,		0 },
			};

			ZEN_DECLARE_CLASS(zCModel, zCVisualAnimate);

			zCModel()			{ }
			virtual ~zCModel()	{ }

		private:

		};
		
		class zCMorphMesh : public zCVisualAnimate
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	0 },
				{ GAME_GOTHIC1,				0 },
				{ GAME_GOTHICSEQUEL,		0 },
				{ GAME_GOTHIC2,				0 },
				{ GAME_GOTHIC2ADDON,		0 },
			};

			ZEN_DECLARE_CLASS(zCMorphMesh, zCVisualAnimate);

			zCMorphMesh()			{ }
			virtual ~zCMorphMesh()	{ }

		private:

		};
	};
};