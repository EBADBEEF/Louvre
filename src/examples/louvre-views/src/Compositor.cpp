#include <LLayerView.h>
#include <LAnimation.h>
#include <LTextureView.h>
#include <LTimer.h>
#include <LLog.h>

#include "Client.h"
#include "Global.h"
#include "Compositor.h"
#include "Output.h"
#include "Surface.h"
#include "Seat.h"
#include "Pointer.h"
#include "Keyboard.h"
#include "Toplevel.h"
#include "TextRenderer.h"
#include "Topbar.h"

Compositor::Compositor() : LCompositor(),
    scene(),
    backgroundLayer(scene.mainView()),
    surfacesLayer(scene.mainView()),
    workspacesLayer(scene.mainView()),
    fullscreenLayer(scene.mainView()),
    overlayLayer(scene.mainView()),
    tooltipsLayer(scene.mainView())
{
    // Set black as default background color
    scene.mainView()->setClearColor(0.f, 0.f, 0.f, 1.f);
}

Compositor::~Compositor() {}

void Compositor::initialized()
{
    // Change the keyboard map to "latam"
    seat()->keyboard()->setKeymap(NULL, NULL, "latam", NULL);

    G::loadDockTextures();
    G::loadCursors();
    G::loadToplevelTextures();
    G::loadFonts();
    G::createTooltip();
    G::loadApps();

    clockMinuteTimer = new LTimer([](LTimer *timer)
    {
        if (G::font()->regular)
        {
            char text[128];
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(text, sizeof(text), "%a %b %d, %I:%M %p", timeinfo);

            LTexture *newClockTexture = G::font()->regular->renderText(text, 22);

            if (newClockTexture)
            {
                for (Output *o : G::outputs())
                {
                    if (o->topbar && o->topbar->clock)
                    {
                        o->topbar->clock->setTexture(newClockTexture);
                        o->topbar->update();
                    }
                }

                if (G::compositor()->clockTexture)
                {
                    delete G::compositor()->clockTexture;
                    G::compositor()->clockTexture = newClockTexture;
                }
            }
        }

        timer->start(millisecondsUntilNextMinute() + 1500);
    });

    // Start the timer right on to setup the clock texture
    clockMinuteTimer->start(1);

    Int32 totalWidth = 0;

    // Initialize and arrange outputs (screens) left to right
    for (LOutput *output : *seat()->outputs())
    {
        // Set scale 2 to HiDPI screens
        output->setScale(output->dpi() >= 120 ? 2 : 1);
        output->setPos(LPoint(totalWidth, 0));
        totalWidth += output->size().w();
        compositor()->addOutput(output);
        output->repaint();
    }
}

LClient *Compositor::createClientRequest(LClient::Params *params)
{
    return new Client(params);
}

LOutput *Compositor::createOutputRequest()
{
    return new Output();
}

LSurface *Compositor::createSurfaceRequest(LSurface::Params *params)
{
    return new Surface(params);
}

LSeat *Compositor::createSeatRequest(LSeat::Params *params)
{
    return new Seat(params);
}

LPointer *Compositor::createPointerRequest(LPointer::Params *params)
{
    return new Pointer(params);
}

LKeyboard *Compositor::createKeyboardRequest(LKeyboard::Params *params)
{
    return new Keyboard(params);
}

LToplevelRole *Compositor::createToplevelRoleRequest(LToplevelRole::Params *params)
{
    return new Toplevel(params);
}

Int32 Compositor::millisecondsUntilNextMinute()
{
    time_t rawtime;
    struct tm *timeinfo;
    struct timespec spec;

    // Get the current time
    clock_gettime(CLOCK_REALTIME, &spec);
    rawtime = spec.tv_sec;
    timeinfo = localtime(&rawtime);

    // Calculate the number of seconds until the next minute
    int secondsUntilNextMinute = 60 - timeinfo->tm_sec;

    // Calculate the number of milliseconds until the next minute
    int msUntilNextMinute = secondsUntilNextMinute * 1000 - spec.tv_nsec / 1000000;

    return msUntilNextMinute;
}
