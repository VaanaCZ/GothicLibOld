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
				{ GAME_DEMO3,				0		},
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		64513	},
				{ GAME_CHRISTMASEDITION,	64513	},
				{ GAME_GOTHIC1,				64513	},
				{ GAME_GOTHICSEQUEL,		64513	},
				{ GAME_GOTHIC2,				64513	},
				{ GAME_GOTHIC2ADDON,		64513	},
			};

			ZEN_DECLARE_CLASS(zCWorld, zCObject);

			zCWorld()			{ }
			virtual ~zCWorld()	{ }

			bool SaveWorld(FileStream*, GAME);
			bool LoadWorld(FileStream*, GAME);

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			zCBspTree*	bsp;
			zCVob*		vobTree;
			zCWayNet*	wayNet;

			std::unordered_map<std::string, zCVob*> vobTable;

		private:

			// .ZEN
			bool ArcVobTree(zCArchiver*, zCVob*, size_t&, GAME);
			bool UnarcVobTree(zCArchiver*, zCVob*, size_t&, GAME);

			// .PWF
			bool SaveWorldFile(FileStream*, GAME);
			bool LoadWorldFile(FileStream*, GAME);

			bool SaveVobTree(FileStream*, zCVob*, size_t&, GAME);
			bool LoadVobTree(FileStream*, zCVob*, GAME);

		};

		class oCWorld : public zCWorld
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		64513	},
				{ GAME_CHRISTMASEDITION,	64513	},
				{ GAME_GOTHIC1,				64513	},
				{ GAME_GOTHICSEQUEL,		64513	},
				{ GAME_GOTHIC2,				64513	},
				{ GAME_GOTHIC2ADDON,		64513	},
				{ GAME_GOTHIC2ADDON,		36865	}, // Spacer 2.6
			};

			ZEN_DECLARE_CLASS(oCWorld, zCWorld);

			oCWorld()			{ }
			virtual ~oCWorld()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

		private:

		};

		/*
			BSP
		*/
		enum BSPCHUNK_TYPE
		{
			BSPCHUNK_START				= 0xC000,
			BSPCHUNK_POLYS				= 0xC010,
			BSPCHUNK_TREE				= 0xC040,
			BSPCHUNK_LIGHTS				= 0xC045,
			BSPCHUNK_OUTDOOR_SECTORS	= 0xC050,
			BSPCHUNK_END				= 0xC0FF
		};

		enum zTBspTreeMode
		{
			zBSP_MODE_INDOOR,
			zBSP_MODE_OUTDOOR
		};

		class zCBspBase
		{
		public:

			zCBspBase() { }

			bool SaveBIN(FileStream*, zCBspTree*, GAME);
			bool LoadBIN(FileStream*, zCBspTree*, GAME);

			virtual bool IsNode() = 0;

			zTBBox3D	bbox;
			uint32_t	polyOffset;
			uint32_t	polyCount;
			
		};

		class zCBspNode : public zCBspBase
		{
		public:

			zCBspNode() { }

			virtual bool IsNode() { return true; }

			zTPlane		plane;
			zCBspBase*	front = nullptr;
			zCBspBase*	back = nullptr;
			bool		renderLod;	// Legacy

		};

		class zCBspLeaf : public zCBspBase
		{
		public:

			zCBspLeaf() { }

			virtual bool IsNode() { return false; }

		};

		class zCBspSector
		{
		public:

			zCBspSector() {}

			std::string				name;
			std::vector<uint32_t>	nodes;
			std::vector<uint32_t>	portalPolys;

		};

		class zCBspTree
		{
		public:

			zCBspTree()				{ }
			virtual ~zCBspTree()	{ }

			bool SaveBIN(FileStream*, GAME);
			bool LoadBIN(FileStream*, GAME);

			zCMesh						mesh;
			zTBspTreeMode				mode;
			std::vector<uint32_t>		polys;
			zCBspBase*					root = nullptr;
			std::vector<zVEC3>			lightPositions;
			std::vector<zCBspSector>	sectors;
			std::vector<uint32_t>		portalPolys;

			size_t nodeCount = 0; //todo: friend
			size_t leafCount = 0;//todo: friend

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
				spcCDragVob [PRE-DEMO5]
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
				{ GAME_DEMO3,				0		},
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				52224	},
				{ GAME_GOTHIC2ADDON,		52224	},
			};

			ZEN_DECLARE_CLASS(zCVob, zCObject);

			zCVob()				{ vobType = zVOB_TYPE_NORMAL; }
			virtual ~zCVob()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			bool SaveVob(FileStream*, GAME);
			bool LoadVob(FileStream*, GAME);

			std::vector<zCVob*> children;

			/*
				Properties
			*/

			zTVobType			vobType					= zVOB_TYPE_NORMAL;		// Legacy
			int					vobID;											// Legacy
			std::string			presetName;
			zBOOL				drawBBox3D				= false;				// Legacy
			zTBBox3D			bbox3DWS				= {};
			zMAT3				trafoRot				= {};					// Legacy
			zVEC3				trafoPos				= {};					// Legacy
			zMAT3				trafoOSToWSRot			= {};
			zVEC3				trafoOSToWSPos			= {};
			std::string			vobName;
			std::string			visual;
			zBOOL				showVisual				= true;
			zTVisualCamAlign	visualCamAlign			= zVISUAL_CAMALIGN_NONE;
			zTAnimationMode		visualAniMode			= zVISUAL_ANIMODE_NONE;	// Gothic 2
			float				visualAniModeStrength	= 0;					// Gothic 2
			float				vobFarClipZScale		= 0;					// Gothic 2
			zBOOL				cdStatic				= false;
			zBOOL				cdDyn					= false;
			zBOOL				staticVob				= false;
			zTDynShadowType		dynShadow				= zDYN_SHADOW_TYPE_NONE;
			int					zbias					= 0;					// Gothic 2
			zBOOL				isAmbient				= false;				// Gothic 2
			zBOOL				physicsEnabled			= false;				// Savegame
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
				{ GAME_DEMO5,				0				},
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	17408			},
				{ GAME_GOTHIC1,				12289			},
				{ GAME_GOTHICSEQUEL,		12289			},
				{ GAME_GOTHIC2,				20483			},
				{ GAME_GOTHIC2ADDON,		20483			},
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
				{ GAME_SEPTEMBERDEMO,		3840	},
				{ GAME_CHRISTMASEDITION,	3073	},
				{ GAME_GOTHIC1,				30720	},
				{ GAME_GOTHICSEQUEL,		30720	},
				{ GAME_GOTHIC2,				12289	},
				{ GAME_GOTHIC2ADDON,		12289	},
				{ GAME_GOTHIC2ADDON,		33793	}, // Spacer 2.6
			};

			ZEN_DECLARE_CLASS(zCCSCamera, zCVob);

			zCCSCamera()			{ }
			virtual ~zCCSCamera()	{ }
			
			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			zTCamTrj_FOR		camTrjFOR						= zCAMTRJ_KEY_FOR_WORLD;
			zTCamTrj_FOR		targetTrjFOR					= zCAMTRJ_KEY_FOR_WORLD;
			zTCamTrj_LoopMode	loopMode						= zCAMTRJ_LOOPMODE_NONE;
			zTSplLerpMode		splLerpMode						= zTSPL_LERP_PATH;
			zBOOL				ignoreFORVobRotCam				= false;
			zBOOL				ignoreFORVobRotTarget			= false;
			zBOOL				adaptToSurroundings				= true;
			zBOOL				easeToFirstKey					= false;
			zBOOL				easeFromLastKey					= false;
			float				totalTime						= 10.0f;
			std::string			autoCamFocusVobName;
			zBOOL				autoCamPlayerMovable			= true;
			zBOOL				autoCamUntriggerOnLastKey		= true;
			float				autoCamUntriggerOnLastKeyDelay	= 0.0f;
			zBOOL				paused							= false;	// Savegame
			zBOOL				started							= false;	// Savegame
			zBOOL				gotoTimeMode					= false;	// Savegame
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
				{ GAME_SEPTEMBERDEMO,		56833	},
				{ GAME_CHRISTMASEDITION,	62209	},
				{ GAME_GOTHIC1,				34560	},
				{ GAME_GOTHICSEQUEL,		34560	},
				{ GAME_GOTHIC2,				9218	},
				{ GAME_GOTHIC2ADDON,		9218	},
				{ GAME_GOTHIC2ADDON,		31489	}, // Spacer 2.6
			};

			ZEN_DECLARE_CLASS(zCCamTrj_KeyFrame, zCVob);

			zCCamTrj_KeyFrame()				{ }
			virtual ~zCCamTrj_KeyFrame()	{ }
			
			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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
			zBOOL					timeIsFixed			= false;
			zMAT4					originalPose		= { };							// Savegame

		private:

		};

		/*
			oCDummyVobGenerator
		*/

		class oCDummyVobGenerator : public zCVob
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0				},
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	17408			},
				{ GAME_GOTHIC1,				12289			},
				{ GAME_GOTHICSEQUEL,		12289			},
				{ GAME_GOTHIC2,				20483			},
				{ GAME_GOTHIC2ADDON,		20483			},
			};

			ZEN_DECLARE_CLASS(oCDummyVobGenerator, zCVob);

			oCDummyVobGenerator()			{ }
			virtual ~oCDummyVobGenerator()	{ }

		private:

		};

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
				{ GAME_DEMO5,				0				},
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	17408			},
				{ GAME_GOTHIC1,				12289			},
				{ GAME_GOTHICSEQUEL,		12289			},
				{ GAME_GOTHIC2,				20483			},
				{ GAME_GOTHIC2ADDON,		20483			},
			};

			ZEN_DECLARE_CLASS_ABSTRACT(zCEffect, zCVob);

			zCEffect()			{ }
			virtual ~zCEffect()	{ }

		private:

		};

		class zCEarthquake : public zCEffect
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	0		},
				{ GAME_GOTHIC1,				0		},
				{ GAME_GOTHICSEQUEL,		0		},
				{ GAME_GOTHIC2,				20483	},
				{ GAME_GOTHIC2ADDON,		20483	},
				{ GAME_GOTHIC2ADDON,		0		}, // Spacer 2.6
			};

			ZEN_DECLARE_CLASS(zCEarthquake, zCEffect);

			zCEarthquake()			{ }
			virtual ~zCEarthquake()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			float radius		= 0.0f;
			float timeSec		= 0.0f;
			zVEC3 amplitudeCM	= { };

		private:

		};

		class zCPFXControler : public zCEffect
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				20483	},
				{ GAME_GOTHIC2ADDON,		20483	},
				{ GAME_GOTHIC2ADDON,		52224	}, // Spacer 2.6
			};

			ZEN_DECLARE_CLASS(zCPFXControler, zCEffect);

			zCPFXControler()			{ }
			virtual ~zCPFXControler()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			std::string pfxName;
			zBOOL killVobWhenDone	= true;
			zBOOL pfxStartOn			= true;

		private:

		};

		class zCVobAnimate : public zCEffect
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				52224	},
				{ GAME_GOTHIC2ADDON,		52224	},
			};

			ZEN_DECLARE_CLASS(zCVobAnimate, zCEffect);

			zCVobAnimate()			{ }
			virtual ~zCVobAnimate()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			zBOOL startOn	= true;
			zBOOL isRunning	= false; // Savegame

		private:

		};

		class zCTouchDamage : public zCEffect
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	17408			},
				{ GAME_GOTHIC1,				64704			},
				{ GAME_GOTHICSEQUEL,		64704			},
				{ GAME_GOTHIC2,				33026			},
				{ GAME_GOTHIC2ADDON,		33026			},
			};

			ZEN_DECLARE_CLASS(zCTouchDamage, zCEffect);

			zCTouchDamage()				{ }
			virtual ~zCTouchDamage()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			enum zTDamageCollType
			{
				CT_NONE,
				CT_BOX,
				CT_POINT,
			};

			float				damage					= 0.0f;
			zBOOL				Barrier					= false;
			zBOOL				Blunt					= false;
			zBOOL				Edge					= false;
			zBOOL				Fire					= false;
			zBOOL				Fly						= false;
			zBOOL				Magic					= false;
			zBOOL				Point					= false;
			zBOOL				Fall					= false;
			float				damageRepeatDelaySec	= 0.0f;
			float				damageVolDownScale		= 1.0f;
			zTDamageCollType	damageCollType			= CT_BOX;

		private:

		};

		class oCTouchDamage : public zCTouchDamage
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	0				},
				{ GAME_GOTHIC1,				36865			},
				{ GAME_GOTHICSEQUEL,		36865			},
				{ GAME_GOTHIC2,				36865			},
				{ GAME_GOTHIC2ADDON,		36865			},
			};

			ZEN_DECLARE_CLASS(oCTouchDamage, zCTouchDamage);

			oCTouchDamage()				{ }
			virtual ~oCTouchDamage()	{ }

		private:

		};

		class zCVobLensFlare : public zCEffect
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		29888	},
				{ GAME_CHRISTMASEDITION,	35009	},
				{ GAME_GOTHIC1,				64704	},
				{ GAME_GOTHICSEQUEL,		64704	},
				{ GAME_GOTHIC2,				193		},
				{ GAME_GOTHIC2ADDON,		193		},
			};

			ZEN_DECLARE_CLASS(zCVobLensFlare, zCEffect);

			zCVobLensFlare()			{ }
			virtual ~zCVobLensFlare()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			std::string lensflareFX;

		private:

		};

		class zCVobScreenFX : public zCEffect
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				193		},
				{ GAME_GOTHIC2ADDON,		193		},
			};

			ZEN_DECLARE_CLASS(zCVobScreenFX, zCEffect);

			zCVobScreenFX()				{ }
			virtual ~zCVobScreenFX()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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
				{ GAME_DEMO3,				0 },
				{ GAME_DEMO5,				0 },
				{ GAME_SEPTEMBERDEMO,		0 },
				{ GAME_CHRISTMASEDITION,	0 },
				{ GAME_GOTHIC1,				0 },
				{ GAME_GOTHICSEQUEL,		0 },
				{ GAME_GOTHIC2,				0 },
				{ GAME_GOTHIC2ADDON,		0 },
			};

			ZEN_DECLARE_CLASS(oCItem, oCVob);

			oCItem()				{ vobType = zVOB_TYPE_ITEM; }
			virtual ~oCItem()		{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			virtual bool Save(FileStream*, GAME);
			virtual bool Load(FileStream*, GAME);

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
				{ GAME_DEMO5,				36865	},
				{ GAME_DEMO5,				0		}, // TESTLEVEL_EVENT.ZEN
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	47105	},
				{ GAME_GOTHIC1,				47105	},
				{ GAME_GOTHICSEQUEL,		47105	},
				{ GAME_GOTHIC2,				47105	},
				{ GAME_GOTHIC2ADDON,		47105	},
			};

			ZEN_DECLARE_CLASS(oCMOB, oCVob);

			oCMOB()				{ vobType = zVOB_TYPE_MOB; }
			virtual ~oCMOB()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			std::string		focusName;
			int				hitpoints		= 0;
			int				damage			= 0;
			zBOOL			moveable		= false;
			zBOOL			takeable		= false;
			zBOOL			focusOverride	= false;
			oTSndMaterial	soundMaterial	= SND_MAT_WOOD;
			std::string		visualDestroyed;
			std::string		owner;
			std::string		ownerGuild;
			zBOOL			isDestroyed		= false;	// Props

		private:

		};

		class oCMobInter : public oCMOB
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				36865	},
				{ GAME_DEMO5,				0		}, // TESTLEVEL_EVENT.ZEN
				{ GAME_SEPTEMBERDEMO,		35585	},
				{ GAME_CHRISTMASEDITION,	35585	},
				{ GAME_GOTHIC1,				35585	},
				{ GAME_GOTHICSEQUEL,		35585	},
				{ GAME_GOTHIC2,				35585	},
				{ GAME_GOTHIC2ADDON,		35585	},
			};

			ZEN_DECLARE_CLASS(oCMobInter, oCMOB);

			oCMobInter()			{ }
			virtual ~oCMobInter()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			int			state			= 0;		// Legacy
			int			stateTarget		= 0;		// Legacy
			int			stateNum		= 0;
			zBOOL		interact		= false;	// Legacy
			std::string	triggerTarget;
			std::string	useWithItem;
			std::string	conditionFunc;
			std::string	onStateFunc;
			zBOOL		rewind			= false;

		private:

		};

		class oCMobBed : public oCMobInter
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				36865	},
				{ GAME_SEPTEMBERDEMO,		61440	},
				{ GAME_CHRISTMASEDITION,	61440	},
				{ GAME_GOTHIC1,				61440	},
				{ GAME_GOTHICSEQUEL,		61440	},
				{ GAME_GOTHIC2,				61440	},
				{ GAME_GOTHIC2ADDON,		61440	},
			};

			ZEN_DECLARE_CLASS(oCMobBed, oCMobInter);

			oCMobBed()			{ }
			virtual ~oCMobBed()	{ }

		private:

		};

		class oCMobFire : public oCMobInter
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		18433	},
				{ GAME_CHRISTMASEDITION,	18433	},
				{ GAME_GOTHIC1,				18433	},
				{ GAME_GOTHICSEQUEL,		18433	},
				{ GAME_GOTHIC2,				18433	},
				{ GAME_GOTHIC2ADDON,		18433	},
			};

			ZEN_DECLARE_CLASS(oCMobFire, oCMobInter);

			oCMobFire()				{ }
			virtual ~oCMobFire()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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
				{ GAME_DEMO5,				36865	},
				{ GAME_SEPTEMBERDEMO,		35585	},
				{ GAME_CHRISTMASEDITION,	35585	},
				{ GAME_GOTHIC1,				35585	},
				{ GAME_GOTHICSEQUEL,		35585	},
				{ GAME_GOTHIC2,				35585	},
				{ GAME_GOTHIC2ADDON,		35585	},
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
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	35585			},
				{ GAME_GOTHIC1,				35585			},
				{ GAME_GOTHICSEQUEL,		35585			},
				{ GAME_GOTHIC2,				35585			},
				{ GAME_GOTHIC2ADDON,		35585			},
			};

			ZEN_DECLARE_CLASS_ABSTRACT(oCMobLockable, oCMobInter);

			oCMobLockable()				{ }
			virtual ~oCMobLockable()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			zBOOL		locked		= false;
			std::string	keyInstance;
			std::string	pickLockStr;


		private:

		};

		class oCMobContainer : public oCMobLockable
		{
		public:
			
			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				36865	},
				{ GAME_DEMO5,				0		}, // TESTLEVEL_EVENT.ZEN
				{ GAME_SEPTEMBERDEMO,		61440	},
				{ GAME_CHRISTMASEDITION,	64513	},
				{ GAME_GOTHIC1,				64513	},
				{ GAME_GOTHICSEQUEL,		64513	},
				{ GAME_GOTHIC2,				64513	},
				{ GAME_GOTHIC2ADDON,		64513	},
			};

			ZEN_DECLARE_CLASS(oCMobContainer, oCMobLockable);

			oCMobContainer()			{ }
			virtual ~oCMobContainer()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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
				{ GAME_SEPTEMBERDEMO,		61440	},
				{ GAME_CHRISTMASEDITION,	64513	},
				{ GAME_GOTHIC1,				64513	},
				{ GAME_GOTHICSEQUEL,		64513	},
				{ GAME_GOTHIC2,				64513	},
				{ GAME_GOTHIC2ADDON,		64513	},
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
				{ GAME_DEMO5,				36865	},
				{ GAME_DEMO5,				0		}, // TESTLEVEL_EVENT.ZEN
				{ GAME_SEPTEMBERDEMO,		35585	},
				{ GAME_CHRISTMASEDITION,	35585	},
				{ GAME_GOTHIC1,				35585	},
				{ GAME_GOTHICSEQUEL,		35585	},
				{ GAME_GOTHIC2,				35585	},
				{ GAME_GOTHIC2ADDON,		35585	},
			};

			ZEN_DECLARE_CLASS(oCMobSwitch, oCMobLockable);

			oCMobSwitch()			{ }
			virtual ~oCMobSwitch()	{ }

		private:

		};

		class oCMobWheel : public oCMobInter
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				36865	},
				{ GAME_SEPTEMBERDEMO,		35585	},
				{ GAME_CHRISTMASEDITION,	35585	},
				{ GAME_GOTHIC1,				35585	},
				{ GAME_GOTHICSEQUEL,		35585	},
				{ GAME_GOTHIC2,				35585	},
				{ GAME_GOTHIC2ADDON,		35585	},
			};

			ZEN_DECLARE_CLASS(oCMobWheel, oCMobInter);

			oCMobWheel()			{ }
			virtual ~oCMobWheel()	{ }

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
				{ GAME_DEMO3,				0 },
				{ GAME_DEMO5,				0 },
			};

			ZEN_DECLARE_CLASS(oCMob, oCVob);

			oCMob()				{ vobType = zVOB_TYPE_MOB; }
			virtual ~oCMob()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			virtual bool Save(FileStream*, GAME);
			virtual bool Load(FileStream*, GAME);

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
				{ GAME_DEMO3,				0				},
				{ GAME_DEMO5,				36865			},
				{ GAME_DEMO5,				0				}, // TESTLEVEL.ZEN
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	52224			},
				{ GAME_GOTHIC1,				52224			},
				{ GAME_GOTHICSEQUEL,		52224			},
				{ GAME_GOTHIC2,				29697			},
				{ GAME_GOTHIC2ADDON,		29697			},
			};

			ZEN_DECLARE_CLASS(oCNpc, oCVob);

			oCNpc()				{ vobType = zVOB_TYPE_NSC; }
			virtual ~oCNpc()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			virtual bool Save(FileStream*, GAME);
			virtual bool Load(FileStream*, GAME);

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
			spcCDragVob

			Legacy class.
			Only included to support demo5 loading.
		*/

		class spcCDragVob : public zCVob
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,	0	},
			};

			ZEN_DECLARE_CLASS(spcCDragVob, zCVob);

			spcCDragVob()			{ }
			virtual ~spcCDragVob()	{ }

		private:

		};

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
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	17408			},
				{ GAME_GOTHIC1,				12289			},
				{ GAME_GOTHICSEQUEL,		12289			},
				{ GAME_GOTHIC2,				20483			},
				{ GAME_GOTHIC2ADDON,		20483			},
			};

			ZEN_DECLARE_CLASS_ABSTRACT(zCTriggerBase, zCVob);

			zCTriggerBase()				{ }
			virtual ~zCTriggerBase()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			std::string triggerTarget;

		private:

		};

		class zCCodeMaster : public zCTriggerBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	0				},
				{ GAME_GOTHIC1,				0				},
				{ GAME_GOTHICSEQUEL,		0				},
				{ GAME_GOTHIC2,				0				},
				{ GAME_GOTHIC2ADDON,		0				},
			};

			ZEN_DECLARE_CLASS(zCCodeMaster, zCTriggerBase);

			zCCodeMaster()			{ }
			virtual ~zCCodeMaster()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			zBOOL		orderRelevant			= false;
			zBOOL		firstFalseIsFailure		= false;
			std::string	triggerTargetFailure;
			zBOOL		untriggerCancels		= false;

			std::vector<std::string>	slaveVobNameList;
			std::vector<zCVob*>			slaveTriggeredList;

		private:

		};

		enum zTMessageType
		{
			MT_NONE,
			MT_TRIGGER,
			MT_UNTRIGGER,
			MT_ENABLE,
			MT_DISABLE,
			MT_TOGGLE_ENABLED,
			MT_RESET
		};

		class zCMessageFilter : public zCTriggerBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	0		},
				{ GAME_GOTHIC1,				0		},
				{ GAME_GOTHICSEQUEL,		0		},
				{ GAME_GOTHIC2,				0		},
				{ GAME_GOTHIC2ADDON,		0		},
			};

			ZEN_DECLARE_CLASS(zCMessageFilter, zCTriggerBase);

			zCMessageFilter() { }
			virtual ~zCMessageFilter() { }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			zTMessageType onTrigger		= MT_NONE;
			zTMessageType onUntrigger	= MT_NONE;

		private:

		};

		enum zTEventMoverSubType
		{
			zEVENT_GOTO_KEY_FIXED_DIRECTLY,
			zEVENT_GOTO_KEY_FIXED_ORDER,
			zEVENT_GOTO_KEY_NEXT,
			zEVENT_GOTO_KEY_PREV,
			zEVENT_MISC_NUM_SUBTYPES
		};

		class zCMoverControler : public zCTriggerBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	0				},
				{ GAME_GOTHIC1,				0				},
				{ GAME_GOTHICSEQUEL,		0				},
				{ GAME_GOTHIC2,				0				},
				{ GAME_GOTHIC2ADDON,		0				},
			};

			ZEN_DECLARE_CLASS(zCMoverControler, zCTriggerBase);

			zCMoverControler()			{ }
			virtual ~zCMoverControler()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			zTEventMoverSubType	moverMessage = zEVENT_GOTO_KEY_FIXED_DIRECTLY;
			int					gotoFixedKey = 0;

		private:

		};

		enum zTRepeatTrigger
		{
			RT_NONE,
			RT_REPEAT,
			RT_REPEAT_TOUCHING
		};

		class zCTrigger : public zCTriggerBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		24960	},
				{ GAME_CHRISTMASEDITION,	47105	},
				{ GAME_GOTHIC1,				47105	},
				{ GAME_GOTHICSEQUEL,		47105	},
				{ GAME_GOTHIC2,				47105	},
				{ GAME_GOTHIC2ADDON,		47105	},
			};

			ZEN_DECLARE_CLASS(zCTrigger, zCTriggerBase);

			zCTrigger()				{ }
			virtual ~zCTrigger()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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

			std::string		respondToVobName;
			int				numTriggerToActivate	= 0;		// Legacy
			int				numCanBeActivated		= 0;
			float			retriggerWaitSec		= 0.0f;
			float			damageThreshold			= 0.0f;
			float			fireDelaySec			= 0.0f;
			zTRepeatTrigger	repeatTrigger			= RT_NONE;	// Legacy
			zBOOL			sendUntrigger			= false;	// Props
			float			nextTimeTriggerable		= 0.0f;		// Savegame
			zCVob*			savedOtherVobPtr		= nullptr;	// Savegame
			int				countCanBeActivated		= 0;		// Savegame	

		private:

		};

		class oCCSTrigger : public zCTrigger
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		24577	},
				{ GAME_CHRISTMASEDITION,	24577	},
				{ GAME_GOTHIC1,				24577	},
				{ GAME_GOTHICSEQUEL,		24577	},
				{ GAME_GOTHIC2,				24577	},
				{ GAME_GOTHIC2ADDON,		24577	},
			};

			ZEN_DECLARE_CLASS(oCCSTrigger, zCTrigger);

			oCCSTrigger()			{ }
			virtual ~oCCSTrigger()	{ }

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
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		39936	},
				{ GAME_CHRISTMASEDITION,	39936	},
				{ GAME_GOTHIC1,				39936	},
				{ GAME_GOTHICSEQUEL,		39936	},
				{ GAME_GOTHIC2,				39936	},
				{ GAME_GOTHIC2ADDON,		39936	},
			};

			ZEN_DECLARE_CLASS(zCMover, zCTrigger);

			zCMover()			{ }
			virtual ~zCMover()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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
			zBOOL						moverLocked			= false;
			zBOOL						autoLinkEnabled		= false;
			zBOOL						autoRotate			= false;				// Gothic 2
			std::string					vobChainName;								// Legacy
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
				{ GAME_SEPTEMBERDEMO,		24577	},
				{ GAME_CHRISTMASEDITION,	24577	},
				{ GAME_GOTHIC1,				24577	},
				{ GAME_GOTHICSEQUEL,		24577	},
				{ GAME_GOTHIC2,				24577	},
				{ GAME_GOTHIC2ADDON,		24577	},
			};

			ZEN_DECLARE_CLASS(oCTriggerChangeLevel, zCTrigger);

			oCTriggerChangeLevel()			{ }
			virtual ~oCTriggerChangeLevel()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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
				{ GAME_SEPTEMBERDEMO,		24960	},
				{ GAME_CHRISTMASEDITION,	40321	},
				{ GAME_GOTHIC1,				59776	},
				{ GAME_GOTHICSEQUEL,		59776	},
				{ GAME_GOTHIC2,				45570	},
				{ GAME_GOTHIC2ADDON,		45570	},
				{ GAME_GOTHIC2ADDON,		5505	}, // Spacer 2.6
			};

			ZEN_DECLARE_CLASS(zCTriggerList, zCTrigger);

			zCTriggerList()			{ }
			virtual ~zCTriggerList()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			zTListProcess				listProcess		= LP_ALL;
			std::vector<std::string>	triggerTargetList;
			std::vector<float>			fireDelayList;
			unsigned char				actTarget		= 0;	// Savegame
			zBOOL						sendOnTrigger	= true;	// Savegame

		private:

		};

		class oCTriggerScript : public zCTrigger
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		24577	},
				{ GAME_CHRISTMASEDITION,	24577	},
				{ GAME_GOTHIC1,				24577	},
				{ GAME_GOTHICSEQUEL,		24577	},
				{ GAME_GOTHIC2,				24577	},
				{ GAME_GOTHIC2ADDON,		24577	},
			};

			ZEN_DECLARE_CLASS(oCTriggerScript, zCTrigger);

			oCTriggerScript()			{ }
			virtual ~oCTriggerScript()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			std::string scriptFunc;

		private:

		};

		class zCTriggerTeleport : public zCTrigger
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0				},
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	40321			},
				{ GAME_GOTHIC1,				59776			},
				{ GAME_GOTHICSEQUEL,		59776			},
				{ GAME_GOTHIC2,				45570			},
				{ GAME_GOTHIC2ADDON,		45570			},
			};

			ZEN_DECLARE_CLASS(zCTriggerTeleport, zCTrigger);

			zCTriggerTeleport()				{ }
			virtual ~zCTriggerTeleport()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			std::string sfxTeleport;

		private:

		};

		class zCTriggerUntouch : public zCTriggerBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	35009			},
				{ GAME_GOTHIC1,				64704			},
				{ GAME_GOTHICSEQUEL,		64704			},
				{ GAME_GOTHIC2,				33026			},
				{ GAME_GOTHIC2ADDON,		33026			},
				{ GAME_GOTHIC2ADDON,		193				}, // Spacer 2.6
			};

			ZEN_DECLARE_CLASS(zCTriggerUntouch, zCTriggerBase);

			zCTriggerUntouch()			{ }
			virtual ~zCTriggerUntouch()	{ }

		private:

		};

		class zCTriggerWorldStart : public zCTriggerBase
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	17408			},
				{ GAME_GOTHIC1,				12289			},
				{ GAME_GOTHICSEQUEL,		12289			},
				{ GAME_GOTHIC2,				20483			},
				{ GAME_GOTHIC2ADDON,		20483			},
				{ GAME_GOTHIC2ADDON,		52224			}, // Spacer 2.6
			};

			ZEN_DECLARE_CLASS(zCTriggerWorldStart, zCTriggerBase);

			zCTriggerWorldStart()			{ }
			virtual ~zCTriggerWorldStart()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			zBOOL fireOnlyFirstTime	= false;
			zBOOL hasFired			= false; // Savegame

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
				{ GAME_DEMO3,				0		},
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				52224	},
				{ GAME_GOTHIC2ADDON,		52224	},
			};

			ZEN_DECLARE_CLASS(zCVobLevelCompo, zCVob);

			zCVobLevelCompo()			{ vobType = zVOB_TYPE_LEVEL_COMPONENT; }
			virtual ~zCVobLevelCompo()	{ }

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
				{ GAME_DEMO3,				0		},
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		15360	},
				{ GAME_CHRISTMASEDITION,	49153	},
				{ GAME_GOTHIC1,				46080	},
				{ GAME_GOTHICSEQUEL,		46080	},
				{ GAME_GOTHIC2,				39168	},
				{ GAME_GOTHIC2ADDON,		39168	},
			};

			ZEN_DECLARE_CLASS(zCVobLight, zCVob);

			zCVobLight()			{ vobType = zVOB_TYPE_LIGHT; }
			virtual ~zCVobLight()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			virtual bool Save(FileStream*, GAME);
			virtual bool Load(FileStream*, GAME);

			/*
				Properties
			*/

			std::string			lightPresetInUse;
			zTVobLightType		lightType			= zVOBLIGHT_TYPE_POINT;
			float				range				= 2000;
			zCOLOR				color				= { };
			float				spotConeAngle		= 0;
			zBOOL				lightStatic			= 0;
			zTVobLightQuality	lightQuality		= zVOBLIGHT_QUAL_FASTEST;
			int					lensflareFXNo		= -1;						// Legacy
			std::string			lensflareFX;
			zBOOL				turnedOn			= true;
			std::vector<float>	rangeAniScaleList;
			float				rangeAniFPS			= 0;
			zBOOL				rangeAniSmooth		= 0;
			std::vector<zCOLOR>	colorAniList;
			float				colorAniFPS			= 0;
			zBOOL				colorAniSmooth		= 0;
			zBOOL				canMove				= 0;						// Gothic 2

		private:

		};

		/*
			zCVobMarker

			Legacy class. Only included to support demo5 loading.
		*/

		class zCVobMarker : public zCVob
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO3,				0 },
				{ GAME_DEMO5,				0 },
			};

			ZEN_DECLARE_CLASS(zCVobMarker, zCVob);

			zCVobMarker()			{ vobType = zVOB_TYPE_MARKER; }
			virtual ~zCVobMarker()	{ }

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
				{ GAME_DEMO3,				0		},
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				52224	},
				{ GAME_GOTHIC2ADDON,		52224	},
			};

			ZEN_DECLARE_CLASS(zCVobSpot, zCVob);

			zCVobSpot()				{ vobType = zVOB_TYPE_SPOT; }
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
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				52224	},
				{ GAME_GOTHIC2ADDON,		52224	},
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
				{ GAME_DEMO3,				0		},
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				52224	},
				{ GAME_GOTHIC2ADDON,		52224	},
			};

			ZEN_DECLARE_CLASS(zCVobStartpoint, zCVob);

			zCVobStartpoint()			{ vobType = zVOB_TYPE_STARTPOINT; }
			virtual ~zCVobStartpoint()	{ }

		private:

		};

		/*
			zCVobWaypoint
		*/

		enum WAYPROP
		{
			WAYPROP_JUMP	= 1,
			WAYPROP_CLIMB	= 2,
			WAYPROP_SWIM	= 4,
			WAYPROP_DIVE	= 8,
			WAYPROP_FREE	= 16
		};

		class zCVobWaypoint : public zCVob
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO3,				0		},
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				52224	},
				{ GAME_GOTHIC2ADDON,		52224	},
			};

			ZEN_DECLARE_CLASS(zCVobWaypoint, zCVob);

			zCVobWaypoint()				{ vobType = zVOB_TYPE_WAYPOINT; }
			virtual ~zCVobWaypoint()	{ }

			WAYPROP wayProp = (WAYPROP)0;

		private:

		};

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
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	17408			},
				{ GAME_GOTHIC1,				12289			},
				{ GAME_GOTHICSEQUEL,		12289			},
				{ GAME_GOTHIC2,				52224			},
				{ GAME_GOTHIC2ADDON,		52224			},
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
				{ GAME_DEMO3,				0		},
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		12289	},
				{ GAME_CHRISTMASEDITION,	12289	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				12289	},
				{ GAME_GOTHIC2ADDON,		12289	},
			};

			ZEN_DECLARE_CLASS(zCVobSound, zCZone);
		
			zCVobSound()			{ vobType = zVOB_TYPE_SOUND; }
			virtual ~zCVobSound()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			float			sndVolume			= 100;
			zTSoundMode		sndMode				= SM_LOOPING;
			float			sndRandDelay		= 5;
			float			sndRandDelayVar		= 2;
			zBOOL			sndStartOn			= 1;
			zBOOL			sndAmbient3D		= 0;
			zBOOL			sndObstruction		= 0;
			float			sndConeAngle		= 0;
			zTSoundVolType	sndVolType			= SV_SPHERE;
			float			sndRadius			= 1500;
			std::string		sndName;
			zBOOL			soundIsRunning		= 0; // Savegame
			zBOOL			soundAllowedToRun	= 0; // Savegame

		private:

		};

		class zCVobSoundDaytime : public zCVobSound
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		12289	},
				{ GAME_CHRISTMASEDITION,	12289	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				12289	},
				{ GAME_GOTHIC2ADDON,		12289	},
			};

			ZEN_DECLARE_CLASS(zCVobSoundDaytime, zCVobSound);
		
			zCVobSoundDaytime()				{ }
			virtual ~zCVobSoundDaytime()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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
				{ GAME_SEPTEMBERDEMO,		VERSION_NONE	},
				{ GAME_CHRISTMASEDITION,	17408			},
				{ GAME_GOTHIC1,				12289			},
				{ GAME_GOTHICSEQUEL,		12289			},
				{ GAME_GOTHIC2,				52224			},
				{ GAME_GOTHIC2ADDON,		52224			},
			};

			ZEN_DECLARE_CLASS_ABSTRACT(zCZoneMusic, zCZone);

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

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			zBOOL	enabled				= true;
			int		priority			= 1;
			zBOOL	ellipsoid			= false;
			float	reverbLevel			= -3.219f;
			float	volumeLevel			= 1.0f;
			zBOOL	loop				= true;
			zBOOL	local_enabled		= true;		// Savegame
			zBOOL	dayEntranceDone		= false;	// Savegame
			zBOOL	nightEntranceDone	= false;	// Savegame

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
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				52224	},
				{ GAME_GOTHIC2ADDON,		52224	},
			};

			ZEN_DECLARE_CLASS(zCZoneVobFarPlane, zCZone);

			zCZoneVobFarPlane()				{ }
			virtual ~zCZoneVobFarPlane()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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
				{ GAME_SEPTEMBERDEMO,		47105	},
				{ GAME_CHRISTMASEDITION,	17408	},
				{ GAME_GOTHIC1,				12289	},
				{ GAME_GOTHICSEQUEL,		12289	},
				{ GAME_GOTHIC2,				52224	},
				{ GAME_GOTHIC2ADDON,		52224	},
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
				{ GAME_SEPTEMBERDEMO,		29888	},
				{ GAME_CHRISTMASEDITION,	35009	},
				{ GAME_GOTHIC1,				64704	},
				{ GAME_GOTHICSEQUEL,		64704	},
				{ GAME_GOTHIC2,				5505	},
				{ GAME_GOTHIC2ADDON,		5505	},
			};

			ZEN_DECLARE_CLASS(zCZoneZFog, zCZone);

			zCZoneZFog()			{ }
			virtual ~zCZoneZFog()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			float	fogRangeCenter	= 3000.0f;
			float	innerRangePerc	= 0.7f;
			zCOLOR	fogColor		= { };
			zBOOL	fadeOutSky = false;	// Gothic 2
			zBOOL	overrideColor	= false;	// Gothic 2

		private:

		};

		class zCZoneZFogDefault : public zCZoneZFog
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_SEPTEMBERDEMO,		29888	},
				{ GAME_CHRISTMASEDITION,	35009	},
				{ GAME_GOTHIC1,				64704	},
				{ GAME_GOTHICSEQUEL,		64704	},
				{ GAME_GOTHIC2,				5505	},
				{ GAME_GOTHIC2ADDON,		5505	},
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

			ZEN_DECLARE_CLASS_ABSTRACT(zCAIBase, zCObject);

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
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		0		},
				{ GAME_CHRISTMASEDITION,	0		},
				{ GAME_GOTHIC1,				53505	},
				{ GAME_GOTHICSEQUEL,		53505	},
				{ GAME_GOTHIC2,				53505	},
				{ GAME_GOTHIC2ADDON,		53505	},
			};

			ZEN_DECLARE_CLASS(zCAIPlayer, zCAIBase);

			zCAIPlayer()			{ }
			virtual ~zCAIPlayer()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

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
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		0		},
				{ GAME_CHRISTMASEDITION,	0		},
				{ GAME_GOTHIC1,				36865	},
				{ GAME_GOTHICSEQUEL,		36865	},
				{ GAME_GOTHIC2,				36865	},
				{ GAME_GOTHIC2ADDON,		36865	},
			};

			ZEN_DECLARE_CLASS(oCAniCtrl_Human, zCAIPlayer);

			oCAniCtrl_Human()			{ }
			virtual ~oCAniCtrl_Human()	{ }

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/

			oCNpc* aiNpc = nullptr;

			int		walkMode		= 0;
			int		weaponMode		= 0;
			int		wmodeLast		= 0;
			int		wmodeSelect		= 0;
			zBOOL	changeWeapon	= false;
			int		actionMode		= 0;

		private:

		};

		class oCAIHuman : public oCAniCtrl_Human
		{
		public:
			
			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO5,				0		},
				{ GAME_SEPTEMBERDEMO,		0		},
				{ GAME_CHRISTMASEDITION,	0		},
				{ GAME_GOTHIC1,				0		},
				{ GAME_GOTHICSEQUEL,		0		},
				{ GAME_GOTHIC2,				0		},
				{ GAME_GOTHIC2ADDON,		36865	},
			};

			ZEN_DECLARE_CLASS(oCAIHuman, oCAniCtrl_Human);

			oCAIHuman()				{ }
			virtual ~oCAIHuman()	{ }

		private:

		};

		class oCAIHuman_Stand : public zCAIBase
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

			ZEN_DECLARE_CLASS(oCAIHuman_Stand, zCAIBase);

			oCAIHuman_Stand()			{ }
			virtual ~oCAIHuman_Stand()	{ }

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

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/
			
			zBOOL cleared;
			zBOOL active;

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

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			/*
				Properties
			*/
			
			std::string	wpName;
			int			waterDepth		= 0;
			zBOOL		underWater		= false;
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

		class zCOldWay
		{
		public:

			zCOldWay()			{ }
			virtual ~zCOldWay()	{ }

			/*
				Properties
			*/

			zCVob* left;
			zCVob* right;

		private:

		};

		class zCWayNet : public zCObject
		{
		public:

			inline static CLASS_REVISION revisions[] =
			{
				{ GAME_DEMO3,				0 },
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

			virtual bool Archive(zCArchiver*, GAME);
			virtual bool Unarchive(zCArchiver*, GAME);

			bool SaveWaynet(FileStream*);
			bool LoadWaynet(FileStream*);

			zCWorld* world;

			/*
				Properties
			*/

			int waynetVersion			= 1;
			std::vector<zCWaypoint*>	waypointList;
			std::vector<zCWay>			wayList;
			std::vector<zCVob*>			oldWaypointList;	// Legacy
			std::vector<zCOldWay>		oldWayList;			// Legacy

		private:

		};
	};
};
