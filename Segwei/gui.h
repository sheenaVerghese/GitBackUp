#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QToolBar>
#include <QToolButton>
#include <QToolBox>
#include <QLabel>
#include <QToolTip>
#include "LineNumber.h"

class QAction;
class scannar;
namespace Ui {
class GUI;
}

class GUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUI(QWidget *parent = 0);
    ~GUI();

private:

	
    LineNumber *codeEditor;
	QString fileName;
	QString returnValue;
    QPlainTextEdit *textEdit;
    QPlainTextEdit *console;
	QMenuBar *menuBar;
	QHBoxLayout *hLayout;
	QVBoxLayout *vLayout;

	QGroupBox *verticalBox;
	QGroupBox *horizontalBox;
	QWidget *widget;//parent widget
	QMenu *fileMenu;
    
    QToolButton *runButton;
    QToolButton *saveButton;
    QToolButton *cutButton;
    QToolButton *copyButton;
    QToolButton *pasteButton;
    QToolButton *openButton;

    QPushButton *ifButton;
    QPushButton *elseButton;
    QPushButton *forButton;
    QPushButton *whileButton;
    QPushButton *printButton;
    QPushButton *beginButton2;
    QPushButton *readButton;
    QPushButton *charButton;
    QPushButton *stringButton;
    QPushButton *intButton;
    QPushButton *decButton;


    QMenu *editMenu;
    QMenu *debugMenu;
    QMenu *helpMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
  
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *runAct;
    Ui::GUI *ui;

//methods
    void createMenu();
    void createAction();
    void createToolButtons();


private slots:
    void open();

    void about();

    void newFile();

    void saveAs();

    void save();

  

    void undo();

    void redo();

    void cut();

    void copy();

    void paste();

    void run();
    //push button slots
    void ifCode();
    void forCode();
    void ifElseCode();
    void whileCode();
    void charCode();
    void strCode();
    void decCode();
    void intCode();
    void readCode();
    void printCode();
    void beginCode();



protected :
	scannar *scanFile;

};


#endif // LineNumber_H
