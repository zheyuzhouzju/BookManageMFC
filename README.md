# BookManageMFC — 图书管理系统

基于 MFC 的 Windows 桌面图书管理系统，C++ 面向对象课程大作业。

## 功能

- 纸质书与电子书两种类型的增删查改
- 编号唯一性校验、按价格排序
- 登录认证系统
- 自定义 ModernButton 圆角按钮控件
- 数据持久化（books.txt 读写）

## 架构

```
Book (抽象基类)
├── PaperBook  — 页数
└── EBook      — 文件格式 + 文件大小

BookManager   — vector<unique_ptr<Book>> + map 索引，O(log n) 查找
MainDlg       — 主界面（列表展示、添加、删除、排序）
LoginDlg      — 登录对话框
ModernButton  — 自绘圆角按钮（BS_OWNERDRAW）
```

## 编译运行

Visual Studio 2022 打开 `BookManageMFC.sln`：
1. Visual Studio 2022 打开 `BookManageMFC.sln`
2. 如报错 MSB8020，项目属性 → 平台工具集 → 改为 `v143`
3. 确保已安装 "C++ MFC for v14xx build tools"
4. Release x64 生成

## 文件说明

| 文件 | 说明 |
|------|------|
| `Book.h/cpp` | 图书抽象基类 |
| `PaperBook.h/cpp` | 纸质书派生类 |
| `EBook.h/cpp` | 电子书派生类 |
| `BookManager.h/cpp` | 数据管理（增删查排） |
| `MainDlg.h/cpp` | 主界面 |
| `LoginDlg.h/cpp` | 登录界面 |
| `ModernButton.h` | 自绘圆角按钮 |
| `Colors.h` | 配色常量 |
| `class_diagram.png` | UML 类图 |
| `大程报告_终版.pdf` | 完整实验报告 |

## 技术栈

C++17/20 · MFC · Visual Studio 2022 · Windows GDI
