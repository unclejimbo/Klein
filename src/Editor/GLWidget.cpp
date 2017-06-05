#include "Editor/GLWidget.h"
#include "Editor/ArcballController.h"
#include "Core/Scene.h"
#include "Core/SceneNode.h"
#include "Core/PrimitiveGraphics.h"
#include "Core/ResourceManager.h"
#include "Core/Logger.h"
#include "Core/Util.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFrameBufferObject>
#include <QPainter>
#include <QColor>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector2D>
#include <vector>

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

void GLWidget::renderToTexture(QImage& img)
{
	this->makeCurrent();
	_fboOffscreen->bind();

	auto bgColor = QColor(112, 128, 144);
	glClearColor(static_cast<float>(bgColor.redF()), static_cast<float>(bgColor.greenF()),
		static_cast<float>(bgColor.blueF()), static_cast<float>(bgColor.alphaF()));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	if (_scene != nullptr) {
		_scene->render(RENDER_ONSCREEN); // Models in shading pass got rendered to texture, see readColorBuffer for difference
	}
	img = _fboOffscreen->toImage();

	QOpenGLFramebufferObject::bindDefault();
	this->doneCurrent();
}

void GLWidget::readColorBuffer(QImage& img)
{
	this->makeCurrent();
	_fboOffscreen->bind();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	if (_scene != nullptr) {
		_scene->render(RENDER_OFFSCREEN); // Models in offscreen pass got rendered, see renderToTexture for difference
	}
	img = _fboOffscreen->toImage();

	QOpenGLFramebufferObject::bindDefault();
	this->doneCurrent();
}

void GLWidget::readDepthBuffer(std::vector<float>& depth)
{
	this->makeCurrent();
	glBindFramebuffer(GL_FRAMEBUFFER, _fboDepth);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	if (_scene != nullptr) {
		_scene->render(RENDER_OFFSCREEN);
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

PickingInfo GLWidget::pick(int x, int y)
{
	this->makeCurrent();
	glBindFramebuffer(GL_FRAMEBUFFER, _fboPick);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (_scene != nullptr) {
		_scene->render(RENDER_PICK);
	}
	GLuint buf[4];
	glReadPixels(x, this->height() - y, 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, buf);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->doneCurrent();

	PickingInfo info(buf);
	return info;
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

	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	format.setInternalTextureFormat(GL_RGBA);
	format.setMipmap(true);
	format.setSamples(4);
	format.setTextureTarget(GL_TEXTURE_2D);
	_fboOffscreen = std::make_unique<QOpenGLFramebufferObject>(this->width(), this->height(), format);

	GLuint _fboDepth;
	glGenFramebuffers(1, &_fboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, _fboDepth);
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
	glBindFramebuffer(GL_FRAMEBUFFER, _fboDepth);
	glViewport(0, 0, this->width(), this->height());

	glGenFramebuffers(1, &_fboPick);
	glBindFramebuffer(GL_FRAMEBUFFER, _fboPick);
	GLuint colorBuffer1;
	glGenRenderbuffers(1, &colorBuffer1);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer1);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32UI, this->width(), this->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer1);
	GLuint depthBuffer1;
	glGenRenderbuffers(1, &depthBuffer1);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer1);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width(), this->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer1);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glViewport(0, 0, this->width(), this->height());
}

void GLWidget::paintGL()
{
	auto bgColor = QColor(112, 128, 144);
	glClearColor(static_cast<float>(bgColor.redF()), static_cast<float>(bgColor.greenF()),
		static_cast<float>(bgColor.blueF()), static_cast<float>(bgColor.alphaF()));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (_scene != nullptr) {
		_scene->render(RENDER_ONSCREEN);
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
	if (event->button() == Qt::LeftButton) {
		auto info = pick(event->x(), event->y());
		if (info.bufferID != 0) {
			_paintPicked(info);
			Q_EMIT(picked(info));
		}
	}
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

	auto shader = ResourceManager::instance().shaderProgram("KLEIN_Unlit_VColor");
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

void GLWidget::_paintPicked(PickingInfo info)
{
	if (info.primitiveType == PICKING_PRIMITIVE_NONE) {
		return;
	}

	_scene->removeNode("PickedNode");
	auto parent = _scene->node(info.nodeID);
	auto buffer = ResourceManager::instance().glBuffer(info.bufferID);
	this->makeCurrent();

	if (info.bufferSpec == GL_TRIANGLES) {
		if (info.primitiveType == PICKING_PRIMITIVE_VERTEX) {
			QVector3D point;
			buffer->bind();
			buffer->read(info.primitiveID * sizeof(QVector3D), &point, sizeof(QVector3D));
			buffer->release();
			auto node = _scene->addNode(parent, "PickedNode");
			auto graphics = std::make_unique<PrimitiveGraphics>(*this);
			graphics->setPointSize(10);
			graphics->addPoint(point);
			node->addGraphicsComponent(std::move(graphics));
		}

		if (info.primitiveType == PICKING_PRIMITIVE_FACE) {
			QVector3D points[3];
			buffer->bind();
			buffer->read(3 * info.primitiveID * sizeof(QVector3D), points, sizeof(QVector3D) * 3);
			buffer->release();
			auto node = _scene->addNode(parent, "PickedNode");
			auto graphics = std::make_unique<PrimitiveGraphics>(*this);
			graphics->setPointSize(10);
			graphics->addPoint(points[0]);
			graphics->addPoint(points[1]);
			graphics->addPoint(points[2]);
			node->addGraphicsComponent(std::move(graphics));
		}
	}

	if (info.bufferSpec == GL_POINTS) {
		if (info.primitiveType == PICKING_PRIMITIVE_VERTEX) {
			QVector3D point;
			buffer->bind();
			buffer->read(info.primitiveID * sizeof(QVector3D), &point, sizeof(QVector3D));
			buffer->release();
			auto node = _scene->addNode(parent, "PickedNode");
			auto graphics = std::make_unique<PrimitiveGraphics>(*this);
			graphics->setPointSize(10);
			graphics->addPoint(point);
			node->addGraphicsComponent(std::move(graphics));
		}
	}

	this->doneCurrent();
	this->update();
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
