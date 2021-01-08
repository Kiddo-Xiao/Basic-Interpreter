/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "../StanfordCPPLib/error.h"
using namespace std;

Program::Program()=default;

Program::~Program(){this->clear();}

void Program::clear() {
   allpro.erase(allpro.begin(),allpro.end());
}

void Program::addSourceLine(int lineNumber, string line) {
   proline tmp=line;
   allpro[lineNumber]=tmp;//用map结合linenumber&string statement
}

void Program::removeSourceLine(int lineNumber) {
   if(!allpro.count(lineNumber))return;
   delete allpro[lineNumber].new_line;
   allpro[lineNumber].new_line=nullptr;
   allpro.erase(lineNumber);
}

string Program::getSourceLine(int lineNumber) {
   return allpro[lineNumber].old_line; //返回初始程序
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
   allpro[lineNumber].new_line=stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
   return allpro[lineNumber].new_line;//返回翻译后statement
}

int Program::getFirstLineNumber() {
   if(allpro.empty()) return -1;
   return allpro.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
   auto it=allpro.find(lineNumber);//it为map里一个指针，first为map左类型，second右
   it++;
   if(it==allpro.end())return -1;
   else return it->first;
}

void Program::run(EvalState &a){
    if(allpro.empty()) return;
    auto tmp=allpro.begin();
    while(tmp!=allpro.end()){
        try{
            (tmp->second.new_line)->execute(a);
            tmp++;
        }catch( ErrorException &ex){//异常
            TokenScanner scanner;
            if(scanner.getTokenType(ex.getMessage())==NUMBER){
                int num=stringToInteger(ex.getMessage());//获得linenum
                if(allpro.count(num)){
                    tmp=this->allpro.find(num);
                    continue;
                }
                error("LINE NUMBER ERROR");//该行不存在
            }
            else error(ex.getMessage());
        }
    }
}
void Program::list(){
    if(allpro.empty())return ;
    auto tmp=allpro.begin();
    while(tmp!=allpro.end()){
        cout<<tmp->second.old_line<<endl;
        tmp++;
    }
}