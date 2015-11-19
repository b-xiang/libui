#include "PathOpsCubicIntersectionTestData.h"
#include "PathOpsQuadIntersectionTestData.h"
#include "SkCommonFlags.h"
#include "SkPaint.h"
#include "SkPath.h"
#include "SkRandom.h"
#include "SkStrokerPriv.h"
#include "SkTime.h"
#include "Test.h"

DEFINE_bool(timeout, true, "run until alloted time expires");

#define MS_TEST_DURATION 10

const SkScalar widths[] = {-FLT_MAX, -1, -0.1f, -FLT_EPSILON, 0, FLT_EPSILON,
        0.0000001f, 0.000001f, 0.00001f, 0.0001f, 0.001f, 0.01f,
        0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 1, 1.1f, 2, 10, 10e2f, 10e3f, 10e4f, 10e5f, 10e6f, 10e7f,
        10e8f, 10e9f, 10e10f, 10e20f,  FLT_MAX };
size_t widths_count = SK_ARRAY_COUNT(widths);

static void pathTest(const SkPath& path) {
    SkPaint p;
	SkPath fill;
    p.setStyle(SkPaint::kStroke_Style);
    for (size_t index = 0; index < widths_count; ++index) {
        p.setStrokeWidth(widths[index]);
        p.getFillPath(path, &fill);
    }
}

static void cubicTest(const SkPoint c[4]) {
	SkPath path;
	path.moveTo(c[0].fX, c[0].fY);
	path.cubicTo(c[1].fX, c[1].fY, c[2].fX, c[2].fY, c[3].fX, c[3].fY);
	pathTest(path);
}

static void quadTest(const SkPoint c[3]) {
	SkPath path;
	path.moveTo(c[0].fX, c[0].fY);
	path.quadTo(c[1].fX, c[1].fY, c[2].fX, c[2].fY);
	pathTest(path);
}

static void cubicSetTest(const SkDCubic* dCubic, size_t count) {
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
	for (size_t index = 0; index < count; ++index) {
		const SkDCubic& d = dCubic[index];
		SkPoint c[4] = { {(float) d[0].fX, (float) d[0].fY}, {(float) d[1].fX, (float) d[1].fY},
                         {(float) d[2].fX, (float) d[2].fY}, {(float) d[3].fX, (float) d[3].fY} };
	    cubicTest(c);
        if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
            return;
        }
	}
}

static void cubicPairSetTest(const SkDCubic dCubic[][2], size_t count) {
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
	for (size_t index = 0; index < count; ++index) {
		for (int pair = 0; pair < 2; ++pair) {
			const SkDCubic& d = dCubic[index][pair];
			SkPoint c[4] = { {(float) d[0].fX, (float) d[0].fY}, {(float) d[1].fX, (float) d[1].fY},
                             {(float) d[2].fX, (float) d[2].fY}, {(float) d[3].fX, (float) d[3].fY} };
			cubicTest(c);
            if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
                return;
            }
		}
	}
}

static void quadSetTest(const SkDQuad* dQuad, size_t count) {
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
	for (size_t index = 0; index < count; ++index) {
		const SkDQuad& d = dQuad[index];
		SkPoint c[3] = { {(float) d[0].fX, (float) d[0].fY}, {(float) d[1].fX, (float) d[1].fY},
                         {(float) d[2].fX, (float) d[2].fY}  };
	    quadTest(c);
        if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
            return;
        }
	}
}

static void quadPairSetTest(const SkDQuad dQuad[][2], size_t count) {
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
	for (size_t index = 0; index < count; ++index) {
		for (int pair = 0; pair < 2; ++pair) {
			const SkDQuad& d = dQuad[index][pair];
			SkPoint c[3] = { {(float) d[0].fX, (float) d[0].fY}, {(float) d[1].fX, (float) d[1].fY},
                             {(float) d[2].fX, (float) d[2].fY}  };
			quadTest(c);
            if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
                return;
            }
		}
	}
}

DEF_TEST(QuadStrokerSet, reporter) {
	quadSetTest(quadraticLines, quadraticLines_count);
	quadSetTest(quadraticPoints, quadraticPoints_count);
	quadSetTest(quadraticModEpsilonLines, quadraticModEpsilonLines_count);
	quadPairSetTest(quadraticTests, quadraticTests_count);
}

