#include "NImageCropper.h"
#include <QPainter>
#include <QApplication>
#include <QClipboard>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QMouseEvent>
#include <QtDebug>

NImageCropper::NImageCropper(QWidget *parent)
	: NImageDisplayWidget(parent), m_colorCroppingBackground(Qt::darkGray), m_colorCroppingRectBorder(Qt::magenta),
	m_bIsCropPressed(false), m_bIsDrawing(false)
{
	m_colorCroppingBackground.setAlphaF(0.5f);
	SetupUI();
	SetupConnect();
}

NImageCropper::~NImageCropper()
{

}


void NImageCropper::SetCroppingRectBorderColor(const QColor& color)
{
	m_colorCroppingRectBorder = color;
}

void NImageCropper::SetCroppingBackgroundColor(const QColor& color)
{
	m_colorCroppingBackground = color;
	m_colorCroppingBackground.setAlphaF(0.3f);
}

void NImageCropper::paintEvent(QPaintEvent *event)
{
	NImageDisplayWidget::paintEvent(event);
	QPainter painter(this);
	if (!m_matSrc.empty())
	{
		if (m_bIsCropPressed)
		{
			DrawCroppingRect(&painter, m_curCroppingRect);
		}
	}
}

void NImageCropper::mousePressEvent(QMouseEvent *event)
{
	setFocus();
	if (m_matSrc.empty())
		return;

	if (m_bIsCropPressed)
	{
		if (event->button() == Qt::LeftButton)
		{
			if (!m_curCroppingRect.isValid() && (event->modifiers() | Qt::ControlModifier))
			{
				if (QRect(m_nCurrentX, m_nCurrentY, m_nCurrentWidth, m_nCurrentHeight).contains(event->pos()))
				{
					m_startCroppingPoint = event->pos();
					qDebug() << m_startCroppingPoint;
					m_bIsDrawing = true;
				}
				else
				{
					m_startCroppingPoint = QPoint();
					m_bIsDrawing = false;
					NImageDisplayWidget::mousePressEvent(event);
				}
			}
			else
			{
				m_eCursorPosition = CursorPosition(RectifiedRect(m_curCroppingRect), event->pos());
				if (m_eCursorPosition != CursorPositionUndefined)
				{
					if (m_eCursorPosition == CursorPositionMiddle)
					{
						m_bMove = true;
					}
					else
					{
						m_bMove = false;
					}
					m_startCroppingPoint = event->pos();
				}
				else
				{
					NImageDisplayWidget::mousePressEvent(event);
				}
			}
		}
	}
	else
	{
		NImageDisplayWidget::mousePressEvent(event);
	}
}

