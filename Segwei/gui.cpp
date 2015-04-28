/************************************************
 * Author: Sheena V
 * Date: 20/01/2015
 * Description : GUI class for the language.
 * Constructs the GUI
 *
 * Some References:
 * 1)http://stackoverflow.com/questions/9290767/adding-child-in-qmainwindow
 *
 * **********************************************/
#include "gui.h"
#include "ui_gui.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <iostream>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QToolBar>
#include <QToolButton>
#include <QIcon>
#include <QToolBox>
#include "scannar.h"
using namespace std;


GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    createAction();
    createMenu();
	
    //not dependant on a particular resolution
    QRect rec = QApplication::desktop()->screenGeometry();
    int height = rec.height();
    int width = rec.width();
    codeEditor=new LineNumber();

   QLabel *consoleLable=new QLabel;
   consoleLable->setText("Console");
   QLabel *editorLable=new QLabel;
   editorLable->setText("Editor");

   QVBoxLayout *mainLayout =new QVBoxLayout;
   hLayout = new QHBoxLayout;
   vLayout = new QVBoxLayout;

   verticalBox=new QGroupBox;
   horizontalBox=new QGroupBox;
   widget= new QWidget;//parent widget
   console= new QPlainTextEdit;

   //getting a percentage of the screen
   int quarterWidth= width/4;
   int quarterArea=height/4;

   //toolBar elements
   QToolBar *toolBar=new QToolBar;
   runButton=new QToolButton;
   saveButton=new QToolButton;
   cutButton=new QToolButton;
   copyButton=new QToolButton;
   pasteButton=new QToolButton;
   openButton=new QToolButton;

   //toolBox elements
   QToolBox *toolBox=new QToolBox;
   QVBoxLayout *vToolBox=new QVBoxLayout;
   QGroupBox *vGroupBox=new QGroupBox;
   QVBoxLayout *vToolBox2=new QVBoxLayout;
   QGroupBox *vGroupBox2=new QGroupBox;
   QVBoxLayout *vToolBox3=new QVBoxLayout;
   QGroupBox *vGroupBox3=new QGroupBox;
   ifButton= new QPushButton("IF");
   elseButton= new QPushButton("IF..ELSE");
   forButton= new QPushButton("FOR");
   whileButton= new QPushButton("WHILE");
   printButton= new QPushButton("PRINT");
   beginButton2= new QPushButton("BEGIN");
   readButton= new QPushButton("READ");
   charButton= new QPushButton("CHARACTER");
   stringButton= new QPushButton("SENTENCE");
   intButton= new QPushButton("INT");
   decButton= new QPushButton("DECIMAL");
    //adding items to toolbox
    vToolBox->addWidget(ifButton);
    vToolBox->addWidget(elseButton);
    vToolBox->addWidget(forButton);
    vToolBox->addWidget(whileButton);
    vGroupBox->setLayout(vToolBox);
  //second group
    vToolBox2->addWidget(charButton);
    vToolBox2->addWidget(stringButton);
    vToolBox2->addWidget(intButton);
    vToolBox2->addWidget(decButton);
    vGroupBox2->setLayout(vToolBox2);
