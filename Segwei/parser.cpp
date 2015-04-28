/************************************************
 * Author: Sheena V
 * Date: 10/02/2015
 * Description : Parser Class for the Language
 * Module: Parser.cpp
 * Functionalities:
 * 1)Takes in the tokenized code
 * 2)Checks the grammar rules
 * 3)Allows variable hiding
 * 4)Doesn't allow redeclaration in the same scope or block using the same name
 * 5)Checks if variable has been declared and initialized before use
 *
 * **********************************************/


#include "parser.h"
#include "codegenerator.h"

parser::parser():
    vectorSize(0),
    scopeData(0),
    blockData(0),
    lineData(1),
    begin(false),
    errorLine(0),
    newBlock(1),
    end(false),
    countIf(0),
    countFor(0),
    countWhile(0),
    countElse(0),
    bracket(0),
    error(0),
    checked(false),
    idPos(0),
    id(false),
    numericalBrackets(0),
    countLoopVar(0),
    charOrStr(false),
    loop(false)
{

}
//default constructor.
parser::~parser()
{

}
int parser::startParsing(vector<scannar::Tokenizer> token)
{   vectorSize=token.size();
    for(unsigned int i=0;i<vectorSize;i++){
        parseTokenizer.push_back(token.at(i));
    }

    int getMsg=checkStructure();
    //acceessing and setting up the codegenerator class
    if(getMsg==0)
    {

        QPlainTextEdit *edit=new QPlainTextEdit;
        codegen=new codegenerator(edit);
        codegen->setModal(true);
        codegen->show();
        codegen->startCodeGen(parseTokenizer,environmentTable,dequePtr);



    }
    return getMsg;
}
//checks if the structure of the program follows grammar rules.
int parser::checkStructure()
{

    //start has to be BEGIN
    if(parseTokenizer.at(0).tokens!=scannar::BEGIN)
    {
     return 10;
    }

    for(unsigned int i=0;i<vectorSize;i++)
    {
        switch(parseTokenizer.at(i).tokens)
        {
            case scannar::IF:
                    ++countIf;
                    blockInit();
                    prevStatement.push_back("if");
                    if(checkOpenBracket(i,1)==21)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 21;
                    }
                    temp=i;
                    i=getLoopVariables(i);
                    //need to reduce by one otherwise it will miss a keyword
                    --i;

                    //if an error has occured
                    if(error!=0)
                    {
                        errorLine=parseTokenizer.at(temp).lineNum;

                        return error;
                    }

                    break;

             case scannar::END_IF:

                {
                    string statement;
                    if(prevStatement.size()>0)
                    {
                         if(prevStatement.at(prevStatement.size()-1).compare("if")==0||
                                prevStatement.at(prevStatement.size()-1).compare("else")==0){
                             statement=prevStatement.at(prevStatement.size()-1);
                        prevStatement.pop_back();
                        if(statement.compare("if")==0&&countIf>0)
                        {
                          --countIf;
                        }
                        else if(statement.compare("else")==0&&countElse>0)
                        {
                            --countElse;
                        }
                        //extra end if
                        else
                        {
                            errorLine=parseTokenizer.at(i).lineNum;
                            return 17;
                        }
                        blockExit();
                        }
                        else
                        {    errorLine=parseTokenizer.at(i).lineNum;
                            return 19;
                        }
                    }
                    else
                    {
                     errorLine=parseTokenizer.at(i).lineNum;
                     return returnEnd("if");
                    }

                    break;
        }
             case scannar::FOR:
                    if(checkOpenBracket(i,1)==21)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 21;
                    }
                    ++countFor;
                    prevStatement.push_back("for");
                    blockInit();
                    temp=i;
                    i=getLoopVariables(i);
                    //need to reduce by one otherwise it will miss a keyword
                    --i;

                    //if an error has occured
                    if(error!=0)
                    {

                        errorLine=parseTokenizer.at(i).lineNum;
                        return error;
                    }

                    break;

             case scannar::END_FOR:

            if(prevStatement.size()>0)
            {
                    if(prevStatement.at(prevStatement.size()-1).compare("for")==0)
                    {
                        prevStatement.pop_back();
                        if(countFor>0)
                        {
                            --countFor;
                        }
                        //extra end for is an error
                        else
                        {
                            errorLine=parseTokenizer.at(i).lineNum;
                            return 15;
                        }
                        blockExit();
                    }
                    else
                    {

                        errorLine=parseTokenizer.at(i).lineNum;
                        return returnEnd("for");
                    }
            }
            else
            {
                errorLine=parseTokenizer.at(i).lineNum;
                return 15;
            }
                    break;

             case scannar::WHILE:
                    if(checkOpenBracket(i,1)==21)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 21;
                    }
                    prevStatement.push_back("while");
                    ++countWhile;
                    blockInit();
                    temp=i;
                    i=getLoopVariables(i);
                    //need to reduce by one otherwise it will miss a keyword
                    --i;

                    //if an error has occured
                    if(error!=0)
                    {
                        errorLine=parseTokenizer.at(temp).lineNum;

                        return error;
                    }

                    break;

             case scannar::END_WHILE:
            if(prevStatement.size()>0)
            {

                    if(prevStatement.at(prevStatement.size()-1).compare("while")==0)
                    {
                        prevStatement.pop_back();
                    if(countWhile>0)
                    {
                        --countWhile;
                    }
                    //extra end while
                    else
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 16;
                    }
                    blockExit();
                    }
                    else
                    {

                        errorLine=parseTokenizer.at(i).lineNum;
                        return returnEnd("while");
                    }
            }
            else
            {
                errorLine=parseTokenizer.at(i).lineNum;
                return 16;
            }
                    break;

             case scannar::BEGIN:
                   if(!begin)
                   {

                        blockInit();
                        begin=true;

                   }
                   else
                   {
                       //multiple initialisation of begin
                       errorLine=parseTokenizer.at(i).lineNum;
                       return 11;
                   }

                    break;

             case scannar::END:

                     missingEnd=returnEnd("end");
                    if(missingEnd!=0)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return missingEnd;
                    }
                    if(!end)
                    {
                        end=true;
                        //avoid anything after an end is found
                        if((i+1)<vectorSize)
                        {
                            errorLine=parseTokenizer.at(i).lineNum;
                            return 12;
                        }
                    }

                    break;

             case scannar::READ:
                    if(checkOpenBracket(i,1)==21)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 21;
                    }
                    //invalid command
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 30;


             case scannar::PRINT:
                    if(checkOpenBracket(i,1)==21)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 21;
                    }

                    temp=i;
                    i=findSemiColon(i);
                    if(error!=0)
                    {
                        errorLine=parseTokenizer.at(temp).lineNum;
                        return error;
                    }


                    break;
             case scannar::T_LSB:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 23;


             case scannar::T_RSB:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 23;
                    break;

             case scannar::T_RCB:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 23;
                    break;

             case scannar::T_LCB:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 23;
                    break;

             case scannar::T_LBR:
                    ++bracket;
                    break;

             case scannar::T_RBR:
                    if(bracket>0)
                    {
                        --bracket;
                    }
                    else
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 20;
                    }
                    break;
             case scannar::ELSE:

                    if(prevStatement.at(prevStatement.size()-1).compare("if")==0){
                        prevStatement.pop_back();
                        prevStatement.push_back("else");
                            if(checkOpenBracket(i,1)==0)
                            {
                                errorLine=parseTokenizer.at(i).lineNum;
                                return 22;
                            }
                            error=0;
                    //delimits for if and starts a new block/scope as well
                            if(countIf>0)
                            {
                                --countIf;
                                ++countElse;
                            }
                            //goes into this loop if it is not preceded by an if
                            else
                            {
                                 errorLine=parseTokenizer.at(i).lineNum;
                                 return 14;
                            }
                            blockExit();
                            blockInit();
                            }
                    else{
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 14;
                    }
                    break;
             case scannar::EQUALS:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 27;
                    break;

             case scannar::SEMICOLON:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 46;
                    //break;

             case scannar::T_DEC:
                {
                    if(parseTokenizer.at(i+1).tokens!=scannar::T_ID)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 24;
                    }
                    temp=i;
                    i=findSemiColon(i);

                    if(error!=0)
                    {
                        errorLine=parseTokenizer.at(temp).lineNum;

                        return error;
                    }

                    break;
             }

             case scannar::T_INT:
        {
                    if(parseTokenizer.at(i+1).tokens!=scannar::T_ID)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 24;
                    }
                    temp=i;

                    i=findSemiColon(i);

                    if(error!=0)
                    {
                        errorLine=parseTokenizer.at(temp).lineNum;

                        return error;
                    }
                  break;
        }

             case scannar::T_CHAR:
                        {
                            if(parseTokenizer.at(i+1).tokens!=scannar::T_ID)
                            {
                                errorLine=parseTokenizer.at(i).lineNum;
                                return 24;
                            }
                            temp=i;
                            i=findSemiColon(i);

                            if(error!=0)
                            {
                                errorLine=parseTokenizer.at(temp).lineNum;
                                return error;
                            }
                            break;
                     }

             case scannar::T_STR:
                {
                    if(parseTokenizer.at(i+1).tokens!=scannar::T_ID)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        return 24;
                    }
                    temp=i;
                    i=findSemiColon(i);

                    if(error!=0)
                    {
                        errorLine=parseTokenizer.at(temp).lineNum;
                        return error;
                    }
                   break;
             }

             case scannar::T_OP:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 27;
                    break;
             case scannar::T_FSLASH:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 37;

             case scannar::T_BSLASH:
            //invalid character or comment
                   errorLine=parseTokenizer.at(i).lineNum;
                   return 37;

             case scannar::T_DOP:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 37;

            //no occasion where the literals should be assessed
            //here it should be assigned to a variable or in a loop
             case scannar::T_CHARLit:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 30;

             case scannar::T_STRLit:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 30;

             case scannar::T_DECLit:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 30;

             case scannar::T_INTLit:
                    errorLine=parseTokenizer.at(i).lineNum;
                    return 30;

             case scannar::T_ID:
        {
            //declaring the variable


                    if(parseTokenizer.at(i+1).tokens!=scannar::EQUALS)
                    {
                        errorLine=parseTokenizer.at(i).lineNum;
                        // a= a+b; needs to have an equals
                        // doing a or a+34 is meaningless unless it is stored into
                        //a variable.

                        return 34;
                    }
                    temp=i;
                    i=findSemiColon(i);

                    if(error!=0)
                    {
                        errorLine=parseTokenizer.at(temp).lineNum;

                        return error;
                    }
                     break;
        }
           default:

                 break;

        }
    }

    if(!end)
    {
        errorLine=vectorSize+1;
        return 13;
    }
    else if(bracket>0||bracket<0)
    {
        errorLine=vectorSize+1;
        return 13;
    }
    //else is when everything is fine
    else
    {

    //default return : when everything works fine
    return 0;
    }
    return 0;
}

