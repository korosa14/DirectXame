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
	delete modelStage_; // ステージ
	delete modelPlayer_;//プレイヤー
	delete modelBeam_;//ビーム
	delete modelEnemy_;//敵
	delete spriteTitle_;//タイトル
	delete spritesabuTitle_;//サブタイトル
	delete spriteGameover_;//ゲームオーバー

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
	textureHandleStage_ = TextureManager::Load("stage2.jpg");
	modelStage_ = Model::Create();
	//ステージの数ループする
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].Initialize();
	}
	// ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();
	// ステージの位置を変更
	//ステージの数ループする
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f*i-5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};
		// 変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}
	//スコア数値(2Dスプライト)
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumeber_[i] = Sprite::Create(textureHandleNumber_, {300.0f + i * 26, 0});
	}

	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();
	//ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].scale_ = {0.3f, 0.3f, 0.3f};
		worldTransformBeam_[i].Initialize();
	}
	//敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	//敵の数ループする
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].Initialize();
	}
	//デバックテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();
	//タイトル(2Dスプライト)
	textureHandLeTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandLeTitle_, {0, 0});
	//HIT ENTER KEY
	textureHandlesabuTitle_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandlesabuTitle_, {360, 350});
	//ゲームオーバー
	texturHandleGameover_ = TextureManager::Load("gameover.png");
	spriteGameover_ = Sprite::Create(texturHandleGameover_, {0, 0});

	//ゲームスコア
	textureHandleSCORE_ = TextureManager::Load("score.png");
	spriteScore_ = Sprite::Create(textureHandleSCORE_, {150.0f, 0});

	//サウンドデータの読み込み
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");

	//タイトルBGMを再生
	audio_->StopWave(voiceHandleBGM_); // 現在のBGMを停止
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);

	// ライフ(2Dスプライト)
	for (int i = 0; i < 3; i++) {
		spriteLife_[i] = Sprite::Create(textureHandlePlayer_, {800.0f + i * 60, 10});
		spriteLife_[i]->SetSize({40, 40});
	}
}

//タイトル更新
void GameScene::TitleUpdate()
{
	//エンターキーを押した瞬間
	if (input_->TriggerKey(DIK_RETURN))
	{
		//モードをゲームプレイへ変更
		sceneMode_ = 0;
		GamePlayStart();

		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_); // 現在のBGMを停止
		voiceHandleBGM_ =
		    audio_->PlayWave(soundDataHandleGamePlayBGM_, true); // ゲームプレイBGMを再生
	}
	
}

//タイトル表示
void GameScene::TitleDraw2DNear()
{
	//タイトル表示
	spriteTitle_->Draw(); 
	
	//エンター表示
	if (gameTimer_ % 40 >= 20)
	{
		spriteEnter_->Draw();
	}
	// エンター表示
}
//ゲームオーバー表示
void GameScene::GameOverUpdate() 
{
	// エンターキーを押した瞬間
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイへ変更
		sceneMode_ = 1;

		// タイトルBGMを再生
		audio_->StopWave(voiceHandleBGM_); // 現在のBGMを停止
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
	}
}
void GameScene::GameOver2DNear()
{
	//ゲームオーバー表示
	spriteGameover_->Draw();

	// エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

void GameScene::GamePlayStart()
{ 
	gameTimer_ = 0;
	textureHandleSCORE_ = 0;
	playerLife_ = 3;
	playerTimer_ = 0;

	for (int i = 0; i < 10; i++) {
		beamFlag_[i] = 0;
	}
	for (int i = 0; i < 10; i++) {
		enemyFlag_[i] = 0;
	}
	worldTransformPlayer_.translation_.x = 0;
	GamePlayUpdate();

	
}


//ゲームプレイ更新
void GameScene::GamePlayUpdate() {
	PlayerUpdate(); // プレイヤー更新
	BeamUpdate();   // ビーム更新
	EnemyUpdate();  // 敵更新
	Collision();    // 更新処理
	StageUpdate();//ステージ更新
	if (playerLife_ <= 0) {
		
		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_); // 現在のBGMを停止
		voiceHandleBGM_ =
		    audio_->PlayWave(soundDataHandleGameOverBGM_, true); // ゲームプレイBGMを再生

		// ゲームオーバーを表示
		sceneMode_ = 2;
	}
	
}


// 更新
void GameScene::Update() {
	//各シーンの更新処理を呼び出す
	switch (sceneMode_)
	{ 
	case 0:
		GamePlayUpdate();
		
		break;
	case 1:
		TitleUpdate();
		break;
	case 2:
		GameOverUpdate();
	
		break;
	}
	gameTimer_ += 1;
}

