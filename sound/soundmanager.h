//---------------------------------------------------------
//
//音楽マネージャー
//Author::TAKANO
//
//---------------------------------------------------------
#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

//----------------------------------------
//インクルードファイル
//----------------------------------------
#include "sound.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "xaudio2.h"
#include <unordered_map>

//----------------------------------------
//クラス定義
//背景(オブジェクトから派生)
//----------------------------------------
class CSoundManager
{
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
	void AllStop();	// 全ての停止

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
	std::vector<CSound*> m_playSound;
};
#endif // !_SOUND_MANAGER_H
