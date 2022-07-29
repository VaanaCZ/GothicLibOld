#include "zen_world.h"

using namespace GothicLib::ZenGin;

/*
	World classes
*/

bool zCWorld::LoadWorld(FileStream* file)
{
	if (game == GAME_DEMO3)
	{
		if (!LoadWorldFile(file))
		{
			return false;
		}
	}
	else if (game >= GAME_DEMO5)
	{
		zCArchiver archiver;
		archiver.game = game;
		if (!archiver.Read(file))
		{
			return false;
		}

		if (!archiver.ReadObject(this))
		{
			return false;
		}

		archiver.Close();
	}
	else
	{
		LOG_ERROR("Game not specified. Please specify a game to use before loading a world!");
		return false;
	}
}

bool zCWorld::SaveWorld(FileStream* file)
{
	if (game == GAME_DEMO3)
	{
		if (!SaveWorldFile(file))
		{
			return false;
		}
	}
	else if (game >= GAME_DEMO5)
	{
		zCArchiver archiver;
		archiver.game = game;
		if (!archiver.Write(file))
		{
			return false;
		}

		if (!archiver.WriteObject(this))
		{
			return false;
		}

		archiver.Close();
	}
	else
	{
		LOG_ERROR("Game not specified. Please specify a game to use before loading a world!");
		return false;
	}
}

bool zCWorld::Archive(zCArchiver* archiver)
{
	if (!zCObject::Archive(archiver))
		return false;

	// VobTree
	if (vobTree)
	{
		size_t vobCount = 0;

		archiver->WriteChunkStart("VobTree");
		if (!ArcVobTree(archiver, vobTree, vobCount))
		{
			return false;
		}
		archiver->WriteChunkEnd();
	}

	// WayNet
	if (wayNet && game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteChunkStart("WayNet");
		archiver->WriteObject(wayNet);
		archiver->WriteChunkEnd();
	}

	// EndMarker
	archiver->WriteChunkStart("EndMarker");
	archiver->WriteChunkEnd();

	// old WayNet
	if (wayNet && game <= GAME_DEMO5)
	{
		archiver->WriteObject(wayNet);
	}

	return true;
}

bool zCWorld::Unarchive(zCArchiver* archiver)
{
	if (!zCObject::Unarchive(archiver))
		return false;

	// Loop through chunks until the end
	bool loop = true;

	while (loop)
	{
		std::string objectName;

		archiver->ReadChunkStart(&objectName, nullptr, nullptr, nullptr);

		if (objectName == "MeshAndBsp")
		{
			bsp = new zCBspTree();
			bsp->game = game;
			
			if (!bsp->LoadBIN(archiver->GetFile()))
			{
				//return false;
			}
		}
		else if (objectName == "VobTree")
		{
			size_t vobCount = 0;
			vobTree = new zCVob();
			vobTree->game = game;

			if (!UnarcVobTree(archiver, vobTree, vobCount))
			{
				return false;
			}
		}
		else if (objectName == "WayNet")
		{
			wayNet = archiver->ReadObjectAs<zCWayNet*>();
			wayNet->game = game;

			if (!wayNet)
			{
				return false;
			}
		}
		else if (objectName == "EndMarker")
		{
			loop = false;
		}
		else
		{
			LOG_ERROR("Unknown chunk \"" + objectName + "\" encountered in oCWorld!");
			return false;
		}

		archiver->ReadChunkEnd();
	}

	if (!wayNet && game <= GAME_DEMO5)
	{
		wayNet = archiver->ReadObjectAs<zCWayNet*>();
		wayNet->game = game;

		if (!wayNet)
		{
			return false;
		}
	}

	return true;
}

bool zCWorld::ArcVobTree(zCArchiver* archiver, zCVob* vob, size_t& vobCount)
{
	// Write vobs
	if (vobCount != 0)
	{
		archiver->WriteObject(vob);
	}
	
	// Write childs
	int childCount = vob->children.size();
	if (!archiver->WriteInt("childs" + std::to_string(vobCount), childCount))
		return false;

	vobCount++;

	for (size_t i = 0; i < childCount; i++)
	{
		if (!ArcVobTree(archiver, vob->children[i], vobCount))
			return false;
	}

	return true;
}

bool zCWorld::UnarcVobTree(zCArchiver* archiver, zCVob* vob, size_t& vobCount)
{
	// Read vob
	zCVob* nextVob;

	if (vobCount != 0)
	{
		nextVob = archiver->ReadObjectAs<zCVob*>();

		if (!nextVob)
			return false;

		vob->children.push_back(nextVob);
	}
	else
	{
		nextVob = vob;
	}
	
	// Read childs
	int childCount;
	if (!archiver->ReadInt("childs" + std::to_string(vobCount), childCount))
		return false;

	vobCount++;

	for (size_t i = 0; i < childCount; i++)
	{
		if (!UnarcVobTree(archiver, nextVob, vobCount))
			return false;
	}

	return true;
}

bool zCWorld::LoadWorldFile(FileStream* file)
{
	std::string line;

	while (file->ReadLine(line))
	{
		// Check if we entered the right chunk
		if (line.find("VobHierarchy") == 0)
		{
			vobTree = new zCVob();
			vobTree->game = game;

			if (!LoadVobTree(file, vobTree))
			{
				return false;
			}
		}
		else if (line.find("Waynet") == 0)
		{
			wayNet = new zCWayNet();
			wayNet->game = game;

			if (!wayNet->LoadWaynet(file))
			{
				return false;
			}
		}
	}

	return true;
}

bool zCWorld::SaveWorldFile(FileStream* file)
{
	return false;
}

bool zCWorld::LoadVobTree(FileStream* file, zCVob* parentVob)
{	
	bool inVob = false;
	zCVob* vob = parentVob;

	std::string line;

	while (file->ReadLine(line))
	{
		std::string key;
		std::string value;

		bool chunkStart = ParseFileLine(line, key, value);

		if (chunkStart && key == "zCVob")
		{
			int vobId = std::stoi(value);
			zTVobType vobType = (zTVobType)std::stoi(value.substr(value.find(" ") + 1));

			switch (vobType)
			{
			case zVOB_TYPE_NORMAL:			vob = new zCVob();				break;
			case zVOB_TYPE_LIGHT:			vob = new zCVobLight();			break;
			case zVOB_TYPE_SOUND:			vob = new zCVobSound();			break;
			case zVOB_TYPE_LEVEL_COMPONENT:	vob = new zCVobLevelCompo();	break;
			case zVOB_TYPE_SPOT:			vob = new zCVobSpot();			break;
			//case zVOB_TYPE_CAMERA:			vob = new zCVob();				break;
			case zVOB_TYPE_STARTPOINT:		vob = new zCVobStartpoint();	break;
			case zVOB_TYPE_WAYPOINT:		vob = new zCVobWaypoint();		break;
			case zVOB_TYPE_MARKER:			vob = new zCVobMarker();		break;
			case zVOB_TYPE_MOB:				vob = new oCMob();				break;
			case zVOB_TYPE_ITEM:			vob = new oCItem();				break;
			case zVOB_TYPE_NSC:				vob = new oCNpc();				break;
			default:
				break;
			}

			inVob = true;

			vob->vobID = vobId;

			parentVob->children.push_back(vob);
		}
		else if (inVob)
		{
			if (line == "{")
			{
				if (!vob->Load(file))
				{
					return false;
				}
			}
			else if (line == "}")
			{
				inVob = false;
			}
			else if (ParseFileLine(line, key, value))
			{
				if (key == "zCDecal")
				{
					vob->visualPtr = new zCDecal();
					((zCDecal*)vob->visualPtr)->name = vob->visual;
					vob->visualPtr->Load(file);
				}
				else
				{
					// error
				}
			}
		}
		else if (chunkStart && key == "childs")
		{
			int childCount = std::stoi(value);

			for (size_t i = 0; i < childCount; i++)
			{
				if (!LoadVobTree(file, vob))
				{
					return false;
				}
			}

			return true;
		}
	}
}

bool zCWorld::SaveVobTree(FileStream* file, zCVob* parentVob)
{
	return false;
}

bool oCWorld::Archive(zCArchiver* archiver)
{
	if (!zCWorld::Archive(archiver))
		return false;

	//todo: savegame
	
	return true;
}

bool oCWorld::Unarchive(zCArchiver* archiver)
{
	if (!zCWorld::Unarchive(archiver))
		return false;

	//todo: savegame

	return true;
}

/*
	BSP
*/

bool zCBspTree::SaveBIN(FileStream* file)
{
	return false;
}

bool zCBspTree::LoadBIN(FileStream* file)
{
	// Read start
	uint32_t version, length;

	if (!file->Read(&version, sizeof(version)) ||
		!file->Read(&length, sizeof(length)))
	{
		LOG_ERROR("Unexpected end of file when reading MeshAndBsp chunk");
		return false;
	}

	file->Seek(file->Tell() + length); // temp
	return true; // temp

	// Mesh
	if (!mesh.LoadMSH(file))
	{
		return false;
	}

	return false;
}

/*
	Vob classes
*/

/*
	zCVob
		oCVob
*/

