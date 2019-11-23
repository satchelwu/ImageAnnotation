#include "NBaseWindow.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QtMath>
#include "NWindowTitleBar.h"
#include <QMenu>
#include <QMenuBar>
#include <QDockWidget>
#include <QListWidget>
#include "NWindowTitleBar.h"

NBaseWindow::NBaseWindow(QWidget *parent)
	: NDropShadowWidget(parent)
{
	SetupUI();
	SetupConnect();
	/*NWindowTitleBar* titleBar = new NWindowTitleBar(this);*/
	/*qSwap(m_pWindowTitleBar, titleBar);*/
	SetWindowTitleBar(m_pWindowTitleBar);
	SetShadowWidth(2);
}

NBaseWindow::~NBaseWindow()
{
}


// 重新生成布局来解决WindowTitleBar变换的问题
void NBaseWindow::SetWindowTitleBar(NWindowTitleBar* pWindowTitleBar)
{
	m_pTitleDropDownMenu = new QMenu(this);
	QAction * action = new QAction(QStringLiteral("Settings"), this);
	m_pTitleDropDownMenu->addAction(action);

	//pWindowTitleBar = new NWindowTitleBar(this);
	pWindowTitleBar->setFixedHeight(50);
	pWindowTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	connect(pWindowTitleBar, &NWindowTitleBar::postMove, this, [=](const QPoint& vec){
		if (!(this->windowState() & Qt::WindowMaximized || (this->windowState() & Qt::WindowFullScreen)))
		{
			this->move(this->pos() + vec);
		}
	});
	connect(pWindowTitleBar, &NWindowTitleBar::postClose, this, &NBaseWindow::close);
	connect(pWindowTitleBar, &NWindowTitleBar::postMaxmize, this, [=]{

		Qt::WindowStates ws = this->windowState();
		if (ws & Qt::WindowMaximized)
		{
			this->showNormal();
		}
		else if (ws == Qt::WindowNoState)
		{
			this->showMaximized();
		}
	});
	connect(pWindowTitleBar, &NWindowTitleBar::postMinimize, this, [=]{
		if (!((this->windowState() & Qt::WindowMinimized) || this->windowState() & Qt::WindowFullScreen))
		{
			this->showMinimized();
		}
	});

	connect(pWindowTitleBar, &NWindowTitleBar::postDropDownMenuPos, this, [=](const QPoint& globalPos){
		QPoint pos = this->mapFromGlobal(globalPos);
		//pos = m_pWindowTitleBar->mapFromGlobal(globalPos);
		m_pTitleDropDownMenu->popup(globalPos);
	});

	connect(pWindowTitleBar, &NWindowTitleBar::postDoubleClicked, this, [=]{
		Qt::WindowStates ws = this->windowState();
		if (ws & Qt::WindowMaximized)
		{
			this->showNormal();
		}
		else if (ws == Qt::WindowNoState)
		{
			this->showMaximized();
		}
	});
}

NWindowTitleBar* NBaseWindow::WindowTitleBar() const
{
	return m_pWindowTitleBar;
}

void NBaseWindow::SetupUI()
{
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setVerticalSpacing(0);
	//NWindowTitleBar* titleBar = new NWindowTitleBar(this);
	m_pWindowTitleBar = new NWindowTitleBar();
	gridLayout->addWidget(m_pWindowTitleBar, 0, 0);
	m_pMainWindow = new QMainWindow;
	gridLayout->addWidget(m_pMainWindow, 1, 0);
	gridLayout->setRowStretch(1, 2);
	this->setLayout(gridLayout);
// 	QMenu* menu = new QMenu(QStringLiteral("File"), this);
// 	m_pMainWindow->menuBar()->addMenu(menu);
// 	QAction* action = new QAction(QStringLiteral("Add"), this);
// 	menu->addAction(action);
// 	QDockWidget* dockWidget = new QDockWidget(m_pMainWindow);
// 	dockWidget->setFixedSize(200, 500);
// 	m_pMainWindow->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);


// 	NWindowTitleBar* dockTitleBar = new NWindowTitleBar(dockWidget);
// 	dockWidget->setTitleBarWidget(dockTitleBar);
// 	//QGridLayout* dockGridLayout = new QGridLayout;
// 	QListWidget* listWidget = new QListWidget(dockWidget);
// 	listWidget->setFixedSize(500, 500);
// 	dockWidget->setWidget(listWidget);
// 	//dockGridLayout->addWidget(listWidget, 0, 0);
// 	//dockWidget->setLayout(dockGridLayout);
// 	QListWidgetItem* item = new QListWidgetItem(QStringLiteral("Test1"));
// 	listWidget->addItem(item);
// 	item = new QListWidgetItem(QStringLiteral("Test1"));
// 	listWidget->addItem(item);

}

void NBaseWindow::SetupConnect()
{

}
