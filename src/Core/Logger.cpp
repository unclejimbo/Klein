#include "Core/Logger.h"
#include "Core/Common.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QDateTime>
#include <QTextStream>
#include <iostream>

#ifdef _WIN32
#pragma warning(pop)
#endif

void Logger::initialize()
{
	_logFile.open(QIODevice::WriteOnly | QIODevice::Text);
}

void Logger::cleanup()
{
	_logFile.close();
}

void Logger::log(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QString msgType;
	switch (type) {
	case QtDebugMsg:
		msgType = "Debug";
		break;
	case QtInfoMsg:
		msgType = "Info";
		break;
	case QtWarningMsg:
		msgType = "Warning";
		break;
	case QtCriticalMsg:
		msgType = "Critical";
		break;
	case QtFatalMsg:
		msgType = "Fatal";
		break;
	default:
		break;
	}
	std::cout << msg.toStdString() << std::endl;

	auto logMsg = QString("[%1][%2](%3) %4").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")).
		arg(QString("%1(%2)").arg(context.file).arg(context.line)).arg(msgType).arg(msg.toLocal8Bit().constData());
	QTextStream stream(&_logFile);
	stream << logMsg << endl;
	_logFile.flush();
}

QFile Logger::_logFile(KLEIN_LOG_PATH);
