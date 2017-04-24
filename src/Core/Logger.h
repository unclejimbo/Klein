#pragma once

#include <QFile>

// Use macros in order to expand to the correct file context
#define KLEIN_LOG_DEBUG(msg) qDebug(qUtf8Printable(msg))
#define KLEIN_LOG_INFO(msg) qInfo(qUtf8Printable(msg))
#define KLEIN_LOG_WARNING(msg) qWarning(qUtf8Printable(msg))
#define KLEIN_LOG_CRITICAL(msg) qCritical(qUtf8Printable(msg))
#define KLEIN_LOG_FATAL(msg) qFatal(qUtf8Printable(msg))

class Logger
{
public:
	static void initialize();
	static void cleanup();
	static void log(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
	static QFile _logFile;
};
