#include "NImageAnnotationWidget.h"
#include <qmenubar.h>
#include <QGridLayout>
#include <QToolBar>
#include <QPushButton>
#include <QDockWidget>
#include <QSpacerItem>
#include "NWindowTitleBar.h"
#include "NImagePathListWidget.h"
#include <QSplitter>
#include <QStatusBar>
#include <QFileDialog>
#include "NUtility.h"
#include <QStringBuilder>
#include "NAnnotationRectsWidget.h"
#include "NImageObjectMarkerWidget.h"
#include <QToolButton>
#include "NSelectClassLabelWidget.h"
#include <QTextStream>
#include <QFile>
#include "NVocAnnationSerializer.h"
#include <QMessageBox>
#include "NExportLabeledObjectImagesWidget.h"
#include "NUsageWidget.h"

NImageAnnotationWidget::NImageAnnotationWidget(QWidget *parent)
    : NDropShadowWidget(parent), m_bModified(false)
{
    ui.setupUi(this);
    SetupUI();
    SetupConnect();
}

NImageAnnotationWidget::~NImageAnnotationWidget()
{
    m_pSelectClassLabelWidget->deleteLater();
    m_pExportLabeledObjectImagesWidget->deleteLater();
	m_pUsageWidget->deleteLater();
    delete m_pImageAnnotationInfo;
}

void NImageAnnotationWidget::actionOpenDir()
{
    QString strDirPath = QFileDialog::getExistingDirectory(this, QStringLiteral("Select Image Dir"), "");
    if (!strDirPath.isEmpty())
    {
        m_pImageObjectMarkerWidget->SetImage(cv::Mat(), true);
        m_pImagePathListWidget->ClearImagePathList();
        m_pLabelImageDir->setText(strDirPath);
        m_pLabelAnnotationDir->setText(strDirPath);
        QStringList strImagePathList = NUtility::GetImagePathListByDir(strDirPath);
        m_pImagePathListWidget->AddImagPathList(strImagePathList);
        //m_pImagePathListWidget->index

    }

}

void NImageAnnotationWidget::actionChangeSaveDir()
{
    QString strDirPath = QFileDialog::getExistingDirectory(this, QStringLiteral("Select Annotation Dir"), "");
    if (!strDirPath.isEmpty())
    {
        m_pLabelAnnotationDir->setText(strDirPath);
        QString strCurrentImagePath = m_pImagePathListWidget->CurrentImagePath();
        QList<QRect> listRect;
        if (!strCurrentImagePath.isEmpty())
        {
            
            m_pAnnotationSerializer->Read(m_pLabelAnnotationDir->text() % QDir::separator() % QFileInfo(strCurrentImagePath).baseName() % ".xml", *m_pImageAnnotationInfo);
            m_pAnnotationRectsWidget->SetAnnotationRects(&m_pImageAnnotationInfo->m_listAnnotationRects);

            for (int nIndex = 0; nIndex < m_pImageAnnotationInfo->m_listAnnotationRects.size(); ++nIndex)
            {
                listRect.push_back(m_pImageAnnotationInfo->m_listAnnotationRects[nIndex].m_rectOfObject);
            }
        }
        m_pImageObjectMarkerWidget->SetRectList(listRect);
    }
}

void NImageAnnotationWidget::actionNextImage()
{
    m_pImagePathListWidget->NextImagePath();

}

void NImageAnnotationWidget::actionPrevImage()
{
    m_pImagePathListWidget->PrevImagePath();
}