bool zCVob::Archive(zCArchiver* archiver)
{
	if (!zCObject::Archive(archiver))
		return false;

	archiver->WriteGroupBegin("Internals");

	int	pack = 0;

	// Only 1.06l and up use pack
	if (game >= GAME_GOTHIC1)
	{
		archiver->WriteInt(ARC_ARGS(pack));
	}

	if (pack)
	{
		return true;
	}
	else
	{
		if (game <= GAME_SEPTEMBERDEMO)
		{
			archiver->WriteInt(ARC_ARGS(vobID));
		}

		archiver->WriteString(ARC_ARGS(presetName));

		if (game <= GAME_SEPTEMBERDEMO)
		{
			archiver->WriteBool(ARC_ARGS(drawBBox3D));
		}

		archiver->WriteRawFloat(ARC_ARGSF(bbox3DWS));

		if (game <= GAME_SEPTEMBERDEMO)
		{
			archiver->WriteRaw(ARC_ARGSR(trafoRot));
			archiver->WriteVec3(ARC_ARGS(trafoPos));
		}

		archiver->WriteRaw(ARC_ARGSR(trafoOSToWSRot));
		archiver->WriteVec3(ARC_ARGS(trafoOSToWSPos));

		archiver->WriteGroupEnd("Internals");

		archiver->WriteGroupBegin("Vob");

		archiver->WriteString(ARC_ARGS(vobName));
		archiver->WriteString(ARC_ARGS(visual));
		archiver->WriteBool(ARC_ARGS(showVisual));
		archiver->WriteEnum(ARC_ARGSEW(visualCamAlign, "NONE;YAW;FULL"));

		if (game >= GAME_GOTHIC2)
		{
			archiver->WriteEnum(ARC_ARGSEW(visualAniMode, "NONE;WIND;WIND2"));
			archiver->WriteFloat(ARC_ARGS(visualAniModeStrength));
			archiver->WriteFloat(ARC_ARGS(vobFarClipZScale));
		}

		archiver->WriteBool(ARC_ARGS(cdStatic));
		archiver->WriteBool(ARC_ARGS(cdDyn));
		archiver->WriteBool(ARC_ARGS(staticVob));

		if (game >= GAME_SEPTEMBERDEMO)
		{
			archiver->WriteEnum(ARC_ARGSEW(dynShadow, "DS_NONE;DS_BLOB"));
		}

		if (game >= GAME_GOTHIC2)
		{
			archiver->WriteInt(ARC_ARGS(zbias));
			archiver->WriteBool(ARC_ARGS(isAmbient));
		}

		archiver->WriteGroupEnd("Vob");

		archiver->WriteObject("visual", visualPtr);
		archiver->WriteObject("ai", aiPtr);

		if (archiver->IsSavegame())
		{
			archiver->WriteBool(ARC_ARGS(physicsEnabled));
		}
	}

	return true;
}

bool zCVob::Unarchive(zCArchiver* archiver)
{
	if (!zCObject::Unarchive(archiver))
		return false;

	int	pack = 0;

	// Only 1.06l and up use pack
	if (game >= GAME_GOTHIC1)
	{
		archiver->ReadInt(ARC_ARGS(pack));
	}

	if (pack)
	{
		zSVobArcRawData dataRaw;
		archiver->ReadRaw(ARC_ARGSR(dataRaw));

		bbox3DWS		= dataRaw.bbox3DWS;
		trafoOSToWSRot	= dataRaw.trafoRotWS;
		trafoOSToWSPos	= dataRaw.positionWS;

		showVisual				= dataRaw.bitfield.showVisual;
		visualCamAlign			= (zTVisualCamAlign)dataRaw.bitfield.visualCamAlign;
		//visualAniMode			= dataRaw.bitfield.visualAniMode;
		//vobFarClipZScale		= dataRaw.bitfield.vobFarClipZScale;
		//visualAniModeStrength	= dataRaw.bitfield.visualAniModeStrength;
		cdStatic				= dataRaw.bitfield.cdStatic;
		cdDyn					= dataRaw.bitfield.cdDyn;
		staticVob				= dataRaw.bitfield.staticVob;
		dynShadow				= (zTDynShadowType)dataRaw.bitfield.dynShadow;
		//zbias					= dataRaw.bitfield.zbias;
		//isAmbient				= dataRaw.bitfield.isAmbient;

		if (dataRaw.bitfield.hasPresetName)
		{
			archiver->ReadString(ARC_ARGS(presetName));
		}

		if (dataRaw.bitfield.hasVobName)
		{
			archiver->ReadString(ARC_ARGS(vobName));
		}

		if (dataRaw.bitfield.hasVisualName)
		{
			archiver->ReadString(ARC_ARGS(visual));
		}

		if (dataRaw.bitfield.hasRelevantVisualObject)
		{
			visualPtr = archiver->ReadObjectAs<zCVisual*>("visual");
		}

		if (dataRaw.bitfield.hasAIObject)
		{
			aiPtr = archiver->ReadObjectAs<zCAIBase*>("ai");
		}

		if (archiver->IsSavegame())
		{
			physicsEnabled = dataRaw.bitfield.physicsEnabled;

			if (dataRaw.bitfield.hasEventManObject)
			{
				eventManagerPtr = archiver->ReadObjectAs<zCEventManager*>("eventManager");
			}
		}

		return true;
	}
	else
	{
		if (game <= GAME_SEPTEMBERDEMO)
		{
			archiver->ReadInt(ARC_ARGS(vobID));
		}

		archiver->ReadString(ARC_ARGS(presetName));

		if (game <= GAME_SEPTEMBERDEMO)
		{
			archiver->ReadBool(ARC_ARGS(drawBBox3D));
		}

		archiver->ReadRawFloat(ARC_ARGSF(bbox3DWS));

		if (game <= GAME_SEPTEMBERDEMO)
		{
			archiver->ReadRaw(ARC_ARGSR(trafoRot));
			archiver->ReadVec3(ARC_ARGS(trafoPos));
		}

		archiver->ReadRaw(ARC_ARGSR(trafoOSToWSRot));
		archiver->ReadVec3(ARC_ARGS(trafoOSToWSPos));
		archiver->ReadString(ARC_ARGS(vobName));
		archiver->ReadString(ARC_ARGS(visual));
		archiver->ReadBool(ARC_ARGS(showVisual));
		archiver->ReadEnum(ARC_ARGSE(visualCamAlign));

		if (game >= GAME_GOTHIC2)
		{
			archiver->ReadEnum(ARC_ARGSE(visualAniMode));
			archiver->ReadFloat(ARC_ARGS(visualAniModeStrength));
			archiver->ReadFloat(ARC_ARGS(vobFarClipZScale));
		}

		archiver->ReadBool(ARC_ARGS(cdStatic));
		archiver->ReadBool(ARC_ARGS(cdDyn));
		archiver->ReadBool(ARC_ARGS(staticVob));

		if (game >= GAME_SEPTEMBERDEMO)
		{
			archiver->ReadEnum(ARC_ARGSE(dynShadow));
		}

		if (game >= GAME_GOTHIC2)
		{
			archiver->ReadInt(ARC_ARGS(zbias));
			archiver->ReadBool(ARC_ARGS(isAmbient));
		}

		visualPtr = archiver->ReadObjectAs<zCVisual*>("visual");
		aiPtr = archiver->ReadObjectAs<zCAIBase*>("ai");

		if (archiver->IsSavegame())
		{
			archiver->ReadBool(ARC_ARGS(physicsEnabled));
		}
	}

	if (archiver->IsSavegame())
	{
		unsigned char sleepMode;
		archiver->ReadByte(ARC_ARGS(sleepMode));
		this->sleepMode = (zTVobSleepingMode)sleepMode;
		archiver->ReadFloat(ARC_ARGS(nextOnTimer));
	}

	return true;
}

bool zCVob::Save(FileStream* file)
{
	return false;
}

bool zCVob::Load(FileStream* file)
{
	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return true;
			}
			else if (ParseFileLine(line, key, value))
			{
				if (key == "vobType")
				{
					vobType = (zTVobType)std::stoi(value);
				}
				else if (key == "visual")
				{
					visual = value;
				}
				else if (key == "vobName")
				{
					vobName = value;
				}
				else if (key == "visualCamAlign")
				{
					if (value == "NONE")		visualCamAlign = zVISUAL_CAMALIGN_NONE;
					else if (value == "YAW")	visualCamAlign = zVISUAL_CAMALIGN_YAW;
					else if (value == "FULL")	visualCamAlign = zVISUAL_CAMALIGN_FULL;
				}
				else if (key == "showVisual")
				{
					showVisual = (value == "1");
				}
				else if (key == "drawBBox3D")
				{
					drawBBox3D = (value == "1");
				}
				else if (key == "cdStatic")
				{
					cdStatic = (value == "1");
				}
				else if (key == "cdDyn")
				{
					cdDyn = (value == "1");
				}
				else if (key == "staticVob")
				{
					staticVob = (value == "1");
				}
				else if (key == "bbox3DWSpace")
				{
					if (sscanf_s(value.c_str(), "%f %f %f %f %f %f",
						&bbox3DWS.min.x, &bbox3DWS.min.y, &bbox3DWS.min.z,
						&bbox3DWS.max.x, &bbox3DWS.max.y, &bbox3DWS.max.z) != 6)
					{
						return false;
					}
				}
				else if (key == "trafoRot")
				{
					if (sscanf_s(value.c_str(), "%f %f %f %f %f %f %f %f %f",
						&trafoRot.x.x, &trafoRot.x.y, &trafoRot.x.z, 
						&trafoRot.y.x, &trafoRot.y.y, &trafoRot.y.z,
						&trafoRot.z.x, &trafoRot.z.y, &trafoRot.z.z) != 9)
					{
						return false;
					}
				}
				else if (key == "trafoPos")
				{
					if (sscanf_s(value.c_str(), "%f %f %f",
						&trafoPos.x, &trafoPos.y, &trafoPos.z) != 3)
					{
						return false;
					}
				}
				else if (key == "trafoOSToWSRot")
				{
					if (sscanf_s(value.c_str(), "%f %f %f %f %f %f %f %f %f",
						&trafoOSToWSRot.x.x, &trafoOSToWSRot.x.y, &trafoOSToWSRot.x.z, 
						&trafoOSToWSRot.y.x, &trafoOSToWSRot.y.y, &trafoOSToWSRot.y.z,
						&trafoOSToWSRot.z.x, &trafoOSToWSRot.z.y, &trafoOSToWSRot.z.z) != 9)
					{
						return false;
					}
				}
				else if (key == "trafoOSToWSPos")
				{
					if (sscanf_s(value.c_str(), "%f %f %f",
						&trafoOSToWSPos.x, &trafoOSToWSPos.y, &trafoOSToWSPos.z) != 3)
					{
						return false;
					}
				}
			}
		}
	}

	return false;
}

