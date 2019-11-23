#include "NImageObjectMarkerWidget.h"
#include <QPainter>
#include <QApplication>
#include <QClipboard>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QMouseEvent>
#include <QtDebug>

NImageObjectMarkerWidget::NImageObjectMarkerWidget(QWidget *parent)
	: NImageDisplayWidget(parent), m_colorCroppingBackground(Qt::darkGray), m_colorCroppingRectBorder(Qt::magenta), m_eAnnotationMode(Undefined),
	m_colorRectNormal(Qt::blue), m_pCurrentCroppingRect(NULL), m_penCroppingRect(QColor(Qt::red), 2, Qt::DashDotDotLine), m_penHoverRect(QColor(Qt::blue), 2),
	m_penNormalRect(QColor(Qt::green), 2), m_pCurrentHoverRect(NULL)
{
	m_colorCroppingBackground.setAlphaF(0.5f);
	SetupUI();
	SetupConnect();
}

NImageObjectMarkerWidget::~NImageObjectMarkerWidget()
{
}


void NImageObjectMarkerWidget::SetCroppingRectBorderColor(const QColor& color)
{
	m_colorCroppingRectBorder = color;
}

void NImageObjectMarkerWidget::SetCroppingBackgroundColor(const QColor& color)
{
	m_colorCroppingBackground = color;
	m_colorCroppingBackground.setAlphaF(0.3f);
}



void NImageObjectMarkerWidget::BeginCreateBBox()
{
	m_eAnnotationMode = Drawing;
	this->setCursor(Qt::CrossCursor);
	m_pCurrentCroppingRect = new QRect();
	update();
}

void NImageObjectMarkerWidget::EndCreateBBox()
{
	bool bConfirm = false;
	emit postAddedRect(*m_pCurrentCroppingRect, bConfirm);
	if (bConfirm)
	{
		m_listRect.push_back(m_pCurrentCroppingRect);
		m_listRectVisible.push_back(true);
	}
	else
	{
		delete m_pCurrentCroppingRect;
		m_pCurrentCroppingRect = NULL;
		update();
	}
	m_eAnnotationMode = Undefined;
}

void NImageObjectMarkerWidget::SetCurrentRectIndex(int nRectIndex, bool bHidden)
{
	if (m_eAnnotationMode == Drawing)
	{
		if (nRectIndex >= 0 && nRectIndex < m_listRect.size())
		{
			m_listRectVisible[nRectIndex] = !bHidden;
			update();
		}
	}
	else
	{
		if (nRectIndex >= 0 && nRectIndex < m_listRect.size())
		{

			if (m_pCurrentCroppingRect == m_listRect[nRectIndex])
			{
				if (bHidden == true)
				{
					m_pCurrentCroppingRect = NULL;
				}

			}
			else
			{
				if (bHidden == true)
				{
					m_pCurrentCroppingRect = NULL;
				}
				else
				{
					m_pCurrentCroppingRect = m_listRect[nRectIndex];
				}
			}
			m_listRectVisible[nRectIndex] = !bHidden;
			update();
		}
		else
		{
			m_pCurrentCroppingRect = NULL;
			update();
		}
	}

}

void NImageObjectMarkerWidget::DeleteRectIndex(int nRectIndex)
{
	if (nRectIndex >= 0 && nRectIndex < m_listRect.size())
	{
		m_listRect.removeAt(nRectIndex);
		m_listRectVisible.removeAt(nRectIndex);
		m_pCurrentCroppingRect = NULL;
		update();
	}
}

void NImageObjectMarkerWidget::SetRectList(const QList<QRect>& listRect)
{
	//m_pCurrentCroppingRect = NULL;
	m_pCurrentHoverRect = NULL;
	qDeleteAll(m_listRect);
	m_listRect.clear();
	m_listRectVisible.clear();
	QRect* pRect = NULL;
	for (int nIndex = 0; nIndex < listRect.size(); ++nIndex)
	{
		pRect = new QRect(listRect[nIndex]);
		m_listRect.push_back(pRect);
		m_listRectVisible.push_back(true);
	}
}

