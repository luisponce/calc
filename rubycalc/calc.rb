#!/usr/bin/ruby
require 'stringio'
require 'set'

class Token
   attr_reader :type, :line, :col

   def initialize(type,lineNum,colNum)
      @type = type
      @line = lineNum
      @col = colNum
   end
end

class LexicalToken < Token
   attr_reader :lex
   
   def initialize(type,lex,lineNum,colNum)
      super(type,lineNum,colNum)
      
      @lex = lex
   end
end
      
class Scanner
   def initialize(inStream)
      @istream = inStream
		@keywords = Set.new(["S","R"])
      @lineCount = 1
      @colCount = -1
      @needToken = true
      @lastToken = nil
   end

   def putBackToken()
      @needToken = false
   end

   def getToken()
      if !@needToken
         @needToken = true
         return @lastToken
      end

      state = 0
      foundOne = false
      c = @istream.getc()

      if @istream.eof() then
         @lastToken = Token.new(:eof,@lineCount,@colCount)
         return @lastToken
      end

      while !foundOne
         @colCount = @colCount + 1
         case state
         when 0
            lex = ""
            column = @colCount
            line = @lineCount
            if isLetter(c) then state=1
            elsif isDigit(c) then state=2
            elsif c == ?+ then state = 3
            elsif c == ?- then state = 4
            elsif c == ?* then state = 5
            elsif c == ?/ then state = 6
            elsif c == ?( then state = 7
            elsif c == ?) then state = 8
            elsif c == ?% then state = 9
            elsif c == ?\n then 
              @colCount = -1
              @lineCount = @lineCount+1
            elsif isWhiteSpace(c) then state = state #ignore whitespace
            elsif @istream.eof() then
               @foundOne = true
               type = :eof
            else
               raise "Unrecognized Token found at line #{line} and column #{column}\n"
               #raise "Unrecognized Token"
            end
         when 1
            if isLetter(c) or isDigit(c) then state = 1
            else
               if @keywords.include?(lex) then
                  foundOne = true
                  type = :keyword
               else
                  foundOne = true
                  type = :identifier
               end
            end
         when 2
            if isDigit(c) then state = 2
            else
               type = :number
               foundOne = true
            end
         when 3
            type = :add
            foundOne = true
         when 4
            type = :sub
            foundOne = true
         when 5
            type = :times
            foundOne = true
         when 6
            type = :divide
            foundOne = true
         when 7
            type = :lparen
            foundOne = true
         when 8
            type = :rparen
            foundOne = true
         when 9
            type = :mod
            foundOne = true
         end

         if !foundOne then
            lex.concat(c)
            c = @istream.getc()
         end

      end
   
      @istream.ungetc(c)   
      @colCount = @colCount - 1
      if type == :number or type == :identifier or type == :keyword then
         t = LexicalToken.new(type,lex,line,column)
      else
         t = Token.new(type,line,column)
      end

      @lastToken = t
      return t 
   end

private
	def isLetter(c) 
	   return ((?a <= c and c <= ?z) or (?A <= c and c <= ?Z))
	end

	def isDigit(c)
	   return (?0 <= c and c <= ?9)
	end

	def isWhiteSpace(c)
	   return (c == ?\  or c == ?\n or c == ?\t)
	end
end
      
class BinaryNode
   attr_reader :left, :right
   
   def initialize(left,right)
      @left = left
      @right = right
   end
end
   
class UnaryNode
   attr_reader :subTree
   
   def initialize(subTree)
      @subTree = subTree
   end
end

class StoreNode < UnaryNode
   def initialize(subTree)
      super(subTree)
   end

   def evaluate()
      $calc.memory = @subTree.evaluate()
      return $calc.memory
   end

   def compile()
     str = "#Start StoreNode\n"
     
     str << "sp:=sp+one\n"
     str << @subTree.compile
     str << "sp:=sp-one\n"

     str << "store:=M[sp+1]\nM[sp+0]:=store\n"

     str << "#End StoreNode\n"

     return str
   end
end

class RecallNode 
   def evaluate()
      return $calc.memory
   end

   def compile()
     str = "#Start RecallNode\n"
     str << "M[sp+0]:=store\n"
     str << "#End RecallNode\n"

     return str
   end
end

