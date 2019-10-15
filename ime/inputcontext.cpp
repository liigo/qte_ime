
#include "inputcontext.h"
#include "keyboard.h"
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QLineEdit>

CInputContext::CInputContext(QObject *parent) : QInputContext(parent)
{
    pKeyboard = new CKeyboard();
    connect(pKeyboard, SIGNAL(characterGenerated(QString)), SLOT(sendCharacter(QString)));
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

CInputContext::~CInputContext()
{
    delete pKeyboard;
}

QString CInputContext::identifierName()
{
    return QString();
}

QString CInputContext::language()
{
    return QString();
}

void CInputContext::reset()
{
}

bool CInputContext::isComposing() const
{
    return true;
}

bool CInputContext::filterEvent(const QEvent* event)
{
    if (event->type() == QEvent::RequestSoftwareInputPanel) {
        //if (!app::disable_soft_keyboard)
        {
            pKeyboard->pline->clear();
            pKeyboard->setGeometry(100, 210, pKeyboard->width(), pKeyboard->height());
            pKeyboard->show();
        }
        return true;
    } else if (event->type() == QEvent::CloseSoftwareInputPanel) {
        pKeyboard->hide();
        return true;
    }

    return QInputContext::filterEvent(event);
}

static void widget_sendkey(QWidget* pWidget, Qt::Key key, QString text = "", QString release_text = "")
{
    QKeyEvent keyPress(QEvent::KeyPress, key, Qt::NoModifier, text);
    QCoreApplication::sendEvent(pWidget, &keyPress);

    QKeyEvent keyRelease(QEvent::KeyRelease, key, Qt::NoModifier, release_text);
    QCoreApplication::sendEvent(pWidget, &keyRelease);
}

void CInputContext::sendCharacter(QString key_value)
{
    QWidget *pFocusWidget = QApplication::focusWidget();
    if(pFocusWidget == NULL) return;

    QString newstr = pKeyboard->pline->text();

    if (key_value == "\b") {
        widget_sendkey(pFocusWidget, Qt::Key_Backspace);
        int newsize = newstr.size() - 1;
        if (newsize < 0) newsize = 0;
        newstr.resize(newsize);
    } else if (key_value == "\r") {
        widget_sendkey(pFocusWidget, Qt::Key_Enter);
    } else if (key_value == " ") {
        widget_sendkey(pFocusWidget, Qt::Key_Space, key_value);
        newstr += key_value;
    } else {
        widget_sendkey(pFocusWidget, (Qt::Key)0, key_value);
        newstr += key_value;
    }

    pKeyboard->pline->setText(newstr);
}

void CInputContext::updatePosition()
{
    QWidget *pFocusWidget = QApplication::focusWidget();
    if (pFocusWidget != NULL)
    {
//        QWidget *pTopWidget = pFocusWidget->parentWidget();
//        QWidget *pTmpWidget = pTopWidget;
//        while(pTmpWidget != NULL)
//        {
//            pTopWidget = pTmpWidget;
//            pTmpWidget = pTmpWidget->parentWidget();
//        }
//        if (pTopWidget == NULL)
//            pTopWidget = pFocusWidget;

        //panelStartPos = QPoint(0, WND_HEIGHT - 260);
//        panelStartPos = QPoint(pTopWidget->pos().x(),
//                               pTopWidget->pos().y() + pTopWidget->height());


//        panelStartPos = QPoint(pFocusWidget->pos().x(),
//                               pFocusWidget->pos().y() + pFocusWidget->height());
//        panelStartPos = pFocusWidget->mapToGlobal(panelStartPos);
        //panelStartPos = QPoint(0,200);
        increment = 0;
    }
}

void CInputContext::timeout()
{
    int w = pKeyboard->width();
    int h = pKeyboard->height();
    int x = panelStartPos.x();
    increment += 10;
    int y = panelStartPos.y();
    if (increment >= h) {
        timer.stop();
        if (!bShow) {
            pKeyboard->hide();
        }
    } else {
        if (bShow) {
            y = panelStartPos.y() + h - increment;
        } else {
            y = panelStartPos.y() + increment;
        }
    }
    pKeyboard->setGeometry(x, y, w, h);
}

void CInputContext::startMyTimer(bool bShow)
{
    this->bShow = bShow;
    if (bShow) pKeyboard->show();
    if (!timer.isActive()) {
        increment = 0;
        timer.start(10);
    }
}

void CInputContext::showKeyboard()
{
    QWidget* pFocusWidget = qApp->focusWidget();
    if (pFocusWidget && pFocusWidget->testAttribute(Qt::WA_InputMethodEnabled)) {
        QEvent e(QEvent::RequestSoftwareInputPanel);
        QApplication::sendEvent(pFocusWidget, &e);
    }
}

void CInputContext::hideKeyboard()
{
    QWidget* pFocusWidget = qApp->focusWidget();
    if (pFocusWidget) {
        QEvent e(QEvent::CloseSoftwareInputPanel);
        QApplication::sendEvent(pFocusWidget, &e);
    }
}

void CInputContext::mouseHandler(int x, QMouseEvent* event)
{
    (void)x, (void)event;
}

int CProxyStyle::styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData) const
{
    if (hint == QStyle::SH_RequestSoftwareInputPanel)
        return QStyle::RSIP_OnMouseClick;
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}