void NImageAnnotationWidget::actionImagePathChanged(const QString& strCurrentPath, const QString& strPreviousPath)
{
    cv::Mat matOut;
    NUtility::GetImageFromFile(m_pLabelImageDir->text() % QDir::separator() % strCurrentPath, matOut);
    if (!m_pImageObjectMarkerWidget->GetImage().empty() && !strPreviousPath.isEmpty())
    {
        cv::Mat matPrevious = m_pImageObjectMarkerWidget->GetImage();
        m_pImageAnnotationInfo->m_imageSize.m_nWidth = matPrevious.cols;
        m_pImageAnnotationInfo->m_imageSize.m_nHeight = matPrevious.rows;
        m_pImageAnnotationInfo->m_imageSize.m_nDepth = matPrevious.channels();
        SaveAnnotation(strPreviousPath, *m_pImageAnnotationInfo);
    }
    m_bModified = false;
    *m_pImageAnnotationInfo = SImageAnnotationInfo();
    QList<QRect> listRect;
    if (!matOut.empty())
    {
        m_pAnnotationSerializer->Read(m_pLabelAnnotationDir->text() % QDir::separator() % QFileInfo(strCurrentPath).completeBaseName() % ".xml", *m_pImageAnnotationInfo);
        m_pAnnotationRectsWidget->SetAnnotationRects(&m_pImageAnnotationInfo->m_listAnnotationRects);

        for (int nIndex = 0; nIndex < m_pImageAnnotationInfo->m_listAnnotationRects.size(); ++nIndex)
        {
            listRect.push_back(m_pImageAnnotationInfo->m_listAnnotationRects[nIndex].m_rectOfObject);
        }
    }
    m_pImageObjectMarkerWidget->SetRectList(listRect);
    m_pImageObjectMarkerWidget->SetImage(matOut, true);
}

void NImageAnnotationWidget::actionBeginCreateBBox()
{
    if (!m_pImageObjectMarkerWidget->GetImage().empty())
    {
        m_pImageObjectMarkerWidget->BeginCreateBBox();
        m_pActionCreateBBox->setEnabled(false);
    }
}

void NImageAnnotationWidget::actionAddedRect(const QRect& rect, bool& bConfirm)
{
    if (m_pAnnotationRectsWidget->UseDefaultClassLabel())
    {
        bConfirm = true;
        m_pAnnotationRectsWidget->AddAnnotationRect(SAnnotationRect(m_pAnnotationRectsWidget->DefaultClassLabel(), rect, false, false));
        m_bModified = true;
    }
    else
    {
        m_pSelectClassLabelWidget->setFocus(Qt::MouseFocusReason);
        m_pSelectClassLabelWidget->move(QCursor::pos());
        if (QDialog::Accepted == m_pSelectClassLabelWidget->Exec())
        {
            bConfirm = true;
            m_pAnnotationRectsWidget->AddAnnotationRect(SAnnotationRect(m_pSelectClassLabelWidget->CurrentClassLabel(), rect, m_pSelectClassLabelWidget->CurrentIsDifficult(), false));
            m_bModified = true;
        }
        else
        {
            bConfirm = false;
        }
    }
    m_pActionCreateBBox->setEnabled(true);
}

void NImageAnnotationWidget::actionRectChanged(int nIndex, const QRect& rect)
{
    m_bModified = true;
    m_pAnnotationRectsWidget->SetCurrentRectIndex(nIndex, rect);
}

void NImageAnnotationWidget::actionSelectRectIndex(int nRectIndex, bool bHidden)
{
    m_pImageObjectMarkerWidget->SetCurrentRectIndex(nRectIndex, bHidden);
}

void NImageAnnotationWidget::actionDeleteRectIndex(int nRectIndex)
{
    m_bModified = true;
    m_pImageObjectMarkerWidget->DeleteRectIndex(nRectIndex);
}

void NImageAnnotationWidget::actionExportLabeledObjectImages()
{
    m_pExportLabeledObjectImagesWidget->SetImageDirAndAnnotationDir(m_pLabelImageDir->text(), m_pLabelAnnotationDir->text());
    m_pExportLabeledObjectImagesWidget->Exec();
}

bool NImageAnnotationWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Enter)
    {
        this->setCursor(Qt::ArrowCursor);
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        QWidget* pWidget = qobject_cast<QWidget*>(watched);
        if (pWidget)
        {
            pWidget->setFocus();
        }
    }
    return NDropShadowWidget::eventFilter(watched, event);
}

void NImageAnnotationWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Up)
    {
        actionPrevImage();
    }
    else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_Down)
    {
        actionNextImage();
    }
    return NDropShadowWidget::keyPressEvent(event);
}

void NImageAnnotationWidget::closeEvent(QCloseEvent *event)
{
    QString strCurrentImagePath = m_pImagePathListWidget->CurrentImagePath();
    if (!strCurrentImagePath.isEmpty())
    {
        SaveAnnotation(strCurrentImagePath, *m_pImageAnnotationInfo);
    }
    return NDropShadowWidget::closeEvent(event);
}

void NImageAnnotationWidget::SetupUI()
{
    m_pImageAnnotationInfo = new SImageAnnotationInfo();
    m_pAnnotationSerializer = new NVocAnnationSerializer(this);
    //m_pAnnotationRectsWidget = new NAnnotationRectsWidget(this);

    m_pSelectClassLabelWidget = new NSelectClassLabelWidget();
    m_pExportLabeledObjectImagesWidget = new NExportLabeledObjectImagesWidget();
    m_pUsageWidget = new NUsageWidget();

    m_pGridLayout = new QGridLayout;
    m_pGridLayout->setVerticalSpacing(0);

    CreateTitleBar();
    CreateMenubar();
    CreateToolBar();
    CreateContentWidget();
    CreateStatusBar();


    QLayout* layout = this->layout();
    if (layout)
    {
        layout->addItem(m_pGridLayout);
    }
    else
    {
        this->setLayout(m_pGridLayout);
    }
    SetShadowWidth(3);
    m_pImagePathListWidget->setFocus(Qt::MouseFocusReason);
    LoadLabels();
}

void NImageAnnotationWidget::SetupConnect()
{
    QObjectList objList = this->children();
    for (int nIndex = 0; nIndex < objList.size(); ++nIndex)
    {
        objList[nIndex]->installEventFilter(this);
    }

    connect(m_pImagePathListWidget, &NImagePathListWidget::postImagePathChanged, this, &NImageAnnotationWidget::actionImagePathChanged);
    connect(m_pImageObjectMarkerWidget, &NImageObjectMarkerWidget::postAddedRect, this, &NImageAnnotationWidget::actionAddedRect);
    connect(m_pImageObjectMarkerWidget, &NImageObjectMarkerWidget::postRectChanged, this, &NImageAnnotationWidget::actionRectChanged);
    connect(m_pAnnotationRectsWidget, &NAnnotationRectsWidget::postCurrentIndex, this, &NImageAnnotationWidget::actionSelectRectIndex);
    connect(m_pAnnotationRectsWidget, &NAnnotationRectsWidget::postDeleteRectIndex, this, &NImageAnnotationWidget::actionDeleteRectIndex);
}

