#include <iostream>
#include <sstream>
#include <string>
#include "calcex.h"
#include "calculator.h"
using namespace std;

Calculator* calc;

int main(int argc, char* argv[]) {
  if(argc>1){
    string arg = argv[1];
    if(arg=="-i"){
      //interactive mode
      calc = new Calculator();

      string line;
      cout<<"> ";      
      
      calc->turnInteractiveOn();

      while(getline(cin, line)){
	try{
	  int result = calc->eval(line);
	  cout<<"= "<<result<<endl;
	  calc->store(0);
	} catch(Exception ex){
	  cout << "* Program Aborted due to exception!" << endl; 
	}
	
	cout<<"> ";
      }

      delete calc;
      
    } else if(arg=="-c") {
      //ewe compiler mode
      try{
	calc = new Calculator();
	
	string line;
	
	cout<<"Please enter a calculator expression: ";
	
	getline(cin, line);
	
	calc->compile(line);
      } catch(Exception ex){
	cout <<  "Program Aborted due to exception!" << endl;
      }
    }
  }
  else {
      string line;
    
      try {
	
	cout << "Please enter a calculator expression: ";
	
	getline(cin, line);
	// line + '\n';
	
	calc = new Calculator();
	
	int result = calc->eval(line);
	
	cout << "The result is " << result << endl;
      
	delete calc;
	
      }
      catch(Exception ex) {
	cout << "Program Aborted due to exception!" << endl;
      } 
  }
}
  
  
