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
   // displayResult->setReadOnly(true);
    setLayout(mainLayout);
    //this->exec();
    //ui->setupUi(this);
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
  //  cout<<"---------------------all done codegen----------------------------\n"<<endl;
    startCode();
    endCodeGen();

}
void codegenerator::blockInitialisation()
{

    blockData=newBlock;
    parent.push_back(blockData);
    ++newBlock;
   // cout<<"init block data is "<<blockData<<endl;
}

void codegenerator::exitBlock()
{

    //--blockData;
  //  cout<<"exit block data is" <<blockData<<"parent size is "<<parent.size()<<endl;
    parent.pop_back();
    blockData=parent.at(parent.size()-1);
}
//used for while loops and for loops
//rolls the block value to the previous block.
void codegenerator::blockRollBack()
{
   // cout<<"in blockRollBack size b4 pop"<<parent.size()<<endl;
    parent.pop_back();
    blockData=parent.at(parent.size()-1);
    newBlock=blockData+1;
  //  cout<<"in blockRollBack"<<parent.size()<<endl;

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
   // cout<<"in for loop vars\n"<<endl;
    loopVars.clear();
    trackSemiColon.clear();
    int temp=i;
    if(key==1)
    {
        loop=1;
       // cout<<"in key =1"<<endl;
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
     //   cout<<"in key=2"<<endl;
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
    //parseStatement();
        //the 0 has no particular significance in this case
        //computeValues(0);
       // cout<<"to compute Loop"<<endl;
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
   // printTrackSemiColon();
   // cout<<"find ; tok= "<<parseTokenizer.at(i).tokens<<endl;
    loop=0;
    return i;
}

int codegenerator::startCode()
{

        int temp=0;
        //start has to be BEGIN
        int vectorSize=code.size();
        //debug only
      /*  for(int i=0;i<vectorSize;i++)
        {
            cout<<"!!!all the tokens involved are "<<code.at(i).tokens<<endl;
        }*/

        for(unsigned int i=0;i<vectorSize;i++)
        {
            switch(code.at(i).tokens)
            {
                case scannar::IF:
                        ++countIf;

                        //prevStatement.push_back("if");
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
                        //need to reduce by one otherwise it will miss a keyword
                        //--i;
                       // cout<<"in if checkStruct() i is ="<<i<<"token is "<<code.at(i).tokens<<"value is "<<code.at(i).variable<<endl;
                        //cout<<"next token is "<<parseTokenizer.at(i).tokens<<endl;
                        //if an error has occured
                        if(error!=0)
                        {
                            //errorLine=parseTokenizer.at(temp).lineNum;
                         //   cout<<"error is"<<error<<endl;
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
                               //     cout<<"increment loop if ----"<<count<<endl;
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
                                   //  cout<<"decrement  loop if else ----"<<count<<"else counter is "<<elseCounter<<endl;
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

                                  //   cout<<"decrement  loop if end_if ----"<<count<<endl;
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
                           //     cout<<"???value of i in if is "<<i<<"next token is "<<code.at(i+1).tokens<<endl;
                            }

                         //   cout<<"my count is "<<count<<endl;
                            if(code.at(i+1).tokens==scannar::END_IF)
                            {
                                if(code.at(i+2).tokens==scannar::ELSE)
                                {
                                    ++i;
                                }
                            }
                        }
                    //    cout<<"~~~~~~~end of if ~~~~~~~ "<<endl;
                        //--testing only
                       // return 0;
                        //--return is a test case only-- remove after testing
                        break;

                 case scannar::END_IF:
             //   cout<<"in END_IF"<<endl;

                        exitBlock();
                        //remove the last one

                        truthStatements.pop_back();

                        trueFalse=true;

                        break;
                 case scannar::FOR:
                        loop=0;
                        tempForPos=i;
                        blockInitialisation();
                    //    cout<<"block data is in for ----------------------------+++++++++++++ "<<blockData<<endl;
                        //do 3 and 2
                        if(infiniteLoop.size()>loopChecker)
                        {
                      //      cout<<"size is more than the checker\n"<<endl;
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
                          //  cout<<"((((((((((((((((((((((((( i value variable is "<<code.at(i).variable<<" next var is "<<code.at(i+1).variable<<endl;
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
                           // cout<<"((((((((((((((((((((((((( i value variable is "<<code.at(i).variable<<" next var is "<<code.at(i+1).variable<<endl;
                            //checks the 2nd element
                            tokenType=scannar::FOR;
                            i=forLoopVars(i+1);
                            //ignores until after the right bracket
                            //fast fowarding the code.
                            while(code.at(i).tokens!=scannar::T_RBR)
                            {
                                ++i;
                            }
                        //    cout<<"current token is in for loop "<<code.at(i).tokens<<endl;
                            ++loopChecker;
                            --loopCounter;

                        }
                        //the only time it should be equals is the very first time
                        //do 1 and 2
                        else
                        {
                        //   cout<<"size is less than checker "<<endl;
                           tokenType=code.at(i+2).tokens;
                        //   cout<<"(((0)))))the token type in else of for is "<<tokenType<<endl;
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
                       // temp=i;
                       // i=getLoopVars(i);
                        //need to reduce by one otherwise it will miss a keyword
                        //--i;
                       // cout<<"in if checkStruct() i is ="<<i<<"next token is "<<code.at(i+1).variable<<endl;
                       // cout<<"next token is "<<parseTokenizer.at(i).tokens<<endl;
                        //if an error has occured
                        if(error!=0)
                        {
                           // errorLine=parseTokenizer.at(temp).lineNum;
                         //   cout<<"error is"<<error<<endl;
                            return error;
                        }
                        //roll till end for is found
                        if(!trueFalse)
                        {
                            int countForLoop=1;
                           // cout<<"in not treufalse"<<endl;

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
                                   // ++countWhileLoop;
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

                                    // cout<<"decrement  loop if else ----"<<count<<"else counter is "<<elseCounter<<endl;
                                }
                                if(code.at(i+1).tokens==scannar::END_IF)
                                {
                                    if(countForLoop>1)
                                    {
                                     exitBlock();
                                    }
                                }
                                ++i;
                           //     cout<<"value of i is "<<code.at(i-1).variable<<endl;
                            }
                        }
                        //--testing only
                       // return 0;
                      //  cout<<"value of current i at for is "<<code.at(i).variable<<endl;
                        //--return is a test case only-- remove after testing

                        break;

                 case scannar::END_FOR:
               // cout<<"error at end_for"<<endl;
                if(trueFalse)
                {
                  //  cout<<"@@@ in true false of end_while block data is "<<blockData<<endl;
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


                      //          cout<<"---->roll back at while"<<endl;
                        }
                        //code here for first time only
                        if(code.at(i-1).tokens==scannar::END_FOR)
                        {
                           //    cout<<"||||| in end_while before init value is "<<blockData<<endl;

                                blockRollFoward();

                                ++countEnd;
                           //      cout<<"----->in end_while countEnd is "<<countEnd<<endl;

                        }

                        if(code.at(i-1).tokens==scannar::WHILE)
                        {
                            if(countEnd>1)
                            {
                                blockRollBack();
                            }

                                //--countEnd;


                            //    cout<<"---->roll back at while"<<endl;
                        }
                        //code here for first time only
                        if(code.at(i-1).tokens==scannar::END_WHILE)
                        {
                            //   cout<<"||||| in end_for before init value is "<<blockData<<endl;
                               if(countEnd>1)
                               {
                                blockRollFoward();
                               }
                               // ++countEnd;
                              //   cout<<"----->in end_for countEnd is "<<countEnd<<endl;

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

                            // cout<<"decrement  loop if else ----"<<count<<"else counter is "<<elseCounter<<endl;
                        }
                        if(code.at(i-1).tokens==scannar::END_IF)
                        {
                            if(countEnd>1)
                            {
                             blockRollFoward();
                            }
                        }

                        --i;
                     //   cout<<"---->decremented i in end_for "<<code.at(i).tokens<<
                      //        "---->variable is "<<code.at(i).variable<<"block data is  "<<blockData
                           //<<endl;
                        //temporary
                        /*if(countEnd==2&&code.at(i).variable.compare("b")==0)
                        {
                            return 0;
                        }
                        */
                        //cout<<"i is "<<i<<endl;
                    }
                    //need to roll back an extra time so that it goes to the block before
                    //the start of while
                   // cout<<"out of while loop in end while i is "<<i<<endl;
                    blockRollBack();
                    //need to decrement one extra time because at the bottom i'm incrementing i to become 1;
                    i=i-2;
                   // cout<<"---->value here at roll back after end_for i-2 "<<i<<code.at(i).tokens<<endl;
                    --loopChecker;
                   // cout<<"loop checker is ========= "<<loopChecker;
                }

                //if not true which means exit the block
                else
                {
                    exitBlock();
                 //  cout<<"back from exit block"<<endl;
                    //temporarily blocking it
                    trueFalse=true;
                    --loopChecker;
                  //  cout<<"loop checker is =========== "<<loopChecker;
                    infiniteLoop.pop_back();
                    if(infiniteLoop.size()>0)
                    {
                        loopCounter=infiniteLoop.at(infiniteLoop.size()-1);
                    }
                }
               /* if(loopCounter<=9)
                {
                    cout<<"loop counter is 9";
                    cout<<"loop checker is "<<loopChecker;
                    cout<<"size is "<<infiniteLoop.size()<<endl;
                    cout<<"error is "<<error<<endl;
                    return 1;
                }*/
               // cout<<"error was here?"<<endl;
                //cout<<"********************************Block Data end while**<"<<blockData<<endl;
                 //cout<<"--->newBlock is "<<newBlock<<endl;
                //cout<<"^^^^^next token in end_while is "<<code.at(i).tokens<<" variable is "<<code.at(i).variable<<endl;
             //   cout<<"the loop checker value in end_for is "<<loopChecker<<" the size of infiniteLoop is "<<infiniteLoop.size()<<endl;
                infiniteMessage(i+1);
                       // exitBlock();
                        break;

                 case scannar::WHILE:

                       // prevStatement.push_back("while");
                       // ++countWhile;
                        loop=1;
                        tokenType=scannar::WHILE;
                        temp=i;
                        i=getLoopVars(i);
                        //initialisation has to occur only after the while(...) part
                        //this is beacuse no new variable initialisation is allowed
                       // if(trueFalse)
                      //  {
                        blockInitialisation();
                     //   cout<<"block value is -------------------------------------+++++++"<<blockData<<endl;
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


                       // }
                        //need to reduce by one otherwise it will miss a keyword
                       // --i;
                      //  cout<<"&&&&&---->in if checkStruct() i is ="<<i<<"--->current token is"<<code.at(i).tokens<<endl;

                        if(error!=0)
                        {
                           // errorLine=parseTokenizer.at(temp).lineNum;
                         //   cout<<"error is"<<error<<endl;
                            return error;
                        }
                     //   cout<<":::::::::::truefalse is "<<trueFalse<<"parent is "<<parent.size()<<endl;
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

                                    // cout<<"decrement  loop if else ----"<<count<<"else counter is "<<elseCounter<<endl;
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
                            //need to minus by 1 because I need it to go into the end_while case
                          //  --i;
                        }
                       //   cout<<"--->token is "<<code.at(i).tokens<<" the variable is "<<code.at(i).variable<<endl;
                       //   cout<<"---------------------Block Data is in while -->"<<blockData<<endl;
                        //  cout<<"----->newBlock is "<<newBlock<<endl;
                        //if an error has occured
                      //  if(error!=0)
                        //{
                           // errorLine=parseTokenizer.at(temp).lineNum;
                          //  cout<<"error is"<<error<<endl;
                            //return error;
                        //}
                        //--testing only
                       // return 0;
                        //--return is a test case only-- remove after testing
                        break;

                 case scannar::END_WHILE:
                //if the while loop was false then exit the block
                    //    cout<<"////// in end_while"<<endl;
                   //     cout<<"---->value of i before revert is "<<i<<endl;
                     //   cout<<"---->size of parent is "<<parent.size()<<endl;
                        if(trueFalse)
                        {
                         //   cout<<"@@@ in true false of end_while block data is "<<blockData<<endl;
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


                                   //     cout<<"---->roll back at while"<<endl;
                                }
                                //code here for first time only
                                if(code.at(i-1).tokens==scannar::END_WHILE)
                                {
                                  //     cout<<"||||| in end_while before init value is "<<blockData<<endl;
                                        blockRollFoward();
                                        ++countEnd;
                                //         cout<<"----->in end_while countEnd is "<<countEnd<<endl;

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

                                    // cout<<"decrement  loop if else ----"<<count<<"else counter is "<<elseCounter<<endl;
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

                                    // cout<<"decrement  loop if else ----"<<count<<"else counter is "<<elseCounter<<endl;
                                }
                                if(code.at(i-1).tokens==scannar::END_FOR)
                                {
                                    if(countEnd>1)
                                    {
                                     blockRollFoward();
                                    }
                                }

                                --i;
                              //  cout<<"---->decremented i in end_while "<<code.at(i).tokens<<
                               //       "---->variable is "<<code.at(i).variable<<"block data is  "<<blockData
                              //     <<endl;
                                //temporary
                                /*if(countEnd==2&&code.at(i).variable.compare("b")==0)
                                {
                                    return 0;
                                }
                                */
                                //cout<<"i is "<<i<<endl;
                            }
                            //need to roll back an extra time so that it goes to the block before
                            //the start of while
                          //  cout<<"out of while loop in end while i is "<<i<<endl;
                            blockRollBack();
                            //need to decrement one extra time because at the bottom i'm incrementing i to become 1;
                            i=i-2;
                           // cout<<"---->value here at roll back after i-2 "<<i<<code.at(i).tokens<<endl;
                            --loopChecker;
                          //  cout<<"loop checker is ========= "<<loopChecker;
                        }

                        //if not true which means exit the block
                        else
                        {
                            exitBlock();

                            //temporarily blocking it
                            trueFalse=true;
                            --loopChecker;
                           // cout<<"loop checker is =========== "<<loopChecker;
                            infiniteLoop.pop_back();
                            if(infiniteLoop.size()>0)
                            {
                                loopCounter=infiniteLoop.at(infiniteLoop.size()-1);
                            }
                        }
                       /* if(loopCounter<=9)
                        {
                            cout<<"loop counter is 9";
                            cout<<"loop checker is "<<loopChecker;
                            cout<<"size is "<<infiniteLoop.size()<<endl;
                            cout<<"error is "<<error<<endl;
                            return 1;
                        }*/
                      //  cout<<"error was here?"<<endl;
                      //  cout<<"********************************Block Data end while**<"<<blockData<<endl;
                      //   cout<<"--->newBlock is "<<newBlock<<endl;
                      //  cout<<"^^^^^next token in end_while is "<<code.at(i).tokens<<" variable is "<<code.at(i).variable<<endl;
                        infiniteMessage(i+1);
                        /*if(loopCounter<=9)
                        {
                            cout<<"-------------------------------------------------------------------loop Counter "<<loopCounter<<"loopChecker is "<<loopChecker<<"size is "<<infiniteLoop.size()<<endl;
                            return 1;
                        }*/
                     //   cout<<"error is"<<error<<endl;
                break;

                 case scannar::BEGIN:
                       if(!begin)
                       {
                           /* scopeData=1;
                            blockData=1;
                            lineData=parseTokenizer.at(i).lineNum;*/
                            blockInitialisation();
                         //   cout<<"in begin parser i="<<i<<endl;
                            begin=true;
                          /*  if(parseTokenizer.size()>1)
                            {
                                if(!isLoop(i+1))
                                {
                                    i=findSemiColonStatement(i+1);
                                    if(error!=0)
                                    {

                                        return error;
                                    }

                                }
                            }*/
                            //cout<<"parser begin token is "<<parseTokenizer.at(i+1).tokens<<endl;
                       }
                       else
                       {
                           //multiple initialisation of begin
                      //     cout<<"parser multiple begin"<<endl;
                          // errorLine=parseTokenizer.at(i).lineNum;
                           return 11;
                       }

                        break;

                 case scannar::END:
                       // cout<<"in end";
                         //missingEnd=returnEnd("end");

                            end=true;

                      // blockExit();
                        break;

                 case scannar::READ:

                        //invalid command
                        return 30;
                       // break;

                 case scannar::PRINT:
                        tokenType=scannar::T_STR;
                        //to indicate type print
                        computeVal=1;
                      //  cout<<"in print"<<endl;
                        i=findSemiColonStatement(i);
                        if(error!=0)
                        {
                            return error;
                        }


                        break;
                 case scannar::T_LSB:
                       // errorLine=parseTokenizer.at(i).lineNum;
                        return 23;


                 case scannar::T_RSB:
                        //errorLine=parseTokenizer.at(i).lineNum;
                        return 23;
                        break;

                 case scannar::T_RCB:
                     //   errorLine=parseTokenizer.at(i).lineNum;
                        return 23;
                        break;

                 case scannar::T_LCB:
                       // errorLine=parseTokenizer.at(i).lineNum;
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
                          //  errorLine=parseTokenizer.at(i).lineNum;
                            return 20;
                        }
                        break;
                 case scannar::ELSE:
                       // cout<<"in ESLE"<<endl;

                        exitBlock();
                        //debugging only
                        /*for(int l=0;l<parent.size();l++)
                        {
                            cout<<"in else parent value is ............ "<<parent.at(l)<<endl;
                        }*/

                        blockInitialisation();
                      //  cout<<"size of truthStatements is "<<truthStatements.size()<<endl;
                        //getting the truth value of the if related to the else
                        trueFalse=truthStatements.at(truthStatements.size()-1);
                        if(trueFalse)
                        {
                            int count=1;
                            //setting count to 0 if the next value is an end if
                            //avoid going into the while loop below
                           // cout<<"else ******* to skip"<<endl;
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
                                //    cout<<"increment loop if of else ----"<<count<<endl;
                                }

                                if(code.at(i+1).tokens==scannar::END_IF)
                                {
                                    if(count>1)
                                    {
                                     exitBlock();
                                    }
                                     --count;
                                   //  cout<<"decrement  loop if end_if of else----"<<count<<endl;
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
                        //truthStatements.pop_back();
                        break;
                 case scannar::EQUALS:
                       // errorLine=parseTokenizer.at(i).lineNum;
                        return 27;
                        break;

                 case scannar::SEMICOLON:
                        //errorLine=parseTokenizer.at(i).lineNum;
                        return 46;
                        //break;

                 case scannar::T_DEC:
                    {
                        tokenType=scannar::T_DEC;
                        temp=i;
                        //ignores the dec part and goes straight to the id
                        i=findSemiColonStatement(i+1);
                       // cout<<"value is "<<value<<" i is before add ="<<i<<endl;

                       // cout<<"i after add is "<<i<<endl;
                        if(error!=0)
                        {
                           // errorLine=parseTokenizer.at(temp).lineNum;
                         //   cout<<"error is"<<error<<endl;
                            return error;
                        }
                       // cout<<"i is "<<i<<endl;

                        break;
                 }

                 case scannar::T_INT:
            {
                        tokenType=scannar::T_INT;
                        temp=i;
                        //ignores the type int and goes straight to the id type
                        i=findSemiColonStatement(i+1);
                       // cout<<"value is "<<value<<" i is before add ="<<i<<endl;

                     //   cout<<"!!!i after add T_INT token is  "<<code.at(i).tokens<<"next var is "<<code.at(i+1).variable<<endl;
                        if(error!=0)
                        {
                           // errorLine=parseTokenizer.at(temp).lineNum;
                         //   cout<<"error is"<<error<<endl;
                            return error;
                        }
                      //  cout<<"i is "<<i<<endl;

                        break;
            }

                 case scannar::T_CHAR:
                            {
                                tokenType=scannar::T_CHAR;
                                temp=i;
                                //ignores the char and goes straight to the id type
                                i=findSemiColonStatement(i+1);
                                //cout<<"token is "<<parseTokenizer.at(i).tokens<<" i is before add ="<<i<<endl;

                             //   cout<<"i after add is "<<i<<endl;
                                if(error!=0)
                                {
                                    //errorLine=parseTokenizer.at(temp).lineNum;
                               //     cout<<"error is"<<error<<endl;
                                    return error;
                                }
                             //   cout<<"i is "<<i<<endl;

                                break;
                         }

                 case scannar::T_STR:
                    {
                        tokenType=scannar::T_STR;
                        temp=i;
                        //ignores the str part and goes straight to the id part
                        i=findSemiColonStatement(i+1);
                       // cout<<"value is "<<value<<" i is before add ="<<i<<endl;

                      //  cout<<"i after add is "<<i<<endl;
                        if(error!=0)
                        {
                           // errorLine=parseTokenizer.at(temp).lineNum;
                         //   cout<<"error is"<<error<<endl;
                            return error;
                        }
                      //  cout<<"i is "<<i<<endl;

                        break;
                 }
                        //break;
                 case scannar::T_OP:
                       // errorLine=parseTokenizer.at(i).lineNum;
                        return 27;
                        break;
                 case scannar::T_FSLASH:
                      //  errorLine=parseTokenizer.at(i).lineNum;
                        return 37;

                 case scannar::T_BSLASH:
                //invalid character or comment
                       //errorLine=parseTokenizer.at(i).lineNum;
                       return 37;

                 case scannar::T_DOP:
                      //  errorLine=parseTokenizer.at(i).lineNum;
                        return 37;

                //no occasion where the literals should be assessed
                //here it should be assigned to a variable or in a loop
                 case scannar::T_CHARLit:
                       // errorLine=parseTokenizer.at(i).lineNum;
                        return 30;

                 case scannar::T_STRLit:
                       // errorLine=parseTokenizer.at(i).lineNum;
                        return 30;

                 case scannar::T_DECLit:
                        //errorLine=parseTokenizer.at(i).lineNum;
                        return 30;

                 case scannar::T_INTLit:
                      //  errorLine=parseTokenizer.at(i).lineNum;
                        return 30;

                 case scannar::T_ID:
            {
                //declaring the variable
                       // cout<<"in T_Id"<<endl;

                        tokenType=scannar::T_ID;
                        temp=i;
                        i=findSemiColonStatement(i);
                       // cout<<"value is "<<value<<" i is before add ="<<i<<endl;

                      //  cout<<"~~~i after add is in T_ID "<<i<<"next value is "<<code.at(i+1).variable<<endl;
                        if(error!=0)
                        {
                           // errorLine=parseTokenizer.at(temp).lineNum;
                          //  cout<<"error is"<<error<<endl;
                            return error;
                        }
                       // cout<<"i is "<<i<<endl;
                       /* int sizeDq=dequePtr.size();
                        int k=1;//just a loop variable
                        //loop checks if the block has ever been declared before by checking
                        //the "pointers"

                        while(k<=sizeDq&&dequePtr.at(sizeDq-k).block>blockData)
                        {
                            ++k;
                            cout<<"in while loop"<<endl;
                        }
                        cout<<"after while"<<endl;

                        //means the block has no variable prior to this
                        //also block size is smaller than the others
                        if(k>sizeDq)
                        {
                            cout<<"bigger size"<<endl;

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
                                 errorLine=parseTokenizer.at(i).lineNum;
                                 return errorState;
                             }

                        }
                        else
                        {
                            cout<<"in else"<<endl;
                            //some variables in that block has been declared
                            //so need to check those
                            //doesn't allow duplicate varible names in the
                            //same block lavel
                            if(dequePtr.at(sizeDq-k).block==blockData)
                            {
                                cout<<"in same block else if k= "<<k<<endl;
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
                                       parseTokenizer.at(i).variable)==0)

                                    {
                                        if(!checkType(parseTokenizer.at(i+2).tokens,parseTokenizer.at(i-1).tokens))
                                        {
                                               variableExists=true;
                                        }
                                        else
                                        {
                                        errorLine=parseTokenizer.at(i).lineNum;
                                        return 26;
                                        }

                                    }
                                    ++j;
                                }
                                //name doesn't exist
                                if(!variableExists){
                                    int errorState=addToEnvironmentTable(i,1);
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
                                        errorLine=parseTokenizer.at(i).lineNum;
                                        return errorState;
                                    }
                                }

                            }
                            //the whole block not been declared before
                            else
                            {
                                cout<<"wakaka"<<endl;
                                int getPosition=dequePtr.at(sizeDq-(k-1)).position;
                                int errorState=addToEnvironmentTable(i,1);
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
                        printEnv();*/
                        break;
            }
               default:
                   /* size_t start=data.length()-1;
                     if(data.substr(0,1).compare("\"")==0){
                         toks->tokens=T_STRLit;
                         toks->variable=data;
                         toks->lineNum=lineNumber;
                         tokenize.push_back(*toks);
                         cout<<"in \" of default "<<lineNumber<<endl;

                     }
                     else if(data.substr(0,1).compare("'")==0)
                     {
                         toks->tokens=T_CHARLit;
                         toks->variable=data;
                         toks->lineNum=lineNumber;
                         tokenize.push_back(*toks);
                         cout<<"in CHARlIT "<<lineNumber<<endl;
                        }
                     else if(atoi(data.c_str()))
                     {
                         toks->tokens=T_INTLit;
                         toks->variable=data;
                         toks->lineNum=lineNumber;
                         tokenize.push_back(*toks);
                         cout<<"in INTlIT "<<lineNumber<<endl;
                     }
                     else if(atof(data.c_str()))
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
                         toks->tokens=T_ID;
                         toks->variable=data;
                         toks->lineNum=lineNumber;
                         tokenize.push_back(*toks);
                         cout<<"in else its an ID "<<lineNumber<<endl;

                     }*/
                    //cout<<"in default parsing error "<<parseTokenizer.at(i).tokens<<endl;
                     break;

            }
        }
       /* if(countIf>0||countElse>0||countFor>0||countWhile>0)
        {
            cout<<"number of vector line "<<vectorSize+1<<endl;
            errorLine=vectorSize+1;
            return 18;
        }*/
       // cout<<"out of loop checkStructure()"<<endl;
        //int msgValue=0;
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
    //i=i+1;
    //cout<<"possible problem"<<endl;
    //printDq();

    int sizeDq=envPtr.size();
    int k=1;//just a loop variable
    //int test =envPtr.at(0).position;
  //  cout<<"test value is = "<<test<<endl;
   // bool variableExists=false;
    //loop checks if the block has ever been declared before by checking
    //the "pointers"


    int parentBlock;
   // cout<<"in lookUp real"<<endl;
    //cout<<"lookUp real token is "<<trackSemiColon.at(i).tokens<<endl;
    //the parent starts from the current block
    if(trackSemiColon.at(i).tokens==scannar::T_ID)
    {
       // bool inWhile=false;
      //  cout<<"parent Size"<<parent.size()<<endl;
        if(sizeDq!=0)
        {
            for(parentBlock=parent.size()-1;parentBlock>=0;parentBlock--)
            {
                //k is a loop variable
                k=1;
               // cout<<"num of parent= "<<parent.size()<<endl;
               // cout<<"parent block ="<<parentBlock<<endl;
               // cout<<"parent is ---------->"<<parent.at(parentBlock)<<endl;
                while(k<=sizeDq&&envPtr.at(sizeDq-k).block!=parent.at(parentBlock))
                {
                   // inWhile=true;
                   // cout<<"in while loop lOOKUP real sizeDq-k is "<<sizeDq-k<<endl;
                    //k is just alooping variable. It acts as a counter
                    ++k;
                }
                //cout<<"after while in lookUp real parentB= "<<parentBlock<<endl;

                //if the block is found then
                /*if(inWhile)
                {
                    cout<<"inWhile2"<<endl;
                    k-=1;
                }*/
               // cout<<"after inWhile 2 value of k is "<<k<<endl;
                //k is a looping variable. It acts as a counter
                if(k<=sizeDq&&k>0)
                {
                 //   cout<<"in k<=sizeDq if lookup real "<<k<<endl;

                    int getPos= envPtr.at(sizeDq-k).position;
                   // cout<<"get pos= "<<getPos<<endl;
                   // cout<<"env block value is "<<environment.at(getPos).block<<endl;

                    while(getPos<environment.size()&&
                          environment.at(getPos).block==parent.at(parentBlock))
                    {
                      //  cout<<"inwhile block==parentBlock if lookup real "<<getPos<<" environment size"<<environment.size()<<endl;

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
                          //  cout<<"same variable "<<environment.at(getPos).variable<<endl;
                            if(environment.at(getPos).value.length()>0)
                            {
                              //  cout<<"@#$%%%its not a blank "<<environment.at(getPos).value.length()<<"value is "<<environment.at(getPos).value<<endl;

                            //for 'a' in a=a+b;--> is an example only
                            if(i==0)
                            {
                                idType=environment.at(getPos).tokens;
                                idPos=getPos;
                               // cout<<"in lookUp() real i==0"<<endl;
                               // checked=true;
                                return true;
                            }
                            else
                            {
                                idPos=getPos;
                               // cout<<"variable at idPos is -------> "<<environment.at(idPos).variable<<" value is "<<environment.at(idPos).value<<endl;
                                //checks if the variable has been initialised before.
                                if(environment.at(getPos).init)
                                {
                                assignId=environment.at(getPos).tokens;
                                //cout<<"in if of lookup real of id init assignId"<<endl;
                               // cout<<"lookUp idType is "<<idType<<endl;
                                //IF IDtYPE INIT
                                /*if(countLoopVar==1)
                                {
                                    //most likely comes from checkLoop
                                  idType=assignId;
                                  return true;
                                }*/
                                 // return checkType(assignId,idType);
                                    return true;
                                }
                                else
                                {
                                 //   cout<<false;
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
   // cout<<"in getLoop vars block is "<<blockData<<endl;
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
   // cout<<i<<endl;
    //this part is just to get rid of the last bracket

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
    //parseStatement();
        //the 0 has no particular significance in this case
        //computeValues(0);
     //   cout<<"to compute Loop"<<endl;
        computeLoop(0);
    }
    else
    {
        errorLine=code.at(temp).lineNum;
        error= 27;
    }
   // printTrackSemiColon();
   // cout<<"find ; tok= "<<parseTokenizer.at(i).tokens<<endl;
    return i;
   // return 0;
}
int codegenerator::findSemiColonStatement(int i)
{
    //clear old data first
    trackSemiColon.clear();
    loop=0;
    mulDiv=false;
    addMinus=false;
  //  bool checked=false;
    int temp=i;
    int openBracket=0;
    //get upto but not including the ';'
   // cout<<"in find semi colon"<<endl;

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
   // cout<<i<<"semicolon is "<<trackSemiColon.size()<<endl;
    if(trackSemiColon.size()<3)
    {
        return i;
    }
    //this part is just to get rid of the last bracket
    if(trackSemiColon.at(0).tokens==scannar::PRINT)
    {
        trackSemiColon.pop_back();
        //cout<<"in find semi colon print "
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
    //parseStatement();
        //the 0 has no particular significance in this case
        computeValues(0);
    }
    else
    {
        errorLine=code.at(temp).lineNum;
        error= 27;
    }
   // printTrackSemiColon();
   // cout<<"find ; tok= "<<parseTokenizer.at(i).tokens<<endl;
    return i;
    //return 0;
}
//int i is just 0
//all values are redirected to be computed here
int codegenerator::computeValues(int i=0)
{
    //cout<<"tokenType at compute values are "<<tokenType<<endl;
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
                   // cout<<"tokenType is if"<<endl;
                    lookUpVars(i);
                    //tokenType=environment.at(idPos).tokens;
                   // cout<<"in if of compute values------------------> "<<environment.at(idPos).value<<endl;
                    trackSemiColon.at(i).variable=environment.at(idPos).value;
                    trackSemiColon.at(i).tokens=environment.at(idPos).tokens;
                 //   cout<<"token type in if compute values is "<<tokenType<<endl;
                }

            }
            if(trackSemiColon.at(2).variable.compare("-")==0)
            {
                string concat=checkNegation(2);
                //string concat =trackSemiColon.at(2).variable + trackSemiColon.at(3).variable;
                //cout<<"concated value in compute if "<<concat<<endl;
                trackSemiColon.at(2).variable=concat;
                trackSemiColon.at(2).tokens=scannar::T_INT;
                //just ftm
               // trackSemiColon.pop_back();
                //this gets rid of the 4th element if counting from base 1 ie 1,2...
                trackSemiColon.erase(trackSemiColon.begin()+3);
               /* for(int j=0; j<trackSemiColon.size();j++)
                {
                    cout<<"token is first 1800 "<<trackSemiColon.at(j).tokens<<"variable is "<<trackSemiColon.at(i).variable<<endl;


                }*/
            }
            for(int i=2;i<trackSemiColon.size();i++)
            {
                if(trackSemiColon.at(i).tokens==scannar::EQUALS
                        &&trackSemiColon.at(i+1).tokens==scannar::EQUALS)
                {

                    ++i;
                    //cout<<"hoooooooppppppppp"<<trackSemiColon.at(i).tokens<<endl;
                }
                if(trackSemiColon.at(i).tokens==scannar::T_COP
                        ||trackSemiColon.at(i).variable.compare("!=")==0
                        ||trackSemiColon.at(i).tokens==scannar::EQUALS)
                {
                    //cout<<"next value is a negation "<<trackSemiColon.at(i+1).tokens<<trackSemiColon.at(i+1).variable<<endl;
                   if(trackSemiColon.at(i+1).variable.compare("-")==0)
                   {
                      // cout<<"fine"<<endl;
                       string concat=checkNegation(i+1);
                       //string concat =trackSemiColon.at(i+1).variable+trackSemiColon.at(i+2).variable;
                       trackSemiColon.at(i+1).variable=concat;
                       trackSemiColon.at(i+1).tokens=trackSemiColon.at(i+2).tokens;
                     //  cout<<"wooop"<<endl;
                       //just ftm
                      // trackSemiColon.pop_back();
                       //this gets rid of the 4th element if counting from base 1 ie 1,2...
                       trackSemiColon.erase(trackSemiColon.begin()+(i+2));
                      /* for(int j=0; j<trackSemiColon.size();j++)
                       {
                           cout<<"token is "<<trackSemiColon.at(j).tokens<<"variable is "<<trackSemiColon.at(i).variable<<endl;


                       }*/
                       --i;
                   }

                }

            }

                tokenType=trackSemiColon.at(2).tokens;

          //  cout<<"back;";
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
             // displayResult->insertPlainText(QString::fromStdString(value));
            //type str would be correct cos it inputs a str
            trackSemiColon.at(2).variable=value;
            trackSemiColon.at(2).tokens=tokenType;
            print(value);
            //reducing the size by 2 since the last two are just brackets
            trackSemiColon.pop_back();
            trackSemiColon.pop_back();
          //  cout<<"read size is "<<trackSemiColon.size();
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
                    //string concat =trackSemiColon.at(2).variable + trackSemiColon.at(3).variable;
                   // cout<<"concated value in compute Int is "<<concat<<endl;
                    trackSemiColon.at(2).variable=concat;
                    trackSemiColon.at(2).tokens=scannar::T_INT;
                    //just ftm
                   // trackSemiColon.pop_back();
                    //this gets rid of the 4th element if counting from base 1 ie 1,2...
                    trackSemiColon.erase(trackSemiColon.begin()+3);
                  //  cout<<"2nd element is "<<trackSemiColon.at(2).variable<<endl;

                }
            }
            //if there is a multiply or divide
            if(mulDiv)
            {
                //starts from 2 because a=1+2... is 0,1,2,3,4
                //this part is to do all mul and divs
                for(int pos=2;pos<trackSemiColon.size();pos++)
                {
                 //   cout<<"pos is "<<pos<<endl;
                 //   cout<<"trackSemiColon size is "<<trackSemiColon.size()<<endl;
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
                      /*  for(int i=0;i<trackSemiColon.size();i++)
                        {
                            cout<<"semicolon values are "<<trackSemiColon.at(i).variable<<endl;

                        }*/
                        //test only
                      //  trackSemiColon.clear();

                    }
                    //just testing this out
                   // trackSemiColon.pop_back();

                }
            }
            //for addition and subtraction
             if(addMinus)
             {
               // cout<<"in else line 912 of add and subs of int"<<endl;
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
                            //    cout<<"in finding denom in add........"<<endl;
                                lookUpVars(pos+1);
                                denominator=environment.at(idPos).value;
                              //  cout<<"denom value is "<<denominator<<endl;
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
              /*  for(int i=0;i<trackSemiColon.size();i++)
                {
                    cout<<"semicolon values in else line 957 are "<<trackSemiColon.at(i).variable<<endl;

                }*/
                //cout<<"out of for loop line 1186"<<endl;

            }
     //char type
    if(tokenType==scannar::T_CHAR||tokenType==scannar::T_CHARLit)
    {
       // cout<<"str"<<endl;
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
          //  cout<<"char is "<<value<<endl;
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
                  //  cout<<"char is "<<value<<endl;
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
              //  cout<<"found is in strLit 1055 "<<found<<endl;
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
                                //cout<<"found is in strLit 1055 "<<found<<endl;
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
                              //  cout<<"value2 is "<<value2<<endl;
                                size_t found= value2.find_last_of("\"");
                              //  cout<<"found is in strLit 1055 "<<found<<endl;
                                value2=value2.substr(1,found-1);
                             //   cout<<"value oof str is after getting rid of quotes is "<<value2<<endl;

                            }
                            else if(trackSemiColon.at(pos+1).tokens==scannar::T_CHARLit)
                            {
                                value2=trackSemiColon.at(pos+1).variable;
                                //gets rid of the ' ' open single qoutes part.
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
                            // cout<<"concated values 1115 are "<<value<<endl;

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
                                //cout<<"found is in strLit 1055 "<<found<<endl;
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
    //debugging only
  /*  for(int test=0;test<trackSemiColon.size();test++)
    {
        cout<<"-----------------------------------------------<<>>the values are in compute values "<<trackSemiColon.at(test).variable<<endl;
    } */

    //end of debugging code

    //overhere is the code to update the env table
    if(trackSemiColon.size()==3)
    {
        if(trackSemiColon.at(0).tokens!=scannar::PRINT)
        {
            //cout<<"in compute Values size is 3"<<endl;
            //getting the idPos for the first variable

            lookUpVars(0,1);
            updateEnvTable(idPos);
        }
        if(trackSemiColon.at(0).tokens==scannar::PRINT)
        {
            print(trackSemiColon.at(2).variable);
          /*  size_t find;
            find=0;
            string newLine=trackSemiColon.at(2).variable;
            int firstEntry=0;
            while(find!=string::npos)
            {

                cout<<"in here"<<endl;
                      if(firstEntry!=0)
                      {
                      find=newLine.find("\\",find+1);

                      }
                      else
                      {
                          find=newLine.find("\\");


                      }
                      //cout<<"length is "<<name.length()<<endl;
                      if(find!=string::npos)
                      {
                          string next=newLine.substr(find+1,1);
                        if(next.compare("n")==0)
                        {
                        newLine.replace(find,2,"\n");
                        cout<<"newline is "<<newLine<<endl;
                        }
                        if(next.compare("t")==0)
                        {
                            newLine.replace(find,2,"\t");
                        }
                      }
                      firstEntry=1;
            }
           // string newLine=trackSemiColon.at(2).variable+"\n";
            newLine=newLine+"\n";
            displayResult->insertPlainText(QString::fromStdString(newLine));
            */
        }

    }
    return 0;

}
//int i is the value for the position of the variable in the envTable as received by the idPos
void codegenerator::updateEnvTable(int i)
{
 //   cout<<"806 env is "<<environment.at(i).value<<endl;
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
   // displayResult->insertPlainText(QString::fromStdString(value));
  //  cout<<"in 924 updateEnvTable value is "<<environment.at(i).value<<" variable is "<<environment.at(i).variable<<" i value is "<<i<<endl;
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
  //  cout<<"value is "<<value<<endl;
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
    //cout<<":::::::::::: in add values are "<<numerator<<" second is "<<"denominator"<<endl;
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
    //cout<<"value is "<<value<<endl;
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
       // cout<<"scannar point!npos isdec"<<endl;
        size_t numerator = data.substr(0,point).find_first_not_of(regex);
        size_t denominator= data.substr(point+1).find_first_not_of(regex);
      //  cout<<"numerator= "<<numerator<<endl;
      //  cout<<"denom= "<<denominator<<endl;
        string num1=data.substr(0,point);
        string num2=data.substr(point+1);
      //  cout<<"num2 is "<<num2.c_str()<<" length is "<<num2.length();
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
       //cout<<"after numerator for decimal "<<endl;
       if(denominator<num2.length())
       {
         //  cout<<"not npos isDec"<<endl;
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
   // cout<<"number is "<<number<<endl;
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

        //cout<<"in here"<<endl;
              if(firstEntry!=0)
              {
              find=newLine.find("\\",find+1);

              }
              else
              {
                  find=newLine.find("\\");


              }
              //cout<<"length is "<<name.length()<<endl;
              if(find!=string::npos)
              {
                  string next=newLine.substr(find+1,1);
                if(next.compare("n")==0)
                {
                newLine.replace(find,2,"\n");
               // cout<<"newline is "<<newLine<<endl;
                }
                if(next.compare("t")==0)
                {
                    newLine.replace(find,2,"\t");
                }
              }
              firstEntry=1;
    }
   // string newLine=trackSemiColon.at(2).variable+"\n";
    newLine=newLine+"\n";
    displayResult->insertPlainText(QString::fromStdString(newLine));
}
int codegenerator::computeLoop(int k)
{
    int pos=0;
    bool first=true;
   // cout<<"in compute loop"<<endl;
    while(pos<=loopVars.size())
    {
        //looping variable i
        int i=0;
        int size=0;
        addMinus=false;
        mulDiv=false;
        trackSemiColon.clear();
       // cout<<"into the first while pos<loopSize"<< trackSemiColon.size()<<endl;
        while(pos<loopVars.size()
              &&loopVars.at(pos).tokens!=scannar::T_DOP)
        {
          //  cout<<"pos b4 "<<pos<<endl;
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
          // cout<<"--------------------value pushed into track---------"<<loopVars.at(pos).variable<<endl;
            trackSemiColon.push_back(loopVars.at(pos));
            //cout<<"trackSemicolon size in compute loop "<<trackSemiColon.size()<<"loopVars size is "<<loopVars.size()<<endl;
            ++pos;
            ++size;
          //  cout<<"pos is "<<pos<<endl;
        }
        //need to increment it by an extra 1 because the exit condition is the T_DOP or RBR
       /* if(pos<loopVars.size())
        {
        cout<<"value here is--------------------------"<<loopVars.at(pos).variable<<endl;
        }*/
        ++pos;



        computeValues(0);
       // cout<<"out of second while and into compute values"<<endl;
       /* for(int i=0;i<trackSemiColon.size();i++)
        {
            cout<<"loop vars here 1806 after compute values in compute loop are "<<trackSemiColon.at(i).variable<<endl;

        }*/
       // cout<<"at 2 value is "<<trackSemiColon.at(2).variable<<endl;
        //cout<<"at 5 variable is "<<trackSemiColon.at(5).variable<<endl;
       // cout<<"in compute loop after returning from compute values"<<endl;
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

               // cout<<"in true false "<<pos-size+1<<"pos ="<<pos<<endl;
              //  cout<<"size of loopVar is "<<loopVars.size()<<endl;
                loopVars.at(pos-size+1).variable="true";

            }
            else
            {
                 loopVars.at(pos-size+1).variable="false";
            }
            loopVars.at(pos-size+1).tokens=scannar::T_STR;
           // cout<<"here??"<<endl;

            loopVars.erase(loopVars.begin()+(pos-size+2),loopVars.begin()+(pos-1));


           // loopVars.erase(loopVars.begin()+(pos-size+2),loopVars.begin()+(pos-1));

            //should be either 2 or 3
            pos=pos-size+3;
            //need to transfer back to scannar::IF
            tokenType=tempToken;
            first=false;
           // cout<<"loop vars size after reduction "<<loopVars.size()<<endl;
           // cout<<"new pos value is "<<pos<<endl;
            //debugging only
           /* if(pos<loopVars.size())
            {
            cout<<"value after pos change is "<<loopVars.at(pos).variable<<endl;
            }*/
        }
    }
   // cout<<"size of loop vars is "<<loopVars.size()<<endl;
    /*for(int i=0;i<loopVars.size();i++)
    {
        cout<<"loop vars here 1858 are "<<loopVars.at(i).variable<<endl;

    }*/
    if(loopVars.at(loopVars.size()-1).tokens==scannar::T_RBR)
    {
        loopVars.pop_back();
    }
    //will actually be 4 if including the close bracket
    //debugging
    /*if(loopVars.size()>3)
       {
    cout<<"token type is "<<loopVars.at(3).tokens<<endl;
    } */
    while(loopVars.size()>3)
    {
       // cout<<"in 3rd while loop"<<endl;
        for(int pos=0;pos<loopVars.size();pos++)
        {
            //cout<<"in for loop "<<endl;

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
               // cout<<"size is "<<loopVars.size()<<endl;
                loopVars.erase(loopVars.begin()+pos,loopVars.begin()+plus2);
               // cout<<"size after reduction "<<loopVars.size()<<endl;
               --pos;
                /*for(int k=0;k<loopVars.size();k++)
                {
                    cout<<"loop vars at || or && are "<<loopVars.at(k).variable<<endl;
                }*/
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
   // cout<<"first value is "<<trackSemiColon.at(2).variable<<"second value is "<<trackSemiColon.at(5).variable;
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
    //cout<<"*** in oR OPretator ***"<<endl;
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
