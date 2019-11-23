#include "NImagePathListWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>

NImagePathListWidget::NImagePathListWidget(QWidget *parent)
	: QWidget(parent)
{
	SetupUI();
	SetupConnect();
}

NImagePathListWidget::~NImagePathListWidget()
{

}

void NImagePathListWidget::AddImagePath(const QString& strImagePath)
{
	m_pImagePathListWidget->addItem(new QListWidgetItem(strImagePath));
}

void NImagePathListWidget::AddImagPathList(const QStringList& strImagePathList)
{
	for (int nIndex = 0; nIndex < strImagePathList.size(); ++nIndex)
	{
		m_pImagePathListWidget->addItem(new QListWidgetItem(strImagePathList[nIndex]));
	}
	m_pImagePathListWidget->setCurrentRow(0);
}

void NImagePathListWidget::ClearImagePathList()
{
	m_pImagePathListWidget->clear();
}

void NImagePathListWidget::SetTitle(const QString& strTitle)
{
	m_pLabelTitle->setText(strTitle);
}

QListWidgetItem* NImagePathListWidget::CurrentItem()
{
	return m_pImagePathListWidget->currentItem();
}

QString NImagePathListWidget::CurrentImagePath()
{
	QListWidgetItem* item = m_pImagePathListWidget->currentItem();
	if (item)
	{
		return item->text();
	}
	return QString();
}

const QString NImagePathListWidget::GetImagePath(int nRowIndex)
{
	QListWidgetItem* item = m_pImagePathListWidget->item(nRowIndex);
	if (item)
	{
		return item->text();
	}
	return QString();
}

void NImagePathListWidget::NextImagePath()
{
	if (m_pImagePathListWidget->count() > 0)
	{
		int nRow = m_pImagePathListWidget->currentRow();
		if (++nRow >= m_pImagePathListWidget->count())
		{
			nRow = 0;
		}
		m_pImagePathListWidget->setCurrentRow(nRow);

	}
}

void NImagePathListWidget::PrevImagePath()
{
	if (m_pImagePathListWidget->count() > 0)
	{
		int nRow = m_pImagePathListWidget->currentRow();
		if (--nRow < 0)
		{
			nRow = m_pImagePathListWidget->count() - 1;
		}
		m_pImagePathListWidget->setCurrentRow(nRow);
	}
}

void NImagePathListWidget::SetupUI()
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	m_pLabelTitle = new QLabel(this);
	layout->addWidget(m_pLabelTitle);
	m_pImagePathListWidget = new QListWidget(this);
	layout->addWidget(m_pImagePathListWidget);
	this->setLayout(layout);
}

void NImagePathListWidget::SetupConnect()
{
	connect(m_pImagePathListWidget, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem* pItem){
		if (pItem)
		{
			QString strImagePath = pItem->text();
			emit postDBClickedImagePath(strImagePath);
		}
	});

	connect(m_pImagePathListWidget, &QListWidget::currentRowChanged, this, [=](int nCurrentRow){
		if (nCurrentRow != -1)
		{
			emit postCurrentImagePathChanged(m_pImagePathListWidget->item(nCurrentRow)->text());
		}
	});
	connect(m_pImagePathListWidget, &QListWidget::currentItemChanged, this, [=](QListWidgetItem* current, QListWidgetItem* previous){

		QString strCurrentPath, strPreviousPath;
		if (current)
		{
			strCurrentPath = current->text();
		}
		if (previous)
		{
			strPreviousPath = previous->text();
		}
		emit postImagePathChanged(strCurrentPath, strPreviousPath);

	});
	connect(m_pImagePathListWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem* item){
		if (item)
		{
			emit postClickedImagePath(item->text());
		}
	});
	//connect(m_pImagePathListWidget->&QListWidget::)

}
