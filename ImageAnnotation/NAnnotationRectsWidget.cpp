#include "NAnnotationRectsWidget.h"
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QCheckBox>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QLineEdit>
#include <QCheckBox>


/*Q_DECLARE_METATYPE(SAnnotationRect*);*/
NAnnotationRectsWidget::NAnnotationRectsWidget(QWidget *parent)
	: QWidget(parent)
{
	SetupUI();
	SetupConnect();
}

NAnnotationRectsWidget::~NAnnotationRectsWidget()
{

}

void NAnnotationRectsWidget::AddAnnotationRect(const SAnnotationRect& annotationRect)
{
	m_pCheckboxRectListWidget->clearSelection();
	QListWidgetItem* item;
	QString strShowName = annotationRect.m_bDifficult ? QStringLiteral("%1(Difficult)").arg(annotationRect.m_strObjectClassName) : annotationRect.m_strObjectClassName;
	if (annotationRect.m_bHidden == false)
	{
		item = new QListWidgetItem(m_iconChecked, strShowName);
	}
	else
	{
		item = new QListWidgetItem(m_iconUnchecked, strShowName);
	}
	//item->setData(Qt::UserRole, reinterpret_cast<long long>(&m_pListAnnotationRect[m_pListAnnotationRect->size() - 1]));
	m_pCheckboxRectListWidget->addItem(item);
	m_pListAnnotationRect->push_back(annotationRect);
	m_pCheckboxRectListWidget->setCurrentRow(m_pCheckboxRectListWidget->count() - 1);

	// 	m_pCheckboxRectListWidget->clearSelection();
	// 	QListWidgetItem * item;
	// 	if (annotationRect.m_bHidden == false)
	// 	{
	// 		item = new QListWidgetItem(m_iconChecked, annotationRect.m_strObjectClassName);
	// 	}
	// 	else
	// 	{
	// 		item = new QListWidgetItem(m_iconUnchecked, annotationRect.m_strObjectClassName);
	// 	}
	// 	m_pCheckboxRectListWidget->addItem(item);
	// 	item->setData(Qt::UserRole, QVariant::fromValue(annotationRect));
	// 	m_pCheckboxRectListWidget->setCurrentRow(m_pCheckboxRectListWidget->count() - 1);
	// 	QHBoxLayout* layout = new QHBoxLayout();
	// 	QWidget* widget = new QWidget;
	// 	QCheckBox* checkBox = new QCheckBox(widget);
	// 	checkBox->setChecked(true);
	// 	QLabel* label = new QLabel(annotationRect.m_strObjectClassName, widget);
	// 	layout->addWidget(checkBox);
	// 	layout->addWidget(label);
	// 	widget->setLayout(layout);
	// 	m_pCheckboxRectListWidget->addItem(item);
	// 	m_pCheckboxRectListWidget->setItemWidget(item, widget);

}

void NAnnotationRectsWidget::SetAnnotationRects(QList<SAnnotationRect>* pListAnnotationRect)
{
	m_pCheckboxRectListWidget->clear();
	m_pListAnnotationRect = pListAnnotationRect;
	for (int nIndex = 0; nIndex < pListAnnotationRect->size(); ++nIndex)
	{
		QListWidgetItem* item;
		SAnnotationRect& annotationRect = (*pListAnnotationRect)[nIndex];
		QString strShowName = annotationRect.m_bDifficult ? QStringLiteral("%1(Difficult)").arg(annotationRect.m_strObjectClassName) : annotationRect.m_strObjectClassName;
		if (annotationRect.m_bHidden == false)
		{
			item = new QListWidgetItem(m_iconChecked, strShowName);
		}
		else
		{
			item = new QListWidgetItem(m_iconUnchecked, strShowName);
		}
		m_pCheckboxRectListWidget->addItem(item);
		//item->setData(Qt::UserRole,reinterpret_cast<long long>(&m_pListAnnotationRect[m_pListAnnotationRect->size() - 1]));
	}
	m_pCheckboxRectListWidget->setCurrentRow(m_pCheckboxRectListWidget->count() - 1);
}

void NAnnotationRectsWidget::SetTitle(const QString& strTitle)
{
	m_pLabelTitle->setText(strTitle);
}

void NAnnotationRectsWidget::SetCurrentRectIndex(int nRowInex, const QRect& rect)
{
	if (nRowInex >= 0 && nRowInex < m_pCheckboxRectListWidget->count())
	{
		m_pCheckboxRectListWidget->setCurrentRow(nRowInex);
		UpdateRow(nRowInex, rect);
	}
	else
	{
		m_pCheckboxRectListWidget->clearSelection();
		m_pCheckboxRectListWidget->setCurrentRow(-1);
	}

}

bool NAnnotationRectsWidget::UseDefaultClassLabel()
{
	if (m_pCheckBoxUseDefaultClassLabel->isChecked() && !m_pLineEditClassLabel->text().trimmed().isEmpty())
	{
		return true;
	}
	else
	{
		return false;
	}
}

QString NAnnotationRectsWidget::DefaultClassLabel()
{
	return m_pLineEditClassLabel->text().trimmed();
}

