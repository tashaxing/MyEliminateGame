#include "SimpleAudioEngine.h"
#include "menu_scene.h"
#include "main_scene.h"

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
	
	// ��lambda���ʽ��Ϊ�˵��ص�
	auto start_menu_item = MenuItemLabel::create(start_label, [&](Ref *sender) {
		CCLOG("start game"); // ע�⣬ֻ��debugģʽ�Ż����log

		// ת������Ϸ������
		auto main_game_scene = MainGameScene::create();
		TransitionScene *transition = TransitionFade::create(0.5f, main_game_scene, Color3B(255, 255, 255));
		Director::getInstance()->replaceScene(transition);
	});
	start_menu_item->setPosition(kScreenOrigin.x + kScreenSize.width / 2, kScreenOrigin.y + kScreenSize.height / 2);
	
	auto menu = Menu::createWithItem(start_menu_item);
	menu->setPosition(Vec2::ZERO);

	addChild(menu, 1);

	return true;
}