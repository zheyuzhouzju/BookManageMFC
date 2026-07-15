#pragma once

#include "Book.h"

class EBook : public Book {
    std::string m_format;
    double      m_fileSize;

public:
    EBook() : Book(), m_fileSize(0.0) {}
    EBook(const std::string& id, const std::string& name,
          double prc, const std::string& fmt, double sz);

    void        showInfo()    const override;
    std::string getBookType() const override { return "电子图书"; }
    void        saveToFile(std::ofstream& ofs) const override;

    const std::string& getFormat()   const { return m_format; }
    double             getFileSize() const { return m_fileSize; }

    void setFormat(const std::string& f);
    void setFileSize(double s);
};