int parser::getLineNumber()
{
    return errorLine;
}

void parser::blockInit()
{
    ++scopeData;
    blockData=newBlock;
    parent.push_back(blockData);
    ++newBlock;
}

void parser::blockExit()
{
    --scopeData;
    parent.pop_back();
    blockData=parent.at(parent.size()-1);
}
//i is the position before open bracket
int parser::checkOpenBracket(int i,int key)
{
    if(key==1)
    {
        if(parseTokenizer.at(i+1).tokens!=scannar::T_LBR)
        {
            //error
            error=21;
            return 21;
        }
    }
    if(key==2)
    {
        if(trackSemiColon.at(i+1).tokens!=scannar::T_LBR)
        {

            //error
            error=21;
            return 21;
        }
    }
    //is okay a.k.a it works fine
    return 0;
}
//does static type checking here. Checks if the assigned types are compatible
bool parser::checkType(scannar::token value,scannar::token type)
{
    if(!loop)
    {


       if(type==scannar::T_CHAR&&value==scannar::T_CHARLit)
       {
           return true;
       }
       else if(type==scannar::T_STR&&value==scannar::T_STRLit)
       {
           return true;
       }
       else if(type==scannar::T_DEC&&value==scannar::T_DECLit)
       {
           return true;
       }
       else if(type==scannar::T_INT&&value==scannar::T_INTLit)
       {
           return true;
       }
       else  if(type==scannar::T_CHAR&&value==scannar::T_CHAR)
       {
           return true;
       }
       //from here onwards its for assignment variables like a=b;
       else if(type==scannar::T_STR&&value==scannar::T_STR)
       {
           return true;
       }
       else if(type==scannar::T_STR)
       {
           return true;
       }
       else if(type==scannar::T_DEC&&value==scannar::T_DEC)
       {
           return true;
       }
       else if(type==scannar::T_INT&&value==scannar::T_INT)
       {
           return true;
       }
       //if it is a mathematical operator.
       else if(type==scannar::T_INT&&value==scannar::T_OP)
       {
            return true;
       }
       else if(type==scannar::T_DEC&&value==scannar::T_OP)
       {
            return true;
       }
       //overload the function for '+'--> becomes append
       else if(type==scannar::T_STR&&value==scannar::T_OP)
       {
            return true;
       }
       else if(type==scannar::T_DEC&&value==scannar::T_INT)
       {
           return true;
       }
       else if(type==scannar::T_DEC&&value==scannar::T_INTLit)
       {
           return true;
       }
       else if(value==scannar::READ)
       {
          return true;
       }
       else if(type==scannar::T_DEC&&(value==scannar::T_RBR||value==scannar::T_LBR))
       {
           return true;
       }
       else if(type==scannar::T_INT&&(value==scannar::T_RBR||value==scannar::T_LBR))
       {
           return true;
       }
       //returns true for all cos print converts all types to type sentence
       else if(trackSemiColon.at(0).tokens==scannar::PRINT)
       {
           return true;
       }
       else
       {
           return false;
       }
    }
    //if it is a loop
    else
    {
        if(type==scannar::T_CHAR&&value==scannar::T_CHARLit)
        {
            return true;
        }
        else if(type==scannar::T_STR&&value==scannar::T_STRLit)
        {
            return true;
        }
        else if(type==scannar::T_DEC&&value==scannar::T_DECLit)
        {
            return true;
        }
        else if(type==scannar::T_INT&&value==scannar::T_INTLit)
        {
            return true;
        }
        else  if(type==scannar::T_CHAR&&value==scannar::T_CHAR)
        {
            return true;
        }
        //from here onwards its for assignment variables like a=b;
        else if(type==scannar::T_STR&&value==scannar::T_STR)
        {
            return true;
        }

        else if(type==scannar::T_DEC&&value==scannar::T_DEC)
        {
            return true;
        }
        else if(type==scannar::T_INT&&value==scannar::T_INT)
        {
            return true;
        }
        //if it is a mathematical operator.
        else if(type==scannar::T_INT&&value==scannar::T_OP)
        {
             return true;
        }
        else if(type==scannar::T_DEC&&value==scannar::T_OP)
        {
             return true;
        }
        //overload the function for '+'--> becomes append
        else if(type==scannar::T_STR&&value==scannar::T_OP)
        {
             return true;
        }
        else if(type==scannar::T_DEC&&value==scannar::T_INT)
        {
            return true;
        }
        else if(type==scannar::T_DEC&&value==scannar::T_INTLit)
        {
            return true;
        }
        else if(type==scannar::T_INT&&value==scannar::T_DEC)
        {
            return true;
        }
        else if(type==scannar::T_INT&&value==scannar::T_DECLit)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

}
//changes the position value of the environment pointer
//rehashing the values
void parser::modifyDqPositions(int insertPos)
{
    for(unsigned int i=insertPos;i<dequePtr.size();i++)
    {
        dequePtr.at(i).position+=1;
    }
}
//int pos here refers to the position of the id in the vector starting from 0...
int parser::addToEnvironmentTable(int pos, int key=1)
{
    environmentVars.block=blockData;
    environmentVars.lineNum=trackSemiColon.at(pos).lineNum;
    environmentVars.variable=trackSemiColon.at(pos).variable;
    environmentVars.tokens=trackSemiColon.at(pos-1).tokens;
   if(parent.size()>=2)
   {
    environmentVars.pred=parent.at(parent.size()-2);
   }
   else
   {
       environmentVars.pred=0;

   }

    if(key==1)
    {


    //add the variable to the environment table

     if(parseTokenizer.at(pos+1).tokens==scannar::EQUALS)
     {

        //checks if variable is initialised to the correct type
        if(checkType(parseTokenizer.at(pos+2).tokens,parseTokenizer.at(pos-1).tokens))
        {

            environmentVars.init=true;
        }
         //if variable not initialised to correct type
        else
        {
            errorLine=parseTokenizer.at(pos).lineNum;
            error=25;
            return 25;
        }
     }
      //if variable just only declared
      else
      {
            environmentVars.init=false;
      }
    }
    if(key==2)
    {

         if(trackSemiColon.size()>2&&trackSemiColon.at(pos+1).tokens==scannar::EQUALS)
         {

                environmentVars.init=true;

         }
          //if variable just only declared
          else
          {
                environmentVars.init=false;
          }
         //need to push
         addToDqandEnvironment();
         printEnv();
         printDq();
    }
     return 0;
}
//debugging and testing code
void parser::printDq()
{
    cout<<"-------------Deque Value--------------------------\n"<<endl;
    for(int i=0;i<dequePtr.size();i++)
    {
        cout<<"block= "<<dequePtr.at(i).block<<" position= "<<dequePtr.at(i).position<<endl;
    }
}
//debugging and testing code.
void parser::printEnv()
{
    cout<<"-------------Env Value--------------------------\n"<<endl;
    for(int i=0;i<environmentTable.size();i++)
    {
        cout<<"block= "<<environmentTable.at(i).block<<" var= "<<environmentTable.at(i).variable<<" token= "<<environmentTable.at(i).tokens<<" init "
           <<environmentTable.at(i).init<<" value= "<<environmentTable.at(i).value<<endl;
    }
}
//int i is the position to start looking for semicolon
//i is base 1
int parser::findSemiColon(int i)
{
    //clear old data first
    trackSemiColon.clear();
    checked=false;
    temp=i;
    loop=false;
    int openBracket=0;

    //get upto but not including the ';'
    while(i<parseTokenizer.size()&&parseTokenizer.at(i).tokens!=scannar::SEMICOLON)
    {
        if(parseTokenizer.at(i).tokens==scannar::T_LBR)
        {
            if(parseTokenizer.at(i-1).tokens!=scannar::READ
                    &&parseTokenizer.at(i-1).tokens!=scannar::PRINT)
            {

                errorLine=parseTokenizer.at(temp).lineNum;
                error =22;
            }

            else
            {
                ++openBracket;
            }


        }
        if(parseTokenizer.at(i).tokens==scannar::T_RBR)
        {
            --openBracket;
        }
        trackSemiColon.push_back(parseTokenizer.at(i));
        ++i;
    }

    if(i>=parseTokenizer.size()-1)
    {
        if(parseTokenizer.at(parseTokenizer.size()-1).tokens!=scannar::END)
        {
            errorLine=parseTokenizer.at(temp).lineNum;
            //missing end
            error=13;
        }
        else
        {
            errorLine=parseTokenizer.at(temp).lineNum;
            //missing semicolon
            error= 29;
        }
    }
    else if(openBracket>0||openBracket<0)
    {
        errorLine=parseTokenizer.at(temp).lineNum;
        error= 22;
    }
    else if(trackSemiColon.size()!=0)
    {
    parseStatement();
    }
    else
    {
        errorLine=parseTokenizer.at(temp).lineNum;
        error= 27;
    }
    printTrackSemiColon();
    return i;
}
//check if it is a loop
bool parser::isLoop(int pos)
{

    if(parseTokenizer.at(pos).tokens==scannar::IF||
       parseTokenizer.at(pos).tokens==scannar::FOR||
       parseTokenizer.at(pos).tokens==scannar::WHILE||
       parseTokenizer.at(pos).tokens==scannar::ELSE)
    {
        return true;
    }
    //statement ends
    if(parseTokenizer.at(pos).tokens==scannar::END_IF||
            parseTokenizer.at(pos).tokens==scannar::END_FOR||
            parseTokenizer.at(pos).tokens==scannar::END_WHILE||
            parseTokenizer.at(pos).tokens==scannar::END)
    {

        return true;
    }
    return false;
}
//this function checks the statements
int parser::parseStatement()
{
    bool assignRead =false;
    for(int i=0;i<trackSemiColon.size();i++)
    {
         switch(trackSemiColon.at(i).tokens)
    {
        case scannar::IF:
         {

                int check=checkLoop(0);
                if(check!=1)
                {
                    errorLine=trackSemiColon.at(i).lineNum;
                    error=check;
                }
                return check;

         }


         case scannar::FOR:
            {

                 int check=forLoop(0);
                 if(check!=1)
                 {
                     errorLine=trackSemiColon.at(i).lineNum;
                     error=check;
                 }
                 return check;

         }



         case scannar::WHILE:
         {

                int check=checkLoop(0);
                if(check!=1)
                {
                    errorLine=trackSemiColon.at(i).lineNum;
                    error=check;
                }
                return check;

         }
                break;



         case scannar::READ:

             //This part here ensures that the read is assigned a variable
             //cos we need to send the reading into a variable
            //if it is a type ID
             if(trackSemiColon.at(0).tokens==scannar::T_ID
                     &&trackSemiColon.size()!=5)
             {
                errorLine=trackSemiColon.at(i).lineNum;
                error=27;
                return 27;
             }
             //if it is a literal type
             else if(trackSemiColon.at(0).tokens!=scannar::T_ID
                     &&trackSemiColon.size()!=6)
             {
                 errorLine=trackSemiColon.at(i).lineNum;
                 error=27;
                 return 27;
             }
             else
             {
            // cout<<"in read in parseStat()"<<endl;
                if(checkOpenBracket(i,2)==21)
                {
                    errorLine=trackSemiColon.at(i).lineNum;
                    error=21;
                    return 21;
                }
                //needs to be +2 cos readFromScreen() the closing is 2 from readFromScreen
                if(trackSemiColon.at(i+2).tokens!=scannar::T_RBR)
                {
                    //checking for closed brackets

                    errorLine=parseTokenizer.at(i).lineNum;
                    error=38;
                    return 38;
                }
                i+=2;
                //has to be only readFromScreen() and no more
                if(i!=trackSemiColon.size()-1)
                {
                    errorLine=trackSemiColon.at(i).lineNum;
                    error=30;
                    //invalid command received
                    return 30;
                }
                if(assignRead)
                {
                   addToEnvironmentTable(1,2);
                }

             }
                return 1;

//need to check in between
         case scannar::PRINT:
         {
                if(checkOpenBracket(i,2)==21)
                {
                    errorLine=trackSemiColon.at(i).lineNum;
                    error=21;
                    return 21;
                }
                //ensures that the last stored is a close bracket )
                 if(trackSemiColon.at(trackSemiColon.size()-1).tokens!=scannar::T_RBR)
                 {
                     errorLine=trackSemiColon.at(i).lineNum;
                     error=38;
                     return 38;
                 }
                 int check;
               //  cout<<trackSemiColon.size()<<endl;
                 if(2<trackSemiColon.size()-1)
                 {

                  check =checkAssignment(2);
                 }
                 else
                 {
                     errorLine=trackSemiColon.at(i).lineNum;
                     //invalid command
                     error=30;
                     return error;
                 }

                 if(check!=1)
                 {
                     errorLine=trackSemiColon.at(i).lineNum;
                     error=check;
                     return check;
                 }
                 else
                 {
                     return 0;
                 }
                break;
         }
         case scannar::T_LSB:
                errorLine=trackSemiColon.at(i).lineNum;
                error=23;
                return 23;


         case scannar::T_RSB:
                errorLine=trackSemiColon.at(i).lineNum;
                error=23;
                return 23;


         case scannar::T_RCB:
                errorLine=trackSemiColon.at(i).lineNum;
                return 23;
                break;

         case scannar::T_LCB:
                errorLine=trackSemiColon.at(i).lineNum;
                error=23;
                return 23;


         case scannar::T_LBR:
                ++bracket;
                break;

         case scannar::T_RBR:
                if(bracket>0)
                {
                    --bracket;
                }
                else
                {
                    errorLine=trackSemiColon.at(i).lineNum;
                    error=20;
                    return 20;
                }
                break;




         case scannar::T_DEC:
         {

                if(trackSemiColon.at(i+1).tokens!=scannar::T_ID)
                {
                    errorLine=trackSemiColon.at(i).lineNum;
                    error=24;
                    return 24;
                }
                int check =checkVarType();
                //if it is a read
                if(check==0)
                {
                    //goes to the readFromScreen()
                    i+=2;
                    assignRead=true;
                }
                else if(check==1)
                {

                    addToEnvironmentTable(i+1,2);

                    return 0;
                }
                else
                {
                    errorLine=trackSemiColon.at(i).lineNum;

                    error=check;
                    return check;
                }

                break;
         }

         case scannar::T_INT:
         {

                if(trackSemiColon.at(i+1).tokens!=scannar::T_ID)
                {
                    errorLine=trackSemiColon.at(i).lineNum;
                    error=24;
                    return 24;
                }
                int check =checkVarType();
                //if it is a read
                if(check==0)
                {
                    //goes to the readFromScreen()
                    i+=2;
                    assignRead=true;
                }
                else if(check==1)
                {

                    addToEnvironmentTable(i+1,2);

                    return 0;
                }
                else
                {
                    errorLine=trackSemiColon.at(i).lineNum;

                    error=check;
                    return check;
                }

                break;
         }


         case scannar::T_CHAR:
         {

                    if(trackSemiColon.at(i+1).tokens!=scannar::T_ID)
                    {
                        errorLine=trackSemiColon.at(i).lineNum;
                        error=24;
                        return 24;
                    }
                    int check =checkVarType();
                    //if it is a read
                    if(check==0)
                    {
                        //goes to the readFromScreen()
                        i+=2;
                        assignRead=true;
                    }
                    else if(check==1)
                    {

                        addToEnvironmentTable(i+1,2);

                        return 0;
                    }
                    else
                    {
                        errorLine=trackSemiColon.at(i).lineNum;

                        error=check;
                        return check;
                    }

                    break;
             }

         case scannar::T_STR:
             {

                    if(trackSemiColon.at(i+1).tokens!=scannar::T_ID)
                    {
                        errorLine=trackSemiColon.at(i).lineNum;
                        error=24;
                        return 24;
                    }
                    int check =checkVarType();
                    //if it is a read
                    if(check==0)
                    {
                        //goes to the readFromScreen()
                        i+=2;
                        assignRead=true;
                    }
                    else if(check==1)
                    {

                        addToEnvironmentTable(i+1,2);

                        return 0;
                    }
                    else
                    {
                        errorLine=trackSemiColon.at(i).lineNum;

                        error=check;
                        return check;
                    }

                    break;
             }


         case scannar::T_ID:
         {

            if(i==0)
            {

               if (!lookUp(0))
               {

                   //identifier not found
                   errorLine=trackSemiColon.at(i).lineNum;
                   error=24;
                   return 24;
               }
            }

                int check =checkVarType();

                //if it is a read
                if(check==0)
                {

                    //goes to the readFromScreen()
                    ++i;

                    assignRead=true;
                }
                // else if set initialized to true
                else if(check==1)
                {

                    environmentTable.at(idPos).init=true;

                    return 0;
                }
                else
                {
                    errorLine=trackSemiColon.at(i).lineNum;

                    error=check;
                    return check;
                }

                break;
         }
       default:

            errorLine=trackSemiColon.at(i).lineNum;
            //invalid command received
            return 30;
             //break;

    }
    }
    //need to do this otherwise the vector will still contain some
    //old information
    trackSemiColon.clear();
}

bool parser::checkCloseBrackets(int i, int key)
{
    if(key==1)
    {
    if(parseTokenizer.at(i+1).tokens!=scannar::T_RBR)
    {
        //error state
        return false;
    }
    }
    if(key==2)
    {
        if(trackSemiColon.at(i+1).tokens!=scannar::T_RBR)
        {
            //error state
            return false;
        }
    }
    return true;
}

//int i// i stands for T_ID position
//checks if the variables are being redeclared in the same block
//if not being redeclared then insert it to the environment table
int parser::checkVariables(int i)
{
    int sizeDq=dequePtr.size();
    int k=1;//just a loop variable
    //loop checks if the block has ever been declared before by checking
    //the "pointers"

    while(k<=sizeDq&&dequePtr.at(sizeDq-k).block>blockData)
    {
        ++k;

    }

    //means the block has no variable prior to this
    //also block size is smaller than the others
    if(k>sizeDq)
    {

         int errorState=addToEnvironmentTable(i,1);
         if(errorState==0)
         {
         ptr.block=blockData;
         ptr.position=0;
         dequePtr.insert(dequePtr.begin(),ptr);
         modifyDqPositions(1);
         environmentTable.push_front(environmentVars);
         }
         else
         {
             errorLine=trackSemiColon.at(i).lineNum;
             return errorState;
         }

    }
    else
    {

        //some variables in that block has been declared
        //so need to check those
        //doesn't allow duplicate varible names in the
        //same block level
        if(dequePtr.at(sizeDq-k).block==blockData)
        {

            int getPosition=dequePtr.at(sizeDq-k).position;
            //just a looping variable
            int j=0;
            int tableSize=environmentTable.size();
            bool variableExists=false;
            //to check if name exists--lookUp--
            while(getPosition+j<tableSize&&
                  blockData==environmentTable.at(getPosition+j).block)
            {
                //checking if variable name already exists
                //checking if its being redeclared
                if(environmentTable.at(getPosition+j).variable.compare(
                   trackSemiColon.at(i).variable)==0)

                {
                    if(!checkType(trackSemiColon.at(i+2).tokens,trackSemiColon.at(i-1).tokens))
                    {
                           variableExists=true;
                    }
                    else
                    {
                    errorLine=trackSemiColon.at(i).lineNum;
                    return 26;
                    }

                }
                ++j;
            }
            //name doesn't exist BUT block has been initialized before
            if(!variableExists){
                int errorState=addToEnvironmentTable(i);
                if(errorState==0)
                {
                ptr.block=blockData;
                //j because at j is when it either reached the end or
                //a bigger block value
                modifyDqPositions(j);
                environmentTable.insert(environmentTable.begin()+(getPosition+j),environmentVars);
                }
                else
                {
                    errorLine=trackSemiColon.at(i).lineNum;
                    return errorState;
                }
            }

        }
        //the whole block not been declared before
        else
        {

            int getPosition=dequePtr.at(sizeDq-(k-1)).position;
            int errorState=addToEnvironmentTable(i);
            if(errorState==0)
            {
            ptr.block=blockData;
            ptr.position=getPosition;
            dequePtr.insert(dequePtr.begin()+(sizeDq-(k-2)),ptr);
            //j because at j is when it either reached the end or
            //a bigger block value
            modifyDqPositions(sizeDq-k);
            environmentTable.insert(environmentTable.begin()+(getPosition),environmentVars);
            }
            else
            {
                errorLine=parseTokenizer.at(i).lineNum;
                return errorState;
            }


        }

    }
    printDq();
    printEnv();
    return 0;
}
//int i is the position of T_ID starting from 0,1... in the trackSemiColon vector
//this lookup is used for initilialisation
bool parser::isInit(int i)
{
    int sizeDq=dequePtr.size();
    int k=1;//just a loop variable

    //loop checks if the block has ever been declared before by checking
    //the "pointers"

    while(k<=sizeDq&&dequePtr.at(sizeDq-k).block>blockData)
    {
        ++k;

    }
    //means the block has no variable prior to this
    //also block size is smaller than the others
    if(k>sizeDq)
    {

        //non existant
        idType=trackSemiColon.at(0).tokens;
        return false;
    }


    else
    {

        if(dequePtr.at(sizeDq-k).block==blockData)
        {

            int getPosition=dequePtr.at(sizeDq-k).position;
            //just a looping variable
            int j=0;
            int tableSize=environmentTable.size();

            //to check if name exists--lookUp--
            while(getPosition+j<tableSize&&
                  blockData==environmentTable.at(getPosition+j).block)
            {
                //checking if variable name exists

                if(environmentTable.at(getPosition+j).variable.compare(
                   trackSemiColon.at(i).variable)==0)

                {
                    return true;

                }
                ++j;
            }
            //name doesn't exist
               idType=trackSemiColon.at(0).tokens;
               return false;

        }
        //the whole block not been declared before
        else
        {
            idType=trackSemiColon.at(0).tokens;
            return false;
        }
    }

}
//checks the variable type
//checks if correct type is being assigned
//pos: position of var_name before equals
int parser::checkVarType()
{
    //initialisation/declaration code
    if(trackSemiColon.at(0).tokens==scannar::T_CHAR||
         trackSemiColon.at(0).tokens==scannar::T_STR||
            trackSemiColon.at(0).tokens==scannar::T_DEC||
            trackSemiColon.at(0).tokens==scannar::T_INT)
    {
        //if the variable has not been declared yet
        if(!isInit(1))
        {

            //will return 1 for pass 0 for fail
            //returns 2 for  not initialised
            //initialisation & assignments
            if(trackSemiColon.size()>2&&trackSemiColon.at(2).tokens==scannar::EQUALS)
            {

                return checkAssignment(3);

            }
            //just declaration
            else if(trackSemiColon.size()==2)
            {

                if(trackSemiColon.at(1).tokens==scannar::T_ID)
                {
                    if(!isInit(1))
                    {
                        //pass
                        return 1;
                    }
                    else
                    {
                        //name already exists
                        return 29;
                    }
                }
                else
                {
                    //invalid operator or type
                    return 27;
                }
            }
            // it is neither initialisation nor is it a declaration
            //eg int;
            else
            {
                //invalid command received
                return 30;
            }

        }
        else
        {

            //name already exists for that block in the environment table
            return 26;
        }
    }
    //if it is T_ID and not a new declaration
    else
    {
            //variable name found
            if(lookUp(0))
            {
                //more than 2 because eg a= is already 2
                if(trackSemiColon.size()>2)
                {
                   return checkAssignment(2);
                }
                else
                {
                    //this command is considered invalid
                    //it doesn't do anything
                    //eg "HELOOOOOO";-- it does nothing
                    //eg2 a;--its not a declaration
                    return 31;
                }
            }
            else
            {
                //variable has not been declared
                return 24;
            }

    }

}
//returns 2 for not init
//return 1 for pass
//returns other values for fail-->corresponds to the error message

//pos here stands for position of the equals starting from base 1...
int parser::checkAssignment(int pos)
{

       id=false;
       numericalBrackets=0;
       int vectorSize=pos+1;
    //assignment or initialization
    if(trackSemiColon.size()>=3&&trackSemiColon.at(pos-1).tokens==scannar::EQUALS)
    {
        int vectorSize=pos+1;

         while(vectorSize<=trackSemiColon.size())
         {

                if(trackSemiColon.at(0).tokens==scannar::T_ID)
                {
                       //variable not found
                        if(!lookUp(0))
                        {
                                return 31;
                        }
                }
                //if the item before the semicolon is an equals return error
                if(trackSemiColon.at(vectorSize-1).tokens==scannar::EQUALS)
                {
                    return 27;
                }
                if((trackSemiColon.at(vectorSize-1).tokens==scannar::T_CHAR
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_CHARLit
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_DEC
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_INT
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_INTLit
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_DECLit
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_STR
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_STRLit
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_ID
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_OP
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_RBR
                        ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_LBR ))

                {

                    //this section here is used mainly for the for loops
                    if(idType==scannar::T_CHAR||idType==scannar::T_CHARLit
                           ||idType==scannar::T_STR||idType==scannar::T_STRLit
                           ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_CHAR
                           ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_CHARLit
                           ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_STR
                           ||idType==scannar::T_STRLit )
                    {
                        charOrStr=true;
                    }
                    //type literal just to set the id
                    //id type will be an indicator that the next should be an operator
                    if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_CHARLit&&
                            trackSemiColon.at(vectorSize-1).variable.length()>3)
                    {
                        return 37;
                    }
                    //if it is a loop code
                    if(loop)
                    {

                        if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_CHAR&&
                                trackSemiColon.at(vectorSize-1).variable.length()>3)
                        {

                            return 37;
                        }
                    }
                   //if it is a literal type and not a variable type
                    if(trackSemiColon.at(vectorSize-1).tokens!=scannar::T_ID&&
                       !checkType(trackSemiColon.at(vectorSize-1).tokens,idType)
                           &&trackSemiColon.at(vectorSize-1).tokens!=scannar::T_OP)
                    {
                        //actual type and expected type differs
                        return 25;
                    }

                    //literal type but does something like decimal a= 2  2;
                    else if(trackSemiColon.at(vectorSize-1).tokens!=scannar::T_OP&&
                            trackSemiColon.at(vectorSize-1).tokens!=scannar::T_LBR&&
                            trackSemiColon.at(vectorSize-1).tokens!=scannar::T_RBR&&
                           checkType(trackSemiColon.at(vectorSize-1).tokens,idType) &&id)
                    {

                        return 30;
                    }
                    //for id types
                    else if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_ID)
                    {

                        if(!lookUp(vectorSize-1))
                        {
                        //make sure that variable is initialized and is of the correct type
                            return 35;
                        }
                        if(id)
                        {

                            return 30;
                        }
                        id=true;

                    }
                    else if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_CHARLit
                           ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_INTLit
                           ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_DECLit
                           ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_STRLit
                           ||trackSemiColon.at(vectorSize-1).tokens==scannar::T_ID)
                    {
                        id=true;
                    }
                    else
                    {
                            if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_OP)

                        {
                            //cannot do anything to type char
                            if(idType==scannar::T_CHAR||idType==scannar::T_CHARLit)
                            {
                                return 36;
                            }
                            id=false;
                        //doesn't support function overload eg a=+a;
                        //it supports a=-a;
                        //after the or symbol it is checking if the operator is the last symbol
                        //if so then false
                            if(vectorSize-1==pos&&trackSemiColon.at(vectorSize-1).variable.compare("-")!=0
                                    ||vectorSize==trackSemiColon.size())
                            {
                                    //invalid operator type
                                    return 27;
                            }

                            //if it is an operator
                            else
                            {

                                    //if variable is + is false ie the only op that can go with
                                    //string is a +
                                    if(trackSemiColon.at(vectorSize-1).variable.compare("+")!=0&&
                                            trackSemiColon.at(0).tokens==scannar::T_STR)
                                    {

                                        return 28;
                                    }
                                    //checks the operator is compatible with the variable type
                                    //also ensures that the operator is not of type ! or
                                    //of the type !=
                                    if (!checkType(trackSemiColon.at(vectorSize-1).tokens,
                                                        trackSemiColon.at(0).tokens)&&
                                             trackSemiColon.at(0).tokens!=scannar::T_STR&&
                                            trackSemiColon.at(vectorSize-1).variable.compare("!")==0
                                            &&trackSemiColon.at(vectorSize-1).variable.compare("!=")==0)
                                    {
                                        //returns actual type and expected type is different
                                        return 25;
                                    }
                                    // ! and != is considered an invalid operator type
                                    // those operators can only be used in the loops or if-statement
                                    if(trackSemiColon.at(vectorSize-1).variable.compare("!")==0
                                            ||trackSemiColon.at(vectorSize-1).variable.compare("!=")==0)
                                    {
                                        //invalid operator

                                        return 27;
                                    }
                              }
                        }

                    }

                    if(idType==scannar::T_DEC||idType==scannar::T_INT)
                   {

                       if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_LBR)
                       {
                           ++numericalBrackets;
                       }
                       if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_RBR)
                       {
                           //the bracket should only close on an id or a literal
                           if(!id)
                           {
                               //return invalid operator or command given

                               return 30;
                           }
                           --numericalBrackets;
                       }
                   }

                    else if(trackSemiColon.at(vectorSize-1).tokens!=scannar::T_OP)

                    {
                        id=true;
                    }

                }
                //if it is read
                else if(trackSemiColon.at(vectorSize-1).tokens==scannar::READ)
                {
                    // here it just returns to the caller
                    //and tells the caller it is a read
                    if(vectorSize-1==pos)
                    {
                        return 0;
                    }
                    else
                    {
                        return 32;
                    }
                }
                else
                {
                    //error if print is used after = sign
                    if(trackSemiColon.at(vectorSize-1).tokens==scannar::PRINT)
                    {

                        return 27;
                    }

                    else
                    {
                        return 30;

                    }
                }

                        ++vectorSize;
         }
         if(numericalBrackets>0)
         {
             //close brackets not found
             return 38;
         }
         if(numericalBrackets<0)
         {
             //extra close brackets
             return 43;
         }

      return 1;
    }
    //printToScreen();
    //send in after '('
    //print only allows variable types and string literals
    else if(trackSemiColon.at(0).tokens==scannar::PRINT)
    {

        if(trackSemiColon.at(2).tokens!=scannar::T_ID&&
                trackSemiColon.at(2).tokens!=scannar::T_STRLit)
        {
            //error invalid type

            return 25;
        }

        while(vectorSize<=trackSemiColon.size())
        {
            if((trackSemiColon.at(vectorSize-1).tokens!=scannar::T_ID&&
               trackSemiColon.at(vectorSize-1).tokens!=scannar::T_STRLit&&
               trackSemiColon.at(vectorSize-1).tokens!=scannar::T_OP)
                    &&vectorSize-1<trackSemiColon.size()-1)

            {
                //error invalid type

                return 25;

            }
            else if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_ID&&!id)
            {
                id=true;

                if(!lookUp(vectorSize-1))
                {
                    //variable not been declared
                    return 35;
                }
            }
            //if it is a string literal
            else if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_STRLit&&!id)
            {

                id=true;

            }
            //doesn't allow printToScreen(+);
            else if(trackSemiColon.at(vectorSize-1).variable.compare("+")==0&&id)
            {
                id=false;


            }

            else if(trackSemiColon.at(vectorSize-1).tokens==scannar::T_OP &&
               trackSemiColon.at(vectorSize-1).variable.compare("+")!=0 )
            {
                //error invalid operator used
                //only operator allowed is + as append

                return 27;
            }
           else if(vectorSize==trackSemiColon.size()&&
                    trackSemiColon.at(vectorSize-1).tokens==scannar::T_RBR)
            {
                if(!id)
                {
                    //invalid operator
                    return 27;
                }

                return 1;
            }
            else
            {
                //if last item in the vector is not a ')'

                if(vectorSize==trackSemiColon.size()&&
                        trackSemiColon.at(vectorSize-1).tokens!=scannar::T_RBR)
                {
                    //return closing bracket expected
                    return 38;
                }

            }
            ++vectorSize;

        }
    }
    else
    {
        //invalid command: expecting =
        return 30;
    }


}
//debugging
void parser::printTrackSemiColon()
{
    cout<<"--------------------PRINT TRACKSEMICOLON-----------\n"<<endl;
    for(int i=0;i<trackSemiColon.size();i++)
    {
        cout<<"i= "<<i<<" token= "<<trackSemiColon.at(i).tokens<<" var= "<<trackSemiColon.at(i).variable <<endl;
    }

}

