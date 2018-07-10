#include "SimpleAudioEngine.h"
#include "game_scene.h"
#include "element.h"

USING_NS_CC;

// 场景中的层次，数字大的在上层
const int kBackGroundLevel = 0; // 背景层
const int kGameBoardLevel = 1;  // 实际的游戏精灵层
const int kMenuLevel = 5; // 菜单层

// 精灵纹理文件，索引值就是类型
const std::vector<std::string> kElementImgArray{
	"images/diamond_red.png",
	"images/diamond_green.png",
	"images/diamond_blue.png",
	"images/candy_red.png",
	"images/candy_green.png",
	"images/candy_blue.png"
};

// 消除时候类型和纹理
const int kElementEliminateType = 10;
const std::string kEliminateStartImg = "images/star.png";

// 界面边距
const float kLeftMargin = 20;
const float kRightMargin = 20;
const float kBottonMargin = 70;

// 精灵使数量
const int kRowNum = 8;
const int kColNum = 8;

// 可消除状态枚举
const int kEliminateInitFlag = 0;
const int kEliminateOneReadyFlag = 1;
const int KEliminateTwoReadyFlag = 2;

// 获得随机精灵纹理索引
int getRandomSpriteIndex(int len)
{
	return rand() % len;
}

// 实例化主场景和层
Scene *GameScene::createScene()
{
	Scene *game_scene = Scene::create();
	Layer *game_layer = GameScene::create();
	game_scene->addChild(game_layer);
	return game_scene;
}

// 初始化主场景
bool GameScene::init()
{
	if (!Layer::init())
		return false;

	// 获得屏幕尺寸常量(必须在类函数里获取)
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

	// 加载游戏界面背景
	Sprite *game_background = Sprite::create("images/game_bg.jpg");
	game_background->setPosition(kScreenOrigin.x + kScreenSize.width / 2, kScreenOrigin.y + kScreenSize.height / 2);
	addChild(game_background, kBackGroundLevel);

	// 初始化游戏地图
	for (int i = 0; i < kRowNum; i++)
	{
		std::vector<ElementProto> line_elements;
		for (int j = 0; j < kRowNum; j++)
		{
			ElementProto element_proto;
			element_proto.type = kElementEliminateType; // 初始化置成消除状态，便于后续生成
			element_proto.marked = false;

			line_elements.push_back(element_proto);
		}
		_game_board.push_back(line_elements);
	}
		
	drawGameBoard();

	// 初始触摸坐标
	_start_pos.row = -1;
	_start_pos.col = -1;

	_end_pos.row = -1;
	_end_pos.col = -1;

	// 初始移动状态
	_is_moving = false;
	_is_can_touch = true;
	_is_can_elimate = 0; // 0, 1, 2三个等级，0为初始，1表示一个精灵ready，2表示两个精灵ready，可以消除

	// 添加触摸事件监听
	EventListenerTouchOneByOne *touch_listener = EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touch_listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	touch_listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this); // 父类的 _eventDispatcher

	// 默认渲染循环调度器
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
	// 遇到边界则返回
	if (row == -1 || row == kRowNum || col == -1 || col == kColNum)
		return;

	int random_type = getRandomSpriteIndex(kElementImgArray.size());

	if (_game_board[row][col].type == kElementEliminateType)
	{
		_game_board[row][col].type = random_type;
		
		if (!hasEliminate())
		{
			// 四个方向递归调用
			fillGameBoard(row + 1, col);
			fillGameBoard(row - 1, col);
			fillGameBoard(row, col - 1);
			fillGameBoard(row, col + 1);
		}
		else
			_game_board[row][col].type = kElementEliminateType; // 还原
	}
}

void GameScene::drawGameBoard()
{
	srand(unsigned(time(0))); // 初始化随机数发生器

	// 先在内存中生成，保证初始没有可消除的
	

	// 获得屏幕尺寸常量(必须在类函数里获取)
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();

	// 添加消除对象矩阵，游戏逻辑与界面解耦
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;
	
	for (int i = 0; i < kRowNum; i++)
	{
		std::vector<ElementProto> line_elements;
		for (int j = 0; j < kColNum; j++)
		{
			// 随机生成精灵类型
			int random_type = getRandomSpriteIndex(kElementImgArray.size()); 

			// 内存添加精灵结构体
			ElementProto element_proto;
			element_proto.type = random_type;
			element_proto.marked = false;
			_game_board[i][j] = element_proto;

			Element *element = Element::create();
			
			element->element_type = random_type;
			element->setTexture(kElementImgArray[random_type]); // 添加随机纹理	
			element->setContentSize(Size(element_size, element_size)); // 在内部设置尺寸

			// 添加掉落特效
			Point init_position(kLeftMargin + (j + 0.5) * element_size, kBottonMargin + (i + 0.5) * element_size + 0.5 * element_size);
			element->setPosition(init_position);
			Point real_position(kLeftMargin + (j + 0.5) * element_size, kBottonMargin + (i + 0.5) * element_size);
			Sequence *sequence = Sequence::create(MoveTo::create(0.5, real_position), CallFunc::create([=]() {
				element->setPosition(real_position); // lambda回调，设置最终真实位置
			}), NULL);
			element->runAction(sequence);
		
			std::string elment_name = StringUtils::format("%d_%d", i, j);
			element->setName(elment_name); // 每个界面精灵给一个唯一的名字标号便于后续寻找
			addChild(element, kGameBoardLevel);	
		}
	}
}

