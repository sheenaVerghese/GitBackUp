/************************************************
 * Author: Sheena V
 * Date: 26/01/2015
 * Module: Error.h
 * Description : Error Class for the Language
 * Functionalities:
 * 1)Returns a string which states the error msg.
 *
 * **********************************************/
#ifndef ERROR_H
#define ERROR_H
#include <iostream>
#include<string>
#include "scannar.h"
#include <sstream>
using namespace std;
class error
{
public:
    error();
    ~error();
    //token not mentioned
    //1
    string scannarError();
    //2
    string unknownType();
    //after initialisation, the use of the variable is wrong
    //3
    string invalidType();
    //4
    string uninitialisedVar();
    //initialisation invalid due to multiple initialisation at same scope level
    //5
    string invalidInitialisation();
    //6
    string closeQuotesNotFound();
    //7
    string invalidIdentifierName();
    //8
    string invalidCharacter();
    //9
    string invalidEscapeSeq();
    //10
    string missingBegin();
    //11
    string multipleBegin();
    //12
    string invalidAfterEnd();
    //13
    string endNotFound();
    //14
    string invalidElse();
    //15
    string extraEndFor();
    //16
    string extraEndWhile();
    //17
    string extraEndIf();
    //18
    string EOFError();
    //19
    string incorrectEnd();
    //20
    string extraBracket();
    //21
    string openBracketNotFound();
    //22
    string invalidOpenBracket();
    //23
    string unidentifiedBrackets();
    //24
    string identifierNotFound();
    //25
    string invalidAssignment();
    //26
    string multipleDeclaration();
    //27
    string invalidOperator();
    //28
    string invalidOperatorForString();
    //29
    string missingSemiColon();
    //30
    string invalidCommand();
    //31
    string uselessCommand();
    //32
    string invalidRead();
    //33
    string multipleEnd();
    //34
    string missingEquals();
    //35
    string uninitAndUndeclaredVars();
    //36
    string invalidCharOp();
    //37
    string invalidParserChar();
    //38
    string closeBracketErr();
    //39
    string endWhile();
    //40
    string endFor();
    //41
    string endIf();
    //42
    string invalidLogicalOp();
    //43
    string extraCloseBrackets();
    //44
    string endLitOrVar();
    //45
    string invalidLoopVar();
    //46
    string invalidSemicolon();
    //47
    string incrementNotFound();
    //48
    //COP stands for comparison operator  such as > < >= <=
    string invalidCOPUsage();
};

#endif // ERROR_H
