
#include "zen_vdfs.h"
#include "zen_base.h"
//#include "ge_filesystem.h"

int main(int argc, char* argv[])
{
	ZEN::FileSystem fs;
	
	//fs.InitializeDirectory(L"D:\\Gothic\\Gothic_demo5");
	//fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic Sequel");
	//fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic Original");

	//ZEN::FileStream* f = fs.OpenFile("WORLD.zen", true);
	ZEN::FileStream* f = new ZEN::FileStream();
	//f->Open("D:\\Users\\vaana\\source\\repos\\pConvert\\res\\TESTLEVEL_UNCOMPILED_0.64b.ZEN", 'r');
	//f->Open("D:\\Users\\vaana\\source\\repos\\pConvert\\res\\SURFACE_UNCOMPILED.ZEN", 'r');
	//f->Open("D:\\Users\\vaana\\source\\repos\\pConvert\\res\\NEWWORLD_PART_CITY_01_UNCOMPILED.ZEN", 'r');
	f->Open("D:\\SteamLibrary\\steamapps\\common\\Gothic Original\\saves\\current\\WORLD.SAV", 'r');

	ZEN::zCArchiver archiver;

	archiver.Read(f);
	

	//GE::FileStream stream;

	//stream.Open("D:\\Temp\\G3_DATA\\Projects_compiled\\Projects.prj", 'r');



	return 0;
}