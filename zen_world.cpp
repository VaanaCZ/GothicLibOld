//
// zen_world.cpp
//
// Copyright (c) 2021-2023 Václav Maroušek
//

#include "zen_world.h"

using namespace GothicLib::ZenGin;

/*
	World classes
*/

bool zCWorld::SaveWorld(FileStream* file, ARCHIVER_MODE mode, GAME game, bool savegame, bool properties, bool briefHeader, std::string user)
{
	if (game == GAME_DEMO3)
	{
		if (!SaveWorldFile(file, game))
		{
			return false;
		}

		file->Close();
	}
	else if (game >= GAME_DEMO5)
	{
		zCArchiver archiver;

		int version = 1;
		if (game <= GAME_DEMO5)
			version = 0;

		if (!archiver.Write(file, mode, version, savegame, properties, briefHeader, user))
		{
			return false;
		}

		if (!archiver.WriteObject(game, this))
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

	return true;
}

bool zCWorld::LoadWorld(FileStream* file, GAME game)
{
	if (game == GAME_DEMO3)
	{
		if (!LoadWorldFile(file, game))
		{
			return false;
		}
	}
	else if (game >= GAME_DEMO5)
	{
		zCArchiver archiver;
		if (!archiver.Read(file))
		{
			return false;
		}

		if (!archiver.ReadObject(game, this))
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

	return true;
}

bool zCWorld::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Archive(archiver, game))
		return false;

	// MeshAndBsp
	if (bsp)
	{
		archiver->WriteChunkStart("MeshAndBsp");
		if (!bsp->SaveBIN(archiver->GetFile(), game))
		{
			return false;
		}
		archiver->WriteChunkEnd();
	}

	// VobTree
	if (vobTree)
	{
		size_t vobCount = 0;

		archiver->WriteChunkStart("VobTree");
		if (!ArcVobTree(archiver, vobTree, vobCount, game))
		{
			return false;
		}
		archiver->WriteChunkEnd();
	}

	// WayNet
	if (wayNet && game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteChunkStart("WayNet");
		archiver->WriteObject(game, wayNet);
		archiver->WriteChunkEnd();
	}

	// EndMarker
	archiver->WriteChunkStart("EndMarker");
	archiver->WriteChunkEnd();

	// old WayNet
	if (wayNet && game <= GAME_DEMO5)
	{
		archiver->WriteObject(game, wayNet);
	}

	return true;
}

bool zCWorld::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Unarchive(archiver, game))
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
			
			if (!bsp->LoadBIN(archiver->GetFile(), game))
			{
				return false;
			}
		}
		else if (objectName == "VobTree")
		{
			size_t vobCount = 0;
			vobTree = new zCVob();
			
			if (!UnarcVobTree(archiver, vobTree, vobCount, game))
			{
				return false;
			}
		}
		else if (objectName == "WayNet")
		{
			wayNet = new zCWayNet();
			wayNet->world	= this;

			wayNet = archiver->ReadObjectAs<zCWayNet*>(game, wayNet);

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
		wayNet = new zCWayNet();
		wayNet->world	= this;

		wayNet = archiver->ReadObjectAs<zCWayNet*>(game, wayNet);

		if (!wayNet)
		{
			return false;
		}
	}

	return true;
}

bool zCWorld::ArcVobTree(zCArchiver* archiver, zCVob* vob, size_t& vobCount, GAME game)
{
	// Write vobs
	if (vobCount != 0)
	{
		archiver->WriteObject(game, vob);
	}
	
	// Write childs
	int childCount = vob->children.size();
	if (!archiver->WriteInt("childs" + std::to_string(vobCount), childCount))
		return false;

	vobCount++;

	for (size_t i = 0; i < childCount; i++)
	{
		if (!ArcVobTree(archiver, vob->children[i], vobCount, game))
			return false;
	}

	return true;
}

bool zCWorld::UnarcVobTree(zCArchiver* archiver, zCVob* vob, size_t& vobCount, GAME game)
{
	// Read vob
	zCVob* nextVob;

	if (vobCount != 0)
	{
		nextVob = archiver->ReadObjectAs<zCVob*>(game);

		if (!nextVob)
			return false;

		if (!nextVob->vobName.empty())
			vobTable[nextVob->vobName] = nextVob;

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
		if (!UnarcVobTree(archiver, nextVob, vobCount, game))
			return false;
	}

	return true;
}

bool zCWorld::SaveWorldFile(FileStream* file, GAME game)
{
	// These versions are not read by the game
	// but they should still be implemented
	// correctly.
	file->WriteLine("// *** Zengin WorldFile v0.01, meshVers: 2, bspVers:0, lmVers:0 ***", "\n");

	file->WriteLine("", "\n");
	file->WriteLine("ZenWorldFile", "\n");
	file->WriteLine("{", "\n");

	if (bsp)
	{
		file->WriteLine("MeshAndBsp", "\n");
		file->WriteLine("{", "\n");

		if (!bsp->SaveBIN(file, game))
		{
			return false;
		}

		file->WriteLine("}", "\n");
		file->WriteLine("", "\n");
	}

	if (vobTree)
	{
		file->WriteLine("VobHierarchy", "\n");
		file->WriteLine("{", "\n");

		size_t vobCount = 0;

		if (!SaveVobTree(file, vobTree, vobCount, game))
			return false;

		file->WriteLine("}", "\n");
	}

	file->WriteLine("}", "\n");

	if (wayNet)
	{
		if (!wayNet->SaveWaynet(file))
			return false;
	}

	return true;
}

bool zCWorld::LoadWorldFile(FileStream* file, GAME game)
{
	std::string line;

	while (file->ReadLine(line))
	{
		// Check if we entered the right chunk
		if (line.find("MeshAndBsp") == 0)
		{
			file->ReadLine(line); // {

			bsp = new zCBspTree();

			if (!bsp->LoadBIN(file, game))
			{
				return false;
			}
		}
		else if (line.find("VobHierarchy") == 0)
		{
			vobTree = new zCVob();

			if (!LoadVobTree(file, vobTree, game))
			{
				return false;
			}
		}
		else if (line.find("Waynet") == 0)
		{
			wayNet = new zCWayNet();
			wayNet->world	= this;

			if (!wayNet->LoadWaynet(file))
			{
				return false;
			}
		}
	}

	return true;
}

bool zCWorld::SaveVobTree(FileStream* file, zCVob* parentVob, size_t& vobCount, GAME game)
{
	if (vobCount != 0)
	{
		if (!parentVob->SaveVob(file, game))
			return false;
	}

	vobCount++;

	file->WriteLine("childs (" + std::to_string(parentVob->children.size()) + ")", "\n");
	file->WriteLine("", "\n");

	for (size_t i = 0; i < parentVob->children.size(); i++)
	{
		if (!SaveVobTree(file, parentVob->children[i], vobCount, game))
			return false;
	}

	return true;
}

bool zCWorld::LoadVobTree(FileStream* file, zCVob* parentVob, GAME game)
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
				if (!vob->LoadVob(file, game))
				{
					return false;
				}

				if (!vob->vobName.empty())
					vobTable[vob->vobName] = vob;
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
					vob->visualPtr->Load(file, game);
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
				if (!LoadVobTree(file, vob, game))
				{
					return false;
				}
			}

			return true;
		}
	}
}

bool oCWorld::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCWorld::Archive(archiver, game))
		return false;

	//todo: savegame
	
	return true;
}

bool oCWorld::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCWorld::Unarchive(archiver, game))
		return false;

	//todo: savegame

	return true;
}

/*
	BSP
*/

bool zCBspBase::SaveBIN(FileStream* file, zCBspTree* tree, GAME game)
{
	file->Write(FILE_ARGS(bbox));
	file->Write(FILE_ARGS(polyOffset));
	file->Write(FILE_ARGS(polyCount));

	tree->nodeCount++;

	if (IsNode())
	{
		zCBspNode* node = (zCBspNode*)this;

		uint8_t flag = 0;

		if (node->front)
		{
			flag |= 1;

			if (!node->front->IsNode())
			{
				flag |= 4;
			}
		}

		if (node->back)
		{
			flag |= 2;

			if (!node->back->IsNode())
			{
				flag |= 8;
			}
		}

		file->Write(FILE_ARGS(flag));
		file->Write(FILE_ARGS(node->plane));

		if (game <= GAME_GOTHICSEQUEL)
		{
			file->Write(FILE_ARGS(node->renderLod));
		}

		if (node->front)
		{
			node->front->SaveBIN(file, tree, game);
		}

		if (node->back)
		{
			node->back->SaveBIN(file, tree, game);
		}
	}
	else
	{
		tree->leafCount++;
	}

	return true;
}

