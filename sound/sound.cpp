//-----------------------------------------------------------------------------
//
//サウンド
//Author::TAKANO
//
//-----------------------------------------------------------------------------

//--------------------------------------
//インクルードファイル
//--------------------------------------
#include "sound.h"
#include "../application.h"
#include "soundmanager.h"
#include <assert.h>

//--------------------------------------
//定数定義
//--------------------------------------
const float VOLUME = 11.0f;
const float PITCH = 11.0f;
const float PROVISION_ZERO = 0.0f;

//--------------------------------------
//コンストラクタ
//--------------------------------------
CSound::CSound()
{
	m_volume = 0.0f;
	m_pitch = 0.0f;
}

//--------------------------------------
//デストラクタ
//--------------------------------------
CSound::~CSound()
{
}

//--------------------------------------
//初期化
//--------------------------------------
HRESULT CSound::Init()
{
	m_volume = 0.0f;
	m_pitch = 1.0f;

	return S_OK;
}

void CSound::Update()
{
	XAUDIO2_VOICE_STATE xa2state;
	m_apSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued == 0)
	{
		Uninit();
	}
}

//--------------------------------------
//終了
//--------------------------------------
void CSound::Uninit()
{
	if (m_apSourceVoice != nullptr)
	{
		// 一時停止
		m_apSourceVoice->Stop(0);

		// ソースボイスの破棄
		m_apSourceVoice->DestroyVoice();
		m_apSourceVoice = nullptr;

	}

	if (m_apDataAudio != nullptr)
	{
		// オーディオデータの開放
		free(m_apDataAudio);
		m_apDataAudio = nullptr;
	}
}

HRESULT CSound::Play()
{
	// 再生
	m_apSourceVoice->Start(0);

	return S_OK;
}

void CSound::Stop()
{
	m_apSourceVoice->Stop(0);
}

void CSound::SetVolume(const float inVolume)
{
	m_volume = inVolume;
	m_apSourceVoice->SetVolume(inVolume);
}

void CSound::SetPitch(const float inPitch)
{
	m_pitch = inPitch;
	m_apSourceVoice->SetFrequencyRatio(inPitch);
}
