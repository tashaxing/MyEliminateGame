#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include "cocos2d.h"

class Element : public cocos2d::Sprite
{
public:
	virtual bool init();
	
	inline void setType(int texture_index)
	{
		_element_type = texture_index;
	}
	inline int getType()
	{
		return _element_type;
	}

	void vanish(); // ������ʧЧ������

	CREATE_FUNC(Element);

private:
	int _element_type; // ��������࣬�����������������
}; 

#endif

