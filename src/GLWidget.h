#pragma once

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>
#include <QOpenGLFrameBufferObject>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QImage>
#include <memory>
#include <vector>

#ifdef _WIN32
#pragma warning(pop)
#endif

class CameraController;
class Scene;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
	Q_OBJECT

public:
	explicit GLWidget(QWidget* parent);
	~GLWidget();

	void bindScene(Scene* scene);
	void readColorBuffer(QImage& img); // Read offscreen-pass nodes
	void readDepthBuffer(std::vector<float>& depth);
	void renderToTexture(QImage& img); // Read render-pass nodes

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
};
