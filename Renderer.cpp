#include "Renderer.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

Renderer::Renderer() {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init(HWND hwnd, int width, int height) {
    m_hwnd = hwnd;
    m_width = width;
    m_height = height;

    HDC hdcWindow = GetDC(hwnd);
    m_hdcBack = CreateCompatibleDC(hdcWindow);
    m_hbmBack = CreateCompatibleBitmap(hdcWindow, m_width, m_height);
    m_hbmOld = (HBITMAP)SelectObject(m_hdcBack, m_hbmBack);
    ReleaseDC(hwnd, hdcWindow);

    return true;
}

void Renderer::resize(int width, int height) {
    if (width <= 0 || height <= 0) return;
    if (m_width == width && m_height == height) return;

    m_width = width;
    m_height = height;

    if (m_hdcBack) {
        SelectObject(m_hdcBack, m_hbmOld);
        DeleteObject(m_hbmBack);

        HDC hdcWindow = GetDC(m_hwnd);
        m_hbmBack = CreateCompatibleBitmap(hdcWindow, m_width, m_height);
        m_hbmOld = (HBITMAP)SelectObject(m_hdcBack, m_hbmBack);
        ReleaseDC(m_hwnd, hdcWindow);
    }
}

void Renderer::shutdown() {
    if (m_hdcBack) {
        SelectObject(m_hdcBack, m_hbmOld);
        DeleteObject(m_hbmBack);
        DeleteDC(m_hdcBack);
        m_hdcBack = nullptr;
    }
}

void Renderer::beginFrame() {
    m_animTick += 0.016f;
    if (m_animTick > 1000.0f) m_animTick = 0.0f;
}

void Renderer::endFrame(HDC destHdc) {
    BitBlt(destHdc, 0, 0, m_width, m_height, m_hdcBack, 0, 0, SRCCOPY);
}

static POINT rotatePoint(float cx, float cy, float lx, float ly, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    POINT p;
    p.x = (LONG)(cx + lx * c + ly * s);
    p.y = (LONG)(cy + lx * s - ly * c);
    return p;
}

static void drawRotatedQuad(HDC hdc, float cx, float cy, float lx1, float ly1, float lx2, float ly2, float angle, COLORREF fillCol, COLORREF strokeCol = 0, int strokeWidth = 0) {
    HBRUSH b = CreateSolidBrush(fillCol);
    HPEN p = (strokeWidth > 0) ? CreatePen(PS_SOLID, strokeWidth, strokeCol) : CreatePen(PS_NULL, 0, 0);
    HGDIOBJ oB = SelectObject(hdc, b);
    HGDIOBJ oP = SelectObject(hdc, p);

    POINT pts[4] = {
        rotatePoint(cx, cy, lx1, ly1, angle),
        rotatePoint(cx, cy, lx2, ly1, angle),
        rotatePoint(cx, cy, lx2, ly2, angle),
        rotatePoint(cx, cy, lx1, ly2, angle)
    };
    Polygon(hdc, pts, 4);

    SelectObject(hdc, oB);
    SelectObject(hdc, oP);
    DeleteObject(b);
    DeleteObject(p);
}

void Renderer::drawHeadlightBeams(const Vec2& screenPos, float angle, float carWidth, float carHeight) {
    float beamLength = 320.0f;
    float beamSpread = 50.0f;

    POINT leftBeam[3];
    POINT rightBeam[3];

    POINT h1 = rotatePoint(screenPos.x, screenPos.y, -carWidth * 0.35f, carHeight * 0.45f, angle);
    POINT lEnd1 = rotatePoint(screenPos.x, screenPos.y, -carWidth * 0.35f - beamSpread, carHeight * 0.45f + beamLength, angle);
    POINT lEnd2 = rotatePoint(screenPos.x, screenPos.y, -carWidth * 0.35f + beamSpread * 0.35f, carHeight * 0.45f + beamLength, angle);
    leftBeam[0] = h1; leftBeam[1] = lEnd1; leftBeam[2] = lEnd2;

    POINT h2 = rotatePoint(screenPos.x, screenPos.y, carWidth * 0.35f, carHeight * 0.45f, angle);
    POINT rEnd1 = rotatePoint(screenPos.x, screenPos.y, carWidth * 0.35f - beamSpread * 0.35f, carHeight * 0.45f + beamLength, angle);
    POINT rEnd2 = rotatePoint(screenPos.x, screenPos.y, carWidth * 0.35f + beamSpread, carHeight * 0.45f + beamLength, angle);
    rightBeam[0] = h2; rightBeam[1] = rEnd1; rightBeam[2] = rEnd2;

    HBRUSH beamBrush = CreateSolidBrush(RGB(240, 248, 255));
    HPEN nullPen = CreatePen(PS_NULL, 0, 0);
    HGDIOBJ oldB = SelectObject(m_hdcBack, beamBrush);
    HGDIOBJ oldP = SelectObject(m_hdcBack, nullPen);

    Polygon(m_hdcBack, leftBeam, 3);
    Polygon(m_hdcBack, rightBeam, 3);

    SelectObject(m_hdcBack, oldB);
    SelectObject(m_hdcBack, oldP);
    DeleteObject(beamBrush);
    DeleteObject(nullPen);
}

// -------------------------------------------------------------
// 1. APEX FALCON (TYPE 0: SLEEK AERODYNAMIC SUPERCAR)
// -------------------------------------------------------------
void Renderer::drawSupercar(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor, const ColorRGB& underglowColor, bool brakeLights, bool drawUnderglow) {
    float cx = screenPos.x;
    float cy = screenPos.y;
    float hw = width * 0.5f;
    float hh = height * 0.5f;

    // Underglow Neon
    if (drawUnderglow) {
        drawRotatedQuad(m_hdcBack, cx, cy, -hw - 10, -hh - 10, hw + 10, hh + 10, angle, underglowColor.toCOLORREF());
    }

    // Shadow
    drawRotatedQuad(m_hdcBack, cx + 5, cy + 5, -hw - 1, -hh - 1, hw + 1, hh + 1, angle, RGB(12, 14, 20));

    // 4 Wide Alloy Wheels with Red Brake Calipers
    float tw = 8.0f, th = 17.0f;
    const float wheels[4][2] = {
        { -hw - 2.0f, hh * 0.52f }, { hw + 2.0f, hh * 0.52f },
        { -hw - 2.0f, -hh * 0.55f }, { hw + 2.0f, -hh * 0.55f }
    };
    for (int w = 0; w < 4; ++w) {
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - tw * 0.5f, wheels[w][1] - th * 0.5f, wheels[w][0] + tw * 0.5f, wheels[w][1] + th * 0.5f, angle, RGB(22, 22, 26), RGB(10, 10, 12), 1);
        // Alloy rim center
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - 2, wheels[w][1] - 4, wheels[w][0] + 2, wheels[w][1] + 4, angle, RGB(200, 205, 215));
        // Red brake caliper
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - 1, wheels[w][1] - 1, wheels[w][0] + 1, wheels[w][1] + 2, angle, RGB(255, 30, 30));
    }

    // Carbon Front Splitter
    drawRotatedQuad(m_hdcBack, cx, cy, -hw - 2, hh * 0.95f, hw + 2, hh + 3, angle, RGB(20, 20, 25), RGB(0, 0, 0), 1);

    // Aerodynamic Coke-Bottle Chassis (Narrow waist, sculpted fenders)
    HBRUSH bodyBrush = CreateSolidBrush(bodyColor.toCOLORREF());
    HPEN bodyPen = CreatePen(PS_SOLID, 2, RGB(15, 15, 20));
    HGDIOBJ oB = SelectObject(m_hdcBack, bodyBrush);
    HGDIOBJ oP = SelectObject(m_hdcBack, bodyPen);

    POINT bodyPts[10] = {
        rotatePoint(cx, cy, -hw + 6, hh, angle),          // Nose Left
        rotatePoint(cx, cy, hw - 6, hh, angle),           // Nose Right
        rotatePoint(cx, cy, hw + 1, hh * 0.72f, angle),   // Front Right Fender
        rotatePoint(cx, cy, hw - 4, 0.0f, angle),         // Sculpted Waist Right
        rotatePoint(cx, cy, hw + 2, -hh * 0.65f, angle),  // Rear Right Fender
        rotatePoint(cx, cy, hw - 3, -hh, angle),          // Rear Right Tail
        rotatePoint(cx, cy, -hw + 3, -hh, angle),         // Rear Left Tail
        rotatePoint(cx, cy, -hw - 2, -hh * 0.65f, angle), // Rear Left Fender
        rotatePoint(cx, cy, -hw + 4, 0.0f, angle),        // Sculpted Waist Left
        rotatePoint(cx, cy, -hw - 1, hh * 0.72f, angle)   // Front Left Fender
    };
    Polygon(m_hdcBack, bodyPts, 10);

    // Carbon Hood Scoop Vents
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.45f, hh * 0.45f, -hw * 0.15f, hh * 0.65f, angle, RGB(24, 26, 32));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.15f, hh * 0.45f, hw * 0.45f, hh * 0.65f, angle, RGB(24, 26, 32));

    // Side Mirrors
    drawRotatedQuad(m_hdcBack, cx, cy, -hw - 4, hh * 0.32f, -hw + 1, hh * 0.22f, angle, bodyColor.toCOLORREF(), RGB(0, 0, 0), 1);
    drawRotatedQuad(m_hdcBack, cx, cy, hw - 1, hh * 0.32f, hw + 4, hh * 0.22f, angle, bodyColor.toCOLORREF(), RGB(0, 0, 0), 1);

    // Glossy Tinted Glass Cockpit (Front Windshield, Side Windows, Rear Engine Bay)
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.72f, hh * 0.38f, hw * 0.72f, hh * 0.12f, angle, RGB(18, 30, 48), RGB(10, 18, 30), 1);
    // Sun glare reflection on windshield
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.5f, hh * 0.34f, -hw * 0.2f, hh * 0.16f, angle, RGB(160, 210, 255));

    // Roof & Rear Engine Louvers
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.65f, hh * 0.12f, hw * 0.65f, -hh * 0.25f, angle, RGB((int)(bodyColor.r * 0.85f), (int)(bodyColor.g * 0.85f), (int)(bodyColor.b * 0.85f)));
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.55f, -hh * 0.28f, hw * 0.55f, -hh * 0.62f, angle, RGB(18, 22, 30));

    // GT Dual-Tier Carbon Wing Spoiler with Endplates
    drawRotatedQuad(m_hdcBack, cx, cy, -hw - 4, -hh + 2, hw + 4, -hh - 5, angle, RGB(18, 18, 22), bodyColor.toCOLORREF(), 2);
    // Endplates
    drawRotatedQuad(m_hdcBack, cx, cy, -hw - 5, -hh + 5, -hw - 3, -hh - 7, angle, RGB(255, 255, 255));
    drawRotatedQuad(m_hdcBack, cx, cy, hw + 3, -hh + 5, hw + 5, -hh - 7, angle, RGB(255, 255, 255));

    // Projector LED Headlights
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.75f, hh * 0.85f, -hw * 0.35f, hh * 0.98f, angle, RGB(0, 240, 255));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.35f, hh * 0.85f, hw * 0.75f, hh * 0.98f, angle, RGB(0, 240, 255));

    // Taillights / Brake Light Strip
    COLORREF tailColor = brakeLights ? RGB(255, 25, 25) : RGB(190, 15, 15);
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.85f, -hh + 1, hw * 0.85f, -hh + 5, angle, tailColor, brakeLights ? RGB(255, 160, 160) : RGB(255, 50, 50), 1);

    SelectObject(m_hdcBack, oB);
    SelectObject(m_hdcBack, oP);
    DeleteObject(bodyBrush);
    DeleteObject(bodyPen);
}

