/************************************************
 * Author: Sheena V
 * Date: 22/01/2015
 * Description : Scanner Class for the Language
 * Module: Scannar.cpp
 * Functionalities:
 * 1)Takes the input string
 * 2)Appends and empty space to the back of the string
 * 3)Ensures that the vector is empty before processing more information
 * 4)Splits the strings. Ignores all spaces.
 * 5)Delimiters are stored seperately.
 * 6)Appends tokens
 * 7)Checks if && and || are together
 * 8)Checks if backslash is supported by the program
 * 9)Ignores all comments
 *
 * **********************************************/
#include "scannar.h"
#include "error.h"
#include "parser.h"
#include "codegenerator.h"

//default constructor
scannar::scannar()
{
    cout << "in default scannar" << endl;
}

//constructor called from GUI
scannar::scannar(string str):
    lineNumber(1),
    found(false),
    numOfOp(0)

{

    cout << str.c_str();
    srcCode = str;
    cout <<"hello i'm through"<<endl;
    initialiseMap();

}

string scannar::getReturn()
{
    return srcCode;
}

//function only called when run button pressed
int scannar::splitStr()
{       deallocateVector();
        //need to add this otherwise the last value will be cut of.
        //Space used because space will be ignored
        srcCode+=" \n";
        //position
        size_t pos=0;
        size_t range=1;
        bool push=false;
        //numerals,alphabets and _
        string regex="abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ_.";
       // string regexquotes="\"'";
        //filtering all the non aplhabetical and numerical symbols
        size_t delimiter = srcCode.find_first_not_of(regex);
        size_t oldPos=0;

      while (delimiter!=std::string::npos)
      {
          size_t  length=delimiter-pos;
          //if not string,char or comment
          if(srcCode.substr(delimiter,1).compare("\"")!=0
                  &&srcCode.substr(delimiter,1).compare("'")!=0
                  &&srcCode.substr(delimiter,2).compare("//")!=0)
          {
          cout<<"length is "<<length<<endl;

         //length>0 because when then :: problems get eliminated
          if(length>0)
          {
             codeFragments.push_back(srcCode.substr(pos,length));
             cout<<"substr is  "<<srcCode.substr(pos,length).c_str()<<endl;
          }
       // codeFragments.push_back(srcCode.substr(pos,length));
       // codeFragments.push_back(srcCode.substr(delimiter,1));
          //gets rid of space
           if(srcCode.substr(delimiter,1).compare(" ")!=0)
           {
                codeFragments.push_back(srcCode.substr(delimiter,1));

                 cout<<"substr +1 is "<<srcCode.substr(delimiter,1).c_str()<<endl;

            }
           range=1;
        }
          else{
               bool comment=false;
                found=false;
                int tempDelim;
                int count=0;
               size_t findQuotes=0;
            //  cout<<"delimiter is quotes";
              cout<<"quote in "<<endl;
              //if string
              if(srcCode.substr(delimiter,1).compare("\"")==0)
              {
               findQuotes= srcCode.find_first_of("\"",delimiter+1);
               /* while(!found)
                {
                    cout<<"quote in while scannar "<<endl;
                    findQuotes= srcCode.find_first_of("\"",delimiter+1);
                    cout<<"findQ is "<<findQuotes<<endl;
                    cout<<"srcCodeLen= "<<srcCode.length()<<endl;
                    if(findQuotes>srcCode.length())
                    {

                       findQuotes=string::npos;
                       break;
                    }
                    if(srcCode.substr(findQuotes-1,1).compare("\\")==0)
                    {
                        cout<<"delimited quotes"<<srcCode.substr(findQuotes-1,1).c_str()<<endl;
                        if(count==0)
                        {
                            tempDelim=delimiter;
                            count++;
                        }
                        delimiter=findQuotes;
                    }
                    else
                    {   cout<<"quote in while scannar else "<<endl;
                        delimiter=tempDelim;
                       // delimiter=findQuotes;
                        found=true;
                    }
                }*/
              }
              //if comment
              else if(srcCode.substr(delimiter,1).compare("/")==0){
                  cout<<"in / else"<<endl;
                  if(srcCode.substr(delimiter+1,1).compare("/")==0)
                  {
                      //test
                      if(length>0)
                      {
                         codeFragments.push_back(srcCode.substr(pos,length));
                         //cout<<"substr is  "<<srcCode.substr(pos,length).c_str()<<endl;
                      }
                    findQuotes= srcCode.find_first_of("\n",delimiter+2);
                    cout<<"in if";
                    comment=true;
                  }
                 /* else{
                    //division
                      findQuotes=delimiter;
                      //return 8;
                  }*/
              }
              else
              {
                  findQuotes= srcCode.find_first_of("'",delimiter+1);
              }

               if(findQuotes==string::npos&&comment==false)
               {
                   cout<<"error cannot find closing \" "<<endl;
                   range=delimiter;
                   return 6;
               }
               else
               {
                  length=findQuotes-delimiter;
                  range=length+1;
                  if(!comment)
                  {
                      codeFragments.push_back(srcCode.substr(delimiter,length+1));
                       cout<<"substr is in quotes "<<srcCode.substr(delimiter,length+1).c_str()<<endl;
                  }
                   else
                  {
                    ++lineNumber;
                     codeFragments.push_back("\n");
                  }
             }
             push=true;


          }
        cout<<delimiter<<endl;
        pos=delimiter+range;
        //used only for comments
        oldPos=delimiter;
        delimiter=srcCode.find_first_not_of(regex,delimiter+range);

      }
    //  cout<<"count id "<<count<<endl;
      cout<<"vector codeFragments size = "<<codeFragments.size()<<endl;
     // cout<<srcCode.c_str()<<endl;
          int i=0;
          for(i=0;i<codeFragments.size();i++){

                cout<<"push back for after splitting = "<<codeFragments.at(i)<<endl;
                if(codeFragments.at(i).compare("\n")==0){
                    cout<<"hello"<<endl;
                }
            }
     int msg= appendToken();
return msg;

}
void scannar::deallocateVector(){
    codeFragments.clear();
    lineNumber=1;
    tokenize.clear();
}

