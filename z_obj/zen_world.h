#pragma once

#include "zen_base.h"
#include "zen_visual.h"

namespace GothicLib
{
	namespace ZenGin
	{
		class zCBspTree;
		class zCVob;
		class zCCamTrj_KeyFrame;
		class zCVisual;
		class zCAIBase;
		class zCWayNet;
		class zCEventManager;

		/*
			World classes
		*/

		class zCWorld : public zCObject
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		1 },
				{ GAME_CHRISTMASEDITION,	1 },
				{ GAME_GOTHIC1,				1 },
				{ GAME_GOTHICSEQUEL,		1 },
				{ GAME_GOTHIC2,				1 },
				{ GAME_GOTHIC2ADDON,		1 },
			};

			ZEN_DECLARE_CLASS(zCWorld, zCObject);

			zCWorld()			{ }
			virtual ~zCWorld()	{ }
			
			bool LoadWorld(FileStream*);
			bool SaveWorld(FileStream*);

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			zCBspTree*	bsp;
			zCVob*		vobTree;
			zCWayNet*	wayNet;

		private:

			// .ZEN
			bool ArcVobTree(zCArchiver*, zCVob*, size_t&);
			bool UnarcVobTree(zCArchiver*, zCVob*, size_t&);

			// .PWF
			bool LoadWorldFile(FileStream*);
			bool SaveWorldFile(FileStream*);

			bool LoadVobTree(FileStream*, zCVob*);
			bool SaveVobTree(FileStream*, zCVob*);

		};

		class oCWorld : public zCWorld
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

			ZEN_DECLARE_CLASS(oCWorld, zCWorld);

			oCWorld()			{ }
			virtual ~oCWorld()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

		private:

		};

		/*
			BSP
		*/

		class zCBspTree
		{
		public:

			zCBspTree()				{ }
			virtual ~zCBspTree()	{ }

			bool SaveBIN(FileStream*);
			bool LoadBIN(FileStream*);

			GAME game;

			zCMesh mesh;

		private:

		};

		/*
			Vob classes

			zCVob
				zCCSCamera
				zCCamTrj_KeyFrame
				oCDummyVobGenerator
				zCEffect
					zCEarthquake
					zCMusicControler
					zCPFXControler
					zCTouchAnimate
						zCTouchAnimateSound
					zCTouchDamage
						oCTouchDamage
					oCVisualFX
						oCVisFX_Lightning [GOTHIC 1]
						oCVisFX_MultiTarget
					zCVobAnimate
					zCVobLensFlare
					zCVobScreenFX
				oCObjectGenerator
				zCTriggerBase
					zCCodeMaster
					zCMessageFilter
					zCMoverControler
					zCTrigger
						oCCSTrigger
						zCMover
						oCTriggerChangeLevel
						zCTriggerJumper [DEMO5]
						zCTriggerList
						oCTriggerScript
						zCTriggerTeleport
					zCTriggerUntouch
					zCTriggerWorldStart
				oCVob
					oCItem
					oCMOB
						oCMobInter
							oCMobBed
							oCMobFire
							oCMobItemSlot
							oCMobLadder
							oCMobLockable
								oCMobContainer
								oCMobDoor
							oCMobSwitch
							oCMobWheel
					oCMob [DEMO5]
					oCNpc
				zCVobLevelCompo
				zCVobLight
				zCVobMarker [DEMO5]
				zCVobSpot
				zCVobStair
				zCVobStartpoint
				zCVobWaypoint
				zCZone
					zCVobSound
						zCVobSoundDaytime
					zCZoneMusic
						oCZoneMusic
							oCZoneMusicDefault
					zCZoneReverb
						zCZoneReverbDefault
					zCZoneVobFarPlane
						zCZoneVobFarPlaneDefault
					zCZoneZFog
						zCZoneZFogDefault
			
		*/

		/*
			zCVob
				oCVob
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

		enum zTVobType
		{
			zVOB_TYPE_NORMAL,
			zVOB_TYPE_LIGHT,
			zVOB_TYPE_SOUND,
			zVOB_TYPE_LEVEL_COMPONENT,
			zVOB_TYPE_SPOT,
			zVOB_TYPE_CAMERA,
			zVOB_TYPE_STARTPOINT,
			zVOB_TYPE_WAYPOINT,
			zVOB_TYPE_MARKER,
			zVOB_TYPE_SEPARATOR = 127,
			zVOB_TYPE_MOB,
			zVOB_TYPE_ITEM,
			zVOB_TYPE_NSC
		};

#pragma pack (push, 1)
		/*struct zSVobArcRawData
		{
			zTBBox3D	bbox3DWS;
			zVEC3		positionWS;
			zMAT3		trafoRotWS;
			struct zSBitField
			{
				uint8_t showVisual : 1;
				uint8_t visualCamAlign : 2;
				uint8_t cdStatic : 1;
				uint8_t cdDyn : 1;
				uint8_t staticVob : 1;
				uint8_t dynShadow : 2;
				uint8_t hasPresetName : 1;
				uint8_t hasVobName : 1;
				uint8_t hasVisualName : 1;
				uint8_t hasRelevantVisualObject : 1;
				uint8_t hasAIObject : 1;
				uint8_t hasEventManObject : 1;
				uint8_t physicsEnabled : 1;
				uint8_t visualAniMode : 2;
				uint8_t zbias : 5;
				uint8_t isAmbient : 1;
			} bitfield;
			float		visualAniStrength;
			float		vobFarClipZScale;
		};*/

		struct zSVobArcRawData
		{
			zTBBox3D	bbox3DWS;
			zVEC3		positionWS;
			zMAT3		trafoRotWS;
			struct zSBitField
			{
				uint8_t showVisual : 1;
				uint8_t visualCamAlign : 2;
				uint8_t cdStatic : 1;
				uint8_t cdDyn : 1;
				uint8_t staticVob : 1;
				uint8_t dynShadow : 2;
				uint8_t hasPresetName : 1;
				uint8_t hasVobName : 1;
				uint8_t hasVisualName : 1;
				uint8_t hasRelevantVisualObject : 1;
				uint8_t hasAIObject : 1;
				uint8_t hasEventManObject : 1;
				uint8_t physicsEnabled : 1;
			} bitfield;
		};
