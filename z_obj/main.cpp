
#include "vdfs.hpp"

int main(int argc, char* argv[])
{
	VDFS::FileSystem fs;
	
	fs.InitializeDirectory(L"D:\\Gothic\\Gothic_demo5");
	//fs.InitializeDirectory(L"D:\\SteamLibrary\\steamapps\\common\\Gothic Sequel");

	//VDFS::FileStream file;
	//
	//file.Open("D:\\Gothic\\Gothic_demo3\\config.d", 'r');
	//
	//std::string test;
	//
	//
	//while (file.ReadLine(test))
	//{
	//	std::cout << test << std::endl;
	//
	//}

	fs.PrintDebug();


	return 0;
}