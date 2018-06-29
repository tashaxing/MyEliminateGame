#include "SimpleAudioEngine.h"
#include "game_scene.h"
#include "element.h"

USING_NS_CC;

// ʵ�����������Ͳ�
Scene *GameScene::createScene()
{
	auto game_scene = Scene::create();
	auto game_layer = GameScene::create();
	game_scene->addChild(game_layer);
	return game_scene;
}

// ��ʼ��������
bool GameScene::init()
{
	if (!Layer::init())
		return false;

	// �����Ļ�ߴ糣��(�������ຯ�����ȡ)
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

	// ������Ϸ���汳��
	auto game_background = Sprite::create("images/game_bg.jpg");
	game_background->setPosition(kScreenOrigin.x + kScreenSize.width / 2, kScreenOrigin.y + kScreenSize.height / 2);
	addChild(game_background, 0);

	// ��������������
	auto element = Element::createElement

	// ��Ӵ����¼�����
	auto touch_listener = EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touch_listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this); // ����� _eventDispatcher

	return true;
}

bool GameScene::onTouchBegan(Touch *touch, Event *event)
{
	CCLOG("touch begin, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);
	return true;

}

void GameScene::onTouchEnded(Touch *touch, Event *event)
{
	CCLOG("touch end, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);
}

void GameScene::onEnter()
{
	// ������layer onenter ���ܲ�׽�����¼�
	Layer::onEnter();
	CCLOG("enter game scene");
}

void GameScene::onExit()
{
	Layer::onExit();
	CCLOG("exit game scene");
}