// -------------------------------------------------------------
// 2. VIPER GT (TYPE 1: MUSCULAR AMERICAN V8 WITH RACING STRIPES)
// -------------------------------------------------------------
void Renderer::drawMuscleCar(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor, const ColorRGB& underglowColor, bool brakeLights, bool drawUnderglow) {
    float cx = screenPos.x;
    float cy = screenPos.y;
    float hw = width * 0.5f;
    float hh = height * 0.5f;

    if (drawUnderglow) {
        drawRotatedQuad(m_hdcBack, cx, cy, -hw - 10, -hh - 10, hw + 10, hh + 10, angle, underglowColor.toCOLORREF());
    }

    // Shadow
    drawRotatedQuad(m_hdcBack, cx + 5, cy + 5, -hw - 1, -hh - 1, hw + 1, hh + 1, angle, RGB(12, 14, 20));

    // Wide Drag Radial Wheels
    float tw = 9.0f, th = 18.0f;
    const float wheels[4][2] = {
        { -hw - 2.0f, hh * 0.5f }, { hw + 2.0f, hh * 0.5f },
        { -hw - 3.0f, -hh * 0.55f }, { hw + 3.0f, -hh * 0.55f }
    };
    for (int w = 0; w < 4; ++w) {
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - tw * 0.5f, wheels[w][1] - th * 0.5f, wheels[w][0] + tw * 0.5f, wheels[w][1] + th * 0.5f, angle, RGB(25, 25, 30), RGB(0, 0, 0), 1);
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - 2, wheels[w][1] - 4, wheels[w][0] + 2, wheels[w][1] + 4, angle, RGB(220, 220, 230));
    }

    // Muscular Widebody Chassis (Flared Wheel Arches, Broad Nose)
    HBRUSH bodyBrush = CreateSolidBrush(bodyColor.toCOLORREF());
    HPEN bodyPen = CreatePen(PS_SOLID, 2, RGB(15, 15, 20));
    HGDIOBJ oB = SelectObject(m_hdcBack, bodyBrush);
    HGDIOBJ oP = SelectObject(m_hdcBack, bodyPen);

    POINT bodyPts[8] = {
        rotatePoint(cx, cy, -hw + 3, hh, angle),          // Front Left
        rotatePoint(cx, cy, hw - 3, hh, angle),           // Front Right
        rotatePoint(cx, cy, hw + 2, hh * 0.45f, angle),   // Flared Front Fender
        rotatePoint(cx, cy, hw + 3, -hh * 0.75f, angle),  // Flared Rear Fender
        rotatePoint(cx, cy, hw - 2, -hh, angle),          // Rear Right Tail
        rotatePoint(cx, cy, -hw + 2, -hh, angle),         // Rear Left Tail
        rotatePoint(cx, cy, -hw - 3, -hh * 0.75f, angle), // Flared Rear Fender
        rotatePoint(cx, cy, -hw - 2, hh * 0.45f, angle)   // Flared Front Fender
    };
    Polygon(m_hdcBack, bodyPts, 8);

    // Dual White Racing Stripes Down the Center
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.35f, -hh, -hw * 0.12f, hh, angle, RGB(250, 250, 255));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.12f, -hh, hw * 0.35f, hh, angle, RGB(250, 250, 255));

    // Heavy Power Hood Scoop / Blower
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.25f, hh * 0.4f, hw * 0.25f, hh * 0.7f, angle, RGB(20, 20, 25), RGB(0, 0, 0), 1);

    // Fastback Cockpit & Rear Window
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.75f, hh * 0.35f, hw * 0.75f, hh * 0.1f, angle, RGB(16, 26, 42));
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.72f, -hh * 0.22f, hw * 0.72f, -hh * 0.65f, angle, RGB(16, 26, 42));

    // Ducktail Trunk Spoiler
    drawRotatedQuad(m_hdcBack, cx, cy, -hw - 1, -hh + 3, hw + 1, -hh - 3, angle, RGB(20, 20, 25), RGB(0, 0, 0), 1);

    // Aggressive Dual Round Headlights & Taillights
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.8f, hh * 0.88f, -hw * 0.45f, hh * 0.98f, angle, RGB(255, 235, 140));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.45f, hh * 0.88f, hw * 0.8f, hh * 0.98f, angle, RGB(255, 235, 140));

    COLORREF tailCol = brakeLights ? RGB(255, 20, 20) : RGB(180, 10, 10);
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.78f, -hh + 1, -hw * 0.38f, -hh + 5, angle, tailCol);
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.38f, -hh + 1, hw * 0.78f, -hh + 5, angle, tailCol);

    SelectObject(m_hdcBack, oB);
    SelectObject(m_hdcBack, oP);
    DeleteObject(bodyBrush);
    DeleteObject(bodyPen);
}

// -------------------------------------------------------------
// 3. CYBER PHANTOM (TYPE 2: HYPERCAR WITH LE MANS SHARK FIN)
// -------------------------------------------------------------
void Renderer::drawHypercar(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor, const ColorRGB& underglowColor, bool brakeLights, bool drawUnderglow) {
    float cx = screenPos.x;
    float cy = screenPos.y;
    float hw = width * 0.5f;
    float hh = height * 0.5f;

    if (drawUnderglow) {
        drawRotatedQuad(m_hdcBack, cx, cy, -hw - 12, -hh - 12, hw + 12, hh + 12, angle, underglowColor.toCOLORREF());
    }

    drawRotatedQuad(m_hdcBack, cx + 6, cy + 6, -hw - 2, -hh - 2, hw + 2, hh + 2, angle, RGB(10, 12, 18));

    // Wheels
    float tw = 8.0f, th = 17.0f;
    const float wheels[4][2] = {
        { -hw - 2.0f, hh * 0.55f }, { hw + 2.0f, hh * 0.55f },
        { -hw - 2.0f, -hh * 0.55f }, { hw + 2.0f, -hh * 0.55f }
    };
    for (int w = 0; w < 4; ++w) {
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - tw * 0.5f, wheels[w][1] - th * 0.5f, wheels[w][0] + tw * 0.5f, wheels[w][1] + th * 0.5f, angle, RGB(20, 20, 24), RGB(0, 240, 255), 1);
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - 1, wheels[w][1] - 1, wheels[w][0] + 1, wheels[w][1] + 2, angle, RGB(0, 240, 255));
    }

    // Le Mans Prototype Hypercar Body (Aerodynamic pontoons & center spine)
    HBRUSH bodyBrush = CreateSolidBrush(bodyColor.toCOLORREF());
    HPEN bodyPen = CreatePen(PS_SOLID, 2, RGB(15, 20, 28));
    HGDIOBJ oB = SelectObject(m_hdcBack, bodyBrush);
    HGDIOBJ oP = SelectObject(m_hdcBack, bodyPen);

    POINT bodyPts[10] = {
        rotatePoint(cx, cy, -hw + 8, hh, angle),          // Nose Center Left
        rotatePoint(cx, cy, hw - 8, hh, angle),           // Nose Center Right
        rotatePoint(cx, cy, hw + 2, hh * 0.8f, angle),    // Front Aero Fender
        rotatePoint(cx, cy, hw - 3, hh * 0.1f, angle),    // Side Air Channel
        rotatePoint(cx, cy, hw + 3, -hh * 0.7f, angle),   // Rear Wide Pontoon
        rotatePoint(cx, cy, hw - 4, -hh, angle),          // Rear Right Diffuser
        rotatePoint(cx, cy, -hw + 4, -hh, angle),         // Rear Left Diffuser
        rotatePoint(cx, cy, -hw - 3, -hh * 0.7f, angle),  // Rear Wide Pontoon
        rotatePoint(cx, cy, -hw + 3, hh * 0.1f, angle),   // Side Air Channel
        rotatePoint(cx, cy, -hw - 2, hh * 0.8f, angle)    // Front Aero Fender
    };
    Polygon(m_hdcBack, bodyPts, 10);

    // Front Nose Aero Cutouts / Air Tunnels
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.4f, hh * 0.55f, -hw * 0.1f, hh * 0.88f, angle, RGB(15, 18, 24));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.1f, hh * 0.55f, hw * 0.4f, hh * 0.88f, angle, RGB(15, 18, 24));

    // Teardrop Glass Cockpit Canopy
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.65f, hh * 0.4f, hw * 0.65f, -hh * 0.2f, angle, RGB(12, 28, 48), RGB(0, 240, 255), 1);
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.35f, hh * 0.35f, -hw * 0.1f, hh * 0.1f, angle, RGB(140, 220, 255));

    // LE MANS CENTER SHARK FIN (Aerodynamic Spine)
    drawRotatedQuad(m_hdcBack, cx, cy, -2.5f, hh * 0.1f, 2.5f, -hh * 0.85f, angle, RGB(240, 245, 255), RGB(0, 0, 0), 1);

    // Swan-Neck Mounted Carbon Aero Wing
    drawRotatedQuad(m_hdcBack, cx, cy, -hw - 4, -hh + 3, hw + 4, -hh - 5, angle, RGB(15, 15, 20), RGB(0, 240, 255), 2);

    // Modern Continuous LED Headlight Blade
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.85f, hh * 0.92f, hw * 0.85f, hh * 0.98f, angle, RGB(0, 240, 255));

    // Continuous Full-Width LED Brake Light Blade
    COLORREF tailCol = brakeLights ? RGB(255, 30, 30) : RGB(200, 10, 10);
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.9f, -hh + 1, hw * 0.9f, -hh + 5, angle, tailCol, brakeLights ? RGB(255, 180, 180) : RGB(255, 40, 40), 1);

    SelectObject(m_hdcBack, oB);
    SelectObject(m_hdcBack, oP);
    DeleteObject(bodyBrush);
    DeleteObject(bodyPen);
}