void NImageObjectMarkerWidget::paintEvent(QPaintEvent *event)
{
	NImageDisplayWidget::paintEvent(event);
	QPainter painter(this);
	if (!m_matSrc.empty())
	{
		painter.setPen(QPen(m_colorRectNormal, 2));
		QRect rectTmp;
		for (int nIndex = 0; nIndex < m_listRect.size(); ++nIndex)
		{
			if (m_pCurrentCroppingRect != m_listRect[nIndex] && m_listRectVisible[nIndex])
				DrawRect(&painter, *m_listRect[nIndex], m_penNormalRect);
		}
		if (m_pCurrentCroppingRect)
		{
			if (m_eAnnotationMode == Drawing)
			{
				DrawRect(&painter, *m_pCurrentCroppingRect, m_penCroppingRect);
			}
			else
			{
				int nRowIndex = RectIndex(m_pCurrentCroppingRect);
				if (m_listRectVisible[nRowIndex])
				{
					DrawRect(&painter, *m_pCurrentCroppingRect, m_penCroppingRect);
				}
			}

		}
		else
		{
			if (m_pCurrentHoverRect)
			{
				DrawRect(&painter, *m_pCurrentHoverRect, m_penHoverRect);
			}
		}
	}
}

void NImageObjectMarkerWidget::mousePressEvent(QMouseEvent *event)
{
	if (m_matSrc.empty())
		return;
	this->setFocus();
	if (event->button() == Qt::LeftButton)
	{
		if (m_eAnnotationMode == Drawing)
		{
			m_startDrawingPoint = event->pos();
		}
		else
		{

			m_pCurrentCroppingRect = RectAt(event->pos());
			if (m_pCurrentCroppingRect != NULL)
			{
				m_eCursorPosition = CursorPosition(RectifiedRect(*m_pCurrentCroppingRect), event->pos());
				if (m_eCursorPosition == CursorPositionMiddle)
				{
					m_eAnnotationMode = NImageObjectMarkerWidget::Move;
					m_startDrawingPoint = event->pos();
				}
				else if (m_eCursorPosition == CursorPositionUndefined)
				{
					m_eAnnotationMode = NImageObjectMarkerWidget::Undefined;
					NImageDisplayWidget::mousePressEvent(event);

					//setCursor(Qt::ArrowCursor);
				}
				else
				{
					m_eAnnotationMode = NImageObjectMarkerWidget::Resize;
				}
				UpdateCursorIcon(event->pos(), *m_pCurrentCroppingRect);
				update();
			}
			else
			{
				m_pCurrentHoverRect = NULL;
				m_eAnnotationMode = NImageObjectMarkerWidget::Undefined;
				NImageDisplayWidget::mousePressEvent(event);
				update();
			}

			if (m_eAnnotationMode != NImageObjectMarkerWidget::Undefined && m_pCurrentCroppingRect != NULL)
			{

				emit postRectChanged(RectIndex(m_pCurrentCroppingRect), *m_pCurrentCroppingRect);
			}
		}
	}
}

void NImageObjectMarkerWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (m_matSrc.empty())
		return;
	if (event->buttons() & Qt::LeftButton)
	{
		if (m_eAnnotationMode == NImageObjectMarkerWidget::Drawing)
		{
			*m_pCurrentCroppingRect = QRect(m_startDrawingPoint, event->pos());
			*m_pCurrentCroppingRect = MapToOrignalImage(m_pCurrentCroppingRect->normalized());
			RestrictRect(*m_pCurrentCroppingRect);
			update();
		}
		else if (m_eAnnotationMode == NImageObjectMarkerWidget::Move)
		{
			if (m_listRectVisible[RectIndex(m_pCurrentCroppingRect)])
			{
				m_pCurrentCroppingRect->moveTopLeft(m_pCurrentCroppingRect->topLeft() + MapToOrignalImage(event->pos()) - MapToOrignalImage(m_startDrawingPoint));
				RestrictRect(m_pCurrentCroppingRect);
				m_startDrawingPoint = event->pos();
				update();
			}
		}
		else if (m_eAnnotationMode == NImageObjectMarkerWidget::Resize)
		{
			if (m_listRectVisible[RectIndex(m_pCurrentCroppingRect)])
			{
				switch (m_eCursorPosition)
				{
				case CursorPositionTop:
				{
					m_pCurrentCroppingRect->setTop(MapToOrignalImage(event->pos()).y());
				}
					break;
				case CursorPositionBottom:
				{
					m_pCurrentCroppingRect->setBottom(MapToOrignalImage(event->pos()).y());
				}
					break;
				case CursorPositionLeft:
				{
					m_pCurrentCroppingRect->setX(MapToOrignalImage(event->pos()).x());
				}
					break;
				case CursorPositionRight:
				{
					m_pCurrentCroppingRect->setRight(MapToOrignalImage(event->pos()).x());
				}
					break;
				case CursorPositionTopLeft:
				{
					m_pCurrentCroppingRect->setTopLeft(MapToOrignalImage(event->pos()));
				}
					break;
				case CursorPositionTopRight:
				{
					m_pCurrentCroppingRect->setTopRight(MapToOrignalImage(event->pos()));
				}
					break;
				case CursorPositionBottomLeft:
				{
					m_pCurrentCroppingRect->setBottomLeft(MapToOrignalImage(event->pos()));
				}
					break;
				case CursorPositionBottomRight:
				{
					m_pCurrentCroppingRect->setBottomRight(MapToOrignalImage(event->pos()));
				}
					break;
				}
				//*m_pCurrentCroppingRect = m_pCurrentCroppingRect->normalized();
				RestrictResizingRect(m_eCursorPosition, *m_pCurrentCroppingRect);
				RestrictRect(m_pCurrentCroppingRect);
				//*m_pCurrentCroppingRect = m_pCurrentCroppingRect->normalized();
				update();
			}
		}
		else
		{
			NImageDisplayWidget::mouseMoveEvent(event);
		}
	}
	else
	{
		if (m_eAnnotationMode != Drawing)
		{
			if (m_pCurrentCroppingRect == NULL)
			{
				m_pCurrentHoverRect = RectAt(event->pos());
				if (m_pCurrentHoverRect != NULL)
				{
					//m_eCursorPosition = CursorPosition(RectifiedRect(*m_pCurrentHoverRect), event->pos());
					UpdateCursorIcon(event->pos(), *m_pCurrentHoverRect);
					//update();
				}
				else
				{
					setCursor(Qt::ArrowCursor);
					//update();
				}
			}
			else
			{
				UpdateCursorIcon(event->pos(), *m_pCurrentCroppingRect);
			}
			update();
		}
	}
}

void NImageObjectMarkerWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_matSrc.empty())
		return;
	if (event->button() == Qt::LeftButton)
	{
		if (m_eAnnotationMode == Drawing)
		{

			EndCreateBBox();

		}
		else
		{
			// 			if (m_eAnnotationMode != NImageObjectMarkerWidget::Undefined)
			// 			{
			emit postRectChanged(RectIndex(m_pCurrentCroppingRect), *m_pCurrentCroppingRect);
			//}
			m_eAnnotationMode = Undefined;
		}
	}
}


void NImageObjectMarkerWidget::keyPressEvent(QKeyEvent *event)
{
	if (m_matSrc.empty())
		return;
	if (event->key() == Qt::Key_Escape)
	{
		m_pCurrentCroppingRect = NULL;
		update();
	}
	//return NImageDisplayWidget::keyPressEvent(event);
}

void NImageObjectMarkerWidget::keyReleaseEvent(QKeyEvent *event)
{
	if (m_matSrc.empty())
		return;
	//return NImageDisplayWidget::keyReleaseEvent(event);
}

void NImageObjectMarkerWidget::DrawCroppingRect(QPainter* painter, const QRect& rectCropping, const QColor& colorCroppingRectBorder, const QColor& colorCroppingBackground)
{
	QRect rect = RectifiedRect(rectCropping);
	//painter->setPen(colorCroppingBackground);
	// 	QRect rectTopLeft(QPoint(0, 0), QPoint(rect.left(), rect.bottom() - 1));
	// 	QRect rectBottomLeft(QPoint(0, rect.bottom()), QPoint(rect.right() - 1, this->geometry().bottom()));
	// 	QRect rectBottomRight(QPoint(rect.right(), rect.top() + 1), QPoint(this->geometry().bottomRight()));
	// 	QRect rectTopRight(QPoint(rect.x() + 1, 0), QPoint(this->geometry().right(), rect.y()));
	// 	if (rectTopLeft.isValid())
	// 	{
	// 		painter->fillRect(rectTopLeft, colorCroppingBackground);
	// 	}
	// 	if (rectBottomLeft.isValid())
	// 	{
	// 		painter->fillRect(rectBottomLeft, colorCroppingBackground);
	// 	}
	// 	if (rectBottomRight.isValid())
	// 	{
	// 		painter->fillRect(rectBottomRight, colorCroppingBackground);
	// 	}
	// 	if (rectTopRight.isValid())
	// 	{
	// 		painter->fillRect(rectTopRight, colorCroppingBackground);
	// 	}
	// 矫正矩形框 ， 从原始的图像(0, 0) 坐标开始矫正成现在放大之后，平移之后的
	// 首先平移到现在的图像的左上角，然后再放大
	if (!rect.isEmpty())
	{
		QPen pen = QPen(m_colorCroppingRectBorder, 2, Qt::DashDotLine, Qt::SquareCap);
		painter->setPen(pen);
		painter->drawRect(rect);
	}
}


