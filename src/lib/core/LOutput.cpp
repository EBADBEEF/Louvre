#include <private/LOutputPrivate.h>
#include <private/LCompositorPrivate.h>
#include <private/LPainterPrivate.h>
#include <private/LSurfacePrivate.h>
#include <private/LCursorPrivate.h>

#include <protocols/Wayland/private/GOutputPrivate.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <LToplevelRole.h>
#include <LRegion.h>
#include <LSeat.h>
#include <LOutputMode.h>
#include <LTime.h>
#include <LLog.h>

using namespace Louvre;

LOutput::LOutput()
{
    m_imp = new LOutputPrivate();
    imp()->output = this;
    imp()->rectC.setX(0);
    imp()->rectC.setY(0);
}

LOutput::~LOutput()
{
    delete m_imp;
}

const list<LOutputMode *> &LOutput::modes() const
{
    return *compositor()->imp()->graphicBackend->getOutputModes((LOutput*)this);
}

const LOutputMode *LOutput::preferredMode() const
{
    return compositor()->imp()->graphicBackend->getOutputPreferredMode((LOutput*)this);
}

const LOutputMode *LOutput::currentMode() const
{
    return compositor()->imp()->graphicBackend->getOutputCurrentMode((LOutput*)this);
}

void LOutput::setMode(const LOutputMode *mode)
{
    if (mode == currentMode())
        return;

    imp()->pendingMode = (LOutputMode*)mode;
    imp()->state = ChangingMode;
    compositor()->imp()->graphicBackend->setOutputMode(this, (LOutputMode*)mode);
    imp()->state = Initialized;
}

Int32 LOutput::currentBuffer() const
{
    return compositor()->imp()->graphicBackend->getOutputCurrentBufferIndex((LOutput*)this);
}

UInt32 LOutput::buffersCount() const
{
    return compositor()->imp()->graphicBackend->getOutputBuffersCount((LOutput*)this);
}

LTexture *LOutput::bufferTexture(UInt32 bufferIndex)
{
    return compositor()->imp()->graphicBackend->getOutputBuffer((LOutput*)this, bufferIndex);
}

void LOutput::setScale(Int32 scale)
{
    imp()->outputScale = scale;

    compositor()->imp()->updateGlobalScale();

    imp()->rectC.setBR((sizeB()*compositor()->globalScale())/scale);

    for (LClient *c : compositor()->clients())
    {
        for (GOutput *gOutput : c->outputGlobals())
        {
            if (this == gOutput->output())
            {
                gOutput->sendConfiguration();
                break;
            }
        }
    }
}

Int32 LOutput::scale() const
{
    return imp()->outputScale;
}

void LOutput::repaint()
{
    compositor()->imp()->graphicBackend->scheduleOutputRepaint(this);
}

Int32 LOutput::dpi()
{
    float w = sizeB().w();
    float h = sizeB().h();

    float Wi = physicalSize().w();
    Wi /= 25.4;
    float Hi = physicalSize().h();
    Hi /= 25.4;

    return sqrtf(w*w+h*h)/sqrtf(Wi*Wi+Hi*Hi);
}

const LSize &LOutput::physicalSize() const
{
    return *compositor()->imp()->graphicBackend->getOutputPhysicalSize((LOutput*)this);
}

const LSize &LOutput::sizeB() const
{
    return currentMode()->sizeB();
}

const LRect &LOutput::rectC() const
{
    return imp()->rectC;
}

const LPoint &LOutput::posC() const
{
    return rectC().topLeft();
}

const LSize &LOutput::sizeC() const
{
    return rectC().bottomRight();
}

EGLDisplay LOutput::eglDisplay()
{
    return compositor()->imp()->graphicBackend->getOutputEGLDisplay(this);
}

LOutput::State LOutput::state() const
{
    return imp()->state;
}

const char *LOutput::name() const
{
    return compositor()->imp()->graphicBackend->getOutputName((LOutput*)this);
}

const char *LOutput::model() const
{
    return compositor()->imp()->graphicBackend->getOutputModelName((LOutput*)this);
}

const char *LOutput::manufacturer() const
{
    return compositor()->imp()->graphicBackend->getOutputManufacturerName((LOutput*)this);
}

const char *LOutput::description() const
{
    return compositor()->imp()->graphicBackend->getOutputDescription((LOutput*)this);
}

void LOutput::setPosC(const LPoint &posC)
{
    imp()->rectC.setX(posC.x());
    imp()->rectC.setY(posC.y());
}

LPainter *LOutput::painter() const
{
    return imp()->painter;
}
