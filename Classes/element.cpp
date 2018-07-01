#include "element.h"

USING_NS_CC;

bool Element::init()
{
	if (!Sprite::init())
		return false;

	return true;
}

void Element::vanish()
{
	auto texture = getTexture();
	
}




