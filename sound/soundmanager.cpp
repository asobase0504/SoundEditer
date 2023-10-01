//*****************************************************************************
//
// 音源マネージャー
// Author::TAKANO
// Author Yuda Kaito
//
//*****************************************************************************
//=============================================================================
// include
//=============================================================================
#include "../application.h"
#include <assert.h>
#include "soundmanager.h"
#include "sound.h"
#include "sound_distance.h"

// ファイル管理
#include <fstream>
#include "../letter.h"
#include "../file.h"

//=============================================================================
// 静的変数の宣言
//=============================================================================
CSoundManager* CSoundManager::m_instance = nullptr;
float CSoundManager::MASTER_VOLUME = 1.0f;
float CSoundManager::BGM_VOLUME = 1.0f;
float CSoundManager::SE_VOLUME = 1.0f;

//-----------------------------------------------------------------------------
// シングルトンの呼び出し
//-----------------------------------------------------------------------------
CSoundManager* CSoundManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new CSoundManager;
		m_instance->Init();
	}

	return m_instance;
}

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
CSoundManager::CSoundManager()
{
	m_pXAudio2 = nullptr;				// XAudio2オブジェクトへのインターフェイス
	m_pMasteringVoice = nullptr;		// マスターボイス
}

//-----------------------------------------------------------------------------
//デストラクタ
//-----------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Init()
{
	//--------------------------------------
	// COMライブラリの初期化
	//--------------------------------------
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	if (FAILED(XAudio2Create(&m_pXAudio2, 0)))
	{
		// COMライブラリの終了処理
		CoUninitialize();

		assert(false);

		return E_FAIL;
	}
	
	// マスターボイスの生成
	if (FAILED(m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice)))
	{
		if (m_pXAudio2)
		{
			// XAudio2オブジェクトの開放
			m_pXAudio2->Release();
			m_pXAudio2 = nullptr;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		assert(false);

		return E_FAIL;
	}

	LoadPath();		//音の読込

	return S_OK;
}

//-----------------------------------------------------------------------------
//終了
//-----------------------------------------------------------------------------
void CSoundManager::Uninit()
{
	// マスターボイスの破棄
	m_pMasteringVoice->DestroyVoice();
	m_pMasteringVoice = nullptr;

	if (m_pXAudio2 != nullptr)
	{
		// XAudio2オブジェクトの開放
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}

	// COMライブラリの終了処理
	CoUninitialize();
}

//-----------------------------------------------------------------------------
// 再生(生成)
//-----------------------------------------------------------------------------
CSound* CSoundManager::Play(std::string inTag)
{
	IXAudio2SourceVoice* sourceVoice;

	HRESULT result = m_pXAudio2->CreateSourceVoice(&sourceVoice, &(m_soundData[inTag].format));

	assert(SUCCEEDED(result));

	// オーディオバッファの登録
	sourceVoice->SubmitSourceBuffer(&m_soundData[inTag].buffer);

	CSound* sound = new CSound;
	sound->Init();
	sound->SetSourceVoice(sourceVoice);
	sound->Play();

	return sound;
}

CSoundDistance * CSoundManager::PlayDistance(std::string inTag, const D3DXVECTOR3& inPos)
{
	IXAudio2SourceVoice* sourceVoice;

	HRESULT result = m_pXAudio2->CreateSourceVoice(&sourceVoice, &(m_soundData[inTag].format));

	assert(SUCCEEDED(result));

	// オーディオバッファの登録
	sourceVoice->SubmitSourceBuffer(&m_soundData[inTag].buffer);

	CSoundDistance* sound = new CSoundDistance;
	sound->Init();
	sound->SetSourceVoice(sourceVoice);
	sound->Play(inPos);

	return sound;
}

//-----------------------------------------------------------------------------
// Path読込み
//-----------------------------------------------------------------------------
void CSoundManager::LoadPath()
{
	std::ifstream ifs("Data/JSON/SoundPath.json");
	nlohmann::json Loadfile;
	ifs >> Loadfile;

	if (ifs)
	{
		for (int i = 0; i < (int)Loadfile["SOUND"].size(); i++)
		{
			LoadSound(Loadfile["SOUND"][i]["TAG"], UTF8toSjis(Loadfile["SOUND"][i]["PATH"]), Loadfile["SOUND"][i]["LOOP"]);
		}
	}
}

//-----------------------------------------------------------------------------
// 音源のデータ読込み
//-----------------------------------------------------------------------------
HRESULT CSoundManager::LoadSound(std::string tag, std::string path,bool loop)
{
	HRESULT hr;

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
	hFile = CreateFile((LPCSTR)path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		assert(false);
		return HRESULT_FROM_WIN32(GetLastError());
	}
	if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
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

	BYTE* pDataAudio;
	DWORD sizeAudio;

	// オーディオデータ読み込み
	hr = CheckChunk(hFile, 'atad', &sizeAudio, &dwChunkPosition);
	if (FAILED(hr))
	{
		//プログラムを緊急で止める
		assert(false);
		return S_FALSE;
	}

	pDataAudio = (BYTE*)malloc(sizeAudio);
	hr = ReadChunkData(hFile, pDataAudio, sizeAudio, dwChunkPosition);
	if (FAILED(hr))
	{
		//プログラムを緊急で止める
		assert(false);
		return S_FALSE;
	}

	// ファイルをクローズ
	CloseHandle(hFile);

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = sizeAudio;
	buffer.pAudioData = pDataAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	if (loop)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else
	{
		buffer.LoopCount = 0;
	}

	m_soundData[tag].buffer = buffer;
	m_soundData[tag].format = wfx.Format;

	return S_OK;
}

//-----------------------------------------------------------------------------
// 一塊の確認
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// 一塊の読み取り
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