void parser::addToDqandEnvironment()
{
    //declaring the variable

            int sizeDq=dequePtr.size();
            int k=1;//just a loop variable
            //loop checks if the block has ever been declared before by checking
            //the "pointers"

            while(k<=sizeDq&&dequePtr.at(sizeDq-k).block>blockData)
            {
                ++k;

            }

            //means the block has no variable prior to this
            //also block size is smaller than the others
            //to push infront
            if(k>sizeDq)
            {
                 ptr.block=blockData;
                 ptr.position=0;
                 dequePtr.insert(dequePtr.begin(),ptr);
                 modifyDqPositions(1);
                 environmentTable.push_front(environmentVars);
            }
            else
            {

                //block been declared before
                if(dequePtr.at(sizeDq-k).block==blockData)
                {
                   int getPosition=dequePtr.at(sizeDq-k).position;
                    //just a looping variable
                    int j=0;
                    while(getPosition+j<environmentTable.size()&&
                          blockData==environmentTable.at(getPosition+j).block)
                    {
                        ++j;
                    }
                    ptr.block=blockData;
                    //j because at j is when it either reached the end or
                    //a bigger block value

                    modifyDqPositions(sizeDq-k+1);
                    environmentTable.insert(environmentTable.begin()+(getPosition+j),environmentVars);


                  }


                //the whole block not been declared before
                else
                {

                    int pos=sizeDq-k;
                    int getPosition=dequePtr.at(pos).position;

                    int j=0;
                    while(getPosition+j<environmentTable.size()&&
                          blockData>environmentTable.at(getPosition+j).block)
                    {
                        ++j;
                    }
                    ptr.block=blockData;
                    //j gets the value when blockData is smaller
                    //since counting starts from 0 so do -1
                    ptr.position=getPosition+j-1;

                    dequePtr.insert(dequePtr.begin()+(sizeDq-(k-1)),ptr);

                    //j because at j is when it either reached the end or
                    //a bigger block value
                    modifyDqPositions(sizeDq-k+1);
                    environmentTable.insert(environmentTable.begin()+(getPosition+j),environmentVars);



                }

            }
}

