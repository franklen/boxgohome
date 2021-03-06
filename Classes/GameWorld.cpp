#include "GameWorld.h"
#include "VisibleRect.h"
#include "BoxSprite.h"
#include "Background.h"
#include "GameMap.h"
#include "ArrowSprite.h"
#include "GameUI.h"
#include "ContactLogic.h"
#include "DialogManager.h"
#include "LevelState.h"
#include "TailEffect.h"
#include "LevelSelector.h"
#include "AdManager.h"
#include "Utils.h"

GameWorld::GameWorld()
:m_pArrowSprite(nullptr),
m_pBoxSprite(nullptr),
m_pContactLogic(nullptr),
m_pGameMap(nullptr),
m_pGameUI(nullptr)
{

} 

GameWorld::~GameWorld()
{
	
}
 
Scene* GameWorld::createScene()
{
	auto scene = Scene::createWithPhysics();
	if (scene)
	{	
		auto layer = GameWorld::create();
		scene->addChild(layer);
		return scene;
	}
	return nullptr;
}




bool GameWorld::init()
{
	if (!Node::init()) return false;

	this->initBackground();
	this->initGameMap();

	this->initBoxSprite();
	this->initArrowSprite();
	this->scheduleUpdate();
	this->initGameUI();
	this->initContactLogic();
    
	return true;
}

bool GameWorld::initContactLogic()
{
	this->m_pContactLogic = ContactLogic::create(this);
	this->addChild(this->m_pContactLogic);
	return true;
}

bool GameWorld::initGameUI()
{
	m_pGameUI = GameUI::create(this);
	this->addChild(m_pGameUI);
	return true;
}

bool GameWorld::initArrowSprite()
{
	m_pArrowSprite = ArrowSprite::create();
	this->addChild(m_pArrowSprite);
	m_pArrowSprite->setPosition(m_pBoxSprite->getPosition());
	return true;
}

bool GameWorld::initBoxSprite()
{
	ValueMap valueMap = this->m_pGameMap->getHeroValueMap();
	this->m_pBoxSprite = BoxSprite::create(valueMap);
	Point point = m_pBoxSprite->getPosition() + m_pGameMap->getPosition() - m_pGameMap->getContentSize() / 2;
	this->m_pBoxSprite->setPosition(point);
	if (Utils::getTrailSwitch())
	{
		auto tail = TailEffect::create("maps/hero/avatar_01.png");
		tail->bindSprite(this->m_pBoxSprite);
		this->addChild(tail);
	}
	this->addChild(this->m_pBoxSprite);
	return true;
}

bool GameWorld::initBackground()
{
	auto background = Background::create();
	this->addChild(background);
	return true;
}

bool GameWorld::initGameMap()
{
	std::string filename = LevelState::getInstance()->getMapName();
	m_pGameMap = GameMap::createWithFile(filename);
	m_pGameMap->setPosition(VisibleRect::center());	
	this->addChild(m_pGameMap);
	return true;
}

bool GameWorld::initListener()
{
	this->m_pEventListener = EventListenerTouchOneByOne::create();
	m_pEventListener->onTouchBegan = CC_CALLBACK_2(GameWorld::onTouchBegan, this);
	m_pEventListener->onTouchMoved = CC_CALLBACK_2(GameWorld::onTouchMoved, this);
	m_pEventListener->onTouchEnded = CC_CALLBACK_2(GameWorld::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pEventListener, this);
	return true;
}

bool GameWorld::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	Rect rect = this->m_pBoxSprite->getBoundingBox();
	rect.origin = rect.origin - Size(20, 20);
	rect.size = rect.size + Size(40, 40);
	if (rect.containsPoint(pTouch->getLocation()))
	{
		Point localPoint = pTouch->getLocation();
		Point targetPoint = this->m_pBoxSprite->getPosition();
		m_vNormalDir = (localPoint - targetPoint).getNormalized();
		int distance = (int)(localPoint - targetPoint).getLength();
		this->m_pArrowSprite->showCurrentFrame(distance);
		this->m_pArrowSprite->changeDir(m_vNormalDir);
		m_pArrowSprite->show();
		return true;
	}
	return false;
}
void GameWorld::onTouchMoved(Touch *pTouch, Event *pEvent)
{
	Point localPoint = pTouch->getLocation();
	Point targetPoint = this->m_pBoxSprite->getPosition();
	m_vNormalDir = (localPoint - targetPoint).getNormalized();
	int distance = (int)(localPoint - targetPoint).getLength();
	this->m_pArrowSprite->showCurrentFrame(distance);
	this->m_pArrowSprite->changeDir(m_vNormalDir);
}
void GameWorld::onTouchEnded(Touch *pTouch, Event *pEvent)
{
	
	m_pArrowSprite->hide();
	Point localPoint = pTouch->getLocation();
	Point targetPoint = this->m_pBoxSprite->getPosition();
	m_vNormalDir = (localPoint - targetPoint).getNormalized();
	int distance = (int)(localPoint - targetPoint).getLength();
	if (distance > 20)
	{
		bool isSubJump = this->m_pGameUI->jumpsSelfSub();
		if (isSubJump)
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/launch.mp3");
			this->m_pBoxSprite->applyForce(-m_vNormalDir * distance * 3);
		}
	}
}

