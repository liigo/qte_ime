#ifndef IME_INPUTCONTEXT_H_
#define IME_INPUTCONTEXT_H_

// Liigo 20190909:
// 在前人无名先辈基础上做了一些补充性工作，主要涉及：
// - 修正BUG
// - 文件名/类名/变量名/代码风格修改
// - 重构/简化/消除重复代码，增加注释
// - 调整符号键位布局，尽量与标准键盘一致，支持中英文字符切换
// - 修改词库格式，词汇数量不变而文件大小减少30%，且更便于维护
// - 优化词库/词频/编码，如四字以上词语"中央电视台"zydt->zydst

#include <QInputContext>
#include <QProxyStyle>
#include <QTimer>

class CKeyboard;

class CInputContext: public QInputContext {
    Q_OBJECT

public:
    CInputContext(QObject *parent = NULL);
    ~CInputContext();
    static void showKeyboard();
    static void hideKeyboard();

protected:
    bool filterEvent(const QEvent* event);
    QString identifierName();
    QString language();
    void reset();
    bool isComposing() const;
    void mouseHandler (int x, QMouseEvent* event);
private slots:
    void sendCharacter(QString key_value);
    void timeout();
private:
    void updatePosition();
    void startMyTimer(bool bShow);

private:
    CKeyboard* pKeyboard;
    QTimer timer;
    bool bShow;
    QPoint panelStartPos;
    int increment;
};

class CProxyStyle: public QProxyStyle
{
public:
    int styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData) const;
};


#endif //IME_INPUTCONTEXT_H_
