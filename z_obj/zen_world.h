#pragma once

#include "zen_base.h"

namespace GothicLib
{
	namespace ZenGin
	{
		class zCVob;
		class zCVisual;
		class zCAIBase;
		class zCWayNet;

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

		private:

		};

		/*
			World classes
		*/

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
		inline bool ParsePWFLine(std::string _line, std::string& key, std::string& value)
		{
			std::string line = _line;

			for (size_t i = 0; i < line.size(); i++)
			{
				if (line[i] != ' ')
				{
					if (i != 0)
						line = line.substr(i);
					break;
				}
			}

			size_t sPos = line.find(" (");
			size_t ePos = line.find(")", sPos + 2);

			if (sPos == std::string::npos ||
				ePos == std::string::npos)
			{
				return false;
			}

			key		= line.substr(0, sPos);
			value	= line.substr(sPos + 2, ePos - sPos - 2);

			return true;
		}
#endif

		class zCWorld : public zCObject
		{
		public:

			DEFINE_CLASS(zCWorld, zCObject);

			zCWorld()			{ }
			virtual ~zCWorld()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

			virtual bool UnarcVobTree(zCVob*, zCArchiver*, size_t&);

			void LoadWorld(FileStream*);
			void SaveWorld(FileStream*);

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
			void LoadWorldFile(FileStream*);
			void SaveWorldFile(FileStream*);
#endif

			zCBspTree	bsp;
			zCVob*		vobTree = nullptr;
			zCWayNet*	wayNet = nullptr;

		private:

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
			void LoadPWFVobTree(zCVob*, FileStream*);
			void SavePWFVobTree(zCVob*, FileStream*);
#endif

		};

		class oCWorld : public zCWorld
		{
		public:

			DEFINE_CLASS(oCWorld, zCWorld);

			oCWorld()			{ }
			virtual ~oCWorld()	{ }

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

		class zCVob : public zCObject
		{
		public:

			DEFINE_CLASS(zCVob, zCObject);

			zCVob()				{ }
			virtual ~zCVob()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
			virtual void Save(FileStream*);
			virtual void Load(FileStream*);
#endif

			std::vector<zCVob*> children;

			/*
				Properties
			*/

			int					pack					= 0;
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

		class zCVobSoundDaytime : public zCVobSound
		{
		public:

			DEFINE_CLASS(zCVobSoundDaytime, zCVobSound);
		
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

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
			virtual void Save(FileStream*);
			virtual void Load(FileStream*);
#endif

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

		class oCNpc : public oCVob
		{
		public:

			DEFINE_CLASS(oCNpc, zCVob);

			oCNpc()				{ }
			virtual ~oCNpc()	{ }

			//virtual bool Archive(zCArchiver*);
			//virtual bool Unarchive(zCArchiver*);

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
			virtual void Save(FileStream*);
			virtual void Load(FileStream*);
#endif

			/*
				Properties
			*/

			std::string		instance;					// Legacy

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

			DEFINE_CLASS(oCMOB, zCVob);

			oCMOB()				{ }
			virtual ~oCMOB()	{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
			virtual void Save(FileStream*);
			virtual void Load(FileStream*);
#endif

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
			std::string		instance;					// Legacy

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

		class oCMobLockable : public oCMobInter
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

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

			DEFINE_CLASS(oCMobContainer, oCMobInter);

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

			DEFINE_CLASS(oCMobDoor, oCMobLockable);

			oCMobDoor()				{ }
			virtual ~oCMobDoor()	{ }

		private:

		};

		class oCMobSwitch : public oCMobInter
		{
		public:

			DEFINE_CLASS(oCMobSwitch, oCMobInter);

			oCMobSwitch()			{ }
			virtual ~oCMobSwitch()	{ }

		private:

		};

		class oCMobFire : public oCMobInter
		{
		public:

			DEFINE_CLASS(oCMobFire, oCMobInter);

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

		enum zTListProcess
		{
			LP_ALL,
			LP_NEXT_ONE,
			LP_RAND_ONE
		};