void NImageObjectMarkerWidget::DrawCroppingRect(QPainter* painter, const QRect& rectCropping)
{

	DrawCroppingRect(painter, rectCropping, m_colorCroppingRectBorder, m_colorCroppingBackground);

}

void NImageObjectMarkerWidget::DrawRect(QPainter* painter, const QRect& rect, const QPen& pen)
{
	if (painter)
	{
		QRect rectMapped = RectifiedRect(rect);
		painter->setPen(pen);
		painter->drawRect(rectMapped);
	}
}

void NImageObjectMarkerWidget::UpdateCursorIcon(const QPointF& _mousePosition)
{
	QCursor cursorIcon;
	QRectF rect = RectifiedRect(*m_pCurrentCroppingRect);
	switch (CursorPosition(rect, _mousePosition))
	{
	case CursorPositionTopRight:
	case CursorPositionBottomLeft:
		cursorIcon = QCursor(Qt::SizeBDiagCursor);
		break;
	case CursorPositionTopLeft:
	case CursorPositionBottomRight:
		cursorIcon = QCursor(Qt::SizeFDiagCursor);
		break;
	case CursorPositionTop:
	case CursorPositionBottom:
		cursorIcon = QCursor(Qt::SizeVerCursor);
		break;
	case CursorPositionLeft:
	case CursorPositionRight:
		cursorIcon = QCursor(Qt::SizeHorCursor);
		break;
	case CursorPositionMiddle:
		cursorIcon = m_bIsCropPressed ?
			QCursor(Qt::ClosedHandCursor) :
			QCursor(Qt::OpenHandCursor);
		break;
	case CursorPositionUndefined:
	default:
		cursorIcon = QCursor(Qt::ArrowCursor);
		break;
	}
	//
	this->setCursor(cursorIcon);
}

void NImageObjectMarkerWidget::UpdateCursorIcon(const QPointF& _mousePosition, const QRect& orignalRect)
{
	QCursor cursorIcon;
	QRectF rect = RectifiedRect(orignalRect);
	switch (CursorPosition(rect, _mousePosition))
	{
	case CursorPositionTopRight:
	case CursorPositionBottomLeft:
		cursorIcon = QCursor(Qt::SizeBDiagCursor);
		break;
	case CursorPositionTopLeft:
	case CursorPositionBottomRight:
		cursorIcon = QCursor(Qt::SizeFDiagCursor);
		break;
	case CursorPositionTop:
	case CursorPositionBottom:
		cursorIcon = QCursor(Qt::SizeVerCursor);
		break;
	case CursorPositionLeft:
	case CursorPositionRight:
		cursorIcon = QCursor(Qt::SizeHorCursor);
		break;
	case CursorPositionMiddle:
		cursorIcon = m_bIsCropPressed ?
			QCursor(Qt::ClosedHandCursor) :
			QCursor(Qt::OpenHandCursor);
		break;
	case CursorPositionUndefined:
	default:
		cursorIcon = QCursor(Qt::ArrowCursor);
		break;
	}
	//
	this->setCursor(cursorIcon);
}

QRect NImageObjectMarkerWidget::RectifiedRect(const QRect& rect)
{
	return QRect(rect.x() * m_fCurrentScale + m_nCurrentX, rect.y() * m_fCurrentScale + m_nCurrentY, rect.width() * m_fCurrentScale, rect.height() * m_fCurrentScale);
}



QRect NImageObjectMarkerWidget::MapToOrignalImage(const QRect& rectTransformed)
{
	return QRect((rectTransformed.x() - m_nCurrentX)* 1.0f / m_fCurrentScale, (rectTransformed.y() - m_nCurrentY) * 1.0f / m_fCurrentScale, rectTransformed.width() * 1.0f / m_fCurrentScale, rectTransformed.height() * 1.0f / m_fCurrentScale);
}

