#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

// ����Ҫ�ܹ�Layer���ܽ��մ����¼��ͽ����˳��¼�
class MainGameScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	//virtual void onEnter();
	//virtual void onExit();

	// �������
	//virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	//virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);

	// implement the "static create()" method manually
	CREATE_FUNC(MainGameScene);
};

#endif // __HELLOWORLD_SCENE_H__
