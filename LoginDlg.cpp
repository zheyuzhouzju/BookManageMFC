#include "pch.h"
#include "LoginDlg.h"
#include "MainDlg.h"

IMPLEMENT_DYNAMIC(LoginDlg, CDialogEx)

LoginDlg::LoginDlg(CWnd* pParent)
    : CDialogEx(IDD_LOGIN_DIALOG, pParent)
{}

BOOL LoginDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_bgBrush.CreateSolidBrush(W11_BG);
    m_titleFont.CreatePointFont(200, _T("微软雅黑"));
    m_subFont.CreatePointFont(75, _T("Segoe UI"));
    m_iconFont.CreatePointFont(260, _T("Segoe UI"));

    GetDlgItem(IDC_ICON_STATIC)->SetFont(&m_iconFont);
    GetDlgItem(IDC_TITLE_STATIC)->SetFont(&m_titleFont);
    GetDlgItem(IDC_SUBTITLE_STATIC)->SetFont(&m_subFont);

    m_btnLogin.SubclassDlgItem(IDC_LOGIN_BUTTON, this);
    m_btnLogin.setColors(W11_CYAN, CYAN_HOVER, CYAN_PRESS,
                         RGB(10,10,25), RGB(10,10,25), RGB(10,10,25));

    GetDlgItem(IDC_ACCOUNT_EDIT)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
    GetDlgItem(IDC_PASSWORD_EDIT)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

    return TRUE;
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(LoginDlg, CDialogEx)
    ON_BN_CLICKED(IDC_LOGIN_BUTTON, &LoginDlg::OnBnClickedLogin)
    ON_BN_CLICKED(IDC_SHOWPWD_CHECK, &LoginDlg::OnBnClickedShowPwd)
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL LoginDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect rc;
    GetClientRect(&rc);

    // 渐变必须画在 OnEraseBkgnd 里，OnPaint 画的 STATIC 控件看不到
    pDC->FillSolidRect(&rc, W11_BG);

    // 头图 100px 从上到下由深变浅
    for (int y = 0; y < 100; y++) {
        int r = 35 + (y * 12 / 100);
        int g = 37 + (y * 14 / 100);
        int b = 49 + (y * 16 / 100);
        pDC->FillSolidRect(0, y, rc.right, 1, RGB(r, g, b));
    }
    return TRUE;
}

void LoginDlg::OnPaint()
{
    CPaintDC dc(this);
    CRect rc;
    GetClientRect(&rc);

    // === Form card === (darker panel behind the form fields)
    CRect card(50, 114, rc.right - 50, 250);
    CBrush cardBr(CARD_BG);
    CPen cardPen(PS_SOLID, 1, CARD_BORDER);
    CBrush* oldBr = dc.SelectObject(&cardBr);
    CPen* oldPen = dc.SelectObject(&cardPen);
    dc.RoundRect(&card, CPoint(6, 6));
    dc.SelectObject(oldBr);
    dc.SelectObject(oldPen);

    // Input underlines inside card
    dc.SelectStockObject(DC_PEN);
    dc.SetDCPenColor(RGB(90, 92, 110));
    dc.MoveTo(card.left + 20, 152);  dc.LineTo(card.right - 20, 152);
    dc.MoveTo(card.left + 20, 188);  dc.LineTo(card.right - 20, 188);
}

void LoginDlg::OnBnClickedLogin()
{
    CString account, password;
    GetDlgItemText(IDC_ACCOUNT_EDIT, account);
    GetDlgItemText(IDC_PASSWORD_EDIT, password);

    if (account.IsEmpty() || password.IsEmpty()) {
        AfxMessageBox(_T("账号和密码不能为空。"), MB_OK | MB_ICONWARNING);
        return;
    }

    if (m_loginMgr.validate(account, password)) {
        EndDialog(IDOK);
        MainDlg mainDlg;
        mainDlg.DoModal();
        return;
    }

    m_loginMgr.fail();

    if (m_loginMgr.locked()) {
        AfxMessageBox(_T("尝试次数已用完，程序即将退出。"),
                      MB_OK | MB_ICONSTOP);
        EndDialog(IDCANCEL);
        return;
    }

    CString msg;
    msg.Format(_T("账号或密码错误，还剩 %d 次尝试机会。"),
               m_loginMgr.left());
    AfxMessageBox(msg, MB_OK | MB_ICONERROR);
    SetDlgItemText(IDC_TRYCOUNT_STATIC, msg);
}

void LoginDlg::OnOK()
{
    OnBnClickedLogin();
}

HBRUSH LoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (nCtlColor == CTLCOLOR_STATIC) {
        int id = pWnd->GetDlgCtrlID();
        pDC->SetBkMode(TRANSPARENT);
        if (id == IDC_ICON_STATIC)
            pDC->SetTextColor(W11_CYAN);
        else if (id == IDC_TITLE_STATIC)
            pDC->SetTextColor(RGB(245, 243, 235));
        else if (id == IDC_SUBTITLE_STATIC)
            pDC->SetTextColor(RGB(175, 168, 205));
        else if (id == IDC_TRYCOUNT_STATIC)
            pDC->SetTextColor(RGB(255, 155, 135));
        else if (id == IDC_VERSION_STATIC)
            pDC->SetTextColor(RGB(85, 88, 105));
        else
            pDC->SetTextColor(RGB(215, 210, 200));
        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    if (nCtlColor == CTLCOLOR_DLG) {
        pDC->SetBkColor(W11_BG);
        return m_bgBrush;
    }
    if (nCtlColor == CTLCOLOR_EDIT) {
        pDC->SetBkColor(DARK_INPUT);
        pDC->SetTextColor(W11_INK);
        static CBrush br(DARK_INPUT);
        return br;
    }
    return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

void LoginDlg::OnBnClickedShowPwd()
{
    CButton* pCheck = (CButton*)GetDlgItem(IDC_SHOWPWD_CHECK);
    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_PASSWORD_EDIT);
    if (!pCheck || !pEdit) return;

    if (pCheck->GetCheck() == BST_CHECKED) {
        pEdit->SetPasswordChar(0);
        pEdit->Invalidate();
    } else {
        pEdit->SetPasswordChar('*');
        pEdit->Invalidate();
    }
}
