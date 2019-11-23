#pragma once

#include <QObject>
#include "NAbstractAnnotationSerializer.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
class NVocAnnationSerializer : virtual public NAbstractAnnotationSerializer
{
	Q_OBJECT

public:
	NVocAnnationSerializer(QObject *parent = NULL);
	~NVocAnnationSerializer();

protected:
	virtual bool WriteToFile(const QString& strFilePath, const SImageAnnotationInfo& imageAnnotationInfo) override;

	virtual bool ReadFromFile(const QString& strFilePath, SImageAnnotationInfo& imageAnnotationInfo) override;

private:
	void ReadImageAnnotationInfo(QXmlStreamReader& reader, SImageAnnotationInfo& imageAnnotationInfo);
	void ReadSize(QXmlStreamReader& reader, SImageAnnotationInfo& imageAnnotationInfo);
	void ReadObject(QXmlStreamReader& reader, SImageAnnotationInfo& imageAnnotationInfo);

};
