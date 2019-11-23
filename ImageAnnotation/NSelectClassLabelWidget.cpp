#include "NSelectClassLabelWidget.h"
#include <QLineEdit>
#include "NWindowTitleBar.h"
#include <QGridLayout>
#include <QPushButton>
#include <QListWidget>
#include <QCompleter>
#include <QStringListModel>
#include <QEventLoop>
#include <QToolButton>
#include <QCheckBox>

NSelectClassLabelWidget::NSelectClassLabelWidget(QWidget *parent)
	: NDropShadowWidget(parent)
{
	SetupUI();
	SetupConnect();
	SetShadowWidth(3);
}

NSelectClassLabelWidget::~NSelectClassLabelWidget()
{
}

void NSelectClassLabelWidget::SetLabels(const QStringList& strLabels)
{
	if (m_modelLabels->rowCount())
	{
		m_modelLabels->removeRows(0, m_modelLabels->rowCount());
	}
	m_modelLabels->setStringList(strLabels);
	//m_pListWidgetLabels->clear();
	//m_pListWidgetLabels->addItems(strLabels);
	//m_pListWidgetLabels->setModel(m_modelLabels);
	m_pListViewLables->setModel(m_modelLabels);
}

QDialog::DialogCode NSelectClassLabelWidget::Exec()
{
	// 设置为模态;
	this->setWindowModality(Qt::ApplicationModal);
	this->show();
	// 使用事件循环QEventLoop ,不让exec()方法结束，在用户选择确定或者取消后，关闭窗口结束事件循环，并返回最后用户选择的结果;
	// 根据返回结果得到用户按下了确定还是取消，采取相应的操作。从而模拟出QDialog类的exec()方法;

	m_pEventLoop->exec();
	return m_dialogCode;
}


QString NSelectClassLabelWidget::CurrentClassLabel()
{
	return m_strClassLabel;
}

bool NSelectClassLabelWidget::CurrentIsDifficult()
{
	return m_pCheckBoxIsDifficult->isChecked();
}

bool NSelectClassLabelWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::Enter)
	{
		setCursor(Qt::ArrowCursor);
	}
	return NDropShadowWidget::eventFilter(watched, event);
}

void NSelectClassLabelWidget::closeEvent(QCloseEvent *event)
{
	if (m_pEventLoop->isRunning())
	{
		m_pEventLoop->exit();
	}
	return NDropShadowWidget::closeEvent(event);
}

void NSelectClassLabelWidget::hideEvent(QHideEvent *event)
{
	return NDropShadowWidget::hideEvent(event);
}

void NSelectClassLabelWidget::showEvent(QShowEvent *event)
{
	m_dialogCode = QDialog::Rejected;
	m_pCheckBoxIsDifficult->setChecked(false);
	return NDropShadowWidget::showEvent(event);
}

void NSelectClassLabelWidget::actionConfirm()
{
	QModelIndexList matchIndexList = m_modelLabels->match(m_modelLabels->index(0, 0), Qt::DisplayRole, m_pLineEditClassLabel->text().trimmed(), 1, Qt::MatchExactly);
	if (matchIndexList.count() > 0)
	{

		m_strClassLabel = matchIndexList[0].data().toString();
		m_dialogCode = QDialog::Accepted;
		m_pEventLoop->exit();
		this->close();

	}

}

void NSelectClassLabelWidget::actionCancel()
{
	m_strClassLabel.clear();
	m_dialogCode = QDialog::Rejected;
	m_pEventLoop->exit();
	this->close();
}

void NSelectClassLabelWidget::actionCompleterActivated(const QModelIndex& index)
{
	//m_pListWidgetLabels->setCurrentIndex(index);
	if (index.isValid())
	{
		QString str = index.data().toString();
		QModelIndexList matchIndexList = m_pListViewLables->model()->match(m_modelLabels->index(0, 0), Qt::DisplayRole, str, 1, Qt::MatchExactly);
		if (matchIndexList.count() > 0)
		{
			m_pListViewLables->setCurrentIndex(matchIndexList[0]);
		}

	}
}