bool zCBspBase::LoadBIN(FileStream* file, zCBspTree* tree, GAME game)
{
	file->Read(FILE_ARGS(bbox));
	file->Read(FILE_ARGS(polyOffset));
	file->Read(FILE_ARGS(polyCount));

	if (IsNode())
	{
		zCBspNode* node = (zCBspNode*)this;

		uint8_t flag;
		file->Read(FILE_ARGS(flag));
		file->Read(FILE_ARGS(node->plane));

		if (game <= GAME_GOTHICSEQUEL)
		{
			file->Read(FILE_ARGS(node->renderLod));
		}

		if ((flag & 1) != 0)
		{
			if ((flag & 4) != 0)
			{
				node->front = new zCBspLeaf();
			}
			else
			{
				node->front = new zCBspNode();
			}

			if (!node->front->LoadBIN(file, tree, game))
			{
				return false;
			}
		}

		if ((flag & 2) != 0)
		{
			if ((flag & 8) != 0)
			{
				node->back = new zCBspLeaf();
			}
			else
			{
				node->back = new zCBspNode();
			}

			if (!node->back->LoadBIN(file, tree, game))
			{
				return false;
			}
		}
	}

	return true;
}

bool zCBspTree::SaveBIN(FileStream* file, GAME game)
{
	// Write beginning
	uint64_t startPos = file->Tell();

	uint32_t version = 0;

	if (game >= GAME_GOTHIC2)
	{
		version = 0x04090000;
	}
	else if (game >= GAME_GOTHIC1)
	{
		version = 0x02090000;
	}
	else if (game >= GAME_SEPTEMBERDEMO)
	{
		version = 0x01090000;
	}
	else if (game >= GAME_DEMO5)
	{
		version = 0x00050000;
	}
	else
	{
		version = 0x00020000;
	}

	uint32_t length = 0;

	file->Write(FILE_ARGS(version));
	file->Write(FILE_ARGS(length));

	// Mesh
	if (!mesh.SaveMSH(file, game))
	{
		return false;
	}

	// Write BSP start
	uint64_t bspStart = file->StartBinChunk(BSPCHUNK_START);

	uint16_t bspVersion = 0;

	if (game >= GAME_GOTHIC2)
	{
		bspVersion = 3;
	}
	else if (game >= GAME_GOTHIC1)
	{
		bspVersion = 2;
	}
	else if (game >= GAME_SEPTEMBERDEMO)
	{
		bspVersion = 1;
	}
	else
	{
		bspVersion = 0;
	}

	file->Write(FILE_ARGS(bspVersion));
	file->Write(FILE_ARGS(mode));

	file->EndBinChunk(bspStart);

	// Polys
	uint64_t polysStart = file->StartBinChunk(BSPCHUNK_POLYS);

	uint32_t polyCount = polys.size();
	file->Write(FILE_ARGS(polyCount));

	if (polys.size() > 0)
	{
		file->Write(&polys[0], sizeof(uint32_t) * polys.size());
	}

	file->EndBinChunk(polysStart);

	// Tree
	uint64_t treeStart = file->StartBinChunk(BSPCHUNK_TREE);

	nodeCount = leafCount = 0;

	uint64_t countPos = file->Tell();

	file->Write(FILE_ARGS(nodeCount));
	file->Write(FILE_ARGS(leafCount));

	if (!root->SaveBIN(file, this, game))
	{
		return false;
	}

	uint64_t currPos2 = file->Tell();

	file->Seek(countPos);

	file->Write(FILE_ARGS(nodeCount));
	file->Write(FILE_ARGS(leafCount));

	file->Seek(currPos2);

	file->EndBinChunk(treeStart);

	// Lights
	if (game >= GAME_GOTHIC1)
	{
		uint64_t lightsStart = file->StartBinChunk(BSPCHUNK_LIGHTS);
	
		// For converting between versions
		if (lightPositions.size() != leafCount)
		{
			for (size_t i = 0; i < leafCount; i++)
			{
				zVEC3 newLight;
				newLight.x = newLight.y = newLight.z = -99.0f;
				lightPositions.push_back(newLight);
			}
		}

		if (lightPositions.size() > 0)
		{
			file->Write(&lightPositions[0], sizeof(zVEC3) * lightPositions.size());
		}

		file->EndBinChunk(lightsStart);
	}

	// Sectors
	uint64_t sectorsStart = file->StartBinChunk(BSPCHUNK_OUTDOOR_SECTORS);

	uint32_t sectorCount = sectors.size();
	file->Write(FILE_ARGS(sectorCount));
	
	for (size_t i = 0; i < sectors.size(); i++)
	{
		zCBspSector& sector = sectors[i];

		file->WriteLine(sector.name, "\n");

		uint32_t nodeCount = sector.nodes.size();
		uint32_t portalCount = sector.portalPolys.size();

		file->Write(FILE_ARGS(nodeCount));
		file->Write(FILE_ARGS(portalCount));

		if (sector.nodes.size() > 0)
		{
			file->Write(&sector.nodes[0], sizeof(uint32_t) * sector.nodes.size());
		}

		if (sector.portalPolys.size() > 0)
		{
			file->Write(&sector.portalPolys[0], sizeof(uint32_t) * sector.portalPolys.size());
		}
	}

	if (game >= GAME_SEPTEMBERDEMO)
	{
		uint32_t portalCount = portalPolys.size();
		file->Write(FILE_ARGS(portalCount));

		if (portalPolys.size() > 0)
		{
			file->Write(&portalPolys[0], sizeof(uint32_t) * portalPolys.size());
		}
	}

	file->EndBinChunk(sectorsStart);

	// End
	uint64_t endStart = file->StartBinChunk(BSPCHUNK_END);

	uint8_t endMarker = 0x42;
	file->Write(FILE_ARGS(endMarker));

	file->EndBinChunk(endStart);

	// Close chunk
	uint64_t currPos = file->Tell();

	length = currPos - startPos - 8;
	file->Seek(startPos + 4);
	file->Write(FILE_ARGS(length));

	file->Seek(currPos);

	return true;
}

