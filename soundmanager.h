//---------------------------------------------------------
//
//���y�}�l�[�W���[
//Author::TAKANO
//
//---------------------------------------------------------
#ifndef _SOUND_MANAGER_H
#define _SOUND_MANAGER_H

//----------------------------------------
//�C���N���[�h�t�@�C��
//----------------------------------------
#include "sound.h"
#include"d3dx9.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "xaudio2.h"
#include <map>
#include <unordered_map>

//----------------------------------------
//�N���X��`
//�w�i(�I�u�W�F�N�g����h��)
//----------------------------------------
class CSoundManager
{
public:
	struct PARAM
	{//�T�E���h�p�����[�^�[�̍\����
		char *m_pFilename;	// �t�@�C����
		int m_nCntLoop;		// ���[�v�J�E���g
	};

	//�T�E���h�^�C�v
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
	};

	enum EFadeType
	{//�t�F�[�h�Ɏg�p����
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

	CSoundManager();									// �R���X�g���N�^
	~CSoundManager();									// �f�X�g���N�^
	CSound* SoundCreate(SOUND_TYPE type);				// �T�E���h�̐���
	static CSoundManager* ManagerCreate();				// �}�l�[�W���[�̐���
	HRESULT Init();										// ������
	void Uninit();										// �I��
	void Update();										// �X�V
	void ChangeSound();									// �T�E���h�̎��
	void SoundVolume(SOUND_TYPE type);					// ���ʕϊ�
	void FadeInSound(SOUND_TYPE type);					// �T�E���h�t�F�[�h�C��
	void FadeOutSound(SOUND_TYPE type);					// �T�E���h�t�F�[�h�A�E�g
	void ShiftRate(SOUND_TYPE type);					// �s�b�`����
	void LoadPath();									// �T�E���h�p�X���[�h
	void Load();										// ���[�h
	void Save();										// �Z�[�u
	void SoundParameter();								// �T�E���h�̃p�����[�^�[�ݒ�
	//HRESULT Play(std::string inName);					// json�ł̍Đ�
	HRESULT Play(SOUND_TYPE type);						// �^�C�v�ł̍Đ�
	void Stop(SOUND_TYPE type);							// �ꕔ��~
	void Stop();										// �S��~
	void SetVolume(SOUND_TYPE type, float fVolume);		// ����
	void SetRate(SOUND_TYPE type, float rate);			// �s�b�`����

	// Getter
	int GetPathNumber() { return (int)m_path.size(); }
	std::string GetPath(std::string inName) { return m_path.at(m_pathName.at(inName)).path; }
	std::string GetPath(int inIdx) { return m_path.at(inIdx).path; }
	bool GetLoop(std::string inName) { return m_path.at(m_pathName.at(inName)).isLoop; }
	bool GetLoop(int inIdx) { return m_path.at(inIdx).isLoop; }

private:
	void Select();
	void FadeIn(SOUND_TYPE type);
	void FadeOut(SOUND_TYPE type);

	HRESULT CheckChunk(HANDLE hFile,					//���(�`�����N)�̊m�F
		DWORD format,
		DWORD *pChunkSize,
		DWORD *pChunkDataPosition);


	HRESULT ReadChunkData(HANDLE hFile,					//���(�`�����N)�̓ǂݍ���
		void *pBuffer,
		DWORD dwBuffersize,
		DWORD dwBufferoffset);

private:
	IXAudio2 *m_pXAudio2;								// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice *m_pMasteringVoice;			// �}�X�^�[�{�C�X
	IXAudio2SourceVoice *m_apSourceVoice[SOUND_MAX];	// �\�[�X�{�C�X
	BYTE *m_apDataAudio[SOUND_MAX];						// �I�[�f�B�I�f�[�^
	DWORD m_aSizeAudio[SOUND_MAX];						// �I�[�f�B�I�f�[�^�T�C�Y
	int m_nCountFade;									// �t�F�[�h�p�J�E���^�[
	
	CSound* m_pSound[SOUND_MAX];						// �T�E���h�̃|�C���^
	SOUND_TYPE m_type;
	EFadeType m_Fadetype;
	int m_nCount;
	int m_Switch;
	int m_nFadeCounter;

	// �p�X���
	std::unordered_map<std::string, int> m_pathName;
	std::unordered_map<int, SPath> m_path;

	// �f�[�^���
	std::unordered_map<std::string, int, std::less<>> m_dataName;
	std::unordered_map<int, SData, std::less<>> m_data;
	bool m_Fade_Ok;
};
#endif // !_SOUND_MANAGER_H
