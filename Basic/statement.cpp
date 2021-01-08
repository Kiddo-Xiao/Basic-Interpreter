/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"
#include "../StanfordCPPLib/simpio.h"
using namespace std;

/* Implementation of the Statement class */

Statement::Statement() {
   /* Empty */
}

Statement::~Statement() {
   /* Empty */
}

REM_statement::REM_statement() {} ;
REM_statement::~REM_statement() {} ;
void REM_statement::execute(EvalState &state) {} ;

INPUT_statement::INPUT_statement(string &a) { name = a ; }//变量名为a
INPUT_statement::~INPUT_statement() {} ;

void INPUT_statement::execute(EvalState &state) {
    int tmp=0;
    string value;
    while(true){
        try{
            value=getLine(" ? ");
            tmp=stringToInteger(value);
        }catch(...){//all error
            cout<<"INVALID NUMBER"<<endl;
            continue;
        }
        break;
    }
    state.setValue(name,tmp);
}

PRINT_statement::PRINT_statement(Expression *a){exp = a ;}
PRINT_statement::~PRINT_statement() { delete exp ; }
void PRINT_statement::execute(EvalState &state) { cout << exp->eval(state) << endl ; }//eval()可=赋值，也可+-*/返回运算结果

END_statement::END_statement() {} ;
END_statement::~END_statement() {} ;
void END_statement::execute(EvalState &state) { error("[tag] end") ; }//throw"xxxERRORxxx!"

LET_statement::LET_statement( Expression *a ) { exp = a ; }
LET_statement::~LET_statement() { delete exp ; }
void LET_statement::execute(EvalState &state) { exp->eval(state) ; }

GOTO_statement::GOTO_statement( int a ): to_line(a){}
GOTO_statement::~GOTO_statement() {} ;
void GOTO_statement::execute(EvalState &state) { error( integerToString(to_line) ) ; }

IF_statement::IF_statement(string &a, Expression *b, Expression *c, GOTO_statement *d) : comp(a) , fir(b) , sec(c) , to_line(d) {} ;
IF_statement::~IF_statement(){
    delete fir ;
    delete sec ;
    delete to_line ;
}
void IF_statement::execute(EvalState &state){
    bool flag = 0 ;
    int real_first = fir->eval(state) , real_second = sec->eval(state) ;
    if ( comp == "=" ) flag = ( real_first == real_second ) ;
    if ( comp == "<" ) flag = ( real_first < real_second ) ;
    if ( comp == ">" ) flag = ( real_first > real_second ) ;
    if ( !flag ) return ;
    else to_line->execute(state) ;//if成立，转GOTO.execute
}
