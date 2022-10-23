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
#include <unordered_map>

//----------------------------------------
//クラス定義
//背景(オブジェクトから派生)
//----------------------------------------
class CSoundManager : public CSound
{
public:
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

	CSoundManager();			// コンストラクタ
	~CSoundManager();			// デストラクタ
	HRESULT Init(HWND hWnd);	// 初期化
	void Uninit();				// 終了
	void Update();				// 更新
	void ChangeSound();			// サウンドの種類
	void SoundVolume();			// 音量変換
	void FadeInSound();			// サウンドフェードイン
	void FadeOutSound();		// サウンドフェードアウト
	void ShiftRate();			// ピッチ操作
	void LoadPath();			// サウンドパスロード
	void Load();				// ロード
	void Save();				// セーブ
	void SoundParameter();		// サウンドのパラメーター設定

	// Getter
	int GetPathNumber() { return (int)m_path.size(); }
	std::string GetPath(std::string inName) { return m_path.at(m_pathName.at(inName)).path; }
	std::string GetPath(int inIdx) { return m_path.at(inIdx).path; }
	bool GetLoop(std::string inName) { return m_path.at(m_pathName.at(inName)).isLoop; }
	bool GetLoop(int inIdx) { return m_path.at(inIdx).isLoop; }

private:
	void Select();
	void FadeIn();
	void FadeOut();

private:
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
	float m_volume;
	float m_frate;
	bool m_Fade_Ok;
};
#endif // !_SOUND_MANAGER_H
