#pragma once

#include "NDropShadowWidget.h"

class NWindowTitleBar;
class QEventLoop;
class QTextEdit;

class NUsageWidget : public NDropShadowWidget
{
	Q_OBJECT

public:
	NUsageWidget(QWidget *parent = NULL);
	~NUsageWidget();
	QDialog::DialogCode Exec();
protected:
	bool eventFilter(QObject *watched, QEvent *event);
	private Q_SLOTS:
	void actionClose();
private:
	void SetupUI();
	void SetupConnect();
	QString MakeUsage();
private:
	NWindowTitleBar* m_pWindowTitleBar;
	QEventLoop* m_pEventLoop;
	QDialog::DialogCode m_dialogCode;
	QTextEdit* m_pTextEditUsage;
};
