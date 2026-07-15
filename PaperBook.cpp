#include "pch.h"
#include "PaperBook.h"
#include <sstream>
#include <iomanip>

PaperBook::PaperBook(const std::string& id, const std::string& name,
                     double prc, int pages)
    : Book(id, name, prc), m_pageCount(pages)
{}

void PaperBook::showInfo() const {
    std::ostringstream oss;
    oss << "编号：" << getBookID()   << "\r\n"
        << "书名：" << getBookName() << "\r\n"
        << "类型：" << getBookType() << "\r\n"
        << "价格：" << std::fixed << std::setprecision(2) << getPrice() << " 元\r\n"
        << "页数：" << m_pageCount   << " 页";
    CString msg(oss.str().c_str());
    AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
}

void PaperBook::saveToFile(std::ofstream& ofs) const {
    ofs << "PAPER|" << getBookID() << "|" << getBookName()
        << "|" << getPrice() << "|" << m_pageCount << "\n";
}

void PaperBook::setPageCount(int n) {
    if (n > 0) m_pageCount = n;
}