int scannar::appendToken(){
    toks=new Tokenizer;
    lineNumber=1;
    //debugging only
   for(int i=0;i<codeFragments.size();i++)
    {
        cout<<"@@@@@in scannar: code fragments are "<<codeFragments.at(i)<<endl;
    }
   unsigned int i=0;
   while( i<codeFragments.size()){

        data=codeFragments.at(i);
        //debugging only
        if(data.compare("\t")==0)
        {
            cout<<"----------- hi there i am a tab ------"<<endl;
        }
        if(data.compare("\n")!=0&&data.compare("\t")!=0&&data.compare(" ")!=0)
        {
          tempStorage=data;
          transform(data.begin(), data.end(), data.begin(), ::tolower);
          cout<<"through if not newline"<<" data is = "<<data<<endl;
          cout<<data<<endl;
            switch (mapTokens[data]){
                case IF:
                        numOfOp=0;
                        toks->tokens=IF;
                        toks->variable="if";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in if "<<lineNumber<<endl;
                        break;

                case END_IF:
                        numOfOp=0;
                        toks->tokens=END_IF;
                        toks->variable="end_if";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in end if "<<lineNumber<<endl;
                        break;

                case FOR:
                        numOfOp=0;
                        toks->tokens=FOR;
                        toks->variable="for";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in for "<<lineNumber<<endl;
                        break;

                case END_FOR:
                        numOfOp=0;
                        toks->tokens=END_FOR;
                        toks->variable="end_for";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in end for "<<lineNumber<<endl;
                        break;

                case WHILE:
                        numOfOp=0;
                        toks->tokens=WHILE;
                        toks->variable="while";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in while "<<lineNumber<<endl;
                        break;

                case END_WHILE:
                        numOfOp=0;
                        toks->tokens=END_WHILE;
                        toks->variable="end_while";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in end while "<<lineNumber<<endl;
                        break;

                case BEGIN:
                        numOfOp=0;
                        toks->tokens=BEGIN;
                        toks->variable="begin";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in begin "<<lineNumber<<endl;
                        break;

                case END:
                        numOfOp=0;
                        toks->tokens=END;
                        toks->variable="end";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in end "<<lineNumber<<endl;
                        break;

                case READ:
                        numOfOp=0;
                        toks->tokens=READ;
                        toks->variable="read";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in read "<<lineNumber<<endl;
                        break;

                case PRINT:
                        numOfOp=0;
                        toks->tokens=PRINT;
                        toks->variable="print";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in print "<<lineNumber<<endl;
                        break;
                case T_LSB:
                        numOfOp=0;
                        toks->tokens=T_LSB;
                        toks->variable="[";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in [ "<<lineNumber<<endl;
                        break;

                case T_RSB:
                        numOfOp=0;
                        toks->tokens=T_RSB;
                        toks->variable="]";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in ] "<<lineNumber<<endl;
                        break;

                case T_RCB:
                        numOfOp=0;
                        toks->tokens=T_RCB;
                        toks->variable="}";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in } "<<lineNumber<<endl;
                        break;

                case T_LCB:
                        numOfOp=0;
                        toks->tokens=T_LCB;
                        toks->variable="{";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in  { "<<lineNumber<<endl;
                        break;

                case T_LBR:
                        numOfOp=0;
                        toks->tokens=T_LBR;
                        toks->variable="(";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in ( "<<lineNumber<<endl;
                        break;

                case T_RBR:
                        numOfOp=0;
                        toks->tokens=T_RBR;
                        toks->variable=")";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in ) "<<lineNumber<<endl;
                        break;
                case ELSE:
                        numOfOp=0;
                        toks->tokens=ELSE;
                        toks->variable="";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in else "<<lineNumber<<endl;
                        break;
                case EQUALS:
                        numOfOp=0;
                        toks->tokens=EQUALS;
                        toks->variable="=";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in equals "<<lineNumber<<endl;
                        break;

                case SEMICOLON:
                        numOfOp=0;
                        toks->tokens=SEMICOLON;
                        toks->variable="";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in semicolon "<<lineNumber<<endl;
                        break;

                case T_DEC:
                        numOfOp=0;
                        toks->tokens=T_DEC;
                        toks->variable="decimal";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in decimal "<<lineNumber<<endl;
                        break;

                case T_INT:
                        numOfOp=0;
                        cout<<data.c_str()<<endl;
                        toks->tokens=T_INT;
                        toks->variable="int";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in int "<<lineNumber<<endl;
                        break;


                case T_CHAR:
                        numOfOp=0;
                        toks->tokens=T_CHAR;
                        toks->variable="char";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in char "<<lineNumber<<endl;
                        break;
                case T_STR:
                        numOfOp=0;
                        toks->tokens=T_STR;
                        toks->variable="string";
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in str "<<lineNumber<<endl;
                        break;
                case T_OP:
                        ++numOfOp;
                        cout<<"in T_OP numOfOP= "<<numOfOp<<endl;
                        if(numOfOp>=3)
                        {
                            return 8;
                        }
                        character=codeFragments.at(i+1);
                        if(character.compare("=")==0&&codeFragments.at(i).compare("!")==0)
                        {
                            toks->variable=data+character;
                            ++i;
                        }
                        //b--c==b+c
                        else if(character.compare("-")==0&&codeFragments.at(i).compare("-")==0)
                        {
                            toks->variable="+";
                            ++i;
                        }
                        //b+-c==b-c
                        //b-+c==b-c
                        else if(character.compare("-")==0&&codeFragments.at(i).compare("+")==0||
                                character.compare("+")==0&&codeFragments.at(i).compare("-")==0)
                        {
                            toks->variable="-";
                            ++i;
                        }
                        //anything other than a negation after another operator is a false
                        else if(tokenize.at(tokenize.size()-1).tokens==T_OP&&codeFragments.at(i).compare("-")!=0)
                        {
                            //invalid character encountered
                            return 8;
                        }
                       /* else if(tokenize.at(tokenize.size()-1).tokens==T_OP&&codeFragments.at(i).compare("-")==0)
                        {
                            toks->variable=data;
                            ++i;
                        }*/
                        else
                        {
                            cout<<"in else"<<endl;
                        toks->variable=data;
                        }
                        toks->tokens=T_OP;

                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in operator "<<lineNumber<<endl;
                        break;
                case T_COP:
                        character=codeFragments.at(i+1);
                        toks->tokens=T_COP;
                        if(character.compare("=")==0)
                        {
                            toks->variable=data+character;
                            ++i;
                        }
                        else
                        {
                        toks->variable=data;
                        }
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in operator "<<lineNumber<<endl;
                        break;
                case T_FSLASH:
                         numOfOp=0;
                        character=codeFragments.at(i+1);
                        if(character.length()>1)
                        {
                            return 9;
                        }

                        if(character.compare("n")!=0&&character.compare("t")!=0
                                &&character.compare("\"")!=0&&character.compare("'")!=0)
                        {
                           cout<<"in character compare foward slash "<<character<<endl;
                            return 9;
                        }

                        toks->tokens=T_FSLASH;
                        toks->variable=data+codeFragments.at(i+1);
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in FSLASH \\"<<lineNumber<<endl;
                        ++i;
                        break;
                case T_BSLASH:
                //division
            {
                ++numOfOp;
                cout<<"in T_OP numOfOP= "<<numOfOp<<endl;
                if(numOfOp>=3)
                {
                    return 8;
                }
                character=codeFragments.at(i+1);


                 if(character.compare("-")==0)
                {
                    toks->variable="/-";
                    ++i;
                }
                else if(tokenize.at(tokenize.size()-1).tokens==T_OP&&codeFragments.at(i).compare("-")!=0)
                {
                    //invalid character encountered
                    return 8;
                }
               /* else if(tokenize.at(tokenize.size()-1).tokens==T_OP&&codeFragments.at(i).compare("-")==0)
                {
                    toks->variable=data;
                    ++i;
                }*/
                else
                {
                    cout<<"in else"<<endl;
                toks->variable=data;
                }
                toks->tokens=T_OP;

                toks->lineNum=lineNumber;
                tokenize.push_back(*toks);
                cout<<"in operator "<<lineNumber<<endl;
                break;
            }
                       //return 8;

                case T_DOP:
                        numOfOp=0;
                        character=codeFragments.at(i+1);
                        if(character.length()>1){
                            return 8;
                        }
                        if(character.compare("|")!=0&&character.compare("&")!=0){
                            return 8;
                        }
                        toks->tokens=T_DOP;
                        toks->variable=data+character;
                        toks->lineNum=lineNumber;
                        tokenize.push_back(*toks);
                        cout<<"in double operator "<<lineNumber<<endl;
                        ++i;
                        break;

               default:
                    numOfOp=0;
                     size_t start=data.length()-1;

                     if(data.substr(0,1).compare("\"")==0){
                         toks->tokens=T_STRLit;
                         toks->variable=tempStorage;
                         toks->lineNum=lineNumber;
                         tokenize.push_back(*toks);
                         cout<<"tempStorage= "<<tempStorage<<endl;
                         cout<<"in \" of default "<<lineNumber<<endl;

                     }
                     else if(data.substr(0,1).compare("'")==0)
                     {
                         toks->tokens=T_CHARLit;
                         toks->variable=tempStorage;
                         toks->lineNum=lineNumber;
                         tokenize.push_back(*toks);
                         cout<<"tempStorage= "<<tempStorage<<endl;
                         cout<<"in CHARlIT "<<lineNumber<<endl;
                        }
                     else if(isInt(data)==1)
                     {
                         toks->tokens=T_INTLit;
                         toks->variable=data;
                         toks->lineNum=lineNumber;
                         tokenize.push_back(*toks);
                         cout<<"in INTlIT "<<lineNumber<<endl;
                     }

                     else if(isDecimal(data)==1)
                       //      data.find_first_of(".")!=string::npos))
                     //else if(dec)
                     {
                         toks->tokens=T_DECLit;
                         toks->variable=data;
                         toks->lineNum=lineNumber;
                         tokenize.push_back(*toks);
                         cout<<"in DEClit "<<lineNumber<<endl;
                     }

                     else if(data.substr(0,1).compare("_")==0||data.substr(start,1).compare("_")==0
                             ||data.find_first_of(".")!=string::npos)
                     {
                         //code for invalid identifier name
                         return 7;
                     }
                     else if(data.compare("@")==0||data.compare("#")==0||data.compare("$")==0
                             ||data.compare("%")==0||data.compare("^")==0||data.compare("~")==0||
                             data.compare("`")==0||data.compare(":")==0||data.compare(",")==0||
                             data.compare("?")==0){
                         //code for invalid character
                         cout<<"invalid data is= "<<data<<endl;
                         return 8;
                     }
                     //identifiers
                     else
                     {
                         cout<<"b4 checking type id "<<data <<endl;
                         if(isTypeId(data)==1)
                         {

                         toks->tokens=T_ID;
                         toks->variable=tempStorage;
                         toks->lineNum=lineNumber;
                         tokenize.push_back(*toks);
                          cout<<"tempStorage= "<<tempStorage<<endl;
                         cout<<"in else its an ID "<<lineNumber<<endl;
                         }
                         else
                         {
                             //invalid identifier name
                             return 7;
                         }

                     }

                     break;
            }//end switch
        }
        else{
            ++lineNumber;
            cout<<"line number incremented to "<<lineNumber<<endl;
            }

        ++i;

    }
   print(tokenize);
   return 0;
}

