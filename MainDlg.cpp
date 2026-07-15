#include "pch.h"
#include "MainDlg.h"
#include "ModernButton.h"
#include <sstream>
#include <iomanip>

static std::string ToStr(const CString& cs) {
    if (cs.IsEmpty()) return "";
    int n = WideCharToMultiByte(CP_UTF8, 0, cs, -1, NULL, 0, NULL, NULL);
    std::string result(n - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, cs, -1, &result[0], n, NULL, NULL);
    return result;
}

static CString Utf8ToCString(const std::string& u8) {
    if (u8.empty()) return _T("");
    int n = MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, NULL, 0);
    CString result;
    MultiByteToWideChar(CP_UTF8, 0, u8.c_str(), -1, result.GetBuffer(n), n);
    result.ReleaseBuffer();
    return result;
}

IMPLEMENT_DYNAMIC(MainDlg, CDialogEx)

MainDlg::MainDlg(CWnd* pParent)
    : CDialogEx(IDD_MAIN_DIALOG, pParent)
{}

void MainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_BOOKID_EDIT, m_bookID);
    DDX_Text(pDX, IDC_BOOKNAME_EDIT, m_bookName);
    DDX_Text(pDX, IDC_PRICE_EDIT, m_priceStr);
    DDX_Text(pDX, IDC_EXTRA1_EDIT, m_extra1);
    DDX_Text(pDX, IDC_EXTRA2_EDIT, m_extra2);
    DDX_Control(pDX, IDC_TYPE_COMBO, m_typeCombo);
    DDX_Control(pDX, IDC_BOOK_LIST, m_bookList);
    DDX_Control(pDX, IDC_STATUS_TEXT, m_statusText);
}

BEGIN_MESSAGE_MAP(MainDlg, CDialogEx)
    ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &MainDlg::OnCbnSelchangeTypeCombo)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_BOOK_LIST, &MainDlg::OnLvnItemchangedBookList)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_BOOK_LIST, &MainDlg::OnNMCustomdrawBookList)
    ON_BN_CLICKED(IDC_ADD_BUTTON, &MainDlg::OnBnClickedAdd)
    ON_BN_CLICKED(IDC_MODIFY_BUTTON, &MainDlg::OnBnClickedModify)
    ON_BN_CLICKED(IDC_DELETE_BUTTON, &MainDlg::OnBnClickedDelete)
    ON_BN_CLICKED(IDC_SEARCH_BUTTON, &MainDlg::OnBnClickedSearch)
    ON_BN_CLICKED(IDC_SORT_BUTTON, &MainDlg::OnBnClickedSort)
    ON_BN_CLICKED(IDC_SAVE_BUTTON, &MainDlg::OnBnClickedSave)
    ON_BN_CLICKED(IDC_LOAD_BUTTON, &MainDlg::OnBnClickedLoad)
    ON_BN_CLICKED(IDC_EXIT_BUTTON, &MainDlg::OnBnClickedExit)
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL MainDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_bgBrush.CreateSolidBrush(W11_BG);
    m_listFont.CreatePointFont(100, _T("微软雅黑"));
    m_sectionFont.CreatePointFont(100, _T("微软雅黑"));
    SetFont(&m_listFont);

    GetDlgItem(IDC_BOOKID_EDIT)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
    GetDlgItem(IDC_BOOKNAME_EDIT)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
    GetDlgItem(IDC_PRICE_EDIT)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
    GetDlgItem(IDC_EXTRA1_EDIT)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
    GetDlgItem(IDC_EXTRA2_EDIT)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

    m_typeCombo.AddString(_T("纸质图书"));
    m_typeCombo.AddString(_T("电子图书"));
    m_typeCombo.SetCurSel(0);
    updateExtraLabels();

    m_bookList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    m_bookList.InsertColumn(0, _T("编号"),   LVCFMT_LEFT, 80);
    m_bookList.InsertColumn(1, _T("书名"),   LVCFMT_LEFT, 230);
    m_bookList.InsertColumn(2, _T("价格"),   LVCFMT_LEFT, 78);
    m_bookList.InsertColumn(3, _T("类型"),   LVCFMT_LEFT, 70);
    m_bookList.InsertColumn(4, _T("附加1"),  LVCFMT_LEFT, 78);
    m_bookList.InsertColumn(5, _T("附加2"),  LVCFMT_LEFT, 62);

    // CListCtrl 行高取决于小图标尺寸，塞一个空白图标撑高
    CImageList il; il.Create(1, 28, ILC_COLOR32, 0, 1);
    m_bookList.SetImageList(&il, LVSIL_SMALL);
    il.Detach();

    m_bookList.SetBkColor(W11_SURFACE);
    m_bookList.SetTextBkColor(W11_SURFACE);
    m_bookList.SetTextColor(W11_INK);

    m_sectionFont.CreatePointFont(120, _T("微软雅黑"));
    m_statusFont.CreatePointFont(120, _T("微软雅黑"));
    GetDlgItem(IDC_SECTION_TITLE1)->SetFont(&m_sectionFont);
    GetDlgItem(IDC_SECTION_TITLE2)->SetFont(&m_sectionFont);
    GetDlgItem(IDC_STATUS_TEXT)->SetFont(&m_statusFont);

    CWnd* pChild = GetWindow(GW_CHILD);
    while (pChild) {
        TCHAR cls[32];
        ::GetClassName(pChild->GetSafeHwnd(), cls, 32);
        if (_tcscmp(cls, _T("Button")) == 0 && (pChild->GetStyle() & 0xF) == BS_GROUPBOX)
            ::SetWindowTheme(pChild->GetSafeHwnd(), _T(""), _T(""));
        pChild = pChild->GetNextWindow();
    }

    styleButtons();
    styleList();

    GetDlgItem(IDC_BOOKID_EDIT)->EnableWindow(TRUE);
    GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_BOOKID_EDIT)->SetFocus();

    if (!m_mgr.loadFile())
        AfxMessageBox(_T("未找到 books.txt，列表为空。\n可手动添加图书后点击 [保存] 生成文件。"),
                      MB_OK | MB_ICONINFORMATION);
    syncListFromVector();

    return TRUE;
}

