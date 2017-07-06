#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Core>
#include <memory>
#include <vector>
#include <string>

class Scene;
class CameraController;
class QImage;
class QOpenGLDebugLogger;
class QOpenGLDebugMessage;
struct PickingInfo;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
	Q_OBJECT

public:
	GLWidget();
	explicit GLWidget(QWidget* parent);

	void bindScene(Scene* scene);
	void renderToTexture(QImage& img); // Read render-pass nodes
	void readColorBuffer(QImage& img); // Read offscreen-pass nodes
	void readDepthBuffer(std::vector<float>& depth);
	PickingInfo pick(int x, int y);
	void renderPicked(const PickingInfo& info, const std::string& pickNodeID);

Q_SIGNALS:
	void picked(const PickingInfo& info);

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
	void _paintAxis();

private Q_SLOTS:
	void _logOpenGLMsg(const QOpenGLDebugMessage& msg);

private:
	Scene* _scene = nullptr;
	std::unique_ptr<CameraController> _cameraController = nullptr;
	QOpenGLDebugLogger* _glDebugLogger = nullptr;
	std::unique_ptr<QOpenGLFramebufferObject> _fboOffscreen = nullptr;
	GLuint _fboDepth;
	GLuint _fboPick;
};