QPoint NImageObjectMarkerWidget::MapToOrignalImage(const QPoint& point)
{
	return QPoint((point.x() - m_nCurrentX) / m_fCurrentScale, (point.y() - m_nCurrentY) / m_fCurrentScale);
}

QPoint NImageObjectMarkerWidget::MatFromOrignalImage(const QPoint& point)
{
	return QPoint(point.x() * m_fCurrentScale + m_nCurrentX, point.y() * m_fCurrentScale + m_nCurrentY);
}

void NImageObjectMarkerWidget::RestrictRect(QRect& rect)
{
	if (rect.x() < 0)
	{
		rect.setX(0);
	}
	// 	if (rect.x() > m_matSrc.cols - 1)
	// 	{
	// 		rect.setX(m_matSrc.cols - 1);
	// 	}
	if (rect.right() > m_matSrc.cols - 1)
	{
		rect.setRight(m_matSrc.cols - 1);
	}
	// 	if (rect.right() < 0)
	// 	{
	// 		rect.setRight(0);
	// 	}
	if (rect.y() < 0)
	{
		rect.setY(0);
	}
	// 	if (rect.y() > m_matSrc.rows - 1)
	// 	{
	// 		rect.setY(m_matSrc.rows - 1);
	// 	}
	if (rect.bottom() > m_matSrc.rows - 1)
	{
		rect.setBottom(m_matSrc.rows - 1);
	}
	// 	if (rect.bottom() < 0)
	// 	{
	// 		rect.setBottom(0);
	// 	}
}

bool NImageObjectMarkerWidget::RestrictRect(QRect* pRect)
{
	bool bModified = false;
	if (pRect)
	{
		int nWidth = pRect->width();
		int nHeight = pRect->height();
		int nNewLeft = pRect->x();
		int nNewRight = pRect->right();
		int nNewTop = pRect->y();
		int nNewBottom = pRect->bottom();
		if (pRect->x() < 0)
		{
 			pRect->setX(0);
 			pRect->setWidth(nWidth);
			//nNewLeft = 0;
			bModified = true;
		}
		if (pRect->right() > m_matSrc.cols - 1)
		{
			
			pRect->setX(m_matSrc.cols - 1 - nWidth);
 			pRect->setWidth(nWidth);
			//nNewRight = m_matSrc.cols - 1;
			bModified = true;
		}
		if (pRect->y() < 0)
		{
 			pRect->setY(0);
 			pRect->setHeight(nHeight);
			//nNewTop = 0;
			bModified = true;
		}
		if (pRect->bottom() > m_matSrc.rows - 1)
		{
 			pRect->setY(m_matSrc.rows - 1 - nHeight);
 			pRect->setHeight(nHeight);
			//nNewBottom = m_matSrc.rows - 1;
			bModified = true;
		}
		//*pRect = QRect(QPoint(nNewLeft, nNewTop), QPoint(nNewRight, nNewBottom));
	}
	return bModified;
}

void NImageObjectMarkerWidget::RestrictResizingRect(ECursorPosition cursorPosition, QRect& rect)
{
	int nIndient = 20;
	switch (cursorPosition)
	{
	case CursorPositionUndefined:
		break;
	case CursorPositionMiddle:
		break;
	case CursorPositionTop:
	{
		if (rect.y() + nIndient > rect.bottom())
		{
			rect.setY(rect.bottom() - nIndient);
		}
	}
		break;
	case CursorPositionBottom:
	{
		if (rect.bottom() < rect.y() + nIndient)
		{
			rect.setBottom(rect.y() + nIndient);
		}
	}
		break;
	case CursorPositionLeft:
	{
		if (rect.x() + nIndient > rect.right())
		{
			rect.setX(rect.right() - nIndient);
		}
	}
		break;
	case CursorPositionRight:
	{
		if (rect.right() < rect.x() + nIndient)
		{
			rect.setRight(rect.x() + nIndient);
		}
	}
		break;
	case CursorPositionTopLeft:
	{
		if (rect.y() + nIndient > rect.bottom())
		{
			rect.setY(rect.bottom() - nIndient);
		}
		if (rect.x() + nIndient > rect.right())
		{
			rect.setX(rect.right() - nIndient);
		}
	}
		break;
	case CursorPositionTopRight:
		if (rect.y() + nIndient > rect.bottom())
		{
			rect.setY(rect.bottom() - nIndient);
		}
		if (rect.right() < rect.x() + nIndient)
		{
			rect.setRight(rect.x() + nIndient);
		}
		break;
	case CursorPositionBottomLeft:
		if (rect.bottom() < rect.y() + nIndient)
		{
			rect.setBottom(rect.y() + nIndient);
		}
		if (rect.x() + nIndient > rect.right())
		{
			rect.setX(rect.right() - nIndient);
		}
		break;
	case CursorPositionBottomRight:
	{
		if (rect.bottom() < rect.y() + nIndient)
		{
			rect.setBottom(rect.y() + nIndient);
		}
		if (rect.right() < rect.x() + nIndient)
		{
			rect.setRight(rect.x() + nIndient);
		}
	}
		break;
	default:
		break;
	}

}