//int i is the position of T_ID starting from 0,1... in the trackSemiColon vector
//this lookup is used for initilialisation
bool parser::lookUp(int i)
{

    if(dequePtr.size()==0)
    {
        return false;
    }
    int sizeDq=dequePtr.size();
    int k=1;//just a loop variable

    //loop checks if the block has ever been declared before by checking
    //the "pointers"


    int parentBlock;

    //the parent starts from the current block
    if(trackSemiColon.at(i).tokens==scannar::T_ID)
    {
        bool inWhile=false;

        if(sizeDq!=0)
        {
            for(parentBlock=parent.size()-1;parentBlock>=0;parentBlock--)
            {
                k=1;

                while(k<=sizeDq&&dequePtr.at(sizeDq-k).block!=parent.at(parentBlock))
                {
                    inWhile=true;
                     ++k;
                }

                if(k<=sizeDq&&k>0)
                {
                   int getPos= dequePtr.at(sizeDq-k).position;

                    while(getPos<environmentTable.size()&&
                          environmentTable.at(getPos).block==parent.at(parentBlock))
                    {

                        if(environmentTable.at(getPos).variable.
                                compare(trackSemiColon.at(i).variable)==0)
                        {
                            //for 'a' in a=a+b;--> is an example only
                            if(i==0)
                            {
                                idType=environmentTable.at(getPos).tokens;
                                idPos=getPos;
                                checked=true;
                                return true;
                            }
                            else
                            {

                                //checks if the variable has been initialised before.
                                if(environmentTable.at(getPos).init)
                                {
                                assignId=environmentTable.at(getPos).tokens;

                                //IF IDtYPE INIT
                                if(countLoopVar==1)
                                {
                                    //most likely comes from checkLoop
                                  idType=assignId;
                                  return true;
                                }
                                  return checkType(assignId,idType);
                                }
                                else
                                {
                                //    cout<<false;
                                    return false;
                                }
                            }

                        }
                        ++getPos;
                    }
                }
            }
        }
    }
    else
    {

        bool checked= checkType(idType,trackSemiColon.at(i).tokens);
        if(checked)
        {
            assignId=trackSemiColon.at(i).tokens;
        }
        return checked;
    }

    return false;
}

