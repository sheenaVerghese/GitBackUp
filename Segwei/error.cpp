/************************************************
 * Author: Sheena V
 * Date: 26/01/2015
 * Module: error.cpp
 * Description : error Class for the Language
 * Functionalities:
 * 1)Returns a string which states the error msg.
 *
 * **********************************************/

#include "error.h"

error::error()
{

}
error::~error()
{

}
string error::scannarError(){
    return "Tokenizer has given an error\nPlease check Line ";

}

string error::unknownType(){
    return "An unknown type has been used at line...";
}

//after initialisation, the use of the variable is wrong
string error::invalidType(){
    return "The type is invalid for variable:Line ";
}

string error::uninitialisedVar(){
    return "The variable is not initialised at Line ";
}

//initialisation invalid due to multiple initialisation at same scope level
string error::invalidInitialisation(){
    return "Multiple initialisations not allowed: Line ";
}

string error::closeQuotesNotFound(){
    return "Closing Quotes(\" or ') not found: Line ";
}
string error::invalidIdentifierName(){
    return "Invalid Identifier Name:Line";
}
string error::invalidCharacter(){
    return "Invalid character encountered:Line ";
}
string error::invalidEscapeSeq(){
    return "Invalid Escape Sequence:Line ";
}
string error::missingBegin()
{

    return ("Syntax error:Begin not found or cannot be reached: Line ");
}
string error::multipleBegin()
{

    return ("Syntax error:Multiple Begin Statements not Allowed: Line ");
}
string error::invalidAfterEnd()
{
    return "End Found! Invalid characters after Line ";
}
string error::endNotFound()
{
    return "End not Found! End is missing:Line ";
}
string error::invalidElse()
{
    return "Invalid else. Else must always be preceded by an if: Line ";
}
string error::extraEndWhile()
{
    return "Extra END_WHILE found: Line ";
}
string error::extraEndIf()
{
    return "Extra END_IF found: Line ";
}
string error::extraEndFor()
{
    return "Extra END_FOR found: Line ";
}
//missing end_while or end_if or end_for
string error::EOFError()
{
    return "EOF reached. No end statements found:Line ";
}
string error::incorrectEnd()
{
    return "Invalid End Reached. Need to end previous loop: Line ";
}
string error::extraBracket()
{
    return "Extra ')' found: Line ";
}
string error::openBracketNotFound()
{
    return "Syntax Error: Brackets required():Line ";
}
string error::invalidOpenBracket()
{
    return "Syntax Error: Brackets should not be there (): Line";
}
string error::unidentifiedBrackets()
{
    return "Error: Unidentified bracket type.Perhaps you meant these ():Line ";
}
string error::multipleDeclaration()
{
    return "Error: The variable has been declared before:Line ";
}
string error::invalidAssignment()
{
    return "Error:Expected Variable Type and Actual Variable type is different:Line ";
}
string error::identifierNotFound()
{
    return "Error: The variable has not been declared or invalid name:Line ";
}
string error::invalidOperator()
{
    return "Error: The operator is invalid.Please change/remove it:Line ";
}

string error::invalidOperatorForString()
{
    return "Error:Operator not supported by string type:Line ";
}
string error::invalidCommand()
{
    return "Error:Invalid command obtained:Line ";
}
string error::missingSemiColon()
{
    return "Syntax error: Missing semicolon ';' :Line ";
}

string error::uselessCommand()
{
    return "Logical Error: The command given does nothing:Line ";
}
string error::invalidRead()
{
    return "Syntax Error: Wrong usage of readToScreen():Line ";
}

string error::multipleEnd()
{
    return "Syntax Error: Multiple End found:Line ";
}

string error::missingEquals()
{
    return "Syntax Error:Equals operator expected but not found (invalid command found):Line ";
}

string error::uninitAndUndeclaredVars()
{
    return "Syntax Error: Variable has to be of the correct type and initialised:Line ";
}
//cannot have any + or - operator done on characters
string error::invalidCharOp()
{
    return "Syntax error: Character must not have any operation done on it:Line ";
}

string error::invalidParserChar()
{
    return "Syntax Error: Invalid character found :Line ";
}
string error::closeBracketErr()
{
    return "Syntax Error: Close bracket ')' not found:Line ";
}
string error::endWhile()
{
    return "Syntax Error: Missing End_While :Line ";
}

string error::endFor()
{
    return "Syntax error: Missing End_For:Line ";
}

string error::endIf()
{
    return "Syntax Error: Missing End_If:Line ";
}
string error::extraCloseBrackets()
{
    return "Syntax Error: Extra Close bracket ')' found:Line ";
}
string error::invalidLogicalOp()
{
    return "Syntax Error:Invalid Logical Operator || or && found. Please change it :Line ";
}
string error::endLitOrVar()
{
    return "Syntax Error: Need to end with a literal type or a variable :Line ";
}

string error::invalidLoopVar()
{
    return "Syntax error: Loop variables are invalid:Line ";
}

string error::invalidSemicolon()
{
    return "Syntax Error: Semicolon ';' not allowed:Line ";
}
string error::incrementNotFound()
{
    return "Syntax Error: For loops need an increment:Line ";
}
//COP stands for comparison operator  such as > < >= <=
string error::invalidCOPUsage()
{
    return "Syntax Error:Invalid Comparison Operator Usage!Cannot use it with strings and characters:Line ";
}