bool zCBspTree::LoadBIN(FileStream* file, GAME game)
{
	// Read start
	uint32_t version, length;

	if (!file->Read(FILE_ARGS(version)) ||
		!file->Read(FILE_ARGS(length)))
	{
		LOG_ERROR("Unexpected end of file when reading MeshAndBsp chunk");
		return false;
	}

	if (game >= GAME_GOTHIC2)
	{
		if (version != 0x04090000)
		{
			LOG_ERROR("Unknown MeshAndBSP version \"" + std::to_string(version) + "\" found, expected version 0x04090000!");
			return false;
		}
	}
	else if (game >= GAME_GOTHIC1)
	{
		if (version != 0x02090000)
		{
			LOG_ERROR("Unknown MeshAndBSP version \"" + std::to_string(version) + "\" found, expected version 0x02090000!");
			return false;
		}
	}
	else if (game >= GAME_SEPTEMBERDEMO)
	{
		if (version != 0x01090000)
		{
			LOG_ERROR("Unknown MeshAndBSP version \"" + std::to_string(version) + "\" found, expected version 0x01090000!");
			return false;
		}
	}
	else if (game >= GAME_DEMO5)
	{
		if (version != 0x00050000)
		{
			LOG_ERROR("Unknown MeshAndBSP version \"" + std::to_string(version) + "\" found, expected version 0x00050000!");
			return false;
		}
	}
	else
	{
		if (version != 0x00020000)
		{
			LOG_ERROR("Unknown MeshAndBSP version \"" + std::to_string(version) + "\" found, expected version 0x00020000!");
			return false;
		}
	}

	// Mesh
	if (!mesh.LoadMSH(file, game))
	{
		return false;
	}

	// Read BSP chunks
	bool end = false;

	nodeCount = leafCount = 0;

	while (true)
	{
		uint16_t chunkId;
		uint32_t chunkSize;

		if (!file->Read(FILE_ARGS(chunkId)) ||
			!file->Read(FILE_ARGS(chunkSize)))
		{
			LOG_ERROR("Unexpected end of file while reading BSP chunk!");
			return false;
		}

		LOG_DEBUG("Reading zCBspTree chunk of id " + std::to_string(chunkId));

		uint64_t skipPos = file->Tell() + chunkSize;

		switch (chunkId)
		{

		case BSPCHUNK_START:
		{
			uint16_t bspVersion;
			file->Read(FILE_ARGS(bspVersion));

			if (game >= GAME_GOTHIC2)
			{
				if (bspVersion != 3)
				{
					LOG_ERROR("Unknown BSP version \"" + std::to_string(bspVersion) + "\" found, expected version 3!");
					return false;
				}
			}
			else if (game >= GAME_GOTHIC1)
			{
				if (bspVersion != 2)
				{
					LOG_ERROR("Unknown BSP version \"" + std::to_string(bspVersion) + "\" found, expected version 2!");
					return false;
				}
			}
			else if (game >= GAME_SEPTEMBERDEMO)
			{
				if (bspVersion != 1)
				{
					LOG_ERROR("Unknown BSP version \"" + std::to_string(bspVersion) + "\" found, expected version 1!");
					return false;
				}
			}
			else
			{
				if (bspVersion != 0)
				{
					LOG_ERROR("Unknown BSP version \"" + std::to_string(bspVersion) + "\" found, expected version 0!");
					return false;
				}
			}

			file->Read(FILE_ARGS(mode));

			break;
		}

		case BSPCHUNK_POLYS:
		{
			uint32_t polyCount = 0;
			file->Read(FILE_ARGS(polyCount));

			if (polyCount > 0)
			{
				polys.resize(polyCount);
				file->Read(&polys[0], sizeof(uint32_t) * polyCount);
			}

			break;
		}

		case BSPCHUNK_TREE:
		{
			file->Read(FILE_ARGS(nodeCount));
			file->Read(FILE_ARGS(leafCount));

			if (nodeCount > 0 || leafCount > 0)
			{
				root = new zCBspNode();
				if (!root->LoadBIN(file, this, game))
				{
					return false;
				}
			}

			break;
		}

		case BSPCHUNK_LIGHTS:
		{
			if (leafCount > 0)
			{
				lightPositions.resize(leafCount);
				file->Read(&lightPositions[0], sizeof(zVEC3) * leafCount);
			}

			break;
		}

		case BSPCHUNK_OUTDOOR_SECTORS:
		{
			// Sector
			uint32_t sectorCount = 0;
			file->Read(FILE_ARGS(sectorCount));
			
			if (sectorCount > 0)
			{
				sectors.resize(sectorCount);

				for (size_t i = 0; i < sectorCount; i++)
				{
					zCBspSector& sector = sectors[i];

					file->ReadLine(sector.name);

					uint32_t nodeCount, portalCount = 0;
					file->Read(FILE_ARGS(nodeCount));
					file->Read(FILE_ARGS(portalCount));

					if (nodeCount > 0)
					{
						sector.nodes.resize(nodeCount);
						file->Read(&sector.nodes[0], sizeof(uint32_t) * nodeCount);
					}

					if (portalCount > 0)
					{
						sector.portalPolys.resize(portalCount);
						file->Read(&sector.portalPolys[0], sizeof(uint32_t) * portalCount);
					}
				}
			}

			// Portals
			if (game >= GAME_SEPTEMBERDEMO)
			{
				uint32_t portalCount = 0;
				file->Read(FILE_ARGS(portalCount));

				if (portalCount > 0)
				{
					portalPolys.resize(portalCount);
					file->Read(&portalPolys[0], sizeof(uint32_t) * portalCount);
				}
			}

			break;
		}

		case BSPCHUNK_END:
		{
			uint8_t endMarker;
			file->Read(FILE_ARGS(endMarker)); // 42

			end = true;
			break;
		}

		default:
		{
			LOG_WARN("Unknown chunk type \"" + std::to_string(chunkId) + "\" in zCBspTree, skipping!");
			file->Seek(skipPos);

			break;
		}
		}

		if (end)
		{
			break;
		}
	}

	return true;
}

/*
	Vob classes
*/

/*
	zCVob
		oCVob
*/

bool zCVob::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Archive(archiver, game))
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

		archiver->WriteObject("visual", game, visualPtr);
		archiver->WriteObject("ai", game, aiPtr);

		if (archiver->IsSavegame())
		{
			archiver->WriteBool(ARC_ARGS(physicsEnabled));
		}
	}

	return true;
}

bool zCVob::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Unarchive(archiver, game))
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
			visualPtr = archiver->ReadObjectAs<zCVisual*>("visual", game);
		}

		if (dataRaw.bitfield.hasAIObject)
		{
			aiPtr = archiver->ReadObjectAs<zCAIBase*>("ai", game);
		}

		if (archiver->IsSavegame())
		{
			physicsEnabled = dataRaw.bitfield.physicsEnabled;

			if (dataRaw.bitfield.hasEventManObject)
			{
				eventManagerPtr = archiver->ReadObjectAs<zCEventManager*>("eventManager", game);
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

		visualPtr = archiver->ReadObjectAs<zCVisual*>("visual", game);
		aiPtr = archiver->ReadObjectAs<zCAIBase*>("ai", game);

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

bool zCVob::SaveVob(FileStream* file, GAME game)
{
	file->WriteLine("zCVob (" + std::to_string(vobID) + " " +
								std::to_string(vobType) + ")", "\n");

	file->WriteLine("{", "\n");

	file->WriteLine("{", "\n");

	file->WriteLine("vobType (" + std::to_string(vobType) + ")", "\n");
	
	if (!visual.empty())
	{
		file->WriteLine("visual (" + visual + ")", "\n");
	}

	file->WriteLine("vobName (" + vobName + ")", "\n");

	if (visualCamAlign == zVISUAL_CAMALIGN_NONE)
		file->WriteLine("visualCamAlign (NONE)", "\n");
	else if (visualCamAlign == zVISUAL_CAMALIGN_YAW)
		file->WriteLine("visualCamAlign (YAW)", "\n");
	else if (visualCamAlign == zVISUAL_CAMALIGN_FULL)
		file->WriteLine("visualCamAlign (FULL)", "\n");

	file->WriteLine("showVisual (" + std::to_string(showVisual) + ")", "\n");
	file->WriteLine("drawBBox3D (" + std::to_string(drawBBox3D) + ")", "\n");
	file->WriteLine("cdStatic (" + std::to_string(cdStatic) + ")", "\n");
	file->WriteLine("cdDyn (" + std::to_string(cdDyn) + ")", "\n");
	file->WriteLine("staticVob (" + std::to_string(staticVob) + ")", "\n");

	char buff[256];

	sprintf_s(buff, 256, "%.8g %.8g %.8g %.8g %.8g %.8g",
		bbox3DWS.min.x, bbox3DWS.min.y, bbox3DWS.min.z,
		bbox3DWS.max.x, bbox3DWS.max.y, bbox3DWS.max.z);
	file->WriteLine("bbox3DWSpace (" + std::string(buff) + ")", "\n");

	sprintf_s(buff, 256, "%.8g %.8g %.8g %.8g %.8g %.8g %.8g %.8g %.8g",
		trafoRot.x.x, trafoRot.x.y, trafoRot.x.z,
		trafoRot.y.x, trafoRot.y.y, trafoRot.y.z,
		trafoRot.z.x, trafoRot.z.y, trafoRot.z.z);
	file->WriteLine("trafoRot (" + std::string(buff) + " )", "\n");
	
	sprintf_s(buff, 256, "%.8g %.8g %.8g",
		trafoPos.x, trafoPos.y, trafoPos.z);
	file->WriteLine("trafoPos (" + std::string(buff) + " )", "\n");

	sprintf_s(buff, 256, "%.8g %.8g %.8g %.8g %.8g %.8g %.8g %.8g %.8g",
		trafoOSToWSRot.x.x, trafoOSToWSRot.x.y, trafoOSToWSRot.x.z,
		trafoOSToWSRot.y.x, trafoOSToWSRot.y.y, trafoOSToWSRot.y.z,
		trafoOSToWSRot.z.x, trafoOSToWSRot.z.y, trafoOSToWSRot.z.z);
	file->WriteLine("trafoOSToWSRot (" + std::string(buff) + " )", "\n");

	sprintf_s(buff, 256, "%.8g %.8g %.8g",
		trafoOSToWSPos.x, trafoOSToWSPos.y, trafoOSToWSPos.z);
	file->WriteLine("trafoOSToWSPos (" + std::string(buff) + " )", "\n");

	file->WriteLine("}", "\n");

	// Derived classes
	if (!Save(file, game))
		return false;

	// Decal
	zCDecal* decal = dynamic_cast<zCDecal*>(visualPtr);
	if (decal)
	{
		if (!decal->Save(file, game))
			return false;
	}

	file->WriteLine("}", "\n");

	return true;
}

bool zCVob::LoadVob(FileStream* file, GAME game)
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

				if (!Load(file, game))
					return false;

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
					showVisual = std::stoi(value);
				}
				else if (key == "drawBBox3D")
				{
					drawBBox3D = std::stoi(value);
				}
				else if (key == "cdStatic")
				{
					cdStatic = std::stoi(value);
				}
				else if (key == "cdDyn")
				{
					cdDyn = std::stoi(value);
				}
				else if (key == "staticVob")
				{
					staticVob = std::stoi(value);
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

bool zCCSCamera::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCVob::Archive(archiver, game))
		return false;

	archiver->WriteEnum(ARC_ARGSEW(camTrjFOR, "WORLD;OBJECT"));
	archiver->WriteEnum(ARC_ARGSEW(targetTrjFOR, "WORLD;OBJECT"));
	archiver->WriteEnum(ARC_ARGSEW(loopMode, "NONE;RESTART;PINGPONG"));
	archiver->WriteEnum(ARC_ARGSEW(splLerpMode, "UNDEF;PATH;PATH_IGNOREROLL;PATH_ROT_SAMPLES"));
	archiver->WriteBool(ARC_ARGS(ignoreFORVobRotCam));
	archiver->WriteBool(ARC_ARGS(ignoreFORVobRotTarget));
	archiver->WriteBool(ARC_ARGS(adaptToSurroundings));
	archiver->WriteBool(ARC_ARGS(easeToFirstKey));
	archiver->WriteBool(ARC_ARGS(easeFromLastKey));
	archiver->WriteFloat(ARC_ARGS(totalTime));
	archiver->WriteString(ARC_ARGS(autoCamFocusVobName));
	archiver->WriteBool(ARC_ARGS(autoCamPlayerMovable));
	archiver->WriteBool(ARC_ARGS(autoCamUntriggerOnLastKey));
	
	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->WriteFloat(ARC_ARGS(autoCamUntriggerOnLastKeyDelay));
	}

	if (!archiver->IsProperties())
	{
		int numPos		= positions.size();
		int numTargets	= targets.size();

		archiver->WriteInt(ARC_ARGS(numPos));
		archiver->WriteInt(ARC_ARGS(numTargets));

		for (size_t i = 0; i < numPos; i++)
		{
			archiver->WriteObject(game, positions[i]);
		}

		for (size_t i = 0; i < numTargets; i++)
		{
			archiver->WriteObject(game, targets[i]);
		}
	}

	if (archiver->IsSavegame())
	{
		archiver->WriteBool(ARC_ARGS(paused));
		archiver->WriteBool(ARC_ARGS(started));
		archiver->WriteBool(ARC_ARGS(gotoTimeMode));
		archiver->WriteFloat(ARC_ARGS(csTime));
	}

	return true;
}

