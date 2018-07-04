#include "SimpleAudioEngine.h"
#include "game_scene.h"
#include "element.h"

USING_NS_CC;

// ���������ļ�
const std::vector<std::string> kElementImgArray{
	"images/diamond_red.png",
	"images/diamond_green.png",
	"images/diamond_blue.png",
	"images/candy_red.png",
	"images/candy_green.png",
	"images/candy_blue.png"
};

// ����ʱ������
const std::string kEliminateStartImg = "images/star.png";


// ����߾�
const float kLeftMargin = 20;
const float kRightMargin = 20;
const float kBottonMargin = 70;

// ����ʹ����
const int kRowNum = 8;
const int kColNum = 8;

// ������������������
int getRandomSpriteIndex(int len)
{
	return rand() % len;
}

// ʵ�����������Ͳ�
Scene *GameScene::createScene()
{
	Scene *game_scene = Scene::create();
	Layer *game_layer = GameScene::create();
	game_scene->addChild(game_layer);
	return game_scene;
}

// ��ʼ��������
bool GameScene::init()
{
	if (!Layer::init())
		return false;

	// �����Ļ�ߴ糣��(�������ຯ�����ȡ)
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

	// ������Ϸ���汳��
	Sprite *game_background = Sprite::create("images/game_bg.jpg");
	game_background->setPosition(kScreenOrigin.x + kScreenSize.width / 2, kScreenOrigin.y + kScreenSize.height / 2);
	addChild(game_background, 0);

	// ��������������
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;
	srand(unsigned(time(0))); // ��ʼ�������������
	for (int i = 0; i < kRowNum; i++)
	{
		std::vector<Element *> line_elements;
		for (int j = 0; j < kColNum; j++)
		{
			Element *element = Element::create();
			int random_index = getRandomSpriteIndex(kElementImgArray.size()); // ������ɾ���
			element->setTexture(kElementImgArray[random_index]); // ����������	
			element->element_type = random_index;
			element->setContentSize(Size(element_size, element_size)); // ���ڲ����óߴ�
			element->setPosition(kLeftMargin + (j + 0.5) * element_size, kBottonMargin + (i + 0.5) * element_size); // FIXME:�����Ų����м�û�з�϶, 0.5��Ϊ�˶���ê��
			addChild(element, 1);

			line_elements.push_back(element);
		}
		_game_board.push_back(line_elements);
	}

	// ��ʼ��������
	_start_pos.row = -1;
	_start_pos.col = -1;

	_end_pos.row = -1;
	_end_pos.col = -1;

	// ��ʼ�ƶ�״̬
	_is_moving = false;

	// ��Ӵ����¼�����
	EventListenerTouchOneByOne *touch_listener = EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touch_listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	touch_listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this); // ����� _eventDispatcher

	return true;
}

ElementPos GameScene::getElementPosByCoordinate(float x, float y)
{
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	float row = (y - kBottonMargin) / element_size;
	float col = (x - kLeftMargin) / element_size;

	ElementPos pos;
	pos.row = row;
	pos.col = col;

	return pos;
}

void GameScene::swapElementPair(ElementPos p1, ElementPos p2)
{
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	// ��������Ԫ�أ�����任�������任
	Element *element1 = _game_board[p1.row][p1.col];
	Element *element2 = _game_board[p2.row][p2.col];

	// �ڴ��н���
	Vec2 temp_position = Vec2(element1->getPosition().x, element1->getPosition().y);
	int temp_type = element1->element_type;

	element1->setPosition(element2->getPosition());
	element1->element_type = element2->element_type;
	element1->setTexture(kElementImgArray[element2->element_type]);
	element1->setContentSize(Size(element_size, element_size)); 

	element2->setPosition(temp_position);
	element2->element_type = temp_type;
	element2->setTexture(kElementImgArray[temp_type]);
	element2->setContentSize(Size(element_size, element_size));

	// �����¼������������
	auto eliminate_set = checkEliminate();
	for (auto &pos : eliminate_set)
		CCLOG("set, row: %d, col: %d", pos.row, pos.col);
	batchEliminate(eliminate_set);
}