void NImageCropper::mouseMoveEvent(QMouseEvent *event)
{
	//#pragma region MyRegion
	//	QPointF mousePos = event->pos();
	//	if (m_bIsCropPressed)
	//	{
	//		if (m_bIsCropPressed == false)
	//		{
	//			m_eCursorPosition = CursorPosition(m_curCroppingRect, mousePos);
	//			UpdateCursorIcon(mousePos);
	//		}
	//		else if (m_eCursorPosition != ECursorPosition::CursorPositionUndefined && (event->buttons() & Qt::LeftButton))
	//		{
	//			QPointF mouseDelta(mousePos.x() - m_startCroppingPoint.x(), mousePos.y() - m_startCroppingPoint.y());
	//
	//			if (m_eCursorPosition != CursorPositionMiddle)
	//			{
	//				m_curCroppingRect = CalculateGeometry(m_curCroppingRect, m_eCursorPosition, mouseDelta);
	//				if (m_curCroppingRect.isNull())
	//				{
	//					m_bIsCropping = false;
	//					m_vecRect.pop_back();
	//					m_rectSelected = NULL;
	//				}
	//				else
	//				{
	//					*m_rectSelected = MapToOrignalRect(m_curCroppingRect);
	//				}
	//			}
	//			else // 移动中间
	//			{
	//				// 				QRectF rectTmp = m_curCroppingRect;
	//				// 				rectTmp.setX(rectTmp.x() + mouseDelta.x());
	//				// 				rectTmp.setY(rectTmp.y() + mouseDelta.y());
	//				// 				m_curCroppingRect = rectTmp;
	//				// 				if (rectTmp.x() + mouseDelta.x() < m_nCurX)
	//				// 					m_curCroppingRect.setX(m_nCurX);
	//				// 				else if (rectTmp.y() + mouseDelta.y() < m_nCurY)
	//				// 					m_curCroppingRect.setY(m_nCurY);
	//				// 				else if (rectTmp.x() + mouseDelta.x() + rectTmp.width() > m_nCurWidth + m_nCurX)
	//				// 					m_curCroppingRect.setRight(m_nCurX + m_nCurWidth);
	//				// 				else if (rectTmp.y() + mouseDelta.y() + rectTmp.height() > m_nCurY + m_nCurHeight)
	//				// 					m_curCroppingRect.setBottom(m_nCurY + m_nCurHeight);
	//				// 				m_curCroppingRect.setLeft(qMax((float)m_nCurX, (float)(m_curCroppingRect.x() + mouseDelta.x())));
	//				// 				m_curCroppingRect.setTop(qMax((float)m_nCurY, (float)(m_curCroppingRect.y() + mouseDelta.y())));
	//				// 				m_curCroppingRect.setRight(qMin((float)(m_nCurX + m_nCurWidth), (float)(m_curCroppingRect.x() + mouseDelta.x() + m_curCroppingRect.width())));
	//				// 				m_curCroppingRect.setBottom(qMin((float)(m_nCurY + m_nCurHeight), (float)(m_curCroppingRect.y() + mouseDelta.y() + m_curCroppingRect.height())));
	//				QRectF rectTmp = m_curCroppingRect;
	//				m_curCroppingRect.moveTo(m_curCroppingRect.topLeft() + mouseDelta);
	//				if (m_curCroppingRect.x() < m_nCurX)
	//				{
	//					m_curCroppingRect.setX(m_nCurX);
	//					m_curCroppingRect.setWidth(rectTmp.width());
	//				}
	//				else if (m_curCroppingRect.right() > m_nCurX + m_nCurWidth)
	//				{
	//					m_curCroppingRect.setX(m_nCurX + m_nCurWidth - rectTmp.width());
	//					m_curCroppingRect.setWidth(rectTmp.width());
	//				}
	//				if (m_curCroppingRect.y() < m_nCurY)
	//				{
	//					m_curCroppingRect.setY(m_nCurY);
	//					m_curCroppingRect.setHeight(rectTmp.height());
	//				}
	//				else if (m_curCroppingRect.bottom() > m_nCurY + m_nCurHeight)
	//				{
	//					m_curCroppingRect.setBottom(m_nCurY + m_nCurHeight);
	//					m_curCroppingRect.setTop(m_curCroppingRect.bottom() - rectTmp.height());
	//				}
	//
	//				*m_rectSelected = MapToOrignalRect(m_curCroppingRect);
	//			}
	//			//UpdateCursorIcon(mousePos);
	//			m_startCroppingPoint = mousePos;
	//			update();
	//		}
	//		else if (m_eCursorPosition == CursorPositionUndefined && (event->buttons() & Qt::LeftButton))
	//		{
	//			NImageDisplayWidget::mouseMoveEvent(event);
	//		}
	//	}
	//	else
	//	{
	//		update();
	//	}
	//#pragma endregion
	if (m_matSrc.empty())
		return;
	QPoint mousePos = event->pos();
	if (m_bIsCropPressed)
	{
		// 如果正在剪裁，并且是左键按下， 就开始画图
		if (event->buttons() & Qt::LeftButton)
		{
			if (m_bIsDrawing)
			{
				QRect rectTmp = QRect(m_startCroppingPoint, event->pos());
				rectTmp.setBottom(qMin(rectTmp.bottom(), m_nCurrentY + m_nCurrentHeight));
				rectTmp.setRight(qMin(rectTmp.right(), m_nCurrentX + m_nCurrentWidth));
				m_curCroppingRect = MapToOrignalImage(rectTmp.normalized());
				RestrictRect(m_curCroppingRect);
			}
			else
			{
				if (m_eCursorPosition != CursorPositionUndefined)
				{
					if (m_eCursorPosition != CursorPositionMiddle && !m_bMove)
					{
						// 矫正鼠标拖动边框漂移问题
						QPointF mouseDelta(mousePos.x() - m_startCroppingPoint.x(), mousePos.y() - m_startCroppingPoint.y());
						//QRectF rectTmp = CalculateGeometry(RectifiedRect(m_curCroppingRect), m_eCursorPosition, mouseDelta);
						//m_curCroppingRect = MapToOrignalImage(QRect(rectTmp.x(), rectTmp.y(), rectTmp.width(), rectTmp.height()));
						
						QRect rectTmp = m_curCroppingRect;
						QRect rectRectify = RectifiedRect(m_curCroppingRect);
						switch (m_eCursorPosition)
						{
						case CursorPositionTop:
							m_curCroppingRect = QRect(QPoint(rectTmp.x(), rectTmp.bottom() - (rectRectify.bottom() - event->pos().y()) / m_fCurrentScale), QPoint(rectTmp.right(), rectTmp.bottom()));
							break;
						case CursorPositionBottom:
							m_curCroppingRect = QRect(QPoint(rectTmp.x(), rectTmp.y()), QPoint(rectTmp.right(), rectTmp.y() + (event->pos().y() - rectRectify.y()) / m_fCurrentScale));
							break;
						case CursorPositionLeft:
							m_curCroppingRect = QRect(QPoint(rectTmp.right() - (rectRectify.right() - event->pos().x()) / m_fCurrentScale, rectTmp.y()), QPoint(rectTmp.right(), rectTmp.bottom()));
							break;
						case CursorPositionRight:
							m_curCroppingRect = QRect(QPoint(rectTmp.x(), rectTmp.y()), QPoint(rectTmp.x() + (event->pos().x() - rectRectify.x()) / m_fCurrentScale, rectTmp.bottom()));
							break;
						case CursorPositionTopLeft:
							m_curCroppingRect = QRect(QPoint(rectTmp.right() - (rectRectify.right() - event->pos().x()) / m_fCurrentScale, rectTmp.bottom() - (rectRectify.bottom() - event->pos().y()) / m_fCurrentScale), QPoint(rectTmp.right(), rectTmp.bottom()));
							break;
						case CursorPositionTopRight:
							m_curCroppingRect = QRect(QPoint(rectTmp.x(), rectTmp.bottom() - (rectRectify.bottom() - event->pos().y()) / m_fCurrentScale), QPoint(rectTmp.x() + (event->pos().x() - rectRectify.x()) / m_fCurrentScale, rectTmp.bottom()));
							break;
						case CursorPositionBottomLeft:
							m_curCroppingRect = QRect(QPoint(rectTmp.right() - (rectRectify.right() - event->pos().x()) / m_fCurrentScale, rectTmp.y()), QPoint(rectTmp.right(), rectTmp.y() + (event->pos().y() - rectRectify.y()) / m_fCurrentScale));
							break;
						case CursorPositionBottomRight:
							m_curCroppingRect = QRect(QPoint(rectTmp.x(), rectTmp.y()), QPoint(rectTmp.x() + (event->pos().x() - rectRectify.x()) / m_fCurrentScale, rectTmp.y() + (event->pos().y() - rectRectify.y()) / m_fCurrentScale));
							break;
						}
						//m_curCroppingRect = m_curCroppingRect.normalized();
						RestrictRect(m_curCroppingRect);
						if (m_curCroppingRect.width() > 10 && m_curCroppingRect.height() > 10)
						{
							m_startCroppingPoint = event->pos();
						}
					}
					else
					{
						// 矫正鼠标拖动漂移问题
						QRect rectTmp = m_curCroppingRect;
						QRect rectRectify = RectifiedRect(m_curCroppingRect);
						
						QPointF mouseDelta(mousePos.x() - m_startCroppingPoint.x(), mousePos.y() - m_startCroppingPoint.y());
						m_curCroppingRect.moveTopLeft(QPoint(cvRound(m_curCroppingRect.x() + mouseDelta.x() / m_fCurrentScale), cvRound(m_curCroppingRect.y() + mouseDelta.y() / m_fCurrentScale)));

						if (m_curCroppingRect.x() < 0)
						{
							m_curCroppingRect.setX(0);
							m_curCroppingRect.setWidth(rectTmp.width());
						}
						else if (m_curCroppingRect.right() > m_matSrc.cols - 1)
						{
							m_curCroppingRect.setLeft(m_matSrc.cols - 1 - rectTmp.width());
							m_curCroppingRect.setWidth(rectTmp.width());
						}
						if (m_curCroppingRect.y() < 0)
						{
							m_curCroppingRect.setY(0);
							m_curCroppingRect.setHeight(rectTmp.height());
						}
						else if (m_curCroppingRect.bottom() > m_matSrc.rows - 1)
						{
							m_curCroppingRect.setTop(m_matSrc.rows - 1 - rectTmp.height());
							m_curCroppingRect.setHeight(rectTmp.height());
						}
						
						m_startCroppingPoint = event->pos();
					}

				}
				else
				{
					NImageDisplayWidget::mouseMoveEvent(event);
				}
			}
			update();
		}
		else
		{
			UpdateCursorIcon(mousePos);
		}
	}
	else
	{
		m_eCursorPosition = CursorPosition(RectifiedRect(m_curCroppingRect), event->pos());
		NImageDisplayWidget::mouseMoveEvent(event);
	}
}