void GameScene::fillVacantElements()
{

}

void GameScene::swapElementPair(ElementPos p1, ElementPos p2, bool is_reverse)
{
	// 交换时禁止可触摸状态
	_is_can_touch = false;

	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	// 交换的逻辑，分3个层次
	// 内存，游戏精灵层，动画精灵层
	// 顺序需要根据反应速度由先到后，由同步到异步

	// 获得原始精灵相关信息
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

	// ---- 实际交换
	// 内存中交换精灵类型
	std::swap(_game_board[p1.row][p1.col], _game_board[p2.row][p2.col]);

	// 移动动画, move action并不会更新position
	float delay_time = is_reverse ? 0.5 : 0;
	DelayTime *move_delay = DelayTime::create(delay_time); // 反向交换需要延时
	
	MoveTo *move_1to2 = MoveTo::create(0.2, position2);
	MoveTo *move_2to1 = MoveTo::create(0.2, position1);

	log("after move");
	element1->runAction(Sequence::create(move_delay, move_1to2, CallFunc::create([=]() {
		// lambda 表达式回调，注意要用 = 捕获外部指针
		// 重设位置，
		log("e1 moved");
		element1->setPosition(position2);
		// 交换名称
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

	// 恢复触摸状态
	_is_can_touch = true;
}

bool GameScene::hasEliminate()
{
	bool has_elminate = false;
	for (int i = 0; i < kRowNum; i++)
	{
		for (int j = 0; j < kColNum; j++)
		{
			// 要保证精灵和交换的精灵都不是标记为消除
			if (_game_board[i][j].type != kElementEliminateType)
			{
				// 判断上下是否相同
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

				// 判断左右是否相同
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

// 全盘扫描检查可消除精灵，添加到可消除集合
std::vector<ElementPos> GameScene::getEliminateSet()
{
	std::vector<ElementPos> res_eliminate_list;
	// 采用简单的二维扫描来确定可以三消的结果集，横竖连着大于或等于3个就消除，不用递归
	for (int i = 0; i < kRowNum; i++)
		for (int j = 0; j < kColNum; j++)
		{
			// 判断上下是否相同
			if (i - 1 >= 0
				&& _game_board[i - 1][j].type == _game_board[i][j].type
				&& i + 1 < kRowNum
				&& _game_board[i + 1][j].type == _game_board[i][j].type)
			{
				// 添加连着的竖向三个，跳过已添加的和已消除的（虽然有填充，但是保险起见）
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

			// 判断左右是否相同
			if (j - 1 >= 0
				&& _game_board[i][j - 1].type == _game_board[i][j].type
				&& j + 1 < kColNum
				&& _game_board[i][j + 1].type == _game_board[i][j].type)
			{
				// 添加连着的横向三个，跳过已添加的
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
	// 切换精灵图标并消失
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	for (auto &pos : eliminate_list)
	{
		std::string elment_name = StringUtils::format("%d_%d", pos.row, pos.col);
		Element *element = (Element *)(getChildByName(elment_name));
		_game_board[pos.row][pos.col].type = kElementEliminateType; // 标记成消除类型
		element->setTexture(kEliminateStartImg); // 设置成消除纹理
		element->setContentSize(Size(element_size, element_size)); // 在内部设置尺寸
		element->vanish();
	}
		
	// 下降精灵填充空白 type, marked
	
}


ElementPos GameScene::checkGameHint()
{
	// 全盘扫描，尝试移动每个元素到四个方向，如果都没有可消除的，则游戏陷入僵局

	// 初始化提示点
	ElementPos game_hint_point;
	game_hint_point.row = -1;
	game_hint_point.col = -1;

	for (int i = 0; i < kRowNum; i++)
	{
		for (int j = 0; j < kColNum; j++)
		{
			// 上
			if (i < kRowNum - 1)
			{
				// 交换后判断，然后再交换回来
				std::swap(_game_board[i][j], _game_board[i + 1][j]);
				if (hasEliminate())
				{
					game_hint_point.row = i;
					game_hint_point.col = j;

					// 注意这里虽然交换了内存数据，但是消除flag并不是可以动画的状态，所以不会影响到游戏
					std::swap(_game_board[i][j], _game_board[i + 1][j]);
					break;
				}
				std::swap(_game_board[i][j], _game_board[i + 1][j]);
			}

			// 下
			if (i > 0)
			{
				std::swap(_game_board[i][j], _game_board[i - 1][j]);
				if (hasEliminate())
				{
					game_hint_point.row = i;
					game_hint_point.col = j;

					std::swap(_game_board[i][j], _game_board[i - 1][j]);
					break; // 找到一个点就跳出
				}
				std::swap(_game_board[i][j], _game_board[i - 1][j]);
			}

			// 左
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

			// 右 
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

		// 如果判断不是僵局，则跳出循环
		if (game_hint_point.row != -1 && game_hint_point.col != -1)
			break;
	}

	// 如果最后所有精灵都找不到可消除的
	return game_hint_point;
}

void GameScene::update(float dt)
{
	// 需要确保标记清除
	if (_start_pos.row == -1 && _start_pos.col == -1
		&& _end_pos.row == -1 && _end_pos.col == -1)
		_is_can_elimate = kEliminateInitFlag;

	log("eliminate flag: %d", _is_can_elimate);

	// 每帧检查是否僵局
	ElementPos game_hint_point = checkGameHint();
	if (game_hint_point.row == -1 && game_hint_point.col == -1)
		log("the game is dead");
	else
		log("game hint point: row %d, col %d", game_hint_point.row, game_hint_point.col);

	// 交换动画后判断是否可以消除
	if (_is_can_elimate == KEliminateTwoReadyFlag)
	{
		auto eliminate_set = getEliminateSet();
		if (!eliminate_set.empty())
		{
			batchEliminate(eliminate_set);

			// 消除完毕，还原标志位
			_is_can_elimate = kEliminateInitFlag; 

			// 复位移动起始位置
			_start_pos.row = -1;
			_start_pos.col = -1;

			_end_pos.row = -1;
			_end_pos.col = -1;
		}
		else
		{
			// 没有可消除的，如果刚交换过，需要交换回来
			if (_start_pos.row >= 0 && _start_pos.row < kRowNum && _start_pos.col >= 0 && _start_pos.col < kColNum
				&&_end_pos.row >= 0 && _end_pos.row < kRowNum && _end_pos.row >= 0 && _start_pos.col < kColNum
				&& (_start_pos.row != _end_pos.row || _start_pos.col != _end_pos.col))
			{
				// 消除完毕，还原标志位，为反向交换准备
				_is_can_elimate = kEliminateInitFlag;
				swapElementPair(_start_pos, _end_pos, true);

				// 复位移动起始位置
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
	// 只有在可触摸条件下才可以
	if (_is_can_touch)
	{
		// 记录开始触摸的精灵坐标
		_start_pos = getElementPosByCoordinate(touch->getLocation().x, touch->getLocation().y);
		log("start pos, row: %d, col: %d", _start_pos.row, _start_pos.col);
		// 每次触碰算一次新的移动过程
		_is_moving = true;
	}
	
	return true;

}

void GameScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
	//log("touch moved, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

	// 只有在可触摸条件下才可以
	if (_is_can_touch)
	{
		// 根据触摸移动的方向来交换精灵（实际上还可以通过点击两个精灵来实现）

		// 计算相对位移，拖拽精灵，注意范围
		if (_start_pos.row > -1 && _start_pos.row < kRowNum
			&& _start_pos.col > -1 && _start_pos.col < kColNum)
		{
			// 只在水平和数值两个维度进行位移, 并且移动之后固定一个方向
			//float x_delta = touch->getDelta().x;
			//float y_delta = touch->getDelta().y;

			// 通过判断移动后触摸点的位置在哪个范围来决定移动的方向
			Vec2 cur_loacation = touch->getLocation();
			_end_pos = getElementPosByCoordinate(cur_loacation.x, cur_loacation.y);

			if (_is_moving)
			{
				// 根据偏移方向交换精灵
				bool is_need_swap = false;

				log("cur pos, row: %d, col: %d", _end_pos.row, _end_pos.col);
				if (_start_pos.col + 1 == _end_pos.col && _start_pos.row == _end_pos.row) // 水平向右
					is_need_swap = true;
				else if (_start_pos.col - 1 == _end_pos.col && _start_pos.row == _end_pos.row) // 水平向左
					is_need_swap = true;
				else if (_start_pos.row + 1 == _end_pos.row && _start_pos.col == _end_pos.col) // 竖直向上
					is_need_swap = true;
				else if (_start_pos.row - 1 == _end_pos.row && _start_pos.col == _end_pos.col) // 竖直向下
					is_need_swap = true;

				if (is_need_swap)
				{
					// 执行交换
					swapElementPair(_start_pos, _end_pos, false);

					// 回归非移动状态
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
	// 必须先layer onenter 才能捕捉触摸事件
	Layer::onEnter();
	log("enter game scene");
}

void GameScene::onExit()
{
	Layer::onExit();
	log("exit game scene");
}