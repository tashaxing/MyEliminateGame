#include "SimpleAudioEngine.h"
#include "main_scene.h"

USING_NS_CC;

// 实例化主场景和层
Scene* MainGameScene::createScene()
{
	auto game_scene = 
	auto game_layer = MainGameScene::create();
	return MainGameScene::create();
}

// 初始化主场景
bool MainGameScene::init()
{
	if (!Scene::init())
		return false;

	// 获得屏幕尺寸常量(必须在类函数里获取)
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

	// 加载游戏界面背景
	auto game_background = Sprite::create("images/game_bg.jpg");
	game_background->setPosition(kScreenOrigin.x + kScreenSize.width / 2, kScreenOrigin.y + kScreenSize.height / 2);
	addChild(game_background, 0);


	// 添加触摸事件监听
	/*auto touch_listener = EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = CC_CALLBACK_2(MainGameScene::onTouchBegan, this);
	touch_listener->onTouchEnded = CC_CALLBACK_2(MainGameScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);*/

	return true;
}

//bool MainGameScene::onTouchBegan(Touch *touch, Event *event)
//{
//	CCLOG("touch begin: ", touch->getLocation().x, touch->getLocation().y);
//	return true;
//
//}
//
//void MainGameScene::onTouchEnded(Touch *touch, Event *event)
//{
//	CCLOG("touch end: ", touch->getLocation().x, touch->getLocation().y);
//}
//
//void MainGameScene::onEnter()
//{
//	CCLOG("enter main game scene");
//}
//
//void MainGameScene::onExit()
//{
//	CCLOG("exit main game scene");
//}