void scannar::print(vector<Tokenizer> toPrint){
    cout<<"Tokenize---------------------------------\n\n"<<endl;
    for(unsigned int i=0;i<toPrint.size();i++){

          cout<<"push back = "<<toPrint.at(i).tokens<<endl;
          if(toPrint.at(i).tokens==BEGIN){
              cout<<"its begin"<<endl;
          }
          if(toPrint.at(i).tokens==END){
              cout<<"its end"<<endl;
          }
          if(toPrint.at(i).tokens==T_STRLit){
              cout<<"its a strLit"<<endl;
          }
          if(toPrint.at(i).tokens==T_ID){
              cout<<"identifier"<<endl;
          }
          if(toPrint.at(i).tokens==T_INT){
              cout<<"integer annoyingly enough :("<<endl;
          }
          if(toPrint.at(i).tokens==T_FSLASH){
              cout<<"its fslash"<<endl;
              cout<<toPrint.at(i).variable<<endl;
          }


      }
}

void scannar::initialiseMap(){
    mapTokens["if"]=IF;
    mapTokens["while"]=WHILE;
    mapTokens["end_if"]=END_IF;
    mapTokens["end_while"]=END_WHILE;
    mapTokens["for"]=FOR;
    mapTokens["end_for"]=END_FOR;
    mapTokens["begin"]=BEGIN;
    mapTokens["end"]=END;
    mapTokens["readfromscreen"]=READ;
    mapTokens["printtoscreen"]=PRINT;
    mapTokens["="]=EQUALS;
    mapTokens[";"]=SEMICOLON;
    mapTokens["else"]=ELSE;
    mapTokens[")"]=T_RBR;
    mapTokens["("]=T_LBR;
    mapTokens["{"]=T_LCB;
    mapTokens["}"]=BEGIN;
    mapTokens["["]=T_LSB;
    mapTokens["]"]=T_RSB;
    mapTokens["decimal"]=T_DEC;
    mapTokens["int"]=T_INT;
    mapTokens["sentence"]=T_STR;
    mapTokens["char"]=T_CHAR;
    mapTokens["-"]=T_OP;
    mapTokens["+"]=T_OP;
    mapTokens["*"]=T_OP;
    mapTokens["/"]=T_OP;
    mapTokens["!"]=T_OP;
    mapTokens[">"]=T_COP;
    mapTokens["<"]=T_COP;
    mapTokens["&"]=T_DOP;
    mapTokens["\\"]=T_FSLASH;
    mapTokens["|"]=T_DOP;
    mapTokens["/"]=T_BSLASH;



}

