#pragma once

#include "zen_base.h"
#include "zen_texture.h"

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

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			virtual bool Save(FileStream*, GAME);
			virtual bool Load(FileStream*, GAME);

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

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			virtual bool Save(FileStream*, GAME);
			virtual bool Load(FileStream*, GAME);

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

		enum MESHCHUNK_TYPE
		{
			MESHCHUNK_MESHSTART			= 0xB000,
			MESHCHUNK_BBOX				= 0xB010,
			MESHCHUNK_MATERIALS			= 0xB020,
			MESHCHUNK_LIGHTMAPS_OLD		= 0xB025,
			MESHCHUNK_LIGHTMAPS_NEW		= 0xB026,
			MESHCHUNK_VERTS				= 0xB030,
			MESHCHUNK_FEATS				= 0xB040,
			MESHCHUNK_POLYS				= 0xB050,
			MESHCHUNK_MESHEND			= 0xB060
		};

		class zCOBBox3D
		{
		public:

			zCOBBox3D()		{}
			~zCOBBox3D()	{}

			bool LoadBIN(FileStream*, GAME);
			bool SaveBIN(FileStream*, GAME);

			zVEC3 center;
			zVEC3 axis[3];
			zVEC3 extent;

			std::vector<zCOBBox3D> childs;
		};

		struct LightmapChunk
		{
			zVEC3		lmVectors[3];
			uint32_t	texIndex;
		};

		struct Feature
		{
			zVEC2	uv;
			zCOLOR	light;
			zVEC3	normal;
		};

#pragma pack( push, 1 )
		struct PolygonFlags
		{
			unsigned char portalPoly : 2;
			unsigned char occluder : 1;
			unsigned char sectorPoly : 1;
			unsigned char lodFlag : 1;
			unsigned char portalIndoorOutdoor : 1;
			unsigned char ghostOccluder : 1;
		};

		struct PolygonFlagsOld
		{
			unsigned char portalPoly : 2;
			unsigned char occluder : 1;
			unsigned char sectorPoly : 1;
			unsigned char lodFlag : 1;
			unsigned char portalIndoorOutdoor : 1;
			unsigned char ghostOccluder : 1;
			unsigned char normalMainAxis : 2;
			//unsigned char unknownFlag : 1; // Gothic: Sequel
			//unsigned char padding : 5;
			unsigned char padding : 6;
		};
#pragma pack( pop )

		struct Poly
		{
			short			materialIndex;
			short			lightmapIndex;
			zTPlane			plane;
			PolygonFlags	flags;
			uint16_t		sectorIndex;

			std::vector<int> verts;
			std::vector<unsigned int> feats;
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

			bool SaveMSH(FileStream*, GAME);
			bool LoadMSH(FileStream*, GAME);

			zDATE		date;
			std::string	name;
			zTBBox3D	bbox;
			zCOBBox3D	obbox;

			zBOOL	alphaTestingEnabled;
			bool	oldLightmaps;

			std::vector<zCMaterial>		materials;
			std::vector<zCTexture>		textures;
			std::vector<LightmapChunk>	lightmaps;
			std::vector<zVEC3>			verts;
			std::vector<Feature>		feats;
			std::vector<Poly>			polys;

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
