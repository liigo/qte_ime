#ifndef IME_KEYBOARD_H_
#define IME_KEYBOARD_H_

#include <QWidget>
#include <QButtonGroup>
#include <QFile>
#include <QString>
#include <QMultiMap>
#include <QList>
#include <QMessageBox>
#include <QDebug>
#include <QMouseEvent>

#include <iostream>

using namespace std;

class QPushButton;
class QLineEdit;

class CKeyboard: public QWidget
{
    Q_OBJECT

public:
    enum {
        KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
        KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
        KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
        KEY_SHIFT, KEY_BACK, KEY_SYMBOLS, KEY_LANGUAGE, KEY_SPACE, KEY_ENTER, KEY_RETURN,
        KEY_NUM/*必须在最后面*/
    };

    CKeyboard(QWidget *parent = NULL);
    ~CKeyboard();

    QLineEdit *pline;
private:
    void update_key_text();
    void update_letter_text();
    void update_num_text();
    void update_symbols();
    void update_langaue();
    void key_init();
    void datebase_init();

    void change_page(int index);
    void clear_page(void);
    void pre_page();
    void next_page();
    void cn_matching(QString &pinyin);
    void delete_focus_text(void);

signals:
    // 向当前焦点窗口写入文本，详见CInputContext::sendCharacter()
    void characterGenerated(QString keyValue);

private slots:
    void slot_buttonClicked(int id);

private:
    static bool bUpper;
    static bool bSymbol;
    static bool bLanguage;
    QPushButton *ppbKey[KEY_NUM];
    QButtonGroup* button_group;
    QHash<QString,QString> cn_map; // key:拼音 value:以空格分隔的字词
    QList<QString> cn_list; // 候选词
    QString cn_input_text;
    int page_index;

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
private:
    QPoint mLastMousePosition;
    bool mMoving;
};

#endif //IME_KEYBOARD_H_
