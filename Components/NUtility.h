#pragma once
#include <QStringList>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QWidget>


class NUtility
{
public:
	NUtility();
	~NUtility();

	static const QStringList GetImagePathListByDir(const QString& strDirPath);
	static void GetImageFromFile(const QString& strImagePath, cv::Mat& matOut);
	//static QImage CvMat2QImage(const cv::Mat& matSrc);
	static QImage CvMat2QImage(const cv::Mat& matSrc);
	static cv::Mat QImageToMat(const QImage& image);
	static void SetStyleSheet(QWidget* pWidget, const QString& strQssPath);

	template <typename _Tp>
	static void Olbp(cv::InputArray _src, cv::OutputArray _dst)
	{

		// get matrices
		cv::Mat src = _src.getMat();
		// allocate memory for result
		_dst.create(src.rows - 2, src.cols - 2, CV_8UC1);
		cv::Mat dst = _dst.getMat();
		// zero the result matrix
		dst.setTo(0);
		// calculate patterns
		for (int i = 1; i < src.rows - 1; i++) {
			for (int j = 1; j < src.cols - 1; j++) {
				_Tp center = src.at<_Tp>(i, j);
				unsigned char code = 0;
				code |= (src.at<_Tp>(i - 1, j - 1) >= center) << 7;
				code |= (src.at<_Tp>(i - 1, j) >= center) << 6;
				code |= (src.at<_Tp>(i - 1, j + 1) >= center) << 5;
				code |= (src.at<_Tp>(i, j + 1) >= center) << 4;
				code |= (src.at<_Tp>(i + 1, j + 1) >= center) << 3;
				code |= (src.at<_Tp>(i + 1, j) >= center) << 2;
				code |= (src.at<_Tp>(i + 1, j - 1) >= center) << 1;
				code |= (src.at<_Tp>(i, j - 1) >= center) << 0;
				dst.at<unsigned char>(i - 1, j - 1) = code;
			}
		}
	}
};




