//---------------------------------------------------------
//
//サウンド
//Author::TAKANO
//
//---------------------------------------------------------
#ifndef _SOUND_H_
#define _SOUND_H_

//--------------------------------------
//インクルードファイル
//--------------------------------------
#include"d3dx9.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "xaudio2.h"
#include <map>

//--------------------------------------
//クラス定義
//サウンド関連
//--------------------------------------
class CSound
{
public:
	struct PARAM
	{//サウンドパラメーターの構造体
		char *m_pFilename;	// ファイル名
		int m_nCntLoop;		// ループカウント
	};

	enum SOUND_TYPE
	{
		SOUND_NONE = -1,					// 使用されていない状態
		SOUND_BGM_BATTLEBAN,				// BGM 0
		SOUND_BGM_LAST_MEETING,				// BGM 1
		SOUND_BGM_RESULT_BGM,				// BGM 2
		SOUND_BGM_VIRTUAL,					// BGM 3
		SOUND_BGM_BATTLEMEETING_VER_2,		// BGM 4
		SOUND_BGM_NO_NAME,					// BGM 5
		SOUND_BGM_CHALLENGE_TO_TOMORROW,	// BGM 6
		SOUND_BGM_BATTLEMEETING,			// BGM 7
		SOUND_BGM_HEART,					// BGM 8
		SOUND_BGM_DRUM_VOICE,				// BGM 9
		SOUND_BGM_RANKING,					// BGM 10
		SOUND_SE_EXPOSION,					// SE1
		SOUND_SE_HIT,						// SE2
		SOUND_SE_SHOT,						// SE3
		SOUND_MAX,
	};

	CSound();											// コンストラクタ
	~CSound();											// デストラクタ
	HRESULT Init(HWND hWnd);							// 初期化
	void Uninit();										// 終了
	HRESULT Play(std::string inName);					// 再生
	HRESULT Play(SOUND_TYPE type);						// 再生
	void SetVolume(SOUND_TYPE type, float fVolume);		// 音量
	void SetRate(SOUND_TYPE type, float rate);			// ピッチ操作
	void Stop(SOUND_TYPE type);							// 一部停止
	void Stop();										// 全停止

private:
	HRESULT CheckChunk(HANDLE hFile,					//一塊(チャンク)の確認
					   DWORD format, 
					   DWORD *pChunkSize, 
					   DWORD *pChunkDataPosition);


	HRESULT ReadChunkData(HANDLE hFile,					//一塊(チャンク)の読み込み
						  void *pBuffer, 
						  DWORD dwBuffersize, 
						  DWORD dwBufferoffset);

private:

private:
	IXAudio2 *m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;			// マスターボイス
	IXAudio2SourceVoice *m_apSourceVoice[SOUND_MAX];	// ソースボイス
	BYTE *m_apDataAudio[SOUND_MAX];						// オーディオデータ
	DWORD m_aSizeAudio[SOUND_MAX];						// オーディオデータサイズ
	int m_nCountFade;									// フェード用カウンター
};

#endif // !_SOUND_H_
