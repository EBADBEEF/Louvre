#include <private/LCursorPrivate.h>
#include <private/LOutputPrivate.h>
#include <private/LCompositorPrivate.h>
#include <private/LTexturePrivate.h>
#include <private/LPainterPrivate.h>

#include <LCursorRole.h>
#include <LPointer.h>
#include <LSeat.h>
#include <LRect.h>
#include <LPainter.h>
#include <LLog.h>

#include <string.h>
#include <other/cursor.h>
#include <algorithm>

using namespace Louvre;

LCursor::LCursor() : LPRIVATE_INIT_UNIQUE(LCursor)
{
    compositor()->imp()->cursor = this;

    if (!imp()->louvreTexture.setDataB(
            LSize(LOUVRE_DEFAULT_CURSOR_WIDTH, LOUVRE_DEFAULT_CURSOR_HEIGHT),
            LOUVRE_DEFAULT_CURSOR_STRIDE,
            DRM_FORMAT_ARGB8888,
            louvre_default_cursor_data()))
        LLog::warning("[LCursor::LCursor] Failed to create default cursor texture.");

    imp()->defaultTexture = &imp()->louvreTexture;
    imp()->defaultHotspotB = LPointF(9);

    glGenFramebuffers(1, &imp()->glFramebuffer);

    if (imp()->glFramebuffer == 0)
    {
        LLog::error("[LCursor::LCursor] Failed to create GL framebuffer.");
        imp()->hasFb = false;
        goto skipGL;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, imp()->glFramebuffer);
    glGenRenderbuffers(1, &imp()->glRenderbuffer);

    if (imp()->glRenderbuffer == 0)
    {
        LLog::error("[LCursor::LCursor] Failed to create GL renderbuffer.");
        imp()->hasFb = false;
        glDeleteFramebuffers(1, &imp()->glFramebuffer);
        goto skipGL;
    }

    glBindRenderbuffer(GL_RENDERBUFFER, imp()->glRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, LOUVRE_DEFAULT_CURSOR_WIDTH, LOUVRE_DEFAULT_CURSOR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, imp()->glRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LLog::error("[LCursor::LCursor] GL_FRAMEBUFFER incomplete.");
        imp()->hasFb = false;
        glDeleteRenderbuffers(1, &imp()->glRenderbuffer);
        glDeleteFramebuffers(1, &imp()->glFramebuffer);
    }

    skipGL:

    setSize(LSize(24));
    useDefault();
    setVisible(true);
}

LCursor::~LCursor()
{
    compositor()->imp()->cursor = nullptr;

    if (imp()->glRenderbuffer)
        glDeleteRenderbuffers(1, &imp()->glRenderbuffer);

    if (imp()->glFramebuffer)
        glDeleteFramebuffers(1, &imp()->glFramebuffer);
}

void LCursor::useDefault()
{
    if (compositor()->state() == LCompositor::Uninitializing)
        return;

    imp()->clientCursor.reset();

    if (imp()->texture == imp()->defaultTexture && imp()->hotspotB == imp()->defaultHotspotB)
        return;

    setTextureB(imp()->defaultTexture, imp()->defaultHotspotB);
}

void LCursor::replaceDefaultB(const LTexture *texture, const LPointF &hotspot)
{
    if (compositor()->state() == LCompositor::Uninitializing)
        return;

    const bool update { imp()->defaultTexture == imp()->texture };

    if (!texture)
    {
        imp()->defaultTexture = &imp()->louvreTexture;
        imp()->defaultHotspotB = LPointF(9);
    }
    else
    {
        imp()->defaultTexture = (LTexture*)texture;
        imp()->defaultHotspotB = hotspot;
    }

    if (update)
        useDefault();
}

