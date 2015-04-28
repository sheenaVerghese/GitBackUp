#include "codegenerator.h"
#include "ui_codegenerator.h"

codegenerator::codegenerator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::codegenerator),
    blockData(0),
    newBlock(1),
    error(0),
    errorLine(0),
    countIf(0),
    countElse(0),
    countFor(0),
    countWhile(0),
    bracket(0),
    begin(false),
    end(false),
    idPos(-1),
    mulDiv(false),
    addMinus(false),
    computeVal(0),
    loop(0),
    trueFalse(false),
    loopChecker(0),
    loopCounter(20)
{
    QVBoxLayout *mainLayout =new QVBoxLayout;
    vLayout = new QVBoxLayout;
    widget= new QWidget;//parent widget
    verticalBox=new QGroupBox;
    displayResult=new QPlainTextEdit;
    displayResult->insertPlainText("> Starting Code\n");
    vLayout->addWidget(displayResult);
    verticalBox->setLayout(vLayout);
    mainLayout->addWidget(verticalBox);

    setLayout(mainLayout);

}

void codegenerator::startCodeGen(vector<scannar::Tokenizer> tok,
                                 deque<parser::symbolTable> env,
                                 vector<parser::positions> pos)
{
    int vectorSize=tok.size();
    for(unsigned int i=0;i<vectorSize;i++)
    {
        code.push_back(tok.at(i));
    }
    vectorSize=env.size();
    for(unsigned int i=0;i<vectorSize;i++)
    {
       environment.push_back(env.at(i));
    }
    vectorSize=pos.size();
    for(unsigned int i=0;i<vectorSize;i++)
    {
        envPtr.push_back(pos.at(i));
    }

    startCode();
    endCodeGen();

}
void codegenerator::blockInitialisation()
{

    blockData=newBlock;
    parent.push_back(blockData);
    ++newBlock;

}

void codegenerator::exitBlock()
{
    parent.pop_back();
    blockData=parent.at(parent.size()-1);
}
//used for while loops and for loops
//rolls the block value to the previous block.
void codegenerator::blockRollBack()
{

    parent.pop_back();
    blockData=parent.at(parent.size()-1);
    newBlock=blockData+1;

}
//reenter new block
void codegenerator::blockRollFoward()
{
    blockData=blockData+1;
    parent.push_back(blockData);
    newBlock=blockData+1;

}
void codegenerator::infiniteMessage(int pos)
{
    if(loopCounter<0)
    {
        stringstream convert;
        string line;
        convert<<code.at(pos).lineNum;
        line=convert.str();
        displayResult->insertPlainText("Infinite Loop found at line:"+QString::fromStdString(line));
        //just to break
        error=1;
    }
}
//only for second variable part of things
int codegenerator::forLoopVars(int i,int key=1)
{

    loopVars.clear();
    trackSemiColon.clear();
    int temp=i;
    if(key==1)
    {
        loop=1;

        loopVars.push_back(code.at(tempForPos));
        loopVars.push_back(code.at(tempForPos+1));
        while(i<code.size()&&code.at(i).tokens!=scannar::SEMICOLON)
        {
            if(code.at(i).variable.compare("/")==0
                    ||code.at(i).variable.compare("/-")==0
                    ||code.at(i).variable.compare("*")==0
                    ||code.at(i).variable.compare("*-")==0)
            {
                mulDiv=true;
            }
           if(code.at(i).variable.compare("+")==0
                    ||(code.at(i).variable.compare("-")==0&&i!=2))
            {
                addMinus=true;
            }


            loopVars.push_back(code.at(i));
            ++i;
        }
    }
    //when key is 2 enter else
    else
    {

        while(i<code.size()&&code.at(i).tokens!=scannar::T_RBR)
        {
            if(code.at(i).variable.compare("/")==0
                    ||code.at(i).variable.compare("/-")==0
                    ||code.at(i).variable.compare("*")==0
                    ||code.at(i).variable.compare("*-")==0)
            {
                mulDiv=true;
            }
           if(code.at(i).variable.compare("+")==0
                    ||(code.at(i).variable.compare("-")==0&&i!=2))
            {
                addMinus=true;
            }


            trackSemiColon.push_back(code.at(i));
            ++i;
        }
    }
    if(i>=code.size()-1)
    {
        errorLine=code.at(temp).lineNum;
        //missing semicolon
        error= 29;
    }

    else if(loopVars.size()!=0||trackSemiColon.size()!=0)
    {

        if(key==1)
        {
            computeLoop(0);
        }
        else
        {
            computeValues(0);
        }
    }
    else
    {
        errorLine=code.at(temp).lineNum;
        error= 27;
    }

    loop=0;
    return i;
}