bool zCCSCamera::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCVob::Unarchive(archiver, game))
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
				positions[i] = archiver->ReadObjectAs<zCCamTrj_KeyFrame*>(game);
			}
		}

		if (numTargets >= 0)
		{
			targets.resize(numTargets);

			for (size_t i = 0; i < numTargets; i++)
			{
				targets[i] = archiver->ReadObjectAs<zCCamTrj_KeyFrame*>(game);
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

bool zCCamTrj_KeyFrame::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCVob::Archive(archiver, game))
		return false;
	
	archiver->WriteFloat(ARC_ARGS(time));
	archiver->WriteFloat(ARC_ARGS(angleRollDeg));
	archiver->WriteFloat(ARC_ARGS(camFOVScale));
	archiver->WriteEnum(ARC_ARGSEW(motionType, "UNDEF;SMOOTH;LINEAR;STEP;SLOW;FAST;CUSTOM"));
	archiver->WriteEnum(ARC_ARGSEW(motionTypeFOV, "UNDEF;SMOOTH;LINEAR;STEP;SLOW;FAST;CUSTOM"));
	archiver->WriteEnum(ARC_ARGSEW(motionTypeRoll, "UNDEF;SMOOTH;LINEAR;STEP;SLOW;FAST;CUSTOM"));

	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->WriteEnum(ARC_ARGSEW(motionTypeTimeScale, "UNDEF;SMOOTH;LINEAR;STEP;SLOW;FAST;CUSTOM"));
	}

	archiver->WriteGroupBegin("Details");

	archiver->WriteFloat(ARC_ARGS(tension));
	archiver->WriteFloat(ARC_ARGS(bias));
	archiver->WriteFloat(ARC_ARGS(continuity));

	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->WriteFloat(ARC_ARGS(timeScale));
	}

	archiver->WriteBool(ARC_ARGS(timeIsFixed));

	archiver->WriteGroupEnd("Details");

	if (!archiver->IsProperties())
	{
		archiver->WriteRaw(ARC_ARGSR(originalPose));
	}

	return true;
}

bool zCCamTrj_KeyFrame::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCVob::Unarchive(archiver, game))
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

bool zCEarthquake::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Archive(archiver, game))
		return false;

	archiver->WriteGroupBegin("Earthquake");

	archiver->WriteFloat(ARC_ARGS(radius));
	archiver->WriteFloat(ARC_ARGS(timeSec));
	archiver->WriteVec3(ARC_ARGS(amplitudeCM));

	archiver->WriteGroupEnd("Earthquake");

	return true;
}

bool zCEarthquake::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Unarchive(archiver, game))
		return false;

	archiver->ReadFloat(ARC_ARGS(radius));
	archiver->ReadFloat(ARC_ARGS(timeSec));
	archiver->ReadVec3(ARC_ARGS(amplitudeCM));

	return true;
}

bool zCPFXControler::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Archive(archiver, game))
		return false;

	archiver->WriteString(ARC_ARGS(pfxName));
	archiver->WriteBool(ARC_ARGS(killVobWhenDone));
	archiver->WriteBool(ARC_ARGS(pfxStartOn));

	return true;
}

bool zCPFXControler::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Unarchive(archiver, game))
		return false;

	archiver->ReadString(ARC_ARGS(pfxName));
	archiver->ReadBool(ARC_ARGS(killVobWhenDone));
	archiver->ReadBool(ARC_ARGS(pfxStartOn));

	return true;
}

bool zCVobAnimate::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Archive(archiver, game))
		return false;

	archiver->WriteBool(ARC_ARGS(startOn));

	if (archiver->IsSavegame())
	{
		archiver->WriteBool(ARC_ARGS(isRunning));
	}

	return true;
}

bool zCVobAnimate::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Unarchive(archiver, game))
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

bool zCTouchDamage::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Archive(archiver, game))
		return false;

	archiver->WriteGroupBegin("TouchDamage");

	archiver->WriteFloat(ARC_ARGS(damage));

	if (game >= GAME_GOTHIC1)
	{
		archiver->WriteGroupBegin("DamageType");

		archiver->WriteBool(ARC_ARGS(Barrier));
		archiver->WriteBool(ARC_ARGS(Blunt));
		archiver->WriteBool(ARC_ARGS(Edge));
		archiver->WriteBool(ARC_ARGS(Fire));
		archiver->WriteBool(ARC_ARGS(Fly));
		archiver->WriteBool(ARC_ARGS(Magic));
		archiver->WriteBool(ARC_ARGS(Point));
		archiver->WriteBool(ARC_ARGS(Fall));

		archiver->WriteGroupEnd("DamageType");
	}
	else
	{
		int damageType = 0;

		if (Edge)		damageType = 0;
		else if (Blunt)	damageType = 1;
		else if (Point)	damageType = 2;
		else if (Fire)	damageType = 3;
		else if (Magic)	damageType = 4;
		else if (Fly)	damageType = 5;

		archiver->WriteEnum(ARC_ARGSEW(damageType, "DAM_EDGE;DAM_BLUNT;DAM_POINT;DAM_FIRE;DAM_MAGIC;DAM_FLY"));
	}

	archiver->WriteFloat(ARC_ARGS(damageRepeatDelaySec));
	archiver->WriteFloat(ARC_ARGS(damageVolDownScale));
	archiver->WriteEnum(ARC_ARGSEW(damageCollType, "NONE;BOX;POINT"));

	archiver->WriteGroupEnd("TouchDamage");

	return true;
}

bool zCTouchDamage::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Unarchive(archiver, game))
		return false;

	archiver->ReadFloat(ARC_ARGS(damage));

	if (game >= GAME_GOTHIC1)
	{
		archiver->ReadBool(ARC_ARGS(Barrier));
		archiver->ReadBool(ARC_ARGS(Blunt));
		archiver->ReadBool(ARC_ARGS(Edge));
		archiver->ReadBool(ARC_ARGS(Fire));
		archiver->ReadBool(ARC_ARGS(Fly));
		archiver->ReadBool(ARC_ARGS(Magic));
		archiver->ReadBool(ARC_ARGS(Point));
		archiver->ReadBool(ARC_ARGS(Fall));
	}
	else
	{
		int damageType;
		archiver->ReadEnum(ARC_ARGSE(damageType));

		switch (damageType)
		{
		case 0:		Edge = true;	break;
		case 1:		Blunt = true;	break;
		case 2:		Point = true;	break;
		case 3:		Fire = true;	break;
		case 4:		Magic = true;	break;
		case 5:		Fly = true;		break;
		default:					break;
		}
	}

	archiver->ReadFloat(ARC_ARGS(damageRepeatDelaySec));
	archiver->ReadFloat(ARC_ARGS(damageVolDownScale));
	archiver->ReadEnum(ARC_ARGSE(damageCollType));

	return true;
}

bool zCVobLensFlare::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Archive(archiver, game))
		return false;

	archiver->WriteString(ARC_ARGS(lensflareFX));

	return true;
}

