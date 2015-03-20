/************************************************
 * Author: Sheena V
 * Date: 22/01/2015
 * Description : Scanner Class for the Language
 * Module: Scannar.h
 * Functionalities:
 * 1)Takes the input string
 * 2)Appends and empty space to the back of the string
 * 3)Ensures that the vector is empty before processing more information
 * 4)Splits the strings. Ignores all spaces.
 * 5)Delimiters are stored seperately.
 * 6)Appends tokens
 *
 *
 * **********************************************/
#ifndef SCANNAR_H
#define SCANNAR_H
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "gui.h"
#include <algorithm>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <cctype>
class error;
class parser;
class codegenerator;
using namespace std;

class scannar
{
public:
    scannar();
    scannar(string);
    ~scannar();
    string getReturn();
    int splitStr();
    void deallocateVector();
    int appendToken();
    //SQ is single quotes, DQ is double quotes, LCB is left curly brace
    //RCB is right curly brace, SB is square bracket,BR is bracket -->()
    //DEC is decimal, STR is string,T_DOP IS DOUBLE operator(eg && and ||)
    //COP is comparison operator
    enum token{T_INT=1,T_CHAR=2,T_STR=3,T_DEC=4,
               T_INTLit=5,T_CHARLit=6,T_STRLit=7,T_DECLit=8,
               T_OP=9,T_ID=10,T_RBR=11,T_LBR=12,T_LCB=13,T_RCB=14,
               T_LSB=15,T_RSB=16,T_DQ=17,T_SQ=18,BEGIN=19,END=20,IF=21,
               ELSE=22, END_IF=23,END_FOR=24,FOR=25,WHILE=26,END_WHILE=27,
               READ=28,PRINT=29,EQUALS=30,SEMICOLON=31,T_DOP=32,T_FSLASH=33,
               T_BSLASH=34,T_COP=35
              };
   struct Tokenizer{token tokens; string variable; int lineNum;};
   //struct symbolTable{token tokens;string variable;int scope;int block; int lineNum;symbolTable *pred;};
   void print(vector <Tokenizer>);
   void initialiseMap();

   string returnToGUI();
   string consoleMessage(int);
private:
   void clearStream();
   void convertIntToStr();
   Tokenizer *toks;
   string srcCode;
   vector <string> codeFragments;
   vector <Tokenizer>tokenize;
   string lineNo;
   string data;
   map <string,token>mapTokens;
   int lineNumber;
   string msg;
   stringstream convert;
   string character;
   bool found;
   string tempStorage;
   int isDecimal(string);
   int isInt(string);
   int isTypeId(string);
   int numOfOp;
protected:
  error *err;
  parser *parse;

};



#endif // SCANNAR_H
