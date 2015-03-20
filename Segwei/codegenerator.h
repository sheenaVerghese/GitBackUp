#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QBoxLayout>
#include <QGroupBox>
#include <QVBoxLayout>
#include <iostream>
#include <QString>
#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <QInputDialog>
//#include "scannar.h"
#include "parser.h"
using namespace std;
//class scannar;
//class parser;
namespace Ui {
class codegenerator;
}

class codegenerator : public QDialog
{
    Q_OBJECT

public:
    explicit codegenerator(QWidget *parent = 0);
    void startCodeGen(vector<scannar::Tokenizer>tok,deque<parser::symbolTable>env,vector<parser::positions>pos);
    void endCodeGen();
    void blockInitialisation();
    void exitBlock();
    void blockRollBack();
    void blockRollFoward();
    int startCode();
    int findSemiColonStatement(int);
    int getLoopVars(int);
    bool lookUpVars(int, int key);
    int computeValues(int);
    int computeLoop(int);

    void updateEnvTable(int);
    //ints
    string divInt(string,string);
    string divIntNeg(string,string);
    string mulInt(string,string);
    string mulIntNeg(string,string);
    string minusInt(string,string);
    string addInt(string,string);
    //decimals
    string divDec(string,string);
    string divDecNeg(string,string);
    string mulDec(string,string);
    string mulDecNeg(string,string);
    string minusDec(string,string);
    string addDec(string,string);
    bool isDec(string);
    bool isInteger(string);
    bool greater();
    bool lessThan();
    bool equalsEquals();
    bool notEquals();
    bool greaterEquals();
    bool lessEquals();
    void print(string);
    bool orOperator(int,int);
    bool andOperator(int,int);
    void infiniteMessage(int);
    int forLoopVars(int, int);
    ~codegenerator();

private:
//UI
    Ui::codegenerator *ui;
    QVBoxLayout *vLayout;
    QGroupBox *verticalBox;
   // QGroupBox *horizontalBox;
    QWidget *widget;//parent widget
    QPlainTextEdit *displayResult;
//variables
    vector<scannar::Tokenizer>code;
    deque<parser::symbolTable>environment;
    deque<bool>truthStatements;
    vector<parser::positions>envPtr;
    vector <scannar::Tokenizer> trackSemiColon;
    //loopVars==loopVariables
    vector <scannar::Tokenizer> loopVars;
    int blockData;
    int newBlock;
    deque <int> parent;
    scannar::token tokenType;
    scannar::token tempToken;
    int error;
    int errorLine;
    int countIf;
    int countElse;
    int countFor;
    int countWhile;
    int bracket;
    bool begin;
    bool end;
    scannar::token idType;
    scannar::token assignId;
    scannar::Tokenizer *extraToken;
    int idPos;
    //multiply or divide
    bool mulDiv;
    //addition or subtraction
    bool addMinus;
    int computeVal;
    //loop==0 means not loop 1 means if 2 means while 3 means for
    int loop;
    bool trueFalse;
    int loopChecker;
    int loopCounter;
    int tempForPos;
    deque<int>infiniteLoop;


};

#endif // CODEGENERATOR_H
