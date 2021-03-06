/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gm/gm.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkRRect.h"
#include "include/core/SkRect.h"
#include "include/core/SkScalar.h"
#include "include/core/SkSize.h"
#include "include/core/SkString.h"
#include "src/core/SkCanvasPriv.h"
#include "src/core/SkClipOpPriv.h"

namespace skiagm {

// This test exercise SkCanvas::androidFramework_replaceClip behavior
class ComplexClip4GM : public GM {
public:
  ComplexClip4GM(bool aaclip)
    : fDoAAClip(aaclip) {
        this->setBGColor(0xFFDEDFDE);
    }

protected:


    SkString onShortName() {
        SkString str;
        str.printf("complexclip4_%s",
                   fDoAAClip ? "aa" : "bw");
        return str;
    }

    SkISize onISize() { return SkISize::Make(970, 780); }

    // Android Framework will still support the legacy kReplace SkClipOp on older devices, so
    // this represents how to do so while also respecting the device restriction using the newer
    // androidFramework_replaceClip() API.
    void emulateDeviceRestriction(SkCanvas* canvas, const SkIRect& deviceRestriction) {
        // or any other device-space rect intersection
        SkCanvasPriv::ReplaceClip(canvas, deviceRestriction);
        // save for later replace clip ops
        fDeviceRestriction = deviceRestriction;
    }

    void emulateClipRectReplace(SkCanvas* canvas,
                                const SkRect& clipRect,
                                bool aa) {
        SkCanvasPriv::ReplaceClip(canvas, fDeviceRestriction);
        canvas->clipRect(clipRect, SkClipOp::kIntersect, aa);
    }

    void emulateClipRRectReplace(SkCanvas* canvas,
                                 const SkRRect& clipRRect,
                                 bool aa) {
        SkCanvasPriv::ReplaceClip(canvas, fDeviceRestriction);
        canvas->clipRRect(clipRRect, SkClipOp::kIntersect, aa);
    }

    void emulateClipPathReplace(SkCanvas* canvas,
                                const SkPath& path,
                                bool aa) {
        SkCanvasPriv::ReplaceClip(canvas, fDeviceRestriction);
        canvas->clipPath(path, SkClipOp::kIntersect, aa);
    }

    virtual void onDraw(SkCanvas* canvas) {
        SkPaint p;
        p.setAntiAlias(fDoAAClip);
        p.setColor(SK_ColorYELLOW);

        canvas->save();
            // draw a yellow rect through a rect clip
            canvas->save();
                emulateDeviceRestriction(canvas, SkIRect::MakeLTRB(100, 100, 300, 300));
                canvas->drawColor(SK_ColorGREEN);
                emulateClipRectReplace(canvas, SkRect::MakeLTRB(100, 200, 400, 500), fDoAAClip);
                canvas->drawRect(SkRect::MakeLTRB(100, 200, 400, 500), p);
            canvas->restore();

            // draw a yellow rect through a diamond clip
            canvas->save();
                emulateDeviceRestriction(canvas, SkIRect::MakeLTRB(500, 100, 800, 300));
                canvas->drawColor(SK_ColorGREEN);

                SkPath pathClip;
                pathClip.moveTo(SkIntToScalar(650),  SkIntToScalar(200));
                pathClip.lineTo(SkIntToScalar(900), SkIntToScalar(300));
                pathClip.lineTo(SkIntToScalar(650), SkIntToScalar(400));
                pathClip.lineTo(SkIntToScalar(650), SkIntToScalar(300));
                pathClip.close();
                emulateClipPathReplace(canvas, pathClip, fDoAAClip);
                canvas->drawRect(SkRect::MakeLTRB(500, 200, 900, 500), p);
            canvas->restore();

            // draw a yellow rect through a round rect clip
            canvas->save();
                emulateDeviceRestriction(canvas, SkIRect::MakeLTRB(500, 500, 800, 700));
                canvas->drawColor(SK_ColorGREEN);

                emulateClipRRectReplace(
                        canvas, SkRRect::MakeOval(SkRect::MakeLTRB(500, 600, 900, 750)), fDoAAClip);
                canvas->drawRect(SkRect::MakeLTRB(500, 600, 900, 750), p);
            canvas->restore();

            // fill the clip with yellow color showing that androidFramework_replaceClip is
            // in device space
            canvas->save();
                canvas->clipRect(SkRect::MakeLTRB(100, 400, 300, 750),
                                 kIntersect_SkClipOp, fDoAAClip);
                canvas->drawColor(SK_ColorGREEN);
                // should not affect the device-space clip
                canvas->rotate(20.f);
                canvas->translate(50.f, 50.f);
                emulateDeviceRestriction(canvas, SkIRect::MakeLTRB(150, 450, 250, 700));
                canvas->drawColor(SK_ColorYELLOW);
            canvas->restore();

        canvas->restore();
    }
private:
    SkIRect fDeviceRestriction;
    bool    fDoAAClip;

    typedef GM INHERITED;
};

//////////////////////////////////////////////////////////////////////////////

DEF_GM(return new ComplexClip4GM(false);)
DEF_GM(return new ComplexClip4GM(true);)
}