DEF_TEST(CubicStrokerSet, reporter) {
	cubicSetTest(pointDegenerates, pointDegenerates_count);
	cubicSetTest(notPointDegenerates, notPointDegenerates_count);
	cubicSetTest(lines, lines_count);
	cubicSetTest(notLines, notLines_count);
	cubicSetTest(modEpsilonLines, modEpsilonLines_count);
	cubicSetTest(lessEpsilonLines, lessEpsilonLines_count);
	cubicSetTest(negEpsilonLines, negEpsilonLines_count);
	cubicPairSetTest(tests, tests_count);
}

static SkScalar unbounded(SkRandom& r) {
    uint32_t val = r.nextU();
    return SkBits2Float(val);
}

static SkScalar unboundedPos(SkRandom& r) {
    uint32_t val = r.nextU() & 0x7fffffff;
    return SkBits2Float(val);
}

DEF_TEST(QuadStrokerUnbounded, reporter) {
    SkRandom r;
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    int best = 0;
    sk_bzero(gMaxRecursion, sizeof(gMaxRecursion[0]) * 3);
#endif
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
    for (int i = 0; i < 1000000; ++i) {
        SkPath path, fill;
        path.moveTo(unbounded(r), unbounded(r));
        path.quadTo(unbounded(r), unbounded(r), unbounded(r), unbounded(r));
        p.setStrokeWidth(unboundedPos(r));
        p.getFillPath(path, &fill);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
        if (best < gMaxRecursion[2]) {
            if (FLAGS_veryVerbose) {
                SkDebugf("\n%s quad=%d width=%1.9g\n", __FUNCTION__, gMaxRecursion[2],
                        p.getStrokeWidth());
                path.dumpHex();
                SkDebugf("fill:\n");
                fill.dumpHex();
            }
            best = gMaxRecursion[2];
        }
#endif
        if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
            return;
        }
    }
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    if (FLAGS_veryVerbose) {
       SkDebugf("\n%s max quad=%d\n", __FUNCTION__, best);
    }
#endif
}

DEF_TEST(CubicStrokerUnbounded, reporter) {
    SkRandom r;
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    int bestTan = 0;
    int bestCubic = 0;
    sk_bzero(gMaxRecursion, sizeof(gMaxRecursion[0]) * 3);
#endif
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
    for (int i = 0; i < 1000000; ++i) {
        SkPath path, fill;
        path.moveTo(unbounded(r), unbounded(r));
        path.cubicTo(unbounded(r), unbounded(r), unbounded(r), unbounded(r),
                unbounded(r), unbounded(r));
        p.setStrokeWidth(unboundedPos(r));
        p.getFillPath(path, &fill);
    #if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
        if (bestTan < gMaxRecursion[0] || bestCubic < gMaxRecursion[1]) {
            if (FLAGS_veryVerbose) {
                SkDebugf("\n%s tan=%d cubic=%d width=%1.9g\n", __FUNCTION__, gMaxRecursion[0],
                        gMaxRecursion[1], p.getStrokeWidth());
                path.dumpHex();
                SkDebugf("fill:\n");
                fill.dumpHex();
            }
            bestTan = SkTMax(bestTan, gMaxRecursion[0]);
            bestCubic = SkTMax(bestCubic, gMaxRecursion[1]);
        }
    #endif
        if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
            return;
        }
    }
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    if (FLAGS_veryVerbose) {
        SkDebugf("\n%s max tan=%d cubic=%d\n", __FUNCTION__, bestTan, bestCubic);
    }
#endif
}

DEF_TEST(QuadStrokerConstrained, reporter) {
    SkRandom r;
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    int best = 0;
    sk_bzero(gMaxRecursion, sizeof(gMaxRecursion[0]) * 3);
#endif
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
    for (int i = 0; i < 1000000; ++i) {
        SkPath path, fill;
        SkPoint quad[3];
        quad[0].fX = r.nextRangeF(0, 500);
        quad[0].fY = r.nextRangeF(0, 500);
        const SkScalar halfSquared = 0.5f * 0.5f;
        do {
            quad[1].fX = r.nextRangeF(0, 500);
            quad[1].fY = r.nextRangeF(0, 500);
        } while (quad[0].distanceToSqd(quad[1]) < halfSquared);
        do {
            quad[2].fX = r.nextRangeF(0, 500);
            quad[2].fY = r.nextRangeF(0, 500);
        } while (quad[0].distanceToSqd(quad[2]) < halfSquared
                || quad[1].distanceToSqd(quad[2]) < halfSquared);
        path.moveTo(quad[0].fX, quad[0].fY);
        path.quadTo(quad[1].fX, quad[1].fY, quad[2].fX, quad[2].fY);
        p.setStrokeWidth(r.nextRangeF(0, 500));
        p.getFillPath(path, &fill);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
        if (best < gMaxRecursion[2]) {
            if (FLAGS_veryVerbose) {
                SkDebugf("\n%s quad=%d width=%1.9g\n", __FUNCTION__, gMaxRecursion[2],
                        p.getStrokeWidth());
                path.dumpHex();
                SkDebugf("fill:\n");
                fill.dumpHex();
            }
            best = gMaxRecursion[2];
        }
#endif
        if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
            return;
        }
    }
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    if (FLAGS_veryVerbose) {
        SkDebugf("\n%s max quad=%d\n", __FUNCTION__, best);
    }
