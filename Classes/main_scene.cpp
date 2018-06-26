#include "SimpleAudioEngine.h"
#include "main_scene.h"

USING_NS_CC;

// 实例化主场景和层
Scene* MainGameScene::createScene()
{
	return MainGameScene::create();
}

// 初始化主场景
bool MainGameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
		return false;


	return true;
}

// 退出游戏的回调
void MainGameScene::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);


}