class AddNode < BinaryNode
   def initialize(left, right)
     super(left,right)
   end
   
   def evaluate() 
     return @left.evaluate() + @right.evaluate()
   end
   
   def compile()
     ans = "#Start AddNode\n"
     
     ans << "sp:=sp+two\n"
     
     ans << "#AddNode: leftChild in\n"
     ans << @left.compile
     ans << "#AddNode: leftChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp:=M[sp+2]\nM[sp+1]:=tmp\n"

     ans << "sp:=sp+two\n"
     
     ans << "#AddNode: RightChild in\n"
     ans << @right.compile
     ans << "#AddNode: RightChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp2:=M[sp+2]\ntmp:=M[sp+1]\ntmp:=tmp+tmp2\nM[sp+0]:=tmp\n"
     
     ans << "#End AddNode\n"
     
     return ans
   end
end

class SubNode < BinaryNode
   def initialize(left, right)
      super(left,right)
   end
   
   def evaluate() 
     return @left.evaluate() - @right.evaluate()
   end
   
   def compile()
     ans = "#Start SubNode\n"
     
     ans << "sp:=sp+two\n"
     
     ans << "#SubNode: leftChild in\n"
     ans << @left.compile
     ans << "#SubNode: leftChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp:=M[sp+2]\nM[sp+1]:=tmp\n"
     
     ans << "sp:=sp+two\n"
     
     ans << "#SubNode: RightChild in\n"
     ans << @right.compile
     ans << "#SubNode: RightChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp2:=M[sp+2]\ntmp:=M[sp+1]\ntmp:=tmp-tmp2\nM[sp+0]:=tmp\n"
     
     ans << "#End SubNode\n"
     
     return ans
   end
end

class MulNode < BinaryNode
   def initialize(left, right)
      super(left,right)
   end
   
   def evaluate() 
      return @left.evaluate() * @right.evaluate()
   end

   def compile()
     ans = "#Start MulNode\n"
     
     ans << "sp:=sp+two\n"
     
     ans << "#SubNode: leftChild in\n"
     ans << @left.compile
     ans << "#SubNode: leftChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp:=M[sp+2]\nM[sp+1]:=tmp\n"
     
     ans << "sp:=sp+two\n"
     
     ans << "#SubNode: RightChild in\n"
     ans << @right.compile
     ans << "#SubNode: RightChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp2:=M[sp+2]\ntmp:=M[sp+1]\ntmp:=tmp*tmp2\nM[sp+0]:=tmp\n"
     
     ans << "#End MulNode\n"
     
     return ans
   end
end

class DivNode < BinaryNode
   def initialize(left, right)
      super(left,right)
   end
   
   def evaluate() 
      return @left.evaluate() / @right.evaluate()
   end

   def compile()
     ans = "#Start DivNode\n"
     
     ans << "sp:=sp+two\n"
     
     ans << "#SubNode: leftChild in\n"
     ans << @left.compile
     ans << "#SubNode: leftChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp:=M[sp+2]\nM[sp+1]:=tmp\n"
     
     ans << "sp:=sp+two\n"
     
     ans << "#SubNode: RightChild in\n"
     ans << @right.compile
     ans << "#SubNode: RightChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp2:=M[sp+2]\ntmp:=M[sp+1]\ntmp:=tmp/tmp2\nM[sp+0]:=tmp\n"
     
     ans << "#End DivNode\n"
     
     return ans
   end
end

class ModNode < BinaryNode
   def initialize(left, right)
      super(left,right)
   end

   def evaluate()
      return @left.evaluate() % @right.evaluate()
   end

   def compile()
     ans = "#Start ModNode\n"
     
     ans << "sp:=sp+two\n"
     
     ans << "#SubNode: leftChild in\n"
     ans << @left.compile
     ans << "#SubNode: leftChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp:=M[sp+2]\nM[sp+1]:=tmp\n"
     
     ans << "sp:=sp+two\n"
     
     ans << "#SubNode: RightChild in\n"
     ans << @right.compile
     ans << "#SubNode: RightChild out\n"
     
     ans << "sp:=sp-two\n"
     
     ans << "tmp2:=M[sp+2]\ntmp:=M[sp+1]\ntmp:=tmp%tmp2\nM[sp+0]:=tmp\n"
     
     ans << "#End ModNode\n"
     
     return ans
   end
end

class NegateNode < UnaryNode
   def initialize(subTree)
      super(subTree)
   end

   def evaluate()
      return -1*@subTree.evaluate()
   end
   
   def compile()
     ans = "#Start NegateNode\n"
     
     ans << "sp:=sp+one\n"
     ans << @subTree.compile
     ans << "sp:=sp-one\n"
     
     ans << "tmp:=0\ntmp:=tmp-one\ntmp2:=M[sp+1]\ntmp:=tmp*tmp2\nM[sp+0]:=tmp\n"
     
     ans << "#End NegateNode\n"

     return ans
   end
