#include "pch.h"
#include "BookManager.h"
#include <fstream>
#include <algorithm>
#include <sstream>

void BookManager::rebuildIndex() {
    m_index.clear();
    int n = (int)m_books.size();
    for (int i = 0; i < n; ++i)
        m_index[m_books[i]->getBookID()] = i;
}

bool BookManager::isBookIDExist(const std::string& id) const {
    return m_index.find(id) != m_index.end();
}

bool BookManager::addPaperBook(const std::string& id, const std::string& name,
                               double price, int pages) {
    if (isBookIDExist(id)) return false;
    m_books.push_back(std::make_unique<PaperBook>(id, name, price, pages));
    m_index[id] = (int)m_books.size() - 1;
    return true;
}

bool BookManager::addEBook(const std::string& id, const std::string& name,
                           double price, const std::string& fmt, double fsize) {
    if (isBookIDExist(id)) return false;
    m_books.push_back(std::make_unique<EBook>(id, name, price, fmt, fsize));
    m_index[id] = (int)m_books.size() - 1;
    return true;
}

Book* BookManager::findBook(const std::string& id) {
    auto it = m_index.find(id);
    if (it == m_index.end()) return nullptr;
    return m_books[it->second].get();
}

const Book* BookManager::findBook(const std::string& id) const {
    auto it = m_index.find(id);
    if (it == m_index.end()) return nullptr;
    return m_books[it->second].get();
}

void BookManager::showAllBooks() const {
    for (const auto& bp : m_books)
        bp->showInfo();
}

bool BookManager::deleteBook(const std::string& id) {
    auto it = m_index.find(id);
    if (it == m_index.end()) return false;
    m_books.erase(m_books.begin() + it->second);
    rebuildIndex();
    return true;
}

void BookManager::sortBooks() {
    if (m_books.size() < 2) return;
    std::sort(m_books.begin(), m_books.end(),
        [](const std::unique_ptr<Book>& a, const std::unique_ptr<Book>& b) {
            return *a < *b;
        });
    rebuildIndex();
}

bool BookManager::saveFile() const {
    std::ofstream ofs("books.txt");
    if (!ofs.is_open()) return false;
    ofs << "类型|编号|书名|价格|页数/格式|文件大小\n";
    for (const auto& bp : m_books)
        bp->saveToFile(ofs);
    return true;
}

bool BookManager::loadFile() {
    std::ifstream ifs("books.txt");
    if (!ifs.is_open()) return false;

    m_books.clear();
    m_index.clear();

    std::string line;
    int loaded = 0;
    bool firstLine = true;

    while (std::getline(ifs, line)) {
        if (line.empty()) continue;

        if (firstLine && line.find("类型|") == 0) {
            firstLine = false;
            continue;
        }
        firstLine = false;

        std::string fields[6];
        std::stringstream ss(line);
        int fc = 0;
        for (int i = 0; i < 6; ++i) {
            if (!std::getline(ss, fields[i], '|')) break;
            ++fc;
        }

        if (fc < 5) continue;

        const std::string& type = fields[0];
        std::string bid  = fields[1];
        std::string bnm  = fields[2];
        double bpr;
        try { bpr = std::stod(fields[3]); }
        catch (...) { continue; }

        if (type == "PAPER" && fc >= 5) {
            int pg;
            try { pg = std::stoi(fields[4]); }
            catch (...) { continue; }
            m_books.push_back(std::make_unique<PaperBook>(bid, bnm, bpr, pg));
            ++loaded;
        } else if (type == "EBOOK" && fc >= 6) {
            const std::string& fmt = fields[4];
            double fsz;
            try { fsz = std::stod(fields[5]); }
            catch (...) { continue; }
            m_books.push_back(std::make_unique<EBook>(bid, bnm, bpr, fmt, fsz));
            ++loaded;
        }
    }

    rebuildIndex();
    return true;
}
