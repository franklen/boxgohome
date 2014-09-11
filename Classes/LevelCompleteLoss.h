#ifndef __LEVELCOMPLETELOSS_H__
#define __LEVELCOMPLETELOSS_H__

#include "cocos2d.h"
#include "Dialog.h"
#include "cocos\ui\UIWidget.h"

USING_NS_CC;

using namespace cocos2d::ui;


class LevelCompleteLoss: public Node
{
public:
	LevelCompleteLoss();
	~LevelCompleteLoss();

	static LevelCompleteLoss* create(Dialog* dailog);

	bool initWithDialog(Dialog* dialog);
	bool initWithMenu();
	bool initWithTitle();

	void databind(void *data);

	void setResultCallback(DialogCallback callback);

	void btn_menuCallback(Ref*sender,TouchEventType a);
	void btn_resetCallback(Ref*sender,TouchEventType a);

private:

	Dialog*			m_pDialog;
	DialogCallback  m_fCallback;
};

#endif