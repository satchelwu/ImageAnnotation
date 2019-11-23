#pragma once

#include "NDropShadowWidget.h"
#include "ui_NImageAnnotationWidget.h"
#include "NImageDrawer.h"
#include "NImageCropper.h"

class QGridLayout;
class NWindowTitleBar;
class NImagePathListWidget;
class QSplitter;
class QLabel;
class NAnnotationRectsWidget;
class NImageObjectMarkerWidget;
class NSelectClassLabelWidget;
class NAbstractAnnotationSerializer;
class NExportLabeledObjectImagesWidget;
class NUsageWidget;

class NImageAnnotationWidget : public NDropShadowWidget
{
	Q_OBJECT

public:
	NImageAnnotationWidget(QWidget *parent = Q_NULLPTR);
	~NImageAnnotationWidget();
	private Q_SLOTS:
	void actionOpenDir();
	void actionChangeSaveDir();
	void actionNextImage();
	void actionPrevImage();
	void actionImagePathChanged(const QString& strCurrentPath, const QString& strPreviousPath);
	void actionBeginCreateBBox();
	void actionAddedRect(const QRect& rect, bool& bConfirm);
	void actionRectChanged(int nIndex, const QRect& rect);
	void actionSelectRectIndex(int nRectIndex, bool bHidden);
	void actionDeleteRectIndex(int nRectIndex);
	void actionExportLabeledObjectImages();
protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void closeEvent(QCloseEvent *event);
private:
	void SetupUI();
	void SetupConnect();

	void CreateTitleBar();
	void CreateMenubar();
	void CreateToolBar();
	void CreateContentWidget();
	void CreateStatusBar();
	void LoadLabels();
	void SaveAnnotation(const QString& strFileName, SImageAnnotationInfo& annotationInfo);
private:
	Ui::NImageAnnotationWidget	ui;
	NWindowTitleBar*			m_pWindowTitleBar;
	QGridLayout*				m_pGridLayout;
	QMenu*						m_pTitleDropDownMenu;
	NImagePathListWidget*		m_pImagePathListWidget;
	NAnnotationRectsWidget*		m_pAnnotationRectsWidget;
	NImageDrawer*				m_pImageDrawer;
	NImageCropper*				m_pImageCropper;
	NImageObjectMarkerWidget*	m_pImageObjectMarkerWidget;
	NSelectClassLabelWidget*	m_pSelectClassLabelWidget;
	NExportLabeledObjectImagesWidget* m_pExportLabeledObjectImagesWidget;
	NUsageWidget*				m_pUsageWidget;
	QLabel*						m_pLabelImageDir;
	QLabel*						m_pLabelAnnotationDir;

	QAction*					m_pActionPrevImage;
	QAction*					m_pActionNextImage;
	QAction*					m_pActionCreateBBox;
	QAction*					m_pActionExportLabeledObjectImages;

	NAbstractAnnotationSerializer* m_pAnnotationSerializer;
	SImageAnnotationInfo*		m_pImageAnnotationInfo;
	bool m_bModified;
};
