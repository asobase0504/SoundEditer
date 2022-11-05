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
	CSound* SoundCreate(std::string inName);			// �T�E���h�̐���
	CSound* SoundCreate(int inName);					// �T�E���h�̐���
	static CSoundManager* ManagerCreate();				// �}�l�[�W���[�̐���
	HRESULT Init();										// ������
	void Uninit();										// �I��
	void Update();										// �X�V
	void LoadPath();									// �T�E���h�p�X���[�h
	void Load();										// ���[�h
	void Save();										// �Z�[�u

	// �Đ�
	HRESULT Play(std::string inName);					// �Đ�
	HRESULT Play(int inName);							// �Đ�

	// ��~
	void Stop();										// �S��~
	void Stop(std::string type);						// �ꕔ��~
	void Stop(int type);								// �ꕔ��~

	void SetVolume(std::string type, float fVolume);	// ����
	void SetVolume(int type, float fVolume);			// ����

	void SetRate(std::string type, float rate);			// �s�b�`����
	void SetRate(int type, float rate);					// �s�b�`����

	void FadeInSound(std::string type);					// �T�E���h�t�F�[�h�C��
	void FadeInSound(int type);							// �T�E���h�t�F�[�h�C��

	void FadeOutSound(std::string type);				// �T�E���h�t�F�[�h�A�E�g
	void FadeOutSound(int type);						// �T�E���h�t�F�[�h�A�E�g

	// Getter
	int GetPathNumber() { return (int)m_path.size(); }
	std::string GetPath(std::string inName) { return GetPath(m_pathName.at(inName)); }
	std::string GetPath(int inIdx) { return m_path.at(inIdx).path; }
	bool GetLoop(std::string inName) { return GetLoop(m_pathName.at(inName)); }
	bool GetLoop(int inIdx) { return m_path.at(inIdx).isLoop; }

	void SetFadeIn(std::string type);
	void SetFadeIn(int type);
	void FadeOut();

private:

	HRESULT CheckChunk(HANDLE hFile,		// ���(�`�����N)�̊m�F
		DWORD format,
		DWORD *pChunkSize,
		DWORD *pChunkDataPosition);


	HRESULT ReadChunkData(HANDLE hFile,		// ���(�`�����N)�̓ǂݍ���
		void *pBuffer,
		DWORD dwBuffersize,
		DWORD dwBufferoffset);

private:
	IXAudio2 *m_pXAudio2;								// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice *m_pMasteringVoice;			// �}�X�^�[�{�C�X

	int m_nCountFade;													// �t�F�[�h�p�J�E���^�[
	
	std::unordered_map< int, CSound* > m_pSound;	// �T�E���h�̃|�C���^
	std::string m_type;		// �����Ă�̃^�C�v�p�X
	EFadeType m_Fadetype;	// �t�F�[�h��
	int m_nCount;
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