void LCursor::setTextureB(const LTexture *texture, const LPointF &hotspot)
{
    imp()->clientCursor.reset();

    if (!texture)
        return;

    if (imp()->texture == texture && imp()->lastTextureSerial == texture->serial() && hotspot == imp()->hotspotB)
        return;

    if (imp()->texture != texture || imp()->lastTextureSerial != texture->serial())
    {
        imp()->texture = (LTexture*)texture;
        imp()->textureChanged = true;
        imp()->lastTextureSerial = texture->serial();
    }

    imp()->hotspotB = hotspot;
    imp()->update();
}

void LCursor::setCursor(const LClientCursor &clientCursor) noexcept
{
    if (clientCursor.cursorRole())
    {
        setTextureB(clientCursor.cursorRole()->surface()->texture(), clientCursor.cursorRole()->hotspotB());
        imp()->clientCursor.reset(&clientCursor);
    }
    else
        useDefault();

    setVisible(clientCursor.visible());
}

const LClientCursor *LCursor::clientCursor() const noexcept
{
    return imp()->clientCursor.get();
}

void LCursor::move(Float32 x, Float32 y)
{
    imp()->setPos(imp()->pos + LPointF(x,y));
}

void LCursor::move(const LPointF &delta)
{
    imp()->setPos(imp()->pos + delta);
}

void Louvre::LCursor::setPos(const LPointF &pos)
{
    imp()->setPos(pos);
}

void LCursor::setPos(Float32 x, Float32 y)
{
    setPos(LPointF(x, y));
}

void LCursor::setHotspotB(const LPointF &hotspot)
{
    if (imp()->hotspotB != hotspot)
    {
        imp()->hotspotB = hotspot;
        imp()->update();
    }
}

void LCursor::setSize(const LSizeF &size)
{
    if (imp()->size != size)
    {
        imp()->size = size;
        imp()->textureChanged = true;
        imp()->update();
    }
}

void LCursor::setVisible(bool state)
{
    if (state == visible())
        return;

    imp()->isVisible = state;

    if (!visible())
    {
        for (LOutput *o : compositor()->outputs())
            compositor()->imp()->graphicBackend->outputSetCursorTexture(
                        o,
                        nullptr);
    }
    else if (texture())
    {
        imp()->textureChanged = true;
        imp()->update();
    }
}

void LCursor::repaintOutputs(bool nonHardwareOnly)
{
    for (LOutput *o : intersectedOutputs())
        if (!nonHardwareOnly || !hasHardwareSupport(o))
            o->repaint();

    if (clientCursor() && clientCursor()->cursorRole() && clientCursor()->cursorRole()->surface())
    {
        for (LOutput *output : compositor()->outputs())
        {
            if (output == cursor()->output())
                clientCursor()->cursorRole()->surface()->sendOutputEnterEvent(output);
            else
                clientCursor()->cursorRole()->surface()->sendOutputLeaveEvent(output);
        }
    }
}

bool LCursor::visible() const
{
    return imp()->isVisible;
}

bool LCursor::hasHardwareSupport(const LOutput *output) const
{
    if (!imp()->hasFb)
        return false;

    return compositor()->imp()->graphicBackend->outputHasHardwareCursorSupport((LOutput*)output);
}

const LPointF &LCursor::pos() const
{
    return imp()->pos;
}

const LPointF &LCursor::hotspotB() const
{
    return imp()->hotspotB;
}

LTexture *LCursor::texture() const
{
    return imp()->texture;
}

LTexture *LCursor::defaultTexture() const
{
    return imp()->defaultTexture;
}

const LPointF &LCursor::defaultHotspotB() const
{
    return imp()->defaultHotspotB;
}

LTexture *LCursor::defaultLouvreTexture() const
{
    return &imp()->louvreTexture;
}

LOutput *LCursor::output() const
{
    if (imp()->output)
        return imp()->output;

    if (!compositor()->outputs().empty())
    {
        imp()->output = compositor()->outputs().front();
        imp()->textureChanged = true;
    }
    else
        imp()->output = nullptr;

    return imp()->output;
}

const std::vector<LOutput *> &LCursor::intersectedOutputs() const
{
    return imp()->intersectedOutputs;
}

const LRect &LCursor::rect() const
{
    return imp()->rect;
}