void NImageAnnotationWidget::CreateTitleBar()
{
    m_pTitleDropDownMenu = new QMenu(this);
    QAction * action = new QAction(QStringLiteral("Settings"), this);
    m_pTitleDropDownMenu->addAction(action);

    m_pWindowTitleBar = new NWindowTitleBar(this);
    m_pWindowTitleBar->setFixedHeight(50);
    m_pWindowTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_pWindowTitleBar, &NWindowTitleBar::postMove, this, [=](const QPoint& vec){
        if (!(this->windowState() & Qt::WindowMaximized || (this->windowState() & Qt::WindowFullScreen)))
        {
            this->move(this->pos() + vec);
        }
    });
    connect(m_pWindowTitleBar, &NWindowTitleBar::postClose, this, &NImageAnnotationWidget::close);
    connect(m_pWindowTitleBar, &NWindowTitleBar::postMaxmize, this, [=]{

        Qt::WindowStates ws = this->windowState();
        if (ws & Qt::WindowMaximized)
        {
            this->SetShadowWidth(3);
            this->showNormal();
        }
        else if (ws == Qt::WindowNoState)
        {
            this->SetShadowWidth(0);
            this->showMaximized();
        }
    });
    connect(m_pWindowTitleBar, &NWindowTitleBar::postMinimize, this, [=]{
        if (!((this->windowState() & Qt::WindowMinimized) || this->windowState() & Qt::WindowFullScreen))
        {
            this->showMinimized();
        }
    });

    connect(m_pWindowTitleBar, &NWindowTitleBar::postDropDownMenuPos, this, [=](const QPoint& globalPos){
        QPoint pos = this->mapFromGlobal(globalPos);
        //pos = m_pWindowTitleBar->mapFromGlobal(globalPos);
        m_pTitleDropDownMenu->popup(globalPos);
    });

    connect(m_pWindowTitleBar, &NWindowTitleBar::postDoubleClicked, this, [=]{
        Qt::WindowStates ws = this->windowState();
        if (ws & Qt::WindowMaximized)
        {
            this->SetShadowWidth(3);
            this->showNormal();
        }
        else if (ws == Qt::WindowNoState)
        {
            this->SetShadowWidth(0);
            this->showMaximized();
        }
    });

    m_pGridLayout->addWidget(m_pWindowTitleBar, 0, 0, 1, 10);
    //m_pGridLayout->setRowStretch(1, 10);
}

void NImageAnnotationWidget::CreateMenubar()
{
    QMenuBar* menuBar = new QMenuBar(this);
    m_pGridLayout->addWidget(menuBar, 1, 0, 1, 10);
    //m_pGridLayout->setRowStretch(2, 10);

    QMenu* menu = new QMenu(QStringLiteral("File"), this);
    menuBar->addMenu(menu);
    QAction* pAction = new QAction(QIcon(":/labelImg/Resources/icons/open.png"), QStringLiteral("Open Image"), this);
    connect(pAction, &QAction::triggered, this, &NImageAnnotationWidget::actionOpenDir);
    menu->addAction(pAction);

    pAction = new QAction(QIcon(":/labelImg/Resources/icons/open.png"), QStringLiteral("ChangeSaveDir"), this);
    connect(pAction, &QAction::triggered, this, &NImageAnnotationWidget::actionChangeSaveDir);
    menu->addAction(pAction);

    menu = new QMenu(QStringLiteral("Edit"), this);
    menuBar->addMenu(menu);
    pAction = new QAction(QIcon(":/labelImg/Resources/icons/objects.png"), QStringLiteral("Create Box"), this);
    //pAction->setCheckable(true);
    menu->addAction(pAction);

    menu = new QMenu(QStringLiteral("About"), this);
    menuBar->addMenu(menu);
    pAction = new QAction(QStringLiteral("Author"), this);

    connect(pAction, &QAction::triggered, this, [=]{
        QMessageBox::about(this, QStringLiteral("About"), QStringLiteral("Author Wu Qiang thin.wu@163.com"));
    });
    //pAction->setCheckable(true);
    menu->addAction(pAction);

	pAction = new QAction(QStringLiteral("Usage"), this);
	connect(pAction, &QAction::triggered, this, [=]{
		m_pUsageWidget->Exec();
	});
	menu->addAction(pAction);




}