bool zCVobLensFlare::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Unarchive(archiver, game))
		return false;

	archiver->ReadString(ARC_ARGS(lensflareFX));

	return true;
}

bool zCVobScreenFX::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Archive(archiver, game))
		return false;

	return true;
}

bool zCVobScreenFX::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCEffect::Unarchive(archiver, game))
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

bool oCItem::Archive(zCArchiver* archiver, GAME game)
{
	if (!oCVob::Archive(archiver, game))
		return false;

	archiver->WriteString(ARC_ARGS(itemInstance));

	if (archiver->IsSavegame())
	{
		archiver->WriteInt(ARC_ARGS(amount));
		archiver->WriteInt(ARC_ARGS(flags));
	}

	return true;
}

bool oCItem::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!oCVob::Unarchive(archiver, game))
		return false;

	archiver->ReadString(ARC_ARGS(itemInstance));

	if (archiver->IsSavegame())
	{
		archiver->ReadInt(ARC_ARGS(amount));
		archiver->ReadInt(ARC_ARGS(flags));
	}

	return true;
}

bool oCItem::Save(FileStream* file, GAME game)
{
	if (!zCVob::Save(file, game))
		return false;

	file->WriteLine("{", "\n");

	file->WriteLine("INSTANCE (" + itemInstance + ")", "\n");

	file->WriteLine("}", "\n");

	return true;
}

bool oCItem::Load(FileStream* file, GAME game)
{
	if (!zCVob::Load(file, game))
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

bool oCMOB::Archive(zCArchiver* archiver, GAME game)
{
	if (!oCVob::Archive(archiver, game))
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

bool oCMOB::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!oCVob::Unarchive(archiver, game))
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

bool oCMobInter::Archive(zCArchiver* archiver, GAME game)
{
	if (!oCMOB::Archive(archiver, game))
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

bool oCMobInter::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!oCMOB::Unarchive(archiver, game))
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

bool oCMobFire::Archive(zCArchiver* archiver, GAME game)
{
	if (!oCMobInter::Archive(archiver, game))
		return false;

	archiver->WriteGroupBegin("Fire");

	archiver->WriteString(ARC_ARGS(fireSlot));
	archiver->WriteString(ARC_ARGS(fireVobtreeName));

	archiver->WriteGroupEnd("Fire");

	return true;
}

bool oCMobFire::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!oCMobInter::Unarchive(archiver, game))
		return false;

	archiver->ReadString(ARC_ARGS(fireSlot));
	archiver->ReadString(ARC_ARGS(fireVobtreeName));

	return true;
}

bool oCMobLockable::Archive(zCArchiver* archiver, GAME game)
{
	if (!oCMobInter::Archive(archiver, game))
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

bool oCMobLockable::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!oCMobInter::Unarchive(archiver, game))
		return false;

	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->ReadBool(ARC_ARGS(locked));
		archiver->ReadString(ARC_ARGS(keyInstance));
		archiver->ReadString(ARC_ARGS(pickLockStr));
	}

	return true;
}

bool oCMobContainer::Archive(zCArchiver* archiver, GAME game)
{
	if (!oCMobLockable::Archive(archiver, game))
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

bool oCMobContainer::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!oCMobLockable::Unarchive(archiver, game))
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

bool oCMob::Archive(zCArchiver* archiver, GAME game)
{
	if (!oCVob::Archive(archiver, game))
		return false;

	archiver->WriteString(ARC_ARGS(mobInstance));

	return true;
}

bool oCMob::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!oCVob::Unarchive(archiver, game))
		return false;

	archiver->ReadString(ARC_ARGS(mobInstance));

	return true;
}

bool oCMob::Save(FileStream* file, GAME game)
{
	if (!zCVob::Save(file, game))
		return false;

	file->WriteLine("{", "\n");

	file->WriteLine("INSTANCE (" + mobInstance + ")", "\n");

	file->WriteLine("}", "\n");

	return true;
}

bool oCMob::Load(FileStream* file, GAME game)
{
	if (!zCVob::Load(file, game))
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

bool oCNpc::Archive(zCArchiver* archiver, GAME game)
{
	if (!oCVob::Archive(archiver, game))
		return false;

	archiver->WriteString(ARC_ARGS(npcInstance));

	// todo: savegame

	return true;
}

bool oCNpc::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!oCVob::Unarchive(archiver, game))
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

bool oCNpc::Save(FileStream* file, GAME game)
{
	if (!zCVob::Save(file, game))
		return false;

	file->WriteLine("{", "\n");

	file->WriteLine("INSTANCE (" + npcInstance + ")", "\n");

	file->WriteLine("}", "\n");

	return true;
}

bool oCNpc::Load(FileStream* file, GAME game)
{
	if (!zCVob::Load(file, game))
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

bool zCTriggerBase::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCVob::Archive(archiver, game))
		return false;

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->WriteString(ARC_ARGS(triggerTarget));
	}

	return true;
}

bool zCTriggerBase::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCVob::Unarchive(archiver, game))
		return false;

	if (game >= GAME_SEPTEMBERDEMO)
	{
		archiver->ReadString(ARC_ARGS(triggerTarget));
	}

	return true;
}

bool zCCodeMaster::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Archive(archiver, game))
		return false;

	archiver->WriteGroupBegin("CodeMaster");

	archiver->WriteBool(ARC_ARGS(orderRelevant));

	archiver->WriteGroupBegin("OrderRelevant");

	archiver->WriteBool(ARC_ARGS(firstFalseIsFailure));
	archiver->WriteString(ARC_ARGS(triggerTargetFailure));

	archiver->WriteGroupEnd("OrderRelevant");

	archiver->WriteGroupBegin("OrderNotRelevant");

	archiver->WriteBool(ARC_ARGS(untriggerCancels));

	archiver->WriteGroupEnd("OrderNotRelevant");

	if (!archiver->IsProperties())
	{
		unsigned char numSlaves = slaveVobNameList.size();
		archiver->WriteByte(ARC_ARGS(numSlaves));

		for (size_t i = 0; i < numSlaves; i++)
		{
			std::string slaveVobNameKey = "slaveVobName" + std::to_string(i);
			archiver->WriteString(slaveVobNameKey, slaveVobNameList[i]);
		}
	}

	if (archiver->IsSavegame())
	{
		unsigned char numSlavesTriggered = slaveTriggeredList.size();
		archiver->WriteByte(ARC_ARGS(numSlavesTriggered));

		for (size_t i = 0; i < numSlavesTriggered; i++)
		{
			std::string slaveTriggeredKey = "slaveTriggered" + std::to_string(i);
			archiver->WriteObject(slaveTriggeredKey, game, slaveTriggeredList[i]);
		}
	}

	archiver->WriteGroupEnd("CodeMaster");

	return true;
}

bool zCCodeMaster::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Unarchive(archiver, game))
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
			slaveTriggeredList[i] = archiver->ReadObjectAs<zCVob*>(slaveTriggeredKey, game);
		}
	}

	return true;
}

bool zCMessageFilter::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Archive(archiver, game))
		return false;

	archiver->WriteEnum(ARC_ARGSEW(onTrigger, "MT_NONE;MT_TRIGGER;MT_UNTRIGGER;MT_ENABLE;MT_DISABLE;MT_TOGGLE_ENABLED"));
	archiver->WriteEnum(ARC_ARGSEW(onUntrigger, "MT_NONE;MT_TRIGGER;MT_UNTRIGGER;MT_ENABLE;MT_DISABLE;MT_TOGGLE_ENABLED"));

	return true;
}

bool zCMessageFilter::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Unarchive(archiver, game))
		return false;

	archiver->ReadEnum(ARC_ARGSE(onTrigger));
	archiver->ReadEnum(ARC_ARGSE(onUntrigger));

	return true;
}

bool zCMoverControler::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Archive(archiver, game))
		return false;

	archiver->WriteEnum(ARC_ARGSEW(moverMessage, "GOTO_KEY_FIXED_DIRECTLY;_DISABLED_;GOTO_KEY_NEXT;GOTO_KEY_PREV"));
	archiver->WriteInt(ARC_ARGS(gotoFixedKey));

	return true;
}

bool zCMoverControler::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Unarchive(archiver, game))
		return false;

	archiver->ReadEnum(ARC_ARGSE(moverMessage));
	archiver->ReadInt(ARC_ARGS(gotoFixedKey));

	return true;
}

bool zCTrigger::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Archive(archiver, game))
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
		archiver->WriteObject("savedOtherVob", game, savedOtherVobPtr);
		archiver->WriteInt(ARC_ARGS(countCanBeActivated));

		bool isEnabled = flags.isEnabled;
		archiver->WriteBool(ARC_ARGS(isEnabled));
	}

	archiver->WriteGroupEnd("Trigger");

	return true;
}