HBRUSH MainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (nCtlColor == CTLCOLOR_STATIC) {
        int id = pWnd->GetDlgCtrlID();
        if (id == IDC_SECTION_TITLE1 || id == IDC_SECTION_TITLE2) {
            pDC->SetBkColor(W11_BG);
            pDC->SetTextColor(W11_CYAN);
            return m_bgBrush;
        }
        if (id == IDC_STATUS_TEXT) {
            pDC->SetBkColor(W11_SURFACE);
            pDC->SetTextColor(m_statusOk ? W11_CYAN : W11_DANGER);
            static CBrush sb(W11_SURFACE);
            return sb;
        }
        if (id == IDC_EXTRA1_LABEL || id == IDC_EXTRA2_LABEL) {
            // Opaque bg — these labels change text (页数⇔格式), must erase old pixels
            pDC->SetBkColor(W11_BG);
            pDC->SetTextColor(W11_INK);
            return m_bgBrush;
        }
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(W11_INK);
        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    if (nCtlColor == CTLCOLOR_DLG) {
        pDC->SetBkColor(W11_BG);
        return m_bgBrush;
    }
    if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX) {
        pDC->SetBkColor(DARK_INPUT);
        pDC->SetTextColor(W11_INK);
        static CBrush eb(DARK_INPUT);
        return eb;
    }
    return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