//third group
    vToolBox3->addWidget(beginButton2);
    vToolBox3->addWidget(readButton);
    vToolBox3->addWidget(printButton);
    vGroupBox3->setLayout(vToolBox3);
    //setting items to the toolbox
    toolBox->addItem(vGroupBox,"Loops");
    toolBox->addItem(vGroupBox2,"Variable Types");
     toolBox->addItem(vGroupBox3,"Console I/O");
    toolBox->setMaximumWidth(quarterWidth/2);


   //setting icon to toolButtons
    copyButton->setIcon(QIcon((":/Images/Icons/Icons/Copy.png")));
    cutButton->setIcon(QIcon((":/Images/Icons/Icons/Cut.png")));
    runButton->setIcon(QIcon((":/Images/Icons/Icons/GreenArrow.png")));
    openButton->setIcon(QIcon((":/Images/Icons/Icons/Open.png")));
    pasteButton->setIcon(QIcon((":/Images/Icons/Icons/Paste.png")));
    saveButton->setIcon(QIcon((":/Images/Icons/Icons/Save.png")));

    //setting tooltip
    copyButton->setToolTip("Copy");
    cutButton->setToolTip("Cut");
    runButton->setToolTip("Run Code");
    openButton->setToolTip("Open File");
    pasteButton->setToolTip("Paste to Editor");
    saveButton->setToolTip("Save file");
    //setting the actions
    createToolButtons();

    //setting the toolButtons to the toolBar

    toolBar->addWidget(openButton);
    toolBar->addWidget(saveButton);
    toolBar->addSeparator();
    toolBar->addWidget(cutButton);
    toolBar->addWidget(copyButton);
    toolBar->addWidget(pasteButton);
    toolBar->addSeparator();
    toolBar->addWidget(runButton);



   verticalBox->setMaximumWidth(quarterWidth*3);
   console->setMaximumHeight(quarterArea);
   verticalBox->setLayout(vLayout);
   horizontalBox->setLayout(hLayout);

   //setting text to console
    codeEditor->setPlainText("begin\n\nend");

    //vertical Layout
   vLayout->addWidget(editorLable);
   vLayout->addWidget(codeEditor);
   vLayout->addWidget(consoleLable);
   vLayout->addWidget(console);

   //horizontal Layout
   hLayout->addWidget(toolBox);
   hLayout->addWidget(verticalBox);

   //mainLayout
   setMenuBar(menuBar);
   addToolBar(toolBar);
   setCentralWidget(widget);
   mainLayout->addWidget(horizontalBox);
  // mainLayout->setMenuBar(menuBar);
   centralWidget()->setLayout(mainLayout);


}
void GUI::createMenu(){
	menuBar = new QMenuBar;
     fileMenu = new QMenu(tr("&File"), this);
     editMenu = new QMenu (tr("&Edit"),this);
     helpMenu = new QMenu (tr("&Help"),this);
     debugMenu = new QMenu (tr("&Debug"),this);

     //file menu items
         fileMenu->addAction(newAct);
         fileMenu->addAction(openAct);
         fileMenu->addSeparator();
         fileMenu->addAction(saveAct);
         fileMenu->addAction(saveAsAct);
         fileMenu->addSeparator();
         

         //edit menu items
         editMenu->addAction(undoAct);
         editMenu->addAction(redoAct);
         editMenu->addSeparator();
         editMenu->addAction(cutAct);
         editMenu->addAction(copyAct);
         editMenu->addAction(pasteAct);

         debugMenu->addAction(runAct);

         helpMenu->addAction(aboutAct);
         //adding the menu items to the menubar
         menuBar->addMenu(fileMenu);
         menuBar->addMenu(editMenu);
         menuBar->addMenu(debugMenu);
         menuBar->addMenu(helpMenu);

}
void GUI::createAction(){

    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("&SaveAs"), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document to disk as.."));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));



    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last operation"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last operation"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    cutAct = new QAction(tr("&Cut"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                                 "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                                  "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    runAct = new QAction(tr("&Run"), this);
    runAct->setStatusTip(tr("Debug and Compile the code"));
    connect(runAct, SIGNAL(triggered()), this, SLOT(run()));


}

void GUI::open()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "C://",
                                                     tr("Text Files (*.txt)"));

    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(0,"File Open Error",file.errorString());

    }//end if
    else{
        QTextStream input(&file);
        codeEditor->setPlainText(input.readAll());
    }
    file.close();

}

void GUI::about()
{
    QMessageBox::about(this,"Help","Enter the code into the editor.\nUse the loop templates at the right to start off.\nPress the green arrow button to run."
                      );

}

void GUI::newFile()
{
   codeEditor->setPlainText("begin\n\nend");
   console->setPlainText("");

}

