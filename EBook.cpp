#include "pch.h"
#include "EBook.h"
#include <sstream>
#include <iomanip>

EBook::EBook(const std::string& id, const std::string& name,
             double prc, const std::string& fmt, double sz)
    : Book(id, name, prc), m_format(fmt), m_fileSize(sz)
{}

void EBook::showInfo() const {
    std::ostringstream oss;
    oss << "编号："   << getBookID()   << "\r\n"
        << "书名："   << getBookName() << "\r\n"
        << "类型："   << getBookType() << "\r\n"
        << "价格："   << std::fixed << std::setprecision(2) << getPrice() << " 元\r\n"
        << "格式："   << m_format      << "\r\n"
        << "大小："   << std::fixed << std::setprecision(2) << m_fileSize  << " MB";
    CString msg(oss.str().c_str());
    AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
}

void EBook::saveToFile(std::ofstream& ofs) const {
    ofs << "EBOOK|" << getBookID() << "|" << getBookName()
        << "|" << getPrice() << "|" << m_format
        << "|" << m_fileSize << "\n";
}

void EBook::setFormat(const std::string& f) {
    if (!f.empty()) m_format = f;
}

void EBook::setFileSize(double s) {
    if (s > 0.0) m_fileSize = s;
}