// -------------------------------------------------------------
// 4. TITAN ENFORCER (TYPE 3: HEAVY ARMORED GT)
// -------------------------------------------------------------
void Renderer::drawTitanEnforcer(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor, const ColorRGB& underglowColor, bool brakeLights, bool drawUnderglow) {
    float cx = screenPos.x;
    float cy = screenPos.y;
    float hw = width * 0.5f;
    float hh = height * 0.5f;

    if (drawUnderglow) {
        drawRotatedQuad(m_hdcBack, cx, cy, -hw - 10, -hh - 10, hw + 10, hh + 10, angle, underglowColor.toCOLORREF());
    }

    drawRotatedQuad(m_hdcBack, cx + 5, cy + 5, -hw - 1, -hh - 1, hw + 1, hh + 1, angle, RGB(12, 14, 20));

    // Heavy Tread Wheels
    float tw = 9.0f, th = 18.0f;
    const float wheels[4][2] = {
        { -hw - 2.0f, hh * 0.55f }, { hw + 2.0f, hh * 0.55f },
        { -hw - 2.0f, -hh * 0.55f }, { hw + 2.0f, -hh * 0.55f }
    };
    for (int w = 0; w < 4; ++w) {
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - tw * 0.5f, wheels[w][1] - th * 0.5f, wheels[w][0] + tw * 0.5f, wheels[w][1] + th * 0.5f, angle, RGB(25, 25, 30), RGB(0, 0, 0), 1);
    }

    // Heavy Armored GT Body
    HBRUSH bodyBrush = CreateSolidBrush(bodyColor.toCOLORREF());
    HPEN bodyPen = CreatePen(PS_SOLID, 3, RGB(18, 18, 22));
    HGDIOBJ oB = SelectObject(m_hdcBack, bodyBrush);
    HGDIOBJ oP = SelectObject(m_hdcBack, bodyPen);

    drawRotatedQuad(m_hdcBack, cx, cy, -hw, -hh, hw, hh, angle, bodyColor.toCOLORREF(), RGB(18, 18, 22), 2);

    // Heavy Steel Bull-Bar Push Bumper on Nose
    drawRotatedQuad(m_hdcBack, cx, cy, -hw - 2, hh * 0.92f, hw + 2, hh + 6, angle, RGB(40, 42, 50), RGB(200, 200, 210), 2);
    // Vertical Push Bars
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.4f, hh * 0.85f, -hw * 0.25f, hh + 7, angle, RGB(60, 65, 75));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.25f, hh * 0.85f, hw * 0.4f, hh + 7, angle, RGB(60, 65, 75));

    // Armored Cockpit & Roof Rails
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.78f, hh * 0.35f, hw * 0.78f, -hh * 0.45f, angle, RGB(18, 26, 38), RGB(0, 0, 0), 1);
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.72f, hh * 0.05f, -hw * 0.62f, -hh * 0.4f, angle, RGB(210, 215, 225));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.62f, hh * 0.05f, hw * 0.72f, -hh * 0.4f, angle, RGB(210, 215, 225));

    // Heavy Headlights & Taillights
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.85f, hh * 0.8f, -hw * 0.5f, hh * 0.95f, angle, RGB(255, 240, 180));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.5f, hh * 0.8f, hw * 0.85f, hh * 0.95f, angle, RGB(255, 240, 180));

    COLORREF tailCol = brakeLights ? RGB(255, 30, 30) : RGB(180, 20, 20);
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.85f, -hh + 2, -hw * 0.4f, -hh + 7, angle, tailCol);
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.4f, -hh + 2, hw * 0.85f, -hh + 7, angle, tailCol);

    SelectObject(m_hdcBack, oB);
    SelectObject(m_hdcBack, oP);
    DeleteObject(bodyBrush);
    DeleteObject(bodyPen);
}

// -------------------------------------------------------------
// 5. POLICE PURSUIT CRUISER (BLACK & WHITE WITH STROBES)
// -------------------------------------------------------------
void Renderer::drawPoliceCruiser(const Vec2& screenPos, float angle, float width, float height, float sirenTimer) {
    float cx = screenPos.x;
    float cy = screenPos.y;
    float hw = width * 0.5f;
    float hh = height * 0.5f;

    drawRotatedQuad(m_hdcBack, cx + 5, cy + 5, -hw - 1, -hh - 1, hw + 1, hh + 1, angle, RGB(10, 12, 18));

    // Wheels
    float tw = 8.0f, th = 17.0f;
    const float wheels[4][2] = {
        { -hw - 2.0f, hh * 0.52f }, { hw + 2.0f, hh * 0.52f },
        { -hw - 2.0f, -hh * 0.55f }, { hw + 2.0f, -hh * 0.55f }
    };
    for (int w = 0; w < 4; ++w) {
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - tw * 0.5f, wheels[w][1] - th * 0.5f, wheels[w][0] + tw * 0.5f, wheels[w][1] + th * 0.5f, angle, RGB(25, 25, 30), RGB(0, 0, 0), 1);
    }

    // Black & White Pursuit Body
    // Black Base Body
    drawRotatedQuad(m_hdcBack, cx, cy, -hw, -hh, hw, hh, angle, RGB(20, 22, 28), RGB(0, 0, 0), 2);
    // White Roof & Doors Center Section
    drawRotatedQuad(m_hdcBack, cx, cy, -hw + 2, -hh * 0.35f, hw - 2, hh * 0.35f, angle, RGB(245, 245, 250));

    // Front Push Bumper / Ram Bar
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.8f, hh * 0.95f, hw * 0.8f, hh + 5, angle, RGB(40, 42, 50), RGB(200, 200, 210), 1);

    // Tinted Cockpit Windows
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.72f, hh * 0.38f, hw * 0.72f, hh * 0.15f, angle, RGB(18, 28, 42));
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.72f, -hh * 0.15f, hw * 0.72f, -hh * 0.42f, angle, RGB(18, 28, 42));

    // FLASHING HIGH-INTENSITY ROOFTOP STROBE LIGHTBAR
    bool flashState = (std::fmod(sirenTimer, 2.0f) < 1.0f);
    COLORREF redLight = flashState ? RGB(255, 0, 0) : RGB(80, 0, 0);
    COLORREF blueLight = !flashState ? RGB(0, 200, 255) : RGB(0, 30, 80);

    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.65f, -hh * 0.08f, -1.0f, hh * 0.08f, angle, redLight, RGB(255, 255, 255), 1);
    drawRotatedQuad(m_hdcBack, cx, cy, 1.0f, -hh * 0.08f, hw * 0.65f, hh * 0.08f, angle, blueLight, RGB(255, 255, 255), 1);

    // Front Headlights & Taillights
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.8f, hh * 0.85f, -hw * 0.45f, hh * 0.98f, angle, RGB(245, 245, 255));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.45f, hh * 0.85f, hw * 0.8f, hh * 0.98f, angle, RGB(245, 245, 255));

    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.8f, -hh + 1, -hw * 0.4f, -hh + 5, angle, RGB(220, 20, 20));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.4f, -hh + 1, hw * 0.8f, -hh + 5, angle, RGB(220, 20, 20));
}

// -------------------------------------------------------------
// 6. SEMI-TRUCK (18-WHEELER FREIGHT CARRIER)
// -------------------------------------------------------------
void Renderer::drawSemiTruck(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& cabColor) {
    float cx = screenPos.x;
    float cy = screenPos.y;
    float hw = width * 0.5f;
    float hh = height * 0.5f;

    drawRotatedQuad(m_hdcBack, cx + 6, cy + 6, -hw - 1, -hh - 1, hw + 1, hh + 1, angle, RGB(12, 14, 20));

    // Multi-Axle Wheels (Front Cab, Trailer Dual Rear Axles)
    float tw = 8.0f, th = 16.0f;
    const float wheels[6][2] = {
        { -hw - 2.0f, hh * 0.8f }, { hw + 2.0f, hh * 0.8f },
        { -hw - 2.0f, -hh * 0.65f }, { hw + 2.0f, -hh * 0.65f },
        { -hw - 2.0f, -hh * 0.88f }, { hw + 2.0f, -hh * 0.88f }
    };
    for (int w = 0; w < 6; ++w) {
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - tw * 0.5f, wheels[w][1] - th * 0.5f, wheels[w][0] + tw * 0.5f, wheels[w][1] + th * 0.5f, angle, RGB(20, 20, 25));
    }

    // 1. FRONT CABIN (Engine Hood, Chrome Grille, Dual Chrome Exhaust Stacks)
    drawRotatedQuad(m_hdcBack, cx, cy, -hw + 1, hh * 0.45f, hw - 1, hh, angle, cabColor.toCOLORREF(), RGB(15, 15, 20), 2);
    // Chrome Grille & Bumper
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.65f, hh * 0.92f, hw * 0.65f, hh + 4, angle, RGB(225, 230, 240), RGB(0, 0, 0), 1);
    // Dual Vertical Chrome Exhaust Stacks
    drawRotatedQuad(m_hdcBack, cx, cy, -hw - 3, hh * 0.45f, -hw + 1, hh * 0.65f, angle, RGB(220, 225, 235), RGB(0, 0, 0), 1);
    drawRotatedQuad(m_hdcBack, cx, cy, hw - 1, hh * 0.45f, hw + 3, hh * 0.65f, angle, RGB(220, 225, 235), RGB(0, 0, 0), 1);
    // Windshield
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.75f, hh * 0.72f, hw * 0.75f, hh * 0.52f, angle, RGB(18, 30, 48));

    // 2. CORRUGATED CARGO CONTAINER TRAILER
    drawRotatedQuad(m_hdcBack, cx, cy, -hw - 1, -hh, hw + 1, hh * 0.4f, angle, RGB(210, 215, 225), RGB(40, 45, 55), 2);
    // Corrugated Roof Ribs
    for (float ry = -hh + 15.0f; ry < hh * 0.35f; ry += 18.0f) {
        drawRotatedQuad(m_hdcBack, cx, cy, -hw + 2, ry, hw - 2, ry + 3, angle, RGB(165, 170, 180));
    }
    // Rear Hazard Chevron Stripes
    drawRotatedQuad(m_hdcBack, cx, cy, -hw, -hh, hw, -hh + 6, angle, RGB(255, 215, 0), RGB(20, 20, 20), 1);

    // Headlights & Taillights
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.8f, hh * 0.88f, -hw * 0.5f, hh * 0.98f, angle, RGB(255, 245, 200));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.5f, hh * 0.88f, hw * 0.8f, hh * 0.98f, angle, RGB(255, 245, 200));

    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.85f, -hh + 1, -hw * 0.5f, -hh + 5, angle, RGB(230, 25, 25));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.5f, -hh + 1, hw * 0.85f, -hh + 5, angle, RGB(230, 25, 25));
}

