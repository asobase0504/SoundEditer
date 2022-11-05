//---------------------------------------------------------
//
//サウンド
//Author::TAKANO
//
//---------------------------------------------------------
#ifndef _SOUND_H_
#define _SOUND_H_

//--------------------------------------
//インクルードファイル
//--------------------------------------
#include <Windows.h>

//--------------------------------------
//クラス定義
//サウンド関連
//--------------------------------------
class CSound
{
public:

	CSound();										// コンストラクタ
	~CSound();										// デストラクタ
	HRESULT Init();									// 初期化
	void Uninit();									// 終了
	void volUp(float Up);							// プログラム上で作用する用の音量の上げ変数
	void volDown(float Down);						// プログラムで作用する用の音量の下げ変数
	void PitchUp(float rateUp);						// プログラム上で作用する音階上げ用の変数
	void PitchDown(float rateDown);					// プログラム上で作用する音階下げ用の変数
	void Setvol(float vol) { fvol = vol; }			// 音量上げ
	void SetPitch(float rate) { frate = rate; }		// ピッチを上げつつ音階を上げる

	float Getvol() { return fvol; }
	float Getrate() { return frate; }
	IXAudio2SourceVoice* GetSourceVoice() { return m_apSourceVoice; }

private:
	IXAudio2SourceVoice* m_apSourceVoice;	// ソースボイス
	BYTE* m_apDataAudio;					// オーディオデータ
	DWORD m_aSizeAudio;						// オーディオデータサイズ
	float fvol;
	float frate;
};

#endif // !_SOUND_H_