/*
	zCCSCamera
*/

bool zCCSCamera::Archive(zCArchiver* archiver)
{
	if (!zCVob::Archive(archiver))
		return false;

	return true;
}

bool zCCSCamera::Unarchive(zCArchiver* archiver)
{
	if (!zCVob::Unarchive(archiver))
		return false;

	archiver->ReadEnum(ARC_ARGSE(camTrjFOR));
	archiver->ReadEnum(ARC_ARGSE(targetTrjFOR));
	archiver->ReadEnum(ARC_ARGSE(loopMode));
	archiver->ReadEnum(ARC_ARGSE(splLerpMode));
	archiver->ReadBool(ARC_ARGS(ignoreFORVobRotCam));
	archiver->ReadBool(ARC_ARGS(ignoreFORVobRotTarget));
	archiver->ReadBool(ARC_ARGS(adaptToSurroundings));
	archiver->ReadBool(ARC_ARGS(easeToFirstKey));
	archiver->ReadBool(ARC_ARGS(easeFromLastKey));
	archiver->ReadFloat(ARC_ARGS(totalTime));
	archiver->ReadString(ARC_ARGS(autoCamFocusVobName));
	archiver->ReadBool(ARC_ARGS(autoCamPlayerMovable));
	archiver->ReadBool(ARC_ARGS(autoCamUntriggerOnLastKey));
	
	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->ReadFloat(ARC_ARGS(autoCamUntriggerOnLastKeyDelay));
	}

	if (!archiver->IsProperties())
	{
		int numPos, numTargets = 0;

		archiver->ReadInt(ARC_ARGS(numPos));
		archiver->ReadInt(ARC_ARGS(numTargets));

		if (numPos >= 0)
		{
			positions.resize(numPos);

			for (size_t i = 0; i < numPos; i++)
			{
				positions[i] = archiver->ReadObjectAs<zCCamTrj_KeyFrame*>();
			}
		}

		if (numTargets >= 0)
		{
			targets.resize(numTargets);

			for (size_t i = 0; i < numTargets; i++)
			{
				targets[i] = archiver->ReadObjectAs<zCCamTrj_KeyFrame*>();
			}
		}
	}

	if (archiver->IsSavegame())
	{
		archiver->ReadBool(ARC_ARGS(paused));
		archiver->ReadBool(ARC_ARGS(started));
		archiver->ReadBool(ARC_ARGS(gotoTimeMode));
		archiver->ReadFloat(ARC_ARGS(csTime));
	}

	return true;
}

/*
	zCCamTrj_KeyFrame
*/

bool zCCamTrj_KeyFrame::Archive(zCArchiver* archiver)
{
	if (!zCVob::Archive(archiver))
		return false;

	return true;
}

bool zCCamTrj_KeyFrame::Unarchive(zCArchiver* archiver)
{
	if (!zCVob::Unarchive(archiver))
		return false;

	archiver->ReadFloat(ARC_ARGS(time));
	archiver->ReadFloat(ARC_ARGS(angleRollDeg));
	archiver->ReadFloat(ARC_ARGS(camFOVScale));
	archiver->ReadEnum(ARC_ARGSE(motionType));
	archiver->ReadEnum(ARC_ARGSE(motionTypeFOV));
	archiver->ReadEnum(ARC_ARGSE(motionTypeRoll));

	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->ReadEnum(ARC_ARGSE(motionTypeTimeScale));
	}

	archiver->ReadFloat(ARC_ARGS(tension));
	archiver->ReadFloat(ARC_ARGS(bias));
	archiver->ReadFloat(ARC_ARGS(continuity));

	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->ReadFloat(ARC_ARGS(timeScale));
	}

	archiver->ReadBool(ARC_ARGS(timeIsFixed));

	if (!archiver->IsProperties())
	{
		archiver->ReadRaw(ARC_ARGSR(originalPose));
	}

	return true;
}


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

bool zCEarthquake::Archive(zCArchiver* archiver)
{
	if (!zCEffect::Archive(archiver))
		return false;

	return true;
}

bool zCEarthquake::Unarchive(zCArchiver* archiver)
{
	if (!zCEffect::Unarchive(archiver))
		return false;

	archiver->ReadFloat(ARC_ARGS(radius));
	archiver->ReadFloat(ARC_ARGS(timeSec));
	archiver->ReadVec3(ARC_ARGS(amplitudeCM));

	return true;
}

bool zCPFXControler::Archive(zCArchiver* archiver)
{
	if (!zCEffect::Archive(archiver))
		return false;

	return true;
}

bool zCPFXControler::Unarchive(zCArchiver* archiver)
{
	if (!zCEffect::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(pfxName));
	archiver->ReadBool(ARC_ARGS(killVobWhenDone));
	archiver->ReadBool(ARC_ARGS(pfxStartOn));

	return true;
}

bool zCVobAnimate::Archive(zCArchiver* archiver)
{
	if (!zCEffect::Archive(archiver))
		return false;

	archiver->WriteBool(ARC_ARGS(startOn));

	if (archiver->IsSavegame())
	{
		archiver->WriteBool(ARC_ARGS(isRunning));
	}

	return true;
}

bool zCVobAnimate::Unarchive(zCArchiver* archiver)
{
	if (!zCEffect::Unarchive(archiver))
		return false;

	archiver->ReadBool(ARC_ARGS(startOn));

	if (archiver->IsSavegame())
	{
		archiver->ReadBool(ARC_ARGS(isRunning));
	}
	else
	{
		isRunning = startOn;
	}

	return true;
}

bool zCTouchDamage::Archive(zCArchiver* archiver)
{
	if (!zCEffect::Archive(archiver))
		return false;

	return true;
}

bool zCTouchDamage::Unarchive(zCArchiver* archiver)
{
	if (!zCEffect::Unarchive(archiver))
		return false;

	archiver->ReadFloat(ARC_ARGS(damage));
	archiver->ReadBool(ARC_ARGS(Barrier));
	archiver->ReadBool(ARC_ARGS(Blunt));
	archiver->ReadBool(ARC_ARGS(Edge));
	archiver->ReadBool(ARC_ARGS(Fire));
	archiver->ReadBool(ARC_ARGS(Fly));
	archiver->ReadBool(ARC_ARGS(Magic));
	archiver->ReadBool(ARC_ARGS(Point));
	archiver->ReadBool(ARC_ARGS(Fall));
	archiver->ReadFloat(ARC_ARGS(damageRepeatDelaySec));
	archiver->ReadFloat(ARC_ARGS(damageVolDownScale));
	archiver->ReadEnum(ARC_ARGSE(damageCollType));

	return true;
}

bool zCVobLensFlare::Archive(zCArchiver* archiver)
{
	if (!zCEffect::Archive(archiver))
		return false;

	archiver->WriteString(ARC_ARGS(lensflareFX));

	return true;
}

bool zCVobLensFlare::Unarchive(zCArchiver* archiver)
{
	if (!zCEffect::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(lensflareFX));

	return true;
}

bool zCVobScreenFX::Archive(zCArchiver* archiver)
{
	if (!zCEffect::Archive(archiver))
		return false;

	return true;
}

bool zCVobScreenFX::Unarchive(zCArchiver* archiver)
{
	if (!zCEffect::Unarchive(archiver))
		return false;

	if (archiver->IsSavegame())
	{
		// todo
		//ZCR_READRAW(blend);
		//ZCR_READRAW(cinema);
		//ZCR_READRAW(fovMorph);
		//ZCR_READRAW(fovSaved);
		//ZCR_READRAW(fovSaved1st);
	}

	return true;
}

/*
	oCItem
*/

bool oCItem::Archive(zCArchiver* archiver)
{
	if (!oCVob::Archive(archiver))
		return false;

	archiver->WriteString(ARC_ARGS(itemInstance));

	if (archiver->IsSavegame())
	{
		archiver->WriteInt(ARC_ARGS(amount));
		archiver->WriteInt(ARC_ARGS(flags));
	}

	return true;
}

