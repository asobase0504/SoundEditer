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
	m_pXAudio2 = nullptr;				// XAudio2オブジェクトへのインターフェイス
	m_pMasteringVoice = nullptr;		// マスターボイス
	m_apSourceVoice[SOUND_MAX] = {};	// ソースボイス
	m_apDataAudio[SOUND_MAX] = {};		// オーディオデータ
	m_aSizeAudio[SOUND_MAX] = {};		// オーディオデータサイズ

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
//--------------------------------------
CSound * CSoundManager::SoundCreate(SOUND_TYPE type)
{
	if (m_pSound[type] != nullptr)
	{
		Init();
		m_pSound[type] = new CSound;
		LoadPath();
		m_pSound[type]->Init();
	}

	return m_pSound[type];
}

//--------------------------------------
//終了
//--------------------------------------
CSoundManager * CSoundManager::ManagerCreate()
{
	CSoundManager* pManager = new CSoundManager;

	if (pManager != nullptr)
	{
		pManager->Init();
	}

	return pManager;
}

//--------------------------------------
//更新　　以下概要
//機能としてあるものをサンプルとして
HRESULT CSoundManager::Init()
{
	m_Switch = 0;		//セレクト時に使用する変数の初期化
	LoadPath();			//音程の読込
//確認可能
	HRESULT hr;			//HRESULT型の変数
//--------------------------------------
	// COMライブラリの初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(hr))
	{
		// COMライブラリの終了処理
		CoUninitialize();

		// プログラムを緊急で止める
		assert(false);

		return E_FAIL;
	}

	// マスターボイスの生成
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr))
	{
		if (m_pXAudio2)
		{
			// XAudio2オブジェクトの開放
			m_pXAudio2->Release();
			m_pXAudio2 = nullptr;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		// プログラムを緊急で止める
		assert(false);

		return E_FAIL;
	}

	CSoundManager* manager = CApplication::GetSoundManager();

	// サウンドデータの初期化
	for (int nCntSound = 0; nCntSound < SOUND_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile((LPCSTR)manager->GetPath(nCntSound).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			//プログラムを緊急で止める
			assert(false);
			return HRESULT_FROM_WIN32(GetLastError());
		}
		if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// ファイルポインタを先頭に移動
			//プログラムを緊急で止める
			assert(false);
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			//プログラムを緊急で止める
			assert(false);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if (FAILED(hr))
		{
			//プログラムを緊急で止める
			assert(false);
			return S_FALSE;
		}
		if (dwFiletype != 'EVAW')
		{
			//プログラムを緊急で止める
			assert(false);
			return S_FALSE;
		}

		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			//プログラムを緊急で止める
			assert(false);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if (FAILED(hr))
		{
			//プログラムを緊急で止める
			assert(false);
			return S_FALSE;
		}

		// オーディオデータ読み込み
		hr = CheckChunk(hFile, 'atad', &m_aSizeAudio[nCntSound], &dwChunkPosition);
		if (FAILED(hr))
		{
			//プログラムを緊急で止める
			assert(false);
			return S_FALSE;
		}
		m_apDataAudio[nCntSound] = (BYTE*)malloc(m_aSizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, m_apDataAudio[nCntSound], m_aSizeAudio[nCntSound], dwChunkPosition);
		if (FAILED(hr))
		{
			//プログラムを緊急で止める
			assert(false);
			return S_FALSE;
		}

		// ファイルをクローズ
		CloseHandle(hFile);

		// ソースボイスの生成
		hr = m_pXAudio2->CreateSourceVoice(&m_apSourceVoice[nCntSound], &(wfx.Format));
		if (FAILED(hr))
		{
			//プログラムを緊急で止める
			assert(false);
			return S_FALSE;
		}

		// バッファの値設定
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_aSizeAudio[nCntSound];
		buffer.pAudioData = m_apDataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = manager->GetLoop(nCntSound);

		// オーディオバッファの登録
		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}

	return S_OK;
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
	SoundVolume(m_type);

	//サウンドフェードイン
	FadeInSound(m_type);

	//サウンドフェードアウト
	FadeOutSound(m_type);

	//ピッチ操作
	ShiftRate(m_type);

	//サウンドのパラメーター保存
	SoundParameter();
}

//--------------------------------------
//音楽使用管理
//--------------------------------------
HRESULT CSoundManager::Play(SOUND_TYPE type)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[type];
	buffer.pAudioData = m_apDataAudio[type];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_path[type].isLoop;

	// 状態取得
	m_apSourceVoice[type]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
	 // 一時停止
		m_apSourceVoice[type]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[type]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_apSourceVoice[type]->SubmitSourceBuffer(&buffer);

	// 再生
	m_apSourceVoice[type]->Start(0);

	return S_OK;
}


//--------------------------------------
//サウンド停止
//--------------------------------------
void CSoundManager::Stop(SOUND_TYPE type)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	m_apSourceVoice[type]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
	 // 一時停止
		m_apSourceVoice[type]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[type]->FlushSourceBuffers();
	}
}

