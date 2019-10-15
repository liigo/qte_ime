# qte_ime

a simple chinese/pinyin input method (IME) / software keyboad, for Qt Embedded (4.7/4.8)

# QTE IME 使用指南

本文介绍如何将中文输入法虚拟键盘集成到现有Qt Creator项目中。具体操作步骤如下。

## 1. 把 `ime` 目录复制到自己的Qt Creator项目目录内

下文所需的输入法相关文件均在 `ime` 目录内

## 2. 把 `inputcontext.cpp/.h` `keyboard.cpp/.h` 加入到项目中

方法是在Qt Creator内用鼠标右键点击项目，选择菜单“加入现有文件...”

## 3. 把 `res/imeres.qrc` 加入到项目中

方法同上

## 4. 修改 `main` 函数

新的`main`函数代码大致如下，仅供参考：

```
#include "mainwindow.h"
#include "ime/inputcontext.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(new CProxyStyle());
    app.setInputContext(new CInputContext());

    QTextCodec* utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(utf8);

    MainWindow w;
    w.show();

    return app.exec();
}
```

相比Qt Creator生成的`main`函数，主要有以下几项修改：

- 包含头文件 `"ime/inputcontext.h"` 和 `<QTextCodec>`
- 调用 `app.setStype()` `app.setInputContext()`
- 调用 `QTextCodec::setCodecForCStrings(utf8)`

其中`QTextCodec`相关代码是为了正确显示汉字而做的必要的工作，跟输入法本身并无直接关联，应用程序也可通过其他途径显示汉字。

## 阻止特定组件弹出输入法虚拟键盘

```
ui->lineEdit_no_ime->setAttribute(Qt::WA_InputMethodEnabled, false);
```
