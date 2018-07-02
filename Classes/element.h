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

	void vanish(); // 播放消失效果动画

	CREATE_FUNC(Element);

private:
	int _element_type; // 精灵的种类，由纹理的索引来区分
}; 

#endif

