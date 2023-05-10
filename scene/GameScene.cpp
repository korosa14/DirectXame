#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "MathUtilityForText.h"
#include<time.h>

// コンストラクタ
GameScene::GameScene() {}
// デストラクタ
GameScene::~GameScene() {
	delete spriteBG_;   // BG
	delete modelStage_; // ステージ}
	delete modelPlayer_;//プレイヤー
	delete modelBeam_;//ビーム
	delete modelEnemy_;//敵
}
void GameScene::Initialize() {
	srand((unsigned int)time(NULL));
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// BG(2Dスプライト)
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
	// ステージ
	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();
	//// ステージの位置を変更
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	// 変換行列を更新
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_, worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);
	 //変換行列を定数バッファに転送
	worldTransformStage_.TransferMatrix();
	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();
	//ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransformBeam_.Initialize();
	//敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformEnemy_.Initialize();
	//デバックテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();
}
// 更新
void GameScene::Update() {
	PlayerUpdate();//プレイヤー更新
	BeamUpdate();//ビーム更新
	EnemyUpdate();//敵更新
	Collision();//更新処理
}
// プレイヤー更新
void GameScene::PlayerUpdate() {
	// 変換行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();
	// 移動
	// 右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	if (worldTransformPlayer_.translation_.x >= 4)
	{
		worldTransformPlayer_.translation_.x = 4;
	}

	// 左へ移動
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}
	if (worldTransformPlayer_.translation_.x < -4)
	{
		worldTransformPlayer_.translation_.x = -4;
	}
}
//ビーム更新
void GameScene::BeamUpdate() {
	//変換行列を更新
	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
	    worldTransformBeam_.scale_, 
		worldTransformBeam_.rotation_,
	    worldTransformBeam_.translation_);
	//変換行列を定数バッファに転送
	worldTransformBeam_.TransferMatrix();
	BeamBorn();
	BeamMove();
}


// ビーム発生
void GameScene::BeamBorn() {
	if (input_->PushKey(DIK_SPACE)&&beamFlag_==0) {
		beamFlag_ = 1;
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		worldTransformBeam_.translation_.y = worldTransformPlayer_.translation_.y;
		worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
	}
}

//ビーム移動
void GameScene::BeamMove() {
	if (beamFlag_ == 1) {
		worldTransformBeam_.translation_.z += 0.1f;
		worldTransformBeam_.rotation_.x += 0.1f;
	}
	if (worldTransformBeam_.translation_.z >= 40) {
		beamFlag_ = 0;
	}
}

// 敵更新
void GameScene::EnemyUpdate() {
	//変換行列を更新
	worldTransformEnemy_.matWorld_ = MakeAffineMatrix(
	    worldTransformEnemy_.scale_,
		worldTransformEnemy_.rotation_,
	    worldTransformEnemy_.translation_);
	//変換行列を定数バッファに転送
	worldTransformEnemy_.TransferMatrix();
	EnemyMove();
	EnemyBorn();
}
//敵移動
void GameScene::EnemyMove() { 
	if (enemyFlag_ == 1) {
		worldTransformEnemy_.translation_.z -= 0.1f; 
		worldTransformEnemy_.rotation_.x -= 0.1f;
	}
	if (worldTransformEnemy_.translation_.z < -5) {
		enemyFlag_ = 0;
	}
}
//発生
void GameScene::EnemyBorn() {
	if (enemyFlag_ == 0) {
		enemyFlag_ = 1;
		worldTransformEnemy_.translation_.z = 40;
		// 乱数座標指定
		int x = rand() % 80;
		float x2 = (float)x / 10 - 4;
		worldTransformEnemy_.translation_.x = x2;
	}
	
}
//衝突判定
void GameScene::Collision() {
	//衝突判定
	CollisionPlayerEnemy();
	CollisionBeamEnemy();
}
//衝突判定(プレイヤーと数)
void GameScene::CollisionPlayerEnemy() {
	//敵が存在すれば
	if (enemyFlag_ == 1) {
	//差を求める
		float dx = abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_.translation_.x);
		float dz = abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_.translation_.z);
	//衝突したら
		if (dx < 1 && dz < 1) {
		//存在しない
			enemyFlag_ = 0;
		    playerLife_ -= 1;
		}
	}
}
void GameScene::CollisionBeamEnemy() {
	// 敵が存在すれば
	if (enemyFlag_ == 1&& beamFlag_==1) {
		// 差を求める
		float dx = abs(worldTransformBeam_.translation_.x - worldTransformEnemy_.translation_.x);
		float dz = abs(worldTransformBeam_.translation_.z - worldTransformEnemy_.translation_.z);
		// 衝突したら
		if (dx < 1 && dz < 1) {
			// 存在しない
			enemyFlag_ = 0;
		    beamFlag_= 0;
			gameScore_++;
		}
	}
}

    //表示
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);
	// 背景
	spriteBG_->Draw();
	
	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);
	// ステージ
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);
	//プレイヤー
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	//レーザー
	if (beamFlag_ == 1)
	modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	//敵
	modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// ゲームスコア
	char str[100];
	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str, 200, 10, 2);
	// デバックテキスト
	debugText_->Print("AAA", 10, 10, 2);
	debugText_->DrawAll();

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
