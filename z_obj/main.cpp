
#define GOTHICLIB_ALL
#include "gothiclib.h"

#include <iostream>

void logfunc(GothicLib::Log::MESSAGE_LEVEL level, std::string message)
{
	if (level == GothicLib::Log::MESSAGE_DEBUG)
		std::cout << "\033[0m" << "DEBUG : " << message << std::endl;
	else if (level == GothicLib::Log::MESSAGE_INFO)
		std::cout << "\033[0;32m" << "INFO  : " << message << "\033[0m" << std::endl;
	else if (level == GothicLib::Log::MESSAGE_WARN)
		std::cout << "\033[0;33m" << "WARN  : " << message << "\033[0m" << std::endl;
	else if (level == GothicLib::Log::MESSAGE_ERROR)
		std::cout << "\033[0;31m" << "ERROR : " << message << "\033[0m" << std::endl;
}

using namespace GothicLib;

int main(int argc, char* argv[])
{
	Log::SetCallback(&logfunc);

	

	ZenGin::FileSystem fs;
	
	//fs.InitializeDirectory(L"D:\\Gothic\\Gothic_demo5");
	//fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic Sequel");
	//if (!fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic Original"))
	//	return 1;

	//FileStream* f = fs.OpenFile("WORLD.zen", true);
	FileStream* f = new FileStream();
	//f->Open("D:\\Users\\vaana\\source\\repos\\pConvert\\res\\TESTLEVEL_UNCOMPILED_0.64b.ZEN", 'r');
	f->Open("D:\\Users\\vaana\\source\\repos\\pConvert\\res\\SURFACE_UNCOMPILED.ZEN", 'r');
	//f->Open("D:\\Users\\vaana\\source\\repos\\pConvert\\res\\NEWWORLD_PART_CITY_01_UNCOMPILED.ZEN", 'r');
	//f->Open("D:\\SteamLibrary\\steamapps\\common\\Gothic Original\\saves\\current\\WORLD.SAV", 'r');

	ZenGin::zCArchiver archiver;

	archiver.Read(f);
	

	//GE::FileStream stream;

	//stream.Open("D:\\Temp\\G3_DATA\\Projects_compiled\\Projects.prj", 'r');
	


	return 0;
}