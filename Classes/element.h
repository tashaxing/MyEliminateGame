#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include "cocos2d.h"

class Element : public cocos2d::Sprite
{
public:
	virtual bool init();

	int element_type; // 纹理类型

	void vanish(); // 播放消失效果动画
	void vanishCallback(); // 消失回调

	CREATE_FUNC(Element);
}; 

#endif

