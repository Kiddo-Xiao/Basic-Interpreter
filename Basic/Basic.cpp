/*
 * File: Basic.cpp
 * ---------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "statement.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */

void processLine(string line, Program & program, EvalState & state);

/* Main program */

int main() {
   EvalState state;
   Program program;
   //cout << "Stub implementation of BASIC" << endl;
   while (true) {
      try {
         processLine(getLine(), program, state);
      } catch (ErrorException & ex) {
          cout << ex.getMessage() << endl;
      }
   }
   return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(string line, Program & program, EvalState & state) {
   TokenScanner scanner;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line);

   string token;
   if(scanner.hasMoreTokens())token=scanner.nextToken();//此时为第一个token
   else return;
   TokenType type=scanner.getTokenType(token);

   if(type==NUMBER){
       int linenum=stringToInteger(token);
       if(linenum<=0)throw ErrorException("LINE ERROR");
       if(scanner.hasMoreTokens()){
           try{
               Statement *sta=parse_sta(scanner,line);
               program.addSourceLine(linenum,line);
               program.setParsedStatement(linenum,sta);
           }catch(ErrorException &ex){
               program.removeSourceLine(linenum);
               throw ErrorException(ex.getMessage());
           }
       }
       else program.removeSourceLine(linenum);//只有linenum——删除行
       return ;
   }

   if(token=="LET"){
       string name=scanner.nextToken();
       if(!check(name))cout<<"SYNTAX ERROR"<<endl;
       scanner.nextToken();
       Expression *exp=parseExp(scanner);
       int value=exp->eval(state);//a->eval(statement)返回值
       state.setValue(name,value);
       delete exp;
       return ;
   }
   if(token=="INPUT"){
       string name=scanner.nextToken(), str_value;;
       if(!check(name)) throw ErrorException("SYNTAX ERROR");
       int value;
       if(scanner.hasMoreTokens()) throw ErrorException("SYNTAX ERROR");
       while(true){
           try{
               str_value=getLine(" ? ");
               value=stringToInteger(str_value);
           }catch(...){
               cout<<"INVALID NUMBER"<<endl;
               continue;
           }
           break;
       }
       state.setValue(name,value);
       return;
   }
    if ( token == "PRINT" )
    {
        Expression *exp = parseExp(scanner) ;
        cout << exp->eval(state) << endl ;
        delete exp ;
        return ;
    }
    //state存储器
    if(token=="LIST"){program.list();return;}
    if(token=="CLEAR"){program.clear();state.clear();return ;}
    if(token=="QUIT")exit(0);
    if(token=="RUN"){
        try{
            program.run(state);
        }catch(ErrorException &ex){
            if(ex.getMessage()=="[tag] end")return ;
            else error(ex.getMessage());
        }
        return ;
    }
    throw ErrorException( "SYNTAX ERROR" ) ;
}
