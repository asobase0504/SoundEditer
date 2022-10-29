//------------------------------------------------------------------------------
//
//�T�E���h�}�l�[�W���[
//Author::TAKANO
//
//------------------------------------------------------------------------------

//--------------------------------------
//�C���N���[�h�t�@�C��
//--------------------------------------
#include "application.h"
#include "input.h"
#include <assert.h>
#include "soundmanager.h"
#include "sound.h"
#include <fstream>
#include "nlohmann/json.hpp"
#include "letter.h"
#include "file.h"
#include <unordered_map>

//--------------------------------------
//�萔��`
//--------------------------------------
const float BAN_BGM = 0.0007f;
const float TRACK_PITH = 0.001f;
const float MAX_RATE = 11.0f;
const float UP_DOWN_TRACK = 0.01f;

//--------------------------------------
//�R���X�g���N�^
//--------------------------------------
CSoundManager::CSoundManager()
{
	fvol = 0.0f;
	frate = 0.0f;
	m_Switch = 0;
	m_nCount = 0;
	m_Fade_Ok = false;
	m_Fadetype = TYPE_NONE;
}

//--------------------------------------
//�f�X�g���N�^
//--------------------------------------
CSoundManager::~CSoundManager()
{
}

//--------------------------------------
//������
//--------------------------------------
HRESULT CSoundManager::Init(HWND hWnd)
{
	fvol = 0.0f;
	frate = 1.0f;
	m_Switch = 0;
	LoadPath();
	CSound::Init(hWnd);
	return S_OK;
}

//--------------------------------------
//�I��
//--------------------------------------
void CSoundManager::Uninit()
{
	CSound::Uninit();
}

//--------------------------------------
//�X�V�@�@�ȉ��T�v
//�@�\�Ƃ��Ă�����̂��T���v���Ƃ���
//�m�F�\
//--------------------------------------
void CSoundManager::Update()
{
	//�ȑւ�
	ChangeSound();

	//���ʕύX
	SoundVolume();

	//�T�E���h�t�F�[�h�C��
	FadeInSound();

	//�T�E���h�t�F�[�h�A�E�g
	FadeOutSound();

	//�s�b�`����
	ShiftRate();

	//�T�E���h�̃p�����[�^�[�ۑ�
	SoundParameter();
}

//--------------------------------------
//���y�g�p�Ǘ�
//--------------------------------------
void CSoundManager::ChangeSound()
{
	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_L))
	{ //�Ȃ̓���ւ�

		m_Switch = (m_Switch + 1) % 12;

		Select();

		if (m_Switch >= 11)
		{
			m_Switch = 0;
		}
	}
}

//--------------------------------------
//���ʊǗ�
//--------------------------------------
void CSoundManager::SoundVolume()
{
	if (CApplication::GetInput()->GetkeyboardPress(DIK_UP))
	{
		fvol += UP_DOWN_TRACK;
		SetVolume(m_type, fvol);
	}

	if (CApplication::GetInput()->GetkeyboardPress(DIK_DOWN))
	{
		fvol -= UP_DOWN_TRACK;
		SetVolume(m_type, fvol);
	}

	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_LEFT))
	{
		m_Fadetype = TYPE_IN;
		m_Fade_Ok = false;
	}

	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_RIGHT))
	{
		m_Fadetype = TYPE_OUT;
		m_Fade_Ok = false;
	}
}

//--------------------------------------
//�T�E���h�t�F�[�h�C��
//--------------------------------------
void CSoundManager::FadeInSound()
{
	if (fvol <= 1.0f &&
		!m_Fade_Ok &&
		m_Fadetype == TYPE_IN)
	{
		fvol += BAN_BGM;
		SetVolume(m_type, fvol);
	}
	else
	{
		if (m_Fadetype == TYPE_IN)
		{
			m_Fade_Ok = true;
			m_Fadetype = TYPE_NONE;
		}
	}
}

//--------------------------------------
//�T�E���h�t�F�[�h�A�E�g
//--------------------------------------
void CSoundManager::FadeOutSound()
{
	if (fvol >= 0.0f && 
		!m_Fade_Ok &&
		m_Fadetype == TYPE_OUT)
	{
		fvol -= BAN_BGM;
		SetVolume(m_type, fvol);
	}
	else
	{
		if (m_Fadetype == TYPE_OUT)
		{
			m_Fade_Ok = true;
			m_Fadetype = TYPE_NONE;
		}
	}
}

//--------------------------------------
//�s�b�`����
//--------------------------------------
void CSoundManager::ShiftRate()
{
	if (CApplication::GetInput()->GetkeyboardPress(DIK_O))
	{
		frate += TRACK_PITH;
		CSound::SetRate(m_type, frate);

		if (frate >= MAX_RATE)
		{
			frate = MAX_RATE;
		}
	}

	if (CApplication::GetInput()->GetkeyboardPress(DIK_P))
	{
		frate -= TRACK_PITH;
		CSound::SetRate(m_type, frate);

		if (frate <= -MAX_RATE)
		{
			frate = -MAX_RATE;
		}
	}
}

