#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"

// �˵�ҳ��ֻ�ǵ�����scene������layer
class MenuScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene *createScene();
	virtual bool init();

	CREATE_FUNC(MenuScene);
};

#endif
