#include "element.h"

USING_NS_CC;

bool Element::init()
{
	if (!Sprite::init())
		return false;

	// ��ʼ��
	element_type = -1;
	is_marked = false;

	return true;
}

void Element::vanish()
{
	// ��ʾ��Ч����ʧ
	removeFromParent();
}






