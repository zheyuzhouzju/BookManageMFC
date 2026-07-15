#pragma once

#include "Book.h"

class PaperBook : public Book {
    int m_pageCount;

public:
    PaperBook() : Book(), m_pageCount(0) {}
    PaperBook(const std::string& id, const std::string& name,
              double prc, int pages);

    void        showInfo()    const override;
    std::string getBookType() const override { return "纸质图书"; }
    void        saveToFile(std::ofstream& ofs) const override;

    int  getPageCount() const { return m_pageCount; }
    void setPageCount(int n);
};