QRect* NImageObjectMarkerWidget::RectAt(const QPoint& mousePos)
{
	int nIndient = 2;
	QPoint originPos = MapToOrignalImage(mousePos);
	if (m_pCurrentCroppingRect != NULL)
	{
		if (m_pCurrentCroppingRect->adjusted(-nIndient, -nIndient, nIndient, nIndient).contains(originPos))
		{
			return m_pCurrentCroppingRect;
		}
		else
		{
			for (int nIndex = m_listRect.size() - 1; nIndex >= 0; --nIndex)
			{
				if (m_listRect[nIndex]->adjusted(-nIndient, -nIndient, nIndient, nIndient).contains(originPos))
				{
					return m_listRect[nIndex];
				}
			}
			return NULL;
		}

	}
	else
	{
		for (int nIndex = 0; nIndex < m_listRect.size(); ++nIndex)
		{
			if (m_listRect[nIndex]->adjusted(-nIndient, -nIndient, nIndient, nIndient).contains(originPos))
			{
				return m_listRect[nIndex];
			}
		}
		return NULL;
	}

}

int NImageObjectMarkerWidget::RectIndex(const QRect* pRect)
{
	QList<QRect*>::iterator iter = qFind(m_listRect.begin(), m_listRect.end(), pRect);
	if (iter != m_listRect.end())
	{
		return iter - m_listRect.begin();
	}
	return -1;
}

