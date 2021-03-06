#include "DialogManager.h"
#include "LevelComplete.h"
#include "LevelCompleteLoss.h"
#include "SettingsDiaog.h"

static DialogManager* dialogManager = nullptr;

DialogManager* DialogManager::getInstance()
{
	if (dialogManager == nullptr)
	{
		dialogManager = new (std::nothrow) DialogManager;
	}
	return dialogManager;
}

Dialog* DialogManager::showLvelComplete(void *data, DialogCallback callback)
{
	auto dialog = Dialog::create();
	auto layer = LevelComplete::create(dialog);
	layer->databind(data);
	layer->setResultCallback(callback);
	dialog->setContentPanel(layer);
	dialog->showDialog();
	return dialog;
}

Dialog* DialogManager::showLevelCompleteLoss(void *data, DialogCallback callback)
{
	auto dialog = Dialog::create();
	auto layer = LevelCompleteLoss::create(dialog);
	layer->databind(data);
	layer->setResultCallback(callback);
	dialog->setContentPanel(layer);
	dialog->showDialog();
	return dialog;
}


Dialog* DialogManager::showSettingDialog()
{
	auto dialog = Dialog::create();
	auto layer = SettingsDiaog::create(dialog);
	dialog->setContentPanel(layer);
	dialog->setEnableClickClose(true);
	dialog->showDialog();
	return dialog;
}