void MainDlg::styleButtons()
{
    m_btnAdd.SubclassDlgItem(IDC_ADD_BUTTON, this);
    m_btnAdd.ModifyStyle(0, BS_OWNERDRAW);
    m_btnAdd.setColors(BTN_ADD_BG, BTN_ADD_HOVER, BTN_ADD_PRESS, BTN_TEXT, BTN_TEXT, BTN_TEXT);

    m_btnModify.SubclassDlgItem(IDC_MODIFY_BUTTON, this);
    m_btnModify.ModifyStyle(0, BS_OWNERDRAW);
    m_btnModify.setColors(BTN_EDIT_BG, BTN_EDIT_HOVER, BTN_EDIT_PRESS, BTN_TEXT, BTN_TEXT, BTN_TEXT);

    m_btnDelete.SubclassDlgItem(IDC_DELETE_BUTTON, this);
    m_btnDelete.ModifyStyle(0, BS_OWNERDRAW);
    m_btnDelete.setColors(BTN_DEL_BG, BTN_DEL_HOVER, BTN_DEL_PRESS, BTN_TEXT, BTN_TEXT, BTN_TEXT);

    m_btnSearch.SubclassDlgItem(IDC_SEARCH_BUTTON, this);
    m_btnSearch.ModifyStyle(0, BS_OWNERDRAW);
    m_btnSearch.setColors(BTN_SEARCH_BG, BTN_SEARCH_HVR, BTN_SEARCH_PRS, BTN_TEXT, BTN_TEXT, BTN_TEXT);

    m_btnSort.SubclassDlgItem(IDC_SORT_BUTTON, this);
    m_btnSort.ModifyStyle(0, BS_OWNERDRAW);
    m_btnSort.setColors(BTN_SORT_BG, BTN_SORT_HOVER, BTN_SORT_PRESS, BTN_TEXT, BTN_TEXT, BTN_TEXT);

    m_btnSave.SubclassDlgItem(IDC_SAVE_BUTTON, this);
    m_btnSave.ModifyStyle(0, BS_OWNERDRAW);
    m_btnSave.setColors(BTN_SAVE_BG, BTN_SAVE_HOVER, BTN_SAVE_PRESS, BTN_TEXT, BTN_TEXT, BTN_TEXT);

    m_btnLoad.SubclassDlgItem(IDC_LOAD_BUTTON, this);
    m_btnLoad.ModifyStyle(0, BS_OWNERDRAW);
    m_btnLoad.setColors(BTN_LOAD_BG, BTN_LOAD_HOVER, BTN_LOAD_PRESS, BTN_TEXT, BTN_TEXT, BTN_TEXT);

    m_btnExit.SubclassDlgItem(IDC_EXIT_BUTTON, this);
    m_btnExit.ModifyStyle(0, BS_OWNERDRAW);
    m_btnExit.setColors(BTN_EXIT_BG, BTN_EXIT_HOVER, BTN_EXIT_PRESS, BTN_TEXT, BTN_TEXT, BTN_TEXT);
}

void MainDlg::styleList()
{
    m_bookList.SetFont(&m_listFont);
}

BOOL MainDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect rc;
    GetClientRect(&rc);
    pDC->FillSolidRect(&rc, W11_BG);
    return TRUE;
}

void MainDlg::OnPaint()
{
    // GROUPBOX controls provide section framing and titles.
    // No custom painting needed.
    CDialogEx::OnPaint();
}

void MainDlg::OnNMCustomdrawBookList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLVCUSTOMDRAW lvCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
    *pResult = CDRF_DODEFAULT;

    switch (lvCD->nmcd.dwDrawStage) {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;  // 告诉系统我们要逐行自己画
        return;

    case CDDS_ITEMPREPAINT: {  // 每行绘制前调一次，我们设背景色
        int nRow = (int)lvCD->nmcd.dwItemSpec;
        CDC* pDC = CDC::FromHandle(lvCD->nmcd.hdc);
        CRect rc = lvCD->nmcd.rc;

        bool sel = (m_bookList.GetItemState(nRow, LVIS_SELECTED) & LVIS_SELECTED) != 0;

        if (sel) {
            pDC->FillSolidRect(&rc, W11_CYAN);
            lvCD->clrText = RGB(10,10,25);
            lvCD->clrTextBk = W11_CYAN;
        } else if (nRow % 2 == 0) {
            pDC->FillSolidRect(&rc, W11_ROW_ALT);
            lvCD->clrText = W11_INK;
            lvCD->clrTextBk = W11_ROW_ALT;
        } else {
            pDC->FillSolidRect(&rc, W11_SURFACE);
            lvCD->clrText = W11_INK;
            lvCD->clrTextBk = W11_SURFACE;
        }

        *pResult = CDRF_NEWFONT;
        return;
    }
    }
}

void MainDlg::showStatus(const CString& msg, bool ok)
{
    m_statusOk = ok;
    m_statusMsg = msg;
    m_statusText.SetWindowText(msg);
    m_statusText.Invalidate();
    if (ok)
        MessageBeep(MB_ICONINFORMATION);
    else
        MessageBeep(MB_ICONERROR);
}