// -------------------------------------------------------------
// 7. TRAFFIC SEDAN / COUPE
// -------------------------------------------------------------
void Renderer::drawTrafficSedan(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor) {
    float cx = screenPos.x;
    float cy = screenPos.y;
    float hw = width * 0.5f;
    float hh = height * 0.5f;

    drawRotatedQuad(m_hdcBack, cx + 4, cy + 4, -hw, -hh, hw, hh, angle, RGB(14, 16, 22));

    // Wheels
    float tw = 7.0f, th = 15.0f;
    const float wheels[4][2] = {
        { -hw - 1.5f, hh * 0.55f }, { hw + 1.5f, hh * 0.55f },
        { -hw - 1.5f, -hh * 0.55f }, { hw + 1.5f, -hh * 0.55f }
    };
    for (int w = 0; w < 4; ++w) {
        drawRotatedQuad(m_hdcBack, cx, cy, wheels[w][0] - tw * 0.5f, wheels[w][1] - th * 0.5f, wheels[w][0] + tw * 0.5f, wheels[w][1] + th * 0.5f, angle, RGB(25, 25, 30));
    }

    // Modern Curved Sedan Chassis
    drawRotatedQuad(m_hdcBack, cx, cy, -hw, -hh, hw, hh, angle, bodyColor.toCOLORREF(), RGB(15, 15, 20), 2);

    // Front & Rear Bumpers
    drawRotatedQuad(m_hdcBack, cx, cy, -hw + 2, hh * 0.88f, hw - 2, hh + 2, angle, RGB((int)(bodyColor.r * 0.75f), (int)(bodyColor.g * 0.75f), (int)(bodyColor.b * 0.75f)));
    drawRotatedQuad(m_hdcBack, cx, cy, -hw + 2, -hh - 2, hw - 2, -hh * 0.88f, angle, RGB((int)(bodyColor.r * 0.75f), (int)(bodyColor.g * 0.75f), (int)(bodyColor.b * 0.75f)));

    // Cockpit Windows & Roof
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.72f, hh * 0.35f, hw * 0.72f, hh * 0.12f, angle, RGB(20, 32, 48));
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.68f, hh * 0.12f, hw * 0.68f, -hh * 0.22f, angle, RGB((int)(bodyColor.r * 0.88f), (int)(bodyColor.g * 0.88f), (int)(bodyColor.b * 0.88f)));
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.72f, -hh * 0.22f, hw * 0.72f, -hh * 0.52f, angle, RGB(20, 32, 48));

    // Headlights & Taillights
    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.8f, hh * 0.85f, -hw * 0.4f, hh * 0.98f, angle, RGB(255, 248, 220));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.4f, hh * 0.85f, hw * 0.8f, hh * 0.98f, angle, RGB(255, 248, 220));

    drawRotatedQuad(m_hdcBack, cx, cy, -hw * 0.8f, -hh + 1, -hw * 0.4f, -hh + 5, angle, RGB(210, 20, 20));
    drawRotatedQuad(m_hdcBack, cx, cy, hw * 0.4f, -hh + 1, hw * 0.8f, -hh + 5, angle, RGB(210, 20, 20));
}