		class zCTriggerList : public zCTrigger
		{
		public:

			DEFINE_CLASS(zCTriggerList, zCTrigger);

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

			DEFINE_CLASS(zCMover, zCTrigger);

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

		class oCItem : public oCVob
		{
		public:

			DEFINE_CLASS(oCItem, zCVob);

			oCItem()				{ }
			virtual ~oCItem()		{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
			virtual void Save(FileStream*);
			virtual void Load(FileStream*);
#endif

			/*
				Properties
			*/
			
			std::string itemInstance;
			int amount					= 1; // Savegame
			int flags					= 0; // Savegame

		private:

		};

		class zCZone : public zCVob
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

			zCZone()			{ }
			virtual ~zCZone()	{ }

		private:

		};

		class zCZoneMusic : public zCZone
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

			zCZoneMusic()			{ }
			virtual ~zCZoneMusic()	{ }

		private:

		};

		class oCZoneMusic : public zCZoneMusic
		{
		public:

			DEFINE_CLASS(oCZoneMusic, zCVob);

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

			DEFINE_CLASS(oCZoneMusicDefault, oCZoneMusic);

			oCZoneMusicDefault()			{ }
			virtual ~oCZoneMusicDefault()	{ }

		private:

		};

		class zCZoneZFog : public zCZone
		{
		public:

			DEFINE_CLASS(zCZoneZFog, zCVob);

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

			DEFINE_CLASS(zCZoneZFogDefault, zCZoneZFog);

			zCZoneZFogDefault()				{ }
			virtual ~zCZoneZFogDefault()	{ }

		private:

		};

		class zCZoneVobFarPlane : public zCZone
		{
		public:

			DEFINE_CLASS(zCZoneVobFarPlane, zCVob);

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

			DEFINE_CLASS(zCZoneVobFarPlaneDefault, zCZoneVobFarPlane);

			zCZoneVobFarPlaneDefault()			{ }
			virtual ~zCZoneVobFarPlaneDefault()	{ }

		private:

		};

		class zCEffect : public zCVob
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

			zCEffect()			{ }
			virtual ~zCEffect()	{ }

		private:

		};

		class zCPFXControler : public zCEffect
		{
		public:

			DEFINE_CLASS(zCPFXControler, zCVob);

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

		class zCVobScreenFX : public zCEffect
		{
		public:

			DEFINE_CLASS(zCVobScreenFX, zCVob);

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

		class zCVobAnimate : public zCEffect
		{
		public:

			DEFINE_CLASS(zCVobAnimate, zCVob);

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

		class zCVobStair : public zCVob
		{
		public:

			DEFINE_CLASS(zCVobStair, zCVob);

			zCVobStair()			{ }
			virtual ~zCVobStair()	{ }

		private:

		};

		class zCVobStartpoint : public zCVob
		{
		public:

			DEFINE_CLASS(zCVobStartpoint, zCVob);

			zCVobStartpoint()			{ }
			virtual ~zCVobStartpoint()	{ }

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

		class zCVisualAnimate : public zCVisual
		{
		public:

			static inline ClassDefinition* classDef = nullptr;

			zCVisualAnimate()			{ }
			virtual ~zCVisualAnimate()	{ }

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
		
		class zCMorphMesh : public zCVisualAnimate
		{
		public:

			DEFINE_CLASS(zCMorphMesh, zCVisualAnimate);

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

			DEFINE_CLASS(zCDecal, zCVisual);

			zCDecal()				{ }
			virtual ~zCDecal()		{ }

			virtual bool Archive(zCArchiver*);
			virtual bool Unarchive(zCArchiver*);

#ifdef GOTHICLIB_ZENGIN_OLD_SAVE_LOAD
			virtual void Save(FileStream*);
			virtual void Load(FileStream*);
#endif

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

		/*
			Waynet
		*/

		class zCWaypoint : public zCObject
		{
		public:

			DEFINE_CLASS(zCWaypoint, zCObject);

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

			DEFINE_CLASS(zCWayNet, zCObject);

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