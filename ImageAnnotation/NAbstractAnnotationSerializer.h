#pragma once

#include <QObject>
#include "NCommon.h"

class NAbstractAnnotationSerializer : virtual public QObject
{
	Q_OBJECT

public:
	NAbstractAnnotationSerializer(QObject *parent = NULL);
	virtual ~NAbstractAnnotationSerializer();

	bool Write(const QString& strFilePath, const SImageAnnotationInfo& imageAnnotationInfo);
	bool Read(const QString& strFilePath,  SImageAnnotationInfo& imageAnnotation);

protected:
	virtual bool WriteToFile(const QString& strFilePath, const SImageAnnotationInfo& imageAnnotationInfo) = 0;
	virtual bool ReadFromFile(const QString& strFilePath, SImageAnnotationInfo& imageAnnotationInfo) = 0;
};