void NAnnotationRectsWidget::actionAnnotationRectItemClicked(QListWidgetItem* item)
{
	if (item)
	{
		QPoint curMousePos = cursor().pos();
		QPoint mousePos = m_pCheckboxRectListWidget->mapFromGlobal(cursor().pos());
		int nXDelta = mousePos.x() - m_pCheckboxRectListWidget->pos().x();

		SAnnotationRect& annotationRect = (*m_pListAnnotationRect)[m_pCheckboxRectListWidget->row(item)];
		if (nXDelta > 0 && nXDelta < 20)
		{
			annotationRect.m_bHidden = !annotationRect.m_bHidden;
			if (annotationRect.m_bHidden == false)
			{
				item->setIcon(m_iconChecked);
			}
			else
			{
				item->setIcon(m_iconUnchecked);
			}
			//item->setData(Qt::UserRole, QVariant::fromValue(annotationRect));
		}
		//SAnnotationRect* annotationRect = reinterpret_cast<SAnnotationRect*>(item->data(Qt::UserRole).toLongLong());
		emit postCurrentIndex(m_pCheckboxRectListWidget->row(item), annotationRect.m_bHidden);
	}
}

void NAnnotationRectsWidget::actionContextMenuRequesed(const QPoint& pos)
{
	QListWidgetItem* item = m_pCheckboxRectListWidget->itemAt(pos);
	if (item)
	{
		m_pContextMenu->popup(m_pCheckboxRectListWidget->mapToGlobal(pos));
	}
}

void NAnnotationRectsWidget::actionDeleteRect()
{
	int nRowIndex = m_pCheckboxRectListWidget->currentRow();
	if (nRowIndex != -1)
	{
		delete m_pCheckboxRectListWidget->takeItem(nRowIndex);
		m_pListAnnotationRect->removeAt(nRowIndex);
		emit postDeleteRectIndex(nRowIndex);
	}
}

//  bool NAnnotationRectsWidget::eventFilter(QObject *watched, QEvent *event)
//  {
//  	if (watched == m_pCheckboxRectListWidget)
//  	{
//  		if (event->type() == QEvent::MouseButtonPress)
//  		{
//  			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
//  			if (mouseEvent->button() == Qt::LeftButton)
//  			{
//  				QListWidgetItem* item = m_pCheckboxRectListWidget->itemAt(mouseEvent->pos());
//  				SAnnotationRect& anotationRect = qvariant_cast<SAnnotationRect>(item->data(Qt::UserRole));
//  				int nXdelta = mouseEvent->pos().x() - m_pCheckboxRectListWidget->pos().x();
//  			}
//  		}
//  	}
//  	return QWidget::eventFilter(watched, event);
//  }

void NAnnotationRectsWidget::SetupUI()
{
	m_pContextMenu = new QMenu(this);
	m_pActionDelete = new QAction(this);
	m_pActionDelete->setText(QStringLiteral("Delete"));
	m_pActionDelete->setIcon(QIcon(":/ImageAnnotation/Resources/Images/delete.png"));
	m_pContextMenu->addAction(m_pActionDelete);


	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	m_pLabelTitle = new QLabel(this);
	layout->addWidget(m_pLabelTitle);

	QWidget * pWidgetDefaultPannel = new QWidget(this);
	pWidgetDefaultPannel->setObjectName("pWidgetDefaultPannel");
	QHBoxLayout* pPannelLayout = new QHBoxLayout();
	m_pCheckBoxUseDefaultClassLabel = new QCheckBox(QStringLiteral("Use Default Label"), this);
	pPannelLayout->addWidget(m_pCheckBoxUseDefaultClassLabel);
	m_pLineEditClassLabel = new QLineEdit(this);
	//m_pLineEditClassLabel->setFocusPolicy(Qt::NoFocus);
	pPannelLayout->addWidget(m_pLineEditClassLabel);
	pWidgetDefaultPannel->setLayout(pPannelLayout);

	layout->addWidget(pWidgetDefaultPannel);

	m_pCheckboxRectListWidget = new QListWidget(this);
	m_pCheckboxRectListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	layout->addWidget(m_pCheckboxRectListWidget);
	this->setLayout(layout);
	m_pCheckboxRectListWidget->installEventFilter(this);
	m_iconChecked = QIcon(":/ImageAnnotation/Resources/QtImages/checkbox_checked.png");
	m_iconUnchecked = QIcon(":/ImageAnnotation/Resources/QtImages/checkbox_unchecked.png");
	m_pCheckboxRectListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void NAnnotationRectsWidget::SetupConnect()
{
	connect(m_pCheckboxRectListWidget, &QListWidget::itemClicked, this, &NAnnotationRectsWidget::actionAnnotationRectItemClicked);
	//connect(m_pCheckboxRectListWidget, &QListWidget::currentItemChanged, this, &NAnnotationRectsWidget::actionAnnotationRectItemClicked);
	connect(m_pCheckboxRectListWidget, &QListWidget::customContextMenuRequested, this, &NAnnotationRectsWidget::actionContextMenuRequesed);
	connect(m_pActionDelete, &QAction::triggered, this, &NAnnotationRectsWidget::actionDeleteRect);
// 	connect(m_pLineEditClassLabel, &QLineEdit::editingFinished, this, [=]{
// 		m_pLineEditClassLabel->
// 	});
	connect(m_pLineEditClassLabel, &QLineEdit::editingFinished, this, [=]{
		m_pLineEditClassLabel->clearFocus();
	});
}

void NAnnotationRectsWidget::UpdateRow(int nRowIndex, const QRect& rect)
{
	QListWidgetItem* item = m_pCheckboxRectListWidget->item(nRowIndex);
	if (item != NULL)
	{
		//SAnnotationRect* ar = reinterpret_cast<SAnnotationRect*>(item->data(Qt::UserRole).toLongLong());
		//ar->m_rectOfObject = rect;
		//item->setData(Qt::UserRole, static_cast<long long>(ar));
		(*m_pListAnnotationRect)[nRowIndex].m_rectOfObject = rect;
	}
}
