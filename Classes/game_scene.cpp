#include "SimpleAudioEngine.h"
#include "game_scene.h"
#include "element.h"

USING_NS_CC;

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
const int kElementEliminateType = -1;
const std::string kEliminateStartImg = "images/star.png";


// 界面边距
const float kLeftMargin = 20;
const float kRightMargin = 20;
const float kBottonMargin = 70;

// 精灵使数量
const int kRowNum = 8;
const int kColNum = 8;

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
	addChild(game_background, 0);

	// 添加消除对象矩阵
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;
	srand(unsigned(time(0))); // 初始化随机数发生器
	for (int i = 0; i < kRowNum; i++)
	{
		std::vector<Element *> line_elements;
		for (int j = 0; j < kColNum; j++)
		{
			Element *element = Element::create();
			int random_index = getRandomSpriteIndex(kElementImgArray.size()); // 随机生成精灵
			element->setTexture(kElementImgArray[random_index]); // 添加随机纹理	
			element->element_type = random_index;
			element->setContentSize(Size(element_size, element_size)); // 在内部设置尺寸
			element->setPosition(kLeftMargin + (j + 0.5) * element_size, kBottonMargin + (i + 0.5) * element_size); // FIXME:紧密排布，中间没有缝隙, 0.5是为了对齐锚点
			addChild(element, 1);

			line_elements.push_back(element);
		}
		_game_board.push_back(line_elements);
	}

	// 初始触摸坐标
	_start_pos.row = -1;
	_start_pos.col = -1;

	_end_pos.row = -1;
	_end_pos.col = -1;

	// 初始移动状态
	_is_moving = false;
	_is_can_touch = true;

	// 添加触摸事件监听
	EventListenerTouchOneByOne *touch_listener = EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touch_listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	touch_listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this); // 父类的 _eventDispatcher

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
	// 交换时禁止可触摸状态
	_is_can_touch = false;

	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	// 交换两个元素，矩阵变换，动画变换
	Element *element1 = _game_board[p1.row][p1.col];
	Element *element2 = _game_board[p2.row][p2.col];

	Vec2 position1 = element1->getPosition();
	Vec2 position2 = element2->getPosition();

	MoveTo *move_1to2 = MoveTo::create(0.2, position2);
	MoveTo *move_2to1 = MoveTo::create(0.2, position1);

	element1->runAction(move_1to2);
	element2->runAction(move_2to1);

	//// 内存中交换
	//Vec2 temp_position = Vec2(element1->getPosition().x, element1->getPosition().y);
	//int temp_type = element1->element_type;

	////element1->setPosition(element2->getPosition());
	//element1->element_type = element2->element_type;
	//element1->setTexture(kElementImgArray[element2->element_type]);
	//element1->setContentSize(Size(element_size, element_size)); 

	////element2->setPosition(temp_position);
	//element2->element_type = temp_type;
	//element2->setTexture(kElementImgArray[temp_type]);
	//element2->setContentSize(Size(element_size, element_size));

	// 恢复触摸状态
	_is_can_touch = true;
}

