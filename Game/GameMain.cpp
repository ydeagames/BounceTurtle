//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   GameMain.cpp
//!
//! @brief  �Q�[���֘A�̃\�[�X�t�@�C��
//!
//! @date   2018/06/11
//!
//! @author �R������
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

// �w�b�_�t�@�C���̓ǂݍ��� ================================================
#include "GameMain.h"
#include <math.h>




// �\���̂̒�` ============================================================
// �x�N�g��
typedef struct
{
	float x, y;
} Vec2f;

// �X�v���C�g
typedef struct
{
	HGRP texture;
	Vec2f pos;
	Vec2f size;
} Sprite;

// �F
typedef struct
{
	int r, g, b;
} Color;

// �I�u�W�F�N�g
typedef struct
{
	Sprite sprite_front;
	Sprite sprite_back;
	Vec2f pos;
	Vec2f vel;
	Color color;
	float rota;
	float scale;
	Vec2f hitsize;
} GameObject;




// �萔�̒�` ==============================================================
// �I�u�W�F�N�g�̐�
#define NUM_OBJECTS 10
// �T�E���h�̐�
#define NUM_SOUNDS 8
// �摜�̑傫��
#define IMAGE_SIZE 256


// �O���[�o���ϐ��̒�` ====================================================
int g_input_state;

// �I�u�W�F�N�g�̑O�i
HGRP g_texture_front;
// �I�u�W�F�N�g�̔w�i
HGRP g_texture_back;

// �I�u�W�F�N�g
GameObject g_objects[NUM_OBJECTS];

// ���܂ɂȂ�T�E���h
HSND g_sounds[NUM_SOUNDS];
// �K����T�E���h
HSND g_sound_hit;



// �֐��̐錾 ==============================================================
void InitializeGame(void);  // �Q�[���̏���������
void UpdateGame(void);      // �Q�[���̍X�V����
void RenderGame(void);      // �Q�[���̕`�揈��
void FinalizeGame(void);    // �Q�[���̏I������




// �֐��̒�` ==============================================================
//----------------------------------------------------------------------
//! @brief �Q�[���̏���������
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void InitializeGame(void)
{
	// �I�u�W�F�N�g�̃e�N�X�`���ǂݍ���
	g_texture_front = LoadGraph("Resources\\Textures\\ObjFront.png");
	g_texture_back = LoadGraph("Resources\\Textures\\ObjBack.png");

	// �T�E���h�ǂݍ���
	g_sound_hit = LoadSoundMem("Resources\\Audio\\hit1.ogg");
	for (int i = 0; i < NUM_SOUNDS; i++)
	{
		char str[50];
		sprintf_s(str, "Resources\\Audio\\kick%d.ogg", i + 1);
		g_sounds[i] = LoadSoundMem(str);
	}

	// �I�u�W�F�N�g������
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		// �|�C���^�ɑ��
		GameObject *obj = &g_objects[i];

		obj->sprite_front.texture = g_texture_front;
		obj->sprite_front.pos = { 0, 0 };
		obj->sprite_front.size = { IMAGE_SIZE, IMAGE_SIZE };
		obj->sprite_back = obj->sprite_front;
		obj->sprite_back.texture = g_texture_back;
		obj->pos = { (float)GetRand(SCREEN_WIDTH), (float)GetRand(SCREEN_HEIGHT) };
		obj->vel = { (float)GetRand(10) - 5, (float)GetRand(10) - 5 };
		obj->color = { GetRand(255), GetRand(255), GetRand(255) };
		obj->rota = 0;
		obj->scale = (GetRand(100) / 100.f + .5f)*.25f;
		obj->hitsize = { (IMAGE_SIZE - 20)*obj->scale , (IMAGE_SIZE - 20)*obj->scale };
	}
}



