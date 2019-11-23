#pragma once
#include <QRect>
enum ECursorPosition
{
	CursorPositionUndefined,
	CursorPositionMiddle,
	CursorPositionTop,
	CursorPositionBottom,
	CursorPositionLeft,
	CursorPositionRight,
	CursorPositionTopLeft,
	CursorPositionTopRight,
	CursorPositionBottomLeft,
	CursorPositionBottomRight
};

struct SImageSize
{
	int m_nWidth;
	int m_nHeight;
	int m_nDepth;
};

struct SAnnotationRect
{
	//QString m_strImagePath;
	QString m_strObjectClassName;
	QRect m_rectOfObject;
	bool m_bDifficult;
	bool m_bHidden;
	SAnnotationRect() :m_bHidden(false), m_bDifficult(false)
	{

	}
	SAnnotationRect(const QString& strObjectClassName, const QRect& rect, bool bDifficult = false, bool bHidden = false) :
		m_strObjectClassName(strObjectClassName), m_rectOfObject(rect),m_bDifficult(bDifficult), m_bHidden(bHidden)
	{

	}
};

// ×¢ÊÍ±ê×¼
struct SImageAnnotationInfo
{
	QString m_strImageFileFolder;
	QString m_strImageFilePath;
	QString m_strImageFileName;
	QString m_strImageAnnotationPath;
	SImageSize m_imageSize;
	QList<SAnnotationRect> m_listAnnotationRects;
};