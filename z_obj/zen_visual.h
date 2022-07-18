#pragma once

#include "zen_base.h"

namespace GothicLib
{
	namespace ZenGin
	{
		/*
			Visual classes
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
				{ GAME_GOTHIC2NOTR,			0 },
			};

			ZEN_DECLARE_CLASS_ABSTRACT(zCVisual, zCObject);
		
			zCVisual()			{ }
			virtual ~zCVisual()	{ }

		private:

		};

		class zCVisualAnimate : public zCVisual
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
				{ GAME_GOTHIC2NOTR,			0 },
			};

			ZEN_DECLARE_CLASS_ABSTRACT(zCVisualAnimate, zCVisual);

			zCVisualAnimate()			{ }
			virtual ~zCVisualAnimate()	{ }

		private:

		};

		class zCMesh : public zCVisual
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
				{ GAME_GOTHIC2NOTR,			0 },
			};

			ZEN_DECLARE_CLASS(zCMesh, zCVisual);
		
			zCMesh()			{ }
			virtual ~zCMesh()	{ }

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
				{ GAME_GOTHIC2NOTR,			0 },
			};

			ZEN_DECLARE_CLASS(zCParticleFX, zCVisual);

			zCParticleFX()			{ }
			virtual ~zCParticleFX()	{ }

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
				{ GAME_GOTHIC2NOTR,			0 },
			};

			ZEN_DECLARE_CLASS(zCModel, zCVisualAnimate);

			zCModel()			{ }
			virtual ~zCModel()	{ }

		private:

		};

		class zCProgMeshProto : public zCVisual
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
				{ GAME_GOTHIC2NOTR,			0 },
			};

			ZEN_DECLARE_CLASS(zCProgMeshProto, zCVisual);

			zCProgMeshProto()			{ }
			virtual ~zCProgMeshProto()	{ }

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
				{ GAME_GOTHIC2NOTR,			0 },
			};

			ZEN_DECLARE_CLASS(zCMorphMesh, zCVisualAnimate);

			zCMorphMesh()			{ }
			virtual ~zCMorphMesh()	{ }

		private:

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

		class zCDecal : public zCVisual
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
				{ GAME_GOTHIC2NOTR,			0 },
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
			zVEC2					decalDim		= { 25, 25 };
			zVEC2					decalOffset		= {};
			bool					decal2Sided		= false;
			zTRnd_AlphaBlendFunc	decalAlphaFunc	= zRND_ALPHA_FUNC_MAT_DEFAULT;
			float					decalTexAniFPS	= 0.0f;

		private:

		};
	};
};