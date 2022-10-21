//--------------------------------------------------------------------------------------
//
// マネージャーアプリケーション
// Author::TAKANO
//
//--------------------------------------------------------------------------------------

//--------------------------------------
//インクルードファイル
//--------------------------------------
#include "renderer.h"
#include "input.h"
#include "application.h"
#include "soundmanager.h"

//--------------------------------------
//グローバル変数
//--------------------------------------
CRenderer *CApplication::m_aRenderer;
CInput* CApplication::m_pInput;
CSoundManager *CApplication::m_pSoundManager;

//--------------------------------------
//コンストラクタ
//--------------------------------------
CApplication::CApplication()
{
	
}

//--------------------------------------
//デストラクタ
//--------------------------------------
CApplication::~CApplication()
{
}

//--------------------------------------
//初期化
//--------------------------------------
HRESULT CApplication::Init(HINSTANCE hInctance, HWND hWnd, bool bWindow)
{
	//ポインタにゴミが入っていないかチェック
	ReleseDustCheck();
	
	//動的確保
	m_aRenderer = new CRenderer;
	m_pInput = new CInput;
	m_pSoundManager = new CSoundManager;

	//初期化
	m_aRenderer->Init(hWnd, bWindow);

	m_pInput->Initkeyboard(hInctance,hWnd);

	m_pSoundManager->Init(hWnd);

	return S_OK;
}

//--------------------------------------
//終了
//--------------------------------------
void CApplication::Uninit()
{
	//ゴミが入っていないか確認
	ReleseDustCheck();
}

//--------------------------------------
//更新
//--------------------------------------
void CApplication::Update()
{
	if (m_aRenderer != nullptr)
	{//レンダラーのポインタに値が入っていたとき
		m_aRenderer->Update();						//レンダラーの更新
	}

	if (m_pInput != nullptr)
	{//キーボードのポインタに値が入っていたとき
		m_pInput->Updatekeyboard();			//キーボードの更新
	}

	if (m_pSoundManager != nullptr)
	{
		m_pSoundManager->Update();
	}
}

//--------------------------------------
//描画
//--------------------------------------
void CApplication::Draw()
{
	if (m_aRenderer != nullptr)
	{
		//レンダラーの描画
		m_aRenderer->Draw();
	}
}

//--------------------------------------
//終了処理とダストチェック用関数
//--------------------------------------
void CApplication::ReleseDustCheck()
{
	if (m_aRenderer != nullptr)
	{//レンダラーのポインタに何かしらの値が入っていたら
		m_aRenderer->Uninit();					//終了処理
		delete m_aRenderer;						//レンダラーポインタを解放
		m_aRenderer = nullptr;					//ヌルを代入
	}

	if (m_pInput != nullptr)
	{//キーボードのポインタに何かしらの値が入っていたら
		m_pInput->Uninitkeyboard();		//終了処理
		delete m_pInput;				//キーボードのポインタを解放
		m_pInput = nullptr;				//ヌルを代入
	}

	if (m_pSoundManager != nullptr)
	{
		m_pSoundManager->Uninit();
	}
}

//---------------------------------------
//レンダラー情報取得
//---------------------------------------
CRenderer * CApplication::GetRenderer()
{
	return m_aRenderer;
}

//---------------------------------------
//コントロールの情報取得
//---------------------------------------
CInput * CApplication::GetInput()
{
	return m_pInput;
}

//---------------------------------------
//モード情報取得
//---------------------------------------
CSoundManager * CApplication::GetSoundManager()
{
	return m_pSoundManager;
}
