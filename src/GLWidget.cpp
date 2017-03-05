#include "GLWidget.h"

#include "ArcballController.h"
#include "Logger.h"
#include "ResourceManager.h"
#include "Scene.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QColor>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFrameBufferObject>
#include <QOpenGLVertexArrayObject>
#include <QPainter>
#include <QVector3D>
#include <QVector2D>
#include <vector>

#ifdef _WIN32
#pragma warning(pop)
#endif

GLWidget::GLWidget(QWidget* parent)
	: QOpenGLWidget(parent)
{
}

GLWidget::~GLWidget() = default;

void GLWidget::bindScene(Scene* scene)
{
	_scene = scene;
	if (_scene != nullptr) {
		_cameraController = std::make_unique<ArcballController>(*_scene->camera(), this->width(), this->height());
	}
}

void GLWidget::readColorBuffer(QImage& img)
{
	this->makeCurrent();
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	format.setInternalTextureFormat(GL_RGBA);
	format.setMipmap(true);
	format.setSamples(4);
	format.setTextureTarget(GL_TEXTURE_2D);
	QOpenGLFramebufferObject offscreen(this->width(), this->height(), format);
	offscreen.bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	if (_scene != nullptr) {
		_scene->render(RenderPass::offscreen);
	}
	img = offscreen.toImage();

	QOpenGLFramebufferObject::bindDefault();
	this->doneCurrent();
}

void GLWidget::readDepthBuffer(std::vector<float>& depth)
{
	this->makeCurrent();

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	GLuint colorBuffer;
	glGenRenderbuffers(1, &colorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, this->width(), this->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width(), this->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, this->width(), this->height());

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	if (_scene != nullptr) {
		_scene->render(RenderPass::offscreen);
	}
	auto buf = new GLfloat[this->width() * this->height()];
	glReadPixels(0, 0, this->width(), this->height(), GL_DEPTH_COMPONENT, GL_FLOAT, buf);
	depth.reserve(this->width() * this->height());
	for (int i = 0; i < this->width() * this->height(); ++i) {
		depth.push_back(buf[i]);
	}
	delete[] buf;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->doneCurrent();
}

void GLWidget::renderToTexture(QImage& img)
{
	this->makeCurrent();
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	format.setInternalTextureFormat(GL_RGBA);
	format.setMipmap(true);
	format.setSamples(4);
	format.setTextureTarget(GL_TEXTURE_2D);
	QOpenGLFramebufferObject offscreen(this->width(), this->height(), format);
	offscreen.bind();

	auto bgColor = QColor(112, 128, 144);
	glClearColor(static_cast<float>(bgColor.redF()), static_cast<float>(bgColor.greenF()),
		static_cast<float>(bgColor.blueF()), static_cast<float>(bgColor.alphaF()));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	if (_scene != nullptr) {
		_scene->render(RenderPass::shading);
	}
	img = offscreen.toImage();

	QOpenGLFramebufferObject::bindDefault();
	this->doneCurrent();
}

void GLWidget::initializeGL()
{
	this->initializeOpenGLFunctions();
	glEnable(GL_DEPTH_TEST);

	_glDebugLogger = new QOpenGLDebugLogger(this);
	if (_glDebugLogger->initialize()) {
		connect(_glDebugLogger, &QOpenGLDebugLogger::messageLogged, this, &GLWidget::_logOpenGLMsg);
		_glDebugLogger->startLogging();
	}
	else {
		KLEIN_LOG_WARNING("OpenGL debug logger failed to initialize");
	}
}

void GLWidget::paintGL()
{
	auto bgColor = QColor(112, 128, 144);
	glClearColor(static_cast<float>(bgColor.redF()), static_cast<float>(bgColor.greenF()),
		static_cast<float>(bgColor.blueF()), static_cast<float>(bgColor.alphaF()));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (_scene != nullptr) {
		_scene->render(RenderPass::shading);
	}

	if (_scene != nullptr) {
		_paintAxis();
	}
}

void GLWidget::resizeGL(int w, int h)
{
	if (_scene != nullptr) {
		_scene->camera()->setAspect((w + 0.0f) / h);
		_cameraController->onResize(w, h);
	}
	this->update();
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
	_cameraController->keyPressEvent(event);
	this->update();
}

void GLWidget::keyReleaseEvent(QKeyEvent* event)
{
	_cameraController->keyReleaseEvent(event);
	this->update();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
	_cameraController->mouseMoveEvent(event);
	this->update();
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
	_cameraController->mousePressEvent(event);
	this->update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	_cameraController->mouseReleaseEvent(event);
	this->update();
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
	_cameraController->wheelEvent(event);
	this->update();
}

