//============================
//
// �t�@�C���ݒ�w�b�^�[
// Author:hamada ryuuga
//
//============================
#ifndef _FILE_H_			// ���̃}�N����`������ĂȂ�������
#define _FILE_H_			// ��d�C���N���[�h�h�~�̃}�N����`

//=====================================
// �C���N���[�h
//=====================================
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "nlohmann/json.hpp"

#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <sstream>

#include "main.h"

//=====================================
// �v���g�^�C�v�錾
//=====================================
// ����֐�
void OutputStatus();
void SaveJson(nlohmann::json inJson, std::string inFile);
void LoodJson(const char* cUrl);

#endif
