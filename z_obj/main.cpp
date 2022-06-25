
#include "zen_vdfs.h"
#include "zen_base.h"

int main(int argc, char* argv[])
{
	ZEN::FileSystem fs;
	
	//fs.InitializeDirectory(L"D:\\Gothic\\Gothic_demo5");
	//fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic Sequel");
	//fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic");

	//ZEN::FileStream* f = fs.OpenFile("WORLD.zen", true);
	ZEN::FileStream* f = new ZEN::FileStream();
	f->Open("D:\\Users\\vaana\\source\\repos\\pConvert\\res\\SURFACE_UNCOMPILED.ZEN", 'r');

	ZEN::zCArchiver archiver;

	archiver.Read(f);

	return 0;
}