//i starts from 0..n and is the index of the vector
//to be used mainly after cheking everything to set the initialisation to true
int parser::findPosition(int i)
{
    int j=0;// j is a looping variable only
    //j just finds which index number a particular block is at

    while(dequePtr.at(j).block!=blockData)
    {

        ++j;
    }

    int getPos= dequePtr.at(j).position;
    while(getPos<environmentTable.size()&&
          environmentTable.at(getPos).block==blockData)
    {
        if(environmentTable.at(getPos).variable.compare(trackSemiColon.at(i).variable)==0)
        {
            return getPos;
        }
        ++getPos;
    }

    //some error occured
    return -1;
}
int parser::returnEnd(string loopType)
{
    if(!prevStatement.empty())
    {
        string loop=prevStatement.at(prevStatement.size()-1);

        if(loop.compare(loopType)!=0)
        {
            if(loop.compare("while")==0)
            {
                //end while not found
                return 39;
            }
            else if(loop.compare("if")==0||loop.compare("else")==0)
            {
                //end_if not found
                return 41;
            }
            else
            {
                //return  end_for not found
                return 40;
            }
        }
        return 0;
    }
    //over here the prevStatement will be empty
    else if(loopType.compare("end")==0)
    {
        return 0;
    }
    //here it is an extra end statement
    else
    {
        return 13;
    }
    //no problem
    return 0;
}

