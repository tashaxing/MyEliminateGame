#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class Element;

// ���������ֵ�ṹ��
struct ElementPos
{
	int row;
	int col;
	
	// fixme: the constructor will not compile success in coco2dx
	//ElementPos(int _row, int _col): row(_row), col(_col)
	//{}
};

// �߼�����ṹ��
struct ElementProto
{
	int type;
	bool marked;
};


// ����Ҫ�ܹ�Layer���ܽ��մ����¼��ͽ����˳��¼�
class GameScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene *createScene();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	// �������
	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
	virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);

	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);

private:
	std::vector<std::vector<ElementProto>> _game_board; // ��������, �洢��������
	ElementPos _start_pos, _end_pos; // ��ק����ʼ����ֹλ�ñ��
	bool _is_moving; // �Ƿ����ƶ���
	bool _is_can_touch; // �Ƿ�ɴ���
	ElementPos getElementPosByCoordinate(float x, float y);
	
	void generateGameBoard(); // �������������Ϸ��ͼ�������п������ľ���
	void fillVacantElements(); // ���ȱ��Ϸ��ͼ�������п������ľ���
	void swapElementPair(ElementPos p1, ElementPos p2, bool is_reverse); // ������������
	std::vector<ElementPos> checkEliminate(); // ���������ľ���
	void batchEliminate(const std::vector<ElementPos> &eliminate_list); // ִ������
	bool checkGameDead(); // �����Ϸ�Ƿ����
};

#endif // __HELLOWORLD_SCENE_H__
