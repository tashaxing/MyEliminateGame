#include "SimpleAudioEngine.h"
#include "menu_scene.h"

USING_NS_CC;



Scene *MenuScene::createScene()
{
	return MenuScene::create();
}

bool MenuScene::init()
{
	if (!Scene::init())
		return false;	

	// �����Ļ�ߴ糣��(�������ຯ�����ȡ)
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

	// ���ز˵�ҳ�汳��
	auto menu_background = Sprite::create("diamond.png");
	menu_background->setPosition(kScreenOrigin.x + kScreenSize.width / 2, kScreenOrigin.y + kScreenSize.height / 2);
	addChild(menu_background, 0);

	return true;
}