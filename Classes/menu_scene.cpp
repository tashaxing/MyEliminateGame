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
	auto menu_background = Sprite::create("images/menu_bg.jpg");
	menu_background->setPosition(kScreenOrigin.x + kScreenSize.width / 2, kScreenOrigin.y + kScreenSize.height / 2);
	addChild(menu_background, 0);

	// ��ӿ�ʼ�˵�
	auto start_label = Label::createWithTTF("Start Game", "fonts/Marker Felt.ttf", 35);
	start_label->setTextColor(cocos2d::Color4B::RED);
	start_label->setPosition(kScreenOrigin.x + kScreenSize.width / 2, kScreenOrigin.y + kScreenSize.height / 2);
	addChild(start_label, 1);

	return true;
}