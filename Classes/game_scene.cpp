#include "SimpleAudioEngine.h"
#include "game_scene.h"
#include "element.h"

USING_NS_CC;

// ���������ļ�������ֵ��������
const std::vector<std::string> kElementImgArray{
	"images/diamond_red.png",
	"images/diamond_green.png",
	"images/diamond_blue.png",
	"images/candy_red.png",
	"images/candy_green.png",
	"images/candy_blue.png"
};

// ���Ϊ����������
const int kElementMarkedType = -1;

// ����ʱ�����ͺ�����
const int kElementEliminateType = 10;
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

	// ����������������Ϸ�߼���������
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;
	srand(unsigned(time(0))); // ��ʼ�������������
	for (int i = 0; i < kRowNum; i++)
	{
		std::vector<int> line_elements;
		for (int j = 0; j < kColNum; j++)
		{
			Element *element = Element::create();
			int random_index = getRandomSpriteIndex(kElementImgArray.size()); // ������ɾ���
			element->setTexture(kElementImgArray[random_index]); // ����������	
			element->element_type = random_index;
			element->setContentSize(Size(element_size, element_size)); // ���ڲ����óߴ�
			element->setPosition(kLeftMargin + (j + 0.5) * element_size, kBottonMargin + (i + 0.5) * element_size); // FIXME:�����Ų����м�û�з�϶, 0.5��Ϊ�˶���ê��
			
			std::string elment_name = StringUtils::format("%d_%d", i, j);
			element->setName(elment_name); // ÿ�����澫���һ��Ψһ�����ֱ�ű��ں���Ѱ��
			addChild(element, 1);

			line_elements.push_back(element->element_type);
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
	_is_can_touch = true;

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
	// ����ʱ��ֹ�ɴ���״̬
	_is_can_touch = false;

	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	// ��������Ԫ�أ�����任�������任
	std::string name1 = StringUtils::format("%d_%d", p1.row, p1.col);
	std::string name2 = StringUtils::format("%d_%d", p2.row, p2.col);
	Node *element1 = getChildByName(name1);
	Node *element2 = getChildByName(name2);

	// ԭʼλ����Ϣ
	Vec2 position1 = element1->getPosition();
	Vec2 position2 = element2->getPosition();

	CCLOG("position1, x: %f, y: %f", position1.x, position2.y);
	CCLOG("position2, x: %f, y: %f", position2.x, position2.y);

	MoveTo *move_1to2 = MoveTo::create(0.2, position2);
	MoveTo *move_2to1 = MoveTo::create(0.2, position1);

	// ��������
	element1->runAction(move_1to2);
	element2->runAction(move_2to1);

	element1->setName(name2);
	element2->setName(name1);

	// ע���������任���ܵ��¶�������

	// �ڴ��н���
	std::swap(_game_board[p1.row][p1.col], _game_board[p2.row][p2.col]);

	// �ָ�����״̬
	_is_can_touch = true;
}

// ȫ��ɨ������������飬��ӵ�����������
std::vector<ElementPos> GameScene::checkEliminate()
{
	std::vector<ElementPos> res_eliminate_list;
	// ���ü򵥵Ķ�άɨ����ȷ�����������Ľ�������������Ŵ��ڻ����3�������������õݹ�
	for (int i = 0; i < kRowNum; i++)
		for (int j = 0; j < kColNum; j++)
		{
			// �ж������Ƿ���ͬ
			if (i - 1 >= 0
				&& _game_board[i - 1][j] == _game_board[i][j]
				&& i + 1 < kRowNum
				&& _game_board[i + 1][j] == _game_board[i][j])
			{
				// ������ŵ�������������������ӵĺ��������ģ���Ȼ����䣬���Ǳ��������
				if (_game_board[i][j] != kElementMarkedType && _game_board[i][j] != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i][j] = kElementMarkedType;
				}
				if (_game_board[i - 1][j] != kElementMarkedType && _game_board[i - 1][j] != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i - 1;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i - 1][j] = kElementMarkedType;
				}
				if (_game_board[i + 1][j] != kElementMarkedType && _game_board[i + 1][j] != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i + 1;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i + 1][j] = kElementMarkedType;
				}
			}

			// �ж������Ƿ���ͬ
			if (j - 1 >= 0
				&& _game_board[i][j - 1] == _game_board[i][j]
				&& j + 1 < kColNum
				&& _game_board[i][j + 1] == _game_board[i][j])
			{
				// ������ŵĺ�����������������ӵ�
				if (_game_board[i][j] != kElementMarkedType && _game_board[i][j] != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i][j] = kElementMarkedType;
				}
				if (_game_board[i][j - 1] != kElementMarkedType && _game_board[i][j - 1] != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j - 1;

					res_eliminate_list.push_back(pos);
					_game_board[i][j - 1] = kElementMarkedType;
				}
				if (_game_board[i][j + 1] != kElementMarkedType && _game_board[i][j + 1] != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j + 1;

					res_eliminate_list.push_back(pos);
					_game_board[i][j + 1] = kElementMarkedType;
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
		std::string elment_name = StringUtils::format("%d_%d", pos.row, pos.col);
		Element *element = (Element *)(getChildByName(elment_name));
		_game_board[pos.row][pos.col] = kElementEliminateType; // ��ǳ���������
		element->setTexture(kEliminateStartImg); // ���ó���������
		element->setContentSize(Size(element_size, element_size)); // ���ڲ����óߴ�
		element->vanish();
	}
		
	// �½��������հ�
	
}

