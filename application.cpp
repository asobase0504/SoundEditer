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
#include <assert.h>

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

	m_pSoundManager->Init();
	m_Switch = 0;	//セレクト時に使用する変数の初期化

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

	ChangeSound();
	SoundParameter();
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

void CApplication::SoundVolume()
{
	//if (CApplication::GetInput()->GetkeyboardPress(DIK_UP))
	//{
	//	m_pSound[type]->volUp(UP_DOWN_TRACK);
	//	m_pSoundManager->SetVolume(m_type, m_pSound[type]->Getvol());
	//}

	//if (CApplication::GetInput()->GetkeyboardPress(DIK_DOWN))
	//{
	//	m_pSound[type]->volDown(UP_DOWN_TRACK);
	//	m_pSoundManager->SetVolume("", m_pSound[type]->Getvol());
	//}

	//if (CApplication::GetInput()->GetkeyboardTrigger(DIK_LEFT))
	//{
	//	m_Fadetype = TYPE_IN;
	//	m_Fade_Ok = false;
	//}

	//if (CApplication::GetInput()->GetkeyboardTrigger(DIK_RIGHT))
	//{
	//	m_Fadetype = TYPE_OUT;
	//	m_Fade_Ok = false;
	//}

	//if (CApplication::GetInput()->GetkeyboardPress(DIK_O))
	//{
	//	m_pSound[type]->PitchUp(TRACK_PITH);
	//	m_pSoundManager->SetRate(m_type, m_pSound[type]->Getrate());
	//}

	//if (CApplication::GetInput()->GetkeyboardPress(DIK_P))
	//{
	//	m_pSound[type]->PitchDown(TRACK_PITH);
	//	m_pSoundManager->SetRate(m_type, m_pSound[type]->Getrate());
	//}

}

void CApplication::ShiftRate()
{
}

void CApplication::SoundParameter()
{
	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_N))
	{
		m_pSoundManager->Save();
	}

	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_M))
	{
		m_pSoundManager->Load();
	}
}

void CApplication::ChangeSound()
{
	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_L))
	{ //曲の入れ替え

		m_Switch = (m_Switch + 1) % 12;

		Select();

		if (m_Switch >= 11)
		{
			m_Switch = 0;
		}
	}
}

void CApplication::Select()
{
	std::string tag;

	switch (m_Switch)
	{
	case 0:

		/*変更なし*/

		break;

	case 1:
		tag = "BATTLEBAN";
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 2:
		tag = "LAST_MEETING";
		m_pSoundManager->Stop();
		m_pSoundManager->Play("LAST_MEETING");
		break;

	case 3:
		tag = "RESULT_BGM";
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 4:
		tag = "VIRTUAL";
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 5:
		tag = "BATTLEMEETING_VER_2";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);

		break;

	case 6:
		tag = "NO_NAME";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 7:
		tag = "CHALLENGE_TO_TOMORROW";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 8:
		tag = "BATTLEMEETING";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 9:
		tag = "HEART";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 10:
		tag = "DRUM_VOICE";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 11:
		tag = "RANKING";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	default:
		assert(false);
		break;
	}
}