#endif
}

DEF_TEST(CubicStrokerConstrained, reporter) {
    SkRandom r;
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    int bestTan = 0;
    int bestCubic = 0;
    sk_bzero(gMaxRecursion, sizeof(gMaxRecursion[0]) * 3);
#endif
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
    for (int i = 0; i < 1000000; ++i) {
        SkPath path, fill;
        SkPoint cubic[4];
        cubic[0].fX = r.nextRangeF(0, 500);
        cubic[0].fY = r.nextRangeF(0, 500);
        const SkScalar halfSquared = 0.5f * 0.5f;
        do {
            cubic[1].fX = r.nextRangeF(0, 500);
            cubic[1].fY = r.nextRangeF(0, 500);
        } while (cubic[0].distanceToSqd(cubic[1]) < halfSquared);
        do {
            cubic[2].fX = r.nextRangeF(0, 500);
            cubic[2].fY = r.nextRangeF(0, 500);
        } while (  cubic[0].distanceToSqd(cubic[2]) < halfSquared
                || cubic[1].distanceToSqd(cubic[2]) < halfSquared);
        do {
            cubic[3].fX = r.nextRangeF(0, 500);
            cubic[3].fY = r.nextRangeF(0, 500);
        } while (  cubic[0].distanceToSqd(cubic[3]) < halfSquared
                || cubic[1].distanceToSqd(cubic[3]) < halfSquared
                || cubic[2].distanceToSqd(cubic[3]) < halfSquared);
        path.moveTo(cubic[0].fX, cubic[0].fY);
        path.cubicTo(cubic[1].fX, cubic[1].fY, cubic[2].fX, cubic[2].fY, cubic[3].fX, cubic[3].fY);
        p.setStrokeWidth(r.nextRangeF(0, 500));
        p.getFillPath(path, &fill);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
        if (bestTan < gMaxRecursion[0] || bestCubic < gMaxRecursion[1]) {
            if (FLAGS_veryVerbose) {
                SkDebugf("\n%s tan=%d cubic=%d width=%1.9g\n", __FUNCTION__, gMaxRecursion[0],
                        gMaxRecursion[1], p.getStrokeWidth());
                path.dumpHex();
                SkDebugf("fill:\n");
                fill.dumpHex();
            }
            bestTan = SkTMax(bestTan, gMaxRecursion[0]);
            bestCubic = SkTMax(bestCubic, gMaxRecursion[1]);
        }
#endif
        if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
            return;
        }
    }
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    if (FLAGS_veryVerbose) {
        SkDebugf("\n%s max tan=%d cubic=%d\n", __FUNCTION__, bestTan, bestCubic);
    }
#endif
}

DEF_TEST(QuadStrokerRange, reporter) {
    SkRandom r;
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    int best = 0;
    sk_bzero(gMaxRecursion, sizeof(gMaxRecursion[0]) * 3);
#endif
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
    for (int i = 0; i < 1000000; ++i) {
        SkPath path, fill;
        SkPoint quad[3];
        quad[0].fX = r.nextRangeF(0, 500);
        quad[0].fY = r.nextRangeF(0, 500);
        quad[1].fX = r.nextRangeF(0, 500);
        quad[1].fY = r.nextRangeF(0, 500);
        quad[2].fX = r.nextRangeF(0, 500);
        quad[2].fY = r.nextRangeF(0, 500);
        path.moveTo(quad[0].fX, quad[0].fY);
        path.quadTo(quad[1].fX, quad[1].fY, quad[2].fX, quad[2].fY);
        p.setStrokeWidth(r.nextRangeF(0, 500));
        p.getFillPath(path, &fill);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
        if (best < gMaxRecursion[2]) {
            if (FLAGS_veryVerbose) {
                SkDebugf("\n%s quad=%d width=%1.9g\n", __FUNCTION__, gMaxRecursion[2],
                        p.getStrokeWidth());
                path.dumpHex();
                SkDebugf("fill:\n");
                fill.dumpHex();
            }
            best = gMaxRecursion[2];
        }
#endif
        if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
            return;
        }
    }
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    if (FLAGS_verbose) {
        SkDebugf("\n%s max quad=%d\n", __FUNCTION__, best);
    }