int codegenerator::startCode()
{

        int temp=0;
        //start has to be BEGIN
        int vectorSize=code.size();


        for(unsigned int i=0;i<vectorSize;i++)
        {
            switch(code.at(i).tokens)
            {
                case scannar::IF:
                        ++countIf;

                        tokenType=scannar::IF;
                        loop=1;
                        temp=i;
                        i=getLoopVars(i);
                        //it has to be in this position for if loop otherwise if there is
                        //variable hiding then it will detect that variable instead of the previous
                        //ones
                        blockInitialisation();
                        //setting up the truth statement deque
                        truthStatements.push_back(trueFalse);

                        //if an error has occured
                        if(error!=0)
                        {

                            return error;
                        }
                        if(!trueFalse)
                        {
                            int count=1;
                            int elseCounter=0;
                            //if the next one is an else or an end if to
                            //avoid going into the while loop, set count to 0
                            if(code.at(i+1).tokens==scannar::END_IF
                                    ||code.at(i+1).tokens==scannar::ELSE)
                            {
                                count=0;
                            }
                            while((code.at(i+1).tokens!=scannar::END_IF
                                  &&code.at(i+1).tokens!=scannar::ELSE)
                                  ||count>1)
                            {
                                if(code.at(i+1).tokens==scannar::IF)
                                {
                                    blockInitialisation();
                                    ++count;

                                }
                                if(code.at(i+1).tokens==scannar::ELSE)
                                {
                                    if(count>1)
                                    {
                                    exitBlock();
                                    blockInitialisation();
                                    }
                                    ++elseCounter;
                                    --count;

                                }
                                if(code.at(i+1).tokens==scannar::END_IF)
                                {
                                    if(count>1)
                                    {
                                     exitBlock();
                                    }
                                    if(elseCounter>0)
                                    {
                                        --elseCounter;
                                    }
                                    else
                                    {
                                     --count;
                                    }


                                }
                                if(code.at(i+1).tokens==scannar::FOR)
                                {

                                    if(count>1)
                                    {
                                    blockInitialisation();
                                    }


                                }
                                if(code.at(i+1).tokens==scannar::END_FOR)
                                {

                                    if(count>1)
                                    {
                                    exitBlock();
                                    }

                                }
                                if(code.at(i+1).tokens==scannar::WHILE)
                                {

                                    if(count>1)
                                    {
                                    blockInitialisation();
                                    }


                                }
                                if(code.at(i+1).tokens==scannar::END_WHILE)
                                {

                                    if(count>1)
                                    {
                                    exitBlock();
                                    }

                                }
                                ++i;

                            }

                            if(code.at(i+1).tokens==scannar::END_IF)
                            {
                                if(code.at(i+2).tokens==scannar::ELSE)
                                {
                                    ++i;
                                }
                            }
                        }

                        break;

                 case scannar::END_IF:
                          exitBlock();
                        //remove the last one

                        truthStatements.pop_back();

                        trueFalse=true;

                        break;
                 case scannar::FOR:
                        loop=0;
                        tempForPos=i;
                        blockInitialisation();
                        //do 3 and 2
                        if(infiniteLoop.size()>loopChecker)
                        {

                            int countSemiColon=0;
                            while(countSemiColon!=2)
                            {
                                if(code.at(i).tokens==scannar::SEMICOLON)
                                {
                                    ++countSemiColon;
                                }
                               ++i;
                            }
                            //increment by extra 1 to go to position after the ;

                            //2 stands for the key. It can be anything other than the default key=1
                            //checks the 3rd element

                            tokenType=scannar::FOR;
                            i=forLoopVars(i,2);
                            countSemiColon=0;
                            while(countSemiColon!=2)
                            {
                                if(code.at(i).tokens==scannar::SEMICOLON)
                                {
                                    ++countSemiColon;
                                }
                               --i;
                            }
                            //add 1 to be after the return so tht i will be one after the ;
                            ++i;

                            //checks the 2nd element
                            tokenType=scannar::FOR;
                            i=forLoopVars(i+1);
                            //ignores until after the right bracket
                            //fast fowarding the code.
                            while(code.at(i).tokens!=scannar::T_RBR)
                            {
                                ++i;
                            }

                            ++loopChecker;
                            --loopCounter;

                        }
                        //the only time it should be equals is the very first time
                        //do 1st part and 2nd part of the loop conditions
                        else
                        {

                           tokenType=code.at(i+2).tokens;

                           //doing the first part
                           if(tokenType==scannar::T_ID)
                           {
                               i=findSemiColonStatement(i+2);
                           }
                           else
                           {
                               //ignores the int part of things
                               i=findSemiColonStatement(i+3);
                           }
                           //need to add 1 cos the returned value will be semicolon
                            ++i;
                           //checks the condition variable for key =1;
                            tokenType=scannar::FOR;

                            i=forLoopVars(i);
                            //ignores until after the right bracket
                            //fast fowarding the code.
                            while(code.at(i).tokens!=scannar::T_RBR)
                            {
                                ++i;
                            }
                            ++loopChecker;
                            loopCounter=19;
                            infiniteLoop.push_back(loopCounter);
                        }

                        //if an error has occured
                        if(error!=0)
                        {

                            return error;
                        }
                        //roll till end for is found
                        if(!trueFalse)
                        {
                            int countForLoop=1;


                            while(code.at(i+1).tokens!=scannar::END_FOR
                                  ||countForLoop>1)
                            {
                                if(code.at(i+1).tokens==scannar::FOR)
                                {
                                    blockInitialisation();

                                    ++countForLoop;
                                }

                                if(code.at(i+1).tokens==scannar::END_FOR)
                                {

                                    if(countForLoop>1)
                                    {
                                        exitBlock();
                                    }
                                    --countForLoop;

                                }
                                if(code.at(i+1).tokens==scannar::WHILE)
                                {
                                    if(countForLoop>1)
                                    {

                                    blockInitialisation();
                                    }

                                }

                                if(code.at(i+1).tokens==scannar::END_WHILE)
                                {

                                    if(countForLoop>1)
                                    {
                                    exitBlock();
                                    }

                                }
                                if(code.at(i+1).tokens==scannar::IF)
                                {
                                    if(countForLoop>1)
                                    {
                                    blockInitialisation();
                                    }

                                }
                                if(code.at(i+1).tokens==scannar::ELSE)
                                {
                                    if(countForLoop>1)
                                    {
                                    exitBlock();
                                    blockInitialisation();
                                    }


                                }
                                if(code.at(i+1).tokens==scannar::END_IF)
                                {
                                    if(countForLoop>1)
                                    {
                                     exitBlock();
                                    }
                                }
                                ++i;

                            }
                        }
                        break;

                 case scannar::END_FOR:

                if(trueFalse)
                {

                    int countEnd=1;

                    //revert till you find while and then go one step backwards.
                    while(code.at(i-1).tokens!=scannar::FOR
                          ||countEnd>1)
                    {

                        if(code.at(i-1).tokens==scannar::FOR)
                        {
                            if(countEnd>1)
                            {
                                blockRollBack();
                            }

                                --countEnd;



                        }
                        //code here for first time entry only
                        if(code.at(i-1).tokens==scannar::END_FOR)
                        {
                           blockRollFoward();

                                ++countEnd;

                        }

                        if(code.at(i-1).tokens==scannar::WHILE)
                        {
                            if(countEnd>1)
                            {
                                blockRollBack();
                            }

                        }
                        //code here for first time only
                        if(code.at(i-1).tokens==scannar::END_WHILE)
                        {

                               if(countEnd>1)
                               {
                                blockRollFoward();
                               }

                        }


                        if(code.at(i-1).tokens==scannar::IF)
                        {
                            if(countEnd>1)
                            {
                            blockRollBack();
                            }

                        }
                        if(code.at(i-1).tokens==scannar::ELSE)
                        {
                            if(countEnd>1)
                            {
                            blockRollFoward();
                            blockRollBack();
                            }


                        }
                        if(code.at(i-1).tokens==scannar::END_IF)
                        {
                            if(countEnd>1)
                            {
                             blockRollFoward();
                            }
                        }

                        --i;

                    }
                    //need to roll back an extra time so that it goes to the block before
                    //the start of while

                    blockRollBack();
                    //need to decrement one extra time because at the bottom i'm incrementing i to become 1;
                    i=i-2;

                    --loopChecker;

                }

                //if not true which means exit the block
                else
                {
                    exitBlock();

                    //temporarily blocking it
                    trueFalse=true;
                    --loopChecker;

                    infiniteLoop.pop_back();
                    if(infiniteLoop.size()>0)
                    {
                        loopCounter=infiniteLoop.at(infiniteLoop.size()-1);
                    }
                }

                infiniteMessage(i+1);

                        break;

                 case scannar::WHILE:

                        loop=1;
                        tokenType=scannar::WHILE;
                        temp=i;
                        i=getLoopVars(i);
                        //initialisation has to occur only after the while(...) part
                        //this is beacuse no new variable initialisation is allowed

                        blockInitialisation();

                         if(infiniteLoop.size()>loopChecker)
                         {
                             ++loopChecker;
                             --loopCounter;

                         }
                         //the only time it should be equals is the very first time
                         else
                         {

                             ++loopChecker;
                             loopCounter=19;
                             infiniteLoop.push_back(loopCounter);
                         }


                       if(error!=0)
                        {

                            return error;
                        }

                        if(!trueFalse)
                        {
                            int countWhileLoop=1;

                            while(code.at(i+1).tokens!=scannar::END_WHILE
                                  ||countWhileLoop>1)
                            {
                                //only at first time entry
                                if(code.at(i+1).tokens==scannar::WHILE)
                                {

                                    blockInitialisation();
                                    ++countWhileLoop;
                                }

                                if(code.at(i+1).tokens==scannar::END_WHILE)
                                {

                                    if(countWhileLoop>1)
                                    {
                                    exitBlock();
                                    }
                                    --countWhileLoop;
                                }
                                if(code.at(i+1).tokens==scannar::FOR)
                                {

                                    if(countWhileLoop>1)
                                    {
                                    blockInitialisation();
                                    }


                                }
                                if(code.at(i+1).tokens==scannar::END_FOR)
                                {

                                    if(countWhileLoop>1)
                                    {
                                    exitBlock();
                                    }

                                }
                                if(code.at(i+1).tokens==scannar::IF)
                                {
                                    if(countWhileLoop>1)
                                    {
                                    blockInitialisation();
                                    }

                                }
                                if(code.at(i+1).tokens==scannar::ELSE)
                                {
                                    if(countWhileLoop>1)
                                    {
                                    exitBlock();
                                    blockInitialisation();
                                    }


                                }
                                if(code.at(i+1).tokens==scannar::END_IF)
                                {
                                    if(countWhileLoop>1)
                                    {
                                     exitBlock();
                                    }
                                }
                                ++i;
                            }

                        }

                        break;

                 case scannar::END_WHILE:
                //if the while loop was false then exit the block

                        if(trueFalse)
                        {

                            int countEnd=1;

                            //revert till you find while and then go one step backwards.
                            while(code.at(i-1).tokens!=scannar::WHILE
                                  ||countEnd>1)
                            {

                                if(code.at(i-1).tokens==scannar::WHILE)
                                {
                                    if(countEnd>1)
                                    {
                                        blockRollBack();
                                    }

                                        --countEnd;



                                }
                                //code here for first time only
                                if(code.at(i-1).tokens==scannar::END_WHILE)
                                {

                                        blockRollFoward();
                                        ++countEnd;


                                }


                                if(code.at(i-1).tokens==scannar::IF)
                                {
                                    if(countEnd>1)
                                    {
                                    blockRollBack();
                                    }

                                }
                                if(code.at(i-1).tokens==scannar::ELSE)
                                {
                                    if(countEnd>1)
                                    {
                                    blockRollFoward();
                                    blockRollBack();
                                    }


                                }
                                if(code.at(i-1).tokens==scannar::END_IF)
                                {
                                    if(countEnd>1)
                                    {
                                    blockRollFoward();
                                    }

                                }
                                if(code.at(i-1).tokens==scannar::FOR)
                                {
                                    if(countEnd>1)
                                    {
                                    blockRollBack();
                                    }


                                }
                                if(code.at(i-1).tokens==scannar::END_FOR)
                                {
                                    if(countEnd>1)
                                    {
                                     blockRollFoward();
                                    }
                                }

                                --i;

                            }
                            //need to roll back an extra time so that it goes to the block before
                            //the start of while

                            blockRollBack();
                            //need to decrement one extra time because at the bottom i'm incrementing i to become 1;
                            i=i-2;

                            --loopChecker;

                        }

                        //if not true which means exit the block
                        else
                        {
                            exitBlock();

                            //temporarily blocking it
                            trueFalse=true;
                            --loopChecker;

                            infiniteLoop.pop_back();
                            if(infiniteLoop.size()>0)
                            {
                                loopCounter=infiniteLoop.at(infiniteLoop.size()-1);
                            }
                        }


                        infiniteMessage(i+1);

                break;

                 case scannar::BEGIN:
                       if(!begin)
                       {

                            blockInitialisation();

                            begin=true;

                       }
                       else
                       {
                           //multiple initialisation of begin

                           return 11;
                       }

                        break;

                 case scannar::END:
                       end=true;
                       break;

                 case scannar::READ:

                        //invalid command
                        return 30;


                 case scannar::PRINT:
                        tokenType=scannar::T_STR;
                        //to indicate type print
                        computeVal=1;

                        i=findSemiColonStatement(i);
                        if(error!=0)
                        {
                            return error;
                        }


                        break;
                 case scannar::T_LSB:

                        return 23;


                 case scannar::T_RSB:

                        return 23;


                 case scannar::T_RCB:

                        return 23;


                 case scannar::T_LCB:

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

                            return 20;
                        }
                        break;
                 case scannar::ELSE:

                        exitBlock();


                        blockInitialisation();

                        //getting the truth value of the if related to the else
                        trueFalse=truthStatements.at(truthStatements.size()-1);
                        if(trueFalse)
                        {
                            int count=1;
                            //setting count to 0 if the next value is an end if
                            //avoid going into the while loop below

                            if(code.at(i+1).tokens==scannar::END_IF)
                            {
                                count=0;
                            }
                            while(code.at(i+1).tokens!=scannar::END_IF
                                  ||count>1)
                            {
                                if(code.at(i+1).tokens==scannar::IF)
                                {
                                    blockInitialisation();

                                    ++count;

                                }

                                if(code.at(i+1).tokens==scannar::END_IF)
                                {
                                    if(count>1)
                                    {
                                     exitBlock();
                                    }
                                     --count;

                                }
                                if(code.at(i+1).tokens==scannar::WHILE)
                                {

                                    if(count>1)
                                    {
                                    blockInitialisation();
                                    }


                                }
                                if(code.at(i+1).tokens==scannar::END_WHILE)
                                {

                                    if(count>1)
                                    {
                                    exitBlock();
                                    }

                                }
                                if(code.at(i+1).tokens==scannar::FOR)
                                {

                                    if(count>1)
                                    {
                                    blockInitialisation();
                                    }


                                }
                                if(code.at(i+1).tokens==scannar::END_FOR)
                                {

                                    if(count>1)
                                    {
                                    exitBlock();
                                    }

                                }
                                ++i;
                            }
                        }

                        break;
                 case scannar::EQUALS:

                        return 27;


                 case scannar::SEMICOLON:

                        return 46;


                 case scannar::T_DEC:
                    {
                        tokenType=scannar::T_DEC;
                        temp=i;
                        //ignores the dec part and goes straight to the id
                        i=findSemiColonStatement(i+1);

                        if(error!=0)
                        {

                            return error;
                        }

                        break;
                 }

                 case scannar::T_INT:
            {
                        tokenType=scannar::T_INT;
                        temp=i;
                        //ignores the type int and goes straight to the id type
                        i=findSemiColonStatement(i+1);
                        if(error!=0)
                        {

                            return error;
                        }
                       break;
            }

                 case scannar::T_CHAR:
                            {
                                tokenType=scannar::T_CHAR;
                                temp=i;
                                //ignores the char and goes straight to the id type
                                i=findSemiColonStatement(i+1);

                                if(error!=0)
                                {

                                    return error;
                                }
                              break;
                         }

                 case scannar::T_STR:
                    {
                        tokenType=scannar::T_STR;
                        temp=i;
                        //ignores the str part and goes straight to the id part
                        i=findSemiColonStatement(i+1);

                        if(error!=0)
                        {

                            return error;
                        }

                        break;
                 }

                 case scannar::T_OP:

                        return 27;

                 case scannar::T_FSLASH:

                        return 37;

                 case scannar::T_BSLASH:
                //invalid character or comment

                       return 37;

                 case scannar::T_DOP:

                        return 37;

                //no occasion where the literals should be assessed
                //here it should be assigned to a variable or in a loop
                 case scannar::T_CHARLit:

                        return 30;

                 case scannar::T_STRLit:

                        return 30;

                 case scannar::T_DECLit:

                        return 30;

                 case scannar::T_INTLit:

                        return 30;

                 case scannar::T_ID:
            {
                //declaring the variable

                        tokenType=scannar::T_ID;
                        temp=i;
                        i=findSemiColonStatement(i);
                        if(error!=0)
                        {

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
}
bool codegenerator::lookUpVars(int i,int key=0)
{


    int sizeDq=envPtr.size();
    int k=1;//just a loop variable

    //loop checks if the block has ever been declared before by checking
    //the "pointers"


    int parentBlock;

    //the parent starts from the current block
    if(trackSemiColon.at(i).tokens==scannar::T_ID)
    {

        if(sizeDq!=0)
        {
            for(parentBlock=parent.size()-1;parentBlock>=0;parentBlock--)
            {
                //k is a loop variable
                k=1;

                while(k<=sizeDq&&envPtr.at(sizeDq-k).block!=parent.at(parentBlock))
                {

                    //k is just alooping variable. It acts as a counter
                    ++k;
                }

                //k is a looping variable. It acts as a counter
                if(k<=sizeDq&&k>0)
                {


                    int getPos= envPtr.at(sizeDq-k).position;


                    while(getPos<environment.size()&&
                          environment.at(getPos).block==parent.at(parentBlock))
                    {

                        if(getPos<environment.size()&&environment.at(getPos).variable.
                                compare(trackSemiColon.at(i).variable)==0)
                        {
                            //need to put it out here just in case it is actually an assignment
                            idPos=getPos;
                            //key =1 is code for assigning a value to the variable
                            if(key==1)
                            {
                                return true;
                            }

                            if(environment.at(getPos).value.length()>0)
                            {

                            //for 'a' in a=a+b;--> is an example only
                            if(i==0)
                            {
                                idType=environment.at(getPos).tokens;
                                idPos=getPos;

                                return true;
                            }
                            else
                            {
                                idPos=getPos;
                                //checks if the variable has been initialised before.
                                if(environment.at(getPos).init)
                                {
                                assignId=environment.at(getPos).tokens;

                                    return true;
                                }
                                else
                                {

                                    return false;
                                }
                            }
                        }

                        }
                        ++getPos;
                    }
                }
            }
        }
    }


    return false;
}
int codegenerator::getLoopVars(int i)
{
    trackSemiColon.clear();
    loopVars.clear();
    mulDiv=false;
    addMinus=false;
   // bool checked=false;
    int temp=i;
    int openBracket=0;
    //get upto but not including the ')'

    while(i<code.size()&&code.at(i).tokens!=scannar::T_RBR)
    {
        if(code.at(i).variable.compare("/")==0
                ||code.at(i).variable.compare("/-")==0
                ||code.at(i).variable.compare("*")==0
                ||code.at(i).variable.compare("*-")==0)
        {
            mulDiv=true;
        }
       if(code.at(i).variable.compare("+")==0
                ||(code.at(i).variable.compare("-")==0&&i!=2))
        {
            addMinus=true;
        }


        loopVars.push_back(code.at(i));
        ++i;
    }



    if(i>=code.size()-1)
    {
        errorLine=code.at(temp).lineNum;
        //missing semicolon
        error= 29;
    }
    else if(openBracket>0||openBracket<0)
    {
        errorLine=code.at(temp).lineNum;
        error= 22;
    }
    else if(loopVars.size()!=0)
    {

        //the 0 has no particular significance in this case

        computeLoop(0);
    }
    else
    {
        errorLine=code.at(temp).lineNum;
        error= 27;
    }

    return i;

}
int codegenerator::findSemiColonStatement(int i)
{
    //clear old data first
    trackSemiColon.clear();
    loop=0;
    mulDiv=false;
    addMinus=false;

    int temp=i;
    int openBracket=0;
    //get upto but not including the ';'

    while(i<code.size()&&code.at(i).tokens!=scannar::SEMICOLON)
    {
        if(code.at(i).variable.compare("/")==0
                ||code.at(i).variable.compare("/-")==0
                ||code.at(i).variable.compare("*")==0
                ||code.at(i).variable.compare("*-")==0)
        {
            mulDiv=true;
        }
       if(code.at(i).variable.compare("+")==0
                ||(code.at(i).variable.compare("-")==0&&i!=2))
        {
            addMinus=true;
        }


        trackSemiColon.push_back(code.at(i));
        ++i;
    }

    if(trackSemiColon.size()<3)
    {
        return i;
    }
    //this part is just to get rid of the last bracket
    if(trackSemiColon.at(0).tokens==scannar::PRINT)
    {
        trackSemiColon.pop_back();

    }
    //returns i if it is an uninitialised variable

    if(i>=code.size()-1)
    {
        errorLine=code.at(temp).lineNum;
        //missing semicolon
        error= 29;
    }
    else if(openBracket>0||openBracket<0)
    {
        errorLine=code.at(temp).lineNum;
        error= 22;
    }
    else if(trackSemiColon.size()!=0)
    {

        //the 0 has no particular significance in this case
        computeValues(0);
    }
    else
    {
        errorLine=code.at(temp).lineNum;
        error= 27;
    }

    return i;

}
//int i is just 0
//all values are redirected to be computed here
int codegenerator::computeValues(int i=0)
{

    //in here do multiple computation
    //computation always starts after 1 ie from 2 onwards cos a=... is 0,1,2..
        if(tokenType==scannar::T_ID)
        {
            lookUpVars(0);
            tokenType=environment.at(idPos).tokens;
        }
        if(tokenType==scannar::IF||tokenType==scannar::WHILE||tokenType==scannar::FOR)
        {
            //starts at 2 because it is always if(a
            if(tokenType==scannar::IF)
            {
                tempToken=scannar::IF;
            }
            if(tokenType==scannar::WHILE)
            {
                tempToken=scannar::WHILE;
            }
            if(tokenType==scannar::FOR)
            {
                tempToken=scannar::FOR;
            }
            for(int i=2;i<trackSemiColon.size();i++)
            {
                //replaces all the idtype with the actual values
                if(trackSemiColon.at(i).tokens==scannar::T_ID)
                {

                    lookUpVars(i);
                    trackSemiColon.at(i).variable=environment.at(idPos).value;
                    trackSemiColon.at(i).tokens=environment.at(idPos).tokens;

                }

            }
            if(trackSemiColon.at(2).variable.compare("-")==0)
            {
                string concat=checkNegation(2);

                trackSemiColon.at(2).variable=concat;
                trackSemiColon.at(2).tokens=scannar::T_INT;

                //this gets rid of the 4th element if counting from base 1 ie 1,2...
                trackSemiColon.erase(trackSemiColon.begin()+3);

            }
            for(int i=2;i<trackSemiColon.size();i++)
            {
                if(trackSemiColon.at(i).tokens==scannar::EQUALS
                        &&trackSemiColon.at(i+1).tokens==scannar::EQUALS)
                {

                    ++i;

                }
                if(trackSemiColon.at(i).tokens==scannar::T_COP
                        ||trackSemiColon.at(i).variable.compare("!=")==0
                        ||trackSemiColon.at(i).tokens==scannar::EQUALS)
                {
                     if(trackSemiColon.at(i+1).variable.compare("-")==0)
                   {

                       string concat=checkNegation(i+1);
                       trackSemiColon.at(i+1).variable=concat;
                       trackSemiColon.at(i+1).tokens=trackSemiColon.at(i+2).tokens;

                       trackSemiColon.erase(trackSemiColon.begin()+(i+2));

                       --i;
                   }

                }

            }

                tokenType=trackSemiColon.at(2).tokens;


        }
        //if read from screen
        if(trackSemiColon.at(2).tokens==scannar::READ)
        {
            bool ok;
             QString text = QInputDialog::getText(this, tr("Enter value"),tr("Value:"), QLineEdit::Normal,"", &ok);
             if(text.isEmpty()==true||!ok)
             {
                 displayResult->insertPlainText("Error: Invalid input given. Program Stopped!");
                 error=50;
                 return 50;

             }
             string value=text.QString::toStdString();
             if(tokenType==scannar::T_DEC)
             {
                 if(!isDec(value))
                 {
                     displayResult->insertPlainText("Error:Invalid input. Demical type expected.\nProgram stopped.\n");
                     //invalid input decimal expected
                     error=50;
                     return 50;
                 }

             }
             if(tokenType==scannar::T_INT)
             {
                if(!isInteger(value))
                {
                    displayResult->insertPlainText("Error:Invalid input. Integer type expected.\nProgram stopped.\n");
                    //invalid input int expected
                    error=50;
                    return 50;
                }
             }
              if(tokenType==scannar::T_CHAR)
             {
                 if(value.length()>1)
                 {
                     displayResult->insertPlainText("Error:Invalid input. Character type expected.\nProgram stopped.\n");
                     //invalid input char expected
                     error=50;
                     return 50;
                 }
             }

            //type str would be correct cos it inputs a str
            trackSemiColon.at(2).variable=value;
            trackSemiColon.at(2).tokens=tokenType;
            print(value);
            //reducing the size by 2 since the last two are just brackets
            trackSemiColon.pop_back();
            trackSemiColon.pop_back();

            //setting it to this so that it doesn't go into the other if loops below
            tokenType=scannar::READ;
        }
        //need to scan for mul or div first
        if(tokenType==scannar::T_INT||tokenType==scannar::T_DEC
                ||tokenType==scannar::T_DECLit||tokenType==scannar::T_INTLit)
        {
            if(trackSemiColon.size()>3)
            {
                //negative values to concat the negative and the values
                if(trackSemiColon.at(2).tokens==scannar::T_OP)
                {
                    if(trackSemiColon.at(3).tokens==scannar::T_ID)
                    {
                        lookUpVars(3);
                        trackSemiColon.at(3).variable=environment.at(idPos).value;
                        trackSemiColon.at(3).tokens=environment.at(idPos).tokens;
                    }
                    string concat=checkNegation(2);
                    trackSemiColon.at(2).variable=concat;
                    trackSemiColon.at(2).tokens=scannar::T_INT;

                    trackSemiColon.erase(trackSemiColon.begin()+3);

                }
            }
            //if there is a multiply or divide
            if(mulDiv)
            {
                //starts from 2 because a=1+2... is 0,1,2,3,4
                //this part is to do all mul and divs
                for(int pos=2;pos<trackSemiColon.size();pos++)
                {

                    //here it is a negative number at the start so just to concatenate it

                    if(trackSemiColon.at(pos).tokens==scannar::T_OP)
                    {
                        //divison
                        if(trackSemiColon.at(pos).variable.compare("/")==0
                                ||trackSemiColon.at(pos).variable.compare("/-")==0
                                || trackSemiColon.at(pos).variable.compare("*")==0
                                ||trackSemiColon.at(pos).variable.compare("*-")==0
                                )
                        {
                            //need to check if second value is a 0
                            //will need to erase 2 things
                            string numerator;
                            string denominator;
                            if(trackSemiColon.at(pos-1).tokens==scannar::T_ID)
                            {
                                lookUpVars(pos-1);
                                numerator=environment.at(idPos).value;
                            }
                            else
                            {
                                numerator=trackSemiColon.at(pos-1).variable;
                            }
                            if(trackSemiColon.at(pos+1).tokens==scannar::T_ID)
                            {
                                lookUpVars(pos+1);
                                denominator=environment.at(idPos).value;
                            }
                            else
                            {
                                denominator=trackSemiColon.at(pos+1).variable;
                            }
                            if(trackSemiColon.at(pos).variable.compare("/")==0
                                    ||trackSemiColon.at(pos).variable.compare("/-")==0)
                            {
                                if(denominator.compare("0")==0)
                                {
                                    error=49;
                                    displayResult->setPlainText("Error: Divison by zero ocurred!");
                                    return 49;

                                }

                            }
                             string result;
                            if(trackSemiColon.at(pos).variable.compare("/")==0)
                            {
                                if((tokenType==scannar::T_INT||tokenType==scannar::T_INTLit)&&loop!=1)
                                {
                                 result=divInt(numerator,denominator);
                                }
                                //is for type decimal
                                else
                                {
                                     result=divDec(numerator,denominator);
                                }
                            }
                            else if(trackSemiColon.at(pos).variable.compare("/-")==0)
                            {
                                if((tokenType==scannar::T_INT||tokenType==scannar::T_INTLit)&&loop!=1)
                                {
                                result=divIntNeg(numerator,denominator);
                                }
                                else
                                {
                                    result=divDecNeg(numerator,denominator);
                                }
                            }
                            else if(trackSemiColon.at(pos).variable.compare("*")==0)
                            {
                                if((tokenType==scannar::T_INT||tokenType==scannar::T_INTLit)&&loop!=1)
                                {
                                result=mulInt(numerator,denominator);
                                }
                                else
                                {
                                    result=mulDec(numerator,denominator);
                                }
                            }
                            else
                            {
                                if((tokenType==scannar::T_INT||tokenType==scannar::T_INTLit)&&loop!=1)
                                {
                                result=mulIntNeg(numerator,denominator);
                                }
                                else
                                {
                                    result=mulDecNeg(numerator,denominator);
                                }
                            }
                            trackSemiColon.at(pos-1).variable=result;
                            trackSemiColon.at(pos-1).tokens=scannar::T_INT;
                            int plus2=pos+2;
                            trackSemiColon.erase(trackSemiColon.begin()+pos,trackSemiColon.begin()+plus2);

                            //cos 2 values are reduced
                            --pos;

                        }


                    }


                }
            }
            //for addition and subtraction
             if(addMinus)
             {

                for(int pos=2;pos<trackSemiColon.size();pos++)
                {
                   // cout<<"size of tracksemicolon in else add and sub of int is "<<trackSemiColon.size()<<endl;
                    string numerator;
                    string denominator;
                    //if it is an operator
                    if(trackSemiColon.at(pos).tokens==scannar::T_OP)
                    {
                        if(trackSemiColon.at(pos).variable.compare("+")==0
                                ||trackSemiColon.at(pos).variable.compare("-")==0)
                        {
                            if(trackSemiColon.at(pos-1).tokens==scannar::T_ID)
                            {
                                lookUpVars(pos-1);
                                numerator=environment.at(idPos).value;
                            }
                            else
                            {
                                numerator=trackSemiColon.at(pos-1).variable;
                            }
                            if(trackSemiColon.at(pos+1).tokens==scannar::T_ID)
                            {

                                lookUpVars(pos+1);
                                denominator=environment.at(idPos).value;

                            }
                            else
                            {
                                denominator=trackSemiColon.at(pos+1).variable;
                            }

                             string result;
                            if(trackSemiColon.at(pos).variable.compare("+")==0)
                            {
                                if((tokenType==scannar::T_INT||tokenType==scannar::T_INTLit)&&loop!=1)
                                {
                                 result=addInt(numerator,denominator);
                                }
                                else
                                {
                                    result=addDec(numerator,denominator);
                                }
                            }
                            else
                            {
                                //goes into this if it is of type int and not a loop comparison part
                                if((tokenType==scannar::T_INT||tokenType==scannar::T_INTLit)&&loop!=1)
                                {
                                result=minusInt(numerator,denominator);
                                }
                                else
                                {
                                    result=minusDec(numerator,denominator);
                                }
                            }

                            trackSemiColon.at(pos-1).variable=result;
                            trackSemiColon.at(pos-1).tokens=scannar::T_INT;
                            int plus2=pos+2;
                            trackSemiColon.erase(trackSemiColon.begin()+pos,trackSemiColon.begin()+plus2);

                            //cos 2 values are reduced
                            --pos;
                        }
                    }

                }
             }


            }
     //char type
    if(tokenType==scannar::T_CHAR||tokenType==scannar::T_CHARLit)
    {

        string value;
        //if assigned a variable
        if(loop==0)
        {
        if(trackSemiColon.at(2).tokens==scannar::T_ID)
        {
            lookUpVars(2);
            value=environment.at(idPos).value;

        }
        //if it is a charLit
        else
        {
            value=trackSemiColon.at(2).variable;
            //gets rid of the ' ' open single qoutes part.
            value=value.substr(1,1);

        }
        trackSemiColon.at(2).variable=value;
        trackSemiColon.at(2).tokens=scannar::T_CHAR;
        }
        //for the moment it does for all loops
        else
        {
            for(int pos=2;pos<trackSemiColon.size();pos++)
            {
                if(trackSemiColon.at(pos).tokens==scannar::T_ID
                        ||trackSemiColon.at(pos).tokens==scannar::T_CHARLit)
                {
                if(trackSemiColon.at(pos).tokens==scannar::T_ID)
                {
                    lookUpVars(pos);
                    value=environment.at(idPos).value;

                }
                //if it is a charLit
                else
                {
                    value=trackSemiColon.at(pos).variable;
                    //gets rid of the ' ' open single qoutes part.
                    value=value.substr(1,1);

                }
                trackSemiColon.at(pos).variable=value;
                trackSemiColon.at(pos).tokens=scannar::T_CHAR;
                }
            }

        }
    }
    if(tokenType==scannar::T_STR||tokenType==scannar::T_STRLit)
    {
        string value;
        string value2;
        if(trackSemiColon.size()==3)
        {
            if(trackSemiColon.at(2).tokens==scannar::T_CHARLit)
            {
                value=trackSemiColon.at(2).variable;
                //gets rid of the ' ' open single qoutes part.
                value=value.substr(1,1);
            }
            else if(trackSemiColon.at(2).tokens==scannar::T_STRLit)
            {
                value=trackSemiColon.at(2).variable;
                size_t found= value.find_last_of("\"");

                value=value.substr(1,found-1);
            }
            //if it is an int or decimal
            else
            {
                if(trackSemiColon.at(2).tokens==scannar::T_ID)
                {
                    lookUpVars(2);
                    value=environment.at(idPos).value;
                }
                else
                {
                value=trackSemiColon.at(2).variable;
                }
            }

            trackSemiColon.at(2).variable=value;
            trackSemiColon.at(2).tokens=scannar::T_STR;

        }
        else
        {
            if(trackSemiColon.size()>3)
            {
                for(int pos=2;pos<trackSemiColon.size();pos++)
                {
                    //if append
                    if(trackSemiColon.at(pos).tokens==scannar::T_OP)
                    {
                        if(trackSemiColon.at(pos).variable.compare("+")==0)
                        {
                            if(trackSemiColon.at(pos-1).tokens==scannar::T_STRLit)
                            {
                                value=trackSemiColon.at(pos-1).variable;
                                size_t found= value.find_last_of("\"");

                                value=value.substr(1,found-1);
                            }
                            else if(trackSemiColon.at(pos-1).tokens==scannar::T_CHARLit)
                            {
                                value=trackSemiColon.at(pos-1).variable;
                                //gets rid of the ' ' open single qoutes part.
                                value=value.substr(1,1);

                            }
                            else if(trackSemiColon.at(pos-1).tokens==scannar::T_ID)
                            {
                                lookUpVars(pos-1);
                                value=environment.at(idPos).value;

                            }
                            else
                            {
                                 value=trackSemiColon.at(pos-1).variable;
                            }
                             if(trackSemiColon.at(pos+1).tokens==scannar::T_STRLit)
                            {
                                value2=trackSemiColon.at(pos+1).variable;

                                size_t found= value2.find_last_of("\"");

                                value2=value2.substr(1,found-1);
                             //   cout<<"value oof str is after getting rid of quotes is "<<value2<<endl;

                            }
                            else if(trackSemiColon.at(pos+1).tokens==scannar::T_CHARLit)
                            {
                                value2=trackSemiColon.at(pos+1).variable;

                                value2=value2.substr(1,1);

                            }
                             else if(trackSemiColon.at(pos+1).tokens==scannar::T_ID)
                             {
                                 lookUpVars(pos+1);
                                 value2=environment.at(idPos).value;

                             }

                            else
                            {
                                value2=trackSemiColon.at(pos+1).variable;
                            }
                             value=value+value2;


                             trackSemiColon.at(pos-1).variable=value;
                             trackSemiColon.at(pos-1).tokens=scannar::T_STR;
                             int plus2=pos+2;
                             trackSemiColon.erase(trackSemiColon.begin()+pos,trackSemiColon.begin()+plus2);
                            --pos;
                     }
                   }
                    //if loop
                    if(loop==1)
                    {
                        for(int pos=2;pos<trackSemiColon.size();pos++)
                        {
                            if(trackSemiColon.at(pos).tokens==scannar::T_STRLit)
                            {
                                value=trackSemiColon.at(pos).variable;
                                size_t found= value.find_last_of("\"");

                                value=value.substr(1,found-1);
                            }
                            else if(trackSemiColon.at(pos).tokens==scannar::T_CHARLit)
                            {
                                value=trackSemiColon.at(pos).variable;
                                //gets rid of the ' ' open single qoutes part.
                                value=value.substr(1,1);

                            }
                            else if(trackSemiColon.at(pos).tokens==scannar::T_ID)
                            {
                                lookUpVars(pos);
                                value=environment.at(idPos).value;

                            }
                            else
                            {
                                 value=trackSemiColon.at(pos).variable;
                            }
                            //just replacing it with a value
                            trackSemiColon.at(pos).variable=value;
                            trackSemiColon.at(pos).tokens=scannar::T_STR;
                        }

                    }
                }
            }
        }
    }

    //overhere is the code to update the env table
    if(trackSemiColon.size()==3)
    {
        if(trackSemiColon.at(0).tokens!=scannar::PRINT)
        {

            //getting the idPos for the first variable

            lookUpVars(0,1);
            updateEnvTable(idPos);
        }
        if(trackSemiColon.at(0).tokens==scannar::PRINT)
        {
            print(trackSemiColon.at(2).variable);

        }

    }
    return 0;

}
//int i is the value for the position of the variable in the envTable as received by the idPos
void codegenerator::updateEnvTable(int i)
{

    string value;
    if(trackSemiColon.at(2).tokens==scannar::T_ID)
    {
        lookUpVars(2);
        value=environment.at(idPos).value;
    }
    else
    {
        value=trackSemiColon.at(2).variable;
    }
    environment.at(i).value=value;

}

void codegenerator::endCodeGen()
{
    displayResult->insertPlainText("> Done.");
}

string codegenerator::divInt(string numerator, string denominator)
{
    int number1;
    int number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    int value=number1/number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}

string codegenerator::divIntNeg(string numerator, string denominator)
{
    int number1;
    int number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    int value=number1/-number2;

    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
string codegenerator::mulInt(string numerator, string denominator)
{
    int number1;
    int number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    int value=number1*number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
string codegenerator::addInt(string numerator, string denominator)
{

    int number1;
    int number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    int value=number1+number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
string codegenerator::minusInt(string numerator, string denominator)
{
    int number1;
    int number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    int value=number1- number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
string codegenerator::mulIntNeg(string numerator, string denominator)
{
    int number1;
    int number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    int value=number1*-number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
//decimal functions

string codegenerator::divDec(string numerator, string denominator)
{
    double number1;
    double number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    double value=number1/number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}

string codegenerator::divDecNeg(string numerator, string denominator)
{
    double number1;
    double number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    double value=number1/-number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
string codegenerator::mulDec(string numerator, string denominator)
{
    double number1;
    double number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    double value=number1*number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
string codegenerator::addDec(string numerator, string denominator)
{
    double number1;
    double number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    double value=number1+number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
string codegenerator::minusDec(string numerator, string denominator)
{
    double number1;
    double number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    double value=number1- number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
string codegenerator::mulDecNeg(string numerator, string denominator)
{
    double number1;
    double number2;
    istringstream ( numerator ) >> number1;
    istringstream (denominator)>> number2;
    double value=number1*-number2;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;

}
bool codegenerator::isDec(string data)
{
    size_t point=data.find_first_of(".");
    string regex="0123456789";

    //check for decimal
    if(point!=string::npos)
    {

        size_t numerator = data.substr(0,point).find_first_not_of(regex);
        size_t denominator= data.substr(point+1).find_first_not_of(regex);

        string num1=data.substr(0,point);
        string num2=data.substr(point+1);

       if(numerator!=string::npos)
       {
           if(numerator==0)
           {
                string negative=data.substr(0,1);
               if(negative.compare("-")==0)
               {
                   numerator=data.substr(0,point).find_first_not_of(regex,1);
                   if(numerator<num1.length())
                   {
                       return false;
                   }

               }
           }
       }

       if(denominator<num2.length())
       {

           return false;
       }


    }
    else
    {
        //decimal type can support integers
        if(isInteger(data)==1)
        {
            return true;
        }
        return false;
    }


return true;
}

bool codegenerator::isInteger(string data)
{
    string regex="0123456789";
    size_t number=data.find_first_not_of(regex);

    if(number!=string::npos)
    {
        if(number==0)
        {
          string negative=data.substr(0,1);
          if(negative.compare("-")==0)
          {
              number=data.find_first_not_of(regex,1);
              if(number!=string::npos)
              {
                  return false;
              }
              else
              {
                  return true;
              }
          }
        }
        return false;
    }

    return true;
}
void codegenerator::print(string data)
{
    size_t find;
    find=0;
    string newLine=data;
    int firstEntry=0;
    while(find!=string::npos)
    {

              if(firstEntry!=0)
              {
              find=newLine.find("\\",find+1);

              }
              else
              {
                  find=newLine.find("\\");


              }

              if(find!=string::npos)
              {
                  string next=newLine.substr(find+1,1);
                if(next.compare("n")==0)
                {
                newLine.replace(find,2,"\n");

                }
                if(next.compare("t")==0)
                {
                    newLine.replace(find,2,"\t");
                }
              }
              firstEntry=1;
    }

    newLine=newLine+"\n";
    displayResult->insertPlainText(QString::fromStdString(newLine));
}
int codegenerator::computeLoop(int k)
{
    int pos=0;
    bool first=true;

    while(pos<=loopVars.size())
    {
        //looping variable i
        int i=0;
        int size=0;
        addMinus=false;
        mulDiv=false;
        trackSemiColon.clear();

        while(pos<loopVars.size()
              &&loopVars.at(pos).tokens!=scannar::T_DOP)
        {

            if(!first)
            {
                trackSemiColon.push_back(loopVars.at(0));
                trackSemiColon.push_back(loopVars.at(1));
                size+=2;
                first=true;
            }
            if(loopVars.at(pos).variable.compare("/")==0
                    ||loopVars.at(pos).variable.compare("/-")==0
                    ||loopVars.at(pos).variable.compare("*")==0
                    ||loopVars.at(pos).variable.compare("*-")==0)
            {
                mulDiv=true;
            }
           if(loopVars.at(pos).variable.compare("+")==0
                    ||(loopVars.at(pos).variable.compare("-")==0&&i!=2))
            {
                addMinus=true;
            }

            trackSemiColon.push_back(loopVars.at(pos));
            ++pos;
            ++size;

        }
        //need to increment it by an extra 1 because the exit condition is the T_DOP or RBR

        ++pos;



        computeValues(0);

        //3 cos if it is if(a>3 then size is 5 but if it is if(a==3 then size is 6
        if(trackSemiColon.size()==6||trackSemiColon.size()==5)
        {
            if(trackSemiColon.at(3).variable.compare("!=")==0)
            {
                trueFalse=notEquals();
            }
            else if(trackSemiColon.at(3).variable.compare("=")==0)
            {
                trueFalse=equalsEquals();
            }
            else if(trackSemiColon.at(3).variable.compare(">")==0)
            {
                trueFalse=greater();
            }
            else if(trackSemiColon.at(3).variable.compare("<")==0)
            {
                trueFalse=lessThan();
            }
            else if(trackSemiColon.at(3).variable.compare(">=")==0)
            {
                trueFalse=greaterEquals();
            }
            else
            {
                trueFalse=lessEquals();
            }
            if(trueFalse)
            {


                loopVars.at(pos-size+1).variable="true";

            }
            else
            {
                 loopVars.at(pos-size+1).variable="false";
            }
            loopVars.at(pos-size+1).tokens=scannar::T_STR;

            loopVars.erase(loopVars.begin()+(pos-size+2),loopVars.begin()+(pos-1));


            //should be either 2 or 3
            pos=pos-size+3;
            //need to transfer back to scannar::IF
            tokenType=tempToken;
            first=false;

        }
    }

    if(loopVars.at(loopVars.size()-1).tokens==scannar::T_RBR)
    {
        loopVars.pop_back();
    }
    //will actually be 4 if including the close bracket

    while(loopVars.size()>3)
    {

        for(int pos=0;pos<loopVars.size();pos++)
        {


            if(loopVars.at(pos).tokens==scannar::T_DOP)
            {
                if(loopVars.at(pos).variable.compare("||")==0)
                {
                    trueFalse=orOperator(pos-1,pos+1);
                }
                else
                {
                    trueFalse=andOperator(pos-1,pos+1);
                }
                if(trueFalse)
                {
                    loopVars.at(pos-1).variable="true";
                }
                else
                {
                    loopVars.at(pos-1).variable="false";
                }

                loopVars.at(pos-1).tokens=scannar::T_STR;
                int plus2=pos+2;

                loopVars.erase(loopVars.begin()+pos,loopVars.begin()+plus2);

               --pos;

            }
        }
    }
    return 0;
}
bool codegenerator::notEquals()
{
    if(trackSemiColon.at(2).variable.compare(trackSemiColon.at(4).variable)==0)
    {
        return false;
    }
   return true;
}
bool codegenerator::equalsEquals()
{

    if(trackSemiColon.at(2).variable.compare(trackSemiColon.at(5).variable)==0)
    {
        return true;
    }
   return false;
}
bool codegenerator::lessEquals()
{
    double number1;
    double number2;
    istringstream ( trackSemiColon.at(2).variable ) >> number1;
    istringstream (trackSemiColon.at(4).variable)>> number2;
    //double value=number1/number2;
    if(number1<=number2)
    {
        return true;
    }
    return false;

}
bool codegenerator::greaterEquals()
{
    double number1;
    double number2;
    istringstream ( trackSemiColon.at(2).variable ) >> number1;
    istringstream (trackSemiColon.at(4).variable)>> number2;
    //double value=number1/number2;
    if(number1>=number2)
    {
        return true;
    }
    return false;
}
bool codegenerator::greater()
{
    double number1;
    double number2;
    istringstream ( trackSemiColon.at(2).variable ) >> number1;
    istringstream (trackSemiColon.at(4).variable)>> number2;
    //double value=number1/number2;
    if(number1>number2)
    {
        return true;
    }
    return false;
}
bool codegenerator::lessThan()
{
    double number1;
    double number2;
    istringstream ( trackSemiColon.at(2).variable ) >> number1;
    istringstream (trackSemiColon.at(4).variable)>> number2;
    //double value=number1/number2;
    if(number1<number2)
    {
        return true;
    }
    return false;
}
bool codegenerator::orOperator(int value1, int value2)
{

    if(loopVars.at(value1).variable.compare("true")==0)
    {
        return true;
    }
    if(loopVars.at(value2).variable.compare("true")==0)
    {
        return true;
    }
    return false;
}
bool codegenerator::andOperator(int value1, int value2)
{
    if(loopVars.at(value1).variable.compare("false")==0
            ||loopVars.at(value2).variable.compare("false")==0)
    {
        return false;
    }
    return true;
}

string codegenerator::checkNegation(int pos)
{
    int number1;
    istringstream ( trackSemiColon.at(pos+1).variable ) >> number1;
    int value=-number1;
    stringstream convert;
    string result;
    convert<<value;
    result=convert.str();
    return result;
}

codegenerator::~codegenerator()
{
    delete ui;
}
