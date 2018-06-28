#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

// 必须要能够Layer才能接收触摸事件和进入退出事件
class MainGameScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	//virtual void onEnter();
	//virtual void onExit();

	// 触摸检测
	//virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	//virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);

	// implement the "static create()" method manually
	CREATE_FUNC(MainGameScene);
};

#endif // __HELLOWORLD_SCENE_H__
