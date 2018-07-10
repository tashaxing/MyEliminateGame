#include "SimpleAudioEngine.h"
#include "game_scene.h"
#include "element.h"

USING_NS_CC;

// �����еĲ�Σ����ִ�����ϲ�
const int kBackGroundLevel = 0; // ������
const int kGameBoardLevel = 1;  // ʵ�ʵ���Ϸ�����
const int kMenuLevel = 5; // �˵���

// ���������ļ�������ֵ��������
const std::vector<std::string> kElementImgArray{
	"images/diamond_red.png",
	"images/diamond_green.png",
	"images/diamond_blue.png",
	"images/candy_red.png",
	"images/candy_green.png",
	"images/candy_blue.png"
};

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

// ������״̬ö��
const int kEliminateInitFlag = 0;
const int kEliminateOneReadyFlag = 1;
const int KEliminateTwoReadyFlag = 2;

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
	addChild(game_background, kBackGroundLevel);

	// ��ʼ����Ϸ��ͼ
	for (int i = 0; i < kRowNum; i++)
	{
		std::vector<ElementProto> line_elements;
		for (int j = 0; j < kRowNum; j++)
		{
			ElementProto element_proto;
			element_proto.type = kElementEliminateType; // ��ʼ���ó�����״̬�����ں�������
			element_proto.marked = false;

			line_elements.push_back(element_proto);
		}
		_game_board.push_back(line_elements);
	}
		
	drawGameBoard();

	// ��ʼ��������
	_start_pos.row = -1;
	_start_pos.col = -1;

	_end_pos.row = -1;
	_end_pos.col = -1;

	// ��ʼ�ƶ�״̬
	_is_moving = false;
	_is_can_touch = true;
	_is_can_elimate = 0; // 0, 1, 2�����ȼ���0Ϊ��ʼ��1��ʾһ������ready��2��ʾ��������ready����������

	// ��Ӵ����¼�����
	EventListenerTouchOneByOne *touch_listener = EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touch_listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	touch_listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this); // ����� _eventDispatcher

	// Ĭ����Ⱦѭ��������
	scheduleUpdate();

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

void GameScene::fillGameBoard(int row, int col)
{
	// �����߽��򷵻�
	if (row == -1 || row == kRowNum || col == -1 || col == kColNum)
		return;

	int random_type = getRandomSpriteIndex(kElementImgArray.size());

	if (_game_board[row][col].type == kElementEliminateType)
	{
		_game_board[row][col].type = random_type;
		
		if (!hasEliminate())
		{
			// �ĸ�����ݹ����
			fillGameBoard(row + 1, col);
			fillGameBoard(row - 1, col);
			fillGameBoard(row, col - 1);
			fillGameBoard(row, col + 1);
		}
		else
			_game_board[row][col].type = kElementEliminateType; // ��ԭ
	}
}

void GameScene::drawGameBoard()
{
	srand(unsigned(time(0))); // ��ʼ�������������

	// �����ڴ������ɣ���֤��ʼû�п�������
	

	// �����Ļ�ߴ糣��(�������ຯ�����ȡ)
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

	// ����������������Ϸ�߼���������
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;
	
	for (int i = 0; i < kRowNum; i++)
	{
		std::vector<ElementProto> line_elements;
		for (int j = 0; j < kColNum; j++)
		{
			// ������ɾ�������
			int random_type = getRandomSpriteIndex(kElementImgArray.size()); 

			// �ڴ���Ӿ���ṹ��
			ElementProto element_proto;
			element_proto.type = random_type;
			element_proto.marked = false;
			_game_board[i][j] = element_proto;

			Element *element = Element::create();
			
			element->element_type = random_type;
			element->setTexture(kElementImgArray[random_type]); // ����������	
			element->setContentSize(Size(element_size, element_size)); // ���ڲ����óߴ�

			// ��ӵ�����Ч
			Point init_position(kLeftMargin + (j + 0.5) * element_size, kBottonMargin + (i + 0.5) * element_size + 0.5 * element_size);
			element->setPosition(init_position);
			Point real_position(kLeftMargin + (j + 0.5) * element_size, kBottonMargin + (i + 0.5) * element_size);
			Sequence *sequence = Sequence::create(MoveTo::create(0.5, real_position), CallFunc::create([=]() {
				element->setPosition(real_position); // lambda�ص�������������ʵλ��
			}), NULL);
			element->runAction(sequence);
		
			std::string elment_name = StringUtils::format("%d_%d", i, j);
			element->setName(elment_name); // ÿ�����澫���һ��Ψһ�����ֱ�ű��ں���Ѱ��
			addChild(element, kGameBoardLevel);	
		}
	}
}

