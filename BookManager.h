#pragma once

#include "PaperBook.h"
#include "EBook.h"
#include <vector>
#include <map>
#include <memory>
#include <string>

// 图书数据管理类 —— 维护 vector 容器和 map 索引
class BookManager {
    std::vector<std::unique_ptr<Book>> m_books;   // 智能指针存书
    std::map<std::string, int>         m_index;   // 编号到下标，O(log n) 查找

    void rebuildIndex();  // 删书或排序后索引会乱，重新遍历 vector 生成 map

public:
    BookManager() = default;

    bool isBookIDExist(const std::string& id) const;

    // 添加（返回是否成功）
    bool addPaperBook(const std::string& id, const std::string& name,
                      double price, int pages);
    bool addEBook(const std::string& id, const std::string& name,
                  double price, const std::string& fmt, double fsize);

    // 按编号查找，返回指针（未找到返回 nullptr）
    Book*       findBook(const std::string& id);
    const Book* findBook(const std::string& id) const;

    Book* searchBook(const std::string& id)       { return findBook(id); }
    void  showAllBooks() const;   // 遍历 m_books，调用每个元素的 showInfo()

    bool deleteBook(const std::string& id);
    void sortBooks();        // 按价格升序

    bool saveFile() const;   // 保存到 books.txt
    bool loadFile();         // 从 books.txt 加载

    const std::vector<std::unique_ptr<Book>>& getBooks() const { return m_books; }
    size_t getCount() const { return m_books.size(); }
};
