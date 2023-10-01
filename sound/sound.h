//---------------------------------------------------------
//
//サウンド
//Author::TAKANO
//Author::YudaKaito
//
//---------------------------------------------------------
#ifndef _SOUND_H_
#define _SOUND_H_

//--------------------------------------
//インクルードファイル
//--------------------------------------
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "xaudio2.h"

//--------------------------------------
// クラス定義
// 
// Objectの派生にして毎フレームUpdateを通して下さい。
//--------------------------------------
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

	void SetPitch(const float inPitch);
	void AddPitch(const float inPitch) { SetPitch(inPitch + m_pitch); }
	float GetPitch() { return m_pitch; }

	void SetSourceVoice(IXAudio2SourceVoice* inVoice) { m_apSourceVoice = inVoice; }
	IXAudio2SourceVoice* GetSourceVoice() { return m_apSourceVoice; }

private:
	IXAudio2SourceVoice* m_apSourceVoice;	// ソースボイス
	BYTE* m_apDataAudio;					// オーディオデータ
	DWORD m_aSizeAudio;						// オーディオデータサイズ

	float m_volume;
	float m_pitch;
	int m_loop;
};

#endif // !_SOUND_H_