void GUI::saveAs()
{
     fileName = QFileDialog::getSaveFileName(this, tr("Save File"),"C://",tr("Text (*.txt)"));

    QString str = codeEditor->toPlainText();

    QFile file(fileName);
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&file);
    out<<str<<endl;
    file.close();
}

void GUI::save()
{
    if(fileName!=""||fileName!=NULL){
        QString str = codeEditor->toPlainText();

        QFile file(fileName);
        file.open(QIODevice::WriteOnly|QIODevice::Text);
        QTextStream out(&file);
        out<<str<<endl;
        file.close();
		QMessageBox::information(0, "Saved", "Code Saved");
    }
    else{
        saveAs();
    }


}


void GUI::undo()
{
    codeEditor->undo();
}

void GUI::redo()
{
    
    codeEditor->redo();
}

void GUI::cut()
{
    codeEditor->cut();
}

void GUI::copy()
{
    codeEditor->copy();
}

void GUI::paste()
{
    codeEditor->paste();
}

//run should send the whole string to the scannar or lexer class
void GUI::run()
{
     console->setPlainText("running..");
	 QString constStr = codeEditor->toPlainText();
     string scan = constStr.toStdString();
     console->setPlainText("Running.. ");
     scanFile = new scannar(scan);
     string getMsg=scanFile->returnToGUI();
     console->setPlainText(QString::fromStdString(getMsg));

}

//creates the actions for the toolbuttons
void GUI::createToolButtons()
{
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));
    connect(cutButton, SIGNAL(clicked()), this, SLOT(cut()));
    connect(copyButton, SIGNAL(clicked()), this, SLOT(copy()));
    connect(pasteButton, SIGNAL(clicked()), this, SLOT(paste()));

    //toolBox
    connect(ifButton, SIGNAL(clicked()), this, SLOT(ifCode()));
    connect(elseButton, SIGNAL(clicked()), this, SLOT(ifElseCode()));
    connect(forButton, SIGNAL(clicked()), this, SLOT(forCode()));
    connect(whileButton, SIGNAL(clicked()), this, SLOT(whileCode()));
    connect(stringButton, SIGNAL(clicked()), this, SLOT(strCode()));
    connect(decButton, SIGNAL(clicked()), this, SLOT(decCode()));
    connect(charButton, SIGNAL(clicked()), this, SLOT(charCode()));
    connect(readButton, SIGNAL(clicked()), this, SLOT(readCode()));
    connect(printButton, SIGNAL(clicked()), this, SLOT(printCode()));
    connect(intButton, SIGNAL(clicked()), this, SLOT(intCode()));
    connect(beginButton2, SIGNAL(clicked()), this, SLOT(beginCode()));


}

//pushButtonCodes
void GUI::ifCode()
{
    codeEditor->insertPlainText("\nif()\nend_if\n");
}
void GUI::forCode()
{
    codeEditor->insertPlainText("\nfor()\nend_for\n");
}
void GUI::ifElseCode()
{
    codeEditor->insertPlainText("\nif()\nelse\nend_if\n");
}
void GUI::whileCode()
{
    codeEditor->insertPlainText("\nwhile()\nend_while\n");
}
void GUI::charCode()
{
    codeEditor->insertPlainText("\nchar ;\n");
}
void GUI::strCode()
{
    codeEditor->insertPlainText("\nsentence ;\n");
}
void GUI::decCode()
{
    codeEditor->insertPlainText("\ndecimal ;");
}
void GUI::intCode()
{
    codeEditor->insertPlainText("\nint ;");
}
void GUI::readCode()
{
    codeEditor->insertPlainText("\nreadFromScreen();");
}
void GUI::printCode()
{
    codeEditor->insertPlainText("\nprintToScreen();");
}
void GUI::beginCode()
{
    codeEditor->insertPlainText("\nbegin\nend\n");

}


GUI::~GUI()
{
    delete ui;
}
