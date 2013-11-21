#ifndef ast_h
#define ast_h

using namespace std;

#include <string>

class AST {
 public:
   AST();
   virtual ~AST() = 0;
   virtual int evaluate() = 0;
   virtual string compile() = 0;
};

class BinaryNode : public AST {
 public:
   BinaryNode(AST* left, AST* right);
   ~BinaryNode();

   AST* getLeftSubTree() const;
   AST* getRightSubTree() const;

 private:
   AST* leftTree;
   AST* rightTree;
};

class UnaryNode : public AST {
 public:
   UnaryNode(AST* sub);
   ~UnaryNode();

   AST* getSubTree() const;

 private:
   AST* subTree;
};

class AddNode : public BinaryNode {
 public:
   AddNode(AST* left, AST* right);
   
   int evaluate();
   string compile();
};

class SubNode : public BinaryNode {
 public:
   SubNode(AST* left, AST* right);

   int evaluate();
   string compile();
};

class MulNode : public BinaryNode {
 public:
   MulNode(AST* left, AST* right);
   
   int evaluate();
   string compile();
};

class DivNode : public BinaryNode {
 public:
   DivNode(AST* left, AST* right);
   
   int evaluate();
   string compile();
};

class RecallNode : public AST {
 public:
  RecallNode();
  ~RecallNode();

  int evaluate();
  string compile();
};

class StoreNode : public UnaryNode {
 public:
  StoreNode(AST* sub);
  ~StoreNode();

  int evaluate();
  string compile();
};

class NumNode : public AST {
 public:
   NumNode(int n);

   int evaluate();
   string compile();

 private:
   int val;
};


#endif

