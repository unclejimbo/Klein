#include "Editor/CameraController.h"

CameraController::CameraController(Camera& camera, int width, int height)
	: _camera(&camera), _width(width), _height(height)
{
}

CameraController::~CameraController() = default;

void CameraController::bindCamera(Camera& camera)
{
	_camera = &camera;
}

void CameraController::onResize(int width, int height)
{
	_width = width;
	_height = height;
}