void GameScene::fillVacantElements()
{
	
}

bool GameScene::checkGameDead()
{
	// ȫ��ɨ�裬�����ƶ�ÿ��Ԫ�ص��ĸ����������û�п������ģ�����Ϸ���뽩��
	bool is_game_dead = true;
	for (int i = 0; i < kRowNum; i++)
	{
		for (int j = 0; j < kColNum; j++)
		{
			// ��
			if (i < kRowNum - 1)
			{
				// �������жϣ�Ȼ���ٽ�������
				std::swap(_game_board[i][j], _game_board[i + 1][j]);
				auto eliminate_set = checkEliminate();
				if (!eliminate_set.empty())
				{
					is_game_dead = false;
					std::swap(_game_board[i][j], _game_board[i + 1][j]);
					break;
				}
				std::swap(_game_board[i][j], _game_board[i + 1][j]);
			}

			// ��
			if (i > 0)
			{
				std::swap(_game_board[i][j], _game_board[i - 1][j]);
				auto eliminate_set = checkEliminate();
				if (!eliminate_set.empty())
				{
					is_game_dead = false;
					std::swap(_game_board[i][j], _game_board[i - 1][j]);
					break;
				}
				std::swap(_game_board[i][j], _game_board[i - 1][j]);
			}

			// ��
			if (j > 0)
			{
				std::swap(_game_board[i][j], _game_board[i][j - 1]);
				auto eliminate_set = checkEliminate();
				if (!eliminate_set.empty())
				{
					is_game_dead = false;
					std::swap(_game_board[i][j], _game_board[i][j - 1]);
					break;
				}
				std::swap(_game_board[i][j], _game_board[i][j - 1]);
			}

			// �� 
			if (j < kColNum - 1)
			{
				std::swap(_game_board[i][j], _game_board[i][j + 1]);
				auto eliminate_set = checkEliminate();
				if (!eliminate_set.empty())
				{
					is_game_dead = false;
					std::swap(_game_board[i][j], _game_board[i][j + 1]);
					break;
				}
				std::swap(_game_board[i][j], _game_board[i][j + 1]);
			}
		}
	}

	// ���������о��鶼�Ҳ�����������
	return !is_game_dead;
}

bool GameScene::onTouchBegan(Touch *touch, Event *event)
{
	//CCLOG("touch begin, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);
	// ֻ���ڿɴ��������²ſ���
	if (_is_can_touch)
	{
		// ��¼��ʼ�����ľ�������
		_start_pos = getElementPosByCoordinate(touch->getLocation().x, touch->getLocation().y);
		CCLOG("start pos, row: %d, col: %d", _start_pos.row, _start_pos.col);
		// ÿ�δ�����һ���µ��ƶ�����
		_is_moving = true;
	}
	
	
	return true;

}

void GameScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
	//CCLOG("touch moved, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

	// ֻ���ڿɴ��������²ſ���
	if (_is_can_touch)
	{
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

					// �����¼�������������
					auto eliminate_set = checkEliminate();
					if (!eliminate_set.empty())
					{
						batchEliminate(eliminate_set);

						// ÿ�����������Ƿ񽩾�
						//if (checkGameDead())
							//CCLOG("the game is dead");
					}
					else
					{
						CCLOG("no available eliminate, swap back");
						//swapElementPair(_start_pos, cur_pos);
						swapElementPair(cur_pos, _start_pos);
					}

					// �ع���ƶ�״̬
					_is_moving = false;
				}
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