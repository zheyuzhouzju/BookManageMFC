#pragma once

#include "resource.h"
#include "ModernButton.h"
#include <afxdialogex.h>

// 登录逻辑：校验账号密码，限制尝试次数
class CLoginManager {
    CString m_user;
    CString m_pass;
    int     m_tries;     // 剩余尝试次数

public:
    CLoginManager()
        : m_user(_T("admin"))
        , m_pass(_T("book2026"))
        , m_tries(3)
    {}

    bool validate(const CString& a, const CString& p) const {
        return (a == m_user && p == m_pass);
    }
    int  left()   const { return m_tries; }
    void fail()         { if (m_tries > 0) --m_tries; }
    bool locked() const { return m_tries <= 0; }
};

class LoginDlg : public CDialogEx
{
    DECLARE_DYNAMIC(LoginDlg)

public:
    LoginDlg(CWnd* pParent = nullptr);
    virtual ~LoginDlg() = default;

    enum { IDD = IDD_LOGIN_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

    CLoginManager  m_loginMgr;
    CBrush         m_bgBrush;
    CFont          m_titleFont;
    CFont          m_subFont;
    CFont          m_iconFont;
    CModernButton  m_btnLogin;

public:
    afx_msg void OnBnClickedLogin();
    afx_msg void OnBnClickedShowPwd();
    virtual void OnOK() override;
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
};