// TODO: 2d overlay with QPainter is expected to work in Qt5.9
void GLWidget::_paintAxis()
{
	auto xDir_w = _scene->camera()->view() * (QVector3D(1.0f, 0.0f, 0.0f) + _scene->camera()->focus());
	auto yDir_w = _scene->camera()->view() * (QVector3D(0.0f, 1.0f, 0.0f) + _scene->camera()->focus());
	auto zDir_w = _scene->camera()->view() * (QVector3D(0.0f, 0.0f, 1.0f) + _scene->camera()->focus());

	auto origin_s = QVector2D(50.0f, 50.0f);
	auto xAxis_s = origin_s + QVector2D(xDir_w.x(), -xDir_w.y()) * 30.0f;
	auto yAxis_s = origin_s + QVector2D(yDir_w.x(), -yDir_w.y()) * 30.0f;
	auto zAxis_s = origin_s + QVector2D(zDir_w.x(), -zDir_w.y()) * 30.0f;

	auto origin_n = QVector3D(-1.0f + origin_s.x() * 2.0f / this->width(), 1.0f - origin_s.y() * 2.0f / this->height(), 0.0f);
	auto xAxis_n = QVector3D(-1.0f + xAxis_s.x() * 2.0f / this->width(), 1.0f - xAxis_s.y() * 2.0f / this->height(), 0.0f);
	auto yAxis_n = QVector3D(-1.0f + yAxis_s.x() * 2.0f / this->width(), 1.0f - yAxis_s.y() * 2.0f / this->height(), 0.0f);
	auto zAxis_n = QVector3D(-1.0f + zAxis_s.x() * 2.0f / this->width(), 1.0f - zAxis_s.y() * 2.0f / this->height(), 0.0f);

	QMatrix4x4 proj;
	proj.ortho(-1.0, 1.0f, -1.0f,1.0f, 0.1f, 10.0f);
	auto mvp = proj * _scene->camera()->view();
	auto mvpInverted = mvp.inverted();
	auto origin_l = mvpInverted * origin_n;
	auto xAxis_l = mvpInverted * xAxis_n;
	auto yAxis_l = mvpInverted * yAxis_n;
	auto zAxis_l = mvpInverted * zAxis_n;

	std::vector<QVector3D> positions{ origin_l, xAxis_l, origin_l, yAxis_l, origin_l, zAxis_l };
	std::vector<QVector3D> colors{ QVector3D(1.0f, 0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f),
		QVector3D(0.0f, 1.0, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f) };

	auto shader = ResourceManager::instance().shaderProgram("KLEIN_VertexColorPrimitive");
	shader->bind();
	shader->setUniformValue("mvp", mvp);

	QOpenGLVertexArrayObject vao;
	vao.create();
	vao.bind();

	QOpenGLBuffer vboPosition(QOpenGLBuffer::VertexBuffer);
	vboPosition.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	vboPosition.create();
	vboPosition.bind();
	vboPosition.allocate(positions.data(), 6 * sizeof(QVector3D));
	shader->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	shader->enableAttributeArray(0);
	vboPosition.release();

	QOpenGLBuffer vboColor(QOpenGLBuffer::VertexBuffer);
	vboColor.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	vboColor.create();
	vboColor.bind();
	vboColor.allocate(colors.data(), 6 * sizeof(QVector3D));
	shader->setAttributeBuffer(1, GL_FLOAT, 0, 3);
	shader->enableAttributeArray(1);
	vboColor.release();

	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_LINES, 0, 6);
	glEnable(GL_DEPTH_TEST);

	vao.release();
}

void GLWidget::_logOpenGLMsg(const QOpenGLDebugMessage& msg)
{
	QString error;

	switch (msg.severity()) {
	case QOpenGLDebugMessage::NotificationSeverity:
		error += "--"; return; // Notification message is suppressed
		break;
	case QOpenGLDebugMessage::LowSeverity:
		error += "~~";
		break;
	case QOpenGLDebugMessage::MediumSeverity:
		error += "~!";
		break;
	case QOpenGLDebugMessage::HighSeverity:
		error += "!!";
		break;
	default:
		break;
	}

	error += "(";

#define CASE(c) case QOpenGLDebugMessage::c: error += #c; break;
	switch (msg.source()) {
		CASE(APISource);
		CASE(WindowSystemSource);
		CASE(ShaderCompilerSource);
		CASE(ThirdPartySource);
		CASE(ApplicationSource);
		CASE(OtherSource);
		CASE(InvalidSource);
	}
#undef CASE

	error += ":";

#define CASE(c) case QOpenGLDebugMessage::c: error += #c; break;
	switch (msg.type()) {
		CASE(ErrorType);
		CASE(DeprecatedBehaviorType);
		CASE(UndefinedBehaviorType);
		CASE(PortabilityType);
		CASE(PerformanceType);
		CASE(OtherType);
		CASE(MarkerType);
		CASE(GroupPushType);
		CASE(GroupPopType);
	}
#undef CASE

	error += ") ";
	KLEIN_LOG_DEBUG(error + msg.message());
}
