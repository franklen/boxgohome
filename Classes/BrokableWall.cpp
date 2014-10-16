#include "BrokableWall.h"
#include "Utils.h"

BrokableWall::BrokableWall()
:m_bIsCanContact(true)
{

}

BrokableWall::~BrokableWall()
{

}

BrokableWall* BrokableWall::create( const ValueMap &valueMap , const ValueMap &gidProperties )
{
	auto blueWall = new BrokableWall();
	if (blueWall && blueWall->init(valueMap , gidProperties))
	{
		blueWall->autorelease();
		return blueWall;
	}
	return nullptr;
}

bool BrokableWall::init( const ValueMap &valueMap , const ValueMap &gidProperties )
{
	if (!BaseEntity::initWithMap(valueMap )) return false;
	//std::string img = Utils::getWallByType(this->m_nType);
	std::string img = gidProperties.find("source")->second.asString();
	m_durability = gidProperties.find("durability")->second.asInt();
	if (!BaseEntity::initWithFile(img)) return false;
	setPositionAndRotation();
	this->setPhysicsBody(PhysicsBody::createEdgeBox(this->getContentSize(), PhysicsMaterial(1.0f, 0.4f, 1.0f), 0));//�ܶ� ���� Ħ����

	setContactTestBitmask(0x0002);
	setEntityType(Type_Brokable);
	return true;
}

bool BrokableWall::contactLogicBegin( PhysicsContact &contact, ContactLogic *logic )
{
	// get BoxSprite Body
	PhysicsBody * body = Utils::getBody(contact , Type_BoxSprite );
	float moment = body->getVelocity().getLength();
	if (m_bIsCanContact && moment >  80)
	{
		m_durability--;
		if (m_durability <= 0)
		{
			this->removeFromParent();
		}
		else
		{
			m_bIsCanContact = false;
			this->setSpriteFrame(Sprite::create("maps/Stone elements/elementGlass049.png")->displayFrame());
			this->schedule(schedule_selector(BrokableWall::updateContactState), 0.1f);
		}
	}
	return true;
}

void BrokableWall::contactLogicSeperate( PhysicsContact &contact, ContactLogic *logic )
{
	
}

void BrokableWall::updateContactState(float dt)
{
	this->unschedule(schedule_selector(BrokableWall::updateContactState));
	this->m_bIsCanContact = true;
}

