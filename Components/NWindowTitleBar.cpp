#include "NWindowTitleBar.h"
#include <QKeyEvent>
#include <QFile>
#include <QPainter>
#include <QStyleOption>

NWindowTitleBar::NWindowTitleBar(QWidget *parent)
	: QWidget(parent), m_bCanMove(true), m_bDropDownMenuEnabled(true), m_bMaxmizeStateEnabled(true)
{
	ui.setupUi(this);
	SetupUI();
	SetupConnect();
}

NWindowTitleBar::~NWindowTitleBar()
{

}

void NWindowTitleBar::SetDialogMode(bool bDialogMode)
{
	if (bDialogMode)
	{
		ui.pushButtonDropDownMenu->setHidden(true);
		ui.pushButtonMinimize->setHidden(true);
		ui.pushButtonMaxmize->setHidden(true);
	}
	else
	{
		ui.pushButtonDropDownMenu->setHidden(false);
		ui.pushButtonMinimize->setHidden(false);
		ui.pushButtonMaxmize->setHidden(false);
	}
}

void NWindowTitleBar::SetTitle(const QString& strTitle, int nFontSize /*= 21*/, Qt::Alignment alighment /*= Qt::AlignHCenter | Qt::AlignVCenter*/)
{
	QFont font = ui.labelTitle->font();
	font.setPointSize(nFontSize);
	ui.labelTitle->setFont(font);
	ui.labelTitle->setAlignment(alighment);
	ui.labelTitle->setText(strTitle);
}

void NWindowTitleBar::SetIcon(const QIcon& icon)
{
	ui.labelIcon->setPixmap(icon.pixmap(200, 200));
}

void NWindowTitleBar::SetWindowMaxState(bool bMaxState /*= true*/)
{
	if (bMaxState)
	{
		ui.pushButtonMaxmize->setIcon(m_iconMaximizeState);
	}
	else
	{
		ui.pushButtonMaxmize->setIcon(m_iconNormalState);
	}

}

void NWindowTitleBar::SetMaximizeStateEnabled(bool bEnabled /*= true*/)
{

	m_bMaxmizeStateEnabled = bEnabled;
	if (m_bMaxmizeStateEnabled == true)
	{
		ui.pushButtonMaxmize->setHidden(false);
	}
	else
	{
		ui.pushButtonMaxmize->setHidden(true);
	}

}

void NWindowTitleBar::SetDropDownMenuEnabled(bool bEnabled /*= true*/)
{
	if (bEnabled != m_bDropDownMenuEnabled)
	{
		m_bDropDownMenuEnabled = bEnabled;
	}
	if (m_bDropDownMenuEnabled == true)
	{
		ui.pushButtonDropDownMenu->setHidden(false);
	}
	else
	{
		ui.pushButtonDropDownMenu->setHidden(true);
	}
}

QPushButton* NWindowTitleBar::MinimizeButton()
{
	return ui.pushButtonMinimize;
}

QPushButton* NWindowTitleBar::MaximizeButton()
{
	return ui.pushButtonMaxmize;
}

QPushButton* NWindowTitleBar::CloseButton()
{
	return ui.pushButtonClose;
}

QPushButton* NWindowTitleBar::DropdownButton()
{
	return ui.pushButtonDropDownMenu;
}

QWidget* NWindowTitleBar::TitleIconWidget()
{
	return ui.labelIcon;
}

QWidget* NWindowTitleBar::TitleLabelWidget()
{
	return ui.labelTitle;
}

bool NWindowTitleBar::event(QEvent* event)
{
	return QWidget::event(event);

}

