#ifndef PARSER_H
#define PARSER_H
#include "scannar.h"
//#include "codegenerator.h"
#include <iostream>
#include <vector>
#include <deque>
#include <string>
using namespace std;
class codegenerator;
class parser
{
public:
    parser();
    ~parser();
    int startParsing(vector<scannar::Tokenizer>token);
    int checkStructure();
    struct symbolTable{scannar::token tokens;string variable;int scope;int block; int lineNum;int pred;bool init;string value;};
    int getLineNumber();
    bool checkType(scannar::token,scannar::token);
    void modifyDqPositions(int);
    int addToEnvironmentTable(int, int);
    void printDq();
    void printEnv();
    int findSemiColon(int);
    int parseStatement();
    //need to keep track of how many open brackets there are
    //check if there isn't excessive closed brackets
    bool checkCloseBrackets(int, int);
    bool isLoop(int);
    int checkVariables(int);
    bool isInit(int);
    bool lookUp(int);
    int checkVarType();
    int checkAssignment(int);
    void printTrackSemiColon();
    void addToDqandEnvironment();
    int findPosition(int);
    int returnEnd(string);
    int getLoopVariables(int);
    int checkLoop(int);
    int forLoop(int);
    void blockInit();
    void blockExit();
    struct positions{int position;int block;};



private :
    //using the parent, the last int value is the current block value.
    //anything before those are its parents
    deque <int> parent;
    vector <scannar::Tokenizer> parseTokenizer;
    //keeps everything leading up to a semicolon
    vector <scannar::Tokenizer> trackSemiColon;
    vector <scannar::Tokenizer> tempVector;
    scannar::Tokenizer *toks;
    string data;
    int scopeData;
    int blockData;
    int lineData;
    scannar::token tokenData;
    symbolTable environmentVars;
    bool begin;
    int vectorSize;
    int errorLine;
    int newBlock;
    bool end;
    bool id;
    int countIf;
    int countElse;
    int countFor;
    int countWhile;
    int bracket;
    int error;
    int idPos;
    bool charOrStr;

    //temporary variable to store position value
    int temp;
    //error when missing end
    int missingEnd;
   // symbolTable *parent;

    positions ptr;
    vector <string> prevStatement;
    deque <symbolTable> environmentTable;
    vector <positions>dequePtr;
    scannar::token idType;
    scannar::token assignId;
    //codegenerator codegen;
    bool checked;
    int numericalBrackets;
    int countLoopVar;
    bool loop;
    //----- METHODS---

    int checkOpenBracket(int, int );

protected:
    codegenerator *codegen;

};

#endif // PARSER_H
