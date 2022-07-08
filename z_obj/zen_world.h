#pragma once

#include "zen_base.h"

namespace GothicLib
{
	namespace ZenGin
	{
		class zCVob;
		class zCVisual;
		class zCAIBase;

		/*
			World classes
		*/
		
		class zCWorld : public zCObject
		{
		public:

			DEFINE_CLASS(zCWorld, zCObject);

			zCWorld()			{ }
			virtual ~zCWorld()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			virtual bool UnarcVobTree(zCVob*, zCArchiver*, size_t&);

			zCVob* vobTree;

		private:

		};

		class oCWorld : public zCWorld
		{
		public:

			DEFINE_CLASS(oCWorld, zCWorld);

			oCWorld() { }
			virtual ~oCWorld() { }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

		private:

		};

		/*
			Vob classes
		*/

		enum zTVisualCamAlign
		{
			zVISUAL_CAMALIGN_NONE,
			zVISUAL_CAMALIGN_YAW,
			zVISUAL_CAMALIGN_FULL
		};

		enum zTAnimationMode
		{
			zVISUAL_ANIMODE_NONE,
			zVISUAL_ANIMODE_WIND,
			zVISUAL_ANIMODE_WIND2
		};

		enum zTDynShadowType
		{
			zDYN_SHADOW_TYPE_NONE,
			zDYN_SHADOW_TYPE_BLOB
		};

		class zCVob : public zCObject
		{
		public:

			DEFINE_CLASS(zCVob, zCObject);

			zCVob()				{ }
			virtual ~zCVob()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			std::vector<zCVob*> children;

			/*
				Properties
			*/

			int					pack					= 0;
			std::string			presetName;
			zTBBox3D			bbox3DWS				= {};
			zMAT3				trafoOSToWSRot			= {};
			zVEC3				trafoOSToWSPos			= {};
			std::string			vobName;
			std::string			visual;
			bool				showVisual				= true;
			zTVisualCamAlign	visualCamAlign			= zVISUAL_CAMALIGN_NONE;
			zTAnimationMode		visualAniMode			= zVISUAL_ANIMODE_NONE;	// Gothic 2
			float				visualAniModeStrength	= 0;					// Gothic 2
			float				vobFarClipZScale		= 0;					// Gothic 2
			bool				cdStatic				= false;
			bool				cdDyn					= true;
			bool				staticVob				= true;
			zTDynShadowType		dynShadow				= zDYN_SHADOW_TYPE_NONE;
			int					zbias					= 0;					// Gothic 2
			bool				isAmbient				= false;				// Gothic 2

			zCVisual* visualPtr	= nullptr;
			zCAIBase* aiPtr		= nullptr;

		private:

		};

		class zCVobLevelCompo : public zCVob
		{
		public:

			DEFINE_CLASS(zCVobLevelCompo, zCVob);

			zCVobLevelCompo()			{ }
			virtual ~zCVobLevelCompo()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

		private:

		};

		class zCVobSpot : public zCVob
		{
		public:

			DEFINE_CLASS(zCVobSpot, zCVob);

			zCVobSpot()				{ }
			virtual ~zCVobSpot()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

		private:

		};

		enum zTSoundMode
		{
			SM_LOOPING,
			SM_ONCE,
			SM_RANDOM
		};

		enum zTSoundVolType
		{
			SV_SPHERE,
			SV_ELLIPSOID
		};

		class zCVobSound : public zCVob
		{
		public:

			DEFINE_CLASS(zCVobSound, zCVob);
		
			zCVobSound()			{ }
			virtual ~zCVobSound()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			float			sndVolume		= 100;
			zTSoundMode		sndMode			= SM_LOOPING;
			float			sndRandDelay	= 5;
			float			sndRandDelayVar	= 2;
			bool			sndStartOn		= 1;
			bool			sndAmbient3D	= 0;
			bool			sndObstruction	= 0;
			float			sndConeAngle	= 0;
			zTSoundVolType	sndVolType		= SV_SPHERE;
			float			sndRadius		= 1500;
			std::string		sndName;

		private:

		};

		enum zTVobLightType
		{
			zVOBLIGHT_TYPE_POINT,
			zVOBLIGHT_TYPE_SPOT,
			zVOBLIGHT_TYPE_DIR,
			zVOBLIGHT_TYPE_AMBIENT
		};

		enum zTVobLightQuality
		{
			zVOBLIGHT_QUAL_HI,
			zVOBLIGHT_QUAL_MID,
			zVOBLIGHT_QUAL_FASTEST
		};

		class zCVobLight : public zCVob
		{
		public:

			DEFINE_CLASS(zCVobLight, zCVob);

			zCVobLight()			{ }
			virtual ~zCVobLight()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			std::string			lightPresetInUse;
			zTVobLightType		lightType			= zVOBLIGHT_TYPE_POINT;
			float				range				= 2000;
			zCOLOR				color				= { };
			float				spotConeAngle		= 0;
			bool				lightStatic			= 0;
			zTVobLightQuality	lightQuality		= zVOBLIGHT_QUAL_FASTEST;
			std::string			lensflareFX;
			bool				turnedOn			= true;
			std::string			rangeAniScale;
			float				rangeAniFPS			= 0;
			bool				rangeAniSmooth		= 0;
			std::string			colorAniList;
			float				colorAniFPS			= 0;
			bool				colorAniSmooth		= 0;

		private:

		};

		/*
			Visual classes
		*/

		class zCVisual : public zCObject
		{
		public:

			static inline ClassDefinition* classDef = nullptr;
		
			zCVisual()			{ }
			virtual ~zCVisual()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

		private:

		};

		class zCMesh : public zCVisual
		{
		public:

			DEFINE_CLASS(zCMesh, zCVisual);
		
			zCMesh()			{ }
			virtual ~zCMesh()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

		private:

		};

		class zCParticleFX : public zCVisual
		{
		public:

			DEFINE_CLASS(zCParticleFX, zCVisual);

			zCParticleFX()			{ }
			virtual ~zCParticleFX()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

		private:

		};

		/*
			AI classes
		*/

		class zCAIBase : public zCObject
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

			zCAIBase()			{ }
			virtual ~zCAIBase()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

		private:

		};
	};
};