// 全盘扫描检查可消除精灵，添加到可消除集合
std::vector<ElementPos> GameScene::checkEliminate()
{
	std::vector<ElementPos> res_eliminate_list;
	// 采用简单的二维扫描来确定可以三消的结果集，横竖连着大于或等于3个就消除，不用递归
	for (int i = 0; i < kRowNum; i++)
		for (int j = 0; j < kColNum; j++)
		{
			// 判断上下是否相同
			if (i - 1 >= 0
				&& _game_board[i - 1][j]->element_type == _game_board[i][j]->element_type
				&& i + 1 < kRowNum
				&& _game_board[i + 1][j]->element_type == _game_board[i][j]->element_type)
			{
				// 添加连着的竖向三个，跳过已添加的
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

			// 判断左右是否相同
			if (j - 1 >= 0
				&& _game_board[i][j - 1]->element_type == _game_board[i][j]->element_type
				&& j + 1 < kColNum
				&& _game_board[i][j + 1]->element_type == _game_board[i][j]->element_type)
			{
				// 添加连着的横向三个，跳过已添加的
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
	// 切换精灵图标并消失
	const Size kScreenSize = Director::getInstance()->getVisibleSize();
	const Vec2 kScreenOrigin = Director::getInstance()->getVisibleOrigin();
	float element_size = (kScreenSize.width - kLeftMargin - kRightMargin) / kColNum;

	for (auto &pos : eliminate_list)
	{
		_game_board[pos.row][pos.col]->element_type = kElementEliminateType; // 标记成消除类型
		_game_board[pos.row][pos.col]->setTexture(kEliminateStartImg); // 设置成消除纹理
		_game_board[pos.row][pos.col]->setContentSize(Size(element_size, element_size)); // 在内部设置尺寸
		_game_board[pos.row][pos.col]->vanish();
	}
		
	// 下降精灵填充空白
	
}

void GameScene::fillVacantElements()
{
	
}

bool GameScene::checkGameDead()
{
	// 全盘扫描，尝试移动每个元素到四个方向，如果都没有可消除的，则游戏陷入僵局
	bool is_game_dead = true;
	for (int i = 0; i < kRowNum; i++)
	{
		for (int j = 0; j < kColNum; j++)
		{
			// 上
			if (i < kRowNum - 1)
			{
				// 交换后判断，然后再交换回来
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

			// 下
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

			// 左
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

			// 右 
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

	// 如果最后所有精灵都找不到可消除的
	return !is_game_dead;
}

bool GameScene::onTouchBegan(Touch *touch, Event *event)
{
	//CCLOG("touch begin, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);
	// 只有在可触摸条件下才可以
	if (_is_can_touch)
	{
		// 记录开始触摸的精灵坐标
		_start_pos = getElementPosByCoordinate(touch->getLocation().x, touch->getLocation().y);
		CCLOG("start pos, row: %d, col: %d", _start_pos.row, _start_pos.col);
		// 每次触碰算一次新的移动过程
		_is_moving = true;
	}
	
	
	return true;

}

void GameScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
	//CCLOG("touch moved, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

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
			ElementPos cur_pos = getElementPosByCoordinate(cur_loacation.x, cur_loacation.y);

			if (_is_moving)
			{
				// 根据偏移方向交换精灵

				bool is_need_swap = false;

				CCLOG("cur pos, row: %d, col: %d", cur_pos.row, cur_pos.col);
				if (_start_pos.col + 1 == cur_pos.col && _start_pos.row == cur_pos.row) // 水平向右
					is_need_swap = true;
				else if (_start_pos.col - 1 == cur_pos.col && _start_pos.row == cur_pos.row) // 水平向左
					is_need_swap = true;
				else if (_start_pos.row + 1 == cur_pos.row && _start_pos.col == cur_pos.col) // 竖直向上
					is_need_swap = true;
				else if (_start_pos.row - 1 == cur_pos.row && _start_pos.col == cur_pos.col) // 竖直向下
					is_need_swap = true;

				if (is_need_swap)
				{
					// 执行交换
					swapElementPair(_start_pos, cur_pos);

					// 交换事件后进行消除检查
					auto eliminate_set = checkEliminate();
					if (!eliminate_set.empty())
					{
						batchEliminate(eliminate_set);

						// 每次消除完检查是否僵局
						//if (checkGameDead())
							//CCLOG("the game is dead");
					}
					else
					{
						CCLOG("no available eliminate, swap back");
						swapElementPair(_start_pos, cur_pos);
					}

					// 回归非移动状态
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
	// 必须先layer onenter 才能捕捉触摸事件
	Layer::onEnter();
	CCLOG("enter game scene");
}

void GameScene::onExit()
{
	Layer::onExit();
	CCLOG("exit game scene");
}