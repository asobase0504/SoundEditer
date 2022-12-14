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
#include <assert.h>
#include "soundmanager.h"
#include "sound.h"

// ファイル管理
#include "nlohmann/json.hpp"
#include <fstream>
#include "letter.h"
#include "file.h"

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
	m_apSourceVoice.clear();			// ソースボイス
	m_apDataAudio.clear();				// オーディオデータ
	m_aSizeAudio.clear();				// オーディオデータサイズ

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
// 作成
//--------------------------------------
CSound * CSoundManager::SoundCreate(std::string inName)
{
	return SoundCreate(m_pathName[inName]);
}

//--------------------------------------
// 作成
//--------------------------------------
CSound * CSoundManager::SoundCreate(int inName)
{
	if (m_pSound[inName] != nullptr)
	{
		Init();
		m_pSound[inName] = new CSound;
		LoadPath();
		m_pSound[inName]->Init();
	}

	return m_pSound[inName];
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
// 初期化
// 概要：機能としてあるものをサンプルとして
//--------------------------------------
HRESULT CSoundManager::Init()
{
	LoadPath();		//音程の読込

	//確認可能
	HRESULT hr;		//HRESULT型の変数

	//--------------------------------------
	// COMライブラリの初期化
	//--------------------------------------
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

	/*サウンドデータサウンドに持たせることを目標に*/

	// サウンドデータの初期化
	for (int nCntSound = 0; nCntSound < (int)m_path.size(); nCntSound++)
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
		hFile = CreateFile((LPCSTR)GetPath(nCntSound).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
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
		buffer.LoopCount = GetLoop(nCntSound);

		// オーディオバッファの登録
		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}

	return S_OK;
}

//--------------------------------------
//終了
//--------------------------------------
void CSoundManager::Uninit()
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < (int)m_apSourceVoice.size(); nCntSound++)
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
// 更新
// 概要:機能としてあるものをサンプルとして確認可能
//--------------------------------------
void CSoundManager::Update()
{
	//音量変更
	//SoundVolume(m_type);

	//サウンドフェードイン
	//FadeInSound(m_type);

	//サウンドフェードアウト
	//FadeOutSound(m_type);

	//ピッチ操作
	//ShiftRate(m_type);
}

//--------------------------------------
//全てのサウンド停止
//--------------------------------------
void CSoundManager::Stop()
{
	for (int nCntSound = 0; nCntSound < (int)m_pSound.size(); nCntSound++)
	{
		if (m_pSound[nCntSound]->GetSourceVoice() == nullptr)
		{
			continue;
		}

		// 一時停止
		m_pSound[nCntSound]->GetSourceVoice()->Stop(0);
	}
}

//--------------------------------------
// 音量の設定
//--------------------------------------
void CSoundManager::SetVolume(std::string type, float fVolume)
{
	SetVolume(m_pathName[type], fVolume);
}

//--------------------------------------
// 音量の設定
//--------------------------------------
void CSoundManager::SetVolume(int type, float fVolume)
{
	m_pSound[type]->GetSourceVoice()->SetVolume(fVolume);
}

//--------------------------------------
// ピッチの設定
//--------------------------------------
void CSoundManager::SetRate(std::string type, float rate)
{
	SetRate(m_pathName[type], rate);
}

//--------------------------------------
// ピッチの設定
//--------------------------------------
void CSoundManager::SetRate(int type, float rate)
{
	m_apSourceVoice[type]->SetFrequencyRatio(rate);
}

//--------------------------------------
// フェードイン
//--------------------------------------
void CSoundManager::FadeInSound(std::string type)
{
	FadeInSound(m_pathName[type]);
}

//--------------------------------------
// フェードイン
//--------------------------------------
void CSoundManager::FadeInSound(int type)
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
// フェードアウト
//--------------------------------------
void CSoundManager::FadeOutSound(std::string type)
{
	FadeOutSound(m_pathName[type]);
}

//--------------------------------------
// フェードアウト
//--------------------------------------
void CSoundManager::FadeOutSound(int type)
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
// セーブ
//--------------------------------------
void CSoundManager::Save()
{
	nlohmann::json Savefile;

	//Savefile["VOLUME"] = m_volume;
	//Savefile["PITCH"] = m_frate;
	//Savefile["SOUND_TYPE"] = m_type;
	//Savefile["SOUND_NUM"] = m_Switch;

	SaveJson(Savefile, "Data/JSON/SoundData.json");
}

//--------------------------------------
// 再生
//--------------------------------------
HRESULT CSoundManager::Play(std::string inName)
{
	return Play(m_pathName[inName]);
}

//--------------------------------------
// 再生
//--------------------------------------
HRESULT CSoundManager::Play(int inName)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[inName];
	buffer.pAudioData = m_apDataAudio[inName];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_path[inName].isLoop;

	// 状態取得
	m_apSourceVoice[inName]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
	 // 一時停止
		m_apSourceVoice[inName]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[inName]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_apSourceVoice[inName]->SubmitSourceBuffer(&buffer);

	// 再生
	m_apSourceVoice[inName]->Start(0);

	return S_OK;
}

//--------------------------------------
//サウンド停止
//--------------------------------------
void CSoundManager::Stop(std::string type)
{
	Stop(m_pathName[type]);
}

//--------------------------------------
//サウンド停止
//--------------------------------------
void CSoundManager::Stop(int type)
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
//サウンドアップ
//--------------------------------------
void CSoundManager::SetFadeIn(std::string type)
{
	SetFadeIn(m_pathName[type]);
}

//--------------------------------------
//サウンドアップ
//--------------------------------------
void CSoundManager::SetFadeIn(int type)
{
	m_pSound[type]->Setvol(0.0f);
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