void GameWorld::onEnter()
{
	Node::onEnter();

	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/gameworld.mp3", true);
}

void GameWorld::onEnterTransitionDidFinish()
{
	Node::onEnterTransitionDidFinish();

	AdManager::getInstance()->showBannerAD();

	_physicsWorld = this->getScene()->getPhysicsWorld();
	_physicsWorld->setGravity(Vect(0.0f, -300.0f));

	//_physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	/*auto node = Node::create();
	node->setPhysicsBody(PhysicsBody::createEdgeBox(VisibleRect::getVisibleRect().size));
	node->setPosition(VisibleRect::center());
	this->addChild(node);*/

	this->m_pGameUI->setDefaultValue();
}

void GameWorld::onExit()
{
    Node::onExit();
    
	AdManager::getInstance()->hideBannerAD();
    
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void GameWorld::update(float dt)
{
	if (m_pArrowSprite->isVisible())
	{
		Point newPoint = -m_vNormalDir * this->m_pBoxSprite->getContentSize().width / 2 +
			this->m_pBoxSprite->getPosition();
		this->m_pArrowSprite->setPosition(newPoint);
	}
}

void GameWorld::win()
{
	CCLOG("oh win !, enter next level");
    if(this->m_nCurrentState != kState_start) return;
    this->m_nCurrentState = kState_win;
	this->m_pGameUI->stop();
	DialogManager::getInstance()->showLvelComplete(this->m_pGameUI, [=](void* data){
		int type = *(int*)data;
		switch (type)
		{
		case 1://select game
			Utils::replaceScene(LevelSelectScene::createScene());
			break;
		case 2://restart
			this->restart();
			break;
		case 3://next level
			this->nextLevel();
			break;
		default:
			break;
		}
	});
}

void GameWorld::lose(int tag)
{
    if(this->m_nCurrentState != kState_start) return;
    m_nCurrentState = kState_lose;
	this->m_pGameUI->stop();
	DialogManager::getInstance()->showLevelCompleteLoss(&tag, [=](void* data){
		int type = *(int*)data;
		switch (type)
		{
		case 1://select Game
			Utils::replaceScene(LevelSelectScene::createScene());
			break;
		case 2://restart game
			this->restart();
			break;
		}
	});
}

void GameWorld::nextLevel()
{
	CCLOG("enter level");
	int currentSelectLevel = LevelState::getInstance()->getSelectedLevel();
	LevelState::getInstance()->setSelectedLevel(currentSelectLevel + 1);
	std::string currentMap = LevelState::getInstance()->getMapName();
	this->m_pGameMap->loadMapFile(currentMap);
	this->m_pContactLogic->loadDefaultData();
	this->m_pGameUI->setDefaultValue();
	this->loadDefaultData();
}

void GameWorld::restart()
{
	CCLOG("game over ,  restart game");
	this->m_pContactLogic->loadDefaultData();
	this->m_pGameMap->loadDefaultData();
	this->m_pGameUI->setDefaultValue();
	this->loadDefaultData();
}

void GameWorld::loadDefaultData()
{
	this->_eventDispatcher->removeEventListener(m_pEventListener);
	ValueMap valueMap = this->m_pGameMap->getHeroValueMap();
	this->m_pBoxSprite->loadDefaultData(valueMap);
	Point point = m_pBoxSprite->getPosition() +m_pGameMap->getPosition()
            - m_pGameMap->getContentSize() / 2;
	this->m_pBoxSprite->setPosition(point);
    this->m_nCurrentState = kState_start;
}

void GameWorld::startGame()
{
	AdManager::getInstance()->showBannerAD();
	this->m_pContactLogic->startGame();
	this->initListener();
    m_nCurrentState = kState_start;
}