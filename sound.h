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
	/*struct PARAM
	{//サウンドパラメーターの構造体
		char *m_pFilename;	// ファイル名
		int m_nCntLoop;		// ループカウント
	};

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
	};*/

	CSound();											// コンストラクタ
	~CSound();											// デストラクタ
	HRESULT Init();										// 初期化
	void Uninit();										// 終了
	void volUp(float Up);							// プログラム上で作用する用の音量の上げ変数
	void volDown(float Down);						// プログラムで作用する用の音量の下げ変数
	void PitchUp(float rateUp);						// プログラム上で作用する音階上げ用の変数
	void PitchDown(float rateDown);					// プログラム上で作用する音階下げ用の変数
	void Setvol(float vol) { fvol = vol; };				// 音量上げ
	void SetPitch(float rate) { frate = rate; };		// ピッチを上げつつ音階を上げる

	float Getvol() { return fvol; };
	float Getrate() { return frate; };

private:
	float fvol;
	float frate;
};

#endif // !_SOUND_H_
