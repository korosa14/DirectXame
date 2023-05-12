#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugText.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();
	void BeamMove();//ビーム移動
	void BeamBorn();//ビーム発生

	void Collision();//衝突判定
	void CollisionPlayerEnemy();//衝突判定(プレイヤーと数)
	void CollisionBeamEnemy();//衝突判定(ビームと敵)

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	void PlayerUpdate(); // プレイヤー更新
	void BeamUpdate();//ビーム更新

	void EnemyUpdate();//敵更新
	void EnemyMove();//敵移動
	void EnemyBorn();//敵発生

	void GamePlayUpdate();//ゲームプレイ更新
	void GamePlayerDrow3D();//ゲームプレイ3D表示
	void GamePlayerDrow2DBack();//ゲームプレイ背景2D表示
	void GamePlayerDrow2DNear();//ゲームプレイ近景2D表示


private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// BG(スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_;
	//プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;
	//ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_;

	int beamFlag_ = 0;//ビーム存在フラグ
	int enemyFlag_ = 0;//敵存在フラグ
	int gameScore_ = 0;//ゲームスコア
	int playerLife_ = 3;//プレイヤーライフ
	int sceneMode_ = 0;//シーンモード
	// 敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_;

	//当たり判定
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