bool oCItem::Unarchive(zCArchiver* archiver)
{
	if (!oCVob::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(itemInstance));

	if (archiver->IsSavegame())
	{
		archiver->ReadInt(ARC_ARGS(amount));
		archiver->ReadInt(ARC_ARGS(flags));
	}

	return true;
}

bool oCItem::Save(FileStream* file)
{
	if (!zCVob::Save(file))
		return false;

	return true;
}

bool oCItem::Load(FileStream* file)
{
	if (!zCVob::Load(file))
		return false;

	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return true;
			}
			else if (ParseFileLine(line, key, value))
			{
				if (key == "INSTANCE")
				{
					itemInstance = value;
				}
			}
		}
	}

	return false;
}

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

bool oCMOB::Archive(zCArchiver* archiver)
{
	if (!oCVob::Archive(archiver))
		return false;

	archiver->WriteGroupBegin("MOB");

	archiver->WriteString(ARC_ARGS(focusName));
	archiver->WriteInt(ARC_ARGS(hitpoints));

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteInt(ARC_ARGS(damage));
	}

	archiver->WriteBool(ARC_ARGS(moveable));
	archiver->WriteBool(ARC_ARGS(takeable));

	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->WriteBool(ARC_ARGS(focusOverride));
	}

	archiver->WriteEnum(ARC_ARGSEW(soundMaterial, "WOOD;STONE;METAL;LEATHER;CLAY;GLAS"));
	archiver->WriteString(ARC_ARGS(visualDestroyed));
	archiver->WriteString(ARC_ARGS(owner));
	archiver->WriteString(ARC_ARGS(ownerGuild));

	archiver->WriteGroupEnd("MOB");

	if (!archiver->IsProperties())
	{
		archiver->WriteBool(ARC_ARGS(isDestroyed));
	}

	return true;
}

bool oCMOB::Unarchive(zCArchiver* archiver)
{
	if (!oCVob::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(focusName));
	archiver->ReadInt(ARC_ARGS(hitpoints));

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadInt(ARC_ARGS(damage));
	}

	archiver->ReadBool(ARC_ARGS(moveable));
	archiver->ReadBool(ARC_ARGS(takeable));

	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->ReadBool(ARC_ARGS(focusOverride));
	}

	archiver->ReadEnum(ARC_ARGSE(soundMaterial));
	archiver->ReadString(ARC_ARGS(visualDestroyed));
	archiver->ReadString(ARC_ARGS(owner));
	archiver->ReadString(ARC_ARGS(ownerGuild));

	if (!archiver->IsProperties())
	{
		archiver->ReadBool(ARC_ARGS(isDestroyed));
	}

	return true;
}

bool oCMobInter::Archive(zCArchiver* archiver)
{
	if (!oCMOB::Archive(archiver))
		return false;

	if (!archiver->IsProperties())
	{
		if (game <= GAME_CHRISTMASEDITION)
		{
			archiver->WriteInt(ARC_ARGS(state));
			archiver->WriteInt(ARC_ARGS(stateTarget));
		}

		archiver->WriteInt(ARC_ARGS(stateNum));

		if (game <= GAME_DEMO5)
		{
			archiver->WriteBool(ARC_ARGS(interact));
		}
	}

	archiver->WriteString(ARC_ARGS(triggerTarget));

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteString(ARC_ARGS(useWithItem));
		archiver->WriteString(ARC_ARGS(conditionFunc));
		archiver->WriteString(ARC_ARGS(onStateFunc));
		archiver->WriteBool(ARC_ARGS(rewind));
	}

	return true;
}

bool oCMobInter::Unarchive(zCArchiver* archiver)
{
	if (!oCMOB::Unarchive(archiver))
		return false;

	if (!archiver->IsProperties())
	{
		if (game <= GAME_CHRISTMASEDITION)
		{
			archiver->ReadInt(ARC_ARGS(state));
			archiver->ReadInt(ARC_ARGS(stateTarget));
		}

		archiver->ReadInt(ARC_ARGS(stateNum));

		if (game <= GAME_DEMO5)
		{
			archiver->ReadBool(ARC_ARGS(interact));
		}
	}

	archiver->ReadString(ARC_ARGS(triggerTarget));

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadString(ARC_ARGS(useWithItem));
		archiver->ReadString(ARC_ARGS(conditionFunc));
		archiver->ReadString(ARC_ARGS(onStateFunc));
		archiver->ReadBool(ARC_ARGS(rewind));
	}

	return true;
}

bool oCMobFire::Archive(zCArchiver* archiver)
{
	if (!oCMobInter::Archive(archiver))
		return false;

	archiver->WriteGroupBegin("Fire");

	archiver->WriteString(ARC_ARGS(fireSlot));
	archiver->WriteString(ARC_ARGS(fireVobtreeName));

	archiver->WriteGroupEnd("Fire");

	return true;
}

bool oCMobFire::Unarchive(zCArchiver* archiver)
{
	if (!oCMobInter::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(fireSlot));
	archiver->ReadString(ARC_ARGS(fireVobtreeName));

	return true;
}

bool oCMobLockable::Archive(zCArchiver* archiver)
{
	if (!oCMobInter::Archive(archiver))
		return false;
	
	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->WriteGroupBegin("Lockable");

		archiver->WriteBool(ARC_ARGS(locked));
		archiver->WriteString(ARC_ARGS(keyInstance));
		archiver->WriteString(ARC_ARGS(pickLockStr));

		archiver->WriteGroupEnd("Lockable");
	}

	return true;
}

bool oCMobLockable::Unarchive(zCArchiver* archiver)
{
	if (!oCMobInter::Unarchive(archiver))
		return false;

	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->ReadBool(ARC_ARGS(locked));
		archiver->ReadString(ARC_ARGS(keyInstance));
		archiver->ReadString(ARC_ARGS(pickLockStr));
	}

	return true;
}

bool oCMobContainer::Archive(zCArchiver* archiver)
{
	if (!oCMobLockable::Archive(archiver))
		return false;

	archiver->WriteGroupBegin("Container");

	archiver->WriteString(ARC_ARGS(contains));

	if (game <= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteBool(ARC_ARGS(locked));
		archiver->WriteString(ARC_ARGS(keyInstance));

		if (game == GAME_SEPTEMBERDEMO)
		{
			archiver->WriteString(ARC_ARGS(pickLockStr));
		}
	}

	archiver->WriteGroupEnd("Container");

	return true;
}

bool oCMobContainer::Unarchive(zCArchiver* archiver)
{
	if (!oCMobLockable::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(contains));

	if (game <= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadBool(ARC_ARGS(locked));
		archiver->ReadString(ARC_ARGS(keyInstance));

		if (game == GAME_SEPTEMBERDEMO)
		{
			archiver->ReadString(ARC_ARGS(pickLockStr));
		}
	}

	return true;
}

/*
	oCMob

	Legacy class, not to be confused with oCMOB and its derivatives.
	Only included to support demo5 loading.
*/

bool oCMob::Archive(zCArchiver* archiver)
{
	if (!oCVob::Archive(archiver))
		return false;

	return true;
}

bool oCMob::Unarchive(zCArchiver* archiver)
{
	if (!oCVob::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(mobInstance));

	return true;
}

bool oCMob::Save(FileStream* file)
{
	if (!zCVob::Save(file))
		return false;

	return false;
}

bool oCMob::Load(FileStream* file)
{
	if (!zCVob::Load(file))
		return false;

	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return true;
			}
			else if (ParseFileLine(line, key, value))
			{
				if (key == "INSTANCE")
				{
					mobInstance = value;
				}
			}
		}
	}

	return false;
}

/*
	oCNpc
*/

bool oCNpc::Archive(zCArchiver* archiver)
{
	if (!oCVob::Archive(archiver))
		return false;

	return true;
}

bool oCNpc::Unarchive(zCArchiver* archiver)
{
	if (!oCVob::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(npcInstance));

	if (archiver->IsSavegame())
	{
		archiver->ReadVec3(ARC_ARGS(modelScale));
		archiver->ReadFloat(ARC_ARGS(modelFatness));

		int numOverlays = 0;
		archiver->ReadInt(ARC_ARGS(numOverlays));

		for (size_t i = 0; i < numOverlays; i++)
		{
			break;
		}

		archiver->ReadInt(ARC_ARGS(flags));
		archiver->ReadInt(ARC_ARGS(guild));
		archiver->ReadInt(ARC_ARGS(guildTrue));
		archiver->ReadInt(ARC_ARGS(guildTrue));
		archiver->ReadInt(ARC_ARGS(level));
		archiver->ReadInt(ARC_ARGS(xp));
		archiver->ReadInt(ARC_ARGS(xpnl));
		archiver->ReadInt(ARC_ARGS(lp));

	}

	return true;
}

bool oCNpc::Save(FileStream* file)
{
	if (!zCVob::Save(file))
		return false;

	return false;
}

bool oCNpc::Load(FileStream* file)
{
	if (!zCVob::Load(file))
		return false;

	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return true;
			}
			else if (ParseFileLine(line, key, value))
			{
				if (key == "INSTANCE")
				{
					npcInstance = value;
				}
			}
		}
	}

	return false;
}

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

bool zCTriggerBase::Archive(zCArchiver* archiver)
{
	if (!zCVob::Archive(archiver))
		return false;

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteString(ARC_ARGS(triggerTarget));
	}

	return true;
}