void GameScene::fillVacantElements()
{

}

void GameScene::swapElementPair(ElementPos p1, ElementPos p2, bool is_reverse)
{
	// ����ʱ��ֹ�ɴ���״̬
	_is_can_touch = false;

	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	// �������߼�����3�����
	// �ڴ棬��Ϸ����㣬���������
	// ˳����Ҫ���ݷ�Ӧ�ٶ����ȵ�����ͬ�����첽

	// ���ԭʼ���������Ϣ
	std::string name1 = StringUtils::format("%d_%d", p1.row, p1.col);
	std::string name2 = StringUtils::format("%d_%d", p2.row, p2.col);

	Element *element1 = (Element *)getChildByName(name1);
	Element *element2 = (Element *)getChildByName(name2);

	Point position1 = element1->getPosition();
	Point position2 = element2->getPosition();

	int type1 = element1->element_type;
	int type2 = element2->element_type;

	log(is_reverse ? "==== reverse move ====" : "==== normal move ====");

	log("before move");

	log("p1 name: %s", element1->getName().c_str());
	log("p2 name: %s", element2->getName().c_str());

	log("position1, x: %f, y: %f", element1->getPosition().x, element1->getPosition().y);
	log("position2, x: %f, y: %f", element2->getPosition().x, element2->getPosition().y);

	// ---- ʵ�ʽ���
	// �ڴ��н�����������
	std::swap(_game_board[p1.row][p1.col], _game_board[p2.row][p2.col]);

	// �ƶ�����, move action���������position
	float delay_time = is_reverse ? 0.5 : 0;
	DelayTime *move_delay = DelayTime::create(delay_time); // ���򽻻���Ҫ��ʱ
	
	MoveTo *move_1to2 = MoveTo::create(0.2, position2);
	MoveTo *move_2to1 = MoveTo::create(0.2, position1);

	log("after move");
	element1->runAction(Sequence::create(move_delay, move_1to2, CallFunc::create([=]() {
		// lambda ���ʽ�ص���ע��Ҫ�� = �����ⲿָ��
		// ����λ�ã�
		log("e1 moved");
		element1->setPosition(position2);
		// ��������
		element1->setName(name2);

		_is_can_elimate++;

		log("p1 name: %s", element1->getName().c_str());
		log("position1, x: %f, y: %f", element1->getPosition().x, element1->getPosition().y);
	}), NULL));
	element2->runAction(Sequence::create(move_delay, move_2to1, CallFunc::create([=]() {
		log("e2 moved");
		element2->setPosition(position1);
		element2->setName(name1);

		_is_can_elimate++;

		log("p2 name: %s", element2->getName().c_str());
		log("position2, x: %f, y: %f", element2->getPosition().x, element2->getPosition().y);
	}), NULL));

	// �ָ�����״̬
	_is_can_touch = true;
}

bool GameScene::hasEliminate()
{
	bool has_elminate = false;
	for (int i = 0; i < kRowNum; i++)
	{
		for (int j = 0; j < kColNum; j++)
		{
			// Ҫ��֤����ͽ����ľ��鶼���Ǳ��Ϊ����
			if (_game_board[i][j].type != kElementEliminateType)
			{
				// �ж������Ƿ���ͬ
				if (i - 1 >= 0
					&& _game_board[i - 1][j].type != kElementEliminateType
					&& _game_board[i - 1][j].type == _game_board[i][j].type
					&& i + 1 < kRowNum
					&& _game_board[i + 1][j].type != kElementEliminateType
					&& _game_board[i + 1][j].type == _game_board[i][j].type)
				{
					has_elminate = true;
					break;
				}

				// �ж������Ƿ���ͬ
				if (j - 1 >= 0
					&& _game_board[i][j - 1].type != kElementEliminateType
					&& _game_board[i][j - 1].type == _game_board[i][j].type
					&& j + 1 < kColNum
					&& _game_board[i][j - 1].type != kElementEliminateType
					&& _game_board[i][j + 1].type == _game_board[i][j].type)
				{
					has_elminate = true;
					break;
				}
			}
		}

		if (has_elminate)
			break;
	}

	return has_elminate;
}

