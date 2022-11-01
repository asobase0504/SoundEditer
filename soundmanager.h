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
#include <map>
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

	//サウンドタイプ
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
	CSound* SoundCreate(SOUND_TYPE type);				// サウンドの生成
	static CSoundManager* ManagerCreate();				// マネージャーの生成
	HRESULT Init();										// 初期化
	void Uninit();										// 終了
	void Update();										// 更新
	void ChangeSound();									// サウンドの種類
	void SoundVolume(SOUND_TYPE type);					// 音量変換
	void FadeInSound(SOUND_TYPE type);					// サウンドフェードイン
	void FadeOutSound(SOUND_TYPE type);					// サウンドフェードアウト
	void ShiftRate(SOUND_TYPE type);					// ピッチ操作
	void LoadPath();									// サウンドパスロード
	void Load();										// ロード
	void Save();										// セーブ
	void SoundParameter();								// サウンドのパラメーター設定
	//HRESULT Play(std::string inName);					// jsonでの再生
	HRESULT Play(SOUND_TYPE type);						// タイプでの再生
	void Stop(SOUND_TYPE type);							// 一部停止
	void Stop();										// 全停止
	void SetVolume(SOUND_TYPE type, float fVolume);		// 音量
	void SetRate(SOUND_TYPE type, float rate);			// ピッチ操作

	// Getter
	int GetPathNumber() { return (int)m_path.size(); }
	std::string GetPath(std::string inName) { return m_path.at(m_pathName.at(inName)).path; }
	std::string GetPath(int inIdx) { return m_path.at(inIdx).path; }
	bool GetLoop(std::string inName) { return m_path.at(m_pathName.at(inName)).isLoop; }
	bool GetLoop(int inIdx) { return m_path.at(inIdx).isLoop; }

private:
	void Select();
	void FadeIn(SOUND_TYPE type);
	void FadeOut(SOUND_TYPE type);

	HRESULT CheckChunk(HANDLE hFile,					//一塊(チャンク)の確認
		DWORD format,
		DWORD *pChunkSize,
		DWORD *pChunkDataPosition);


	HRESULT ReadChunkData(HANDLE hFile,					//一塊(チャンク)の読み込み
		void *pBuffer,
		DWORD dwBuffersize,
		DWORD dwBufferoffset);

private:
	IXAudio2 *m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;			// マスターボイス
	IXAudio2SourceVoice *m_apSourceVoice[SOUND_MAX];	// ソースボイス
	BYTE *m_apDataAudio[SOUND_MAX];						// オーディオデータ
	DWORD m_aSizeAudio[SOUND_MAX];						// オーディオデータサイズ
	int m_nCountFade;									// フェード用カウンター
	
	CSound* m_pSound[SOUND_MAX];						// サウンドのポインタ
	SOUND_TYPE m_type;
	EFadeType m_Fadetype;
	int m_nCount;
	int m_Switch;
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