bool zCTriggerBase::Unarchive(zCArchiver* archiver)
{
	if (!zCVob::Unarchive(archiver))
		return false;

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadString(ARC_ARGS(triggerTarget));
	}

	return true;
}

bool zCCodeMaster::Archive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Archive(archiver))
		return false;

	return true;
}

bool zCCodeMaster::Unarchive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Unarchive(archiver))
		return false;

	archiver->ReadBool(ARC_ARGS(orderRelevant));
	archiver->ReadBool(ARC_ARGS(firstFalseIsFailure));
	archiver->ReadString(ARC_ARGS(triggerTargetFailure));
	archiver->ReadBool(ARC_ARGS(untriggerCancels));

	if (!archiver->IsProperties())
	{
		unsigned char numSlaves = 6;
		archiver->ReadByte(ARC_ARGS(numSlaves));

		slaveVobNameList.resize(numSlaves);

		for (size_t i = 0; i < numSlaves; i++)
		{
			std::string slaveVobNameKey = "slaveVobName" + std::to_string(i);
			std::string slaveVobName;
			archiver->ReadString(slaveVobNameKey, slaveVobName);

			slaveVobNameList[i] = slaveVobName;
		}
	}

	if (archiver->IsSavegame())
	{
		unsigned char numSlavesTriggered = 0;
		archiver->ReadByte(ARC_ARGS(numSlavesTriggered));

		slaveTriggeredList.resize(numSlavesTriggered);

		for (size_t i = 0; i < numSlavesTriggered; i++)
		{
			std::string slaveTriggeredKey = "slaveTriggered" + std::to_string(i);
			slaveTriggeredList[i] = archiver->ReadObjectAs<zCVob*>(slaveTriggeredKey);
		}
	}

	return true;
}

bool zCMessageFilter::Archive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Archive(archiver))
		return false;

	return true;
}

bool zCMessageFilter::Unarchive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Unarchive(archiver))
		return false;

	archiver->ReadEnum(ARC_ARGSE(onTrigger));
	archiver->ReadEnum(ARC_ARGSE(onUntrigger));

	return true;
}

bool zCMoverControler::Archive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Archive(archiver))
		return false;

	return true;
}

bool zCMoverControler::Unarchive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Unarchive(archiver))
		return false;

	archiver->ReadEnum(ARC_ARGSE(moverMessage));
	archiver->ReadInt(ARC_ARGS(gotoFixedKey));

	return true;
}

bool zCTrigger::Archive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Archive(archiver))
		return false;

	archiver->WriteGroupBegin("Trigger");

	archiver->WriteGroupBegin("ActivationFilter");

	if (!archiver->IsProperties())
	{
		if (game >= GAME_SEPTEMBERDEMO)
		{
			archiver->WriteRaw(ARC_ARGSR(flags));
		}

		archiver->WriteRaw(ARC_ARGSR(filterFlags));
	}
	else
	{
		bool reactToOnTrigger = filterFlags.reactToOnTrigger;
		archiver->WriteBool(ARC_ARGS(reactToOnTrigger));

		bool reactToOnTouch = filterFlags.reactToOnTouch;
		archiver->WriteBool(ARC_ARGS(reactToOnTouch));

		bool reactToOnDamage = filterFlags.reactToOnDamage;
		archiver->WriteBool(ARC_ARGS(reactToOnDamage));

		bool respondToObject = filterFlags.respondToObject;
		archiver->WriteBool(ARC_ARGS(respondToObject));

		bool respondToPC = filterFlags.respondToPC;
		archiver->WriteBool(ARC_ARGS(respondToPC));

		bool respondToNPC = filterFlags.respondToNPC;
		archiver->WriteBool(ARC_ARGS(respondToNPC));

		bool startEnabled = flags.startEnabled;
		archiver->WriteBool(ARC_ARGS(startEnabled));
	}

	archiver->WriteString(ARC_ARGS(respondToVobName));

	if (game <= GAME_DEMO5)
	{
		archiver->WriteInt(ARC_ARGS(numTriggerToActivate));
	}

	archiver->WriteInt(ARC_ARGS(numCanBeActivated));
	archiver->WriteFloat(ARC_ARGS(retriggerWaitSec));
	archiver->WriteFloat(ARC_ARGS(damageThreshold));

	archiver->WriteGroupEnd("ActivationFilter");

	archiver->WriteGroupBegin("FireBehavior");

	if (game <= GAME_DEMO5)
	{
		archiver->WriteString(ARC_ARGS(triggerTarget));
	}

	archiver->WriteFloat(ARC_ARGS(fireDelaySec));

	if (game <= GAME_DEMO5)
	{
		archiver->WriteEnum(ARC_ARGSEW(repeatTrigger, "RT_NONE;RT_REPEAT;RT_REPEAT_TOUCHING"));
	}

	if (archiver->IsProperties())
	{
		archiver->WriteBool(ARC_ARGS(sendUntrigger));
	}

	archiver->WriteGroupEnd("FireBehavior");

	if (archiver->IsSavegame())
	{
		archiver->WriteFloat(ARC_ARGS(nextTimeTriggerable));
		archiver->WriteObject("savedOtherVob", savedOtherVobPtr);
		archiver->WriteInt(ARC_ARGS(countCanBeActivated));

		bool isEnabled = flags.isEnabled;
		archiver->WriteBool(ARC_ARGS(isEnabled));
	}

	archiver->WriteGroupEnd("Trigger");

	return true;
}

bool zCTrigger::Unarchive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Unarchive(archiver))
		return false;

	if (!archiver->IsProperties())
	{
		if (game >= GAME_SEPTEMBERDEMO)
		{
			archiver->ReadRaw(ARC_ARGSR(flags));
		}

		archiver->ReadRaw(ARC_ARGSR(filterFlags));
	}
	else
	{
		bool reactToOnTrigger;
		archiver->ReadBool(ARC_ARGS(reactToOnTrigger));
		filterFlags.reactToOnTrigger = reactToOnTrigger;

		bool reactToOnTouch;
		archiver->ReadBool(ARC_ARGS(reactToOnTouch));
		filterFlags.reactToOnTouch = reactToOnTouch;

		bool reactToOnDamage;
		archiver->ReadBool(ARC_ARGS(reactToOnDamage));
		filterFlags.reactToOnDamage = reactToOnDamage;

		bool respondToObject;
		archiver->ReadBool(ARC_ARGS(respondToObject));
		filterFlags.respondToObject = respondToObject;

		bool respondToPC;
		archiver->ReadBool(ARC_ARGS(respondToPC));
		filterFlags.respondToPC = respondToPC;

		bool respondToNPC;
		archiver->ReadBool(ARC_ARGS(respondToNPC));
		filterFlags.respondToNPC = respondToNPC;

		bool startEnabled;
		archiver->ReadBool(ARC_ARGS(startEnabled));
		flags.startEnabled = startEnabled;
	}

	archiver->ReadString(ARC_ARGS(respondToVobName));

	if (game <= GAME_DEMO5)
	{
		archiver->ReadInt(ARC_ARGS(numTriggerToActivate));
	}

	archiver->ReadInt(ARC_ARGS(numCanBeActivated));
	archiver->ReadFloat(ARC_ARGS(retriggerWaitSec));
	archiver->ReadFloat(ARC_ARGS(damageThreshold));

	if (game <= GAME_DEMO5)
	{
		archiver->ReadString(ARC_ARGS(triggerTarget));
	}

	archiver->ReadFloat(ARC_ARGS(fireDelaySec));

	if (game <= GAME_DEMO5)
	{
		archiver->ReadEnum(ARC_ARGSE(repeatTrigger));
	}

	if (archiver->IsProperties())
	{
		archiver->ReadBool(ARC_ARGS(sendUntrigger));
	}

	if (archiver->IsSavegame())
	{
		archiver->ReadFloat(ARC_ARGS(nextTimeTriggerable));
		savedOtherVobPtr = archiver->ReadObjectAs<zCVob*>("savedOtherVob");
		archiver->ReadInt(ARC_ARGS(countCanBeActivated));

		bool isEnabled;
		archiver->ReadBool(ARC_ARGS(isEnabled));
		flags.isEnabled = isEnabled;
	}
	else
	{
		flags.isEnabled = flags.startEnabled;
	}

	return true;
}

