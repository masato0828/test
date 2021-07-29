#include<DxLib.h>
#include<cmath>
#include<memory>
#include"Geometry.h"
#include"HomingShot.h"

///�����蔻��֐�
///@param posA A�̍��W
///@param radiusA A�̔��a
///@param posB B�̍��W
///@param radiusB B�̔��a
bool IsHit(const Position2& posA, float radiusA, const Position2& posB,  float radiusB) {
	//�����蔻����������Ă�������
	auto diff = posB - posA;
	auto totalRadius = radiusA + radiusB;
	return totalRadius * totalRadius >= diff.SQMagnitude();

}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	SetGraphMode(640, 480, 32);
	ChangeWindowMode(true);
	SetMainWindowText("2016036_���،��a��");
	if (DxLib_Init() != 0) {
		return -1;
	}
	SetDrawScreen(DX_SCREEN_BACK);

	//�w�i�p
	int bgH[4];
	LoadDivGraph("img/bganim.png", 4, 4, 1, 256, 192, bgH);

	int skyH = LoadGraph("img/sky.png");
	int sky2H = LoadGraph("img/sky2.png");

	auto bulletH = LoadGraph("img/bullet.png");
	int playerH[10];
	LoadDivGraph("img/player.png", 10, 5, 2, 16, 24, playerH);

	int enemyH[2];
	LoadDivGraph("img/enemy.png", 2, 2, 1, 32, 32, enemyH);

	struct Bullet {
		Position2 pos;//���W
		Vector2 vel;//���x
		Vector2 accel;
		bool isActive = false;//�����Ă邩�`�H
	};

	//�e�̔��a
	float bulletRadius = 5.0f;

	//���@�̔��a
	float playerRadius = 10.0f;

	//�K����256���炢����Ƃ�
	Bullet bullets[1024];

	HomingShot homingShots[16] = {};//�v���C���[�̃z�[�~���O�e

	Position2 enemypos(320, 100);//�G���W
	Position2 playerpos(320, 400);//���@���W

	unsigned int frame = 0;//�t���[���Ǘ��p
	using namespace std;
	char keystate[256] = {};
	char lastKeyState[256] = {};
	bool isDebugMode = false;
	bool isRightHoming = false;
	int skyy = 0;
	int skyy2 = 0;
	int bgidx = 0;
	constexpr float homing_shot_speed = 10.0f;

	constexpr int frame_for_bullet_type = 180;
	constexpr int bullet_type_num = 7;

	Vector2 housyadir = (playerpos - enemypos);
	float housyabangle = atan2(housyadir.y, housyadir.x);


	while (ProcessMessage() == 0) {
		ClearDrawScreen();

		GetHitKeyStateAll(keystate);

		isDebugMode = keystate[KEY_INPUT_P];

		//�w�i
		DrawExtendGraph(0, 0, 640, 480, bgH[bgidx / 8], false);
		bgidx = (bgidx + 1) % 32;

		//SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
		skyy = (skyy + 1) % 480;
		skyy2 = (skyy2 + 2) % 480;
		DrawExtendGraph(0, skyy, 640, skyy + 480, skyH, true);
		DrawExtendGraph(0, skyy - 480, 640, skyy, skyH, true);
		DrawExtendGraph(0, skyy2, 640, skyy2 + 480, sky2H, true);
		DrawExtendGraph(0, skyy2 - 480, 640, skyy2, sky2H, true);


		//�v���C���[
		if (keystate[KEY_INPUT_RIGHT]) {
			playerpos.x = min(640, playerpos.x + 4);
		}
		else if (keystate[KEY_INPUT_LEFT]) {
			playerpos.x = max(0, playerpos.x - 4);
		}
		if (keystate[KEY_INPUT_UP]) {
			playerpos.y = max(0, playerpos.y - 4);
		}
		else if (keystate[KEY_INPUT_DOWN]) {
			playerpos.y = min(480, playerpos.y + 4);
		}
		if (keystate[KEY_INPUT_Z] && !lastKeyState[KEY_INPUT_Z]) {
			//�z�[�~���O�e����
			int count = 0;
			for (auto& hs : homingShots) {
				DrawString(100, 100, "����", 0x000000);
				if (!hs.isActive) {
					hs.isActive = true;
					hs.pos = playerpos;
					//hs.vel = { isRightHoming?homing_shot_speed:-homing_shot_speed,0.0f };//����
					hs.vel = { count==0 ? homing_shot_speed : -homing_shot_speed,20.0f };//����
					hs.vel.Normalize();
					hs.vel *= homing_shot_speed;
					hs.trail.Clear();
					isRightHoming = !isRightHoming;
					if (++count > 2)
					{
						break;
					}
				}
			}
		}

		//�v���C���[�̃z�[�~���O�e�`��
		for (auto& hshot : homingShots) {
			if (!hshot.isActive)continue;

			if (frame % 2)
			{
				hshot.trail.Update();
			}
			hshot.pos += hshot.vel;
			hshot.trail.Draw();
			/*for (int i = 1; i < 5; i++) {

				auto tailPos = hshot.pos - hshot.vel * static_cast<float>(i);
				auto thickness = static_cast<float>(6 - i);
				DrawLineAA(hshot.pos.x, hshot.pos.y, tailPos.x, tailPos.y, 0xff0000, thickness * 4.0f);
			}*/

			hshot.vel = (hshot.vel +(enemypos - hshot.pos).Normalized()).Normalized() * homing_shot_speed;

			////�G�ւ̃x�N�g������э��̑��x�x�N�g���𐳋K��
			//auto nVelocity = hshot.vel.Normalized();
			//auto nToEnemy = (enemypos - hshot.pos).Normalized();
			//auto dot = Dot(nVelocity, nToEnemy);//cos�����o��
			//auto angle = acos(dot);//�p�x���o��i0�𒆐S�Ƃ������Ώ́j
			//angle = std::fminf(angle, DX_PI_F / 24.0f);
			//float sign = Cross(nVelocity, nToEnemy) > 0.0f ? 1.0f : -1.0f;
			//angle += atan2(hshot.vel.y, hshot.vel.x) + sign * angle;
			//hshot.vel = Vector2(cos(angle), sin(angle)) * homing_shot_speed;

			DrawCircleAA(hshot.pos.x, hshot.pos.y, 5.0f, 16, 0xff0000);
			
			//�G�ɓ�������
			if ((enemypos - hshot.pos).SQMagnitude()<900.0f) {
				hshot.isActive = false;
			}
			//�e���E��
			if (hshot.pos.x + 16 < 0 || hshot.pos.x - 16 > 640 ||
				hshot.pos.y + 24 < 0 || hshot.pos.y - 24 > 480) {
				hshot.isActive = false;
			}
		}

		DrawCircleAA(enemypos.x, enemypos.y, 30.0f, 16, 0x00ff00, false, 3.0f);

		int pidx = (frame / 4 % 2) * 5 + 3;
		DrawRotaGraph(playerpos.x, playerpos.y, 2.0f, 0.0f, playerH[pidx], true);
		if (isDebugMode) {
			//���@�̖{��(�����蔻��)
			DrawCircle(playerpos.x, playerpos.y, playerRadius, 0xffaaaa, false, 3);
		}

		constexpr float base_speed = 5.0f;
		constexpr int blazing_period = 12;
		//if (frame < frame_for_bullet_type)
		//{
		//	DrawString(0, 0, "SHOTTYPE : �^���ɔ�΂�", 0x000000);
		//	//�e����
		//	if (frame % blazing_period == 0) {
		//		for (auto& b : bullets) {
		//			if (!b.isActive) {
		//				b.pos = enemypos;//���W�͓G�@
		//				b.vel = Vector2(0, base_speed);//�܂��͐^���ɔ�΂�
		//				b.accel = Vector2{ 0.0f,0.0f };
		//				b.isActive = true;
		//				break;
		//			}
		//		}
		//	}

		//}
		//else if (frame < frame_for_bullet_type * 2)//���@�_���e
		//{
		//	DrawString(0, 0, "SHOTTYPE : ���@�_���e", 0x000000);
		//	//�e����
		//	if (frame % blazing_period == 0) {
		//		for (auto& b : bullets) {
		//			if (!b.isActive) {
		//				b.pos = enemypos;//���W�͓G�@
		//				b.vel = (playerpos - enemypos).Normalized() * base_speed;//�܂��͐^���ɔ�΂�
		//				b.accel = Vector2{ 0.0f,0.0f };
		//				b.isActive = true;
		//				break;
		//			}
		//		}
		//	}

		//}
		//else if (frame < frame_for_bullet_type * 3)//3wayshot
		//{
		//	DrawString(0, 0, "SHOTTYPE : 3WAYSHOT", 0x000000);
		//	if (frame % blazing_period == 0)
		//	{
		//		int count = 0;
		//		auto dir = (playerpos - enemypos);
		//		float bangle = atan2(dir.y, dir.x);
		//		bangle -= DX_PI_F / 6.0f;
		//		//�e����
		//		for (auto& b : bullets) {
		//			if (!b.isActive) {
		//				b.pos = enemypos;//���W�͓G�@
		//				b.vel = Vector2(cos(bangle), sin(bangle)) * base_speed;//�܂��͐^���ɔ�΂�
		//				b.accel = Vector2{ 0.0f,0.0f };
		//				b.isActive = true;
		//				bangle += DX_PI_F / 6.0f;
		//				++count;
		//			}
		//			if (count == 3)
		//			{
		//				break;
		//			}
		//		}


		//	}
		//}
		//else if (frame < frame_for_bullet_type * 4)//���ˏ�e
		//{
		//	DrawString(0, 0, "SHOTTYPE : ���ˏ�e", 0x000000);
		//	if (frame % blazing_period == 0)
		//	{
		//		int count = 0;
		//		//�e����
		//		for (auto& b : bullets) {


		//			if (!b.isActive) {
		//				b.pos = enemypos;//���W�͓G�@
		//				b.vel = Vector2(cos(housyabangle), sin(housyabangle)) * base_speed;//�܂��͐^���ɔ�΂�
		//				b.accel = Vector2{ 0.0f,0.0f };
		//				b.isActive = true;
		//				housyabangle += 0.1f;
		//				++count;
		//			}
		//			if (count == 4)
		//			{
		//				break;
		//			}
		//		}
		//	}
		//}
		//else if (frame < frame_for_bullet_type * 5)//�S���ʃV���b�g
		//{
		//	DrawString(0, 0, "SHOTTYPE : �S����SHOT", 0x000000);
		//	if (frame % blazing_period == 0)
		//	{
		//		int count = 0;
		//		auto dir = Vector2(0, base_speed);
		//		float bangle = atan2(dir.y, dir.x);
		//		bangle -= DX_PI_F / 6.0f;
		//		//�e����
		//		for (auto& b : bullets) {
		//			if (!b.isActive) {
		//				b.pos = enemypos;//���W�͓G�@
		//				b.accel = Vector2{ 0.0f,0.0f };
		//				b.vel = Vector2(cos(bangle), sin(bangle)) * base_speed;//�܂��͐^���ɔ�΂�
		//				b.isActive = true;
		//				bangle += DX_PI_F / 6.0f;
		//				++count;
		//			}
		//			if (count == 12)
		//			{
		//				break;
		//			}
		//		}


		//	}
		//}
		//else if (frame < frame_for_bullet_type * 6)//3wayshot
		//{
		//DrawString(0, 0, "SHOTTYPE : ����", 0x000000);
		//if (frame % blazing_period == 0)
		//{
		//	int count = 0;
		//	auto dir = (enemypos - playerpos);
		//	float bangle = atan2(dir.y, dir.x);
		//	bangle -= DX_PI_F / 6.0f;
		//	//�e����
		//	for (auto& b : bullets) {
		//		if (!b.isActive) {
		//			b.pos = enemypos;//���W�͓G�@
		//			b.vel = Vector2(cos(bangle), sin(bangle)) * base_speed;//�܂��͐^���ɔ�΂�
		//			b.accel = Vector2{ 0.0f,0.1f };
		//			b.isActive = true;
		//			bangle += DX_PI_F / 6.0f;
		//			++count;
		//		}
		//		if (count == 2)
		//		{
		//			break;
		//		}
		//	}


		//}
		//}
		//else //�΂�܂��e
		//{
		//	DrawString(0, 0, "SHOTTYPE : �΂�܂��e", 0x000000);
		//	if (frame % blazing_period == 0)
		//	{
		//		int count = 0;
		//		//auto dir = (playerpos - enemypos);
		//		//float bangle = atan2(dir.y, dir.x);
		//		float bangle = (rand() % 15) * DX_PI / 180.f;
		//		//�e����
		//		for (auto& b : bullets) {
		//			if (!b.isActive) {
		//				b.pos = enemypos;//���W�͓G�@
		//				b.accel = Vector2{ 0.0f,0.0f };
		//				auto speed = base_speed / 4 + (rand() % 10) * base_speed / 4;
		//				b.vel = Vector2(cos(bangle), sin(bangle)) * speed;//�܂��͐^���ɔ�΂�
		//				b.isActive = true;
		//				
		//				++count;
		//			}
		//			if (count == 0)
		//			{
		//				bangle -= DX_PI_F / 8.0f;
		//			}
		//			if (count == 1)
		//			{
		//				bangle += DX_PI_F / 8.0f;
		//			}
		//			if (count == 2)
		//			{
		//				bangle -= DX_PI_F / 8.0f;
		//			}
		//			if (count == 3)
		//			{
		//				bangle += DX_PI_F / 8.0f;
		//			}
		//			if (count == 4)
		//			{
		//				bangle -= DX_PI_F / 8.0f;
		//			}
		//			if (count == 5)
		//			{
		//				bangle += DX_PI_F / 8.0f;
		//			}
		//			if (count == 6)
		//			{
		//				bangle += DX_PI_F / 8.0f;
		//			}
		//			if (count == 7)
		//			{
		//				break;
		//			}
		//		}
		//	}
		//}
		//�e�̍X�V����ѕ\��
		for (auto& b : bullets) {
			if (!b.isActive) {
				continue;
			}

			//�e�̌��ݍ��W�ɒe�̌��ݑ��x�����Z���Ă�������
			b.vel += b.accel;
			b.pos += b.vel;

			auto dir = b.pos - enemypos;
			float angle = atan2(dir.y, dir.x);
			//�e�̊p�x��atan2�Ōv�Z���Ă��������Bangle�ɒl������񂾂�I�D

			DrawRotaGraph(b.pos.x, b.pos.y, 1.0f, angle, bulletH, true);

			if (isDebugMode) {
				//�e�̖{��(�����蔻��)
				DrawCircle(b.pos.x, b.pos.y, bulletRadius, 0x0000ff, false, 3);
			}
			//�e���E��
			if (b.pos.x + 16 < 0 || b.pos.x - 16 > 640 ||
				b.pos.y + 24 < 0 || b.pos.y - 24 > 480) {
				b.isActive = false;
			}

			//������I
			//����IsHit�͎����������Ă܂���B�����ŏ����Ă��������B
			if (IsHit(b.pos, bulletRadius, playerpos, playerRadius)) {
				//�������������������Ă��������B
				DrawString(0, 36, "HIT", 0xffffff, true);
				b.isActive = false;

			}
		}

		//�G�̕\��
		enemypos.x = abs((int)((frame + 320) % 1280) - 640);
		int eidx = (frame / 4 % 2);
		DrawRotaGraph(enemypos.x, enemypos.y, 2.0f, 0.0f, enemyH[eidx], true);

		if (isDebugMode) {
			//�G�̖{��(�����蔻��)
			DrawCircle(enemypos.x, enemypos.y, 5, 0xffffff, false, 3);
		}
		frame = (frame + 1) % (frame_for_bullet_type * bullet_type_num);

		ScreenFlip();
		std::copy(begin(keystate), end(keystate), begin(keystate));

	}
		DxLib_End();
	
}