void Renderer::drawProp(const WorldProp& prop, const Vec2& cameraPos) {
    if (prop.collected) return;

    float sx = prop.pos.x - cameraPos.x;
    float sy = (float)m_height - (prop.pos.y - cameraPos.y);

    if (sx < -120 || sx > m_width + 120 || sy < -120 || sy > m_height + 120) return;

    if (prop.type == PropType::COIN_PICKUP) {
        float spin = std::abs(std::cos(m_animTick * 6.0f)) * 14.0f + 4.0f;
        HBRUSH cBrush = CreateSolidBrush(RGB(255, 215, 0));
        HPEN cPen = CreatePen(PS_SOLID, 2, RGB(200, 140, 0));
        HGDIOBJ oB = SelectObject(m_hdcBack, cBrush);
        HGDIOBJ oP = SelectObject(m_hdcBack, cPen);
        Ellipse(m_hdcBack, (int)(sx - spin), (int)(sy - 12), (int)(sx + spin), (int)(sy + 12));
        SelectObject(m_hdcBack, oB);
        SelectObject(m_hdcBack, oP);
        DeleteObject(cBrush);
        DeleteObject(cPen);
    } else if (prop.type == PropType::NITRO_PICKUP) {
        HBRUSH nBrush = CreateSolidBrush(RGB(0, 240, 255));
        HPEN nPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 220));
        HGDIOBJ oB = SelectObject(m_hdcBack, nBrush);
        HGDIOBJ oP = SelectObject(m_hdcBack, nPen);
        Rectangle(m_hdcBack, (int)(sx - 8), (int)(sy - 16), (int)(sx + 8), (int)(sy + 16));
        SelectObject(m_hdcBack, oB);
        SelectObject(m_hdcBack, oP);
        DeleteObject(nBrush);
        DeleteObject(nPen);
    } else if (prop.type == PropType::TREE_OAK_LARGE || prop.type == PropType::TREE_OAK_MEDIUM) {
        float rad = (prop.type == PropType::TREE_OAK_LARGE) ? 38.0f * prop.scale : 28.0f * prop.scale;

        HBRUSH shBrush = CreateSolidBrush(RGB(25, 95, 35));
        HPEN nullPen = CreatePen(PS_NULL, 0, 0);
        HGDIOBJ oB = SelectObject(m_hdcBack, shBrush);
        HGDIOBJ oP = SelectObject(m_hdcBack, nullPen);
        Ellipse(m_hdcBack, (int)(sx - rad + 8), (int)(sy - rad + 8), (int)(sx + rad + 14), (int)(sy + rad + 14));

        HBRUSH darkFoliage = CreateSolidBrush(RGB(22, 90, 32));
        SelectObject(m_hdcBack, darkFoliage);
        Ellipse(m_hdcBack, (int)(sx - rad), (int)(sy - rad), (int)(sx + rad), (int)(sy + rad));

        HBRUSH midFoliage = CreateSolidBrush(RGB(48, 158, 54));
        SelectObject(m_hdcBack, midFoliage);
        Ellipse(m_hdcBack, (int)(sx - rad * 0.7f), (int)(sy - rad * 0.85f), (int)(sx + rad * 0.35f), (int)(sy + rad * 0.2f));
        Ellipse(m_hdcBack, (int)(sx - rad * 0.3f), (int)(sy - rad * 0.4f), (int)(sx + rad * 0.75f), (int)(sy + rad * 0.65f));
        Ellipse(m_hdcBack, (int)(sx - rad * 0.85f), (int)(sy - rad * 0.2f), (int)(sx + rad * 0.2f), (int)(sy + rad * 0.85f));

        HBRUSH lightFoliage = CreateSolidBrush(RGB(85, 205, 80));
        SelectObject(m_hdcBack, lightFoliage);
        Ellipse(m_hdcBack, (int)(sx - rad * 0.65f), (int)(sy - rad * 0.75f), (int)(sx + rad * 0.1f), (int)(sy));
        Ellipse(m_hdcBack, (int)(sx - rad * 0.2f), (int)(sy - rad * 0.6f), (int)(sx + rad * 0.45f), (int)(sy + rad * 0.05f));

        HBRUSH trunkBrush = CreateSolidBrush(RGB(110, 75, 40));
        HPEN trunkPen = CreatePen(PS_SOLID, 1, RGB(70, 45, 20));
        SelectObject(m_hdcBack, trunkBrush);
        SelectObject(m_hdcBack, trunkPen);
        Ellipse(m_hdcBack, (int)(sx - 5), (int)(sy - 5), (int)(sx + 5), (int)(sy + 5));

        SelectObject(m_hdcBack, oB);
        SelectObject(m_hdcBack, oP);
        DeleteObject(shBrush);
        DeleteObject(nullPen);
        DeleteObject(darkFoliage);
        DeleteObject(midFoliage);
        DeleteObject(lightFoliage);
        DeleteObject(trunkBrush);
        DeleteObject(trunkPen);
    } else if (prop.type == PropType::TREE_CHERRY_BLOSSOM) {
        float rad = 32.0f * prop.scale;

        HBRUSH shBrush = CreateSolidBrush(RGB(25, 95, 35));
        HPEN nullPen = CreatePen(PS_NULL, 0, 0);
        HGDIOBJ oB = SelectObject(m_hdcBack, shBrush);
        HGDIOBJ oP = SelectObject(m_hdcBack, nullPen);
        Ellipse(m_hdcBack, (int)(sx - rad + 6), (int)(sy - rad + 6), (int)(sx + rad + 10), (int)(sy + rad + 10));

        HBRUSH darkPink = CreateSolidBrush(RGB(220, 100, 150));
        HBRUSH midPink = CreateSolidBrush(RGB(255, 150, 195));
        HBRUSH lightPink = CreateSolidBrush(RGB(255, 205, 230));

        SelectObject(m_hdcBack, darkPink);
        Ellipse(m_hdcBack, (int)(sx - rad), (int)(sy - rad), (int)(sx + rad), (int)(sy + rad));

        SelectObject(m_hdcBack, midPink);
        Ellipse(m_hdcBack, (int)(sx - rad * 0.7f), (int)(sy - rad * 0.8f), (int)(sx + rad * 0.4f), (int)(sy + rad * 0.3f));
        Ellipse(m_hdcBack, (int)(sx - rad * 0.3f), (int)(sy - rad * 0.3f), (int)(sx + rad * 0.8f), (int)(sy + rad * 0.8f));

        SelectObject(m_hdcBack, lightPink);
        Ellipse(m_hdcBack, (int)(sx - rad * 0.5f), (int)(sy - rad * 0.7f), (int)(sx + rad * 0.1f), (int)(sy));

        SelectObject(m_hdcBack, oB);
        SelectObject(m_hdcBack, oP);
        DeleteObject(shBrush);
        DeleteObject(nullPen);
        DeleteObject(darkPink);
        DeleteObject(midPink);
        DeleteObject(lightPink);
    } else if (prop.type == PropType::TREE_PINE) {
        float rad = 26.0f * prop.scale;

        HBRUSH shBrush = CreateSolidBrush(RGB(25, 95, 35));
        HPEN nullPen = CreatePen(PS_NULL, 0, 0);
        HGDIOBJ oB = SelectObject(m_hdcBack, shBrush);
        HGDIOBJ oP = SelectObject(m_hdcBack, nullPen);
        Ellipse(m_hdcBack, (int)(sx - rad + 6), (int)(sy - rad + 6), (int)(sx + rad + 10), (int)(sy + rad + 10));

        HBRUSH pineDark = CreateSolidBrush(RGB(15, 75, 35));
        HBRUSH pineMid = CreateSolidBrush(RGB(30, 125, 55));
        HBRUSH pineLight = CreateSolidBrush(RGB(65, 175, 85));

        SelectObject(m_hdcBack, pineDark);
        Ellipse(m_hdcBack, (int)(sx - rad), (int)(sy - rad), (int)(sx + rad), (int)(sy + rad));

        SelectObject(m_hdcBack, pineMid);
        Ellipse(m_hdcBack, (int)(sx - rad * 0.7f), (int)(sy - rad * 0.7f), (int)(sx + rad * 0.7f), (int)(sy + rad * 0.7f));

        SelectObject(m_hdcBack, pineLight);
        Ellipse(m_hdcBack, (int)(sx - rad * 0.4f), (int)(sy - rad * 0.4f), (int)(sx + rad * 0.4f), (int)(sy + rad * 0.4f));

        SelectObject(m_hdcBack, oB);
        SelectObject(m_hdcBack, oP);
        DeleteObject(shBrush);
        DeleteObject(nullPen);
        DeleteObject(pineDark);
        DeleteObject(pineMid);
        DeleteObject(pineLight);
    } else if (prop.type == PropType::BUSH_FLOWER || prop.type == PropType::BUSH_GREEN) {
        float brad = 16.0f * prop.scale;

        HBRUSH bushBrush = CreateSolidBrush(RGB(45, 160, 55));
        HPEN bushPen = CreatePen(PS_SOLID, 1, RGB(25, 100, 35));
        HGDIOBJ oB = SelectObject(m_hdcBack, bushBrush);
        HGDIOBJ oP = SelectObject(m_hdcBack, bushPen);

        Ellipse(m_hdcBack, (int)(sx - brad), (int)(sy - brad), (int)(sx + brad), (int)(sy + brad));
        Ellipse(m_hdcBack, (int)(sx - brad * 0.6f), (int)(sy - brad * 1.3f), (int)(sx + brad * 0.6f), (int)(sy));

        if (prop.type == PropType::BUSH_FLOWER) {
            HBRUSH flowerBrush = CreateSolidBrush(RGB(255, 225, 50));
            SelectObject(m_hdcBack, flowerBrush);
            Ellipse(m_hdcBack, (int)(sx - 3), (int)(sy - 8), (int)(sx + 3), (int)(sy - 2));
            Ellipse(m_hdcBack, (int)(sx - 8), (int)(sy + 2), (int)(sx - 2), (int)(sy + 8));
            Ellipse(m_hdcBack, (int)(sx + 4), (int)(sy + 1), (int)(sx + 10), (int)(sy + 7));
            DeleteObject(flowerBrush);
        }

        SelectObject(m_hdcBack, oB);
        SelectObject(m_hdcBack, oP);
        DeleteObject(bushBrush);
        DeleteObject(bushPen);
    } else if (prop.type == PropType::ROCK_BOULDER) {
        float rrad = 18.0f * prop.scale;

        HBRUSH rockBrush = CreateSolidBrush(RGB(130, 135, 145));
        HPEN rockPen = CreatePen(PS_SOLID, 2, RGB(80, 85, 95));
        HGDIOBJ oB = SelectObject(m_hdcBack, rockBrush);
        HGDIOBJ oP = SelectObject(m_hdcBack, rockPen);

        Ellipse(m_hdcBack, (int)(sx - rrad), (int)(sy - rrad * 0.8f), (int)(sx + rrad), (int)(sy + rrad * 0.8f));

        HBRUSH hlBrush = CreateSolidBrush(RGB(180, 185, 195));
        SelectObject(m_hdcBack, hlBrush);
        Ellipse(m_hdcBack, (int)(sx - rrad * 0.6f), (int)(sy - rrad * 0.5f), (int)(sx + rrad * 0.1f), (int)(sy));
        DeleteObject(hlBrush);

        SelectObject(m_hdcBack, oB);
        SelectObject(m_hdcBack, oP);
        DeleteObject(rockBrush);
        DeleteObject(rockPen);
    } else if (prop.type == PropType::CHECKPOINT_GATE || prop.type == PropType::FINISH_GATE) {
        bool isFin = (prop.type == PropType::FINISH_GATE);
        HBRUSH gBrush = CreateSolidBrush(RGB(35, 40, 55));
        HPEN gPen = CreatePen(PS_SOLID, 3, isFin ? RGB(255, 215, 0) : RGB(0, 240, 255));
        HGDIOBJ oB = SelectObject(m_hdcBack, gBrush);
        HGDIOBJ oP = SelectObject(m_hdcBack, gPen);

        Rectangle(m_hdcBack, (int)(sx - prop.width * 0.5f), (int)(sy - 14), (int)(sx + prop.width * 0.5f), (int)(sy + 14));

        SetBkMode(m_hdcBack, TRANSPARENT);
        SetTextColor(m_hdcBack, isFin ? RGB(255, 220, 0) : RGB(0, 240, 255));
        HFONT hF = CreateFontA(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
        HGDIOBJ oF = SelectObject(m_hdcBack, hF);

        RECT r = { (int)(sx - prop.width * 0.5f), (int)(sy - 14), (int)(sx + prop.width * 0.5f), (int)(sy + 14) };
        DrawTextA(m_hdcBack, isFin ? "--- FINISH LINE ---" : ">> CHECKPOINT >>", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(m_hdcBack, oF);
        SelectObject(m_hdcBack, oB);
        SelectObject(m_hdcBack, oP);
        DeleteObject(gBrush);
        DeleteObject(gPen);
        DeleteObject(hF);
    }
}

void Renderer::renderWorld(const Road& road, const Car& car, const Traffic& traffic, ParticleSystem& particles, WeatherType weather, float screenShakeX, float screenShakeY) {
    float lookAheadY = car.getSpeed() * 0.22f;
    float targetCamX = car.getPos().x - (float)m_width * 0.5f;
    float targetCamY = car.getPos().y - (float)m_height * 0.68f + lookAheadY;

    // Smooth exponential camera interpolation for buttery smooth panning
    if (m_cameraPos.y < 1.0f) {
        m_cameraPos.x = targetCamX;
        m_cameraPos.y = targetCamY;
    } else {
        m_cameraPos.x += (targetCamX - m_cameraPos.x) * 0.18f;
        m_cameraPos.y += (targetCamY - m_cameraPos.y) * 0.22f;
    }

    Vec2 effCamPos(m_cameraPos.x + screenShakeX, m_cameraPos.y + screenShakeY);

    // 1. FILL NATURE GROUND (Meadow Grass)
    HBRUSH groundBrush = CreateSolidBrush(road.getShoulderColor().toCOLORREF());
    HGDIOBJ oldB = SelectObject(m_hdcBack, groundBrush);
    Rectangle(m_hdcBack, 0, 0, m_width, m_height);
    SelectObject(m_hdcBack, oldB);
    DeleteObject(groundBrush);

    // 2. GRAVEL SHOULDER STRIPS
    float roadWidth = road.getRoadWidth();
    int roadScreenLeft = (int)(-roadWidth * 0.5f - effCamPos.x);
    int roadScreenRight = (int)(roadWidth * 0.5f - effCamPos.x);
    int shoulderWidth = 30;

    HBRUSH gravelBrush = CreateSolidBrush(RGB(115, 110, 95));
    HPEN nullPen = CreatePen(PS_NULL, 0, 0);
    HGDIOBJ oB = SelectObject(m_hdcBack, gravelBrush);
    HGDIOBJ oP = SelectObject(m_hdcBack, nullPen);

    Rectangle(m_hdcBack, roadScreenLeft - shoulderWidth, 0, roadScreenLeft, m_height);
    Rectangle(m_hdcBack, roadScreenRight, 0, roadScreenRight + shoulderWidth, m_height);

    // 3. STRAIGHT ASPHALT HIGHWAY
    HBRUSH asphaltBrush = CreateSolidBrush(road.getAsphaltColor().toCOLORREF());
    SelectObject(m_hdcBack, asphaltBrush);
    Rectangle(m_hdcBack, roadScreenLeft, 0, roadScreenRight, m_height);

    SelectObject(m_hdcBack, oB);
    SelectObject(m_hdcBack, oP);
    DeleteObject(gravelBrush);
    DeleteObject(asphaltBrush);
    DeleteObject(nullPen);

    // 4. CURBS, CENTER MEDIAN, AND DASHED LANE MARKINGS
    HPEN curbPen = CreatePen(PS_SOLID, 5, road.getCurbColor().toCOLORREF());
    HPEN centerPen = CreatePen(PS_SOLID, 3, RGB(255, 215, 0));
    HPEN lanePen = CreatePen(PS_DASH, 2, road.getLineColor().toCOLORREF());

    SelectObject(m_hdcBack, curbPen);
    MoveToEx(m_hdcBack, roadScreenLeft, 0, NULL);
    LineTo(m_hdcBack, roadScreenLeft, m_height);
    MoveToEx(m_hdcBack, roadScreenRight, 0, NULL);
    LineTo(m_hdcBack, roadScreenRight, m_height);

    int centerScreenX = (int)(0.0f - effCamPos.x);
    SelectObject(m_hdcBack, centerPen);
    MoveToEx(m_hdcBack, centerScreenX, 0, NULL);
    LineTo(m_hdcBack, centerScreenX, m_height);

    int lane1X = (int)(-roadWidth * 0.25f - effCamPos.x);
    int lane2X = (int)(roadWidth * 0.25f - effCamPos.x);

    SelectObject(m_hdcBack, lanePen);
    MoveToEx(m_hdcBack, lane1X, 0, NULL);
    LineTo(m_hdcBack, lane1X, m_height);
    MoveToEx(m_hdcBack, lane2X, 0, NULL);
    LineTo(m_hdcBack, lane2X, m_height);

    DeleteObject(curbPen);
    DeleteObject(centerPen);
    DeleteObject(lanePen);

    // 5. TIRE SKID MARKS & PARTICLES
    particles.render(m_hdcBack, Vec2(effCamPos.x, effCamPos.y - m_height));

    // 6. ROADSIDE NATURE PROPS (TREES, CANOPIES, BUSHES, ROCKS)
    for (const WorldProp& prop : road.getProps()) {
        drawProp(prop, effCamPos);
    }

    // 7. RENDER AI TRAFFIC CARS (WITH UPGRADED MODELS)
    for (const TrafficCar& tc : traffic.getCars()) {
        Vec2 screenPos(tc.pos.x - effCamPos.x, (float)m_height - (tc.pos.y - effCamPos.y));

        if (weather == WeatherType::CYBER_NIGHT) {
            drawHeadlightBeams(screenPos, tc.angle, tc.width, tc.height);
        }

        if (tc.type == TrafficType::POLICE_CRUISER) {
            drawPoliceCruiser(screenPos, tc.angle, tc.width, tc.height, tc.sirenTimer);
        } else if (tc.type == TrafficType::SEMI_TRUCK) {
            drawSemiTruck(screenPos, tc.angle, tc.width, tc.height, tc.color);
        } else if (tc.type == TrafficType::SPORTS_CAR) {
            drawSupercar(screenPos, tc.angle, tc.width, tc.height, tc.color, ColorRGB(0, 0, 0), false, false);
        } else {
            drawTrafficSedan(screenPos, tc.angle, tc.width, tc.height, tc.color);
        }
    }

    // 8. RENDER PLAYER CAR (WITH UPGRADED SUPERCAR / MUSCLE / HYPERCAR MODELS)
    Vec2 playerScreenPos(car.getPos().x - effCamPos.x, (float)m_height - (car.getPos().y - effCamPos.y));

    if (weather == WeatherType::CYBER_NIGHT || weather == WeatherType::RAINY) {
        drawHeadlightBeams(playerScreenPos, car.getAngle(), car.getWidth(), car.getHeight());
    }

    bool brakeLights = (car.getSpeed() > 20.0f && GetAsyncKeyState(VK_DOWN) < 0) || (GetAsyncKeyState(VK_SPACE) < 0);
    int pType = car.getCarType();

    if (pType == 0) {
        drawSupercar(playerScreenPos, car.getAngle(), car.getWidth(), car.getHeight(), car.getBodyColor(), car.getUnderglowColor(), brakeLights, true);
    } else if (pType == 1) {
        drawMuscleCar(playerScreenPos, car.getAngle(), car.getWidth(), car.getHeight(), car.getBodyColor(), car.getUnderglowColor(), brakeLights, true);
    } else if (pType == 2) {
        drawHypercar(playerScreenPos, car.getAngle(), car.getWidth(), car.getHeight(), car.getBodyColor(), car.getUnderglowColor(), brakeLights, true);
    } else {
        drawTitanEnforcer(playerScreenPos, car.getAngle(), car.getWidth(), car.getHeight(), car.getBodyColor(), car.getUnderglowColor(), brakeLights, true);
    }
}

void Renderer::renderHUD(const Car& car, const Road& road, float stageTime, int score, int combo, float comboTimer, bool isMuted, GameMode mode) {
    SetBkMode(m_hdcBack, TRANSPARENT);

    // 1. TOP STATUS BAR (Glassmorphism Dark)
    HBRUSH barBrush = CreateSolidBrush(RGB(14, 18, 28));
    HGDIOBJ oldB = SelectObject(m_hdcBack, barBrush);
    Rectangle(m_hdcBack, 0, 0, m_width, 60);
    SelectObject(m_hdcBack, oldB);
    DeleteObject(barBrush);

    HPEN linePen = CreatePen(PS_SOLID, 2, RGB(0, 220, 255));
    HGDIOBJ oldP = SelectObject(m_hdcBack, linePen);
    MoveToEx(m_hdcBack, 0, 60, NULL);
    LineTo(m_hdcBack, m_width, 60);
    SelectObject(m_hdcBack, oldP);
    DeleteObject(linePen);

    HFONT hFontMain = CreateFontA(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HFONT hFontSmall = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HGDIOBJ oldFont = SelectObject(m_hdcBack, hFontMain);

    // Score display
    char scoreBuf[64];
    sprintf(scoreBuf, "SCORE: %06d", score);
    SetTextColor(m_hdcBack, RGB(255, 220, 40));
    TextOutA(m_hdcBack, 30, 16, scoreBuf, (int)strlen(scoreBuf));

    // Stage Name / Mode
    SetTextColor(m_hdcBack, RGB(255, 255, 255));
    std::string titleStr = (mode == GameMode::ENDLESS_TRAFFIC) ? "ENDLESS NATURE SPEEDWAY" : road.getStageName();
    TextOutA(m_hdcBack, m_width / 2 - 140, 16, titleStr.c_str(), (int)titleStr.length());

    // Timer display (Pulsing Red if low time)
    char timeBuf[64];
    int mins = (int)stageTime / 60;
    int secs = (int)stageTime % 60;
    int millis = (int)((stageTime - std::floor(stageTime)) * 100);
    sprintf(timeBuf, "TIME: %02d:%02d.%02d", mins, secs, millis);

    if (stageTime <= 10.0f && std::fmod(m_animTick, 0.4f) < 0.2f) {
        SetTextColor(m_hdcBack, RGB(255, 40, 40));
    } else {
        SetTextColor(m_hdcBack, RGB(0, 240, 255));
    }
    TextOutA(m_hdcBack, m_width - 240, 16, timeBuf, (int)strlen(timeBuf));

    // 2. SPEEDOMETER & TACHOMETER (Bottom Right)
    drawSpeedometerHUD(m_width - 150, m_height - 110, car.getSpeedMph(), car.getMaxSpeed() * 0.16f, car.getRpm(), car.getGear());

    // 3. NITRO BOOST GAUGE (Bottom Center)
    drawNitroGauge(m_width / 2 - 140, m_height - 50, 280, 24, car.getNitro(), 100.0f, car.isNitroActive());

    // 4. "CLOSE CALL" COMBO POPUP
    if (comboTimer > 0.0f && combo > 0) {
        SelectObject(m_hdcBack, hFontMain);
        char comboBuf[64];
        sprintf(comboBuf, "CLOSE CALL! x%d (+%d)", combo, combo * 500);
        SetTextColor(m_hdcBack, RGB(255, 80, 220));
        TextOutA(m_hdcBack, m_width / 2 - 120, m_height / 2 - 80, comboBuf, (int)strlen(comboBuf));
    }

    // 5. DRIFT MULTIPLIER ALERT
    if (car.isDrifting()) {
        SelectObject(m_hdcBack, hFontSmall);
        SetTextColor(m_hdcBack, RGB(255, 200, 40));
        const char* driftTxt = ">> 2D POWER SLIDE +NITRO <<";
        TextOutA(m_hdcBack, m_width / 2 - 100, m_height / 2 - 40, driftTxt, (int)strlen(driftTxt));
    }

    // 6. CRASH ALERT
    if (car.isCrashed()) {
        SelectObject(m_hdcBack, hFontMain);
        SetTextColor(m_hdcBack, RGB(255, 30, 30));
        const char* crashMsg = "! CRASHED ! RECOVERING...";
        TextOutA(m_hdcBack, m_width / 2 - 130, m_height / 2, crashMsg, (int)strlen(crashMsg));
    }

    // 7. BOTTOM CONTROLS HINT
    SelectObject(m_hdcBack, hFontSmall);
    SetTextColor(m_hdcBack, RGB(140, 150, 170));
    char hintBuf[128];
    sprintf(hintBuf, "[WASD/Arrows] Precision Drive  [Space] 2D Drift  [Shift/N] Nitro  [M] %s",
        (isMuted ? "Audio: OFF" : "Audio: ON"));
    TextOutA(m_hdcBack, 25, m_height - 30, hintBuf, (int)strlen(hintBuf));

    SelectObject(m_hdcBack, oldFont);
    DeleteObject(hFontMain);
    DeleteObject(hFontSmall);
}

void Renderer::drawSpeedometerHUD(int cx, int cy, float speedMph, float maxSpeedMph, float rpm, int gear) {
    int rad = 80;

    HBRUSH dialBrush = CreateSolidBrush(RGB(18, 22, 34));
    HPEN dialPen = CreatePen(PS_SOLID, 3, RGB(0, 200, 255));
    HGDIOBJ oldB = SelectObject(m_hdcBack, dialBrush);
    HGDIOBJ oldP = SelectObject(m_hdcBack, dialPen);
    Ellipse(m_hdcBack, cx - rad, cy - rad, cx + rad, cy + rad);

    HPEN tickPen = CreatePen(PS_SOLID, 2, RGB(255, 40, 60));
    SelectObject(m_hdcBack, tickPen);
    Arc(m_hdcBack, cx - rad + 8, cy - rad + 8, cx + rad - 8, cy + rad - 8, cx + 40, cy - 40, cx + rad - 8, cy);
    DeleteObject(tickPen);

    float speedAngle = -2.35f + (speedMph / maxSpeedMph) * 4.7f;
    int needleX = (int)(cx + std::cos(speedAngle) * (rad - 16));
    int needleY = (int)(cy + std::sin(speedAngle) * (rad - 16));

    HPEN needlePen = CreatePen(PS_SOLID, 3, RGB(255, 50, 50));
    SelectObject(m_hdcBack, needlePen);
    MoveToEx(m_hdcBack, cx, cy, NULL);
    LineTo(m_hdcBack, needleX, needleY);
    DeleteObject(needlePen);

    HBRUSH hubBrush = CreateSolidBrush(RGB(220, 220, 230));
    SelectObject(m_hdcBack, hubBrush);
    Ellipse(m_hdcBack, cx - 8, cy - 8, cx + 8, cy + 8);
    DeleteObject(hubBrush);

    HFONT hDigit = CreateFontA(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HGDIOBJ oldF = SelectObject(m_hdcBack, hDigit);
    SetTextColor(m_hdcBack, RGB(255, 255, 255));

    char spdStr[32];
    sprintf(spdStr, "%d MPH", (int)speedMph);
    RECT r = { cx - 50, cy + 20, cx + 50, cy + 50 };
    DrawTextA(m_hdcBack, spdStr, -1, &r, DT_CENTER | DT_SINGLELINE);

    char gearStr[16];
    sprintf(gearStr, "G%d", gear);
    SetTextColor(m_hdcBack, RGB(0, 240, 255));
    RECT rG = { cx - 30, cy - 45, cx + 30, cy - 20 };
    DrawTextA(m_hdcBack, gearStr, -1, &rG, DT_CENTER | DT_SINGLELINE);

    SelectObject(m_hdcBack, oldF);
    SelectObject(m_hdcBack, oldB);
    SelectObject(m_hdcBack, oldP);
    DeleteObject(dialBrush);
    DeleteObject(dialPen);
    DeleteObject(hDigit);
}

void Renderer::drawNitroGauge(int x, int y, int w, int h, float nitro, float maxNitro, bool active) {
    HBRUSH bgBrush = CreateSolidBrush(RGB(20, 25, 35));
    HPEN framePen = CreatePen(PS_SOLID, 2, active ? RGB(255, 0, 220) : RGB(0, 220, 255));
    HGDIOBJ oldB = SelectObject(m_hdcBack, bgBrush);
    HGDIOBJ oldP = SelectObject(m_hdcBack, framePen);
    Rectangle(m_hdcBack, x, y, x + w, y + h);

    float fillRatio = (maxNitro > 0.0f) ? (nitro / maxNitro) : 0.0f;
    if (fillRatio > 1.0f) fillRatio = 1.0f;
    if (fillRatio < 0.0f) fillRatio = 0.0f;

    int fillW = (int)((w - 4) * fillRatio);
    if (fillW > 0) {
        ColorRGB nColor = active ? ColorRGB(255, 0, 200) : ColorRGB(0, 220, 255);
        HBRUSH fillBrush = CreateSolidBrush(nColor.toCOLORREF());
        SelectObject(m_hdcBack, fillBrush);
        Rectangle(m_hdcBack, x + 2, y + 2, x + 2 + fillW, y + h - 2);
        DeleteObject(fillBrush);
    }

    HFONT hFont = CreateFontA(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HGDIOBJ oldF = SelectObject(m_hdcBack, hFont);
    SetTextColor(m_hdcBack, RGB(255, 255, 255));
    RECT r = { x, y, x + w, y + h };
    DrawTextA(m_hdcBack, active ? ">> NITRO OVERDRIVE ACTIVE <<" : "NITRO ENERGY", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(m_hdcBack, oldF);
    SelectObject(m_hdcBack, oldB);
    SelectObject(m_hdcBack, oldP);
    DeleteObject(bgBrush);
    DeleteObject(framePen);
    DeleteObject(hFont);
}

void Renderer::renderMainMenu(int selectedItem, int highScore, int credits) {
    HBRUSH bgBrush = CreateSolidBrush(RGB(12, 14, 24));
    HGDIOBJ oldB = SelectObject(m_hdcBack, bgBrush);
    Rectangle(m_hdcBack, 0, 0, m_width, m_height);
    SelectObject(m_hdcBack, oldB);
    DeleteObject(bgBrush);

    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(25, 35, 55));
    HGDIOBJ oldP = SelectObject(m_hdcBack, gridPen);
    for (int y = 0; y < m_height; y += 40) {
        MoveToEx(m_hdcBack, 0, y, NULL);
        LineTo(m_hdcBack, m_width, y);
    }
    for (int x = 0; x < m_width; x += 40) {
        MoveToEx(m_hdcBack, x, 0, NULL);
        LineTo(m_hdcBack, x, m_height);
    }
    SelectObject(m_hdcBack, oldP);
    DeleteObject(gridPen);

    HFONT hFontTitle = CreateFontA(58, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HFONT hFontSubtitle = CreateFontA(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HFONT hFontMenu = CreateFontA(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");

    HGDIOBJ oldF = SelectObject(m_hdcBack, hFontTitle);
    SetBkMode(m_hdcBack, TRANSPARENT);
    SetTextColor(m_hdcBack, RGB(0, 240, 255));
    RECT rTitle = { 0, 90, m_width, 160 };
    DrawTextA(m_hdcBack, "APEX CYBER DRIVE 2D", -1, &rTitle, DT_CENTER | DT_SINGLELINE);

    SelectObject(m_hdcBack, hFontSubtitle);
    SetTextColor(m_hdcBack, RGB(255, 0, 180));
    RECT rSub = { 0, 165, m_width, 210 };
    DrawTextA(m_hdcBack, "TOP-DOWN NATURE SPEEDWAY", -1, &rSub, DT_CENTER | DT_SINGLELINE);

    char scoreInfo[128];
    sprintf(scoreInfo, "HIGH SCORE: %06d   |   CREDITS: $%d", highScore, credits);
    SetTextColor(m_hdcBack, RGB(255, 220, 40));
    RECT rScore = { 0, 220, m_width, 260 };
    DrawTextA(m_hdcBack, scoreInfo, -1, &rScore, DT_CENTER | DT_SINGLELINE);

    const char* menuItems[5] = {
        "1. GRAND PRIX CAMPAIGN",
        "2. ENDLESS HIGHWAY RUSH",
        "3. TIME ATTACK TRIAL",
        "4. GARAGE & UPGRADES",
        "5. EXIT GAME"
    };

    SelectObject(m_hdcBack, hFontMenu);
    int startY = 290;
    int itemH = 65;

    for (int i = 0; i < 5; ++i) {
        int itemY = startY + i * itemH;
        int itemW = 460;
        int itemX = m_width / 2 - itemW / 2;

        bool selected = (i == selectedItem);
        HBRUSH btnBrush = CreateSolidBrush(selected ? RGB(0, 140, 220) : RGB(22, 28, 44));
        HPEN btnPen = CreatePen(PS_SOLID, 2, selected ? RGB(0, 240, 255) : RGB(40, 50, 75));
        HGDIOBJ bOld = SelectObject(m_hdcBack, btnBrush);
        HGDIOBJ pOld = SelectObject(m_hdcBack, btnPen);

        Rectangle(m_hdcBack, itemX, itemY, itemX + itemW, itemY + 50);

        SelectObject(m_hdcBack, bOld);
        SelectObject(m_hdcBack, pOld);
        DeleteObject(btnBrush);
        DeleteObject(btnPen);

        SetTextColor(m_hdcBack, selected ? RGB(255, 255, 255) : RGB(170, 185, 210));
        RECT rItem = { itemX, itemY, itemX + itemW, itemY + 50 };
        DrawTextA(m_hdcBack, menuItems[i], -1, &rItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    SelectObject(m_hdcBack, oldF);
    DeleteObject(hFontTitle);
    DeleteObject(hFontSubtitle);
    DeleteObject(hFontMenu);
}

void Renderer::renderGarage(int selectedCar, const SaveData& save, int selectedStat, int selectedColor) {
    HBRUSH bgBrush = CreateSolidBrush(RGB(14, 18, 30));
    HGDIOBJ oldB = SelectObject(m_hdcBack, bgBrush);
    Rectangle(m_hdcBack, 0, 0, m_width, m_height);
    SelectObject(m_hdcBack, oldB);
    DeleteObject(bgBrush);

    HFONT hTitle = CreateFontA(42, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HFONT hFont = CreateFontA(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HFONT hSmall = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");

    HGDIOBJ oldF = SelectObject(m_hdcBack, hTitle);
    SetBkMode(m_hdcBack, TRANSPARENT);
    SetTextColor(m_hdcBack, RGB(0, 240, 255));
    RECT rTitle = { 40, 30, m_width - 40, 80 };
    DrawTextA(m_hdcBack, "GARAGE & 2D TUNING", -1, &rTitle, DT_LEFT | DT_SINGLELINE);

    char credStr[64];
    sprintf(credStr, "CREDITS: $%d", save.credits);
    SetTextColor(m_hdcBack, RGB(255, 220, 40));
    DrawTextA(m_hdcBack, credStr, -1, &rTitle, DT_RIGHT | DT_SINGLELINE);

    const char* carNames[4] = { "1. Apex Falcon (Supercar)", "2. Viper GT (Muscle Speed)", "3. Cyber Phantom (Hypercar)", "4. Titan Enforcer (Heavy)" };
    const int unlockPrices[4] = { 0, 2500, 5000, 8000 };

    SelectObject(m_hdcBack, hFont);

    int carListY = 110;
    for (int i = 0; i < 4; ++i) {
        bool isSel = (i == selectedCar);
        bool isUnlocked = (save.unlockedCars[i] != 0);

        HBRUSH btnB = CreateSolidBrush(isSel ? RGB(0, 120, 200) : RGB(25, 30, 48));
        HPEN btnP = CreatePen(PS_SOLID, 2, isSel ? RGB(0, 240, 255) : RGB(45, 55, 80));
        HGDIOBJ bO = SelectObject(m_hdcBack, btnB);
        HGDIOBJ pO = SelectObject(m_hdcBack, btnP);

        Rectangle(m_hdcBack, 40, carListY + i * 65, 340, carListY + i * 65 + 50);

        SelectObject(m_hdcBack, bO);
        SelectObject(m_hdcBack, pO);
        DeleteObject(btnB);
        DeleteObject(btnP);

        char cNameBuf[128];
        if (isUnlocked) {
            sprintf(cNameBuf, "%s", carNames[i]);
            SetTextColor(m_hdcBack, isSel ? RGB(255, 255, 255) : RGB(180, 200, 230));
        } else {
            sprintf(cNameBuf, "%s [LOCKED: $%d]", carNames[i], unlockPrices[i]);
            SetTextColor(m_hdcBack, RGB(140, 140, 150));
        }
        RECT rC = { 50, carListY + i * 65, 330, carListY + i * 65 + 50 };
        DrawTextA(m_hdcBack, cNameBuf, -1, &rC, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    // Dynamic 360 Rotating Preview in Garage
    const ColorRGB palette[5] = {
        ColorRGB(235, 45, 45), ColorRGB(0, 235, 255), ColorRGB(50, 255, 80), ColorRGB(30, 30, 35), ColorRGB(225, 185, 45)
    };
    ColorRGB previewColor = palette[save.paintColorIndex[selectedCar] % 5];
    ColorRGB previewUnderglow = ColorRGB(0, 240, 255);
    Vec2 prevPos((float)(m_width / 2 + 120), 240.0f);
    float prevAngle = m_animTick * 1.4f;

    if (selectedCar == 0) {
        drawSupercar(prevPos, prevAngle, 54.0f, 105.0f, previewColor, previewUnderglow, false, true);
    } else if (selectedCar == 1) {
        drawMuscleCar(prevPos, prevAngle, 58.0f, 108.0f, previewColor, previewUnderglow, false, true);
    } else if (selectedCar == 2) {
        drawHypercar(prevPos, prevAngle, 56.0f, 112.0f, previewColor, previewUnderglow, false, true);
    } else {
        drawTitanEnforcer(prevPos, prevAngle, 62.0f, 114.0f, previewColor, previewUnderglow, false, true);
    }

    const char* statNames[4] = { "Top Speed", "Acceleration", "Handling / Grip", "Nitro Capacity" };
    int currentLevels[4] = {
        save.upgradeSpeed[selectedCar],
        save.upgradeAccel[selectedCar],
        save.upgradeHandling[selectedCar],
        save.upgradeNitro[selectedCar]
    };

    int statY = 380;
    int statX = 390;
    int upgradePrice = 800;

    for (int s = 0; s < 4; ++s) {
        bool isSel = (s == selectedStat);
        SetTextColor(m_hdcBack, isSel ? RGB(0, 240, 255) : RGB(220, 220, 230));

        char statBuf[64];
        sprintf(statBuf, "%s (Lv %d/5):", statNames[s], currentLevels[s]);
        TextOutA(m_hdcBack, statX, statY + s * 45, statBuf, (int)strlen(statBuf));

        for (int p = 1; p <= 5; ++p) {
            bool filled = (p <= currentLevels[s]);
            HBRUSH pipB = CreateSolidBrush(filled ? RGB(0, 230, 255) : RGB(40, 45, 60));
            HGDIOBJ bO = SelectObject(m_hdcBack, pipB);
            Rectangle(m_hdcBack, statX + 220 + (p - 1) * 32, statY + s * 45 + 4, statX + 220 + (p - 1) * 32 + 24, statY + s * 45 + 24);
            SelectObject(m_hdcBack, bO);
            DeleteObject(pipB);
        }

        if (currentLevels[s] < 5) {
            char upgBuf[32];
            sprintf(upgBuf, "Upgrade ($%d)", upgradePrice * currentLevels[s]);
            SetTextColor(m_hdcBack, RGB(255, 220, 40));
            TextOutA(m_hdcBack, statX + 400, statY + s * 45, upgBuf, (int)strlen(upgBuf));
        } else {
            SetTextColor(m_hdcBack, RGB(80, 255, 120));
            TextOutA(m_hdcBack, statX + 400, statY + s * 45, "[MAXED]", 7);
        }
    }

    SetTextColor(m_hdcBack, RGB(255, 255, 255));
    TextOutA(m_hdcBack, statX, 580, "Paint Colors (Key 1-5):", 23);
    for (int c = 0; c < 5; ++c) {
        HBRUSH cB = CreateSolidBrush(palette[c].toCOLORREF());
        HPEN cP = CreatePen(PS_SOLID, (save.paintColorIndex[selectedCar] == c) ? 3 : 1, RGB(255, 255, 255));
        HGDIOBJ bO = SelectObject(m_hdcBack, cB);
        HGDIOBJ pO = SelectObject(m_hdcBack, cP);
        Ellipse(m_hdcBack, statX + 240 + c * 42, 575, statX + 240 + c * 42 + 30, 605);
        SelectObject(m_hdcBack, bO);
        SelectObject(m_hdcBack, pO);
        DeleteObject(cB);
        DeleteObject(cP);
    }

    SelectObject(m_hdcBack, hSmall);
    SetTextColor(m_hdcBack, RGB(140, 160, 190));
    const char* garHint = "[Up/Down] Select Stat  [Enter/Space] Buy Upgrade/Unlock Car  [Left/Right] Switch Car  [Esc] Back to Menu";
    TextOutA(m_hdcBack, 40, m_height - 35, garHint, (int)strlen(garHint));

    SelectObject(m_hdcBack, oldF);
    DeleteObject(hTitle);
    DeleteObject(hFont);
    DeleteObject(hSmall);
}

void Renderer::renderStageClear(int stage, int score, int rewardCredits, float timeTaken) {
    int boxW = 550;
    int boxH = 360;
    int boxX = m_width / 2 - boxW / 2;
    int boxY = m_height / 2 - boxH / 2;

    HBRUSH bgBrush = CreateSolidBrush(RGB(15, 20, 35));
    HPEN borderPen = CreatePen(PS_SOLID, 3, RGB(0, 240, 255));
    HGDIOBJ oldB = SelectObject(m_hdcBack, bgBrush);
    HGDIOBJ oldP = SelectObject(m_hdcBack, borderPen);
    Rectangle(m_hdcBack, boxX, boxY, boxX + boxW, boxY + boxH);

    HFONT hTitle = CreateFontA(42, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HFONT hText = CreateFontA(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HGDIOBJ oldF = SelectObject(m_hdcBack, hTitle);

    SetBkMode(m_hdcBack, TRANSPARENT);
    SetTextColor(m_hdcBack, RGB(255, 215, 0));
    RECT rT = { boxX, boxY + 25, boxX + boxW, boxY + 80 };
    DrawTextA(m_hdcBack, "STAGE COMPLETED!", -1, &rT, DT_CENTER | DT_SINGLELINE);

    SelectObject(m_hdcBack, hText);
    SetTextColor(m_hdcBack, RGB(255, 255, 255));

    char line1[64], line2[64], line3[64];
    sprintf(line1, "Stage: %d", stage);
    sprintf(line2, "Stage Score: %06d", score);
    sprintf(line3, "Reward: +$%d Credits", rewardCredits);

    TextOutA(m_hdcBack, boxX + 60, boxY + 110, line1, (int)strlen(line1));
    TextOutA(m_hdcBack, boxX + 60, boxY + 155, line2, (int)strlen(line2));
    SetTextColor(m_hdcBack, RGB(0, 240, 255));
    TextOutA(m_hdcBack, boxX + 60, boxY + 200, line3, (int)strlen(line3));

    SetTextColor(m_hdcBack, RGB(255, 220, 40));
    RECT rNext = { boxX, boxY + 280, boxX + boxW, boxY + 330 };
    DrawTextA(m_hdcBack, "Press [ENTER / SPACE] to Continue", -1, &rNext, DT_CENTER | DT_SINGLELINE);

    SelectObject(m_hdcBack, oldF);
    SelectObject(m_hdcBack, oldB);
    SelectObject(m_hdcBack, oldP);
    DeleteObject(bgBrush);
    DeleteObject(borderPen);
    DeleteObject(hTitle);
    DeleteObject(hText);
}

void Renderer::renderGameOver(int score, int highScore, int creditsEarned, bool newHighScore) {
    int boxW = 550;
    int boxH = 360;
    int boxX = m_width / 2 - boxW / 2;
    int boxY = m_height / 2 - boxH / 2;

    HBRUSH bgBrush = CreateSolidBrush(RGB(25, 12, 18));
    HPEN borderPen = CreatePen(PS_SOLID, 3, RGB(255, 40, 60));
    HGDIOBJ oldB = SelectObject(m_hdcBack, bgBrush);
    HGDIOBJ oldP = SelectObject(m_hdcBack, borderPen);
    Rectangle(m_hdcBack, boxX, boxY, boxX + boxW, boxY + boxH);

    HFONT hTitle = CreateFontA(44, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HFONT hText = CreateFontA(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HGDIOBJ oldF = SelectObject(m_hdcBack, hTitle);

    SetBkMode(m_hdcBack, TRANSPARENT);
    SetTextColor(m_hdcBack, RGB(255, 40, 60));
    RECT rT = { boxX, boxY + 25, boxX + boxW, boxY + 80 };
    DrawTextA(m_hdcBack, "TIME EXPIRED!", -1, &rT, DT_CENTER | DT_SINGLELINE);

    SelectObject(m_hdcBack, hText);
    SetTextColor(m_hdcBack, RGB(255, 255, 255));

    char line1[64], line2[64], line3[64];
    sprintf(line1, "Final Score: %06d", score);
    sprintf(line2, "High Score: %06d %s", highScore, newHighScore ? "(NEW RECORD!)" : "");
    sprintf(line3, "Credits Earned: +$%d", creditsEarned);

    TextOutA(m_hdcBack, boxX + 60, boxY + 110, line1, (int)strlen(line1));
    SetTextColor(m_hdcBack, newHighScore ? RGB(255, 215, 0) : RGB(200, 200, 210));
    TextOutA(m_hdcBack, boxX + 60, boxY + 155, line2, (int)strlen(line2));
    SetTextColor(m_hdcBack, RGB(0, 240, 255));
    TextOutA(m_hdcBack, boxX + 60, boxY + 200, line3, (int)strlen(line3));

    SetTextColor(m_hdcBack, RGB(255, 220, 40));
    RECT rNext = { boxX, boxY + 280, boxX + boxW, boxY + 330 };
    DrawTextA(m_hdcBack, "Press [ENTER / SPACE] for Main Menu", -1, &rNext, DT_CENTER | DT_SINGLELINE);

    SelectObject(m_hdcBack, oldF);
    SelectObject(m_hdcBack, oldB);
    SelectObject(m_hdcBack, oldP);
    DeleteObject(bgBrush);
    DeleteObject(borderPen);
    DeleteObject(hTitle);
    DeleteObject(hText);
}

void Renderer::renderPauseMenu(int selectedItem) {
    int boxW = 400;
    int boxH = 260;
    int boxX = m_width / 2 - boxW / 2;
    int boxY = m_height / 2 - boxH / 2;

    HBRUSH bgBrush = CreateSolidBrush(RGB(18, 22, 36));
    HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(0, 240, 255));
    HGDIOBJ oldB = SelectObject(m_hdcBack, bgBrush);
    HGDIOBJ oldP = SelectObject(m_hdcBack, borderPen);
    Rectangle(m_hdcBack, boxX, boxY, boxX + boxW, boxY + boxH);

    HFONT hTitle = CreateFontA(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HFONT hMenu = CreateFontA(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");
    HGDIOBJ oldF = SelectObject(m_hdcBack, hTitle);

    SetBkMode(m_hdcBack, TRANSPARENT);
    SetTextColor(m_hdcBack, RGB(0, 240, 255));
    RECT rT = { boxX, boxY + 20, boxX + boxW, boxY + 70 };
    DrawTextA(m_hdcBack, "GAME PAUSED", -1, &rT, DT_CENTER | DT_SINGLELINE);

    SelectObject(m_hdcBack, hMenu);
    const char* items[3] = { "RESUME RACE", "RESTART STAGE", "QUIT TO MENU" };

    for (int i = 0; i < 3; ++i) {
        bool sel = (i == selectedItem);
        SetTextColor(m_hdcBack, sel ? RGB(255, 220, 40) : RGB(170, 180, 200));
        RECT r = { boxX, boxY + 85 + i * 50, boxX + boxW, boxY + 125 + i * 50 };
        char buf[64];
        sprintf(buf, "%s %s", sel ? ">" : " ", items[i]);
        DrawTextA(m_hdcBack, buf, -1, &r, DT_CENTER | DT_SINGLELINE);
    }

    SelectObject(m_hdcBack, oldF);
    SelectObject(m_hdcBack, oldB);
    SelectObject(m_hdcBack, oldP);
    DeleteObject(bgBrush);
    DeleteObject(borderPen);
    DeleteObject(hTitle);
    DeleteObject(hMenu);
}
