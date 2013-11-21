#include "parser.h"
#include "calcex.h"
#include "calculator.h"
#include <string>
#include <sstream>

Parser::Parser(istream* in) {
   scan = new Scanner(in);
}

Parser::~Parser() {
   try {
      delete scan;
   } catch (...) {}
}

AST* Parser::parse() {
   return Prog();
}

AST* Parser::Prog() {
   AST* result = Expr();
   Token* t = scan->getToken();

   if (t->getType() != eof) {
     calc->isInteractiveOn()? cout<<"* " : cout<<"" ; 
     cout << "Syntax Error: Expected EOF, found token at column "
	   << t->getCol() << endl;
      throw ParseError;
   }

   return result;
}

AST* Parser::Expr() {
   return RestExpr(Term());
}

AST* Parser::RestExpr(AST* e) {
   Token* t = scan->getToken();

   if (t->getType() == add) {
      return RestExpr(new AddNode(e,Term()));
   }

   if (t->getType() == sub)
      return RestExpr(new SubNode(e,Term()));

   scan->putBackToken();

   return e;
}

AST* Parser::Term() {
  return RestTerm(Storable());
}

AST* Parser::RestTerm(AST* e) {
   Token* t = scan->getToken();

   if (t->getType() == times) {
      return RestTerm(new MulNode(e,Storable()));
   }

   if (t->getType() == divide)
      return RestTerm(new DivNode(e,Storable()));

   scan->putBackToken();

   return e;
}

AST* Parser::Storable() {
  AST* factor = Factor();
  AST* ret;

  Token* t = scan->getToken();

  if(t->getType() == keyword){
    if(t->getLex() == "S" || t->getLex() == "s"){
      ret = new StoreNode(factor);
    } else {
      calc->isInteractiveOn()? cout<<"* " : cout<<"" ;
      cout<<"Syntax Error: expected S, found token at column "
	  << t->getCol() <<endl;
      throw ParseError;
    }
  } else {
    scan->putBackToken();
    ret = factor;
  }

  return ret;
}

AST* Parser::Factor() {
  
  AST* ret;
  Token* t = scan->getToken();

  if (t->getType() == number) {
      istringstream in(t->getLex());
      int val;
      in >> val;
      ret = new NumNode(val);
  }
  else if(t->getType() == keyword){
    if(t->getLex() == "R" || t->getLex() == "r") 
      ret = new RecallNode();
    else {
      calc->isInteractiveOn()? cout<<"* " : cout<<"";
      cout<<"Syntax Error: expected R, found token at column "
	  << t->getCol() <<endl;
      throw ParseError;
    }
  }
  else if(t->getType() == lparen){
    ret = Expr();
    t = scan->getToken();
    if (t->getType() != rparen){
      calc->isInteractiveOn()? cout<<"* " : cout<<"";
      cout<<"Syntax Error: expected ), found token at column "
	  << t->getCol() <<endl;
      throw ParseError;
    }
  } else {
    calc->isInteractiveOn()? cout<<"* " : cout<<"";
    cout<<"Syntax Error: expected number, R, r, or (, found token at column "
	<< t->getCol() <<endl;
    throw ParseError;
  }

  return ret;
}
   