void NSelectClassLabelWidget::actionLabelDoubleClicked(const QModelIndex& index)
{
	if (index.isValid())
	{
		m_strClassLabel = index.data().toString();
		m_dialogCode = QDialog::Accepted;
		m_pEventLoop->exit();
		this->close();
	}
}


void NSelectClassLabelWidget::SetupUI()
{
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setSpacing(0);
	//gridLayout->setMargin(10);
	// 	gridLayout->setVerticalSpacing(2);
	// 	gridLayout->setHorizontalSpacing(3);
	m_pWindowTitleBar = new NWindowTitleBar(this);
	m_pWindowTitleBar->setFixedHeight(40);
	m_pWindowTitleBar->setObjectName("m_pWindowTitleBar");
	m_pWindowTitleBar->SetTitle(QStringLiteral("Select Class Label"));
	m_pWindowTitleBar->SetDialogMode(true);
	gridLayout->addWidget(m_pWindowTitleBar, 0, 0);

	QHBoxLayout* hLayout = new QHBoxLayout();
	//hLayout->setMargin(5);
	hLayout->setMargin(0);
	hLayout->setSpacing(0);
	QWidget* inputWidget = new QWidget(this);
	/*inputWidget->setContentsMargins(3, 3, 3, 3);*/
	//inputWidget->setContentsMargins()
	inputWidget->setObjectName("inputWidget");
	inputWidget->setLayout(hLayout);
	inputWidget->setContentsMargins(20, 1, 20, 1);
	m_pLineEditClassLabel = new QLineEdit(this);
	//m_pLineEditClassLabel->setContentsMargins(5, 0, 0, 0);
	//m_pLineEditClassLabel->setMinimumHeight(40);
	m_pLineEditClassLabel->setObjectName("m_pLineEditClassLabel");
	hLayout->addWidget(m_pLineEditClassLabel);
	m_pToolButtonClear = new QToolButton(this);
	m_pToolButtonClear->setToolButtonStyle(Qt::ToolButtonIconOnly);
	m_pToolButtonClear->setIcon(QIcon(":/ImageAnnotation/Resources/Images/edit-clear.png"));
	m_pToolButtonClear->setObjectName("m_pToolButtonClear");
	hLayout->addWidget(m_pToolButtonClear);
	gridLayout->addWidget(inputWidget, 1, 0);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->setMargin(1);
	QWidget* buttonContainterWidget = new QWidget(this);
	buttonContainterWidget->setObjectName("buttonContainerWidget");
	buttonContainterWidget->setLayout(buttonLayout);
	// 	QSpacerItem* spaceItem = new QSpacerItem(20, 20, QSizePolicy::Expanding);
	// 	buttonLayout->addSpacerItem(spaceItem);
	buttonLayout->addStretch(0);
	m_pCheckBoxIsDifficult = new QCheckBox(this);
	m_pCheckBoxIsDifficult->setText(QStringLiteral("Difficult"));
	buttonLayout->addWidget(m_pCheckBoxIsDifficult);
	m_pToolButtonConfirm = new QToolButton(this);
	m_pToolButtonConfirm->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_pToolButtonConfirm->setIcon(QIcon(":/labelImg/Resources/icons/done.png"));
	m_pToolButtonConfirm->setText(QStringLiteral("Confirm"));
	m_pToolButtonConfirm->setObjectName("m_pToolButtonConfirm");
	buttonLayout->addWidget(m_pToolButtonConfirm);
	m_pToolButtonCancel = new QToolButton(this);
	m_pToolButtonCancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_pToolButtonCancel->setIcon(QIcon(":/labelImg/Resources/icons/cancel.png"));
	m_pToolButtonCancel->setText(QStringLiteral("Cancel"));
	m_pToolButtonCancel->setObjectName("m_pToolButtonCancel");
	buttonLayout->addWidget(m_pToolButtonCancel);

	gridLayout->addWidget(buttonContainterWidget, 2, 0);

	//m_pListWidgetLabels = new QListWidget(this);
	//m_pListWidgetLabels->setObjectName("m_pListWidgetLabels");
	m_pListViewLables = new QListView(this);
	m_pListViewLables->setEditTriggers(QAbstractItemView::NoEditTriggers);

	//gridLayout->addWidget(m_pListWidgetLabels, 3, 0);
	gridLayout->addWidget(m_pListViewLables, 3, 0);
	gridLayout->setRowStretch(3, 10);
	this->setLayout(gridLayout);

	m_pCompleterLabels = new QCompleter(this);
	m_modelLabels = new QStringListModel(this);
	//m_modelLabels->setStringList(QStringList("dog"));
	m_pCompleterLabels->setModel(m_modelLabels);
	m_pCompleterLabels->setCompletionMode(QCompleter::PopupCompletion);

	m_pLineEditClassLabel->setCompleter(m_pCompleterLabels);

	m_pEventLoop = new QEventLoop(this);
	//this->setFocusPolicy(Qt::FocusPolicy(Qt::StrongFocus | Qt::WheelFocus));
	setMouseTracking(true);
	//m_pListWidgetLabels->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_pListViewLables->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void NSelectClassLabelWidget::SetupConnect()
{
	QObjectList objList = this->children();
	for (int nIndex = 0; nIndex < objList.size(); ++nIndex)
	{
		objList[nIndex]->installEventFilter(this);
	}

	connect(m_pListViewLables, &QListView::doubleClicked, this, &NSelectClassLabelWidget::actionLabelDoubleClicked);

	connect(m_pCompleterLabels, static_cast<void(QCompleter::*)(const QModelIndex &)>(&QCompleter::activated), this, &NSelectClassLabelWidget::actionCompleterActivated);

// 	connect(m_pCompleterLabels, static_cast<void(QCompleter::*)(const QModelIndex &)>(&QCompleter::highlighted),
// 		[=](const QModelIndex &index){
// 		//m_pListWidgetLabels->setCurrentIndex(index);
// 		int nCount = m_pCompleterLabels->completionCount();
// 		if (index.isValid())
// 		{
// 			//m_pListViewLables->setCurrentIndex(index);
// 			m_pListViewLables->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
// 		}
// 
// 		//m_pListViewLables->setSelectionModel(m_pCompleterLabels->completionModel())
// 	});

	connect(m_pToolButtonClear, &QPushButton::clicked, m_pLineEditClassLabel, &QLineEdit::clear);
	//connect(m_pListWidgetLabels, &QListWidget::itemDoubleClicked, this, &NSelectClassLabelWidget::actionConfirm);

	connect(m_pToolButtonConfirm, &QPushButton::clicked, this, &NSelectClassLabelWidget::actionConfirm);
	connect(m_pToolButtonCancel, &QPushButton::clicked, this, &NSelectClassLabelWidget::actionCancel);

	connect(m_pWindowTitleBar, &NWindowTitleBar::postMove, this, [=](const QPoint& vec){
		if (!(this->windowState() & Qt::WindowMaximized || (this->windowState() & Qt::WindowFullScreen)))
		{
			this->move(this->pos() + vec);
		}
	});
	connect(m_pWindowTitleBar, &NWindowTitleBar::postClose, this, [=]{
		actionCancel();
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
	connect(m_pWindowTitleBar, &NWindowTitleBar::postMinimize, this, [=]{
		if (!((this->windowState() & Qt::WindowMinimized) || this->windowState() & Qt::WindowFullScreen))
		{
			this->showMinimized();
		}
	});

	// 	connect(m_pWindowTitleBar, &NWindowTitleBar::postDropDownMenuPos, this, [=](const QPoint& globalPos){
	// 		QPoint pos = this->mapFromGlobal(globalPos);
	// 		//pos = m_pWindowTitleBar->mapFromGlobal(globalPos);
	// 		m_pTitleDropDownMenu->popup(globalPos);
	// 	});

	connect(m_pWindowTitleBar, &NWindowTitleBar::postDoubleClicked, this, [=]{
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