void MainDlg::syncListFromVector()
{
    m_bookList.DeleteAllItems();
    const auto& books = m_mgr.getBooks();
    for (size_t i = 0; i < books.size(); ++i) {
        const Book* b = books[i].get();
        int idx = m_bookList.InsertItem((int)i, Utf8ToCString(b->getBookID()));
        m_bookList.SetItemText(idx, 1, Utf8ToCString(b->getBookName()));

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << b->getPrice();
        m_bookList.SetItemText(idx, 2, CString(oss.str().c_str()));

        m_bookList.SetItemText(idx, 3, Utf8ToCString(b->getBookType()));

        // dynamic_cast 区分具体类型，填不同的附加列
        if (auto* pb = dynamic_cast<const PaperBook*>(b)) {
            m_bookList.SetItemText(idx, 4, CString((std::to_string(pb->getPageCount()) + " 页").c_str()));
            m_bookList.SetItemText(idx, 5, _T("--"));
        } else if (auto* eb = dynamic_cast<const EBook*>(b)) {
            m_bookList.SetItemText(idx, 4, Utf8ToCString(eb->getFormat()));
            std::ostringstream fs;
            fs << std::fixed << std::setprecision(2) << eb->getFileSize() << " MB";
            m_bookList.SetItemText(idx, 5, CString(fs.str().c_str()));
        }
    }
}

void MainDlg::clearEditFields()
{
    m_bookID = _T("");
    m_bookName = _T("");
    m_priceStr = _T("");
    m_extra1 = _T("");
    m_extra2 = _T("");
    UpdateData(FALSE);
    GetDlgItem(IDC_BOOKID_EDIT)->EnableWindow(TRUE);
    GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(FALSE);
}

void MainDlg::updateExtraLabels()
{
    int sel = m_typeCombo.GetCurSel();
    if (sel == 0) {
        SetDlgItemText(IDC_EXTRA1_LABEL, _T("页数："));
        GetDlgItem(IDC_EXTRA1_LABEL)->Invalidate();
        GetDlgItem(IDC_EXTRA2_LABEL)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EXTRA2_EDIT)->ShowWindow(SW_HIDE);
        m_extra2 = _T("");
        GetDlgItem(IDC_EXTRA2_EDIT)->SetWindowText(_T(""));
        // 隐藏的控件区域要强制重绘，否则会有灰色残影
        CRect r; GetDlgItem(IDC_EXTRA2_LABEL)->GetWindowRect(&r); ScreenToClient(&r);
        InvalidateRect(&r);
    } else {
        SetDlgItemText(IDC_EXTRA1_LABEL, _T("格式："));
        GetDlgItem(IDC_EXTRA1_LABEL)->Invalidate();
        SetDlgItemText(IDC_EXTRA2_LABEL, _T("大小(MB)："));
        GetDlgItem(IDC_EXTRA2_LABEL)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_EXTRA2_LABEL)->Invalidate();
        GetDlgItem(IDC_EXTRA2_EDIT)->ShowWindow(SW_SHOW);
        m_extra2 = _T("");
        GetDlgItem(IDC_EXTRA2_EDIT)->SetWindowText(_T(""));
    }
}

void MainDlg::fillEditFromBook(const Book* book)
{
    if (!book) return;
    m_bookID = Utf8ToCString(book->getBookID());
    m_bookName = Utf8ToCString(book->getBookName());

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << book->getPrice();
    m_priceStr = CString(oss.str().c_str());

    if (auto* pb = dynamic_cast<const PaperBook*>(book)) {
        m_typeCombo.SetCurSel(0);
        updateExtraLabels();  // 先清旧标签再填新值，避免残留

        m_extra1 = CString(std::to_string(pb->getPageCount()).c_str());
        m_extra2 = _T("");
    } else if (auto* eb = dynamic_cast<const EBook*>(book)) {
        m_typeCombo.SetCurSel(1);
        updateExtraLabels();
        m_extra1 = Utf8ToCString(eb->getFormat());
        std::ostringstream fs;
        fs << std::fixed << std::setprecision(2) << eb->getFileSize();
        m_extra2 = CString(fs.str().c_str());
    }
    UpdateData(FALSE);
}

