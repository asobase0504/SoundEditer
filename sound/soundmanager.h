//*****************************************************************************
//
// 音源マネージャー
// Author::TAKANO
// Author YudaKaito
//
//*****************************************************************************
#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

//=============================================================================
// include
//=============================================================================
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "xaudio2.h"
#include <unordered_map>

//=============================================================================
// 前方宣言
//=============================================================================
class CSound;
class CSoundDistance;

//=============================================================================
// クラス定義
//=============================================================================
class CSoundManager
{
public:
	static float MASTER_VOLUME;
	static float BGM_VOLUME;
	static float SE_VOLUME;
private:
	CSoundManager();
	static CSoundManager* m_instance;
public:
	static CSoundManager* GetInstance();
	~CSoundManager();
public:
	struct SSoundData
	{
		XAUDIO2_BUFFER buffer;
		WAVEFORMATEX format;
	};

	HRESULT Init();		// 初期化
	void Uninit();		// 終了

	CSound* Play(std::string tag);	// 再生
	CSoundDistance* PlayDistance(std::string tag,const D3DXVECTOR3& inPos);	// 再生

/* メンバー関数 */
private:

	// 読込み
	void LoadPath();	// パスロード
	HRESULT LoadSound(std::string tag,std::string path,bool loop);

	// 一塊(チャンク)の確認
	HRESULT CheckChunk(HANDLE hFile,
		DWORD format,
		DWORD *pChunkSize,
		DWORD *pChunkDataPosition);


	// 一塊(チャンク)の読み込み
	HRESULT ReadChunkData(HANDLE hFile,
		void *pBuffer,
		DWORD dwBuffersize,
		DWORD dwBufferoffset);

/* メンバー変数 */
private:
	IXAudio2 *m_pXAudio2;	// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;	// マスターボイス

	// Sound
	std::unordered_map<std::string, SSoundData> m_soundData;
};
#endif // !_SOUND_MANAGER_H
