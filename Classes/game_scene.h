#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class Element;

// 精灵的行列值结构体
struct ElementPos
{
	int row;
	int col;
	
	// fixme: the constructor will not compile success in coco2dx
	//ElementPos(int _row, int _col): row(_row), col(_col)
	//{}
};

// 必须要能够Layer才能接收触摸事件和进入退出事件
class GameScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	// 触摸检测
	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
	virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);

	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);

private:
	std::vector<std::vector<Element *>> _game_board; // 精灵阵列
	ElementPos _start_pos, _end_pos; // 拖拽的起始和终止位置标号
	bool _is_moving; // 是否在移动中
	ElementPos getElementPosByCoordinate(float x, float y);
};

#endif // __HELLOWORLD_SCENE_H__