// ȫ��ɨ������������飬��ӵ�����������
std::vector<ElementPos> GameScene::getEliminateSet()
{
	std::vector<ElementPos> res_eliminate_list;
	// ���ü򵥵Ķ�άɨ����ȷ�����������Ľ�������������Ŵ��ڻ����3�������������õݹ�
	for (int i = 0; i < kRowNum; i++)
		for (int j = 0; j < kColNum; j++)
		{
			// �ж������Ƿ���ͬ
			if (i - 1 >= 0
				&& _game_board[i - 1][j].type == _game_board[i][j].type
				&& i + 1 < kRowNum
				&& _game_board[i + 1][j].type == _game_board[i][j].type)
			{
				// ������ŵ�������������������ӵĺ��������ģ���Ȼ����䣬���Ǳ��������
				if (!_game_board[i][j].marked && _game_board[i][j].type != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i][j].marked = true;
				}
				if (!_game_board[i - 1][j].marked && _game_board[i - 1][j].type != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i - 1;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i - 1][j].marked = true;
				}
				if (!_game_board[i + 1][j].marked && _game_board[i + 1][j].type != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i + 1;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i + 1][j].marked = true;
				}
			}

			// �ж������Ƿ���ͬ
			if (j - 1 >= 0
				&& _game_board[i][j - 1].type == _game_board[i][j].type
				&& j + 1 < kColNum
				&& _game_board[i][j + 1].type == _game_board[i][j].type)
			{
				// ������ŵĺ�����������������ӵ�
				if (!_game_board[i][j].marked && _game_board[i][j].type != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j;

					res_eliminate_list.push_back(pos);
					_game_board[i][j].marked = true;
				}
				if (!_game_board[i][j - 1].marked && _game_board[i][j - 1].type != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j - 1;

					res_eliminate_list.push_back(pos);
					_game_board[i][j - 1].marked = true;
				}
				if (!_game_board[i][j + 1].marked && _game_board[i][j + 1].type != kElementEliminateType)
				{
					ElementPos pos;
					pos.row = i;
					pos.col = j + 1;

					res_eliminate_list.push_back(pos);
					_game_board[i][j + 1].marked = true;
				}
			}
		}

	return res_eliminate_list;
}

void GameScene::batchEliminate(const std::vector<ElementPos> &eliminate_list)
{
	// �л�����ͼ�겢��ʧ
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	for (auto &pos : eliminate_list)
	{
		std::string elment_name = StringUtils::format("%d_%d", pos.row, pos.col);
		Element *element = (Element *)(getChildByName(elment_name));
		_game_board[pos.row][pos.col].type = kElementEliminateType; // ��ǳ���������
		element->setTexture(kEliminateStartImg); // ���ó���������
		element->setContentSize(Size(element_size, element_size)); // ���ڲ����óߴ�
		element->vanish();
	}
		
	// �½��������հ� type, marked
	
}


ElementPos GameScene::checkGameHint()
{
	// ȫ��ɨ�裬�����ƶ�ÿ��Ԫ�ص��ĸ����������û�п������ģ�����Ϸ���뽩��

	// ��ʼ����ʾ��
	ElementPos game_hint_point;
	game_hint_point.row = -1;
	game_hint_point.col = -1;

	for (int i = 0; i < kRowNum; i++)
	{
		for (int j = 0; j < kColNum; j++)
		{
			// ��
			if (i < kRowNum - 1)
			{
				// �������жϣ�Ȼ���ٽ�������
				std::swap(_game_board[i][j], _game_board[i + 1][j]);
				if (hasEliminate())
				{
					game_hint_point.row = i;
					game_hint_point.col = j;

					// ע��������Ȼ�������ڴ����ݣ���������flag�����ǿ��Զ�����״̬�����Բ���Ӱ�쵽��Ϸ
					std::swap(_game_board[i][j], _game_board[i + 1][j]);
					break;
				}
				std::swap(_game_board[i][j], _game_board[i + 1][j]);
			}

			// ��
			if (i > 0)
			{
				std::swap(_game_board[i][j], _game_board[i - 1][j]);
				if (hasEliminate())
				{
					game_hint_point.row = i;
					game_hint_point.col = j;

					std::swap(_game_board[i][j], _game_board[i - 1][j]);
					break; // �ҵ�һ���������
				}
				std::swap(_game_board[i][j], _game_board[i - 1][j]);
			}

			// ��
			if (j > 0)
			{
				std::swap(_game_board[i][j], _game_board[i][j - 1]);
				if (hasEliminate())
				{
					game_hint_point.row = i;
					game_hint_point.col = j;

					std::swap(_game_board[i][j], _game_board[i][j - 1]);
					break;
				}
				std::swap(_game_board[i][j], _game_board[i][j - 1]);
			}

			// �� 
			if (j < kColNum - 1)
			{
				std::swap(_game_board[i][j], _game_board[i][j + 1]);
				if (hasEliminate())
				{
					game_hint_point.row = i;
					game_hint_point.col = j;

					std::swap(_game_board[i][j], _game_board[i][j + 1]);
					break;
				}
				std::swap(_game_board[i][j], _game_board[i][j + 1]);
			}
		}

		// ����жϲ��ǽ��֣�������ѭ��
		if (game_hint_point.row != -1 && game_hint_point.col != -1)
			break;
	}

	// ���������о��鶼�Ҳ�����������
	return game_hint_point;
}