bool zCMover::Archive(zCArchiver* archiver)
{
	if (!zCTrigger::Archive(archiver))
		return false;
	
	archiver->WriteGroupBegin("Mover");

	archiver->WriteEnum(ARC_ARGSEW(moverBehavior, "2STATE_TOGGLE;2STATE_TRIGGER_CTRL;2STATE_OPEN_TIME;NSTATE_LOOP;NSTATE_SINGLE_KEYS"));
	archiver->WriteFloat(ARC_ARGS(touchBlockerDamage));
	archiver->WriteFloat(ARC_ARGS(stayOpenTimeSec));
	archiver->WriteBool(ARC_ARGS(moverLocked));
	
	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteBool(ARC_ARGS(autoLinkEnabled));
	}

	if (game >= GAME_GOTHIC2)
	{
		archiver->WriteBool(ARC_ARGS(autoRotate));
	}

	if (game <= GAME_DEMO5)
	{
		archiver->WriteString(ARC_ARGS(vobChainName));
	}

	archiver->WriteGroupBegin("Keyframe");

	unsigned short numKeyframes = keyframeList.size();
	archiver->WriteWord(ARC_ARGS(numKeyframes));

	if (numKeyframes > 0)
	{
		archiver->WriteFloat(ARC_ARGS(moveSpeed));
		archiver->WriteEnum(ARC_ARGSEW(posLerpType, "LINEAR;CURVE"));
		archiver->WriteEnum(ARC_ARGSEW(speedType, "CONST;SLOW_START_END;SLOW_START;SLOW_END;SEG_SLOW_START_END;SEG_SLOW_START;SEG_SLOW_END"));

		if (!archiver->IsProperties())
		{
			archiver->WriteRaw("keyframes", (char*)&keyframeList[0],
				numKeyframes * sizeof(zTMov_Keyframe));
		}
	}

	archiver->WriteGroupEnd("Keyframe");

	if (archiver->IsSavegame())
	{
		archiver->WriteVec3(ARC_ARGS(actKeyPosDelta));
		archiver->WriteFloat(ARC_ARGS(actKeyframeF));
		archiver->WriteInt(ARC_ARGS(actKeyframe));
		archiver->WriteInt(ARC_ARGS(nextKeyframe));
		archiver->WriteFloat(ARC_ARGS(moveSpeedUnit));
		archiver->WriteFloat(ARC_ARGS(advanceDir));
		archiver->WriteEnum(ARC_ARGSEW(moverState, ""));
		archiver->WriteInt(ARC_ARGS(numTriggerEvents));
		archiver->WriteFloat(ARC_ARGS(stayOpenTimeDest));
	}

	archiver->WriteGroupBegin("Sound");

	archiver->WriteString(ARC_ARGS(sfxOpenStart));
	archiver->WriteString(ARC_ARGS(sfxOpenEnd));
	archiver->WriteString(ARC_ARGS(sfxMoving));
	archiver->WriteString(ARC_ARGS(sfxCloseStart));
	archiver->WriteString(ARC_ARGS(sfxCloseEnd));
	archiver->WriteString(ARC_ARGS(sfxLock));
	archiver->WriteString(ARC_ARGS(sfxUnlock));
	archiver->WriteString(ARC_ARGS(sfxUseLocked));

	archiver->WriteGroupEnd("Sound");

	archiver->WriteGroupEnd("Mover");

	return true;
}

bool zCMover::Unarchive(zCArchiver* archiver)
{
	if (!zCTrigger::Unarchive(archiver))
		return false;

	archiver->ReadEnum(ARC_ARGSE(moverBehavior));
	archiver->ReadFloat(ARC_ARGS(touchBlockerDamage));
	archiver->ReadFloat(ARC_ARGS(stayOpenTimeSec));
	archiver->ReadBool(ARC_ARGS(moverLocked));
	
	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadBool(ARC_ARGS(autoLinkEnabled));
	}

	if (game >= GAME_GOTHIC2)
	{
		archiver->ReadBool(ARC_ARGS(autoRotate));
	}

	if (game <= GAME_DEMO5)
	{
		archiver->ReadString(ARC_ARGS(vobChainName));
	}

	unsigned short numKeyframes = 0;
	archiver->ReadWord(ARC_ARGS(numKeyframes));

	if (numKeyframes > 0)
	{
		archiver->ReadFloat(ARC_ARGS(moveSpeed));
		archiver->ReadEnum(ARC_ARGSE(posLerpType));
		archiver->ReadEnum(ARC_ARGSE(speedType));

		if (!archiver->IsProperties())
		{
			keyframeList.resize(numKeyframes);

			archiver->ReadRaw("keyframes", (char*)&keyframeList[0],
				numKeyframes * sizeof(zTMov_Keyframe));
		}
	}

	if (archiver->IsSavegame())
	{
		archiver->ReadVec3(ARC_ARGS(actKeyPosDelta));
		archiver->ReadFloat(ARC_ARGS(actKeyframeF));
		archiver->ReadInt(ARC_ARGS(actKeyframe));
		archiver->ReadInt(ARC_ARGS(nextKeyframe));
		archiver->ReadFloat(ARC_ARGS(moveSpeedUnit));
		archiver->ReadFloat(ARC_ARGS(advanceDir));
		archiver->ReadEnum(ARC_ARGSE(moverState));
		archiver->ReadInt(ARC_ARGS(numTriggerEvents));
		archiver->ReadFloat(ARC_ARGS(stayOpenTimeDest));
	}

	archiver->ReadString(ARC_ARGS(sfxOpenStart));
	archiver->ReadString(ARC_ARGS(sfxOpenEnd));
	archiver->ReadString(ARC_ARGS(sfxMoving));
	archiver->ReadString(ARC_ARGS(sfxCloseStart));
	archiver->ReadString(ARC_ARGS(sfxCloseEnd));
	archiver->ReadString(ARC_ARGS(sfxLock));
	archiver->ReadString(ARC_ARGS(sfxUnlock));
	archiver->ReadString(ARC_ARGS(sfxUseLocked));

	return true;
}

bool oCTriggerChangeLevel::Archive(zCArchiver* archiver)
{
	if (!zCTrigger::Archive(archiver))
		return false;

	archiver->WriteString(ARC_ARGS(levelName));
	archiver->WriteString(ARC_ARGS(startVobName));

	return true;
}

bool oCTriggerChangeLevel::Unarchive(zCArchiver* archiver)
{
	if (!zCTrigger::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(levelName));
	archiver->ReadString(ARC_ARGS(startVobName));

	return true;
}

bool zCTriggerList::Archive(zCArchiver* archiver)
{
	if (!zCTrigger::Archive(archiver))
		return false;

	return true;
}

bool zCTriggerList::Unarchive(zCArchiver* archiver)
{
	if (!zCTrigger::Unarchive(archiver))
		return false;

	archiver->ReadEnum(ARC_ARGSE(listProcess));

	unsigned char numTarget = 6;

	if (!archiver->IsProperties())
	{
		archiver->ReadByte(ARC_ARGS(numTarget));
	}

	fireDelayList.resize(numTarget);
	triggerTargetList.resize(numTarget);

	for (size_t i = 0; i < numTarget; i++)
	{
		std::string	currTriggerTarget;
		float		currFireDelay;

		std::string currTriggerTargetKey	= "triggerTarget" + std::to_string(i);
		std::string currFireDelayKey		= "fireDelay" + std::to_string(i);

		archiver->ReadString(currTriggerTargetKey, currTriggerTarget);
		archiver->ReadFloat(currFireDelayKey, currFireDelay);

		triggerTargetList[i]	= currTriggerTarget;
		fireDelayList[i]		= currFireDelay;
	}

	if (archiver->IsSavegame())
	{
		archiver->ReadByte(ARC_ARGS(actTarget));
		archiver->ReadBool(ARC_ARGS(sendOnTrigger));
	}

	return true;
}

bool oCTriggerScript::Archive(zCArchiver* archiver)
{
	if (!zCTrigger::Archive(archiver))
		return false;

	return true;
}

bool oCTriggerScript::Unarchive(zCArchiver* archiver)
{
	if (!zCTrigger::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(scriptFunc));

	return true;
}

bool zCTriggerTeleport::Archive(zCArchiver* archiver)
{
	if (!zCTrigger::Archive(archiver))
		return false;

	return true;
}

bool zCTriggerTeleport::Unarchive(zCArchiver* archiver)
{
	if (!zCTrigger::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(sfxTeleport));

	return true;
}

bool zCTriggerWorldStart::Archive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Archive(archiver))
		return false;

	return true;
}

bool zCTriggerWorldStart::Unarchive(zCArchiver* archiver)
{
	if (!zCTriggerBase::Unarchive(archiver))
		return false;

	archiver->ReadBool(ARC_ARGS(fireOnlyFirstTime));

	if (archiver->IsSavegame())
	{
		archiver->ReadBool(ARC_ARGS(hasFired));
	}

	return true;
}

/*
	zCVobLevelCompo
*/

/*
	zCVobLight
*/

bool zCVobLight::Archive(zCArchiver* archiver)
{
	if (!zCVob::Archive(archiver))
		return false;

	archiver->WriteGroupBegin("VobLight");

	archiver->WriteString(ARC_ARGS(lightPresetInUse));

	// start of zCVobLightData::Unarchive
	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteEnum(ARC_ARGSEW(lightType, "POINT;SPOT;_RES_;_RES_"));
	}

	archiver->WriteFloat(ARC_ARGS(range));
	archiver->WriteColor(ARC_ARGS(color));

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteFloat(ARC_ARGS(spotConeAngle));
	}

	archiver->WriteBool(ARC_ARGS(lightStatic));
	archiver->WriteEnum(ARC_ARGSEW(lightQuality, "HIGH;MEDIUM;LOW_FASTEST"));

	if (game <= GAME_DEMO5)
	{
		archiver->WriteInt(ARC_ARGS(lensflareFXNo));
	}

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteString(ARC_ARGS(lensflareFX));
	}

	if (archiver->IsProperties() || !lightStatic || game <= GAME_DEMO5)
	{
		archiver->WriteGroupBegin("Dynamic Light");

		if (game >= GAME_SEPTEMBERDEMO)
		{
			archiver->WriteBool(ARC_ARGS(turnedOn));
		}

		archiver->WriteString(ARC_ARGS(rangeAniScale));
		archiver->WriteFloat(ARC_ARGS(rangeAniFPS));
		archiver->WriteBool(ARC_ARGS(rangeAniSmooth));
		archiver->WriteString(ARC_ARGS(colorAniList));
		archiver->WriteFloat(ARC_ARGS(colorAniFPS));
		archiver->WriteBool(ARC_ARGS(colorAniSmooth));

		if (game >= GAME_GOTHIC2)
		{
			archiver->WriteBool(ARC_ARGS(canMove));
		}

		archiver->WriteGroupEnd("Dynamic Light");
	}

	archiver->WriteGroupEnd("VobLight");

	return true;
}