void NImageObjectMarkerWidget::SetupUI()
{
	this->setMouseTracking(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
	this->setFocus(Qt::ShortcutFocusReason);
}

void NImageObjectMarkerWidget::SetupConnect()
{

}

ECursorPosition NImageObjectMarkerWidget::CursorPosition(const QRectF& _cropRect, const QPointF& _mousePosition)
{
	ECursorPosition cursorPosition = CursorPositionUndefined;
	if (_cropRect.contains(_mousePosition))
	{

		if (isPointNearSide(_cropRect.top(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.left(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionTopLeft;
		}
		else if (isPointNearSide(_cropRect.bottom(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.left(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionBottomLeft;
		}
		else if (isPointNearSide(_cropRect.top(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.right(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionTopRight;
		}
		else if (isPointNearSide(_cropRect.bottom(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.right(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionBottomRight;
		}
		else if (isPointNearSide(_cropRect.left(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionLeft;
		}
		else if (isPointNearSide(_cropRect.right(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionRight;
		}
		else if (isPointNearSide(_cropRect.top(), _mousePosition.y()))
		{
			cursorPosition = CursorPositionTop;
		}
		else if (isPointNearSide(_cropRect.bottom(), _mousePosition.y()))
		{
			cursorPosition = CursorPositionBottom;
		}
		else {
			cursorPosition = CursorPositionMiddle;
		}
	}
	return cursorPosition;
}

const QRectF NImageObjectMarkerWidget::CalculateGeometry(const QRectF& _sourceGeometry, const ECursorPosition _cursorPosition, const QPointF& _mouseDelta)
{
	QRectF resultGeometry = _sourceGeometry;
	int nMinLength = 10;
	switch (_cursorPosition)
	{
	case CursorPositionTopLeft:
		resultGeometry.setLeft(qMax((float)m_nCurrentX, (float)(_sourceGeometry.left() + _mouseDelta.x())));
		resultGeometry.setTop(qMax((float)m_nCurrentY, (float)(_sourceGeometry.top() + _mouseDelta.y())));
		// 		if (resultGeometry.x() < m_nCurX)
		// 			resultGeometry.setX(m_nCurX);
		// 		if (resultGeometry.y() < m_nCurY)
		// 			resultGeometry.setY(m_nCurY);
		resultGeometry.setLeft(qMin(resultGeometry.left(), resultGeometry.right() - nMinLength));
		resultGeometry.setTop(qMin(resultGeometry.top(), resultGeometry.bottom() - nMinLength));
		break;
	case CursorPositionTopRight:
		resultGeometry.setTop(qMax((float)m_nCurrentY, (float)(_sourceGeometry.top() + _mouseDelta.y())));
		resultGeometry.setRight(qMin((float)(m_nCurrentX + m_nCurrentWidth), (float)(_sourceGeometry.right() + _mouseDelta.x())));
		// 		if (resultGeometry.x() + resultGeometry.width() > m_nCurX + m_nCurWidth)
		// 			resultGeometry.setWidth(m_nCurX + m_nCurWidth - resultGeometry.x());
		// 		if (resultGeometry.y() + resultGeometry.height() > m_nCurY + m_nCurHeight)
		// 			resultGeometry.setHeight(m_nCurY + m_nCurHeight - resultGeometry.y());
		resultGeometry.setTop(qMin(resultGeometry.top(), resultGeometry.bottom() - nMinLength));
		resultGeometry.setRight(qMax(resultGeometry.left() + nMinLength, resultGeometry.right()));
		break;
	case CursorPositionBottomLeft:
		resultGeometry.setBottom(qMin((float)(m_nCurrentY + m_nCurrentHeight), (float)(_sourceGeometry.bottom() + _mouseDelta.y())));
		resultGeometry.setLeft(qMax((float)m_nCurrentX, (float)(_sourceGeometry.left() + _mouseDelta.x())));

		resultGeometry.setBottom(qMax(resultGeometry.top() + nMinLength, resultGeometry.bottom()));
		resultGeometry.setLeft(qMin(resultGeometry.left(), resultGeometry.right() - nMinLength));
		break;
	case CursorPositionBottomRight:
		resultGeometry.setBottom(qMin((float)(m_nCurrentY + m_nCurrentHeight), (float)(_sourceGeometry.bottom() + _mouseDelta.y())));
		resultGeometry.setRight(qMin((float)(m_nCurrentX + m_nCurrentWidth), (float)(_sourceGeometry.right() + _mouseDelta.x())));

		resultGeometry.setRight(qMax(resultGeometry.left() + nMinLength, resultGeometry.right()));
		resultGeometry.setBottom(qMax(resultGeometry.top() + nMinLength, resultGeometry.bottom()));
		break;
	case CursorPositionTop:
		resultGeometry.setTop(qMax((float)m_nCurrentY, (float)(_sourceGeometry.top() + _mouseDelta.y())));
		resultGeometry.setTop(qMin(resultGeometry.top(), resultGeometry.bottom() - nMinLength));

		break;
	case CursorPositionBottom:
		resultGeometry.setBottom(qMin((float)(m_nCurrentY + m_nCurrentHeight), (float)(_sourceGeometry.bottom() + _mouseDelta.y())));
		resultGeometry.setBottom(qMax(resultGeometry.top() + nMinLength, resultGeometry.bottom()));

		break;
	case CursorPositionLeft:
		resultGeometry.setLeft(qMax((float)m_nCurrentX, (float)(_sourceGeometry.left() + _mouseDelta.x())));
		resultGeometry.setLeft(qMin(resultGeometry.left(), resultGeometry.right() - nMinLength));

		break;
	case CursorPositionRight:
		resultGeometry.setRight(qMin((float)(m_nCurrentX + m_nCurrentWidth), (float)(_sourceGeometry.right() + _mouseDelta.x())));
		resultGeometry.setRight(qMax(resultGeometry.left() + nMinLength, resultGeometry.right()));

		break;
	default:
		break;
	}
	if ((resultGeometry.left() >= resultGeometry.right()) ||
		(resultGeometry.top() >= resultGeometry.bottom()))
	{
		resultGeometry = QRectF(resultGeometry.left(), resultGeometry.top(), 10, 10);
	}

	return resultGeometry;
}

bool NImageObjectMarkerWidget::IsPointInRect(const QPoint& pt, const QRect& rect)
{
	return rect.contains(pt);
}