//i is base 0
//is the position value
//starts from after openbracket the value of i
int parser::getLoopVariables(int i)
{

    //clear old data first
    trackSemiColon.clear();
    checked=false;
    loop=true;
    temp=i;
    int brackets=1;
    //get upto but not including the ')'

    while(i<parseTokenizer.size()&&brackets!=0)
    {

        //these if statements take into account multiple open and close brackets
        //i!=1 is just that the first open bracket should be ignored for the logic to work
        if(parseTokenizer.at(i).tokens==scannar::T_LBR&&i!=temp+1)
        {
            //++brackets;
            errorLine=parseTokenizer.at(temp).lineNum;
            error=22;
        }
        else if(parseTokenizer.at(i).tokens==scannar::T_RBR)
        {

            --brackets;

        }
        //ensures that the brackets are not included
        //for the sake of parsing --> to easen parsing
         else
        {
            //this is for the first openbracket to be excluded
            if(parseTokenizer.at(i).tokens!=scannar::T_LBR)
            {

            trackSemiColon.push_back(parseTokenizer.at(i));
            }
        }
        ++i;
    }

    if(brackets>0)
    {

        errorLine=parseTokenizer.at(temp).lineNum;
        //missing closeBracket
        error=38;
    }
    if(brackets<0)
    {
        errorLine=parseTokenizer.at(temp).lineNum;
        //extra close brackets
        error=43;

    }
    if(i>parseTokenizer.size()-1)
    {
        errorLine=parseTokenizer.at(temp).lineNum;
        //missing closeBracket
        error=38;
    }
    else if(trackSemiColon.size()>3)
    {
    parseStatement();
    }
    else
    {

        errorLine=parseTokenizer.at(temp).lineNum;
        //invalid command receieved
        error= 30;
    }
    printTrackSemiColon();

    if(i>=parseTokenizer.size())
    {

        i=1;
    }
    return i;
}
//i starts from base 0
//i will be 0;
int parser::checkLoop(int i)
{

        //pos is 2 cos 0 is the while or if part and 1 is '('
        int pos=i+1;
        int count=1;
        bool id=false;
        bool comparisonOP=false;
        int negNum=0;
        //just to keep track of the first variable so that
        //the first variable can be assigned
        countLoopVar=1;
        while(pos<trackSemiColon.size())
        {

            //goes in only if it is not starting with a T_ID or a literal
            //or starts with a negation
            if(count==1&&trackSemiColon.at(pos).tokens!=scannar::T_CHARLit
                    &&trackSemiColon.at(pos).tokens!=scannar::T_STRLit
                    &&trackSemiColon.at(pos).tokens!=scannar::T_DECLit
                    &&trackSemiColon.at(pos).tokens!=scannar::T_INTLit
                    &&trackSemiColon.at(pos).tokens!=scannar::T_ID
                    &&trackSemiColon.at(pos).variable.compare("-")!=0)
            {

                //invalid command received
               return 30;
            }
            //if being declared or initialised
            if(!id)
            {
                if(trackSemiColon.at(pos).tokens==scannar::T_ID)
                {

                        if(countLoopVar==1)
                        {
                            idType=assignId;

                        }
                        if(!lookUp(pos))
                        {
                            return 35;
                        }

                        id=true;



                }
                //literals
                else if(trackSemiColon.at(pos).tokens==scannar::T_CHARLit
                        ||trackSemiColon.at(pos).tokens==scannar::T_STRLit
                        ||trackSemiColon.at(pos).tokens==scannar::T_DECLit
                        ||trackSemiColon.at(pos).tokens==scannar::T_INTLit)
                {
                    id=true;
                    if(countLoopVar==1)
                    {
                        if(trackSemiColon.at(pos).tokens==scannar::T_CHARLit)
                        {
                            if(trackSemiColon.at(pos).variable.length()>3)
                            {
                                return 37;
                            }

                            idType=scannar::T_CHAR;
                        }
                        else if(trackSemiColon.at(pos).tokens==scannar::T_STRLit)
                        {
                            idType=scannar::T_STR;
                        }
                        else if(trackSemiColon.at(pos).tokens==scannar::T_INTLit)
                        {
                            idType=scannar::T_INT;
                        }
                        //IF t_dec
                        else
                        {
                            idType=scannar::T_DEC;
                        }
                    }
                    else
                    {
                        if(!checkType(trackSemiColon.at(pos).tokens,idType))
                        {
                            //return expected type and actual type is different
                            return 25;
                        }
                        if(trackSemiColon.at(pos).tokens==scannar::T_CHARLit)
                        {
                            if(trackSemiColon.at(pos).variable.length()>3)
                            {
                                return 37;
                            }
                        }
                      //  cout<<"in if  else"<<endl;
                    }
                }
                else if(trackSemiColon.at(pos).variable.compare("-")==0)
                {
                        negNum=1;
                        //after a negation it must be a decimal, int or an id type
                        if( trackSemiColon.at(pos+1).tokens!=scannar::T_DECLit
                                &&trackSemiColon.at(pos+1).tokens!=scannar::T_INTLit
                                &&trackSemiColon.at(pos+1).tokens!=scannar::T_ID)
                        {
                            //returns expected variable and actual type differs
                            return 25;
                        }
                        if(countLoopVar==1)
                        {
                            //negation can only be used with numerical type
                            //T_dec chosen because it is a broader version
                            //it accepts both ints and decimal values
                            idType=scannar::T_DEC;
                        }
                        else
                        {
                            if(!checkType(scannar::T_INT,idType))
                            {
                                //if the initial type is not of numeral type then error
                                return 27;
                            }
                        }
                        id=false;

                }
                else
                {
                    //invalid command received
                    return 30;
                }
                ++countLoopVar;
                ++count;
            }
            //if it is an operator
            //id is true here
            else
            {


                id= false;
                if(trackSemiColon.at(pos).tokens==scannar::T_OP)
                {

                    if(trackSemiColon.at(pos).variable.compare("+")==0||
                            trackSemiColon.at(pos).variable.compare("-")==0||
                            trackSemiColon.at(pos).variable.compare("*")==0||
                            trackSemiColon.at(pos).variable.compare("/")==0||
                            trackSemiColon.at(pos).variable.compare("/-")==0)


                    {
                         if(idType!=scannar::T_DEC&&idType!=scannar::T_INT)
                         {

                            //invalid operator used
                            return 27;

                         }

                    }
                     if(trackSemiColon.at(pos).variable.compare("!")==0)
                     {

                        //invalid operator used
                        return 27;
                     }

                }
                else if(pos+1<trackSemiColon.size()&&
                        trackSemiColon.at(pos).variable.compare("=")==0&&
                      trackSemiColon.at(pos+1).variable.compare("=")!=0  )
                {
                    //invalid operator

                    return 27;
                }
                //equals equals symbol
                else if(pos+1<trackSemiColon.size()&&
                        trackSemiColon.at(pos).tokens==scannar::EQUALS&&
                        trackSemiColon.at(pos+1).tokens==scannar::EQUALS
                        &&!comparisonOP)
                {

                    comparisonOP=true;
                    ++pos;
                }
                //if && or ||
                else if((countLoopVar+negNum)%2!=0&&trackSemiColon.at(pos).tokens==scannar::T_DOP)
                {
                    //invalid use of || or &&
                    return 42;
                }
                //resetting countLoopVar cos idType can change now
                //type && and ||
                else if((countLoopVar+negNum)%2==0&&trackSemiColon.at(pos).tokens==scannar::T_DOP&&countLoopVar>3)
                {
                    //resets count to 1 later when it does count++;
                    countLoopVar=0;
                    count=0;
                    comparisonOP=false;
                    negNum=0;
                }
                //if it is a COP -->comparision operator
                else
                {
                    //countLoopVar incremented an extra time
                    //this helps with the logic for checking if comparison is made

                    if(trackSemiColon.at(pos).tokens==scannar::T_COP&&(countLoopVar+negNum)%2==0
                            &&!comparisonOP)
                    {

                        if(idType==scannar::T_CHAR||idType==scannar::T_CHARLit
                                ||idType==scannar::T_STR||idType==scannar::T_STRLit)
                        {
                            //invalid operator on string type and char type
                            return 48;
                        }
                        comparisonOP=true;
                    }
                    else
                    {

                        //invalid command received
                        return 30;
                    }


                }
                ++count;
                ++countLoopVar;

            }

        ++pos;
        }
        //needs to end with a literal or an ID type
        if(!id)
        {
            //invalid command received
            return 44;
        }


    //pass
    return 1;
}