void NImageAnnotationWidget::CreateToolBar()
{
    QToolBar* toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolbar->setMovable(true);
    toolbar->setAllowedAreas(Qt::AllToolBarAreas);
    //toolbar->addWidget(new QPushButton(QStringLiteral("createBox"), this));

    m_pGridLayout->addWidget(toolbar, 2, 0, 1, 10);

    QAction* pAction = new QAction(QIcon(":/labelImg/Resources/icons/open.png"), QStringLiteral("OpenDir"), this);
    pAction->setShortcut(Qt::CTRL | Qt::Key_O);
    toolbar->addAction(pAction);
    connect(pAction, &QAction::triggered, this, &NImageAnnotationWidget::actionOpenDir);

    pAction = new QAction(QIcon(":/labelImg/Resources/icons/open.png"), QStringLiteral("ChangeSaveDir"), this);
    pAction->setShortcut(Qt::CTRL | Qt::Key_P);
    toolbar->addAction(pAction);
    connect(pAction, &QAction::triggered, this, &NImageAnnotationWidget::actionChangeSaveDir);

    m_pActionPrevImage = new QAction(QIcon(":/labelImg/Resources/icons/prev.png"), QStringLiteral("Prev Image"), this);
    QList<QKeySequence>  listShortCut;
    listShortCut << Qt::Key_Up << Qt::Key_Left;
    m_pActionPrevImage->setShortcuts(listShortCut);
    toolbar->addAction(m_pActionPrevImage);
    connect(m_pActionPrevImage, &QAction::triggered, this, &NImageAnnotationWidget::actionPrevImage);

    m_pActionNextImage = new QAction(QIcon(":/labelImg/Resources/icons/next.png"), QStringLiteral("Next Image"), this);
    listShortCut.clear();
    listShortCut << Qt::Key_Down << Qt::Key_Right;
    m_pActionNextImage->setShortcuts(listShortCut);
    toolbar->addAction(m_pActionNextImage);
    connect(m_pActionNextImage, &QAction::triggered, this, &NImageAnnotationWidget::actionNextImage);

    m_pActionCreateBBox = new QAction(QIcon(":/labelImg/Resources/icons/objects.png"), QStringLiteral("Create(w)"), this);
    m_pActionCreateBBox->setShortcut(Qt::Key_W);
    connect(m_pActionCreateBBox, &QAction::triggered, this, &NImageAnnotationWidget::actionBeginCreateBBox);
    toolbar->addAction(m_pActionCreateBBox);
    
    m_pActionExportLabeledObjectImages = new QAction(QIcon(":/labelImg/Resources/icons/export.png"), QStringLiteral("Export"), this);
    connect(m_pActionExportLabeledObjectImages, &QAction::triggered, this, &NImageAnnotationWidget::actionExportLabeledObjectImages);
    toolbar->addAction(m_pActionExportLabeledObjectImages);
}

void NImageAnnotationWidget::CreateContentWidget()
{
    QSplitter* hSplitter = new QSplitter(Qt::Horizontal);
    m_pGridLayout->addWidget(hSplitter, 3, 0, 1, 10);

    //m_pImageDrawer = new NImageDrawer(this);
    //hSplitter->addWidget(m_pImageDrawer);

    m_pImageObjectMarkerWidget = new NImageObjectMarkerWidget(this);
	m_pImageObjectMarkerWidget->setMouseTracking(true);
    hSplitter->addWidget(m_pImageObjectMarkerWidget);


    QSplitter* vSplitter = new QSplitter(Qt::Vertical);
    //vSplitter->setContentsMargins(0, 0, 0, 0);
    m_pAnnotationRectsWidget = new NAnnotationRectsWidget(this);
    m_pAnnotationRectsWidget->SetTitle(QStringLiteral("Annotation Rects"));
    vSplitter->addWidget(m_pAnnotationRectsWidget);

    // 	SAnnotationRect annotationRect;
    // 	annotationRect.m_strObjectClassName = QStringLiteral("dog");
    // 	annotationRect.m_rectOfObject = QRect(0, 0, 300, 500);
    // 
    // 	m_pAnnotationRectsWidget->AddAnnotationRect(annotationRect);

    m_pImagePathListWidget = new NImagePathListWidget(this);
    m_pImagePathListWidget->SetTitle(QStringLiteral("Image Path List"));
    vSplitter->addWidget(m_pImagePathListWidget);



    hSplitter->addWidget(vSplitter);

    hSplitter->setStretchFactor(0, 2);
    hSplitter->setStretchFactor(1, 1);
    //m_pImagePathListWidget->setFixedWidth(200);

    // 	m_pImagePathListWidget = new NImagePathListWidget(this);
    // 	m_pSpliterContent->addWidget(m_pImagePathListWidget);
    //m_pImagePathListWidget->setFixedWidth(200);


    //QStringList strList;
    //strList << QStringLiteral("Test1") << QStringLiteral("Test2");
    //m_pImagePathListWidget->AddImagPathList(strList);
    //m_pGridLayout->setRowStretch(5, 10);
}