// ȫ��ɨ��
std::vector<ElementPos> GameScene::checkEliminate()
{
	std::vector<ElementPos> res_eliminate_list;
	// ���ü򵥵Ķ�άɨ����ȷ�����������Ľ�������������Ŵ��ڻ����3�������������õݹ�
	for (int i = 0; i < kRowNum; i++)
		for (int j = 0; j < kColNum; j++)
		{
			// �ж������Ƿ���ͬ
			if (i - 1 >= 0
				&& _game_board[i - 1][j]->element_type == _game_board[i][j]->element_type
				&& i + 1 < kRowNum
				&& _game_board[i + 1][j]->element_type == _game_board[i][j]->element_type)
			{
				// ������ŵ�������������������ӵ�
				if (!_game_board[i][j]->is_marked)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i][j]->is_marked = true;
				}
				if (!_game_board[i - 1][j]->is_marked)
				{
					ElementPos pos;
					pos.row = i - 1;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i - 1][j]->is_marked = true;
				}
				if (!_game_board[i + 1][j]->is_marked)
				{
					ElementPos pos;
					pos.row = i + 1;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i + 1][j]->is_marked = true;
				}
			}

			// �ж������Ƿ���ͬ
			if (j - 1 >= 0
				&& _game_board[i][j - 1]->element_type == _game_board[i][j]->element_type
				&& j + 1 < kColNum
				&& _game_board[i][j + 1]->element_type == _game_board[i][j]->element_type)
			{
				// ������ŵĺ�����������������ӵ�
				if (!_game_board[i][j]->is_marked)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i][j]->is_marked = true;
				}
				if (!_game_board[i][j - 1]->is_marked)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j - 1;

					res_eliminate_list.push_back(pos);
					_game_board[i][j - 1]->is_marked = true;
				}
				if (!_game_board[i][j + 1]->is_marked)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j + 1;

					res_eliminate_list.push_back(pos);
					_game_board[i][j + 1]->is_marked = true;
				}
			}
		}

	return res_eliminate_list;
}

void GameScene::batchEliminate(std::vector<ElementPos> &eliminate_list)
{
	// �л�����ͼ�겢��ʧ
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	for (auto &pos : eliminate_list)
	{
		_game_board[pos.row][pos.col]->setTexture(kEliminateStartImg); // ���ó���������
		_game_board[pos.row][pos.col]->setContentSize(Size(element_size, element_size)); // ���ڲ����óߴ�
		_game_board[pos.row][pos.col]->vanish();
	}
		
}

bool GameScene::onTouchBegan(Touch *touch, Event *event)
{
	//CCLOG("touch begin, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

	// ��¼��ʼ�����ľ�������
	_start_pos = getElementPosByCoordinate(touch->getLocation().x, touch->getLocation().y);
	CCLOG("start pos, row: %d, col: %d", _start_pos.row, _start_pos.col);
	// ÿ�δ�����һ���µ��ƶ�����
	_is_moving = true;
	
	return true;

}

void GameScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
	//CCLOG("touch moved, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

	// ���ݴ����ƶ��ķ������������飨ʵ���ϻ�����ͨ���������������ʵ�֣�

	// �������λ�ƣ���ק���飬ע�ⷶΧ
	if (_start_pos.row > -1 && _start_pos.row < kRowNum
		&& _start_pos.col > -1 && _start_pos.col < kColNum)
	{
		// ֻ��ˮƽ����ֵ����ά�Ƚ���λ��, �����ƶ�֮��̶�һ������
		//float x_delta = touch->getDelta().x;
		//float y_delta = touch->getDelta().y;

		// ͨ���ж��ƶ��������λ�����ĸ���Χ�������ƶ��ķ���
		Vec2 cur_loacation = touch->getLocation();
		ElementPos cur_pos = getElementPosByCoordinate(cur_loacation.x, cur_loacation.y);
		
		
		if (_is_moving)
		{
			// ����ƫ�Ʒ��򽻻�����

			bool is_need_swap = false;
			
			CCLOG("cur pos, row: %d, col: %d", cur_pos.row, cur_pos.col);
			if (_start_pos.col + 1 == cur_pos.col && _start_pos.row == cur_pos.row) // ˮƽ����
				is_need_swap = true;
			else if (_start_pos.col - 1 == cur_pos.col && _start_pos.row == cur_pos.row) // ˮƽ����
				is_need_swap = true;
			else if (_start_pos.row + 1 == cur_pos.row && _start_pos.col == cur_pos.col) // ��ֱ����
				is_need_swap = true;
			else if (_start_pos.row - 1 == cur_pos.row && _start_pos.col == cur_pos.col) // ��ֱ����
				is_need_swap = true;

			if (is_need_swap)
			{
				// ִ�н���
				swapElementPair(_start_pos, cur_pos);

				// �ع���ƶ�״̬
				_is_moving = false;
			}
		}

	}
}

void GameScene::onTouchEnded(Touch *touch, Event *event)
{
	//CCLOG("touch end, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);
	_is_moving = false;
}

void GameScene::onEnter()
{
	// ������layer onenter ���ܲ�׽�����¼�
	Layer::onEnter();
	CCLOG("enter game scene");
}

void GameScene::onExit()
{
	Layer::onExit();
	CCLOG("exit game scene");
}