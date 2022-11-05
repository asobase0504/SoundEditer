//--------------------------------------------------------------------------------------
//
// マネージャーアプリケーション
// Author::TAKANO
//
//--------------------------------------------------------------------------------------
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <d3dx9.h>

//----------------------------------------
//前方宣言
//----------------------------------------
class CRenderer;
class CTexture;
class CTitle;
class CGame;
class CResult;
class CRanking;
class CInput;
class CSoundManager;

//----------------------------------------
//クラス定義
//マネージャー
//----------------------------------------
class CApplication
{
public:
	CApplication();														//コンストラクタ
	~CApplication();													//デストラクタ
	HRESULT Init(HINSTANCE hInctance, HWND hWnd, bool bWindow);			//初期化
	void Uninit();										//終了
	void Update();										//更新
	void Draw();										//描画
	void ReleseDustCheck();								//解放漏れをしていないか確認用関数
	static CRenderer* GetRenderer();					//描画情報の取得
	static CInput* GetInput();
	static CSoundManager* GetSoundManager();			//音楽マネージャー情報の取得

	// 音楽関連
	void SoundVolume();					// 音量変換
	void ShiftRate();					// ピッチ操作
	void SoundParameter();				// サウンドのパラメーター設定
	void ChangeSound();					// サウンドの種類
	void Select();

private:
	static CRenderer* m_aRenderer;										//レンダラーのポインター
	static CInput* m_pInput;
	static CSoundManager* m_pSoundManager;								//音楽マネージャーのポインター
	int m_Switch;
};

#endif // !_APPLICATION_H_
