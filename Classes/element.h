#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include "cocos2d.h"

class Element : public cocos2d::Sprite
{
public:
	virtual bool init();

	int element_type; // ��������

	void vanish(); // ������ʧЧ������
	void vanishCallback(); // ��ʧ�ص�

	CREATE_FUNC(Element);
}; 

#endif

