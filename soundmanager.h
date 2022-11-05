//---------------------------------------------------------
//
//音楽マネージャー
//Author::TAKANO
//
//---------------------------------------------------------
#ifndef _SOUND_MANAGER_H
#define _SOUND_MANAGER_H

//----------------------------------------
//インクルードファイル
//----------------------------------------
#include "sound.h"
#include"d3dx9.h"
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
public:
	struct PARAM
	{//サウンドパラメーターの構造体
		char *m_pFilename;	// ファイル名
		int m_nCntLoop;		// ループカウント
	};

	enum EFadeType
	{//フェードに使用する
		TYPE_NONE = 0,
		TYPE_IN,
		TYPE_OUT,
		TYPE_MAX
	};

	struct SPath
	{
		std::string path;
		bool isLoop;
	};

	struct SData
	{
		float volume;
		float rate;
	};

	CSoundManager();									// コンストラクタ
	~CSoundManager();									// デストラクタ
	CSound* SoundCreate(std::string inName);			// サウンドの生成
	CSound* SoundCreate(int inName);					// サウンドの生成
	static CSoundManager* ManagerCreate();				// マネージャーの生成
	HRESULT Init();										// 初期化
	void Uninit();										// 終了
	void Update();										// 更新
	void LoadPath();									// サウンドパスロード
	void Load();										// ロード
	void Save();										// セーブ

	// 再生
	HRESULT Play(std::string inName);					// 再生
	HRESULT Play(int inName);							// 再生

	// 停止
	void Stop();										// 全停止
	void Stop(std::string type);						// 一部停止
	void Stop(int type);								// 一部停止

	void SetVolume(std::string type, float fVolume);	// 音量
	void SetVolume(int type, float fVolume);			// 音量

	void SetRate(std::string type, float rate);			// ピッチ操作
	void SetRate(int type, float rate);					// ピッチ操作

	void FadeInSound(std::string type);					// サウンドフェードイン
	void FadeInSound(int type);							// サウンドフェードイン

	void FadeOutSound(std::string type);				// サウンドフェードアウト
	void FadeOutSound(int type);						// サウンドフェードアウト

	// Getter
	int GetPathNumber() { return (int)m_path.size(); }
	std::string GetPath(std::string inName) { return GetPath(m_pathName.at(inName)); }
	std::string GetPath(int inIdx) { return m_path.at(inIdx).path; }
	bool GetLoop(std::string inName) { return GetLoop(m_pathName.at(inName)); }
	bool GetLoop(int inIdx) { return m_path.at(inIdx).isLoop; }

	void SetFadeIn(std::string type);
	void SetFadeIn(int type);
	void FadeOut();

private:

	HRESULT CheckChunk(HANDLE hFile,		// 一塊(チャンク)の確認
		DWORD format,
		DWORD *pChunkSize,
		DWORD *pChunkDataPosition);


	HRESULT ReadChunkData(HANDLE hFile,		// 一塊(チャンク)の読み込み
		void *pBuffer,
		DWORD dwBuffersize,
		DWORD dwBufferoffset);

private:
	IXAudio2 *m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;			// マスターボイス

	int m_nCountFade;													// フェード用カウンター
	
	std::unordered_map< int, CSound* > m_pSound;	// サウンドのポインタ
	std::string m_type;		// 流してるのタイプパス
	EFadeType m_Fadetype;	// フェード状況
	int m_nCount;
	int m_nFadeCounter;

	// パス情報
	std::unordered_map<std::string, int> m_pathName;
	std::unordered_map<int, SPath> m_path;

	// データ情報
	std::unordered_map<std::string, int, std::less<>> m_dataName;
	std::unordered_map<int, SData, std::less<>> m_data;
	bool m_Fade_Ok;
};
#endif // !_SOUND_MANAGER_H