string scannar::returnToGUI(){
    err=new error();

   int getMsg= splitStr();
   if(getMsg==0&&tokenize.size()>0){
      parse= new parser();
      getMsg= parse->startParsing(tokenize);
   }
   // int getMsg=1;
   return consoleMessage(getMsg);

}
string scannar::consoleMessage(int getMsg)
{
    convertIntToStr();

    switch(getMsg){
        case 0:
               if(tokenize.size()>0){
               // parse= new parser();
              // getMsg= parse->startParsing(tokenize);
               }
                return "It works fine ftm...Built";
                break;
        case 1:
                return (err->scannarError()+lineNo);
       // return "a";

        case 2:
                return (err->unknownType()+lineNo);
        // return "2";

        case 3:
                return (err->invalidType()+lineNo);
             //return "3";

        case 4:
                return (err->uninitialisedVar()+lineNo);
            //return "4";

        case 5:
                return (err->invalidInitialisation()+lineNo);
           // return "5";

        case 6:
        cout<<"lineNumber= "<<lineNo<<endl;
               return (err->closeQuotesNotFound()+lineNo);
            //return "6";
        case 7:
               return (err->invalidIdentifierName()+lineNo);
        case 8:
               return (err->invalidCharacter()+lineNo);
        case 9:
                return (err->invalidEscapeSeq()+lineNo);
        case 10:
                return (err->missingBegin()+"1");
        case 11:

                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->multipleBegin()+lineNo);
        case 12:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidAfterEnd()+lineNo);
        case 13:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->endNotFound()+lineNo);
        case 14:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidElse()+lineNo);
        case 15:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->extraEndFor()+lineNo);
        case 16:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->extraEndWhile()+lineNo);
        case 17:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->extraEndIf()+lineNo);
        case 18:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->EOFError()+lineNo);
        case 19:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->incorrectEnd()+lineNo);
        case 20:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->extraBracket()+lineNo);
        case 21:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->openBracketNotFound()+lineNo);
        case 22:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidOpenBracket()+lineNo);
        case 23:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->unidentifiedBrackets()+lineNo);
        case 24:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->identifierNotFound()+lineNo);
        case 25:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidAssignment()+lineNo);
        case 26:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->multipleDeclaration()+lineNo);
        case 27:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidOperator()+lineNo);
        case 28:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidOperatorForString()+lineNo);
        case 29:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->missingSemiColon()+lineNo);

        case 30:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidCommand()+lineNo);
        case 31:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->uselessCommand()+lineNo);
        case 32:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidRead()+lineNo);
        case 33:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->multipleEnd()+lineNo);
        case 34:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->missingEquals()+lineNo);
        case 35:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->uninitAndUndeclaredVars()+lineNo);
        case 36:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidCharOp()+lineNo);
        case 37:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidParserChar()+lineNo);
        case 38:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->closeBracketErr()+lineNo);
        case 39:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->endWhile()+lineNo);
        case 40:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->endFor()+lineNo);
        case 41:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->endIf()+lineNo);
        case 42:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidLogicalOp()+lineNo);
        case 43:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->extraCloseBrackets()+lineNo);
        case 44:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->endLitOrVar()+lineNo);
        case 45:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidLoopVar()+lineNo);
        case 46:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidSemicolon()+lineNo);
        case 47:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->incrementNotFound()+lineNo);
        case 48:
                lineNumber=parse->getLineNumber();
                convertIntToStr();
                return (err->invalidCOPUsage()+lineNo);

        default :
                convertIntToStr();
                return "An error occured at "+lineNo;
     }
}