bool zCVobLight::Unarchive(zCArchiver* archiver)
{
	if (!zCVob::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(lightPresetInUse));

	// start of zCVobLightData::Unarchive
	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadEnum(ARC_ARGSE(lightType));
	}

	archiver->ReadFloat(ARC_ARGS(range));
	archiver->ReadColor(ARC_ARGS(color));

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadFloat(ARC_ARGS(spotConeAngle));
	}

	archiver->ReadBool(ARC_ARGS(lightStatic));
	archiver->ReadEnum(ARC_ARGSE(lightQuality));

	if (game <= GAME_DEMO5)
	{
		archiver->ReadInt(ARC_ARGS(lensflareFXNo));
	}

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadString(ARC_ARGS(lensflareFX));
	}

	if (archiver->IsProperties() || !lightStatic || game <= GAME_DEMO5)
	{
		if (game >= GAME_SEPTEMBERDEMO)
		{
			archiver->ReadBool(ARC_ARGS(turnedOn));
		}

		archiver->ReadString(ARC_ARGS(rangeAniScale));
		archiver->ReadFloat(ARC_ARGS(rangeAniFPS));
		archiver->ReadBool(ARC_ARGS(rangeAniSmooth));
		archiver->ReadString(ARC_ARGS(colorAniList));
		archiver->ReadFloat(ARC_ARGS(colorAniFPS));
		archiver->ReadBool(ARC_ARGS(colorAniSmooth));

		if (game >= GAME_GOTHIC2)
		{
			archiver->ReadBool(ARC_ARGS(canMove));
		}
	}

	return true;
}

bool zCVobLight::Save(FileStream* file)
{
	if (!zCVob::Save(file))
		return false;

	return false;
}

bool zCVobLight::Load(FileStream* file)
{
	if (!zCVob::Load(file))
		return false;

	bool inVob = false;

	std::string line;

	while (file->ReadLine(line))
	{
		if (line == "{")
		{
			inVob = true;
		}
		else if (inVob)
		{
			std::string key;
			std::string value;

			if (line == "}")
			{
				inVob = false;
				return true;
			}
			else if (ParseFileLine(line, key, value))
			{
				if (key == "range")
				{
					range = std::stof(value);
				}
				else if (key == "colorRGBA")
				{
					float r, g, b, a;

					if (sscanf_s(value.c_str(), "%f %f %f %f", &r, &g, &b, &a) != 4)
					{
						return false;
					}

					color.r = r;
					color.g = g;
					color.b = b;
					color.a = a;
				}
				else if (key == "lightStatic")
				{
					lightStatic = (value == "1");
				}
				else if (key == "lightQual")
				{
					lightQuality = (zTVobLightQuality)std::stoi(value);
				}
				else if (key == "lensflareFX")
				{
					lensflareFX = value; // todo: fix
				}
				else if (key == "rangeAniScale")
				{
					rangeAniScale = value;
				}
				else if (key == "rangeAniFPS")
				{
					rangeAniFPS = std::stof(value);
				}
				else if (key == "rangeAniSmooth")
				{
					rangeAniSmooth = (value == "1");
				}
				else if (key == "colorAniList")
				{
					colorAniList = value; // todo: parse array
				}
				else if (key == "colorAniFPS")
				{
					colorAniFPS = std::stof(value);
				}
				else if (key == "colorAniSmooth")
				{
					colorAniSmooth = (value == "1");
				}
			}
		}
	}

	return false;
}

/*
	zCVobSpot
*/

/*
	zCVobStair
*/

/*
	zCVobStartpoint
*/

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

bool zCVobSound::Archive(zCArchiver* archiver)
{
	if (!zCZone::Archive(archiver))
		return false;

	archiver->WriteGroupBegin("Sound");

	if (game <= GAME_DEMO5)
	{
		archiver->WriteFloat(ARC_ARGS(sndRadius));
	}

	archiver->WriteFloat(ARC_ARGS(sndVolume));
	archiver->WriteEnum(ARC_ARGSEW(sndMode, "LOOPING;ONCE;RANDOM"));
	archiver->WriteFloat(ARC_ARGS(sndRandDelay));
	archiver->WriteFloat(ARC_ARGS(sndRandDelayVar));
	archiver->WriteBool(ARC_ARGS(sndStartOn));

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteBool(ARC_ARGS(sndAmbient3D));
		archiver->WriteBool(ARC_ARGS(sndObstruction));
		archiver->WriteFloat(ARC_ARGS(sndConeAngle));
		archiver->WriteEnum(ARC_ARGSEW(sndVolType, "SPHERE;ELLIPSOID"));
		archiver->WriteFloat(ARC_ARGS(sndRadius));
	}

	archiver->WriteString(ARC_ARGS(sndName));

	archiver->WriteGroupEnd("Sound");

	if (archiver->IsSavegame())
	{
		archiver->WriteBool(ARC_ARGS(soundIsRunning));
		archiver->WriteBool(ARC_ARGS(soundAllowedToRun));
	}

	return true;
}

bool zCVobSound::Unarchive(zCArchiver* archiver)
{
	if (!zCZone::Unarchive(archiver))
		return false;

	if (game <= GAME_DEMO5)
	{
		archiver->ReadFloat(ARC_ARGS(sndRadius));
	}

	archiver->ReadFloat(ARC_ARGS(sndVolume));
	archiver->ReadEnum(ARC_ARGSE(sndMode));
	archiver->ReadFloat(ARC_ARGS(sndRandDelay));
	archiver->ReadFloat(ARC_ARGS(sndRandDelayVar));
	archiver->ReadBool(ARC_ARGS(sndStartOn));

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadBool(ARC_ARGS(sndAmbient3D));
		archiver->ReadBool(ARC_ARGS(sndObstruction));
		archiver->ReadFloat(ARC_ARGS(sndConeAngle));
		archiver->ReadEnum(ARC_ARGSE(sndVolType));
		archiver->ReadFloat(ARC_ARGS(sndRadius));
	}

	archiver->ReadString(ARC_ARGS(sndName));

	if (archiver->IsSavegame())
	{
		archiver->ReadBool(ARC_ARGS(soundIsRunning));
		archiver->ReadBool(ARC_ARGS(soundAllowedToRun));
	}
	else
	{
		soundIsRunning		= sndStartOn;
		soundAllowedToRun	= sndStartOn;
	}

	return true;
}

bool zCVobSoundDaytime::Archive(zCArchiver* archiver)
{
	if (!zCVobSound::Archive(archiver))
		return false;

	archiver->WriteGroupBegin("SoundDaytime");

	archiver->WriteFloat(ARC_ARGS(sndStartTime));
	archiver->WriteFloat(ARC_ARGS(sndEndTime));
	archiver->WriteString(ARC_ARGS(sndName2));

	archiver->WriteGroupEnd("SoundDaytime");

	return true;
}

bool zCVobSoundDaytime::Unarchive(zCArchiver* archiver)
{
	if (!zCVobSound::Unarchive(archiver))
		return false;

	archiver->ReadFloat(ARC_ARGS(sndStartTime));
	archiver->ReadFloat(ARC_ARGS(sndEndTime));
	archiver->ReadString(ARC_ARGS(sndName2));

	return true;
}

bool oCZoneMusic::Archive(zCArchiver* archiver)
{
	if (!zCZoneMusic::Archive(archiver))
		return false;

	archiver->WriteGroupBegin("DynaMusic");
	
	archiver->WriteBool(ARC_ARGS(enabled));
	archiver->WriteInt(ARC_ARGS(priority));
	archiver->WriteBool(ARC_ARGS(ellipsoid));
	archiver->WriteFloat(ARC_ARGS(reverbLevel));
	archiver->WriteFloat(ARC_ARGS(volumeLevel));
	archiver->WriteBool(ARC_ARGS(loop));

	archiver->WriteGroupEnd("DynaMusic");

	if (archiver->IsSavegame())
	{
		archiver->WriteBool(ARC_ARGS(local_enabled));
		archiver->WriteBool(ARC_ARGS(dayEntranceDone));
		archiver->WriteBool(ARC_ARGS(nightEntranceDone));
	}

	return true;
}

bool oCZoneMusic::Unarchive(zCArchiver* archiver)
{
	if (!zCZoneMusic::Unarchive(archiver))
		return false;

	archiver->ReadBool(ARC_ARGS(enabled));
	archiver->ReadInt(ARC_ARGS(priority));
	archiver->ReadBool(ARC_ARGS(ellipsoid));
	archiver->ReadFloat(ARC_ARGS(reverbLevel));
	archiver->ReadFloat(ARC_ARGS(volumeLevel));
	archiver->ReadBool(ARC_ARGS(loop));

	if (archiver->IsSavegame())
	{
		archiver->ReadBool(ARC_ARGS(local_enabled));
		archiver->ReadBool(ARC_ARGS(dayEntranceDone));
		archiver->ReadBool(ARC_ARGS(nightEntranceDone));
	}
	else
	{
		local_enabled = enabled;
	}

	return true;
}

