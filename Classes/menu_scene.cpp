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

	// 获得屏幕尺寸常量(必须在类函数里获取)
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

	// 加载菜单页面背景
	auto menu_background = Sprite::create("images/menu_bg.jpg");
	menu_background->setPosition(kScreenOrigin.x + kScreenSize.width / 2, kScreenOrigin.y + kScreenSize.height / 2);
	addChild(menu_background, 0);

	// 添加开始菜单
	auto start_label = Label::createWithTTF("Start Game", "fonts/Marker Felt.ttf", 35);
	start_label->setTextColor(cocos2d::Color4B::RED);
	
	// 用lambda表达式作为菜单回调
	auto start_menu_item = MenuItemLabel::create(start_label, [&](Ref *sender) {
		CCLOG("start game"); // 注意，只有debug模式才会输出log

		// 转场到游戏主界面
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