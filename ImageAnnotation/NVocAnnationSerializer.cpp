#include "NVocAnnationSerializer.h"
#include <QFile>

NVocAnnationSerializer::NVocAnnationSerializer(QObject *parent)
	: QObject(parent)
{

}

NVocAnnationSerializer::~NVocAnnationSerializer()
{

}

bool NVocAnnationSerializer::WriteToFile(const QString& strFilePath, const SImageAnnotationInfo& imageAnnotationInfo)
{
	QFile file(strFilePath);
	if (file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QXmlStreamWriter xmlStreamWriter(&file);
		xmlStreamWriter.setAutoFormatting(true);
		xmlStreamWriter.setAutoFormattingIndent(-1);
		xmlStreamWriter.writeStartDocument();
		xmlStreamWriter.writeStartElement(QStringLiteral("annotation"));
		xmlStreamWriter.writeTextElement(QStringLiteral("folder"), imageAnnotationInfo.m_strImageFileFolder);
		xmlStreamWriter.writeTextElement(QStringLiteral("filename"), imageAnnotationInfo.m_strImageFileName);

		xmlStreamWriter.writeStartElement(QStringLiteral("size"));
		xmlStreamWriter.writeTextElement(QStringLiteral("width"), QStringLiteral("%1").arg(imageAnnotationInfo.m_imageSize.m_nWidth));
		xmlStreamWriter.writeTextElement(QStringLiteral("height"), QStringLiteral("%1").arg(imageAnnotationInfo.m_imageSize.m_nHeight));
		xmlStreamWriter.writeTextElement(QStringLiteral("depth"), QStringLiteral("%1").arg(imageAnnotationInfo.m_imageSize.m_nDepth));
		// size
		xmlStreamWriter.writeEndElement();
		xmlStreamWriter.writeTextElement(QStringLiteral("segmented"), QStringLiteral("%1").arg(0));

		const QList<SAnnotationRect>& listAnnotationRect = imageAnnotationInfo.m_listAnnotationRects;
		for (int nIndex = 0; nIndex < listAnnotationRect.size(); ++nIndex)
		{
			const QRect& rect = listAnnotationRect[nIndex].m_rectOfObject;
			xmlStreamWriter.writeStartElement(QStringLiteral("object"));
			xmlStreamWriter.writeTextElement(QStringLiteral("name"), listAnnotationRect[nIndex].m_strObjectClassName);
			xmlStreamWriter.writeTextElement(QStringLiteral("pose"), QStringLiteral("Unspecified"));
			xmlStreamWriter.writeTextElement(QStringLiteral("truncated"), QStringLiteral("%1").arg(0));
			xmlStreamWriter.writeTextElement(QStringLiteral("difficult"), QStringLiteral("%1").arg(listAnnotationRect[nIndex].m_bDifficult));
			xmlStreamWriter.writeStartElement(QStringLiteral("bndbox"));
			xmlStreamWriter.writeTextElement(QStringLiteral("xmin"), QStringLiteral("%1").arg(rect.left()));
			xmlStreamWriter.writeTextElement(QStringLiteral("ymin"), QStringLiteral("%1").arg(rect.top()));
			xmlStreamWriter.writeTextElement(QStringLiteral("xmax"), QStringLiteral("%1").arg(rect.right()));
			xmlStreamWriter.writeTextElement(QStringLiteral("ymax"), QStringLiteral("%1").arg(rect.bottom()));
			// bndbox
			xmlStreamWriter.writeEndElement();
			// object
			xmlStreamWriter.writeEndElement();
		}
		//annotation
		xmlStreamWriter.writeEndElement();
		xmlStreamWriter.writeEndDocument();
		file.close();
		return true;
	}
	file.close();
	return false;
}

