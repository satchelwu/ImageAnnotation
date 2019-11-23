#pragma once

#include <QWidget>

class QLabel;
class QListWidget;
class QListWidgetItem;
class NImagePathListWidget : public QWidget
{
	Q_OBJECT

public:
	NImagePathListWidget(QWidget *parent);
	~NImagePathListWidget();



public:
Q_SIGNALS :
	void postClickedImagePath(const QString& strImagePath);
		  void postCurrentImagePathChanged(const QString& strCurrentImagePath);
		  void postImagePathChanged(const QString& strCurrentImagePath, const QString& strPreviousImagePath);
		  void postDBClickedImagePath(const QString& strImagePath);
public:
	void AddImagePath(const QString& strImagePath);
	void AddImagPathList(const QStringList& strImagePathList);
	void ClearImagePathList();
	void SetTitle(const QString& strTitle);
	QListWidgetItem* CurrentItem();
	QString CurrentImagePath();
	const QString GetImagePath(int nRowIndex);

	void NextImagePath();
	void PrevImagePath();

private:

	void SetupUI();
	void SetupConnect();

private:
	QLabel* m_pLabelTitle;
	QListWidget* m_pImagePathListWidget;
};
