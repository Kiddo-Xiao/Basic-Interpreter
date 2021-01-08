/*
 * File: parser.cpp
 * ----------------
 * Implements the parser.h interface.
 */

#include <iostream>
#include <string>

#include "exp.h"
#include "parser.h"

#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/strlib.h"
#include "../StanfordCPPLib/tokenscanner.h"
using namespace std;

/*
 * Implementation notes: parseExp
 * ------------------------------
 * This code just reads an expression and then checks for extra tokens.
 */

Expression *parseExp(TokenScanner & scanner) {
   Expression *exp = readE(scanner);
   if (scanner.hasMoreTokens()) {
      error("parseExp: Found extra token: " + scanner.nextToken());
   }
   return exp;
}

/*
 * Implementation notes: readE
 * Usage: exp = readE(scanner, prec);
 * ----------------------------------
 * This version of readE uses precedence to resolve the ambiguity in
 * the grammar.  At each recursive level, the parser reads operators and
 * subexpressions until it finds an operator whose precedence is greater
 * than the prevailing one.  When a higher-precedence operator is found,
 * readE calls itself recursively to read in that subexpression as a unit.
 */

Expression *readE(TokenScanner & scanner, int prec) {
    //新优先级大继续往后读直到遇到优先级相对之前小的停下给之前优先级大的套括号；
    //如果新优先级一直从小到大，则到最后先算最后一步然后退上一步（递归
   Expression *exp = readT(scanner);
   string token;
   while (true) {
      token = scanner.nextToken();
      int newPrec = precedence(token);
      if (newPrec <= prec) break;
      Expression *rhs = readE(scanner, newPrec);
      exp = new CompoundExp(token, exp, rhs);
   }
   scanner.saveToken(token);
   return exp;
}

/*
 * Implementation notes: readT
 * ---------------------------
 * This function scans a term, which is either an integer, an identifier,
 * or a parenthesized subexpression.
 */

Expression *readT(TokenScanner & scanner) {//推进 读单个表达式
   string token = scanner.nextToken();
   TokenType type = scanner.getTokenType(token);
   if (type == WORD) return new IdentifierExp(token);
   if (type == NUMBER) return new ConstantExp(stringToInteger(token));
   if (token != "(") error("Illegal term in expression");
   Expression *exp = readE(scanner);//prec=0无优先级
   if (scanner.nextToken() != ")") {
      error("Unbalanced parentheses in expression");
   }
   return exp;
}

/*
 * Implementation notes: precedence
 * --------------------------------
 * This function checks the token against each of the defined operators
 * and returns the appropriate precedence value.
 */

int precedence(string token) {
   if (token == "=") return 1;
   if (token == "+" || token == "-") return 2;
   if (token == "*" || token == "/") return 3;
   return 0;
}

Statement *parse_sta(TokenScanner & scanner,string line) {
    string token = scanner.nextToken();
    TokenType type = scanner.getTokenType(token);
    Expression *exp = nullptr;

    if (type != WORD) {//type应当为word指令，如LET
        error("SYNTAX ERROR");
    } else if (token == "INPUT") {//INPUT a;
        if (!scanner.hasMoreTokens()) { error("SYNTAX ERROR"); }
        token = scanner.nextToken();
        type = scanner.getTokenType(token);
        if (type != WORD)error("SYNTAX ERROR");
        if (scanner.hasMoreTokens())error("SYNTAX ERROR");
        return new INPUT_statement(token);//INPUT_statement(a);
    } else if (token == "PRINT") {
        exp = parseExp(scanner);
        if (exp->getType() == COMPOUND) {
            if (((CompoundExp *) exp)->getOp() == "=") {
                delete exp;
                error("SYNTAX ERROR");
            }
        }
        return new PRINT_statement(exp);
    }
    else if (token == "REM")return new REM_statement();
    else if(token=="END")return new END_statement();
    else if(token=="LET"){
        if(!scanner.hasMoreTokens())error("SYNTAX ERROR");
        exp=parseExp(scanner);//读取表达式
        if(exp->getType()!=COMPOUND){delete exp;error("SYNTAX ERROR");}
        if(((CompoundExp *)exp)->getOp() != "=" ||
           ((((CompoundExp *)exp)->getLHS())->getType() != IDENTIFIER) ||//LHS 左值；IDENTIFIER 变量
           (!check(((IdentifierExp *)(((CompoundExp *)exp)->getLHS()))->getName()))){delete exp;error("SYNTAX ERROR");}
        return( new LET_statement(exp));
    }
    else if(token == "GOTO"){
        if(!scanner.hasMoreTokens())error("SYNTAX ERROR");
        token = scanner.nextToken();
        type = scanner.getTokenType(token);
        if(type != NUMBER)error("SYNTAX ERROR");
        if(scanner.hasMoreTokens())error("SYNTAX ERROR");
        int to_line;
        try{
            to_line = stringToInteger(token);//str token转int
        } catch (...) {//无to_line
            error("SYNTAX ERROR");
        }
        return new GOTO_statement(to_line);
    }
    else if(token=="IF"){//IF(a comp b) THEN 1
        if (!scanner.hasMoreTokens()) error("SYNTAX ERROR");
        string comp;//比较操作符
        Expression *fir,*sec;
        GOTO_statement *to_line;
        if(line.find('=')==string::npos){//没有=
            fir=readE(scanner);
            comp=scanner.nextToken();
            if(comp!="<"&&comp!=">"&&comp!="=")delete fir,error("SYNTAX ERROR");
            try{
                sec=readE(scanner);
            }catch(...){delete fir,error("SYNTAX ERROR");}

            token=scanner.nextToken();
            if(token!="THEN")delete fir,error("SYNTAX ERROR");
            token=scanner.nextToken();
            type=scanner.getTokenType(token);
            if(type!=NUMBER) delete fir,delete sec,error("SYNTAX ERROR");
            if(!scanner.hasMoreTokens())delete fir,delete sec,error("SYNTAX ERROR");

            int _line;
            try{
                _line=stringToInteger(token);
            } catch (...) {
                delete fir,delete sec;
                error("SYNTAX ERROR");
            }
            to_line=new GOTO_statement(_line);
            return new IF_statement(comp,fir,sec,to_line);
        }
        else{
            comp="=";
            string substring;
            while(scanner.hasMoreTokens()){
                token=scanner.nextToken();
                if(token=="=")break;
                substring+=token;//abc = 1;substring=a+b+c=abc;
            }
            sec=readE(scanner);
            token=scanner.nextToken();
            if(token!="THEN")delete fir,delete sec,error("SYNTAX ERROR");
            token=scanner.nextToken(),type=scanner.getTokenType(token);
            if(type!=NUMBER)delete fir,delete sec,error("SYNTAX ERROR");
            if(scanner.hasMoreTokens())delete fir,delete sec,error("SYNTAX ERROR");

            int line;
            try {
                line = stringToInteger(token);
            } catch (...) {
                delete fir,delete sec;
                error("SYNTAX ERROR");
            }

            scanner.setInput(substring);
            try{fir=readE(scanner);}
            catch(...){delete fir,delete sec,error("SYNTAX ERROR");}

            to_line=new GOTO_statement(line);
            return new IF_statement(comp,fir,sec,to_line);
        }
    }
    else {error("SYNTAX ERROR");}
}

bool check(string token){
    if(token == "LET" || token == "REM" || token == "PRINT" || token == "END" ||
    token == "IF" ||token == "THEN" || token == "GOTO" || token == "RUN" ||
    token == "LIST" || token == "CLEAR" ||token == "QUIT" || token == "HELP"){
        return false;
    }
    else return true;
}