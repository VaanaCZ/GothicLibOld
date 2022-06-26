#pragma once

#include "zen_base.h"

namespace ZEN
{
	class zCVob;
	class zCVisual;
	class zCAIBase;

	/*
		World classes
	*/

	class zCWorld : public zCObject
	{
		DEFINE_CLASS(zCWorld);

	public:
		
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
		DEFINE_CLASS_EXTENDS(oCWorld, zCWorld);

	public:

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
		DEFINE_CLASS(zCVob);

	public:
		
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
		DEFINE_CLASS_EXTENDS(zCVobLevelCompo, zCVob);

	public:
		
		zCVobLevelCompo()			{ }
		virtual ~zCVobLevelCompo()	{ }

		virtual bool Archive(zCArchiver*);
		virtual bool Unarchive(zCArchiver*);

		std::vector<zCVob*> children;

	private:

	};

	/*
		Visual classes
	*/

	class zCVisual : public zCObject
	{
		DEFINE_CLASS(zCVisual);

	public:
		
		zCVisual()			{ }
		virtual ~zCVisual()	{ }

		virtual bool Archive(zCArchiver*);
		virtual bool Unarchive(zCArchiver*);

	private:

	};

	class zCMesh : public zCVisual
	{
		DEFINE_CLASS(zCMesh);

	public:
		
		zCMesh()			{ }
		virtual ~zCMesh()	{ }

		virtual bool Archive(zCArchiver*);
		virtual bool Unarchive(zCArchiver*);

	private:

	};

	/*
		AI classes
	*/

	class zCAIBase : public zCObject
	{
		DEFINE_CLASS(zCAIBase);

	public:
		
		zCAIBase()			{ }
		virtual ~zCAIBase()	{ }

		virtual bool Archive(zCArchiver*);
		virtual bool Unarchive(zCArchiver*);

	private:

	};
};