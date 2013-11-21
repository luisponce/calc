#include "calculator.h"
#include "parser.h"
#include "ast.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>

Calculator::Calculator():
  memory(0),
  interactiveOn(false)
{}

int Calculator::eval(string expr) {

   Parser* parser = new Parser(new istringstream(expr));
   
   AST* tree = parser->parse();
   
   int result = tree->evaluate();
   
   delete tree;
   
   delete parser;
   
   return result;
}

void Calculator::compile(string expr) {

   Parser* parser = new Parser(new istringstream(expr));
   
   AST* tree = parser->parse();
   
   remove("a.ewe");
   
   string ans = "start:\none:=1\ntwo:=2\nsp:=6\n";

   ans = ans + tree->compile();

   ans+="tmp:=M[sp+0]\nwriteInt(tmp)\nend: halt\n";
   ans+="equ one M[0]\nequ two M [1]\nequ tmp M[2]\n";
   ans+="equ tmp2 M [3]\nequ sp M[4]\nequ store M[5]\n";
   ans+="equ stack M[6]\n";

   ofstream out;
   out.open("a.ewe");
   out << ans;
   out.close();
   
   delete tree;
   
   delete parser;
}


void Calculator::store(int val) {
   memory = val;
}

int Calculator::recall() {
   return memory;
}

bool Calculator::isInteractiveOn(){
  return interactiveOn;
}

void Calculator::turnInteractiveOn(){
  interactiveOn=true;
}
