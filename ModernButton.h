#pragma once
#include "Colors.h"

class CModernButton : public CButton
{
    COLORREF m_bgNormal, m_bgHover, m_bgPress;
    COLORREF m_fgNormal, m_fgHover, m_fgPress;
    bool     m_tracking;
    bool     m_pressed;
    int      m_roundR;

public:
    CModernButton()
        : m_bgNormal(W11_BLUE), m_bgHover(W11_BLUE), m_bgPress(W11_BLUE)
        , m_fgNormal(W11_SURFACE), m_fgHover(W11_SURFACE), m_fgPress(W11_SURFACE)
        , m_tracking(false), m_pressed(false), m_roundR(6)
    {}

    void setColors(COLORREF bg, COLORREF bgHover, COLORREF bgPress,
                   COLORREF fg, COLORREF fgHover, COLORREF fgPress) {
        m_bgNormal = bg; m_bgHover = bgHover; m_bgPress = bgPress;
        m_fgNormal = fg; m_fgHover = fgHover; m_fgPress = fgPress;
    }
    void setPrimary() {
        setColors(W11_CYAN, CYAN_HOVER, CYAN_PRESS, RGB(10,10,25), RGB(10,10,25), RGB(10,10,25));
    }
    void setDanger() {
        setColors(W11_DANGER, darken(W11_DANGER,20), darken(W11_DANGER,40),
                  W11_SURFACE, W11_SURFACE, W11_SURFACE);
    }
    void setUtility() {
        setColors(UTIL_BG, UTIL_HOVER, UTIL_PRESS, W11_MUTED, W11_INK, W11_INK);
    }

    static COLORREF darken(COLORREF c, int d) {
        int r = max(0, GetRValue(c)-d), g = max(0, GetGValue(c)-d), b = max(0, GetBValue(c)-d);
        return RGB(r,g,b);
    }

    void init(UINT id, CWnd* parent, bool primary, bool danger, bool utility) {
        SubclassDlgItem(id, parent);
        ModifyStyle(0, BS_OWNERDRAW);
        if (utility)     setUtility();
        else if (danger) setDanger();
        else             setPrimary();
        if (!primary && !danger && !utility) setPrimary();
    }

protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT lp) override
    {
        CDC* pDC = CDC::FromHandle(lp->hDC);
        CRect rc(lp->rcItem);
        pDC->SetBkMode(TRANSPARENT);

        COLORREF bg, fg;
        if (lp->itemState & ODS_SELECTED)      { bg = m_bgPress; fg = m_fgPress; }
        else if (m_pressed)                     { bg = m_bgPress; fg = m_fgPress; }
        else if (lp->itemState & ODS_HOTLIGHT)  { bg = m_bgHover; fg = m_fgHover; }
        else                                    { bg = m_bgNormal; fg = m_fgNormal; }

        CRect r = rc; r.DeflateRect(1, 1);
        int R = GetRValue(bg), G = GetGValue(bg), B = GetBValue(bg);

        // 从上到下渐变色填充，顶部亮→底部暗，模拟凸起感
        int h = r.Height();
        for (int y = 0; y < h; y++) {
            float t = (float)y / h;
            int rr = (int)(R + (R * 0.25) * (1.0f - t));
            int gg = (int)(G + (G * 0.25) * (1.0f - t));
            int bb = (int)(B + (B * 0.25) * (1.0f - t));
            if (rr > 255) rr = 255; if (gg > 255) gg = 255; if (bb > 255) bb = 255;
            pDC->FillSolidRect(r.left, r.top + y, r.Width(), 1, RGB(rr, gg, bb));
        }

        // Rounded corners mask via frame pen in bg color
        CPen pen(PS_SOLID, 1, bg);
        CPen* oldPen = pDC->SelectObject(&pen);
        CBrush* oldBr = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
        pDC->RoundRect(&r, CPoint(m_roundR, m_roundR));
        pDC->SelectObject(oldPen);
        pDC->SelectObject(oldBr);

        // Bottom shadow line (1px darker)
        int sr = max(0, R - 40), sg = max(0, G - 40), sb = max(0, B - 40);
        pDC->FillSolidRect(r.left + 3, r.bottom - 1, r.Width() - 6, 1, RGB(sr, sg, sb));

        // Gloss highlight on top third
        int glossH = r.Height() / 3;
        for (int y = 0; y < glossH; y++) {
            int a = 28 - (y * 28 / glossH);
            if (a > 0) {
                int rr = min(255, R + a);
                int gg = min(255, G + a);
                int bb = min(255, B + a);
                pDC->FillSolidRect(r.left + 3, r.top + 1 + y, r.Width() - 6, 1, RGB(rr, gg, bb));
            }
        }

        pDC->SetTextColor(fg);
        CString text; GetWindowText(text);
        CFont* oldFont = pDC->SelectObject(GetFont());
        pDC->DrawText(text, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(oldFont);

        if (lp->itemState & ODS_FOCUS) {
            CRect fr = rc; fr.DeflateRect(3, 3);
            pDC->DrawFocusRect(&fr);
        }
    }

};