void NWindowTitleBar::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool NWindowTitleBar::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonRelease)
	{
		if (watched == ui.pushButtonDropDownMenu)
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			QPoint globalPos = ui.pushButtonDropDownMenu->mapToGlobal(QPoint(0, 0) + QPoint(0, ui.pushButtonDropDownMenu->height()));
			if (ui.pushButtonDropDownMenu->rect().contains(mouseEvent->pos()))
			{
				emit postDropDownMenuPos(globalPos);
			}
		}
		else if (watched == ui.pushButtonMinimize)
		{
			if (event->type() == QEvent::MouseButtonRelease)
			{
				QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
				if (ui.pushButtonMinimize->rect().contains(mouseEvent->pos()))
				{
					emit postMinimize();
				}
			}
		}
		else if (watched == ui.pushButtonMaxmize)
		{
			if (event->type() == QEvent::MouseButtonRelease)
			{
				QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
				if (ui.pushButtonMaxmize->rect().contains(mouseEvent->pos()))
				{
					emit postMaxmize();
				}
			}
		}
		else if (watched == ui.pushButtonClose)
		{
			m_bCanMove = false;
			if (event->type() == QEvent::MouseButtonRelease)
			{
				QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
				if (ui.pushButtonClose->rect().contains(mouseEvent->pos()))
				{
					emit postClose();
				}
			}
		}
		m_bCanMove = true;
	}
	else if (event->type() == QMouseEvent::MouseButtonPress)
	{
		if (watched == ui.pushButtonClose || watched == ui.pushButtonDropDownMenu || watched == ui.pushButtonMaxmize || watched == ui.pushButtonMinimize)
		{
			m_bCanMove = false;
		}
		else
		{
			m_bCanMove = true;
		}
	}
	return QWidget::eventFilter(watched, event);
}

void NWindowTitleBar::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_pointPressed = event->pos();
		m_bCanMove = true;
	}
	return QWidget::mousePressEvent(event);

}

void NWindowTitleBar::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		if (m_bCanMove)
		{
			QPoint vec = event->pos() - m_pointPressed;
			emit postMove(vec);
		}
	}
	return QWidget::mouseMoveEvent(event);

}

void NWindowTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_bCanMove = true;
	}
	return QWidget::mouseReleaseEvent(event);
}

void NWindowTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (m_bMaxmizeStateEnabled == true)
			emit postDoubleClicked();
	}
	return QWidget::mouseDoubleClickEvent(event);
}

void NWindowTitleBar::SetupUI()
{
	m_iconNormalState = QIcon(":/icons/Resources/icons/LightStyleIcons/appbar.app.png");
	m_iconMaximizeState = QIcon(":/icons/Resources/icons/LightStyleIcons/appbar.fullscreen.box.png");
	this->setAttribute(Qt::WA_StyledBackground);
	//this->setAttribute(Qt::WA_NoSystemBackground);
	//this->setAttribute(Qt::WA_NoSystemBackground, true);
	//this->setMouseTracking(true);
	//this->setAutoFillBackground(true);
	//this->setStyleSheet("QWidget#NWindowTitleBar {background-color:lightblue;}");

	QFile file(":/MainWindow/Resources/WindowTitleBar.css");
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		QString strQss = file.readAll();
		this->setStyleSheet(strQss);
	}
	ui.pushButtonMinimize->setIcon(QIcon(":/icons/Resources/icons/LightStyleIcons/appbar.minus.png"));
	ui.pushButtonMaxmize->setIcon(QIcon(":/icons/Resources/icons/LightStyleIcons/appbar.app.png"));
	ui.pushButtonClose->setIcon(QIcon(":/icons/Resources/icons/LightStyleIcons/appbar.close.png"));
	ui.pushButtonDropDownMenu->setIcon(QIcon(":/icons/Resources/icons/LightStyleIcons/appbar.cog.png"));
	SetDropDownMenuEnabled(m_bDropDownMenuEnabled);
	SetMaximizeStateEnabled(m_bMaxmizeStateEnabled);
}

void NWindowTitleBar::SetupConnect()
{
	ui.pushButtonClose->installEventFilter(this);
	ui.pushButtonDropDownMenu->installEventFilter(this);
	ui.pushButtonMaxmize->installEventFilter(this);
	ui.pushButtonMinimize->installEventFilter(this);
}