bool zCTrigger::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Unarchive(archiver, game))
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
		zBOOL reactToOnTrigger;
		archiver->ReadBool(ARC_ARGS(reactToOnTrigger));
		filterFlags.reactToOnTrigger = reactToOnTrigger;

		zBOOL reactToOnTouch;
		archiver->ReadBool(ARC_ARGS(reactToOnTouch));
		filterFlags.reactToOnTouch = reactToOnTouch;

		zBOOL reactToOnDamage;
		archiver->ReadBool(ARC_ARGS(reactToOnDamage));
		filterFlags.reactToOnDamage = reactToOnDamage;

		zBOOL respondToObject;
		archiver->ReadBool(ARC_ARGS(respondToObject));
		filterFlags.respondToObject = respondToObject;

		zBOOL respondToPC;
		archiver->ReadBool(ARC_ARGS(respondToPC));
		filterFlags.respondToPC = respondToPC;

		zBOOL respondToNPC;
		archiver->ReadBool(ARC_ARGS(respondToNPC));
		filterFlags.respondToNPC = respondToNPC;

		zBOOL startEnabled;
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
		savedOtherVobPtr = archiver->ReadObjectAs<zCVob*>("savedOtherVob", game);
		archiver->ReadInt(ARC_ARGS(countCanBeActivated));

		zBOOL isEnabled;
		archiver->ReadBool(ARC_ARGS(isEnabled));
		flags.isEnabled = isEnabled;
	}
	else
	{
		flags.isEnabled = flags.startEnabled;
	}

	return true;
}

bool zCMover::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Archive(archiver, game))
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

bool zCMover::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Unarchive(archiver, game))
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

bool oCTriggerChangeLevel::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Archive(archiver, game))
		return false;

	archiver->WriteString(ARC_ARGS(levelName));
	archiver->WriteString(ARC_ARGS(startVobName));

	return true;
}

bool oCTriggerChangeLevel::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Unarchive(archiver, game))
		return false;

	archiver->ReadString(ARC_ARGS(levelName));
	archiver->ReadString(ARC_ARGS(startVobName));

	return true;
}

bool zCTriggerList::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Archive(archiver, game))
		return false;

	archiver->WriteGroupBegin("TriggerList");
	
	archiver->WriteEnum(ARC_ARGSEW(listProcess, "LP_ALL;LP_NEXT_ONE;LP_RAND_ONE"));

	unsigned char numTarget = triggerTargetList.size();

	if (!archiver->IsProperties())
	{
		archiver->WriteByte(ARC_ARGS(numTarget));
	}

	for (size_t i = 0; i < numTarget; i++)
	{
		std::string currTriggerTargetKey	= "triggerTarget" + std::to_string(i);
		std::string currFireDelayKey		= "fireDelay" + std::to_string(i);

		archiver->WriteString(currTriggerTargetKey, triggerTargetList[i]);
		archiver->WriteFloat(currFireDelayKey, fireDelayList[i]);
	}

	if (archiver->IsSavegame())
	{
		archiver->WriteByte(ARC_ARGS(actTarget));
		archiver->WriteBool(ARC_ARGS(sendOnTrigger));
	}

	archiver->WriteGroupEnd("TriggerList");

	return true;
}

bool zCTriggerList::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Unarchive(archiver, game))
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

bool oCTriggerScript::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Archive(archiver, game))
		return false;

	archiver->WriteString(ARC_ARGS(scriptFunc));

	return true;
}

bool oCTriggerScript::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Unarchive(archiver, game))
		return false;

	archiver->ReadString(ARC_ARGS(scriptFunc));

	return true;
}

bool zCTriggerTeleport::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Archive(archiver, game))
		return false;

	archiver->WriteString(ARC_ARGS(sfxTeleport));

	return true;
}

bool zCTriggerTeleport::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTrigger::Unarchive(archiver, game))
		return false;

	archiver->ReadString(ARC_ARGS(sfxTeleport));

	return true;
}

bool zCTriggerWorldStart::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Archive(archiver, game))
		return false;

	archiver->WriteBool(ARC_ARGS(fireOnlyFirstTime));

	if (archiver->IsSavegame())
	{
		archiver->WriteBool(ARC_ARGS(hasFired));
	}

	return true;
}

bool zCTriggerWorldStart::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCTriggerBase::Unarchive(archiver, game))
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

bool zCVobLight::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCVob::Archive(archiver, game))
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


		std::string rangeAniScaleStr = "";

		for (size_t i = 0; i < rangeAniScaleList.size(); i++)
		{
			rangeAniScaleStr += FloatToString(rangeAniScaleList[i]) + " ";
		}

		archiver->WriteString("rangeAniScale", rangeAniScaleStr);


		archiver->WriteFloat(ARC_ARGS(rangeAniFPS));
		archiver->WriteBool(ARC_ARGS(rangeAniSmooth));


		std::string colorAniListStr = "";

		for (size_t i = 0; i < colorAniList.size(); i++)
		{
			if (colorAniList[i].r == colorAniList[i].g &&
				colorAniList[i].r == colorAniList[i].b)
			{
				colorAniListStr += FloatToString(colorAniList[i].r) + " ";
			}
			else
			{
				colorAniListStr += "(" + FloatToString(colorAniList[i].r) + " " +
					FloatToString(colorAniList[i].g) + " " +
					FloatToString(colorAniList[i].b) + ") ";
			}
		}

		archiver->WriteString("colorAniList", colorAniListStr);


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

bool zCVobLight::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCVob::Unarchive(archiver, game))
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

		// Range ani scale list parsing
		std::string rangeAniScaleStr;
		archiver->ReadString("rangeAniScale", rangeAniScaleStr);

		if (!rangeAniScaleStr.empty())
		{
			size_t pos = 0;

			while (true)
			{
				if (pos == rangeAniScaleStr.size())
					break;

				rangeAniScaleList.push_back(atof(&rangeAniScaleStr[pos]));

				pos = rangeAniScaleStr.find(" ", pos);
				if (pos == std::string::npos)
					break;
				pos++;

			}
		}

		archiver->ReadFloat(ARC_ARGS(rangeAniFPS));
		archiver->ReadBool(ARC_ARGS(rangeAniSmooth));

		// Color ani list parsing
		std::string colorAniListStr;
		archiver->ReadString("colorAniList", colorAniListStr);

		if (!colorAniListStr.empty())
		{
			size_t pos = 0;

			while (true)
			{
				if (pos == colorAniListStr.size())
					break;

				float r, g, b;

				if (colorAniListStr[pos] == '(')
				{
					r = atof(&colorAniListStr[pos + 1]);
					pos = colorAniListStr.find(" ", pos) + 1;
					g = atof(&colorAniListStr[pos]);
					pos = colorAniListStr.find(" ", pos) + 1;
					b = atof(&colorAniListStr[pos]);
				}
				else
				{
					r = g = b = atof(&colorAniListStr[pos]);
				}

				zCOLOR colorAni;
				colorAni.r = r;
				colorAni.g = g;
				colorAni.b = b;
				colorAni.a = 255;
				colorAniList.push_back(colorAni);

				pos = colorAniListStr.find(" ", pos);
				if (pos == std::string::npos)
					break;
				pos++;
			}
		}

		archiver->ReadFloat(ARC_ARGS(colorAniFPS));
		archiver->ReadBool(ARC_ARGS(colorAniSmooth));

		if (game >= GAME_GOTHIC2)
		{
			archiver->ReadBool(ARC_ARGS(canMove));
		}
	}

	return true;
}

bool zCVobLight::Save(FileStream* file, GAME game)
{
	if (!zCVob::Save(file, game))
		return false;

	file->WriteLine("{", "\n");

	file->WriteLine(" range (" + FloatToString(range) + ")", "\n");

	file->WriteLine(" colorRGBA (" +
					FloatToString(color.r) + " " +
					FloatToString(color.g) + " " +
					FloatToString(color.b) + " " +
					FloatToString(color.a) + ")", "\n");

	file->WriteLine(" lightStatic (" + std::to_string(lightStatic) + ")", "\n");
	file->WriteLine(" lightQual (" + std::to_string(lightQuality) + ")", "\n");
	file->WriteLine(" lensflareFX (" + std::to_string(lensflareFXNo) + ")", "\n");

	std::string rangeAniScaleStr = "";

	for (size_t i = 0; i < rangeAniScaleList.size(); i++)
	{
		rangeAniScaleStr += FloatToString(rangeAniScaleList[i]) + " ";
	}

	file->WriteLine(" rangeAniScale (" + rangeAniScaleStr + ")", "\n");

	file->WriteLine(" rangeAniFPS (" + FloatToString(rangeAniFPS) + ")", "\n");
	file->WriteLine(" rangeAniSmooth (" + std::to_string(rangeAniSmooth) + ")", "\n");

	std::string colorAniListStr = "";

	for (size_t i = 0; i < colorAniList.size(); i++)
	{
		if (colorAniList[i].r == colorAniList[i].g &&
			colorAniList[i].r == colorAniList[i].b)
		{
			colorAniListStr += FloatToString(colorAniList[i].r) + " ";
		}
		else
		{
			colorAniListStr += "[" + FloatToString(colorAniList[i].r) + " " +
				FloatToString(colorAniList[i].g) + " " +
				FloatToString(colorAniList[i].b) + "] ";
		}
	}

	file->WriteLine(" colorAniList (" + colorAniListStr + ")", "\n");

	file->WriteLine(" colorAniFPS (" + FloatToString(colorAniFPS) + ")", "\n");
	file->WriteLine(" colorAniSmooth (" + std::to_string(colorAniSmooth) + ")", "\n");

	file->WriteLine("}", "\n");

	return true;
}

