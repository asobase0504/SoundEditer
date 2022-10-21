//============================
//
// ファイル設定
// Author:hamada ryuuga
//
//============================
#include "file.h"
#include "letter.h"

void SaveJson(nlohmann::json inJson, std::string inFile)
{
	auto jobj = inJson.dump();
	std::ofstream writing_file;
	writing_file.open(inFile, std::ios::out);
	writing_file << jobj << std::endl;
	writing_file.close();
}
