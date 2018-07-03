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
	ScaleTo *scale_to = ScaleTo::create(0.2, 0.5);
	CallFunc *funcall = CallFunc::create(this, callfunc_selector(Element::vanishCallback));
	Sequence *sequence = Sequence::create(scale_to, funcall, NULL);
	runAction(sequence);
}

void Element::vanishCallback()
{
	removeFromParent();
}




