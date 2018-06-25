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
}