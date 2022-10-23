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
#include <unordered_map>

//----------------------------------------
//�N���X��`
//�w�i(�I�u�W�F�N�g����h��)
//----------------------------------------
class CSoundManager : public CSound
{
public:
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

	CSoundManager();			// �R���X�g���N�^
	~CSoundManager();			// �f�X�g���N�^
	HRESULT Init(HWND hWnd);	// ������
	void Uninit();				// �I��
	void Update();				// �X�V
	void ChangeSound();			// �T�E���h�̎��
	void SoundVolume();			// ���ʕϊ�
	void FadeInSound();			// �T�E���h�t�F�[�h�C��
	void FadeOutSound();		// �T�E���h�t�F�[�h�A�E�g
	void ShiftRate();			// �s�b�`����
	void LoadPath();			// �T�E���h�p�X���[�h
	void Load();				// ���[�h
	void Save();				// �Z�[�u
	void SoundParameter();		// �T�E���h�̃p�����[�^�[�ݒ�

	// Getter
	int GetPathNumber() { return (int)m_path.size(); }
	std::string GetPath(std::string inName) { return m_path.at(m_pathName.at(inName)).path; }
	std::string GetPath(int inIdx) { return m_path.at(inIdx).path; }
	bool GetLoop(std::string inName) { return m_path.at(m_pathName.at(inName)).isLoop; }
	bool GetLoop(int inIdx) { return m_path.at(inIdx).isLoop; }

private:
	void Select();
	void FadeIn();
	void FadeOut();

private:
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
	float m_volume;
	float m_frate;
	bool m_Fade_Ok;
};
#endif // !_SOUND_MANAGER_H