//--------------------------------------
//�T�E���h�p�X�̃��[�h
//--------------------------------------
void CSoundManager::LoadPath()
{
	std::ifstream ifs("Data/JSON/SoundPath.json");
	nlohmann::json Loadfile;
	ifs >> Loadfile;

	if (ifs)
	{
		for (int i = 0;i < (int)Loadfile["SOUND"].size();i++)
		{
			m_pathName.emplace(UTF8toSjis(Loadfile["SOUND"][i]["NAME"]), i);

			// �p�X�̏�����
			SPath path;
			path.path = UTF8toSjis(Loadfile["SOUND"][i]["PATH"]);	// �p�X����
			path.isLoop = Loadfile["SOUND"][i]["LOOP"];	// �J��Ԃ����ۂ�����
			m_path.emplace(i, path);
		}
	}
}

//--------------------------------------
//�T�E���h���[�h
//--------------------------------------
void CSoundManager::Load()
{
	//std::string inString = "MAIN";

	//std::ifstream ifs("Data/JSON/SoundDate.json");
	//nlohmann::json Loadfile;
	//ifs >> Loadfile;

	//if (ifs)
	//{
	//	if (Loadfile["SOUND"].count(inString) == 0)
	//	{// �ĂԒl�����݂��邩
	//		assert(false);
	//	}

	//	// �l�̏o��
	//	SData soundData;
	//	soundData.volume = Loadfile["SOUND"][inString]["VOLUME"];
	//	soundData.rate = Loadfile["SOUND"][inString]["PITCH"];

	//	data.emplace(inString, soundData);

	//	fvol = Loadfile["SOUND"]["TYPE1"];
	//	frate = Loadfile["PITCH"];
	//	m_type = Loadfile["SOUND_TYPE"];
	//	m_Switch = Loadfile["SOUND_NUM"];

	//	Select();

	//	CSound::SetRate(m_type, frate);
	//}
}

//--------------------------------------
//�T�E���h�Z�[�u
//--------------------------------------
void CSoundManager::Save()
{
	nlohmann::json Savefile;

	Savefile["VOLUME"] = fvol;
	Savefile["PITCH"] = frate;
	Savefile["SOUND_TYPE"] = m_type;
	Savefile["SOUND_NUM"] = m_Switch;

	SaveJson(Savefile, "Data/JSON/SoundData.json");
}

//--------------------------------------
//�T�E���h�Z�[�u
//--------------------------------------
void CSoundManager::SoundParameter()
{
	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_N))
	{
		Save();
	}

	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_M))
	{
		Load();
	}
}

//--------------------------------------
//�T�E���h�I��
//--------------------------------------
void CSoundManager::Select()
{
	switch (m_Switch)
	{
	case 0:

		/*�ύX�Ȃ�*/

		break;

	case 1:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_BATTLEBAN);
		m_type = SOUND_BGM_BATTLEBAN;
		SoundVolume();
		break;

	case 2:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_LAST_MEETING);
		m_type = SOUND_BGM_LAST_MEETING;
		SoundVolume();
		break;

	case 3:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_RESULT_BGM);
		m_type = SOUND_BGM_RESULT_BGM;
		SoundVolume();
		break;

	case 4:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_VIRTUAL);
		m_type = SOUND_BGM_VIRTUAL;
		SoundVolume();
		break;

	case 5:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_BATTLEMEETING_VER_2);
		m_type = SOUND_BGM_BATTLEMEETING_VER_2;
		SoundVolume();

		break;

	case 6:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_NO_NAME);
		m_type = SOUND_BGM_NO_NAME;
		SoundVolume();
		break;

	case 7:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_CHALLENGE_TO_TOMORROW);
		m_type = SOUND_BGM_CHALLENGE_TO_TOMORROW;
		SoundVolume();
		break;

	case 8:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_BATTLEMEETING);
		m_type = SOUND_BGM_BATTLEMEETING;
		SoundVolume();
		break;

	case 9:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_HEART);
		m_type = SOUND_BGM_HEART;
		SoundVolume();
		break;

	case 10:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_DRUM_VOICE);
		m_type = SOUND_BGM_DRUM_VOICE;
		SoundVolume();
		break;

	case 11:
		FadeIn();
		CSound::Stop();
		CSound::Play(SOUND_BGM_RANKING);
		m_type = SOUND_BGM_RANKING;
		SoundVolume();
		break;

	default:
		assert(false);
		break;
	}
}

//--------------------------------------
//�T�E���h�A�b�v
//--------------------------------------
void CSoundManager::FadeIn()
{
	fvol = 0.0f;
	m_Fadetype = TYPE_IN;
	m_Fade_Ok = false;
}

//--------------------------------------
//�T�E���h�_�E��
//--------------------------------------
void CSoundManager::FadeOut()
{
	m_Fadetype = TYPE_OUT;
	m_Fade_Ok = false;
}