//--------------------------------------
//全てのサウンド停止
//--------------------------------------
void CSoundManager::Stop()
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound])
		{
			// 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);
		}
	}
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
void CSoundManager::SoundVolume(SOUND_TYPE type)
{
	if (CApplication::GetInput()->GetkeyboardPress(DIK_UP))
	{
		m_pSound[type]->volUp(UP_DOWN_TRACK);
		SetVolume(m_type, m_pSound[type]->Getvol());
	}

	if (CApplication::GetInput()->GetkeyboardPress(DIK_DOWN))
	{
		m_pSound[type]->volDown(UP_DOWN_TRACK);
		SetVolume(m_type, m_pSound[type]->Getvol());
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
void CSoundManager::FadeInSound(SOUND_TYPE type)
{
	if (m_pSound[type]->Getvol() <= 1.0f &&
		!m_Fade_Ok &&
		m_Fadetype == TYPE_IN)
	{
		m_pSound[type]->volUp(BAN_BGM);
		SetVolume(m_type, m_pSound[type]->Getvol());
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
void CSoundManager::FadeOutSound(SOUND_TYPE type)
{
	if (m_pSound[type]->Getvol() >= 0.0f &&
		!m_Fade_Ok &&
		m_Fadetype == TYPE_OUT)
	{
		m_pSound[type]->volDown(BAN_BGM);
		SetVolume(m_type, m_pSound[type]->Getvol());
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
void CSoundManager::ShiftRate(SOUND_TYPE type)
{
	if (CApplication::GetInput()->GetkeyboardPress(DIK_O))
	{
		m_pSound[type]->PitchUp(TRACK_PITH);
		SetRate(m_type, m_pSound[type]->Getrate());
	}

	if (CApplication::GetInput()->GetkeyboardPress(DIK_P))
	{
		m_pSound[type]->PitchDown(TRACK_PITH);
		SetRate(m_type, m_pSound[type]->Getrate());
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

	//	m_volume = Loadfile["SOUND"]["TYPE1"];
	//	m_frate = Loadfile["PITCH"];
	//	m_type = Loadfile["SOUND_TYPE"];
	//	m_Switch = Loadfile["SOUND_NUM"];

	//	Select();

	//	CSound::SetRate(m_type, m_frate);
	//}
}

//--------------------------------------
//サウンドセーブ
//--------------------------------------
void CSoundManager::Save()
{
	nlohmann::json Savefile;

	Savefile["VOLUME"] = m_volume;
	Savefile["PITCH"] = m_frate;
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
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_BATTLEBAN);
		m_type = SOUND_BGM_BATTLEBAN;
		SoundVolume(m_type);
		break;

	case 2:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_LAST_MEETING);
		m_type = SOUND_BGM_LAST_MEETING;
		SoundVolume(m_type);
		break;

	case 3:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_RESULT_BGM);
		m_type = SOUND_BGM_RESULT_BGM;
		SoundVolume(m_type);
		break;

	case 4:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_VIRTUAL);
		m_type = SOUND_BGM_VIRTUAL;
		SoundVolume(m_type);
		break;

	case 5:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_BATTLEMEETING_VER_2);
		m_type = SOUND_BGM_BATTLEMEETING_VER_2;
		SoundVolume(m_type);

		break;

	case 6:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_NO_NAME);
		m_type = SOUND_BGM_NO_NAME;
		SoundVolume(m_type);
		break;

	case 7:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_CHALLENGE_TO_TOMORROW);
		m_type = SOUND_BGM_CHALLENGE_TO_TOMORROW;
		SoundVolume(m_type);
		break;

	case 8:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_BATTLEMEETING);
		m_type = SOUND_BGM_BATTLEMEETING;
		SoundVolume(m_type);
		break;

	case 9:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_HEART);
		m_type = SOUND_BGM_HEART;
		SoundVolume(m_type);
		break;

	case 10:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_DRUM_VOICE);
		m_type = SOUND_BGM_DRUM_VOICE;
		SoundVolume(m_type);
		break;

	case 11:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_RANKING);
		m_type = SOUND_BGM_RANKING;
		SoundVolume(m_type);
		break;

	default:
		assert(false);
		break;
	}
}

//--------------------------------------
//サウンドアップ
//--------------------------------------
void CSoundManager::FadeIn(SOUND_TYPE type)
{
	m_pSound[type]->Setvol(0.0f);
	m_Fadetype = TYPE_IN;
	m_Fade_Ok = false;
}

//--------------------------------------
//サウンドダウン
//--------------------------------------
void CSoundManager::FadeOut(SOUND_TYPE type)
{
	m_Fadetype = TYPE_OUT;
	m_Fade_Ok = false;
}

//--------------------------------------
//終了
//--------------------------------------
void CSoundManager::Uninit()
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound])
		{
			// 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);

			// ソースボイスの破棄
			m_apSourceVoice[nCntSound]->DestroyVoice();
			m_apSourceVoice[nCntSound] = nullptr;

			// オーディオデータの開放
			free(m_apDataAudio[nCntSound]);
			m_apDataAudio[nCntSound] = nullptr;
		}
	}

	// マスターボイスの破棄
	m_pMasteringVoice->DestroyVoice();
	m_pMasteringVoice = nullptr;

	if (m_pXAudio2)
	{
		// XAudio2オブジェクトの開放
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}

	// COMライブラリの終了処理
	CoUninitialize();
}

//--------------------------------------
//音量設定
//--------------------------------------
void CSoundManager::SetVolume(SOUND_TYPE type, float fVolume)
{
	m_apSourceVoice[type]->SetVolume(fVolume);
}

//--------------------------------------
//ピッチ操作
//--------------------------------------
void CSoundManager::SetRate(SOUND_TYPE type, float rate)
{
	m_apSourceVoice[type]->SetFrequencyRatio(rate);
}

//--------------------------------------
//一塊の確認
//--------------------------------------
HRESULT CSoundManager::CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, nullptr) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, nullptr) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, nullptr) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, nullptr, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//--------------------------------------
//一塊の読み取り
//--------------------------------------
HRESULT CSoundManager::ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, nullptr) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
