#pragma once

#include <string>
#include <fstream>
#include <iostream>

// 图书抽象基类，所有图书类型的公共接口
class Book {
    std::string m_bookID;    // 唯一编号
    std::string m_bookName;  // 书名
    double      m_price;     // 价格

public:
    Book() : m_price(0.0) {}
    Book(const std::string& id, const std::string& name, double prc);
    virtual ~Book() = default;  // 虚析构，保证 unique_ptr 正确释放子类

    // 纯虚 —— 子类各自实现展示方式
    virtual void        showInfo()    const = 0;
    virtual std::string getBookType() const = 0;

    // saveToFile 基类给空实现，子类按需重写
    virtual void saveToFile(std::ofstream& ofs) const;

    // 属性访问
    const std::string& getBookID()   const { return m_bookID; }
    const std::string& getBookName() const { return m_bookName; }
    double             getPrice()    const { return m_price; }

    void setBookID(const std::string& id);
    void setBookName(const std::string& nm);
    void setPrice(double p);

    // 按价格比较，用于排序
    bool operator<(const Book& rhs) const;

    // 流输出（题目要求）
    friend std::ostream& operator<<(std::ostream& os, const Book& b);
};