void GameScene::update(float dt)
{
	// ��Ҫȷ��������
	if (_start_pos.row == -1 && _start_pos.col == -1
		&& _end_pos.row == -1 && _end_pos.col == -1)
		_is_can_elimate = kEliminateInitFlag;

	log("eliminate flag: %d", _is_can_elimate);

	// ÿ֡����Ƿ񽩾�
	ElementPos game_hint_point = checkGameHint();
	if (game_hint_point.row == -1 && game_hint_point.col == -1)
		log("the game is dead");
	else
		log("game hint point: row %d, col %d", game_hint_point.row, game_hint_point.col);

	// �����������ж��Ƿ��������
	if (_is_can_elimate == KEliminateTwoReadyFlag)
	{
		auto eliminate_set = getEliminateSet();
		if (!eliminate_set.empty())
		{
			batchEliminate(eliminate_set);

			// ������ϣ���ԭ��־λ
			_is_can_elimate = kEliminateInitFlag; 

			// ��λ�ƶ���ʼλ��
			_start_pos.row = -1;
			_start_pos.col = -1;

			_end_pos.row = -1;
			_end_pos.col = -1;
		}
		else
		{
			// û�п������ģ�����ս���������Ҫ��������
			if (_start_pos.row >= 0 && _start_pos.row < kRowNum && _start_pos.col >= 0 && _start_pos.col < kColNum
				&&_end_pos.row >= 0 && _end_pos.row < kRowNum && _end_pos.row >= 0 && _start_pos.col < kColNum
				&& (_start_pos.row != _end_pos.row || _start_pos.col != _end_pos.col))
			{
				// ������ϣ���ԭ��־λ��Ϊ���򽻻�׼��
				_is_can_elimate = kEliminateInitFlag;
				swapElementPair(_start_pos, _end_pos, true);

				// ��λ�ƶ���ʼλ��
				_start_pos.row = -1;
				_start_pos.col = -1;

				_end_pos.row = -1;
				_end_pos.col = -1;
			}
				
		}
	}
}

bool GameScene::onTouchBegan(Touch *touch, Event *event)
{
	//log("touch begin, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);
	// ֻ���ڿɴ��������²ſ���
	if (_is_can_touch)
	{
		// ��¼��ʼ�����ľ�������
		_start_pos = getElementPosByCoordinate(touch->getLocation().x, touch->getLocation().y);
		log("start pos, row: %d, col: %d", _start_pos.row, _start_pos.col);
		// ÿ�δ�����һ���µ��ƶ�����
		_is_moving = true;
	}
	
	return true;

}

void GameScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
	//log("touch moved, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

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
			_end_pos = getElementPosByCoordinate(cur_loacation.x, cur_loacation.y);

			if (_is_moving)
			{
				// ����ƫ�Ʒ��򽻻�����
				bool is_need_swap = false;

				log("cur pos, row: %d, col: %d", _end_pos.row, _end_pos.col);
				if (_start_pos.col + 1 == _end_pos.col && _start_pos.row == _end_pos.row) // ˮƽ����
					is_need_swap = true;
				else if (_start_pos.col - 1 == _end_pos.col && _start_pos.row == _end_pos.row) // ˮƽ����
					is_need_swap = true;
				else if (_start_pos.row + 1 == _end_pos.row && _start_pos.col == _end_pos.col) // ��ֱ����
					is_need_swap = true;
				else if (_start_pos.row - 1 == _end_pos.row && _start_pos.col == _end_pos.col) // ��ֱ����
					is_need_swap = true;

				if (is_need_swap)
				{
					// ִ�н���
					swapElementPair(_start_pos, _end_pos, false);

					// �ع���ƶ�״̬
					_is_moving = false;
				}
			}

		}
	}
}

void GameScene::onTouchEnded(Touch *touch, Event *event)
{
	//log("touch end, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);
	_is_moving = false;
}

void GameScene::onEnter()
{
	// ������layer onenter ���ܲ�׽�����¼�
	Layer::onEnter();
	log("enter game scene");
}

void GameScene::onExit()
{
	Layer::onExit();
	log("exit game scene");
}