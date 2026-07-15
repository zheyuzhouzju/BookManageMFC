#pragma once

#include "resource.h"
#include "BookManager.h"
#include "ModernButton.h"
#include <afxdialogex.h>
#include <afxcmn.h>

class MainDlg : public CDialogEx
{
    DECLARE_DYNAMIC(MainDlg)

public:
    MainDlg(CWnd* pParent = nullptr);
    virtual ~MainDlg() = default;

    enum { IDD = IDD_MAIN_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

    BookManager m_mgr;

    CString m_bookID;
    CString m_bookName;
    CString m_priceStr;
    CString m_extra1;
    CString m_extra2;

    CComboBox m_typeCombo;
    CListCtrl m_bookList;
    CStatic   m_statusText;

    CModernButton m_btnAdd;
    CModernButton m_btnModify;
    CModernButton m_btnDelete;
    CModernButton m_btnSearch;
    CModernButton m_btnSort;
    CModernButton m_btnSave;
    CModernButton m_btnLoad;
    CModernButton m_btnExit;

    CFont m_listFont;
    CFont m_sectionFont;
    CFont m_statusFont;
    CBrush m_bgBrush;
    CString m_statusMsg;
    bool    m_statusOk;

    void styleButtons();
    void styleList();
    void showStatus(const CString& msg, bool ok);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnNMCustomdrawBookList(NMHDR* pNMHDR, LRESULT* pResult);

    void syncListFromVector();
    void clearEditFields();
    void updateExtraLabels();
    void fillEditFromBook(const Book* book);
    void onSelectBook();
    int  getSelectedIndex();

    afx_msg void OnCbnSelchangeTypeCombo();
    afx_msg void OnLvnItemchangedBookList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedModify();
    afx_msg void OnBnClickedDelete();
    afx_msg void OnBnClickedSearch();
    afx_msg void OnBnClickedSort();
    afx_msg void OnBnClickedSave();
    afx_msg void OnBnClickedLoad();
    afx_msg void OnBnClickedExit();
};