int MainDlg::getSelectedIndex()
{
    POSITION pos = m_bookList.GetFirstSelectedItemPosition();
    if (!pos) return -1;
    return m_bookList.GetNextSelectedItem(pos);
}

void MainDlg::onSelectBook()
{
    int idx = getSelectedIndex();
    if (idx < 0) return;

    CString selID = m_bookList.GetItemText(idx, 0);
    Book* book = m_mgr.findBook(ToStr(selID));
    if (book) {
        fillEditFromBook(book);
        GetDlgItem(IDC_BOOKID_EDIT)->EnableWindow(FALSE);
        GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(TRUE);
        GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(TRUE);
    }
}

void MainDlg::OnCbnSelchangeTypeCombo()
{
    clearEditFields();
    updateExtraLabels();
}

void MainDlg::OnLvnItemchangedBookList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    if (pNMLV->uChanged & LVIF_STATE) {
        bool selected = (pNMLV->uNewState & LVIS_SELECTED) != 0;
        bool wasSelected = (pNMLV->uOldState & LVIS_SELECTED) != 0;
        if (selected && !wasSelected)
            onSelectBook();
        else if (!selected && wasSelected)
            clearEditFields();
    }
    *pResult = 0;
}

void MainDlg::OnBnClickedAdd()
{
    UpdateData(TRUE);

    std::string id(ToStr(m_bookID));
    std::string name(ToStr(m_bookName));

    if (id.empty() || name.empty()) {
        AfxMessageBox(_T("编号和书名不能为空。"), MB_OK | MB_ICONWARNING);
        return;
    }
    if (m_mgr.isBookIDExist(id)) {
        AfxMessageBox(_T("该编号已存在。"), MB_OK | MB_ICONWARNING);
        return;
    }

    double price = 0.0;
    try { price = std::stod(ToStr(m_priceStr)); }
    catch (...) { AfxMessageBox(_T("价格格式无效。"), MB_OK | MB_ICONWARNING); return; }
    if (price < 0) {
        AfxMessageBox(_T("价格不能为负数。"), MB_OK | MB_ICONWARNING);
        return;
    }

    bool ok = false;
    if (m_typeCombo.GetCurSel() == 0) {
        int pages = 0;
        try { pages = std::stoi(ToStr(m_extra1)); }
        catch (...) { AfxMessageBox(_T("页数格式无效。"), MB_OK | MB_ICONWARNING); return; }
        if (pages <= 0) {
            AfxMessageBox(_T("页数必须大于零。"), MB_OK | MB_ICONWARNING);
            return;
        }
        ok = m_mgr.addPaperBook(id, name, price, pages);
    } else {
        std::string fmt(ToStr(m_extra1));
        if (fmt.empty()) {
            AfxMessageBox(_T("格式不能为空。"), MB_OK | MB_ICONWARNING);
            return;
        }
        double fsize = 0.0;
        try { fsize = std::stod(ToStr(m_extra2)); }
        catch (...) { AfxMessageBox(_T("文件大小格式无效。"), MB_OK | MB_ICONWARNING); return; }
        if (fsize <= 0) {
            AfxMessageBox(_T("文件大小必须大于零。"), MB_OK | MB_ICONWARNING);
            return;
        }
        ok = m_mgr.addEBook(id, name, price, fmt, fsize);
    }

    if (ok) {
        syncListFromVector();
        clearEditFields();
        GetDlgItem(IDC_BOOKID_EDIT)->EnableWindow(TRUE);
        showStatus(_T("已添加 ") + m_bookName, true);
    }
}