bool zCVobLight::Load(FileStream* file, GAME game)
{
	if (!zCVob::Load(file, game))
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
					lightStatic = std::stoi(value);
				}
				else if (key == "lightQual")
				{
					lightQuality = (zTVobLightQuality)std::stoi(value);
				}
				else if (key == "lensflareFX")
				{
					lensflareFXNo = std::stoi(value);
				}
				else if (key == "rangeAniScale")
				{
					std::string rangeAniScaleStr = value;

					if (!rangeAniScaleStr.empty())
					{
						size_t pos = 0;

						while (true)
						{
							if (pos == rangeAniScaleStr.size())
								break;

							rangeAniScaleList.push_back(atof(&rangeAniScaleStr[pos]));

							pos = rangeAniScaleStr.find(" ", pos);
							if (pos == std::string::npos)
								break;
							pos++;

						}
					}
				}
				else if (key == "rangeAniFPS")
				{
					rangeAniFPS = std::stof(value);
				}
				else if (key == "rangeAniSmooth")
				{
					rangeAniSmooth = std::stoi(value);
				}
				else if (key == "colorAniList")
				{
					std::string colorAniListStr = value;

					if (!colorAniListStr.empty())
					{
						size_t pos = 0;

						while (true)
						{
							if (pos == colorAniListStr.size())
								break;

							float r, g, b;

							if (colorAniListStr[pos] == '[')
							{
								r = atof(&colorAniListStr[pos + 1]);
								pos = colorAniListStr.find(" ", pos) + 1;
								g = atof(&colorAniListStr[pos]);
								pos = colorAniListStr.find(" ", pos) + 1;
								b = atof(&colorAniListStr[pos]);
							}
							else
							{
								r = g = b = atof(&colorAniListStr[pos]);
							}

							zCOLOR colorAni;
							colorAni.r = r;
							colorAni.g = g;
							colorAni.b = b;
							colorAni.a = 255;
							colorAniList.push_back(colorAni);

							pos = colorAniListStr.find(" ", pos);
							if (pos == std::string::npos)
								break;
							pos++;
						}
					}
				}
				else if (key == "colorAniFPS")
				{
					colorAniFPS = std::stof(value);
				}
				else if (key == "colorAniSmooth")
				{
					colorAniSmooth = std::stoi(value);
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

bool zCVobSound::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCZone::Archive(archiver, game))
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

bool zCVobSound::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCZone::Unarchive(archiver, game))
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

bool zCVobSoundDaytime::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCVobSound::Archive(archiver, game))
		return false;

	archiver->WriteGroupBegin("SoundDaytime");

	archiver->WriteFloat(ARC_ARGS(sndStartTime));
	archiver->WriteFloat(ARC_ARGS(sndEndTime));
	archiver->WriteString(ARC_ARGS(sndName2));

	archiver->WriteGroupEnd("SoundDaytime");

	return true;
}

bool zCVobSoundDaytime::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCVobSound::Unarchive(archiver, game))
		return false;

	archiver->ReadFloat(ARC_ARGS(sndStartTime));
	archiver->ReadFloat(ARC_ARGS(sndEndTime));
	archiver->ReadString(ARC_ARGS(sndName2));

	return true;
}

bool oCZoneMusic::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCZoneMusic::Archive(archiver, game))
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

bool oCZoneMusic::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCZoneMusic::Unarchive(archiver, game))
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

bool zCZoneVobFarPlane::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCZone::Archive(archiver, game))
		return false;

	archiver->WriteGroupBegin("ZoneVobFarPlane");

	archiver->WriteFloat(ARC_ARGS(vobFarPlaneZ));
	archiver->WriteFloat(ARC_ARGS(innerRangePerc));

	archiver->WriteGroupEnd("ZoneVobFarPlane");

	return true;
}

bool zCZoneVobFarPlane::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCZone::Unarchive(archiver, game))
		return false;

	archiver->ReadFloat(ARC_ARGS(vobFarPlaneZ));
	archiver->ReadFloat(ARC_ARGS(innerRangePerc));

	return true;
}

bool zCZoneZFog::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCZone::Archive(archiver, game))
		return false;

	archiver->WriteGroupBegin("ZoneZFog");

	archiver->WriteFloat(ARC_ARGS(fogRangeCenter));
	archiver->WriteFloat(ARC_ARGS(innerRangePerc));
	archiver->WriteColor(ARC_ARGS(fogColor));

	if (game >= GAME_GOTHIC2)
	{
		archiver->WriteBool(ARC_ARGS(fadeOutSky));
		archiver->WriteBool(ARC_ARGS(overrideColor));
	}

	archiver->WriteGroupEnd("ZoneZFog");

	return true;
}

bool zCZoneZFog::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCZone::Unarchive(archiver, game))
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

bool zCAIPlayer::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCAIBase::Archive(archiver, game))
		return false;

	return false;
}

bool zCAIPlayer::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCAIBase::Unarchive(archiver, game))
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

bool oCAniCtrl_Human::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCAIPlayer::Archive(archiver, game))
		return false;

	return false;
}

bool oCAniCtrl_Human::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCAIPlayer::Unarchive(archiver, game))
		return false;

	if (archiver->IsSavegame())
	{
		aiNpc = archiver->ReadObjectAs<oCNpc*>("aiNpc", game);

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

bool zCEventManager::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Archive(archiver, game))
		return false;

	return false;
}

bool zCEventManager::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Unarchive(archiver, game))
		return false;

	return false;
}

/*
	Waynet
*/

bool zCWayNet::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Archive(archiver, game))
		return false;
	
	if (game >= GAME_CHRISTMASEDITION)
	{
		archiver->WriteInt(ARC_ARGS(waynetVersion));
	}

	// Waypoints
	if (waynetVersion == 0)
	{
		int numWaypoints = 1;
			
		if (game >= GAME_SEPTEMBERDEMO)
		{
			numWaypoints = oldWaypointList.size();
		}

		archiver->WriteInt(ARC_ARGS(numWaypoints));

		if (game >= GAME_SEPTEMBERDEMO)
		{
			for (size_t i = 0; i < numWaypoints; i++)
			{
				zCVob* waypoint = oldWaypointList[i];

				std::string waypointKey = "waypoint" + std::to_string(i);

				std::string waypointLine = waypoint->vobName;

				if (dynamic_cast<zCVobWaypoint*>(waypoint) &&
					((zCVobWaypoint*)waypoint)->wayProp != 0)
				{
					waypointLine += " [";

					int wayProp = ((zCVobWaypoint*)waypoint)->wayProp;

					if (wayProp & WAYPROP_JUMP)
						waypointLine += "J";

					if (wayProp & WAYPROP_CLIMB)
						waypointLine += "C";

					if (wayProp & WAYPROP_SWIM)
						waypointLine += "S";

					if (wayProp & WAYPROP_DIVE)
						waypointLine += "D";

					if (wayProp & WAYPROP_FREE)
						waypointLine += "F";

					waypointLine += "]";
				}

				archiver->WriteString(waypointKey, waypointLine);
			}
		}
	}
	else if (waynetVersion == 1)
	{
		int numWaypoints = waypointList.size();
		archiver->WriteInt(ARC_ARGS(numWaypoints));

		for (size_t i = 0; i < numWaypoints; i++)
		{
			std::string waypointKey = "waypoint" + std::to_string(i);
			archiver->WriteObject(waypointKey, game, waypointList[i]);
		}
	}

	// Ways
	if (waynetVersion == 0)
	{
		int numWays = oldWayList.size();
		archiver->WriteInt(ARC_ARGS(numWays));

		for (size_t i = 0; i < numWays; i++)
		{
			zCOldWay way = oldWayList[i];

			std::string wayKey = "way" + std::to_string(i);

			if (way.left != nullptr && way.right != nullptr)
			{
				archiver->WriteString(wayKey, way.left->vobName + "/" + way.right->vobName);
			}
		}
	}
	else if (waynetVersion == 1)
	{
		int numWays = wayList.size();
		archiver->WriteInt(ARC_ARGS(numWays));

		for (size_t i = 0; i < numWays; i++)
		{
			std::string waylKey = "wayl" + std::to_string(i);
			archiver->WriteObject(waylKey, game, wayList[i].left);

			std::string wayrKey = "wayr" + std::to_string(i);
			archiver->WriteObject(wayrKey, game, wayList[i].right);
		}
	}

	return true;
}

