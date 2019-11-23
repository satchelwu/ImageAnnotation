#include "NExportLabeledObjectImagesWidget.h"
#include <QGridLayout>
#include <QProgressBar>
#include "NWindowTitleBar.h"
#include <QLineEdit>
#include <QToolButton>
#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QDirIterator>
#include <QFileInfo>
#include <NVocAnnationSerializer.h>
#include <QStringBuilder>
#include <opencv2/opencv.hpp>

NExportLabeledObjectImagesWidget::NExportLabeledObjectImagesWidget(QWidget *parent)
	: NDropShadowWidget(parent)
{
	this->setWindowFlags(this->windowFlags() | Qt::Dialog);
	SetupUI();
	SetupConnect();
	this->SetShadowWidth(3);
}

NExportLabeledObjectImagesWidget::~NExportLabeledObjectImagesWidget()
{
	if (m_pFutureWatcherExport->isRunning())
	{
		m_pFutureWatcherExport->thread()->exit();
	}
}

void NExportLabeledObjectImagesWidget::SetImageDirAndAnnotationDir(const QString& strImageDirPath, QString& strAnnotationDirPath)
{
	m_strCurrentImageDirPath = strImageDirPath;
	m_strCurrentAnnotationDirPath = strAnnotationDirPath;
}

QDialog::DialogCode NExportLabeledObjectImagesWidget::Exec()
{
	// 设置为模态;
	this->setWindowModality(Qt::ApplicationModal);
	this->show();
	// 使用事件循环QEventLoop ,不让exec()方法结束，在用户选择确定或者取消后，关闭窗口结束事件循环，并返回最后用户选择的结果;
	// 根据返回结果得到用户按下了确定还是取消，采取相应的操作。从而模拟出QDialog类的exec()方法;

	m_pEventLoop->exec();
	return m_dialogCode;
}

void NExportLabeledObjectImagesWidget::actionClose()
{
	m_dialogCode = QDialog::Rejected;
	m_pEventLoop->exit();
	this->close();
}

void NExportLabeledObjectImagesWidget::actionOpenExportPath()
{
	QString strDirPath = QFileDialog::getExistingDirectory(this, QStringLiteral("Select export folder"), "");
	if (!strDirPath.isEmpty())
	{
		m_pLineEditExportPath->setText(strDirPath);
	}
}

void NExportLabeledObjectImagesWidget::actionExecuteExport()
{
	m_pToolButtonExecuteExport->setEnabled(false);
	QString strExportDirPath = m_pLineEditExportPath->text();
	QFuture<void> future = QtConcurrent::run(this, &NExportLabeledObjectImagesWidget::ExportThread, m_strCurrentImageDirPath, m_strCurrentAnnotationDirPath, strExportDirPath);
	m_pFutureWatcherExport->setFuture(future);
}

void NExportLabeledObjectImagesWidget::actionExportFinished()
{
	m_pToolButtonExecuteExport->setEnabled(true);
	m_pProgressBarExport->setValue(100);
}

void NExportLabeledObjectImagesWidget::actionExportProgressValueChanged(int nValue)
{
	m_pProgressBarExport->setValue(nValue);
}

void NExportLabeledObjectImagesWidget::SetupUI()
{
	m_pEventLoop = new QEventLoop(this);
	m_pFutureWatcherExport = new QFutureWatcher<void>(this);
	m_pVocAnnotationSerializer = new NVocAnnationSerializer(this);

	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setVerticalSpacing(5);
	m_pWindowTitleBar = new NWindowTitleBar(this);
	m_pWindowTitleBar->setObjectName("m_pWindowTitleBar");
	m_pWindowTitleBar->SetDialogMode(true);
	m_pWindowTitleBar->setFixedHeight(40);
	m_pWindowTitleBar->SetTitle(QStringLiteral("Export labeled object Images"));
	gridLayout->addWidget(m_pWindowTitleBar, 0, 0);

	m_pWidgetContent = new QWidget(this);
	m_pWidgetContent->setContentsMargins(10, 0, 10, 0);
	m_pWidgetContent->setObjectName("m_pWidgetContent");
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->setMargin(0);
	hLayout->setSpacing(3);
	m_pWidgetContent->setLayout(hLayout);

	m_pLineEditExportPath = new QLineEdit(this);
	m_pLineEditExportPath->setFixedHeight(40);
	m_pLineEditExportPath->setReadOnly(true);
	hLayout->addWidget(m_pLineEditExportPath);
	m_pToolButtonSelectExportPath = new QToolButton(this);
	m_pToolButtonSelectExportPath->setObjectName("m_pToolButtonSelectExportPath");
	m_pToolButtonSelectExportPath->setFixedSize(QSize(40, 40));
	m_pToolButtonSelectExportPath->setIcon(QIcon(":/labelImg/Resources/icons/open.png"));
	hLayout->addWidget(m_pToolButtonSelectExportPath);

	m_pToolButtonExecuteExport = new QToolButton(this);
	m_pToolButtonExecuteExport->setFixedHeight(40);
	m_pToolButtonExecuteExport->setText(QStringLiteral("Export"));
	hLayout->addWidget(m_pToolButtonExecuteExport);

	gridLayout->addWidget(m_pWidgetContent, 1, 0);

	m_pProgressBarExport = new QProgressBar(this);
	m_pProgressBarExport->setTextVisible(true);
	m_pProgressBarExport->setContentsMargins(10, 0, 0, 5);
	gridLayout->addWidget(m_pProgressBarExport, 2, 0);
	//gridLayout->setRowStretch(3, 10);

	this->setLayout(gridLayout);
	this->setFixedHeight(150);
}

