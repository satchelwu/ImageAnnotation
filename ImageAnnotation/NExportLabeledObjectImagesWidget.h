#pragma once

#include "NDropShadowWidget.h"
#include <QFutureWatcher>
#include <opencv2/opencv.hpp>
class NWindowTitleBar;
class QProgressBar;
class QLineEdit;
class QToolButton;
class QEventLoop;
class NVocAnnationSerializer;

class NExportLabeledObjectImagesWidget : public NDropShadowWidget
{
	Q_OBJECT

public:
	NExportLabeledObjectImagesWidget(QWidget *parent = NULL);
	~NExportLabeledObjectImagesWidget();
	void SetImageDirAndAnnotationDir(const QString& strImageDirPath, QString& strAnnotationDirPath);

	QDialog::DialogCode Exec();
	private Q_SLOTS:
	void actionClose();
	void actionOpenExportPath();
	void actionExecuteExport();
	void actionExportFinished();
	void actionExportProgressValueChanged(int nValue);
private:
	void SetupUI();
	void SetupConnect();
	void ExportThread(const QString& strImageDirPath , const QString& strAnnotationDirPath, const QString& strExportDirPath);
	void ExportImageAnnotationInfo(const QString& strFileName, const cv::Mat& matImage, const SImageAnnotationInfo& imageAnnotationInfo, const QString& strExportDirPath);
private:
	NWindowTitleBar* m_pWindowTitleBar;
	QLineEdit* m_pLineEditExportPath;
	QToolButton* m_pToolButtonSelectExportPath;
	QToolButton* m_pToolButtonExecuteExport;
	QProgressBar* m_pProgressBarExport;
	QWidget* m_pWidgetContent;
	QEventLoop* m_pEventLoop;
	QDialog::DialogCode m_dialogCode;
	QString m_strCurrentAnnotationDirPath;
	QString m_strCurrentImageDirPath;
	QFutureWatcher<void>* m_pFutureWatcherExport;
	NVocAnnationSerializer* m_pVocAnnotationSerializer;
};
