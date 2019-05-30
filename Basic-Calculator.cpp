#include <iostream>
#include <cctype>
#include <cmath> 
using namespace std;

// Define node structure
typedef struct _node * nodep;
typedef struct _node{
 short type;        // 0: operator, use op; 1: operand, use value
 union{
  char op;          // For operator
  double value;   // For operand
 };
 nodep next;        // Point to next node
} node;

// Valid operator
// # : for internal usage
#define checkOP 7
const char validOP[]={'(','^','*','/','+','-',')','#'};

// Left Operator : ISP(In-Stack Priority)
// (, ^, *, /, +, -, ), #
// 0  4  4  4  2  2  6 -1
const int ISP[]={0,4,4,4,2,2,6,-1};

// Right Operator : ICP(In-Coming Priority)
// (, ^, *, /, +, -, ), #
// 6  5  3  3  1  1  0 -1
const int ICP[]={6,5,3,3,1,1,0,-1};

// Valid cal. string linked list
nodep calLListHead = NULL;
nodep calLListTail = NULL;

// Stack for operators and operands
nodep oprStack = NULL;
nodep opdStack = NULL;

// Basic functions
int isValidOP(char);        // For checking operator
int isValidChar(char);      // For checking character
int getValidOP(char);       // To get operator id

// For check input string and build cal. string linked list
void addNodeTocalLList(nodep);      // Add free node to cal. string linked list.
                                    // Been used in next two functions.
void addOperand(double);            // Add operand node
void addOperator(char);             // Add operator node
int checkCalStr(string);            // For check input string

// For stack operation
void push();                        // Read head node of cal. string linked list
                                    // then push free node to oprStack or opdStack
void push(nodep);                   // Push free node(result) to oprStack or opdStack
nodep pop(int);                     // Popup top node of oprStack or opdStack
nodep mathCal(nodep,nodep,nodep);   // Mathematics calculation
double calculate();                 // Calculate input string

void showCalLList();                // Show cal. string linked list
void cleanLList();                  // Clean cal. string linked list

// Purpose : Main program
// Params  : void
// Return  : 0 : normal execution
int main()
{
 // Cal. string from user input
 string calStr;
 // Continue or not
 char choice;

 cout << "四則運算計算機" << endl;
 cout << "運算元   : 支援 正整數及小數點" << endl;
 cout << "運算符號 : 支援 +(加) -(減) *(乘) /(除) ^(次方)" << endl;
 cout << "另支援括號. 例 : (2.2+3.3)*5.5" << endl;
 cout << "### 請勿使用空白鍵分隔運算元與運算符號. ###" << endl;

 do{
  cout << "\n請輸入計算式 ";
  cin >> calStr;

  if(!checkCalStr(calStr))
  {
   cout << "計算式無法正常計算" << endl;
   continue;
  }

        // Display input string and result
  showCalLList();
  cout << "= " << calculate() << endl;

  // Cleanup cal. linked list
  cleanLList();

  cout << "是否繼續計算? (任意鍵繼續, N/n 離開)";
  cin >> choice;
 }while(choice != 'N' && choice != 'n');

    //system("pause");
 return 0;
}

// Purpose : For checking oprator
// Params  : symbol of operator
// Return  : 0 : invalid; 1: valid
int isValidOP(char c)
{
 int i;
 for(i = 0; i < checkOP; i++)
 {
  if(c == validOP[i])
   return 1;
 }

 return 0;
}

// Purpose : For checking character
// Params  : Character from input string
// Return  : 0 : invalid; 1: valid
int isValidChar(char c)
{
 if(isdigit(c) || isValidOP(c) || c == '.')
  return 1;
 else
  return 0;
}

// Purpose : To get operator id by search in validOP array
// Params  : Symbol of operator
// Return  : Id of operator
int getValidOP(char c)
{
 int i;

 for(int i=0; i < checkOP; i++)
 {
  if(validOP[i] == c)
   return i;
 }

 return -1;
}

// Purpose : Add free node to cal. string linked list. 
//           Been used in next two functions.
// Params  : New operand or operator node
// Return  : Void
void addNodeTocalLList(nodep node)
{
 if(calLListTail)
 {
        // Has at least one node
  calLListTail->next = node;
  calLListTail = node;
 }
 else
 {
        // The cal. string linked list is empty
  calLListHead = node;
  calLListTail = node;
 }
}

// Purpose : Add operand node
// Params  : The value of operand
// Return  : Void
void addOperand(double value)
{
    // Generate new operand node
 nodep newNode = new node;
 newNode->type = 1;
 newNode->value = value;
 newNode->next = NULL;

    // Add new node to cal. string linked list
 addNodeTocalLList(newNode);
}

