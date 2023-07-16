#ifndef LSURFACEVIEW_H
#define LSURFACEVIEW_H

#include <LView.h>

class Louvre::LSurfaceView : public LView
{
public:
    LSurfaceView(LSurface *surface, LView *parent = nullptr);
    ~LSurfaceView();

    LSurface *surface() const;

    /*!
     * Only primary views will clear its damage when
     * requestNextFrame() is called.\n
     * Default value is true.
     */
    virtual bool primary() const;

    /*!
     * Only primary views will clear its damage when
     * requestNextFrame() is called.
     */
    virtual void setPrimary(bool primary);

    /*!
     * If enabled, nativePosC() will return the position set with
     * setCustomPos() instead of the surface pos.\n
     * Default value is false.
     */
    virtual bool customPosEnabled() const;

    /*!
     * If enabled, nativePosC() will return the position set with
     * setCustomPos() instead of the surface pos.
     */
    virtual void enableCustomPos(bool enable);

    /*!
     * If enabled, inputRegionC() will return the input region set with
     * setCustomInputRegionC() instead of the surface input region.\n
     * Default value is false.
     */
    virtual bool customInputRegionEnabled() const;

    /*!
     * If enabled, inputRegionC() will return the input region set with
     * setCustomInputRegionC() instead of the surface input region.
     */
    virtual void enableCustomInputRegion(bool enabled);

    virtual void setCustomPosC(const LPoint &pos);
    virtual const LPoint &customPosC() const;

    virtual void setCustomInputRegionC(const LRegion *region);
    virtual const LRegion *customInputRegionC() const;

    // LView
    virtual bool nativeMapped() const override;
    virtual const LPoint &nativePosC() const override;
    virtual const LSize &nativeSizeC() const override;
    virtual Int32 bufferScale() const override;
    virtual void enteredOutput(LOutput *output) override;
    virtual void leftOutput(LOutput *output) override;
    virtual const std::list<LOutput*> &outputs() const override;
    virtual bool isRenderable() const override;
    virtual void requestNextFrame(LOutput *output) override;
    virtual const LRegion *damageC() const override;
    virtual const LRegion *translucentRegionC() const override;
    virtual const LRegion *opaqueRegionC() const override;
    virtual const LRegion *inputRegionC() const override;
    virtual void paintRectC(LPainter *p,
                           Int32 srcX, Int32 srcY,
                           Int32 srcW, Int32 srcH,
                           Int32 dstX, Int32 dstY,
                           Int32 dstW, Int32 dstH,
                           Float32 scale,
                           Float32 alpha) override;

LPRIVATE_IMP(LSurfaceView)
};

#endif // LSURFACEVIEW_H
