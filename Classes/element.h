#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include "cocos2d.h"

class Element : public cocos2d::Sprite
{
public:
	static Element *createElement();
	virtual bool init();

	CREATE_FUNC(Element);

};

#endif

