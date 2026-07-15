#include "pch.h"
#include "Book.h"

Book::Book(const std::string& id, const std::string& name, double prc)
    : m_bookID(id), m_bookName(name), m_price(prc)
{}

void Book::saveToFile(std::ofstream& ofs) const {
    (void)ofs;
}

void Book::setBookID(const std::string& id) {
    if (!id.empty()) m_bookID = id;
}

void Book::setBookName(const std::string& nm) {
    if (!nm.empty()) m_bookName = nm;
}

void Book::setPrice(double p) {
    if (p >= 0.0) m_price = p;
}

bool Book::operator<(const Book& rhs) const {
    return m_price < rhs.m_price;
}

std::ostream& operator<<(std::ostream& os, const Book& b) {
    os << b.getBookID() << " | " << b.getBookName()
       << " | " << b.getPrice() << " | " << b.getBookType();
    return os;
}