// Purpose : Add operator node
// Params  : The symbol of operator
// Return  : Void
void addOperator(char op)
{
    // Generate new operator node
 nodep newNode = new node;
 newNode->type = 0;
 newNode->op = op;
 newNode->next = NULL;

    // Add new node to cal. string linked list
 addNodeTocalLList(newNode);
}

// Purpose : Check the cal. string
// Params  : Input string
// Return  : 0 : failed; 1: success
int checkCalStr(string calStr)
{
    // Loop counter, length of cal. string, parentheses counter, value of digit char, base value for decimal
 int i, calStrLen, parenCount, charValue, divBase;
    // True  : after '.', below the decimal point
    // False : before '.', above the decimal point
 bool isDecimal;
    // Temporary value for digit
 double tmp;

 // 0. Get string length
 calStrLen = calStr.length();

 // 1. Check chacter
 for(i = 0; i < calStrLen; i++)
 {
        // If find invalid char, stop processing
  if(!isValidChar(calStr[i]))
  {
   cout << "錯誤 : 發現無法辨識的字元 " << calStr[i] << ", 無法進行後續的計算." << endl;
   return 0;
  }
 }

 // 2. Check parentheses : if the pairs of parentheses is correct
    //                        the final parenCount will be zero.
 parenCount = 0;
 for(i = 0; i < calStrLen; i++)
 {
  if(calStr[i] == '(')
   parenCount++;
  else if(calStr[i] == ')')
   parenCount--;
 }
 if(parenCount)
 {
  cout << "錯誤 : 括號無法正確配對, ";
  if(parenCount>0)
   cout << "少 " << parenCount << " 個右括號." << endl;
  else
   cout << "少 " << 0-parenCount << " 個左括號." << endl;
  return 0;
 }

 // 2. Generate Link List
    // 2.1. initialize variables
 tmp = 0;
 isDecimal = false;
 divBase=10;

    // 2.2. generate operator and operand node and add to tail of cal. string linked list
 for(i = 0; i < calStrLen; i++)
 {
        // Only for converting single digit char to integer
  charValue = 0;

        // Generate node
  if(isValidOP(calStr[i]))
  {
   // Generate previous operand node
   if(i > 0 && isdigit(calStr[i-1]))
   {
    addOperand(tmp);
   }

   // Generate operator node
   addOperator(calStr[i]);

            // Prepare for reading next operand or operator
   tmp = 0;
   isDecimal = false;
   divBase=10;
  }
  else if(calStr[i] == '.')
  {
            // Below the decimal point and only appear between two digit char
   isDecimal = true;
  }
  else
  {
            // Process digit char.
   charValue = (int)calStr[i] - 48;
   if(isDecimal)
   {
                // Below the decimal point
    tmp+=((double)charValue/divBase);
    divBase*=10;
   }
   else
   {
                // Above the decimal point
    tmp*=10;
    tmp+=(double)charValue;
   }
  }
 }

    // Generate last operand if available
 if(isdigit(calStr[calStrLen-1]))
  addOperand(tmp);

 return 1;
}

// Purpose : Read head node of cal. string linked list
//           then push free node to oprStack or opdStack
// Params  : Node pointer of free node
// Return  : Void
void push()
{
    // Get head node of cal. string linked list
 nodep cur = calLListHead;
 calLListHead = calLListHead->next;
 cur->next = NULL;

    // Push free node to oprStack or opdStack
 if(cur->type)
 {
  // Operand
  cur->next = opdStack;
  opdStack = cur;
 }
 else
 {
  // Operator
  cur->next = oprStack;
  oprStack = cur;
 }
}

// Purpose : Push free node(result) to oprStack or opdStack
// Params  : Node pointer of free node
// Return  : Void
void push(nodep node)
{
 if(node->type)
 {
  // Operand
        // Let the next pointer ponit to top node of opdStack
  node->next = opdStack;
        // Move top node pointer ponit to new node
  opdStack = node;
 }
 else
 {
  // Operator
        // Let the next pointer ponit to top node of oprStack
  node->next = oprStack;
        // Move top node pointer ponit to new node
  oprStack = node;
 }
}

// Purpose : Popup top node of oprStack or opdStack
// Params  : 0 : operator; 1 : operand
// Return  : Top node pointer of oprStack or opdStack
nodep pop(int type)
{
    // Top node pointer of oprStack or opdStack
 nodep stackTop;

    // 0 : operator; 1 : operand
 if(type)
 {
  // Operand
        // opdStack is empty
  if(!opdStack)
   return NULL;

        // Point to top node of opdStack
  stackTop = opdStack;
        // Move the top pointer of opdStack to next node
  if(stackTop->next)
   opdStack = opdStack->next;
  else
   opdStack = NULL;
 }
 else
 {
  // Operator
        // oprStack is empty
  if(!oprStack)
   return NULL;

        // Point to top node of oprStack
  stackTop = oprStack;
        // Move the top pointer of oprStack to next node
  if(stackTop->next)
   oprStack = oprStack->next;
  else
   oprStack = NULL;
 }

    // Clean the next pointer of free node
 stackTop->next = NULL;
 return stackTop;
}

