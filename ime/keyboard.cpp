#include "keyboard.h"
#include <QPushButton>
#include <QLayout>
#include <QApplication>
#include <QWidget>
#include <QLineEdit>

bool CKeyboard::bUpper   = false;
bool CKeyboard::bSymbol  = false;
bool CKeyboard::bLanguage = false; //默认为英文输入法

CKeyboard::CKeyboard(QWidget *parent):
    QWidget(parent,  Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint),
    button_group(NULL),
    page_index(0),pline(new QLineEdit)
{
    button_group = new QButtonGroup(this);
    key_init();
    datebase_init();
    //this->hide();
}

CKeyboard::~CKeyboard()
{
    if(button_group){
        delete button_group;
    }
}

void CKeyboard::slot_buttonClicked(int id)
{
	int key = id;
    if (key == KEY_SHIFT) {
        bUpper = !bUpper;
        bSymbol = false;
        update_key_text();
        ppbKey[KEY_1]->setEnabled(true);
        ppbKey[KEY_0]->setEnabled(true);
        return;
    } else if (key == KEY_SYMBOLS) {
        bSymbol = !bSymbol;
        update_symbols();
        ppbKey[KEY_1]->setEnabled(true);
        ppbKey[KEY_0]->setEnabled(true);
    } else if (key == KEY_LANGUAGE) {
        bLanguage = !bLanguage;
        bSymbol = false;
        update_key_text();
        if (bLanguage) {
            page_index = 0;
            change_page(0);
        } else {
            ppbKey[KEY_1]->setEnabled(true);
            ppbKey[KEY_0]->setEnabled(true);
        }
    } else if (key == KEY_BACK) {
        emit characterGenerated("\b");
        if (bLanguage && !bUpper) { //小写 中文
            int size = cn_input_text.size();
            if (size > 0) cn_input_text.resize(size-1);

            if (cn_input_text.size() < 1){
                clear_page();
            } else {
                cn_matching(cn_input_text);
            }
        }
    } else if (key == KEY_SPACE) {
        emit characterGenerated(" ");
    } else if (key == KEY_ENTER) {
        //emit characterGenerated("\n");
        QWidget* pFocusWidget = QApplication::focusWidget();
        pFocusWidget->clearFocus();
        this->hide();
    } else if (key == KEY_RETURN) {
        //cn_input_text.clear();
        QWidget* pFocusWidget = QApplication::focusWidget();
        pFocusWidget->clearFocus();
        this->hide();
    } else if(bSymbol && key <= KEY_Z) {
        if (key == KEY_7) {
            emit characterGenerated("&"); // 此时按钮标题文本实际是"&&"
        } else {
            emit characterGenerated(ppbKey[key]->text());
        }
	} else if ((key == KEY_1) && bLanguage) {
        pre_page();
    } else if ((key == KEY_0) && bLanguage) {
        next_page();
    } else {
        if (bLanguage && !bUpper) { // 中文 小写
            if ((KEY_2 <= key) && (key <= KEY_9)) { // 数字键2~9: 输出候选词
				if(ppbKey[key]->text().size() > 0) {
					delete_focus_text();
					emit characterGenerated(ppbKey[key]->text());
					clear_page();
				}
            } else if((KEY_A <= key) && (key <= KEY_Z)) { // 字母键: 输入拼音
                cn_input_text.push_back(ppbKey[key]->text());
                emit characterGenerated(ppbKey[key]->text());
            }
        } else if (bLanguage && bUpper) { // 中文 大写
            if((KEY_A <= key) && (key <= KEY_Z)) {
                emit characterGenerated(ppbKey[key]->text()); // 输出大写字母
            }
        } else {
            emit characterGenerated(ppbKey[key]->text());
        }

        if (bLanguage) {
            cn_matching(cn_input_text);
        }
    }
}

void CKeyboard::update_langaue()
{
    ppbKey[KEY_LANGUAGE]->setText(bLanguage ? "中" : "En");
    if(bLanguage) cn_input_text.clear();
}

