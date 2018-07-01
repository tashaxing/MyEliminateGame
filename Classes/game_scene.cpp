#include "SimpleAudioEngine.h"
#include "game_scene.h"
#include "element.h"

USING_NS_CC;

// 精灵纹理文件
const auto kElementImgArray = std::vector<std::string>{
	"images/diamond_red.png",
	"images/diamond_green.png",
	"images/diamond_blue.png",
	"images/candy_red.png",
	"images/candy_green.png",
	"images/candy_blue.png"
};

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
	auto game_scene = Scene::create();
	auto game_layer = GameScene::create();
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
	auto game_background = Sprite::create("images/game_bg.jpg");
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
			auto element = Element::create();
			element->setTexture(kElementImgArray[getRandomSpriteIndex(kElementImgArray.size())]); // 添加随机纹理	
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
	auto touch_listener = EventListenerTouchOneByOne::create();
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

bool GameScene::onTouchBegan(Touch *touch, Event *event)
{
	CCLOG("touch begin, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

	// 记录开始触摸的精灵坐标
	_start_pos = getElementPosByCoordinate(touch->getLocation().x, touch->getLocation().y);
	
	return true;

}

void GameScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
	CCLOG("touch moved, x: %f, y: %f", touch->getLocation().x, touch->getLocation().y);

	static int move_direction = 0; // 0 水平，1 竖直

	// 计算相对位移，拖拽精灵
	if (_start_pos.row > -1 && _start_pos.row < kRowNum
		&& _start_pos.col > -1 && _start_pos.col < kColNum)
	{
		// 只有在game board范围的精灵坐标才进行位移
		auto start_element = _game_board[_start_pos.row][_start_pos.col];

		// 只在水平和数值两个维度进行位移, 并且移动之后固定一个方向
		float x_delta = touch->getDelta().x;
		float y_delta = touch->getDelta().y;

		Vec2 start_element_delta;
		
		if (!_is_moving)
		{
			if (fabs(x_delta) > fabs(y_delta))
				move_direction = 0;
			else
				move_direction = 1;

			_is_moving = true;
		}

		// 保持移动方向
		if (move_direction == 0)
		{
			start_element_delta.x = x_delta;
			start_element_delta.y = 0;
		}
		else
		{
			start_element_delta.x = 0;
			start_element_delta.y = y_delta;
		}

		// 获得目标坐标，和目标精灵
		Vec2 target_position = start_element->getPosition() + start_element_delta;
		start_element->setPosition(target_position);

		ElementPos target_pos = getElementPosByCoordinate(target_position.x, target_position.y);
		auto target_element = _game_board[target_pos.row][target_pos.col];

		if (target_pos.row != _start_pos.row || target_pos.col != target_pos.col)
		{
			// 如果移动到目标精灵范围内，则发生交换
			/*Vec2 target_element_delta;
			target_element_delta.x = -start_element_delta.x;
			target_element_delta.y = -start_element_delta.y;*/
			target_element->setPosition(start_element->getPosition());
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