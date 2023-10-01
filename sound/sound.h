//*****************************************************************************
//
//サウンド
//Author::TAKANO
//Author::YudaKaito
//
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_

//=============================================================================
// include
//=============================================================================
#include <Windows.h>
#include <mfapi.h>
#include "xaudio2.h"

/*
// Objectの派生にして毎フレームUpdateを通して下さい。
// Sound用のpriorityがあると管理楽かも
*/

//=============================================================================
// クラス定義
//=============================================================================
class CSound
{
public:

	CSound();		// コンストラクタ
	~CSound();		// デストラクタ
	HRESULT Init();	// 初期化
	void Update();	// 更新
	void Uninit();	// 終了

	// 再生
	HRESULT Play();

	// 停止
	void Stop();

	// 音量
	void SetVolume(const float inVolume);
	void AddVolume(const float inVolume) { SetVolume(inVolume + m_volume); }
	float GetVolume() { return m_volume; }

	// ピッチ
	void SetPitch(const float inPitch);
	void AddPitch(const float inPitch) { SetPitch(inPitch + m_pitch); }
	float GetPitch() { return m_pitch; }

	// 音データ
	void SetSourceVoice(IXAudio2SourceVoice* inVoice) { m_apSourceVoice = inVoice; }

private:
	IXAudio2SourceVoice* m_apSourceVoice;	// ソースボイス

	float m_volume;
	float m_pitch;
	int m_loop;
};

#endif // !_SOUND_H_