void NImageCropper::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_matSrc.empty())
		return;
	if (m_bIsCropPressed)
	{
		if (event->button() == Qt::LeftButton)
		{
			if (m_bIsDrawing)
			{
				if (m_curCroppingRect.width() < 10 && m_curCroppingRect.height() < 10)
				{
					m_curCroppingRect = QRect();
				}
				m_bIsDrawing = false;
			}
			else
			{
				if (m_curCroppingRect.width() < 10 && m_curCroppingRect.height() < 10)
				{
					m_curCroppingRect = QRect();
				}
				NImageDisplayWidget::mouseReleaseEvent(event);
			}
			UpdateCursorIcon(event->pos());
			update();
		}
		else if (event->button() == Qt::MiddleButton)
		{
			if (!m_curCroppingRect.isEmpty() && m_curCroppingRect.isValid())
			{
				emit postCroppedImage(m_matSrc(cv::Rect(m_curCroppingRect.x(), m_curCroppingRect.y(), m_curCroppingRect.width(), m_curCroppingRect.height())).clone());
			}
		}
	}
	else
	{
		NImageDisplayWidget::mouseReleaseEvent(event);
	}

}


void NImageCropper::keyPressEvent(QKeyEvent *event)
{
	if (m_bIsCropPressed)
	{
		if (m_matSrc.empty())
			return;
		if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
		{
			if (!m_curCroppingRect.isEmpty() && m_curCroppingRect.isValid())
			{
				emit postCroppedImage(m_matSrc(cv::Rect(m_curCroppingRect.x(), m_curCroppingRect.y(), m_curCroppingRect.width(), m_curCroppingRect.height())).clone());
			}
		}
	}
	else
	{
		NImageDisplayWidget::keyPressEvent(event);
	}
}