void scannar::clearStream(){
    convert.str(string());
    convert.clear();
}
void scannar::convertIntToStr()
{
    clearStream();
    convert<<lineNumber;
    lineNo=convert.str();
}

//can be 2.-3 which just translates to 2.0-3
int scannar::isDecimal(string data)
{


        size_t point=data.find_first_of(".");
        string regex="0123456789";
        cout <<"scannar data is "<<data;
        //check for decimal
        if(point!=string::npos)
        {
            cout<<"scannar point!npos isdec"<<endl;
            size_t numerator = data.substr(0,point).find_first_not_of(regex);
            size_t denominator= data.substr(point+1).find_first_not_of(regex);

            cout<<"numerator= "<<numerator<<endl;
            cout<<"denom= "<<denominator<<endl;

           if(numerator!=string::npos||denominator!=string::npos)
           {
               cout<<"not npos isDec"<<endl;
               return 2;
           }


        }
        else
        {
            //decimal type can support integers
            if(isInt(data)==1)
            {
                return 1;
            }
            return 2;
        }


    return 1;
}
int scannar::isInt(string data)
{

    string regex="0123456789";
    size_t number=data.find_first_not_of(regex);
    cout<<"number is "<<number<<endl;
    if(number!=string::npos)
    {
        return 2;
    }

    return 1;

}

int scannar::isTypeId(string data)
{
    cout<<"isType id data is "<<data.c_str()<<"walooop"<<endl;
    string regex ="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";
    size_t number= data.find_first_not_of(regex);
    if(number!=string::npos)
    {
       return 2;
    }
    string firstAlp=data.substr(0,1);
    regex="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    number=firstAlp.find_first_not_of(regex);
    if(number!=string::npos)
    {
        return 2;
    }
    return 1;
}

scannar::~scannar()
{
}