//----------------------------------------------------------------------
//! @brief �Q�[���̍X�V����
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void UpdateGame(void)
{
	// �}�E�X�̍��W�擾
	int m_x, m_y;
	GetMousePoint(&m_x, &m_y);

	// ���͂��擾
	g_input_state = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	// �I�u�W�F�N�g�X�V����
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		// �|�C���^�ɑ��
		GameObject *obj = &g_objects[i];

		// ���W�X�V
		{
			obj->pos.x += obj->vel.x;
			obj->pos.y += obj->vel.y;
		}

		// �����蔻��
		{
			// �I�u�W�F�N�g�ɑ΂��Ă̕ǂ̈ʒu
			float padding_left = SCREEN_LEFT + obj->hitsize.x / 2;
			float padding_right = SCREEN_RIGHT - obj->hitsize.x / 2;
			float padding_top = SCREEN_TOP + obj->hitsize.y / 2;
			float padding_bottom = SCREEN_BOTTOM - obj->hitsize.y / 2;

			// �͈͂𒴂�����
			if (obj->pos.x < padding_left || padding_right <= obj->pos.x)
			{
				// ���ʉ���炷
				PlaySoundMem(g_sound_hit, DX_PLAYTYPE_BACK);
				if (GetRand(10)==0)
					// ���܂ɕʂ̉���������
					PlaySoundMem(g_sounds[GetRand(NUM_SOUNDS - 1)], DX_PLAYTYPE_BACK);
				// ���x�𔽑΂ɂ���
				obj->vel.x *= -1;
			}
			if (obj->pos.y < padding_top || padding_bottom <= obj->pos.y)
			{
				// ���ʉ���炷
				PlaySoundMem(g_sound_hit, DX_PLAYTYPE_BACK);
				if (GetRand(10) == 0)
					// ���܂ɕʂ̉���������
					PlaySoundMem(g_sounds[GetRand(NUM_SOUNDS - 1)], DX_PLAYTYPE_BACK);
				// ���x�𔽑΂ɂ���
				obj->vel.y *= -1;
			}
			// �͂ݏo���C��
			obj->pos.x = ClampF(obj->pos.x, padding_left, padding_right);
			obj->pos.y = ClampF(obj->pos.y, padding_top, padding_bottom);
		}

		// �J�[�\���̕���������
		{
			obj->rota = -atan2f(obj->pos.x - m_x, obj->pos.y - m_y);
		}
	}
}



//----------------------------------------------------------------------
//! @brief �Q�[���̕`�揈��
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void RenderGame(void)
{
	// �w�i�𔒂œh��
	SetDrawBright(255, 255, 255);
	DrawBox(SCREEN_LEFT, SCREEN_TOP, SCREEN_RIGHT, SCREEN_BOTTOM, COLOR_WHITE, TRUE);

	// �I�u�W�F�N�g�`�揈��
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		// �|�C���^�ɑ��
		GameObject *obj = &g_objects[i];

		// �I�u�W�F�N�g�w�i(�R�E��)�̐F���w��
		SetDrawBright(obj->color.r, obj->color.g, obj->color.b);
		// �I�u�W�F�N�g�w�i�`��
		DrawRectRotaGraph(
			(int)obj->pos.x, (int)obj->pos.y,
			(int)obj->sprite_back.pos.x, (int)obj->sprite_back.pos.y,
			(int)obj->sprite_back.size.x, (int)obj->sprite_back.size.y,
			obj->scale,
			(double)obj->rota,
			obj->sprite_back.texture,
			TRUE
		);

		SetDrawBright(255, 255, 255);
		// �I�u�W�F�N�g�O�i�`��
		DrawRectRotaGraph(
			(int)obj->pos.x, (int)obj->pos.y,
			(int)obj->sprite_front.pos.x, (int)obj->sprite_front.pos.y,
			(int)obj->sprite_front.size.x, (int)obj->sprite_front.size.y,
			obj->scale,
			(double)obj->rota,
			obj->sprite_front.texture,
			TRUE
		);
	}
}



//----------------------------------------------------------------------
//! @brief �Q�[���̏I������
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void FinalizeGame(void)
{
	// �e�N�X�`���A�����[�h
	DeleteGraph(g_texture_front);
	DeleteGraph(g_texture_back);

	// �T�E���h�A�����[�h
	DeleteSoundMem(g_sound_hit);
	for (int i = 0; i < NUM_SOUNDS; i++)
		DeleteSoundMem(g_sounds[i]);
}