#endif
}

DEF_TEST(CubicStrokerRange, reporter) {
    SkRandom r;
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    int best[2] = { 0 };
    sk_bzero(gMaxRecursion, sizeof(gMaxRecursion[0]) * 3);
#endif
    SkMSec limit = SkTime::GetMSecs() + MS_TEST_DURATION;
    for (int i = 0; i < 1000000; ++i) {
        SkPath path, fill;
        path.moveTo(r.nextRangeF(0, 500), r.nextRangeF(0, 500));
        path.cubicTo(r.nextRangeF(0, 500), r.nextRangeF(0, 500), r.nextRangeF(0, 500),
                r.nextRangeF(0, 500), r.nextRangeF(0, 500), r.nextRangeF(0, 500));
        p.setStrokeWidth(r.nextRangeF(0, 100));
        p.getFillPath(path, &fill);
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
        if (best[0] < gMaxRecursion[0] || best[1] < gMaxRecursion[1]) {
            if (FLAGS_veryVerbose) {
                SkDebugf("\n%s tan=%d cubic=%d width=%1.9g\n", __FUNCTION__, gMaxRecursion[0],
                        gMaxRecursion[1], p.getStrokeWidth());
                path.dumpHex();
                SkDebugf("fill:\n");
                fill.dumpHex();
            }
            best[0] = SkTMax(best[0], gMaxRecursion[0]);
            best[1] = SkTMax(best[1], gMaxRecursion[1]);
        }
#endif
        if (FLAGS_timeout && SkTime::GetMSecs() > limit) {
            return;
        }
    }
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    if (FLAGS_veryVerbose) {
        SkDebugf("\n%s max tan=%d cubic=%d\n", __FUNCTION__, best[0], best[1]);
    }
#endif
}


DEF_TEST(QuadStrokerOneOff, reporter) {
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    sk_bzero(gMaxRecursion, sizeof(gMaxRecursion[0]) * 3);
#endif
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
    p.setStrokeWidth(SkDoubleToScalar(164.683548));

    SkPath path, fill;
path.moveTo(SkBits2Float(0x43c99223), SkBits2Float(0x42b7417e));
path.quadTo(SkBits2Float(0x4285d839), SkBits2Float(0x43ed6645), SkBits2Float(0x43c941c8), SkBits2Float(0x42b3ace3));
    p.getFillPath(path, &fill);
    if (FLAGS_veryVerbose) {
        SkDebugf("\n%s path\n", __FUNCTION__);
        path.dump();
        SkDebugf("fill:\n");
        fill.dump();
    }
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    if (FLAGS_veryVerbose) {
        SkDebugf("max quad=%d\n", gMaxRecursion[2]);
    }
#endif
}

DEF_TEST(CubicStrokerOneOff, reporter) {
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    sk_bzero(gMaxRecursion, sizeof(gMaxRecursion[0]) * 3);
#endif
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
    p.setStrokeWidth(SkDoubleToScalar(42.835968));

    SkPath path, fill;
path.moveTo(SkBits2Float(0x433f5370), SkBits2Float(0x43d1f4b3));
path.cubicTo(SkBits2Float(0x4331cb76), SkBits2Float(0x43ea3340), SkBits2Float(0x4388f498), SkBits2Float(0x42f7f08d), SkBits2Float(0x43f1cd32), SkBits2Float(0x42802ec1));
    p.getFillPath(path, &fill);
    if (FLAGS_veryVerbose) {
        SkDebugf("\n%s path\n", __FUNCTION__);
        path.dump();
        SkDebugf("fill:\n");
        fill.dump();
    }
#if defined(SK_DEBUG) && QUAD_STROKE_APPROXIMATION
    if (FLAGS_veryVerbose) {
        SkDebugf("max tan=%d cubic=%d\n", gMaxRecursion[0], gMaxRecursion[1]);
    }
#endif
}