//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   GameMain.cpp
//!
//! @brief  ゲーム関連のソースファイル
//!
//! @date   2018/06/11
//!
//! @author 山口寛雅
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

// ヘッダファイルの読み込み ================================================
#include "GameMain.h"
#include <math.h>




// 構造体の定義 ============================================================
// ベクトル
typedef struct
{
	float x, y;
} Vec2f;

// スプライト
typedef struct
{
	HGRP texture;
	Vec2f pos;
	Vec2f size;
} Sprite;

// 色
typedef struct
{
	int r, g, b;
} Color;

// オブジェクト
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




// 定数の定義 ==============================================================
// オブジェクトの数
#define NUM_OBJECTS 10
// サウンドの数
#define NUM_SOUNDS 8
// 画像の大きさ
#define IMAGE_SIZE 256


// グローバル変数の定義 ====================================================
int g_input_state;

// オブジェクトの前景
HGRP g_texture_front;
// オブジェクトの背景
HGRP g_texture_back;

// オブジェクト
GameObject g_objects[NUM_OBJECTS];

// たまになるサウンド
HSND g_sounds[NUM_SOUNDS];
// 必ず鳴るサウンド
HSND g_sound_hit;



// 関数の宣言 ==============================================================
void InitializeGame(void);  // ゲームの初期化処理
void UpdateGame(void);      // ゲームの更新処理
void RenderGame(void);      // ゲームの描画処理
void FinalizeGame(void);    // ゲームの終了処理




// 関数の定義 ==============================================================
//----------------------------------------------------------------------
//! @brief ゲームの初期化処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void InitializeGame(void)
{
	// オブジェクトのテクスチャ読み込み
	g_texture_front = LoadGraph("Resources\\Textures\\ObjFront.png");
	g_texture_back = LoadGraph("Resources\\Textures\\ObjBack.png");

	// サウンド読み込み
	g_sound_hit = LoadSoundMem("Resources\\Audio\\hit1.ogg");
	for (int i = 0; i < NUM_SOUNDS; i++)
	{
		char str[50];
		sprintf_s(str, "Resources\\Audio\\kick%d.ogg", i + 1);
		g_sounds[i] = LoadSoundMem(str);
	}

	// オブジェクト初期化
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		// ポインタに代入
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
//! @brief ゲームの更新処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void UpdateGame(void)
{
	// マウスの座標取得
	int m_x, m_y;
	GetMousePoint(&m_x, &m_y);

	// 入力を取得
	g_input_state = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	// オブジェクト更新処理
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		// ポインタに代入
		GameObject *obj = &g_objects[i];

		// 座標更新
		{
			obj->pos.x += obj->vel.x;
			obj->pos.y += obj->vel.y;
		}

		// 当たり判定
		{
			// オブジェクトに対しての壁の位置
			float padding_left = SCREEN_LEFT + obj->hitsize.x / 2;
			float padding_right = SCREEN_RIGHT - obj->hitsize.x / 2;
			float padding_top = SCREEN_TOP + obj->hitsize.y / 2;
			float padding_bottom = SCREEN_BOTTOM - obj->hitsize.y / 2;

			// 範囲を超えたら
			if (obj->pos.x < padding_left || padding_right <= obj->pos.x)
			{
				// 効果音を鳴らす
				PlaySoundMem(g_sound_hit, DX_PLAYTYPE_BACK);
				if (GetRand(10)==0)
					// たまに別の音を混ぜる
					PlaySoundMem(g_sounds[GetRand(NUM_SOUNDS - 1)], DX_PLAYTYPE_BACK);
				// 速度を反対にする
				obj->vel.x *= -1;
			}
			if (obj->pos.y < padding_top || padding_bottom <= obj->pos.y)
			{
				// 効果音を鳴らす
				PlaySoundMem(g_sound_hit, DX_PLAYTYPE_BACK);
				if (GetRand(10) == 0)
					// たまに別の音を混ぜる
					PlaySoundMem(g_sounds[GetRand(NUM_SOUNDS - 1)], DX_PLAYTYPE_BACK);
				// 速度を反対にする
				obj->vel.y *= -1;
			}
			// はみ出し修正
			obj->pos.x = ClampF(obj->pos.x, padding_left, padding_right);
			obj->pos.y = ClampF(obj->pos.y, padding_top, padding_bottom);
		}

		// カーソルの方向を向く
		{
			obj->rota = -atan2f(obj->pos.x - m_x, obj->pos.y - m_y);
		}
	}
}



//----------------------------------------------------------------------
//! @brief ゲームの描画処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void RenderGame(void)
{
	// 背景を白で塗る
	SetDrawBright(255, 255, 255);
	DrawBox(SCREEN_LEFT, SCREEN_TOP, SCREEN_RIGHT, SCREEN_BOTTOM, COLOR_WHITE, TRUE);

	// オブジェクト描画処理
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		// ポインタに代入
		GameObject *obj = &g_objects[i];

		// オブジェクト背景(コウラ)の色を指定
		SetDrawBright(obj->color.r, obj->color.g, obj->color.b);
		// オブジェクト背景描画
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
		// オブジェクト前景描画
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
//! @brief ゲームの終了処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void FinalizeGame(void)
{
	// テクスチャアンロード
	DeleteGraph(g_texture_front);
	DeleteGraph(g_texture_back);

	// サウンドアンロード
	DeleteSoundMem(g_sound_hit);
	for (int i = 0; i < NUM_SOUNDS; i++)
		DeleteSoundMem(g_sounds[i]);
}
