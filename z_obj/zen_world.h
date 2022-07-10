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

		enum zTVobSleepingMode
		{
			zVOB_SLEEPING = 0,
			zVOB_AWAKE = 1,
			zVOB_AWAKE_DOAI_ONLY = 2
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
			bool				cdDyn					= false;
			bool				staticVob				= false;
			zTDynShadowType		dynShadow				= zDYN_SHADOW_TYPE_NONE;
			int					zbias					= 0;					// Gothic 2
			bool				isAmbient				= false;				// Gothic 2
			bool				physicsEnabled			= false;				// Savegame
			zTVobSleepingMode	sleepMode				= zVOB_SLEEPING;		// Savegame
			float				nextOnTimer				= 0.0f;					// Savegame

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

		private:

		};

		class zCVobSpot : public zCVob
		{
		public:

			DEFINE_CLASS(zCVobSpot, zCVob);

			zCVobSpot()				{ }
			virtual ~zCVobSpot()	{ }

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

			float			sndVolume			= 100;
			zTSoundMode		sndMode				= SM_LOOPING;
			float			sndRandDelay		= 5;
			float			sndRandDelayVar		= 2;
			bool			sndStartOn			= 1;
			bool			sndAmbient3D		= 0;
			bool			sndObstruction		= 0;
			float			sndConeAngle		= 0;
			zTSoundVolType	sndVolType			= SV_SPHERE;
			float			sndRadius			= 1500;
			std::string		sndName;
			bool			soundIsRunning		= 0; // Savegame
			bool			soundAllowedToRun	= 0; // Savegame

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

		class oCVob : public zCVob
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

			oCVob()				{ }
			virtual ~oCVob()	{ }

		private:

		};

		enum oTSndMaterial
		{
			SND_MAT_WOOD,
			SND_MAT_STONE,
			SND_MAT_METAL,
			SND_MAT_LEATHER,
			SND_MAT_CLAY,
			SND_MAT_GLAS
		};

		class oCMOB : public oCVob
		{
		public:

			DEFINE_CLASS(oCMOB, oCVob);

			oCMOB()				{ }
			virtual ~oCMOB()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			std::string		focusName;
			int				hitpoints		= 0;
			int				damage			= 0;
			bool			moveable		= false;
			bool			takeable		= false;
			bool			focusOverride	= false;
			oTSndMaterial	soundMaterial	= SND_MAT_WOOD;
			std::string		visualDestroyed;
			std::string		owner;
			std::string		ownerGuild;
			bool			isDestroyed		= false; // Props

		private:

		};

		class oCMobInter : public oCMOB
		{
		public:

			DEFINE_CLASS(oCMobInter, oCMOB);

			oCMobInter()			{ }
			virtual ~oCMobInter()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			int			stateNum		= 0;
			std::string	triggerTarget;
			std::string	useWithItem;
			std::string	conditionFunc;
			std::string	onStateFunc;
			bool		rewind			= false;

		private:

		};

		class oCMobLadder : public oCMobInter
		{
		public:

			DEFINE_CLASS(oCMobLadder, oCMobInter);

			oCMobLadder()			{ }
			virtual ~oCMobLadder()	{ }

		private:

		};

		class zCTriggerBase : public zCVob
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

			zCTriggerBase()				{ }
			virtual ~zCTriggerBase()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			std::string triggerTarget;

		private:

		};

		class zCTrigger : public zCTriggerBase
		{
		public:

			DEFINE_CLASS(zCTrigger, zCTriggerBase);

			zCTrigger()				{ }
			virtual ~zCTrigger()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			struct
			{
				uint8_t	reactToOnTrigger : 1;
				uint8_t	reactToOnTouch : 1;
				uint8_t	reactToOnDamage : 1;
				uint8_t	respondToObject : 1;
				uint8_t	respondToPC : 1;
				uint8_t	respondToNPC : 1;
			} filterFlags;

			struct
			{
				uint8_t	startEnabled : 1;
				uint8_t	isEnabled : 1;
				uint8_t	sendUntrigger : 1;
			} flags;

			std::string	respondToVobName;
			int			numCanBeActivated;
			float		retriggerWaitSec	= 0.0f;
			float		damageThreshold		= 0.0f;
			float		fireDelaySec		= 0.0f;
			bool		sendUntrigger		= false;	// Props
			float		nextTimeTriggerable = 0.0f;		// Savegame
			zCVob*		savedOtherVobPtr	= nullptr;	// Savegame
			int			countCanBeActivated = 0;		// Savegame	

		private:

		};

		class oCTriggerChangeLevel : public zCTrigger
		{
		public:

			DEFINE_CLASS(oCTriggerChangeLevel, zCTrigger);

			oCTriggerChangeLevel()			{ }
			virtual ~oCTriggerChangeLevel()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			std::string levelName;
			std::string	startVobName;

		private:

		};

		class oCItem : public oCVob
		{
		public:

			DEFINE_CLASS(oCItem, oCVob);

			oCItem()				{ }
			virtual ~oCItem()		{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/
			
			std::string itemInstance;
			int amount					= 1; // Savegame
			int flags					= 0; // Savegame

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

		private:

		};

		class zCMesh : public zCVisual
		{
		public:

			DEFINE_CLASS(zCMesh, zCVisual);
		
			zCMesh()			{ }
			virtual ~zCMesh()	{ }

		private:

		};

		class zCParticleFX : public zCVisual
		{
		public:

			DEFINE_CLASS(zCParticleFX, zCVisual);

			zCParticleFX()			{ }
			virtual ~zCParticleFX()	{ }

		private:

		};

		class zCVisualAnimate : public zCVisual
		{
		public:

			DEFINE_CLASS(zCVisualAnimate, zCVisual);

			zCVisualAnimate()			{ }
			virtual ~zCVisualAnimate()	{ }

		private:

		};

		class zCModel : public zCVisualAnimate
		{
		public:

			DEFINE_CLASS(zCModel, zCVisualAnimate);

			zCModel()			{ }
			virtual ~zCModel()	{ }

		private:

		};

		class zCProgMeshProto : public zCVisual
		{
		public:

			DEFINE_CLASS(zCProgMeshProto, zCVisual);

			zCProgMeshProto()			{ }
			virtual ~zCProgMeshProto()	{ }

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

			DEFINE_CLASS(zCDecal, zCVisual);

			zCDecal()				{ }
			virtual ~zCDecal()		{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

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

		/*
			AI classes
		*/

		class zCAIBase : public zCObject
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

			zCAIBase()			{ }
			virtual ~zCAIBase()	{ }

		private:

		};
	};
};