bool zCZoneVobFarPlane::Archive(zCArchiver* archiver)
{
	if (!zCZone::Archive(archiver))
		return false;

	return true;
}

bool zCZoneVobFarPlane::Unarchive(zCArchiver* archiver)
{
	if (!zCZone::Unarchive(archiver))
		return false;

	archiver->ReadFloat(ARC_ARGS(vobFarPlaneZ));
	archiver->ReadFloat(ARC_ARGS(innerRangePerc));

	return true;
}

bool zCZoneZFog::Archive(zCArchiver* archiver)
{
	if (!zCZone::Archive(archiver))
		return false;

	return true;
}

bool zCZoneZFog::Unarchive(zCArchiver* archiver)
{
	if (!zCZone::Unarchive(archiver))
		return false;

	archiver->ReadFloat(ARC_ARGS(fogRangeCenter));
	archiver->ReadFloat(ARC_ARGS(innerRangePerc));
	archiver->ReadColor(ARC_ARGS(fogColor));

	if (game >= GAME_GOTHIC2)
	{
		archiver->ReadBool(ARC_ARGS(fadeOutSky));
		archiver->ReadBool(ARC_ARGS(overrideColor));
	}

	return true;
}

/*
	AI classes
*/

bool zCAIPlayer::Archive(zCArchiver* archiver)
{
	if (!zCAIBase::Archive(archiver))
		return false;

	return false;
}

bool zCAIPlayer::Unarchive(zCArchiver* archiver)
{
	if (!zCAIBase::Unarchive(archiver))
		return false;

	if (archiver->IsSavegame())
	{
		archiver->ReadInt(ARC_ARGS(waterLevel));
		archiver->ReadFloat(ARC_ARGS(floorY));
		archiver->ReadFloat(ARC_ARGS(waterY));
		archiver->ReadFloat(ARC_ARGS(ceilY));
		archiver->ReadFloat(ARC_ARGS(feetY));
		archiver->ReadFloat(ARC_ARGS(headY));
		archiver->ReadFloat(ARC_ARGS(fallDistY));
		archiver->ReadFloat(ARC_ARGS(fallStartY));
	}

	return true;
}

bool oCAniCtrl_Human::Archive(zCArchiver* archiver)
{
	if (!zCAIPlayer::Archive(archiver))
		return false;

	return false;
}

bool oCAniCtrl_Human::Unarchive(zCArchiver* archiver)
{
	if (!zCAIPlayer::Unarchive(archiver))
		return false;

	if (archiver->IsSavegame())
	{
		aiNpc = archiver->ReadObjectAs<oCNpc*>("aiNpc");

		archiver->ReadInt(ARC_ARGS(walkMode));
		archiver->ReadInt(ARC_ARGS(weaponMode));
		archiver->ReadInt(ARC_ARGS(wmodeLast));
		archiver->ReadInt(ARC_ARGS(wmodeSelect));
		archiver->ReadBool(ARC_ARGS(changeWeapon));
		archiver->ReadInt(ARC_ARGS(actionMode));
	}

	return true;
}

/*
	Event manager
*/

bool zCEventManager::Archive(zCArchiver* archiver)
{
	if (!zCObject::Archive(archiver))
		return false;

	return false;
}

bool zCEventManager::Unarchive(zCArchiver* archiver)
{
	if (!zCObject::Unarchive(archiver))
		return false;

	return false;
}

/*
	Waynet
*/

bool zCWayNet::Archive(zCArchiver* archiver)
{
	if (!zCObject::Archive(archiver))
		return false;
	
	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->WriteInt(ARC_ARGS(waynetVersion));
	}

	// Waypoints
	if (waynetVersion == 0)
	{
		int numWaypoints = oldWaypointList.size();
		archiver->WriteInt(ARC_ARGS(numWaypoints));

		for (size_t i = 0; i < numWaypoints; i++)
		{
			std::string waypointKey = "waypoint" + std::to_string(i);
			archiver->WriteString(waypointKey, oldWaypointList[i]);
		}

	}
	else if (waynetVersion == 1)
	{
		int numWaypoints = waypointList.size();
		archiver->WriteInt(ARC_ARGS(numWaypoints));

		for (size_t i = 0; i < numWaypoints; i++)
		{
			std::string waypointKey = "waypoint" + std::to_string(i);
			archiver->WriteObject(waypointKey, waypointList[i]);
		}
	}

	// Ways
	if (waynetVersion == 0)
	{
		int numWays = oldWayList.size();
		archiver->WriteInt(ARC_ARGS(numWays));

		for (size_t i = 0; i < numWays; i++)
		{
			std::string wayKey = "way" + std::to_string(i);
			archiver->WriteString(wayKey, oldWayList[i]);
		}
	}
	else if (waynetVersion == 1)
	{
		int numWays = wayList.size();
		archiver->WriteInt(ARC_ARGS(numWays));

		for (size_t i = 0; i < numWays; i++)
		{
			std::string waylKey = "wayl" + std::to_string(i);
			archiver->WriteObject(waylKey, wayList[i].left);

			std::string wayrKey = "wayr" + std::to_string(i);
			archiver->WriteObject(wayrKey, wayList[i].right);
		}
	}

	return true;
}

bool zCWayNet::Unarchive(zCArchiver* archiver)
{
	if (!zCObject::Unarchive(archiver))
		return false;

	waynetVersion = 0;

	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->ReadInt(ARC_ARGS(waynetVersion));
	}

	// Waypoints
	int numWaypoints = 0;
	archiver->ReadInt(ARC_ARGS(numWaypoints));
	
	if (numWaypoints > 0 && game >= GAME_SEPTEMBERDEMO)
	{
		if (waynetVersion == 0)
		{
			oldWaypointList.resize(numWaypoints);

			for (size_t i = 0; i < numWaypoints; i++)
			{
				std::string waypointKey = "waypoint" + std::to_string(i);
				std::string waypoint;
				archiver->ReadString(waypointKey, waypoint);

				oldWaypointList[i] = waypoint;
			}

		}
		else if (waynetVersion == 1)
		{
			waypointList.resize(numWaypoints);

			for (size_t i = 0; i < numWaypoints; i++)
			{
				std::string waypointKey = "waypoint" + std::to_string(i);
				zCWaypoint* waypoint = archiver->ReadObjectAs<zCWaypoint*>(waypointKey);

				waypointList[i] = waypoint;
			}
		}
	}

	// Ways
	int numWays = 0;
	archiver->ReadInt(ARC_ARGS(numWays));

	if (numWays > 0)
	{
		if (waynetVersion == 0)
		{
			oldWayList.resize(numWays);

			for (size_t i = 0; i < numWays; i++)
			{
				std::string wayKey = "way" + std::to_string(i);
				std::string way;
				archiver->ReadString(wayKey, way);

				oldWayList[i] = way;
			}
		}
		else if (waynetVersion == 1)
		{
			wayList.resize(numWays);

			for (size_t i = 0; i < numWays; i++)
			{
				std::string waylKey = "wayl" + std::to_string(i);
				zCWaypoint* wayl = archiver->ReadObjectAs<zCWaypoint*>(waylKey);

				std::string wayrKey = "wayr" + std::to_string(i);
				zCWaypoint* wayr = archiver->ReadObjectAs<zCWaypoint*>(wayrKey);

				zCWay way;
				way.left = wayl;
				way.right = wayr;
				wayList[i] = way;
			}
		}
	}

	return true;
}

bool zCWayNet::SaveWaynet(FileStream* file)
{
	return false;
}

bool zCWayNet::LoadWaynet(FileStream* file)
{
	std::string line;

	while (file->ReadLine(line))
	{
		std::string key;
		std::string value;
	
		bool chunkStart = ParseFileLine(line, key, value);
	
		if (chunkStart)
		{
			// todo: ways
			// todo: wayProps
			if (key == "Waypoint")
			{
				oldWaypointList.push_back(value);
			}
			else if (key == "Way")
			{
				oldWayList.push_back(value);
			}
		}
		else
		{
			break;
		}
	}

	return true;
}

bool zCWaypoint::Archive(zCArchiver* archiver)
{
	if (!zCObject::Archive(archiver))
		return false;

	archiver->WriteString(ARC_ARGS(wpName));
	archiver->WriteInt(ARC_ARGS(waterDepth));
	archiver->WriteBool(ARC_ARGS(underWater));
	archiver->WriteVec3(ARC_ARGS(position));
	archiver->WriteVec3(ARC_ARGS(direction));

	return true;
}

bool zCWaypoint::Unarchive(zCArchiver* archiver)
{
	if (!zCObject::Unarchive(archiver))
		return false;

	archiver->ReadString(ARC_ARGS(wpName));
	archiver->ReadInt(ARC_ARGS(waterDepth));
	archiver->ReadBool(ARC_ARGS(underWater));
	archiver->ReadVec3(ARC_ARGS(position));
	archiver->ReadVec3(ARC_ARGS(direction));

	return true;
}