void GameScene::DrawScore()
{
	//各桁の値を取り出す
	int eachNumber[5] = {};//各桁の値
	int number = textureHandleSCORE_; // 表示する数字

	int keta = 10000;//最初の桁
	for (int i = 0; i < 5; i++)
	{
		eachNumber[i] = number / keta;//今の桁の値を求める
		number = number % keta;//次の桁以下の値を取り出す
		keta = keta / 10;//桁を進める
	}

	//各桁の数値を描画
	for (int i = 0; i < 5; i++)
	{
		spriteNumeber_[i]->SetSize({32, 64});
		spriteNumeber_[i]->SetTextureRect({32.0f * eachNumber[i],0}, {32, 64});
		spriteNumeber_[i]->Draw();
	}
}

// ゲームプレイ表示3D
void GameScene::GamePlayerDrow3D() {
	// ステージ
	for (int i = 0; i < 20; i++) {
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandleStage_);
	}
	// プレイヤー
	if (playerTimer_%4<2) {
		modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	}
	// レーザー
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == 1)
			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHandleBeam_);
	}
	// 敵
	for (int i = 0; i < 10; i++) {
		if (enemyFlag_[i] == 1)
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		if (enemyFlag_[i] == 2)
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
	}
	
}

// ゲームプレイ更新2D背景
void GameScene::GamePlayerDrow2DBack() {
	// 背景
	spriteBG_->Draw();
}

// ゲームプレイ表示2D近景
void GameScene::GamePlayerDrow2DNear() {
	// ゲームスコア
	DrawScore();
	spriteScore_->Draw();

	// プレイヤーライフ
	for (int i = 0; i < playerLife_; i++) {
		spriteLife_[i]->Draw();
	}
	
	
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
	if (playerTimer_>0)
	{
		playerTimer_-=1;
	}
}

// ビーム発生
void GameScene::BeamBorn() {
	if (beamTimer_ == 0) {
		if (input_->PushKey(DIK_SPACE)) {
			for (int e = 0; e < 10; e++) {

				if (beamFlag_[e] == 0) {
					beamFlag_[e] = 1;
					worldTransformBeam_[e].translation_.x = worldTransformPlayer_.translation_.x;
					beamTimer_ = 1;
					worldTransformBeam_[e].translation_.y = worldTransformPlayer_.translation_.y;
					worldTransformBeam_[e].translation_.z = worldTransformPlayer_.translation_.z;
					break;
				}
			}
		}
	} else {
		beamTimer_++;
		if (beamTimer_ > 10) {
		
		beamTimer_ = 0;
		}
	}
}

//ビーム更新
void GameScene::BeamUpdate() {
	BeamBorn();
	BeamMove();
	for (int i = 0; i < 10; i++) {
		// 変換行列を更新
		worldTransformBeam_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformBeam_[i].scale_, worldTransformBeam_[i].rotation_,
		    worldTransformBeam_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformBeam_[i].TransferMatrix();
	}
}


//ビーム移動
void GameScene::BeamMove() {
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == 1) {
		worldTransformBeam_[i].translation_.z += 0.3f;
		worldTransformBeam_[i].rotation_.x += 0.1f;
		}
		if (worldTransformBeam_[i].translation_.z >= 40) {
		beamFlag_[i] = 0;
		}
	}
}