void CKeyboard::update_symbols()
{
	// Liigo 20190906: 重新分配符号键位，尽量与标准键盘保持一致或相差不远。
	// 此处包含了标准键盘上的所有符号，外加上下左右四个箭头符号，不多不少。
	// 中文模式下尽量使用与同键位英文字符相对应的全角符号，略有变通。
	const static QString en_symbols[] = {
		")", "!", "@", "#", "$", "%", "^", "&&", "*", "(", // 0 - 9
		"←", ",", "<", "→", "+", "/", "\\", ":", "]", ";", // ABCDEFGHIJ
		"\'", "\"", "?", ".", "{", "}", "~", "-", "↓", "_", // KLMNOPQRST
		"[", ">", "↑", "|", "=", "`", // UVWXYZ
	};
	const static QString zh_symbols[] = {
		"）", "！", "@", "#", "￥", "‰", "…", "&&", "*", "（", // 0 - 9
		"←", "，", "《", "→", "+", "/", "、", "：", "’", "；", // ABCDEFGHIJ
		"\'", "\"", "？", "。", "“", "”", "～", "-", "↓", "_", // KLMNOPQRST
		"‘", "》", "↑", "|", "=", "—", // UVWXYZ
	};

    if (bSymbol) {
        for (int key = 0; key < KEY_NUM; key++) {
			if(bLanguage) {
				ppbKey[key]->setText(zh_symbols[key]);
			} else {
				ppbKey[key]->setText(en_symbols[key]);
			}
			if (key >= KEY_Z) break;
        }
    } else {
        update_key_text();
    }
}

void CKeyboard::update_letter_text()
{
    const char letters[] =
        {'a','b','c','d','e','f','g','h','i','j','k','l','m',
         'n','o','p','q','r','s','t','u','v','w','x','y','z'};

    for (int key = KEY_A; key <= KEY_Z; key++) {
        char letter = letters[key - KEY_A];
        if(bUpper) letter = toupper(letter);
        ppbKey[key]->setText(QString(letter));
    }

    ppbKey[KEY_SHIFT]->setText(bUpper ? "SHIFT" : "shift");
}

void CKeyboard::update_num_text()
{
    if (bLanguage && !bSymbol) {
        // 在中文模式下数字键2~9用作显示候选汉字和词语，两端1/0键显示为</>用作翻页。
        // FIXME(liigo) 20190906: 单个数字键显示三字或四字词语时相当拥挤。
        for (int key = KEY_0; key <= KEY_9; key++) {
            ppbKey[key]->setText("");
        }
        ppbKey[KEY_0]->setText(">");
        ppbKey[KEY_1]->setText("<");
    } else {
        const char* nums[] = {"0","1","2","3","4","5","6","7","8","9"};
        for (int key = KEY_0; key <= KEY_9; key++) {
            ppbKey[key]->setText(nums[key - KEY_0]);
        }
    }
}

void CKeyboard::update_key_text()
{
    update_num_text();
    update_letter_text();

    ppbKey[KEY_BACK]->setText("Back");
    ppbKey[KEY_SYMBOLS]->setText("!@#");
    ppbKey[KEY_SPACE]->setText("Space");
    ppbKey[KEY_ENTER]->setText("Enter");
    ppbKey[KEY_RETURN]->setText("Return");

    update_langaue();
}

void CKeyboard::datebase_init()
{
    QFile db_cn_file;
    db_cn_file.setFileName(":/ime/res/pinyin.txt");
    if (!db_cn_file.open(QIODevice::ReadOnly))
        QMessageBox::warning(0,tr("ime"),tr("can't load pinyin.txt"));

    // Liigo 20190909： 词库文件格式以及加载后的内存结构(QMultiMap->QHash)都有重大调整。
    QTextStream floStream(&db_cn_file);
    floStream.setCodec("UTF-8");
    while (!floStream.atEnd()) {
		QString line = floStream.readLine();
        // qDebug() << "line: " << line;
		if (line.startsWith('#')) continue;
        int sp = line.indexOf(' ');
        if (sp > 0) {
            QString pinyin = line.left(sp);
            QString cnlist = line.right(line.length() - sp - 1);
            cn_map.insert(pinyin, cnlist);
            // qDebug() << pinyin << ": " << cnlist;
        }
     }
     db_cn_file.close();
}

// index >= 0
void CKeyboard::change_page(int index)
{
    // 显示备选字词, 每页最多八个，显示在数字键2~9上(1/0键用作翻页)
    int count = cn_list.size();
    for (int key = KEY_2; key <= KEY_9; key++) {
        int i = index * 8 + key - KEY_2;
        ppbKey[key]->setText(cn_list.value(i));
    }

    // 使能翻页键 -/1 ... +/0
    ppbKey[KEY_1]->setEnabled(index > 0);
    ppbKey[KEY_0]->setEnabled(count > (index*8+8));
}

