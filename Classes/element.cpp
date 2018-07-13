#include "element.h"

USING_NS_CC;



bool Element::init()
{
	if (!Sprite::init())
		return false;

	// 初始化
	element_type = -1;

	return true;
}

void Element::appear()
{
	// 延时显示特效再出现
	setVisible(false);
	scheduleOnce(schedule_selector(Element::appearSchedule), 0.3);
}

void Element::appearSchedule(float dt)
{
	setVisible(true);
	setScale(0.5);
	
	ScaleTo *scale_to = ScaleTo::create(0.2, 1.0);
	runAction(scale_to);
}

void Element::vanish()
{
	// 延时显示特效再消失
	ScaleTo *scale_to = ScaleTo::create(0.2, 0.5);
	CallFunc *funcall = CallFunc::create(this, callfunc_selector(Element::vanishCallback));
	Sequence *sequence = Sequence::create(DelayTime::create(0.2), scale_to, funcall, NULL);
	runAction(sequence);
}

void Element::vanishCallback()
{
	removeFromParent();
}




