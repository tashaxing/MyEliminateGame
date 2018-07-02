#include "element.h"

USING_NS_CC;

bool Element::init()
{
	if (!Sprite::init())
		return false;

	// 初始化
	element_type = -1;
	is_marked = false;

	return true;
}

void Element::vanish()
{
	// 显示特效再消失
	removeFromParent();
}