void CKeyboard::clear_page(void)
{
    cn_list.clear();
    for (int key = KEY_2; key <= KEY_9; key++ ) {
        ppbKey[key]->setText("");
    }
    ppbKey[KEY_1]->setEnabled(false);
    ppbKey[KEY_0]->setEnabled(false);
}

void CKeyboard::pre_page()
{
    change_page(--page_index);
}

void CKeyboard::next_page()
{
    change_page(++page_index);
}

void CKeyboard::cn_matching(QString &pinyin)
{
    // qt5新增的 QHash/QMap::first(key) 返回value的引用， 用在此处效率更高， 但qt4不支持。
    cn_list = cn_map.value(pinyin).split(' ', QString::SkipEmptyParts);
    if (cn_list.size() > 0){
        page_index=0;
        change_page(0);
    } else {
        clear_page();
    }
}

void CKeyboard::delete_focus_text(void)
{
    int count = cn_input_text.size();
    if (count == 0)
        return;
    for (; count !=0; count--) {
        emit characterGenerated("\b");
    }
    cn_input_text.clear();
}

// 通过代码创建虚拟键盘内各个按键组件
void CKeyboard::key_init()
{
static int KEY_WIDTH1              = 60;
static int KEY_WIDTH2              = 70;
static int KEY_WIDTH3              = 130;
static int KEY_WIDTH4              = 290;
static int KEY_HEIGHT              = 46;
static int KEY_HEIGHT1             = 50;
static int KEY_INTERVAL_COL_WIDTH  = 3;
static int KEY_INTERVAL_ROW_HEIGHT = 5;
static int KEY_TOP_MARGIN          = 0;
static int KEY_LEFT_MARGIN         = 0;
static int SPECIAL_ROW_OFFSET      = 25;

    //this->setContentsMargins(4, 6, 0, 0);
    QPalette pal = palette();
    pal.setBrush(backgroundRole(), QBrush(QImage(":/ime/res/keyboard.png")));
    setPalette(pal);
    setFixedSize(600, 270);

    pline->setFocusPolicy(Qt::NoFocus);
    pline->setFixedSize(580, 30);
    pline->setReadOnly(true);

    int i;
    for (i = 0; i < KEY_NUM; i++) {
        ppbKey[i] = new QPushButton(this);
        ppbKey[i]->setFocusPolicy(Qt::NoFocus);
        if (i == KEY_BACK || i == KEY_SHIFT) {
            ppbKey[i]->setStyleSheet("QPushButton {font:14pt;border-image:url(:/ime/res/button2.png);}\
                                     QPushButton:hover {\
                                     border-image:url(:/ime/res/hoverbutton2.png);\
                                     border-style:outset;\
                                     background-attachment:fixed;\
                                     }");
            ppbKey[i]->setFixedSize(KEY_WIDTH2, KEY_HEIGHT);

        } else if (i == KEY_ENTER || i== KEY_RETURN) {
            ppbKey[i]->setStyleSheet("QPushButton {font:14pt;border-image:url(:/ime/res/button2.png);}\
                                     QPushButton:hover {\
                                     border-image:url(:/ime/res/hoverbutton2.png);\
                                     border-style:outset;\
                                     background-attachment:fixed;\
                                     }");
            ppbKey[i]->setFixedSize(KEY_WIDTH2, KEY_HEIGHT1);

        } else if (i == KEY_SPACE) {
            ppbKey[i]->setFixedSize(KEY_WIDTH4, KEY_HEIGHT1);
            ppbKey[i]->setStyleSheet("QPushButton {font:14pt;border-image:url(:/ime/res/button4.png);}\
                                     QPushButton:hover {\
                                     border-image:url(:/ime/res/hoverbutton4.png);\
                                     border-style:outset;\
                                     background-attachment:fixed;\
                                     }");
        } else {
            ppbKey[i]->setStyleSheet("QPushButton {font:14pt;border-image:url(:/ime/res/button1.png);}\
                                     QPushButton:hover {\
                                     border-image:url(:/ime/res/hoverbutton1.png);\
                                     border-style:outset;\
                                     background-attachment:fixed;\
                                     }");
            ppbKey[i]->setFixedSize(KEY_WIDTH1, KEY_HEIGHT);
        }
        ppbKey[i]->setFlat(true);
        button_group->addButton(ppbKey[i], i);
    }
    connect(button_group,SIGNAL(buttonClicked(int)),SLOT(slot_buttonClicked(int)));

    update_key_text();

    // number keys line
    QHBoxLayout *phLayout = new QHBoxLayout;
    phLayout->addSpacing(KEY_LEFT_MARGIN);
    for(int key = KEY_1; key <= KEY_9; key++) {
        phLayout->addWidget(ppbKey[key]);
        phLayout->addSpacing(KEY_INTERVAL_COL_WIDTH);
    }
    phLayout->addWidget(ppbKey[KEY_0]);
    phLayout->setSpacing(0);

    // letter keys lines: line1/line2/line3/line4
    QVBoxLayout *pvLayout = new QVBoxLayout;
    pvLayout->addSpacing(KEY_TOP_MARGIN);
    pvLayout->addWidget(pline);
    pvLayout->addSpacing(KEY_INTERVAL_ROW_HEIGHT);
    pvLayout->addLayout(phLayout);
    pvLayout->addSpacing(KEY_INTERVAL_ROW_HEIGHT);

    int line1keys[] = {KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P};
    phLayout = new QHBoxLayout;
    phLayout->addSpacing(KEY_LEFT_MARGIN);
    for(i = 0; i < 9; i++) {
        phLayout->addWidget(ppbKey[line1keys[i]]);
        phLayout->addSpacing(KEY_INTERVAL_COL_WIDTH);
    }
    phLayout->addWidget(ppbKey[KEY_P]);
    phLayout->setSpacing(0);
    pvLayout->addLayout(phLayout);
    pvLayout->addSpacing(KEY_INTERVAL_ROW_HEIGHT);

    const int line2keys[] = {KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L};
    phLayout = new QHBoxLayout;
    phLayout->addSpacing(SPECIAL_ROW_OFFSET);
    for(i = 0; i < 8; i++) {
        phLayout->addWidget(ppbKey[line2keys[i]]);
        phLayout->addSpacing(KEY_INTERVAL_COL_WIDTH);
    }
    phLayout->addWidget(ppbKey[KEY_L]);
    phLayout->addSpacing(SPECIAL_ROW_OFFSET);
    phLayout->setSpacing(0);
    pvLayout->addLayout(phLayout);
    pvLayout->addSpacing(KEY_INTERVAL_ROW_HEIGHT);

    const int line3keys[] = {KEY_SHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_BACK};
    phLayout = new QHBoxLayout;
    phLayout->addSpacing(KEY_LEFT_MARGIN);
    for (i = 0; i < 8; i++) {
        phLayout->addWidget(ppbKey[line3keys[i]]);
        phLayout->addSpacing(KEY_INTERVAL_COL_WIDTH);
    }
    phLayout->addWidget(ppbKey[KEY_BACK]);
    phLayout->setSpacing(0);
    pvLayout->addLayout(phLayout);
    pvLayout->addSpacing(KEY_INTERVAL_ROW_HEIGHT);

    const int line4keys[] = {KEY_SYMBOLS, KEY_LANGUAGE, KEY_SPACE, KEY_ENTER, KEY_RETURN};
    phLayout = new QHBoxLayout;
    phLayout->addSpacing(KEY_LEFT_MARGIN);
    for (i = 0; i < 4; i++) {
        phLayout->addWidget(ppbKey[line4keys[i]]);
        phLayout->addSpacing(KEY_INTERVAL_COL_WIDTH);
    }
    phLayout->addWidget(ppbKey[KEY_RETURN]);
    phLayout->setSpacing(0);
    pvLayout->addLayout(phLayout);

    pvLayout->setSpacing(0);
    setLayout(pvLayout);
}

void CKeyboard::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton) {
        mMoving = true;
        mLastMousePosition = event->globalPos();
    }
}

void CKeyboard::mouseMoveEvent(QMouseEvent* event)
{
    if(mMoving && event->buttons().testFlag(Qt::LeftButton)) {
        this->move(this->pos() + (event->globalPos() - mLastMousePosition));
        mLastMousePosition = event->globalPos();
    }
}

void CKeyboard::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton) mMoving = false;
}