end

class NumNode 
   def initialize(num)
      @num = num
   end
   
   def evaluate() 
      return @num.to_i
   end

   def compile()
     ans = "#Start NumNode\n"
     
     ans << "tmp:="
     ans << @num
     ans << "\nM[sp+0]:=tmp\n"
     
     ans << "#End NumNode\n"
     
     return ans
   end
 end   
      
class Parser
   def initialize(istream)
      @scan = Scanner.new(istream)
   end
   
   def parse()
      return Prog()
   end
   
private
   def Prog()
      result = Expr()
      t = @scan.getToken()
      
      if t.type != :eof then
         raise "Expected EOF. Found ", t.type, ".\n"
         #raise "Parse Error"
      end
      
      return result
   end
   
   def Expr() 
      return RestExpr(Term())
   end
   
   def RestExpr(e) 
      t = @scan.getToken()
      
      if t.type == :add then
         return RestExpr(AddNode.new(e,Term()))
      end
      
      if t.type == :sub then
         return RestExpr(SubNode.new(e,Term()))
      end
      
      @scan.putBackToken()
      
      return e
   end
   
   def Term()
      return RestTerm(Storable())
   end
   
   def RestTerm(e)
      t=@scan.getToken
      if (t.type == :times) then
         return RestTerm(MulNode.new(e,Storable()))
      end 

      if (t.type == :divide) then
         return RestTerm(DivNode.new(e,Storable()))
      end

      if (t.type == :mod) then
         return RestTerm(ModNode.new(e,Storable()))
      end

      @scan.putBackToken()

      return e
   end
   
   def Storable()
      ret=Negate()
      t=@scan.getToken
      if(t.type==:keyword) then	
         if(t.lex=='S') then
	    ret=StoreNode.new(ret)
	    return ret
	 end
       end

       @scan.putBackToken()

       return ret
   end
   
   def Negate()
      t=@scan.getToken
      if(t.type==:sub) then
         return NegateNode.new(Factor())
      else
         @scan.putBackToken()

         return Factor()
      end
   end

   def Factor() 
      t=@scan.getToken()
      if(t.type==:number) then
         return NumNode.new(t.lex)
      end

      if(t.type==:keyword) then 
         if(t.lex=='R') then
 	    return RecallNode.new()
	 end
      end

      if(t.type==:lparen) then
         ret = Expr()
	 t=@scan.getToken
	 if(t.type!=:rparen) then
            raise "Syntax Error: Expected ), found token #{t.type}"
	    #raise "ParseError"
	 end

	 return ret
      end

      raise "Syntax Error: Expected Number, R or (, found token #{t.type}"
      #raise "Parse Error"
   end         
end

class Calculator
  attr_reader :memory
  attr_writer :memory
  
  def initialize()
    @memory = 0
  end
  
  def eval(expr)
    parser = Parser.new(StringIO.new(expr))
    ast = parser.parse()
    return ast.evaluate()
  end
  
  def compile(expr, path)
    parser = Parser.new(StringIO.new(expr))
    ast = parser.parse()
    
    str = "start:\none:=1\ntwo:=2\nsp:=6\n"
    
    str << ast.compile()
    
    str << "tmp:=M[sp+0]\nwriteInt(tmp)\nend: halt\n"
    str << "equ one M[0]\nequ two M [1]\nequ tmp M[2]\n"
    str << "equ tmp2 M [3]\nequ sp M[4]\nequ store M[5]\n"
    str << "equ stack M[6]\n"

    #write to file
    if(File.exists?(path))
      File.delete(path)
    end
    File.open(path,'w'){ |file| file.write(str)}
  end
end

if(ARGV.length>0)
  if(ARGV[0]=="-i")
    #Intreactive Mode
    begin
      begin
        print "> "
        text = STDIN.gets
        $calc = Calculator.new()
        
        puts "= " + $calc.eval(text).to_s
      rescue Exception => e
        print "* "
        puts e.message
      end
    end while (text != nil)
  else
    if (ARGV[0]=="-c")
      #ewe Compiler Mode
      begin
        print "Please enter a calculator expression: "
        text = STDIN.gets
        $calc = Calculator.new()
        
        outputFile = "a.ewe"
        
        $calc.compile(text, outputFile)
        
        puts "wrote to #{outputFile}"
      rescue Exception => e
        puts e.message
      end
    end
  end
else
  #Normal Mode
  begin
    print "Please enter a calculator expression: "
    text = gets
    $calc = Calculator.new()
    
    puts "The result is " + $calc.eval(text).to_s
  rescue Exception => e
    puts e.message
  end
end
