#include "NAbstractAnnotationSerializer.h"

NAbstractAnnotationSerializer::NAbstractAnnotationSerializer(QObject *parent)
	: QObject(parent)
{
}

NAbstractAnnotationSerializer::~NAbstractAnnotationSerializer()
{
}

bool NAbstractAnnotationSerializer::Write(const QString& strFilePath, const SImageAnnotationInfo& imageAnnotationInfo)
{
	return WriteToFile(strFilePath, imageAnnotationInfo);
}

bool NAbstractAnnotationSerializer::Read(const QString& strFilePath, SImageAnnotationInfo& imageAnnotation)
{
	return ReadFromFile(strFilePath, imageAnnotation);
}