void NImageAnnotationWidget::CreateStatusBar()
{
    QWidget* statusWidget = new QWidget(this);
    statusWidget->setObjectName(QStringLiteral("statusWidget"));
    statusWidget->setMaximumHeight(30);
    QGridLayout* statusLayout = new QGridLayout();
    statusLayout->setMargin(0);
    statusLayout->setSpacing(0);

    QHBoxLayout *hlayout1 = new QHBoxLayout();
    hlayout1->setSpacing(0);
    hlayout1->setMargin(0);
    QLabel* labelImageDir = new QLabel(QStringLiteral("Image Dir:"), statusWidget);
    labelImageDir->setObjectName("labelImageDir");
    labelImageDir->setMaximumWidth(100);
    hlayout1->addWidget(labelImageDir);
    m_pLabelImageDir = new QLabel(statusWidget);
    m_pLabelImageDir->setObjectName("m_pLabelImageDir");
    hlayout1->addWidget(m_pLabelImageDir);

    QHBoxLayout *hlayout2 = new QHBoxLayout();
    hlayout2->setSpacing(0);
    hlayout2->setMargin(0);
    QLabel* labelAnnotationDir = new QLabel(QStringLiteral("Annotation Dir:"), statusWidget);
    labelAnnotationDir->setObjectName("labelAnnotationDir");
    labelAnnotationDir->setMaximumWidth(140);
    hlayout2->addWidget(labelAnnotationDir);
    m_pLabelAnnotationDir = new QLabel(statusWidget);
    m_pLabelAnnotationDir->setObjectName("m_pLabelAnnotationDir");
    hlayout2->addWidget(m_pLabelAnnotationDir);
    //statusLayout->setColumnStretch(0, 1);
    //statusLayout->setColumnStretch(1, 1);
    //statusLayout->setColumnStretch(2, 1);
    //statusLayout->setColumnStretch(3, 4);

    statusLayout->addLayout(hlayout1, 0, 0);
    statusLayout->addLayout(hlayout2, 0, 1);

    statusWidget->setLayout(statusLayout);

    m_pGridLayout->addWidget(statusWidget, 4, 0, 1, 10);
    //m_pGridLayout->setRowStretch(5, 10);
    m_pGridLayout->setRowStretch(3, 5);
    m_pGridLayout->setRowStretch(4, 1);
}

void NImageAnnotationWidget::LoadLabels()
{
    QStringList strLabelList;
    QFile file("labels.txt");
    if (file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        QString strLine;
        QTextStream ts(&file);
        while (!ts.atEnd())
        {
            strLabelList << ts.readLine().split(QRegExp("[ \t\n,;]"), QString::SkipEmptyParts);
        }
    }
    m_pSelectClassLabelWidget->SetLabels(strLabelList);
}

void NImageAnnotationWidget::SaveAnnotation(const QString& strFileName, SImageAnnotationInfo& annotationInfo)
{
    if (strFileName.isEmpty())
        return;
    if (m_bModified)
    {
        annotationInfo.m_strImageFileFolder = m_pLabelImageDir->text().trimmed();
        annotationInfo.m_strImageFileName = strFileName;
        m_pAnnotationSerializer->Write(m_pLabelAnnotationDir->text().trimmed() % QDir::separator() % QFileInfo(annotationInfo.m_strImageFileName).completeBaseName() % ".xml", annotationInfo);
    }
}