void NImageCropper::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Shift)
	{
		m_bIsCropPressed = !m_bIsCropPressed;
		emit postCroppingStatusChanged(m_bIsCropPressed);
		update();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		m_curCroppingRect = QRect();
		update();
	}
	else
	{
		return NImageDisplayWidget::keyReleaseEvent(event);
	}
}

void NImageCropper::DrawCroppingRect(QPainter* painter, const QRect& rectCropping, const QColor& colorCroppingRectBorder, const QColor& colorCroppingBackground)
{
	QRect rect = RectifiedRect(rectCropping);
	//painter->setPen(colorCroppingBackground);
	QRect rectTopLeft(QPoint(0, 0), QPoint(rect.left(), rect.bottom() - 1));
	QRect rectBottomLeft(QPoint(0, rect.bottom()), QPoint(rect.right() - 1, this->geometry().bottom()));
	QRect rectBottomRight(QPoint(rect.right(), rect.top() + 1), QPoint(this->geometry().bottomRight()));
	QRect rectTopRight(QPoint(rect.x() + 1, 0), QPoint(this->geometry().right(), rect.y()));
	if (rectTopLeft.isValid())
	{
		painter->fillRect(rectTopLeft, colorCroppingBackground);
	}
	if (rectBottomLeft.isValid())
	{
		painter->fillRect(rectBottomLeft, colorCroppingBackground);
	}
	if (rectBottomRight.isValid())
	{
		painter->fillRect(rectBottomRight, colorCroppingBackground);
	}
	if (rectTopRight.isValid())
	{
		painter->fillRect(rectTopRight, colorCroppingBackground);
	}
	// 矫正矩形框 ， 从原始的图像(0, 0) 坐标开始矫正成现在放大之后，平移之后的
	// 首先平移到现在的图像的左上角，然后再放大
	if (!rect.isEmpty())
	{
		QPen pen = QPen(m_colorCroppingRectBorder, 2, Qt::DashDotLine, Qt::SquareCap);
		painter->setPen(pen);
		painter->drawRect(rect);
	}
}


