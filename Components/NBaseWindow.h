#pragma once

#include "NDropShadowWidget.h"
#include <QPointer>
class QMainWindow;
class NWindowTitleBar;
class QMenu;
class NBaseWindow : virtual public NDropShadowWidget
{
	Q_OBJECT

public:
	NBaseWindow(QWidget *parent = NULL);
	~NBaseWindow();

	void SetWindowTitleBar(NWindowTitleBar* pWindowTitleBar);
	NWindowTitleBar* WindowTitleBar() const;
protected:

private:
	void SetupUI();
	void SetupConnect();

private:
	int m_nShadowWidth;
	QMainWindow* m_pMainWindow;
	NWindowTitleBar* m_pWindowTitleBar;
	QMenu* m_pTitleDropDownMenu;
};
