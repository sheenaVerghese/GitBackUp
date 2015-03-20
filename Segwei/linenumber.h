#ifndef LINENUMBER_H
#define LINENUMBER_H
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QObject>
//#include "editor.h"
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class LineNumberArea;
class MainWindow;
//class editor;

namespace Ui {
class LineNumberArea;
}

class LineNumber : public QPlainTextEdit
{
    Q_OBJECT

public:
     LineNumber(QWidget *parent=0);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    ~LineNumber();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
     void updateLineNumberAreaWidth(int newBlockCount);
     void highlightCurrentLine();
     void updateLineNumberArea(const QRect &, int);

private slots:
    void on_text_textChanged();

private:
  //  Ui::LineNumber *ui;
    QWidget *lineNumberArea;
   //editor *edit;
};


class LineNumberArea : public QWidget
 {
 public:
     LineNumberArea(LineNumber *editor) : QWidget(editor) {
         codeEditor = editor;

     }

     QSize sizeHint() const {
         return QSize(codeEditor->lineNumberAreaWidth(), 0);
     }

 protected:
     void paintEvent(QPaintEvent *event) {
         codeEditor->lineNumberAreaPaintEvent(event);
     }

 private:
   LineNumber *codeEditor;
   Ui::LineNumberArea *ui;
 };

#endif // LineNumber_H
