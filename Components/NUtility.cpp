#include "NUtility.h"
#include <QDir>
#include <QtDebug>
#include <QFile>

NUtility::NUtility()
{
}

NUtility::~NUtility()
{
}

const QStringList NUtility::GetImagePathListByDir(const QString& strDirPath)
{
	QDir dir(strDirPath);
	if (dir.exists())
	{
		QStringList strNameFilters;
		strNameFilters << "*.jpg" << "*.bmp" << "*.png" << "*.jpeg";
		QStringList strImagePathList = dir.entryList(strNameFilters, QDir::Files);
		return QStringList(strImagePathList);
	}
	return QStringList();
}

void NUtility::GetImageFromFile(const QString& strImagePath, cv::Mat& matOut)
{
	matOut = cv::imread(qPrintable(strImagePath), -1);
}

// QImage NUtility::CvMat2QImage(const cv::Mat& matSrc)
// {
// 	static QVector<QRgb> m_vecColorTable;
// 	static bool bFirst = true;
// 	if (bFirst)
// 	{
// 		for (int k = 0; k < 256; ++k)
// 		{
// 			m_vecColorTable.push_back(qRgb(k, k, k));
// 		}
// 		bFirst = false;
// 	}
// 	
// 	if (matSrc.empty())
// 		return QImage();
// 
// 	cv::Mat matTemp;
// 	QImage image;
// 	switch (matSrc.type())
// 	{
// 	case CV_8UC1:
// 		image = QImage(matSrc.data, matSrc.cols, matSrc.rows, matSrc.step, QImage::Format_Indexed8);
// 		image.setColorTable(m_vecColorTable);
// 		return image;
// 	case CV_8UC3:
// 		cv::cvtColor(matSrc, matTemp, cv::COLOR_BGR2RGB);
// 		return QImage(matTemp.data, matTemp.cols, matTemp.rows, matTemp.step, QImage::Format_RGB888);
// 	case CV_8UC4:
// 		cv::cvtColor(matSrc, matTemp, cv::COLOR_BGRA2RGBA);
// 		return QImage(matTemp.data, matTemp.cols, matTemp.rows, matTemp.step, QImage::Format_RGBA8888);
// 	default:
//		return QImage();
// 	}
// }

QImage NUtility::CvMat2QImage(const cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1  
	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)  
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat  
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3  
	else if (mat.type() == CV_8UC3)
	{
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		qDebug() << "CV_8UC4";
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		qDebug() << "ERROR: Mat could not be converted to QImage.";
		return QImage();
	}
}

cv::Mat NUtility::QImageToMat(const QImage& image)
{
	cv::Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Grayscale8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}


void NUtility::SetStyleSheet(QWidget* pWidget, const QString& strQssPath)
{
	if (pWidget)
	{
		QFile file(strQssPath);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			pWidget->setStyleSheet(file.readAll());
		}
		file.close();
	}
}

