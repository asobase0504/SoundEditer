//---------------------------------------------------------
//
//�T�E���h
//Author::TAKANO
//
//---------------------------------------------------------
#ifndef _SOUND_H_
#define _SOUND_H_

//--------------------------------------
//�C���N���[�h�t�@�C��
//--------------------------------------
#include <Windows.h>

//--------------------------------------
//�N���X��`
//�T�E���h�֘A
//--------------------------------------
class CSound
{
public:
	/*struct PARAM
	{//�T�E���h�p�����[�^�[�̍\����
		char *m_pFilename;	// �t�@�C����
		int m_nCntLoop;		// ���[�v�J�E���g
	};

	enum SOUND_TYPE
	{
		SOUND_NONE = -1,					// �g�p����Ă��Ȃ����
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

	CSound();											// �R���X�g���N�^
	~CSound();											// �f�X�g���N�^
	HRESULT Init();										// ������
	void Uninit();										// �I��
	void volUp(float Up);							// �v���O������ō�p����p�̉��ʂ̏グ�ϐ�
	void volDown(float Down);						// �v���O�����ō�p����p�̉��ʂ̉����ϐ�
	void PitchUp(float rateUp);						// �v���O������ō�p���鉹�K�グ�p�̕ϐ�
	void PitchDown(float rateDown);					// �v���O������ō�p���鉹�K�����p�̕ϐ�
	void Setvol(float vol) { fvol = vol; };				// ���ʏグ
	void SetPitch(float rate) { frate = rate; };		// �s�b�`���グ���K���グ��

	float Getvol() { return fvol; };
	float Getrate() { return frate; };

private:
	float fvol;
	float frate;
};

#endif // !_SOUND_H_
