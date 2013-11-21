#ifndef calculator_h
#define calculator_h

#include <string>
 
using namespace std;


class Calculator {
 public:
   Calculator();

   int eval(string expr);
   void compile(string expr);
   void store(int val);
   int recall();
   bool isInteractiveOn();
   void turnInteractiveOn();

 private:
   int memory;
   bool interactiveOn;
};

extern Calculator* calc;

#endif