void NExportLabeledObjectImagesWidget::SetupConnect()
{
	QObjectList objList = this->children();
	for (int nIndex = 0; nIndex < objList.size(); ++nIndex)
	{
		objList[nIndex]->installEventFilter(this);
	}

	connect(m_pFutureWatcherExport, &QFutureWatcher<void>::progressValueChanged, this, &NExportLabeledObjectImagesWidget::actionExportProgressValueChanged);

	connect(m_pFutureWatcherExport, &QFutureWatcher<void>::finished, this, &NExportLabeledObjectImagesWidget::actionExportFinished);

	connect(m_pToolButtonSelectExportPath, &QToolButton::clicked, this, &NExportLabeledObjectImagesWidget::actionOpenExportPath);

	connect(m_pToolButtonExecuteExport, &QToolButton::clicked, this, &NExportLabeledObjectImagesWidget::actionExecuteExport);

	connect(m_pWindowTitleBar, &NWindowTitleBar::postMove, this, [=](const QPoint& vec){
		if (!(this->windowState() & Qt::WindowMaximized || (this->windowState() & Qt::WindowFullScreen)))
		{
			this->move(this->pos() + vec);
		}
	});
	connect(m_pWindowTitleBar, &NWindowTitleBar::postClose, this, [=]{
		actionClose();
		this->close();
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

void NExportLabeledObjectImagesWidget::ExportThread(const QString& strImageDirPath, const QString& strAnnotationDirPath, const QString& strExportDirPath)
{
	QStringList strNameFilters;
	strNameFilters << "*.jpg" << "*.bmp" << "*.png";
	QDirIterator dirIter(strImageDirPath, strNameFilters, QDir::Files);
	QFileInfo imageFileInfo;
	QString strFileNameWithoutExt;
	QFileInfo annotationFileInfo;
	QFile annotationFile;
	SImageAnnotationInfo imageAnnotationInfo;
	cv::Mat matTmp;
	while (dirIter.hasNext())
	{
		QString strFilePath = dirIter.next();
		imageFileInfo.setFile(strFilePath);
		strFileNameWithoutExt = imageFileInfo.completeBaseName();
		matTmp = cv::imread(qPrintable(strFilePath), -1);

		if (!matTmp.empty() && m_pVocAnnotationSerializer->Read(strAnnotationDirPath % QDir::separator() % strFileNameWithoutExt % ".xml", imageAnnotationInfo))
		{
			ExportImageAnnotationInfo(strFileNameWithoutExt, matTmp, imageAnnotationInfo, strExportDirPath);
		}
		
	}
}

void NExportLabeledObjectImagesWidget::ExportImageAnnotationInfo(const QString& strFileName, const cv::Mat& matImage, const SImageAnnotationInfo& imageAnnotationInfo, const QString& strExportDirPath)
{
	QRect imageRect = QRect(0, 0, matImage.cols, matImage.rows);
	QString strSaveDirPath;
	QDir dir;
	QString strFilePath;
	for (int nIndex = 0; nIndex < imageAnnotationInfo.m_listAnnotationRects.size(); ++nIndex)
	{
		const SAnnotationRect& annotationRect = imageAnnotationInfo.m_listAnnotationRects[nIndex];
		if (imageRect.contains(annotationRect.m_rectOfObject) && !annotationRect.m_strObjectClassName.isEmpty())
		{
			strSaveDirPath = QStringLiteral("%1/%2").arg(strExportDirPath).arg(annotationRect.m_strObjectClassName);
			dir.setPath(strSaveDirPath);
			if (!dir.exists())
			{
				dir.mkpath(strSaveDirPath);
			}
			strFilePath = QStringLiteral("%1/%2(%3).jpg").arg(strSaveDirPath).arg(strFileName).arg(nIndex);
			const QRect& rectTmp = annotationRect.m_rectOfObject;
			cv::imwrite(qPrintable(strFilePath), matImage(cv::Rect(rectTmp.x(), rectTmp.y(), rectTmp.width(), rectTmp.height())));
		}

	}
}
