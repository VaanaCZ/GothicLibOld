//
// zen_texture.h
//
// Copyright (c) 2021-2023 Václav Maroušek
//

#pragma once

#include "zen_base.h"

namespace GothicLib
{
	namespace ZenGin
	{
		enum TEXFORMAT
		{
			TEXFORMAT_B8G8R8A8,
			TEXFORMAT_R8G8B8A8,
			TEXFORMAT_A8B8G8R8,
			TEXFORMAT_A8R8G8B8,
			TEXFORMAT_B8G8R8,
			TEXFORMAT_R8G8B8,
			TEXFORMAT_A4R4G4B4,
			TEXFORMAT_A1R5G5B5,
			TEXFORMAT_R5G6B5,
			TEXFORMAT_P8,
			TEXFORMAT_DXT1,
			TEXFORMAT_DXT2,
			TEXFORMAT_DXT3,
			TEXFORMAT_DXT4,
			TEXFORMAT_DXT5
		};

		struct TEXPALETTE
		{
			unsigned char r, g, b;
		};

		// bytes per pixel
		inline uint32_t texFormatBpps[] = 
		{
			4,
			4,
			4,
			4,
			3,
			3,
			2,
			2,
			2,
			1,
			0,
			0,
			0,
			0,
			0
		};

		class zCTexture : public zCResource
		{
		public:
			
			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		0	},
				{ GAME_CHRISTMASEDITION,	0	},
				{ GAME_GOTHIC1,				0	},
				{ GAME_GOTHICSEQUEL,		0	},
				{ GAME_GOTHIC2,				0	},
				{ GAME_GOTHIC2ADDON,		0	},
			};

			ZEN_DECLARE_CLASS(zCTexture, zCResource);

			zCTexture()				{ }
			virtual ~zCTexture()	{ }

			bool SaveTexture(FileStream*);
			bool LoadTexture(FileStream*);

			bool SavePortableBinary(FileStream*);
			bool LoadPortableBinary(FileStream*);

			bool Convert_P8ToR5G6B5();

			void Destroy();

			TEXFORMAT	format;
			uint32_t	width;
			uint32_t	height;
			uint32_t	mipmapCount;
			uint32_t	refWidth;
			uint32_t	refHeight;
			zCOLOR		averageColor;

			TEXPALETTE*	palette = nullptr;
			char**		data = nullptr;

		private:

		};
	};
};