// Purpose : Do +, -, *, / and ^
// Params  : Pointer for left operaand, operator and right operand
// Return  : Result node with operand type
nodep mathCal(nodep left, nodep opr, nodep right)
{
    // Alocate new operand node for result
 nodep result = new node;
 result->type = 1;
 result->value = 0.0;
 result->next = NULL;

    // Mathematic switch
 switch(opr->op)
 {
  case '+': result->value = left->value + right->value; break;
  case '-': result->value = left->value - right->value; break;
  case '*': result->value = left->value * right->value; break;
  case '/': result->value = left->value / right->value; break;
  case '^': result->value = pow(left->value , right->value); break;
  default: break;
 }

 return result;
}

// Purpose : Calculate the mathematic expression
// Params  : Void
// Return  : Result value
double calculate()
{
 // Define node pointer for left operand, right operand, operator and result
 nodep left, right, opr, result;

    // Read the cal. string linked list one by one
 while(calLListHead)
 {
  // Do cal. work by its type
  if(calLListHead->type)
  {
   // Operand
   push();
  }
  else
  {
   // Operator ( ) + - * / ^
   if(calLListHead->op == '(')
   {
    // Operator : (
    push();
   }
   else if(calLListHead->op == ')')
   {
    // Operator : )
    //remove ')' node
    opr = calLListHead;
    calLListHead = calLListHead->next;
    opr->next=NULL;
    delete opr;

    // Popup oprStack until '('
    while(oprStack && oprStack->op != '(')
    {
     // Get right, left operand node from opdStack
     right = pop(1);
     left  = pop(1);
     // Get operator from oprStack
     opr   = pop(0);
     // Execute calculation
     result = mathCal(left,opr,right);
     // Push result node to operand stack
     push(result);
    }
    // Remove '('
    pop(0);
   }
   else
   {
    // Operator : + - * / ^
    if(oprStack)
    {
                    // Compare left operator(from oprStack) and 
                    // right operator(from cal. string linked list)
     if(ISP[getValidOP(oprStack->op)] > ICP[getValidOP(calLListHead->op)])
     {
                        // High priority operator(left operator)
      // Get right, left operand node from opdStack
      right = pop(1);
      left  = pop(1);
      // Get left operator from oprStack
      opr   = pop(0);
      // Execute calculation
      result = mathCal(left,opr,right);
      // Push result node to operand stack
      push(result);
      // Push right operator to operator stack
      push();
     }
     else if(ISP[getValidOP(oprStack->op)] < ICP[getValidOP(calLListHead->op)])
     {
                        // High priority operator(right operator)
      // Push right operator to operator stack
      push();
     }
     else
     {
      // Must not have this situation
     }
    }
    else
    {
     // If operator stack is empty, push the operator to oprStack
     push();
    }
   }
  }

 }

 // To do the last calculation work and popup oprStack untill empty
 while(oprStack)
 {
  // Get right, left operand node from opdStack
  right = pop(1);
  left  = pop(1);
  // Get operator from oprStack
  opr   = pop(0);
  // Execute calculation
  result = mathCal(left,opr,right);
  // Push result node to operand stack
  push(result);
 }

 // Get final result and return its value
 result = pop(1);
 return result->value;
}

// Purpose : Display the cal. string linked list
// Params  : Void
// Return  : Void
void showCalLList()
{
 // Head pointer
 nodep cur = calLListHead;

 // Print cal. string linked list recusively
 while(cur)
 {
        // Display the content of node by its type
        // 0 : operator; 1 : operand
  if(cur->type)
   cout << cur->value << " ";
  else
   cout << cur->op << " ";
  cur = cur->next;
 }
}

// Purpose : Clean the unused linked list
//           Read the cal. string linked list and release unused memory space
// Params  : Void
// Return  : Void
void cleanLList()
{
 // Head pointer
 nodep cur = calLListHead;

 // Delete node from head to tail
 while(cur)
 {
        // Move head of linked list to next position
  calLListHead = calLListHead->next;
        // Remove the free node
  delete cur;
        // Point to head of linked list and do it again untill the linked list is empty
  cur= calLListHead;
 }
    // The linked list had been removed then change the tail pointer of linked list
 calLListTail = NULL;
}