//i starts from base 0
//passes in the position of for loop
int parser::forLoop(int i)
{
    tempVector.clear();
    charOrStr=false;

    for(unsigned int i=0;i<trackSemiColon.size();i++)
    {
        tempVector.push_back(trackSemiColon.at(i));
    }

    //clear old data first
    trackSemiColon.clear();
    checked=false;
    temp=i;
    int count=0;
    //cos I want to ignore i=0 which is the keyword 'for'
    i=1;
    //get upto but not including the ';'

    while(count<3)
    {

        trackSemiColon.clear();
        //cos no semiolon delimiting the last part
        if(count==0||count==1)
        {

            while(i<tempVector.size()&&tempVector.at(i).tokens!=scannar::SEMICOLON)
            {
                trackSemiColon.push_back(tempVector.at(i));

                ++i;
            }

            //if semicolon not found
            if(i>=tempVector.size())
            {
                errorLine=tempVector.at(0).lineNum;

                //missing semicolon
                error= 29;
                return error;
            }

            ++i;
        }
        else
        {

            if(i>=tempVector.size())
            {
                error=47;
                errorLine=tempVector.at(0).lineNum;
                return error;
            }

            while(i<tempVector.size())
            {

                if(tempVector.at(i).tokens==scannar::SEMICOLON)
                {

                    errorLine=tempVector.at(0).lineNum;
                    error=46;
                    return error;
                }

                trackSemiColon.push_back(tempVector.at(i));
                ++i;

            }

        }

        if(trackSemiColon.size()!=0&&error==0)
        {
            if(count==0||count==2)
            {
                loop=false;
                int check= parseStatement();
                //if an error is given out
                if(check!=0)
                {
                    return check;
                }
            }
            else
            {
                loop=true;
                int check=checkLoop(-1);
                //if an error is given out
                if(check!=1)
                {
                    errorLine=tempVector.at(i).lineNum;
                    return check;
                }
            }

        }
        else
        {

            //comes in here if the vector is empty
            if(error==0)
            {

                errorLine=tempVector.at(i).lineNum;
                error= 27;
                return error;
            }
        }
        //if the variable is a char or a str
        if(charOrStr)
        {

            errorLine=tempVector.at(i).lineNum;
            //invalid character
            error= 25;
            return error;
        }

        ++count;
    }
    //if there's still more
    //shouldn't come here
    if(i<tempVector.size()-1)
    {
        errorLine=tempVector.at(i).lineNum;
        //invalid for loop variables
        error= 45;
        return error;
    }
//pass
    return 1;
}
