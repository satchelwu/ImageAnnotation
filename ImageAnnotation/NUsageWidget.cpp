#include "NUsageWidget.h"
#include <QGridLayout>
#include "NWindowTitleBar.h"
#include <QTextEdit>

NUsageWidget::NUsageWidget(QWidget *parent)
	: NDropShadowWidget(parent)
{
	SetupUI();
	SetupConnect();
	SetShadowWidth(3);
}

NUsageWidget::~NUsageWidget()
{

}

QDialog::DialogCode NUsageWidget::Exec()
{
	// 设置为模态;
	//this->setWindowModality(Qt::ApplicationModal);
	this->showNormal();
	// 使用事件循环QEventLoop ,不让exec()方法结束，在用户选择确定或者取消后，关闭窗口结束事件循环，并返回最后用户选择的结果;
	// 根据返回结果得到用户按下了确定还是取消，采取相应的操作。从而模拟出QDialog类的exec()方法;

	//m_pEventLoop->exec();
	return m_dialogCode;
}

bool NUsageWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::Enter)
	{
		setCursor(Qt::ArrowCursor);
	}
	return NDropShadowWidget::eventFilter(watched, event);
}

void NUsageWidget::actionClose()
{
	if (m_pEventLoop->isRunning())
	{
		m_pEventLoop->exit();
	}
	this->close();
}
void NUsageWidget::SetupUI()
{
	m_pEventLoop = new QEventLoop(this);
	QGridLayout* gridLayout = new QGridLayout();
	m_pWindowTitleBar = new NWindowTitleBar(this);
	m_pWindowTitleBar->SetTitle(QStringLiteral("Usage"));
	m_pWindowTitleBar->setFixedHeight(40);
	m_pWindowTitleBar->SetDialogMode(true);
	gridLayout->addWidget(m_pWindowTitleBar, 0, 0);
	m_pTextEditUsage = new QTextEdit(this);
	m_pTextEditUsage->setFixedSize(QSize(800, 600));
	m_pTextEditUsage->setAcceptRichText(true);
	//m_pTextEditUsage->setReadOnly(true);
	//m_pTextEditUsage->setHtml(MakeUsage());
	MakeUsage();
	gridLayout->addWidget(m_pTextEditUsage, 1, 0);

	this->setLayout(gridLayout);
	this->SetFixedSize(true);
}

void NUsageWidget::SetupConnect()
{
 	connect(m_pTextEditUsage, &QTextEdit::textChanged, this, [=]{
 		QString str = m_pTextEditUsage->toHtml();
		QString strPlain = m_pTextEditUsage->toPlainText();
 	});

	connect(m_pWindowTitleBar, &NWindowTitleBar::postMove, this, [=](const QPoint& vec){
		if (!(this->windowState() & Qt::WindowMaximized || (this->windowState() & Qt::WindowFullScreen)))
		{
			this->move(this->pos() + vec);
		}
	});
	connect(m_pWindowTitleBar, &NWindowTitleBar::postClose, this, [=]{
		actionClose();
	});
	connect(m_pWindowTitleBar, &NWindowTitleBar::postMaxmize, this, [=]{

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
// 	connect(m_pWindowTitleBar, &NWindowTitleBar::postMinimize, this, [=]{
// 		if (!((this->windowState() & Qt::WindowMinimized) || this->windowState() & Qt::WindowFullScreen))
// 		{
// 			this->showMinimized();
// 		}
// 	});

	// 	connect(m_pWindowTitleBar, &NWindowTitleBar::postDropDownMenuPos, this, [=](const QPoint& globalPos){
	// 		QPoint pos = this->mapFromGlobal(globalPos);
	// 		//pos = m_pWindowTitleBar->mapFromGlobal(globalPos);
	// 		m_pTitleDropDownMenu->popup(globalPos);
	// 	});

// 	connect(m_pWindowTitleBar, &NWindowTitleBar::postDoubleClicked, this, [=]{
// 		Qt::WindowStates ws = this->windowState();
// 		if (ws & Qt::WindowMaximized)
// 		{
// 			this->showNormal();
// 		}
// 		else if (ws == Qt::WindowNoState)
// 		{
// 			this->showMaximized();
// 		}
// 	});
}

QString NUsageWidget::MakeUsage()
{
	QFile usageFile(":/ImageAnnotation/Resources/Usage.xml");
	if(usageFile.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		m_pTextEditUsage->setHtml(usageFile.readAll());
	}
	return "";
}																																																																																																																																																																																		 