bool NVocAnnationSerializer::ReadFromFile(const QString& strFilePath, SImageAnnotationInfo& imageAnnotationInfo)
{
 	imageAnnotationInfo = SImageAnnotationInfo();
 	QFile file(strFilePath);
 	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
 	{
 		QXmlStreamReader xmlStreamReader(&file);
 		while (!xmlStreamReader.atEnd())
 		{
 			if (xmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
 			{
 				if (xmlStreamReader.name() == QStringLiteral("annotation"))
 					ReadImageAnnotationInfo(xmlStreamReader, imageAnnotationInfo);
 			}
 			if (xmlStreamReader.isEndElement())
 			{
 				if (xmlStreamReader.name() == QStringLiteral("annotation"))
 				{
 					break;
 				}
 			}
 			xmlStreamReader.readNextStartElement();
 		}
		file.close();
 		return true;
 
 	}
 	file.close();
 	return false;
}

void NVocAnnationSerializer::ReadImageAnnotationInfo(QXmlStreamReader& reader, SImageAnnotationInfo& imageAnnotationInfo)
{
	while (!reader.atEnd())
	{
		if (reader.tokenType() == QXmlStreamReader::StartElement)
		{
			if (reader.name() == QStringLiteral("folder"))
			{
				imageAnnotationInfo.m_strImageFileFolder = reader.readElementText(QXmlStreamReader::SkipChildElements);
			}
			else if (reader.name() == QStringLiteral("filename"))
			{
				imageAnnotationInfo.m_strImageFileName = reader.readElementText(QXmlStreamReader::SkipChildElements);
			}
			else if (reader.name() == QStringLiteral("source"))
			{
				reader.skipCurrentElement();
			}
			else if (reader.name() == QStringLiteral("size"))
			{
				ReadSize(reader, imageAnnotationInfo);
			}
			else if (reader.name() == QStringLiteral("object"))
			{
				ReadObject(reader, imageAnnotationInfo);
			}

		}
		if (reader.isEndElement() && reader.name() == QStringLiteral("annotation"))
		{
			break;
		}
		reader.readNextStartElement();
	}
}

void NVocAnnationSerializer::ReadSize(QXmlStreamReader& reader, SImageAnnotationInfo& imageAnnotationInfo)
{
	while (!reader.atEnd())
	{

		if (reader.isStartElement())
		{
			if (reader.name() == QStringLiteral("width"))
			{
				imageAnnotationInfo.m_imageSize.m_nWidth = reader.readElementText(QXmlStreamReader::SkipChildElements).toInt();
			}
			else if (reader.name() == QStringLiteral("height"))
			{
				imageAnnotationInfo.m_imageSize.m_nHeight = reader.readElementText(QXmlStreamReader::SkipChildElements).toInt();
			}
			else if (reader.name() == QStringLiteral("depth"))
			{
				imageAnnotationInfo.m_imageSize.m_nDepth = reader.readElementText(QXmlStreamReader::SkipChildElements).toInt();
			}
		}
		if (reader.isEndElement())
		{
			if (reader.name() == QStringLiteral("size"))
			{
				break;
			}
		}
		reader.readNextStartElement();
	}
}

void NVocAnnationSerializer::ReadObject(QXmlStreamReader& reader, SImageAnnotationInfo& imageAnnotationInfo)
{
	while (!reader.atEnd())
	{
		if (reader.isStartElement() && reader.name() == QStringLiteral("object"))
		{
			QString strName;
			SAnnotationRect annotationRect;
			reader.readNext();
			reader.readNext();
			strName = reader.name().toString();
			if (reader.name() == QStringLiteral("name"))
			{
				annotationRect.m_strObjectClassName = reader.readElementText(QXmlStreamReader::SkipChildElements);
			}
			reader.readNextStartElement();
			if (reader.name() == QStringLiteral("pose"))
			{
				reader.skipCurrentElement();
			}
			reader.readNextStartElement();
			 strName = reader.name().toString();
			if (reader.name() == QStringLiteral("truncated"))
			{
				reader.skipCurrentElement();
			}
			reader.readNextStartElement();
			strName = reader.name().toString();
			if (reader.name() == QStringLiteral("difficult"))
			{
				annotationRect.m_bDifficult = reader.readElementText(QXmlStreamReader::SkipChildElements).toInt();
			}
			reader.readNextStartElement();
			if (reader.name() == QStringLiteral("bndbox"))
			{
				reader.readNext();
				reader.readNext();
				int nLeft = 0, nTop = 0, nRight = 0, nBottom = 0;
				if (reader.name() == QStringLiteral("xmin"))
				{
					nLeft = reader.readElementText(QXmlStreamReader::SkipChildElements).toInt();
				}
				reader.readNext();
				reader.readNext();
				if (reader.name() == QStringLiteral("ymin"))
				{
					nTop = reader.readElementText(QXmlStreamReader::SkipChildElements).toInt();
				}
				reader.readNext();
				reader.readNext();
				if (reader.name() == QStringLiteral("xmax"))
				{
					nRight = reader.readElementText(QXmlStreamReader::SkipChildElements).toInt();
				}
				reader.readNext();
				reader.readNext();
				if (reader.name() == QStringLiteral("ymax"))
				{
					nBottom = reader.readElementText(QXmlStreamReader::SkipChildElements).toInt();
				}
				annotationRect.m_rectOfObject = QRect(QPoint(nLeft, nTop), QPoint(nRight, nBottom));
				imageAnnotationInfo.m_listAnnotationRects.push_back(annotationRect);
				reader.readNext();
				QString strName1 = reader.name().toString();
				reader.readNext();
				QString strName2 = reader.name().toString();

				reader.readNext();
				//QString strName3 = reader.name().toString();
				reader.readNext();
				//QString strName4 = reader.name().toString();
				reader.readNext();
				//QString strName5 = reader.name().toString();
				reader.readNext();
				//strName = reader.name().toString();
			}
		}
		if (!reader.isStartElement() && reader.name() != QStringLiteral("object"))
		{
			break;
		}
	}
}
