/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef KLEIN_IMGUIMANAGER_H
#define KLEIN_IMGUIMANAGER_H

#include <Klein/Core/Export.h>

#include <functional>
#include <QObject>
#include <QSize>
#include <Qt3DCore/QEntity>

namespace Qt3DCore
{
class QTransform;
}

namespace Qt3DRender
{
class QBlendEquation;
class QBlendEquationArguments;
class QBuffer;
class QCamera;
class QColorMask;
class QCullFace;
class QDepthTest;
class QFilterKey;
class QFrameGraphNode;
class QGeometryRenderer;
class QLayer;
class QMaterial;
class QNoDepthMask;
class QParameter;
class QShaderProgram;
class QScissorTest;
class QTexture2D;
} // namespace Qt3DRender

namespace Klein
{

class ImGuiInputEventFilter;

class KLEIN_API ImGuiManager : public QObject
{
    Q_OBJECT

public:
    typedef std::function<void(bool)> FrameFunc;

public:
    explicit ImGuiManager(QObject* parent = nullptr);

    void setFrameFunc(FrameFunc f) { m_frame = f; }

    void setInputEventSource(QObject* src);

    void initialize(Qt3DCore::QEntity* rootEntity,
                    float fontSize = 15.0f,
                    const char* fontFile = nullptr);

    void releaseResources();

    bool isEnabled() const { return m_enabled; }

    void setEnabled(bool enabled);

    float scale() const { return m_scale; }

    void setScale(float scale);

    void resize(QSize size, qreal dpr);

    Qt3DRender::QFrameGraphNode* attachGuiPassTo(
        Qt3DRender::QFrameGraphNode* parent);

    Qt3DRender::QFrameGraphNode* attachSkipGuiPassTo(
        Qt3DRender::QFrameGraphNode* parent);

private:
    struct SharedRenderPassData
    {
        bool valid = false;
        Qt3DRender::QShaderProgram* progES2 = nullptr;
        Qt3DRender::QShaderProgram* progGL3 = nullptr;
        Qt3DRender::QFilterKey* techniqueFilterKey = nullptr;
        Qt3DRender::QDepthTest* depthTest = nullptr;
        Qt3DRender::QNoDepthMask* noDepthWrite = nullptr;
        Qt3DRender::QBlendEquation* blendFunc = nullptr;
        Qt3DRender::QBlendEquationArguments* blendArgs = nullptr;
        Qt3DRender::QCullFace* cullFace = nullptr;
        Qt3DRender::QColorMask* colorMask = nullptr;
        QVector<Qt3DCore::QNode*> enabledToggle;
    } rpd;

    struct CmdEntry
    {
        Qt3DCore::QEntity* entity = nullptr;
        Qt3DRender::QMaterial* material = nullptr;
        Qt3DRender::QGeometryRenderer* geomRenderer = nullptr;
        Qt3DCore::QTransform* transform = nullptr;
        Qt3DRender::QScissorTest* scissor = nullptr;
        Qt3DRender::QParameter* texParam = nullptr;
    };

    struct CmdListEntry
    {
        Qt3DRender::QBuffer* vbuf = nullptr;
        Qt3DRender::QBuffer* ibuf = nullptr;
        QVector<CmdEntry> cmds;
        int activeSize = 0;
    };

    struct DisplayInfo
    {
        QSize size;
        qreal dpr = 0.0;
    };

private:
    void resizePool(CmdListEntry* e, int newSize);

    Qt3DRender::QMaterial* buildMaterial(Qt3DRender::QScissorTest** scissor);

    void updateGeometry(CmdListEntry* e,
                        int idx,
                        uint elemCount,
                        int vertexCount,
                        int indexCount,
                        const void* indexOffset);

    void update3D();

    void updateInput();

private:
    FrameFunc m_frame = nullptr;
    DisplayInfo m_displayInfo;
    QObject* m_inputEventSource = nullptr;
    ImGuiInputEventFilter* m_inputEventFilter = nullptr;
    bool m_inputInitialized = false;
    bool m_enabled = true;
    bool m_captureInput = false;
    float m_scale = 1.0f;
    Qt3DCore::QEntity* m_guiRoot = nullptr;
    Qt3DRender::QCamera* m_camera = nullptr;
    Qt3DRender::QTexture2D* m_atlasTex = nullptr;
    QVector<CmdListEntry> m_cmdList;
    Qt3DRender::QLayer* m_guiLayer = nullptr;
    Qt3DRender::QLayer* m_activeLayer = nullptr;
    Qt3DRender::QFilterKey* m_guiTechniqueFilterKey = nullptr;
};

} // namespace Klein

#endif
