//------------------------------------------------------------------------------
//
//サウンドマネージャー
//Author::TAKANO
//
//------------------------------------------------------------------------------

//--------------------------------------
//インクルードファイル
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
//定数定義
//--------------------------------------
const float BAN_BGM = 0.0007f;
const float TRACK_PITH = 0.001f;
const float MAX_RATE = 11.0f;
const float UP_DOWN_TRACK = 0.01f;

//--------------------------------------
//コンストラクタ
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
//デストラクタ
//--------------------------------------
CSoundManager::~CSoundManager()
{
}

//--------------------------------------
//初期化
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
//終了
//--------------------------------------
void CSoundManager::Uninit()
{
	CSound::Uninit();
}

//--------------------------------------
//更新　　以下概要
//機能としてあるものをサンプルとして
//確認可能
//--------------------------------------
void CSoundManager::Update()
{
	//曲替え
	ChangeSound();

	//音量変更
	SoundVolume();

	//サウンドフェードイン
	FadeInSound();

	//サウンドフェードアウト
	FadeOutSound();

	//ピッチ操作
	ShiftRate();

	//サウンドのパラメーター保存
	SoundParameter();
}

//--------------------------------------
//音楽使用管理
//--------------------------------------
void CSoundManager::ChangeSound()
{
	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_L))
	{ //曲の入れ替え

		m_Switch = (m_Switch + 1) % 12;

		Select();

		if (m_Switch >= 11)
		{
			m_Switch = 0;
		}
	}
}

//--------------------------------------
//音量管理
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
//サウンドフェードイン
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
//サウンドフェードアウト
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
//ピッチ操作
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
//サウンドパスのロード
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

			// パスの情報を代入
			SPath path;
			path.path = UTF8toSjis(Loadfile["SOUND"][i]["PATH"]);	// パスを代入
			path.isLoop = Loadfile["SOUND"][i]["LOOP"];	// 繰り返すか否かを代入
			m_path.emplace(i, path);
		}
	}
}

//--------------------------------------
//サウンドロード
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
	//	{// 呼ぶ値が存在するか
	//		assert(false);
	//	}

	//	// 値の出力
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
//サウンドセーブ
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
//サウンドセーブ
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
//サウンド選択
//--------------------------------------
void CSoundManager::Select()
{
	switch (m_Switch)
	{
	case 0:

		/*変更なし*/

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
//サウンドアップ
//--------------------------------------
void CSoundManager::FadeIn()
{
	fvol = 0.0f;
	m_Fadetype = TYPE_IN;
	m_Fade_Ok = false;
}

//--------------------------------------
//サウンドダウン
//--------------------------------------
void CSoundManager::FadeOut()
{
	m_Fadetype = TYPE_OUT;
	m_Fade_Ok = false;
}