void NImageCropper::DrawCroppingRect(QPainter* painter, const QRect& rectCropping)
{

	DrawCroppingRect(painter, rectCropping, m_colorCroppingRectBorder, m_colorCroppingBackground);

}

void NImageCropper::UpdateCursorIcon(const QPointF& _mousePosition)
{
	QCursor cursorIcon;
	if (m_bIsCropPressed)
	{
		QRectF rect = RectifiedRect(m_curCroppingRect);
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
	}
	else
	{
		cursorIcon = QCursor(Qt::ArrowCursor);
	}
	//
	this->setCursor(cursorIcon);
}

QRect NImageCropper::RectifiedRect(const QRect& rect)
{
	return QRect(rect.x() * m_fCurrentScale + m_nCurrentX, rect.y() * m_fCurrentScale + m_nCurrentY, rect.width() * m_fCurrentScale, rect.height() * m_fCurrentScale);
}



QRect NImageCropper::MapToOrignalImage(const QRect& rectTransformed)
{
	return QRect((rectTransformed.x() - m_nCurrentX)* 1.0f / m_fCurrentScale, (rectTransformed.y() - m_nCurrentY) * 1.0f / m_fCurrentScale, rectTransformed.width() * 1.0f / m_fCurrentScale, rectTransformed.height() * 1.0f / m_fCurrentScale);
}

void NImageCropper::RestrictRect(QRect& rect)
{
	int nLeft = rect.x();
	int nTop = rect.y();
	int nRight = rect.x() + rect.width();
	int nBottom = rect.y() + rect.height();
	if (nLeft < 0)
	{
		nLeft = 0;
	}
	if (nRight > m_matSrc.cols - 1)
	{
		nRight = m_matSrc.cols - 1;
	}
	if (nTop < 0)
	{
		nTop = 0;
	}
	if (nBottom > m_matSrc.rows - 1)
	{
		nBottom = m_matSrc.rows - 1;
	}
	rect = QRect(QPoint(nLeft, nTop), QPoint(nRight, nBottom));
}

void NImageCropper::SetupUI()
{
	this->setMouseTracking(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
	this->setFocus(Qt::MouseFocusReason);
}

void NImageCropper::SetupConnect()
{

}

ECursorPosition NImageCropper::CursorPosition(const QRectF& _cropRect, const QPointF& _mousePosition)
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

const QRectF NImageCropper::CalculateGeometry(const QRectF& _sourceGeometry, const ECursorPosition _cursorPosition, const QPointF& _mouseDelta)
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

bool NImageCropper::IsPointInRect(const QPoint& pt, const QRect& rect)
{
	return rect.contains(pt);
}

