#include "ast.h"
#include <iostream>
#include "calculator.h"
#include <fstream>
#include <string>
#include <sstream>

// for debug information uncomment
//#define debug

AST::AST() {}

AST::~AST() {}

BinaryNode::BinaryNode(AST* left, AST* right):
   AST(),
   leftTree(left),
   rightTree(right)
{}

BinaryNode::~BinaryNode() {
#ifdef debug
   cout << "In BinaryNode destructor" << endl;
#endif

   try {
      delete leftTree;
   } catch (...) {}

   try {
      delete rightTree;
   } catch(...) {}
}
   
AST* BinaryNode::getLeftSubTree() const {
   return leftTree;
}

AST* BinaryNode::getRightSubTree() const {
   return rightTree;
}

UnaryNode::UnaryNode(AST* sub):
   AST(),
   subTree(sub)
{}

UnaryNode::~UnaryNode() {
#ifdef debug
   cout << "In UnaryNode destructor" << endl;
#endif

   try {
      delete subTree;
   } catch (...) {}
}
   
AST* UnaryNode::getSubTree() const {
   return subTree;
}

AddNode::AddNode(AST* left, AST* right):
   BinaryNode(left,right)
{}

int AddNode::evaluate() {
   return getLeftSubTree()->evaluate() + getRightSubTree()->evaluate();
}

string AddNode::compile() {
  string ans;
  ans+="#Start AddNode\n";
  
  ans+="sp:=sp+two\n";
  
  ans+="#AddNode: leftChild in\n";
  ans+=getLeftSubTree()->compile();
  ans+="#AddNode: leftChild out\n";
  
  ans+="sp:=sp-two\n";

  ans+="tmp:=M[sp+2]\nM[sp+1]:=tmp\n";
  
  ans+="sp:=sp+two\n";
  
  ans+="#AddNode: RightChild in\n";
  ans+=getRightSubTree()->compile();
  ans+="#AddNode: RightChild out\n";
  
  ans+="sp:=sp-two\n";

  ans+="tmp2:=M[sp+2]\ntmp:=M[sp+1]\ntmp:=tmp+tmp2\nM[sp+0]:=tmp\n";

  ans+="#End AddNode\n";

  return ans;
}

SubNode::SubNode(AST* left, AST* right):
   BinaryNode(left,right)
{}

int SubNode::evaluate() {
   return getLeftSubTree()->evaluate() - getRightSubTree()->evaluate();
}

string SubNode::compile() {
  string ans;
  ans+="#Start SubNode\n";
  
  ans+="sp:=sp+two\n";
  
  ans+="#SubNode: leftChild in\n";
  ans+=getLeftSubTree()->compile();
  ans+="#SubNode: leftChild out\n";
  
  ans+="sp:=sp-two\n";

  ans+="tmp:=M[sp+2]\nM[sp+1]:=tmp\n";
  
  ans+="sp:=sp+two\n";
  
  ans+="#SubNode: RightChild in\n";
  ans+=getRightSubTree()->compile();
  ans+="#SubNode: RightChild out\n";
  
  ans+="sp:=sp-two\n";

  ans+="tmp2:=M[sp+2]\ntmp:=M[sp+1]\ntmp:=tmp-tmp2\nM[sp+0]:=tmp\n";

  ans+="#End SubNode\n";

  return ans;
}

MulNode::MulNode(AST* left, AST* right):
   BinaryNode(left,right)
{}

int MulNode::evaluate() {
   return getLeftSubTree()->evaluate() * getRightSubTree()->evaluate();
}

string MulNode::compile() {
  string ans;
  ans+="#Start MulNode\n";
  
  ans+="sp:=sp+two\n";
  
  ans+="#MulNode: leftChild in\n";
  ans+=getLeftSubTree()->compile();
  ans+="#MulNode: leftChild out\n";
  
  ans+="sp:=sp-two\n";

  ans+="tmp:=M[sp+2]\nM[sp+1]:=tmp\n";
  
  ans+="sp:=sp+two\n";

  ans+="#MulNode: RightChild in\n";
  ans+=getRightSubTree()->compile();
  ans+="#MulNode: RightChild out\n";
  
  ans+="sp:=sp-two\n";

  ans+="tmp2:=M[sp+2]\ntmp:=M[sp+1]\ntmp:=tmp*tmp2\nM[sp+0]:=tmp\n";

  ans+="#End MulNode\n";

  return ans;
}

DivNode::DivNode(AST* left, AST* right):
   BinaryNode(left,right)
{}

int DivNode::evaluate() {
   return getLeftSubTree()->evaluate() / getRightSubTree()->evaluate();
}

string DivNode::compile() {
  string ans;
  ans+="#Start DivNode\n";
  
  ans+="sp:=sp+two\n";
  
  ans+="#DivNode: leftChild in\n";
  ans+=getLeftSubTree()->compile();
  ans+="#DivNode: leftChild out\n";
  
  ans+="sp:=sp-two\n";

  ans+="tmp:=M[sp+2]\nM[sp+1]:=tmp\n";
  
  ans+="sp:=sp+two\n";

  ans+="#DivNode: RightChild in\n";
  ans+=getRightSubTree()->compile();
  ans+="#DivNode: RightChild out\n";
  
  ans+="sp:=sp-two\n";

  ans+="tmp2:=M[sp+2]\ntmp:=M[sp+1]\ntmp:=tmp/tmp2\nM[sp+0]:=tmp\n";

  ans+="#End DivNode\n";

  return ans;
}

NumNode::NumNode(int n) :
   AST(),
   val(n)
{}

int NumNode::evaluate() {
   return val;
}

string NumNode::compile(){
  string ans;
  
  ans+="#Start NumNode\n";
  
  stringstream ss;
  ss<<val;

  ans+="tmp:=";
  ans+=ss.str();
  ans+="\nM[sp+0]:=tmp\n";

  ans+="#End NumNode\n";

  return ans;
}

RecallNode::RecallNode() :
  AST() 
{}

RecallNode::~RecallNode() {}

int RecallNode::evaluate(){
  return calc->recall();
}

string RecallNode::compile(){
  string ans;
  ans+="#Start RecallNode\n";

  ans+="M[sp+0]:=store\n";

  ans+="#End RecallNode\n";

  return ans;
}

StoreNode::StoreNode(AST* sub) :
  UnaryNode(sub) {}

StoreNode::~StoreNode() {}

int StoreNode::evaluate() {
  int mem = getSubTree()->evaluate();
  calc->store(mem);
  return mem;
}

string StoreNode::compile(){
  string ans;

  ans+="#Start StoreNode\n";

  ans+="sp:=sp+one\n";
  ans+=getSubTree()->compile();
  ans+="sp:=sp-one\n";
  
  ans+="store:=M[sp+1]\nM[sp+0]:=store\n";

  ans+="#End StoreNode\n";

  return ans;
}