void MainDlg::OnBnClickedModify()
{
    int idx = getSelectedIndex();
    if (idx < 0) return;

    CString selID = m_bookList.GetItemText(idx, 0);
    Book* book = m_mgr.findBook(ToStr(selID));
    if (!book) return;

    UpdateData(TRUE);

    std::string newName(ToStr(m_bookName));
    double newPrice = 0.0;
    try { newPrice = std::stod(ToStr(m_priceStr)); }
    catch (...) { AfxMessageBox(_T("价格格式无效。"), MB_OK | MB_ICONWARNING); return; }
    if (newPrice < 0) {
        AfxMessageBox(_T("价格不能为负数。"), MB_OK | MB_ICONWARNING);
        return;
    }

    if (auto* pb = dynamic_cast<PaperBook*>(book)) {
        int newPages = 0;
        try { newPages = std::stoi(ToStr(m_extra1)); }
        catch (...) { AfxMessageBox(_T("页数格式无效。"), MB_OK | MB_ICONWARNING); return; }
        if (newPages <= 0) {
            AfxMessageBox(_T("页数必须大于零。"), MB_OK | MB_ICONWARNING);
            return;
        }
        if (!newName.empty()) pb->setBookName(newName);
        pb->setPrice(newPrice);
        pb->setPageCount(newPages);
    } else if (auto* eb = dynamic_cast<EBook*>(book)) {
        std::string newFmt(ToStr(m_extra1));
        double newFsize = 0.0;
        try { newFsize = std::stod(ToStr(m_extra2)); }
        catch (...) { AfxMessageBox(_T("文件大小格式无效。"), MB_OK | MB_ICONWARNING); return; }
        if (newFsize <= 0) {
            AfxMessageBox(_T("文件大小必须大于零。"), MB_OK | MB_ICONWARNING);
            return;
        }
        if (!newName.empty()) eb->setBookName(newName);
        eb->setPrice(newPrice);
        if (!newFmt.empty()) eb->setFormat(newFmt);
        eb->setFileSize(newFsize);
    }

    syncListFromVector();
    clearEditFields();
    GetDlgItem(IDC_BOOKID_EDIT)->EnableWindow(TRUE);
    showStatus(_T("已修改 ") + m_bookName, true);
}

void MainDlg::OnBnClickedDelete()
{
    int idx = getSelectedIndex();
    if (idx < 0) return;

    CString selID = m_bookList.GetItemText(idx, 0);
    CString selName = m_bookList.GetItemText(idx, 1);

    CString msg;
    msg.Format(_T("确定要删除 \"%s\" 吗？"), (LPCTSTR)selName);
    if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;

    m_mgr.deleteBook(ToStr(selID));
    syncListFromVector();
    clearEditFields();
    showStatus(_T("已删除"), true);
}


void MainDlg::OnBnClickedSearch()
{
    UpdateData(TRUE);
    std::string id(ToStr(m_bookID));
    if (id.empty()) {
        AfxMessageBox(_T("请在编号框中输入要查询的编号。"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    const Book* book = m_mgr.findBook(id);
    if (!book) {
        showStatus(_T("未找到编号为 ") + m_bookID + _T(" 的图书"), false);
        return;
    }

    for (int i = 0; i < m_bookList.GetItemCount(); ++i) {
        if (m_bookList.GetItemText(i, 0) == Utf8ToCString(id)) {
            m_bookList.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED,
                                    LVIS_SELECTED | LVIS_FOCUSED);
            m_bookList.EnsureVisible(i, FALSE);
            onSelectBook();
            break;
        }
    }
}

void MainDlg::OnBnClickedSort()
{
    if (m_mgr.getCount() < 2) {
        AfxMessageBox(_T("图书数量不足，无需排序。"), MB_OK | MB_ICONINFORMATION);
        return;
    }
    m_mgr.sortBooks();
    syncListFromVector();
    clearEditFields();
    GetDlgItem(IDC_BOOKID_EDIT)->EnableWindow(TRUE);
    showStatus(_T("已按价格升序排列"), true);
}

void MainDlg::OnBnClickedSave()
{
    if (m_mgr.saveFile()) {
        CString m; m.Format(_T("已保存 %zu 条记录到 books.txt"), m_mgr.getCount());
        showStatus(m, true);
    } else {
        showStatus(_T("保存失败，无法写入文件"), false);
    }
}

void MainDlg::OnBnClickedLoad()
{
    if (m_mgr.loadFile()) {
        syncListFromVector();
        clearEditFields();
        GetDlgItem(IDC_BOOKID_EDIT)->EnableWindow(TRUE);
        CString msg;
        msg.Format(_T("已加载 %zu 条记录"), m_mgr.getCount());
        showStatus(msg, true);
    } else {
        showStatus(_T("未找到 books.txt 文件"), false);
    }
}

void MainDlg::OnBnClickedExit()
{
    EndDialog(IDOK);
}