#pragma pack (pop)

		class zCVob : public zCObject
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		2 },
				{ GAME_CHRISTMASEDITION,	3 },
				{ GAME_GOTHIC1,				4 },
				{ GAME_GOTHICSEQUEL,		4 },
				{ GAME_GOTHIC2,				5 },
				{ GAME_GOTHIC2ADDON,		5 },
			};

			ZEN_DECLARE_CLASS(zCVob, zCObject);

			zCVob()				{ }
			virtual ~zCVob()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			virtual bool Save(FileStream*);
			virtual bool Load(FileStream*);

			std::vector<zCVob*> children;

			/*
				Properties
			*/

			zTVobType			vobType					= zVOB_TYPE_NORMAL;		// Legacy
			int					vobId;											// Legacy
			std::string			presetName;
			bool				drawBBox3D				= false;				// Legacy
			zTBBox3D			bbox3DWS				= {};
			zMAT3				trafoRot				= {};					// Legacy
			zVEC3				trafoPos				= {};					// Legacy
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

			zCVisual*		visualPtr		= nullptr;
			zCAIBase*		aiPtr			= nullptr;
			zCEventManager* eventManagerPtr	= nullptr;

		private:

		};

		class oCVob : public zCVob
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

			ZEN_DECLARE_CLASS_ABSTRACT(oCVob, zCVob);

			oCVob()				{ }
			virtual ~oCVob()	{ }

		private:

		};

		/*
			zCCSCamera
		*/

		enum zTCamTrj_FOR
		{
			zCAMTRJ_KEY_FOR_WORLD,
			zCAMTRJ_KEY_FOR_OBJECT
		};

		enum zTCamTrj_LoopMode
		{
			zCAMTRJ_LOOPMODE_NONE,
			zCAMTRJ_LOOPMODE_RESTART,
			zCAMTRJ_LOOPMODE_PINGPONG
		};

		enum zTSplLerpMode
		{
			zTSPL_LERP_UNDEF,
			zTSPL_LERP_PATH,
			zTSPL_LERP_PATH_IGNOREROLL,
			zTSPL_LERP_PATH_ROT_SAMPLES
		};

		class zCCSCamera : public zCVob
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	8 },
				{ GAME_GOTHIC1,				8 },
				{ GAME_GOTHICSEQUEL,		8 },
				{ GAME_GOTHIC2,				8 },
				{ GAME_GOTHIC2ADDON,		8 },
			};

			ZEN_DECLARE_CLASS(zCCSCamera, zCVob);

			zCCSCamera()			{ }
			virtual ~zCCSCamera()	{ }
			
			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			zTCamTrj_FOR		camTrjFOR						= zCAMTRJ_KEY_FOR_WORLD;
			zTCamTrj_FOR		targetTrjFOR					= zCAMTRJ_KEY_FOR_WORLD;
			zTCamTrj_LoopMode	loopMode						= zCAMTRJ_LOOPMODE_NONE;
			zTSplLerpMode		splLerpMode						= zTSPL_LERP_PATH;
			bool				ignoreFORVobRotCam				= false;
			bool				ignoreFORVobRotTarget			= false;
			bool				adaptToSurroundings				= true;
			bool				easeToFirstKey					= false;
			bool				easeFromLastKey					= false;
			float				totalTime						= 10.0f;
			std::string			autoCamFocusVobName;
			bool				autoCamPlayerMovable			= true;
			bool				autoCamUntriggerOnLastKey		= true;
			float				autoCamUntriggerOnLastKeyDelay	= 0.0f;
			bool				paused							= false;	// Savegame
			bool				started							= false;	// Savegame
			bool				gotoTimeMode					= false;	// Savegame
			float				csTime							= 0.0f;		// Savegame

			std::vector<zCCamTrj_KeyFrame*> positions;
			std::vector<zCCamTrj_KeyFrame*> targets;

		private:

		};


		/*
			zCCamTrj_KeyFrame
		*/

		enum zTCamTrj_KeyMotionType
		{
			zCAMTRJ_KEY_MOTION_UNDEF,
			zCAMTRJ_KEY_MOTION_SMOOTH,
			zCAMTRJ_KEY_MOTION_LINEAR,
			zCAMTRJ_KEY_MOTION_STEP,
			zCAMTRJ_KEY_MOTION_SLOW,
			zCAMTRJ_KEY_MOTION_FAST,
			zCAMTRJ_KEY_MOTION_CUSTOM
		};

		class zCCamTrj_KeyFrame : public zCVob
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	7 },
				{ GAME_GOTHIC1,				7 },
				{ GAME_GOTHICSEQUEL,		7 },
				{ GAME_GOTHIC2,				7 },
				{ GAME_GOTHIC2ADDON,		7 },
			};

			ZEN_DECLARE_CLASS(zCCamTrj_KeyFrame, zCVob);

			zCCamTrj_KeyFrame()				{ }
			virtual ~zCCamTrj_KeyFrame()	{ }
			
			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			float					time				= -1.0f;
			float					angleRollDeg		= 0.0f;
			float					camFOVScale			= 1.0f;
			zTCamTrj_KeyMotionType	motionType			= zCAMTRJ_KEY_MOTION_SMOOTH;
			zTCamTrj_KeyMotionType	motionTypeFOV		= zCAMTRJ_KEY_MOTION_SMOOTH;
			zTCamTrj_KeyMotionType	motionTypeRoll		= zCAMTRJ_KEY_MOTION_SMOOTH;
			zTCamTrj_KeyMotionType	motionTypeTimeScale	= zCAMTRJ_KEY_MOTION_SMOOTH;
			float					tension				= 0.0f;
			float					bias				= 0.0f;
			float					continuity			= 0.0f;
			float					timeScale			= 1.0f;
			bool					timeIsFixed			= false;
			zMAT4					originalPose		= { };							// Savegame

		private:

		};

		/*
			oCDummyVobGenerator
		*/


		/*
			zCEffect
				zCEarthquake
				zCMusicControler
				zCPFXControler
				zCTouchAnimate
					zCTouchAnimateSound
				zCTouchDamage
					oCTouchDamage
				oCVisualFX
					oCVisFX_Lightning [GOTHIC 1]
					oCVisFX_MultiTarget
				zCVobAnimate
				zCVobLensFlare
				zCVobScreenFX
		*/

		class zCEffect : public zCVob
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

			ZEN_DECLARE_CLASS_ABSTRACT(zCEffect, zCVob);

			zCEffect()			{ }
			virtual ~zCEffect()	{ }

		private:

		};

		class zCPFXControler : public zCEffect
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

			ZEN_DECLARE_CLASS(zCPFXControler, zCEffect);

			zCPFXControler()			{ }
			virtual ~zCPFXControler()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			std::string pfxName;
			bool killVobWhenDone	= true;
			bool pfxStartOn			= true;

		private:

		};

		class zCVobAnimate : public zCEffect
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

			ZEN_DECLARE_CLASS(zCVobAnimate, zCEffect);

			zCVobAnimate()			{ }
			virtual ~zCVobAnimate()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			bool startOn	= true;
			bool isRunning	= false; // Savegame

		private:

		};

		class zCVobScreenFX : public zCEffect
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	0 },
				{ GAME_GOTHIC1,				0 },
				{ GAME_GOTHICSEQUEL,		0 },
				{ GAME_GOTHIC2,				1 },
				{ GAME_GOTHIC2ADDON,		1 },
			};

			ZEN_DECLARE_CLASS(zCVobScreenFX, zCEffect);

			zCVobScreenFX()				{ }
			virtual ~zCVobScreenFX()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			//zTScreenFXSet	blend;			// Savegame
			//zTScreenFXSet	cinema;			// Savegame
			//zTScreenFXSet	fovMorph;		// Savegame
			//zVEC2			fovSaved;		// Savegame
			//zVEC2			fovSavedFirst;	// Savegame

		private:

		};

		/*
			oCItem
		*/

		class oCItem : public oCVob
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

			ZEN_DECLARE_CLASS(oCItem, oCVob);

			oCItem()				{ }
			virtual ~oCItem()		{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			virtual bool Save(FileStream*);
			virtual bool Load(FileStream*);

			/*
				Properties
			*/
			
			std::string itemInstance;
			int amount					= 1; // Savegame
			int flags					= 0; // Savegame

		private:

		};

		/*
			oCMOB
				oCMobInter
					oCMobBed
					oCMobFire
					oCMobItemSlot
					oCMobLadder
					oCMobLockable
						oCMobContainer
						oCMobDoor
					oCMobSwitch
					oCMobWheel
		*/

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

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				1 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	2 },
				{ GAME_GOTHIC1,				2 },
				{ GAME_GOTHICSEQUEL,		2 },
				{ GAME_GOTHIC2,				2 },
				{ GAME_GOTHIC2ADDON,		2 },
			};

			ZEN_DECLARE_CLASS(oCMOB, oCVob);

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
			bool			isDestroyed		= false;	// Props

		private:

		};

		class oCMobInter : public oCMOB
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	3 },
				{ GAME_GOTHIC1,				3 },
				{ GAME_GOTHICSEQUEL,		3 },
				{ GAME_GOTHIC2,				3 },
				{ GAME_GOTHIC2ADDON,		3 },
			};

			ZEN_DECLARE_CLASS(oCMobInter, oCMOB);

			oCMobInter()			{ }
			virtual ~oCMobInter()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			int			state			= 0;	// Legacy
			int			stateTarget		= 0;	// Legacy
			int			stateNum		= 0;
			std::string	triggerTarget;
			std::string	useWithItem;
			std::string	conditionFunc;
			std::string	onStateFunc;
			bool		rewind			= false;

		private:

		};

		class oCMobFire : public oCMobInter
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	2 },
				{ GAME_GOTHIC1,				2 },
				{ GAME_GOTHICSEQUEL,		2 },
				{ GAME_GOTHIC2,				2 },
				{ GAME_GOTHIC2ADDON,		2 },
			};

			ZEN_DECLARE_CLASS(oCMobFire, oCMobInter);

			oCMobFire()				{ }
			virtual ~oCMobFire()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			std::string fireSlot;
			std::string fireVobtreeName;

		private:

		};

		class oCMobLadder : public oCMobInter
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

			ZEN_DECLARE_CLASS(oCMobLadder, oCMobInter);

			oCMobLadder()			{ }
			virtual ~oCMobLadder()	{ }

		private:

		};

		class oCMobLockable : public oCMobInter
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

			ZEN_DECLARE_CLASS_ABSTRACT(oCMobLockable, oCMobInter);

			oCMobLockable()				{ }
			virtual ~oCMobLockable()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			bool		locked		= false;
			std::string	keyInstance;
			std::string	pickLockStr;


		private:

		};

		class oCMobContainer : public oCMobLockable
		{
		public:
			
			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	1 },
				{ GAME_GOTHIC1,				1 },
				{ GAME_GOTHICSEQUEL,		1 },
				{ GAME_GOTHIC2,				1 },
				{ GAME_GOTHIC2ADDON,		1 },
			};

			ZEN_DECLARE_CLASS(oCMobContainer, oCMobLockable);

			oCMobContainer()			{ }
			virtual ~oCMobContainer()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			std::string	contains;

		private:

		};

		class oCMobDoor : public oCMobLockable
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	1 },
				{ GAME_GOTHIC1,				1 },
				{ GAME_GOTHICSEQUEL,		1 },
				{ GAME_GOTHIC2,				1 },
				{ GAME_GOTHIC2ADDON,		1 },
			};

			ZEN_DECLARE_CLASS(oCMobDoor, oCMobLockable);

			oCMobDoor()				{ }
			virtual ~oCMobDoor()	{ }

		private:

		};

		class oCMobSwitch : public oCMobInter
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	0 },
				{ GAME_GOTHIC1,				0 },
				{ GAME_GOTHICSEQUEL,		0 },
				{ GAME_GOTHIC2,				1 },
				{ GAME_GOTHIC2ADDON,		1 },
			};

			ZEN_DECLARE_CLASS(oCMobSwitch, oCMobLockable);

			oCMobSwitch()			{ }
			virtual ~oCMobSwitch()	{ }

		private:

		};

		/*
			oCMob

			Legacy class, not to be confused with oCMOB and its derivatives.
			Only included to support demo5 loading.
		*/

		class oCMob : public oCVob
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
			};

			ZEN_DECLARE_CLASS(oCMob, oCVob);

			oCMob()				{ }
			virtual ~oCMob()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			virtual bool Save(FileStream*);
			virtual bool Load(FileStream*);

			/*
				Properties
			*/

			std::string		mobInstance;				// Legacy

		private:

		};

		/*
			oCNpc
		*/
		
		class oCNpc : public oCVob
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				1 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	5 },
				{ GAME_GOTHIC1,				5 },
				{ GAME_GOTHICSEQUEL,		5 },
				{ GAME_GOTHIC2,				7 },
				{ GAME_GOTHIC2ADDON,		7 },
			};

			ZEN_DECLARE_CLASS(oCNpc, oCVob);

			oCNpc()				{ }
			virtual ~oCNpc()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			virtual bool Save(FileStream*);
			virtual bool Load(FileStream*);

			/*
				Properties
			*/

			std::string		npcInstance;
			zVEC3			modelScale;		// Savegame
			float			modelFatness;	// Savegame
			int				flags;			// Savegame
			int				guild;			// Savegame
			int				guildTrue;		// Savegame
			int				level;			// Savegame
			int				xp;				// Savegame
			int				xpnl;			// Savegame
			int				lp;				// Savegame

		private:

		};

		/*
			oCObjectGenerator
		*/

		/*
			zCTriggerBase
				zCCodeMaster
				zCMessageFilter
				zCMoverControler
				zCTrigger
					oCCSTrigger
					zCMover
					oCTriggerChangeLevel
					zCTriggerJumper [DEMO5]
					zCTriggerList
					oCTriggerScript
					zCTriggerTeleport
				zCTriggerUntouch
				zCTriggerWorldStart
		*/
		
		class zCTriggerBase : public zCVob
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

			ZEN_DECLARE_CLASS_ABSTRACT(zCTriggerBase, zCVob);

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

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	2 },
				{ GAME_GOTHIC1,				2 },
				{ GAME_GOTHICSEQUEL,		2 },
				{ GAME_GOTHIC2,				2 },
				{ GAME_GOTHIC2ADDON,		2 },
			};

			ZEN_DECLARE_CLASS(zCTrigger, zCTriggerBase);

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

		enum zTMoverBehavior
		{
			MB_2STATE_TOGGLE,
			MB_2STATE_TRIGGER_CONTROL,
			MB_2STATE_OPEN_TIMED,
			MB_NSTATE_LOOP,
			MB_NSTATE_SINGLE_KEYS
		};

		enum zTPosLerpType
		{
			PL_LINEAR,
			PL_CURVE
		};

		enum zTSpeedType
		{
			ST_CONST,
			ST_SLOW_START_END,
			ST_SLOW_START,
			ST_SLOW_END,
			ST_SEG_SLOW_START_END,
			ST_SEG_SLOW_START,
			ST_SEG_SLOW_END
		};

		enum zTMoverState
		{
			MOVER_STATE_OPEN,
			MOVER_STATE_OPENING,
			MOVER_STATE_CLOSED,
			MOVER_STATE_CLOSING
		};

		class zCMover : public zCTrigger
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	1 },
				{ GAME_GOTHIC1,				1 },
				{ GAME_GOTHICSEQUEL,		1 },
				{ GAME_GOTHIC2,				1 },
				{ GAME_GOTHIC2ADDON,		1 },
			};

			ZEN_DECLARE_CLASS(zCMover, zCTrigger);

			zCMover()			{ }
			virtual ~zCMover()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			struct zTMov_Keyframe
			{
				zVEC3	pos;
				zCQuat	quat;
			};

			zTMoverBehavior				moverBehavior		= MB_2STATE_TOGGLE;
			float						touchBlockerDamage	= 0.0f;
			float						stayOpenTimeSec		= 2.0f;
			bool						moverLocked			= false;
			bool						autoLinkEnabled		= false;
			float						moveSpeed			= 0.3f;
			zTPosLerpType				posLerpType			= PL_CURVE;
			zTSpeedType					speedType			= ST_SLOW_START_END;
			std::vector<zTMov_Keyframe>	keyframeList;
			zVEC3						actKeyPosDelta		= {};					// Savegame
			float						actKeyframeF		= 0.0f;					// Savegame
			int							actKeyframe			= 0;					// Savegame
			int							nextKeyframe		= 0;					// Savegame
			float						moveSpeedUnit		= 0.0f;					// Savegame
			float						advanceDir			= 0.0f;					// Savegame
			zTMoverState				moverState			= MOVER_STATE_CLOSED;	// Savegame
			int							numTriggerEvents	= 0;					// Savegame
			float						stayOpenTimeDest	= 0.0f;					// Savegame
			std::string					sfxOpenStart;
			std::string					sfxOpenEnd;
			std::string					sfxMoving;
			std::string					sfxCloseStart;
			std::string					sfxCloseEnd;
			std::string					sfxLock;
			std::string					sfxUnlock;
			std::string					sfxUseLocked;

		private:

		};

		class oCTriggerChangeLevel : public zCTrigger
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

			ZEN_DECLARE_CLASS(oCTriggerChangeLevel, zCTrigger);

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

		enum zTListProcess
		{
			LP_ALL,
			LP_NEXT_ONE,
			LP_RAND_ONE
		};

		class zCTriggerList : public zCTrigger
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

			ZEN_DECLARE_CLASS(zCTriggerList, zCTrigger);

			zCTriggerList()			{ }
			virtual ~zCTriggerList()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			zTListProcess				listProcess		= LP_ALL;
			std::vector<std::string>	triggerTargetList;
			std::vector<float>			fireDelayList;
			char						actTarget		= 0;	// Savegame
			bool						sendOnTrigger	= true;	// Savegame

		private:

		};

		class oCTriggerScript : public zCTrigger
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

			ZEN_DECLARE_CLASS(oCTriggerScript, zCTrigger);

			oCTriggerScript()			{ }
			virtual ~oCTriggerScript()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			std::string scriptFunc;

		private:

		};

		/*
			zCVobLevelCompo
		*/

		class zCVobLevelCompo : public zCVob
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

			ZEN_DECLARE_CLASS(zCVobLevelCompo, zCVob);

			zCVobLevelCompo() { }
			virtual ~zCVobLevelCompo() { }

		private:

		};

		/*
			zCVobLight
		*/

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

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	4 },
				{ GAME_GOTHIC1,				4 },
				{ GAME_GOTHICSEQUEL,		4 },
				{ GAME_GOTHIC2,				5 },
				{ GAME_GOTHIC2ADDON,		5 },
			};

			ZEN_DECLARE_CLASS(zCVobLight, zCVob);

			zCVobLight()			{ }
			virtual ~zCVobLight()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			virtual bool Save(FileStream*);
			virtual bool Load(FileStream*);

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
			zCVobSpot
		*/

		class zCVobSpot : public zCVob
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

			ZEN_DECLARE_CLASS(zCVobSpot, zCVob);

			zCVobSpot()				{ }
			virtual ~zCVobSpot()	{ }

		private:

		};

		/*
			zCVobStair
		*/
		
		class zCVobStair : public zCVob
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

			ZEN_DECLARE_CLASS(zCVobStair, zCVob);

			zCVobStair()			{ }
			virtual ~zCVobStair()	{ }

		private:

		};

		/*
			zCVobStartpoint
		*/

		class zCVobStartpoint : public zCVob
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

			ZEN_DECLARE_CLASS(zCVobStartpoint, zCVob);

			zCVobStartpoint()			{ }
			virtual ~zCVobStartpoint()	{ }

		private:

		};

		/*
			zCVobWaypoint
		*/

		/*
			zCZone
				zCVobSound
					zCVobSoundDaytime
				zCZoneMusic
					oCZoneMusic
						oCZoneMusicDefault
				zCZoneReverb
					zCZoneReverbDefault
				zCZoneVobFarPlane
					zCZoneVobFarPlaneDefault
				zCZoneZFog
					zCZoneZFogDefault
		*/

		class zCZone : public zCVob
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

			ZEN_DECLARE_CLASS_ABSTRACT(zCZone, zCVob);

			zCZone()			{ }
			virtual ~zCZone()	{ }

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

		class zCVobSound : public zCZone
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	4 },
				{ GAME_GOTHIC1,				4 },
				{ GAME_GOTHICSEQUEL,		4 },
				{ GAME_GOTHIC2,				4 },
				{ GAME_GOTHIC2ADDON,		4 },
			};

			ZEN_DECLARE_CLASS(zCVobSound, zCZone);
		
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

		class zCVobSoundDaytime : public zCVobSound
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

			ZEN_DECLARE_CLASS(zCVobSoundDaytime, zCVobSound);
		
			zCVobSoundDaytime()				{ }
			virtual ~zCVobSoundDaytime()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			float		sndStartTime	= 0.0f;
			float		sndEndTime		= 0.0f;
			std::string	sndName2;

		private:

		};

		class zCZoneMusic : public zCZone
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

			ZEN_DECLARE_CLASS(zCZoneMusic, zCZone);

			zCZoneMusic()			{ }
			virtual ~zCZoneMusic()	{ }

		private:

		};

		class oCZoneMusic : public zCZoneMusic
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

			ZEN_DECLARE_CLASS(oCZoneMusic, zCZoneMusic);

			oCZoneMusic()			{ }
			virtual ~oCZoneMusic()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			bool	enabled				= true;
			int		priority			= 1;
			bool	ellipsoid			= false;
			float	reverbLevel			= -3.219f;
			float	volumeLevel			= 1.0f;
			bool	loop				= true;
			bool	local_enabled		= true;		// Savegame
			bool	dayEntranceDone		= false;	// Savegame
			bool	nightEntranceDone	= false;	// Savegame

		private:

		};

		class oCZoneMusicDefault : public oCZoneMusic
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

			ZEN_DECLARE_CLASS(oCZoneMusicDefault, oCZoneMusic);

			oCZoneMusicDefault()			{ }
			virtual ~oCZoneMusicDefault()	{ }

		private:

		};

		class zCZoneVobFarPlane : public zCZone
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

			ZEN_DECLARE_CLASS(zCZoneVobFarPlane, zCZone);

			zCZoneVobFarPlane()				{ }
			virtual ~zCZoneVobFarPlane()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			float vobFarPlaneZ		= 3000.0f;
			float innerRangePerc	= 0.7f;

		private:

		};

		class zCZoneVobFarPlaneDefault : public zCZoneVobFarPlane
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

			ZEN_DECLARE_CLASS(zCZoneVobFarPlaneDefault, zCZoneVobFarPlane);

			zCZoneVobFarPlaneDefault()			{ }
			virtual ~zCZoneVobFarPlaneDefault()	{ }

		private:

		};

		class zCZoneZFog : public zCZone
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	1 },
				{ GAME_GOTHIC1,				1 },
				{ GAME_GOTHICSEQUEL,		1 },
				{ GAME_GOTHIC2,				2 },
				{ GAME_GOTHIC2ADDON,		2 },
			};

			ZEN_DECLARE_CLASS(zCZoneZFog, zCZone);

			zCZoneZFog()			{ }
			virtual ~zCZoneZFog()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			float	fogRangeCenter	= 3000.0f;
			float	innerRangePerc	= 0.7f;
			zCOLOR	fogColor		= { };

		private:

		};

		class zCZoneZFogDefault : public zCZoneZFog
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

			ZEN_DECLARE_CLASS(zCZoneZFogDefault, zCZoneZFog);

			zCZoneZFogDefault()				{ }
			virtual ~zCZoneZFogDefault()	{ }

		private:

		};

		/*
			AI classes
		*/

		class zCAIBase : public zCObject
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

			ZEN_DECLARE_CLASS(zCAIBase, zCObject);

			zCAIBase()			{ }
			virtual ~zCAIBase()	{ }

		private:

		};

		class zCAICamera : public zCAIBase
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

			ZEN_DECLARE_CLASS(zCAICamera, zCAIBase);

			zCAICamera()			{ }
			virtual ~zCAICamera()	{ }

		private:

		};

		class zCAIPlayer : public zCAIBase
		{
		public:
			
			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	0 },
				{ GAME_GOTHIC1,				1 },
				{ GAME_GOTHICSEQUEL,		1 },
				{ GAME_GOTHIC2,				1 },
				{ GAME_GOTHIC2ADDON,		1 },
			};

			ZEN_DECLARE_CLASS(zCAIPlayer, zCAIBase);

			zCAIPlayer()			{ }
			virtual ~zCAIPlayer()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			int		waterLevel	= 0;
			float	floorY		= 0.0f;
			float	waterY		= 0.0f;
			float	ceilY		= 0.0f;
			float	feetY		= 0.0f;
			float	headY		= 0.0f;
			float	fallDistY	= 0.0f;
			float	fallStartY	= 0.0f;

		private:

		};

		class oCAniCtrl_Human : public zCAIPlayer
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

			ZEN_DECLARE_CLASS(oCAniCtrl_Human, zCAIPlayer);

			oCAniCtrl_Human()			{ }
			virtual ~oCAniCtrl_Human()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			oCNpc* aiNpc = nullptr;

			int		walkMode		= 0;
			int		weaponMode		= 0;
			int		wmodeLast		= 0;
			int		wmodeSelect		= 0;
			bool	changeWeapon	= false;
			int		actionMode		= 0;

		private:

		};

		class oCAIHuman : public oCAniCtrl_Human
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

			ZEN_DECLARE_CLASS(oCAIHuman, oCAniCtrl_Human);

			oCAIHuman()				{ }
			virtual ~oCAIHuman()	{ }

		private:

		};

		/*
			Event manager
		*/

		class zCEventManager : public zCObject
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

			ZEN_DECLARE_CLASS(zCEventManager, zCObject);

			zCEventManager()				{ }
			virtual ~zCEventManager()		{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/
			
			bool cleared;
			bool active;

			//bool active;

		private:

		};

		/*
			Waynet
		*/

		class zCWaypoint : public zCObject
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

			ZEN_DECLARE_CLASS(zCWaypoint, zCObject);

			zCWaypoint()				{ }
			virtual ~zCWaypoint()		{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/
			
			std::string	wpName;
			int			waterDepth		= 0;
			bool		underWater		= false;
			zVEC3		position		= {};
			zVEC3		direction		= {};

		private:

		};
		
		class zCWay
		{
		public:

			zCWay()				{ }
			virtual ~zCWay()	{ }

			/*
				Properties
			*/

			zCWaypoint* left;
			zCWaypoint* right;

		private:

		};

		class zCWayNet : public zCObject
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

			ZEN_DECLARE_CLASS(zCWayNet, zCObject);

			zCWayNet()				{ }
			virtual ~zCWayNet()		{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			/*
				Properties
			*/

			int waynetVersion			= 0;
			std::vector<zCWaypoint*>	waypointList;
			std::vector<zCWay>			wayList;

		private:

		};
	};
};