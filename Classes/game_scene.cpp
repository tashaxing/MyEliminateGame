#include "SimpleAudioEngine.h"
#include "game_scene.h"
#include "element.h"

USING_NS_CC;

// 精灵纹理文件
const std::vector<std::string> kElementImgArray{
	"images/diamond_red.png",
	"images/diamond_green.png",
	"images/diamond_blue.png",
	"images/candy_red.png",
	"images/candy_green.png",
	"images/candy_blue.png"
};

// 消除时候纹理
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
			element->setPosition(kLeftMargin + (j + 0.5) * element_size, kBottonMargin + (i + 0.5) * element_size);
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
	// 交换两个元素，矩阵变换，动画变换

}

// 全盘扫描
std::vector<ElementPos> GameScene::checkEliminate()
{
	std::vector<ElementPos> res_eliminate_list;
	// 采用简单的二维扫描来确定可以三消的结果集，不用递归
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
		_game_board[pos.row][pos.col]->setTexture(kEliminateStartImg); // 设置成消除纹理
		_game_board[pos.row][pos.col]->setContentSize(Size(element_size, element_size)); // 在内部设置尺寸
		_game_board[pos.row][pos.col]->vanish();
	}
		
}

bool GameScene::onTouchBegan(Touch *touch, Event *event)
{
	CCLOG("touch begin, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

	// 记录开始触摸的精灵坐标
	_start_pos = getElementPosByCoordinate(touch->getLocation().x, touch->getLocation().y);
	// 每次触碰算一次新的移动过程
	_is_moving = true;
	
	auto eliminate_set = checkEliminate();
	for (auto &pos : eliminate_set)
		CCLOG("set, row: %d, col: %d", pos.row, pos.col);
	batchEliminate(eliminate_set);

	return true;

}

void GameScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
	//CCLOG("touch moved, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

	// 根据触摸移动的方向来交换精灵（实际上还可以通过点击两个精灵来实现）

	// 计算相对位移，拖拽精灵，注意范围
	if (_start_pos.row > -1 && _start_pos.row < kRowNum
		&& _start_pos.col > -1 && _start_pos.col < kColNum)
	{
		// 只有在game board范围的精灵坐标才进行位移
		Element *start_element = _game_board[_start_pos.row][_start_pos.col];

		// 只在水平和数值两个维度进行位移, 并且移动之后固定一个方向
		float x_delta = touch->getDelta().x;
		float y_delta = touch->getDelta().y;

		Vec2 start_element_delta;
		
		if (_is_moving)
		{
			int delta_factor = 5;
			// 根据偏移方向交换精灵
			if (fabs(x_delta) > fabs(y_delta))
			{
				if (x_delta > delta_factor)
				{
					// 水平向右
				}
				else if (x_delta < -delta_factor)
				{
					// 水平向左
				}
			}
			else
			{
				if (y_delta > delta_factor)
				{
					// 竖直向上
				}
				else
				{
					// 竖直向下
				}
			}

			// 回归非移动状态
			_is_moving = false;
		}

	}
}

void GameScene::onTouchEnded(Touch *touch, Event *event)
{
	CCLOG("touch end, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);
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