bool zCWayNet::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Unarchive(archiver, game))
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

				std::string waypointLine;
				archiver->ReadString(waypointKey, waypointLine);

				std::string waypointName = waypointLine;
				if (waypointName.find(" ") != std::string::npos)
					waypointName = waypointName.substr(0, waypointName.find(" "));

				if (world->vobTable.find(waypointName) == world->vobTable.end())
				{
					LOG_WARN("Waypoint \"" + waypointName + "\" not found, skipping...");
					continue;
				}

				zCVob* waypoint = world->vobTable[waypointName]; // Sometimes it is a zCVob

				// Wayprop
				size_t sPos = waypointLine.find(" [");
				if (dynamic_cast<zCVobWaypoint*>(waypoint) &&
					sPos != std::string::npos)
				{
					std::string wayPropStr = waypointLine.substr(sPos + 2, waypointLine.find("]") - sPos - 2);

					int wayProp = 0;

					for (size_t i = 0; i < wayPropStr.size(); i++)
					{
						char wayPropC = wayPropStr[i];

						if (wayPropC == 'J')
							wayProp |= WAYPROP_JUMP;
						else if (wayPropC == 'C')
							wayProp |= WAYPROP_CLIMB;
						else if (wayPropC == 'S')
							wayProp |= WAYPROP_SWIM;
						else if (wayPropC == 'D')
							wayProp |= WAYPROP_DIVE;
						else if (wayPropC == 'F')
							wayProp |= WAYPROP_FREE;
					}

					((zCVobWaypoint*)waypoint)->wayProp = (WAYPROP)wayProp;
				}

				oldWaypointList[i] = waypoint;
			}
		}
		else if (waynetVersion == 1)
		{
			waypointList.resize(numWaypoints);

			for (size_t i = 0; i < numWaypoints; i++)
			{
				std::string waypointKey = "waypoint" + std::to_string(i);
				zCWaypoint* waypoint = archiver->ReadObjectAs<zCWaypoint*>(waypointKey, game);

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
				std::string wayLine;
				archiver->ReadString(wayKey, wayLine);

				size_t sPos = wayLine.find("/");

				if (sPos == std::string::npos)
				{
					LOG_WARN("Invalid way format \"" + wayLine + "\"");
					continue;
				}

				std::string waylName = wayLine.substr(0, sPos);
				std::string wayrName = wayLine.substr(sPos + 1);

				// Find the waypoints
				if (world->vobTable.find(waylName) == world->vobTable.end())
				{
					LOG_WARN("Waypoint \"" + waylName + "\" not found, skipping...");
					continue;
				}

				if (world->vobTable.find(wayrName) == world->vobTable.end())
				{
					LOG_WARN("Waypoint \"" + wayrName + "\" not found, skipping...");
					continue;
				}

				zCVob* wayl = world->vobTable[waylName];
				zCVob* wayr = world->vobTable[wayrName];

				zCOldWay way;
				way.left	= wayl;
				way.right	= wayr;
				oldWayList[i] = way;
			}
		}
		else if (waynetVersion == 1)
		{
			wayList.resize(numWays);

			for (size_t i = 0; i < numWays; i++)
			{
				std::string waylKey = "wayl" + std::to_string(i);
				zCWaypoint* wayl = archiver->ReadObjectAs<zCWaypoint*>(waylKey, game);

				std::string wayrKey = "wayr" + std::to_string(i);
				zCWaypoint* wayr = archiver->ReadObjectAs<zCWaypoint*>(wayrKey, game);

				zCWay way;
				way.left	= wayl;
				way.right	= wayr;
				wayList[i] = way;
			}
		}
	}

	return true;
}

bool zCWayNet::SaveWaynet(FileStream* file)
{
	file->WriteLine("Waynet {", "\n");

	for (size_t i = 0; i < oldWaypointList.size(); i++)
	{
		zCVob* waypoint = oldWaypointList[i];

		std::string line = "Waypoint (" + waypoint->vobName + ") ";

		if (dynamic_cast<zCVobWaypoint*>(waypoint) && 
			((zCVobWaypoint*)waypoint)->wayProp != 0)
		{
			line += " [";

			int wayProp = ((zCVobWaypoint*)waypoint)->wayProp;

			if (wayProp & WAYPROP_JUMP)
				line += "J";

			if (wayProp & WAYPROP_CLIMB)
				line += "C";

			if (wayProp & WAYPROP_SWIM)
				line += "S";

			if (wayProp & WAYPROP_DIVE)
				line += "D";

			if (wayProp & WAYPROP_FREE)
				line += "F";

			line += "]";
		}

		file->WriteLine(line, "\n");
	}

	for (size_t i = 0; i < oldWayList.size(); i++)
	{
		zCOldWay way = oldWayList[i];

		if (way.left != nullptr && way.right != nullptr)
		{
			file->WriteLine("Way (" + way.left->vobName + ") (" + way.right->vobName + ")", "\n");
		}
	}

	file->WriteLine("}", "\n");

	return true;
}

bool zCWayNet::LoadWaynet(FileStream* file)
{
	std::string line;

	while (file->ReadLine(line))
	{	
		if (line.find("Waypoint (") == 0)
		{
			std::string waypointName = line.substr(10, line.find(")", 10) - 10);

			if (world->vobTable.find(waypointName) == world->vobTable.end())
			{
				LOG_WARN("Waypoint \"" + waypointName + "\" not found, skipping...");
				continue;
			}

			zCVob* waypoint = world->vobTable[waypointName]; // Sometimes it is a zCVob

			// Wayprop
			size_t sPos = line.find(" [");
			if (dynamic_cast<zCVobWaypoint*>(waypoint) && 
				sPos != std::string::npos)
			{
				std::string wayPropStr = line.substr(sPos + 2, line.find("]") - sPos - 2);

				int wayProp = 0;

				for (size_t i = 0; i < wayPropStr.size(); i++)
				{
					char wayPropC = wayPropStr[i];

					if (wayPropC == 'J')
						wayProp |= WAYPROP_JUMP;
					else if (wayPropC == 'C')
						wayProp |= WAYPROP_CLIMB;
					else if (wayPropC == 'S')
						wayProp |= WAYPROP_SWIM;
					else if (wayPropC == 'D')
						wayProp |= WAYPROP_DIVE;
					else if (wayPropC == 'F')
						wayProp |= WAYPROP_FREE;
				}

				((zCVobWaypoint*)waypoint)->wayProp = (WAYPROP)wayProp;
			}

			oldWaypointList.push_back(waypoint);
		}
		else if (line.find("Way (") == 0)
		{
			size_t mPos = line.find(") (", 5);
			std::string waylName = line.substr(5, mPos - 5);

			mPos += 3;
			std::string wayrName = line.substr(mPos, line.find(")", mPos) - mPos);

			// Find the waypoints
			if (world->vobTable.find(waylName) == world->vobTable.end())
			{
				LOG_WARN("Waypoint \"" + waylName + "\" not found, skipping...");
				continue;
			}

			if (world->vobTable.find(wayrName) == world->vobTable.end())
			{
				LOG_WARN("Waypoint \"" + wayrName + "\" not found, skipping...");
				continue;
			}

			zCVob* wayl = world->vobTable[waylName];
			zCVob* wayr = world->vobTable[wayrName];

			zCOldWay way;
			way.left	= wayl;
			way.right	= wayr;
			oldWayList.push_back(way);
		}
		else
		{
			break;
		}
	}

	return true;
}

bool zCWaypoint::Archive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Archive(archiver, game))
		return false;

	archiver->WriteString(ARC_ARGS(wpName));
	archiver->WriteInt(ARC_ARGS(waterDepth));
	archiver->WriteBool(ARC_ARGS(underWater));
	archiver->WriteVec3(ARC_ARGS(position));
	archiver->WriteVec3(ARC_ARGS(direction));

	return true;
}

bool zCWaypoint::Unarchive(zCArchiver* archiver, GAME game)
{
	if (!zCObject::Unarchive(archiver, game))
		return false;

	archiver->ReadString(ARC_ARGS(wpName));
	archiver->ReadInt(ARC_ARGS(waterDepth));
	archiver->ReadBool(ARC_ARGS(underWater));
	archiver->ReadVec3(ARC_ARGS(position));
	archiver->ReadVec3(ARC_ARGS(direction));

	return true;
}