// 敵更新
void GameScene::EnemyUpdate() {
	EnemyMove();
	EnemyBorn();
	EnemyJump();
	for (int i = 0; i < 10; i++) {
		// 変換行列を更新
		worldTransformEnemy_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformEnemy_[i].scale_, worldTransformEnemy_[i].rotation_,
		    worldTransformEnemy_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformEnemy_[i].TransferMatrix();
	}
	
}
//敵移動
void GameScene::EnemyMove() { 
	for (int i = 0; i < 10; i++) {
		if (enemyFlag_[i] == 1) 
		{
		worldTransformEnemy_[i].translation_.x += enemySpeed_[i];
		if (worldTransformEnemy_[i].translation_.x >= 4)
		{
				enemySpeed_[i] = -0.1f;
		}
		
		if (worldTransformEnemy_[i].translation_.x <= -4) {
				enemySpeed_[i] = 0.1f;
		}

		  worldTransformEnemy_[i].translation_.z -= 0.4f;
		  worldTransformEnemy_[i].rotation_.x -= 0.2f;
		  if (worldTransformEnemy_[i].translation_.z < -5) {
				enemyFlag_[i] = 0;
		  }
		}
		// タイマーにより速度を設定
		worldTransformEnemy_[i].translation_.z -= 0.1f;
		worldTransformEnemy_[i].translation_.z -= gameTimer_ / 1000.0f;
	}
}
//発生
void GameScene::EnemyBorn() {
	if (rand() % 10 == 0) {
		for (int e = 0; e < 10; e++) {
			if (enemyFlag_[e] == 0) {
				enemyFlag_[e] = 1;
				// 敵スピード
				if (rand() % 2 == 0) {
					enemySpeed_[e] = 0.1f;
				} else {
					enemySpeed_[e] = -0.1f;
				}
				worldTransformEnemy_[e].translation_.z = 40;
				// 乱数座標指定
					int x = rand() % 80;
					float x2 = (float)x / 10 - 4;
					worldTransformEnemy_[e].translation_.x = x2;
				    worldTransformEnemy_[e].translation_.y = 0;
					break;
			}
		}
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
	for (int i = 0; i < 10; i++) {
		if (enemyFlag_[i] == 1) {
			// 差を求める
			float dx =
			    abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz =
			    abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);
			// 衝突したら
			if (dx < 1 && dz < 1) {
				// 存在しない
				enemyFlag_[i] = 0;
				playerLife_ -= 1;
				playerTimer_ = 60;

				// プレイヤーヒットSE
				audio_->PlayWave(soundDataHandlePlayerHitSE_);
			}
		}
	}
}
void GameScene::CollisionBeamEnemy() {
	// 敵が存在すれば
	for (int e = 0; e < 10; e++) {
		for (int b = 0; b < 10; b++) {
		if (enemyFlag_[e] == 1 && beamFlag_[b] == 1) {
			
				// 差を求める
				float dx = abs(
				    worldTransformBeam_[b].translation_.x - worldTransformEnemy_[e].translation_.x);
				float dz = abs(
				    worldTransformBeam_[b].translation_.z - worldTransformEnemy_[e].translation_.z);
				// 衝突したら
				if (dx<1&&dz<1) {
					 //存在しない
					enemyFlag_[e] = 2;
					beamFlag_[b] = 0;
					textureHandleSCORE_++;

					enemyJumpSpeed_[e] = 1;
					
					
					// プレイヤーヒットSE
					audio_->PlayWave(soundDataHandleEnemyHitSE_);
				}
			}
		}
	}
}
//敵ジャンプ
void GameScene::EnemyJump() {
	//敵でループ
	for (int i = 0; i < 10; i++) {
	//消滅演出ならば
		if (enemyFlag_[i] == 2) {
			//移動
			worldTransformEnemy_[i].translation_.y += enemyJumpSpeed_[i];
			//速度を減らす                    
			enemyJumpSpeed_[i] -= 0.1f;
			//斜め移動
			worldTransformEnemy_[i].translation_.x += enemySpeed_[i]*2;
		
			//下へ落ちると消滅
			if (worldTransformEnemy_[i].translation_.y < -3) {
				enemyFlag_[i] = 0;
			}
		}
	}
}

//ステージ
void GameScene::StageUpdate() {
	//各ステージでループ
	for (int i = 0; i < 20; i++) {
	//手前に移動
		worldTransformStage_[i].translation_.z -= 0.1f;
		//端まで来たら奥へ戻る
		if (worldTransformStage_[i].translation_.z < -5) {
			worldTransformStage_[i].translation_.z += 40;
		}
		//変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, 
			worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		//変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}
}

    //表示
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);
	//各シーンの背景2D表示を呼び出す
	    switch (sceneMode_) 
		{
	case 0:
		GamePlayerDrow2DBack();
		break;
	case 1:
		break;
	case 2:
		GamePlayerDrow2DBack();
		if (playerTimer_ % 4 < 2)
		{
			
		}
		break;
		}
	// 更新
	

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
	// 各シーンの3D表示を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayerDrow3D(); // ゲームプレイ3D表示
		break;
	case 1:
		break;
	case 2:
		GamePlayerDrow3D(); // ゲームプレイ3D表示
		break;
	}
	
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion
	
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// 各シーンの近景2D表示を呼び出す
	switch (sceneMode_) {
	case 0:
		
		GamePlayerDrow2DNear(); // ゲームプレイ2D近景表示
		break;
	case 1:
		TitleDraw2DNear(); // タイトル
		break;
	case 2:
		
		GameOver2DNear(); // ゲームオーバー
		GamePlayerDrow2DNear(); // ゲームプレイ2D近景表示

		break;
	}
	// デバックテキスト
	//debugText_->Print("AAA", 10, 10, 2);
	debugText_->DrawAll();

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
