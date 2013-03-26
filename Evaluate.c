/*
    Copyright 2009 by Kevin Pluck

    This file is part of InvisiCalc.

    InvisiCalc is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    InvisiCalc is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with InvisiCalc.  If not, see <http://www.gnu.org/licenses/>.
*/

//http://www.faqts.com/knowledge_base/view.phtml/aid/29780
//test3
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include "Evaluate.h"
#define MAX_TOKEN_LENGTH 50
#define STACK_SIZE 50
int Evaluate(char *strInfixCode,double *fltResult)
{
    //Tokenize
    int intLengthOf_strInfixCode = strlen(strInfixCode), intCount=0;
    char strToken[MAX_TOKEN_LENGTH];
    char stack[STACK_SIZE];
    double operand_stack[STACK_SIZE];
    int intStackPos=0,intOperandStackPos=0;
    int blnAtLeastOneOperatorFound=0;
    int blnFailedToEvaluate=0;
    int blnDivideByZero=0;
    int blnInvalidInput=0;

#define MAX_FUNCS 18
    char arrFunctions [MAX_FUNCS][6] = {"sqrt","pi","e","c","sin","cos","tan","asin","acos","atan","exp","ln","sinh","cosh","tanh","gamma","phi","log"};
    char arrFuncToken [MAX_FUNCS]    = {_SQRT ,_PI ,_E ,_C ,_SIN ,_COS ,_TAN ,_ASIN ,_ACOS ,_ATAN ,_EXP ,_LOG ,_SINH ,_COSH ,_TANH ,_GAMMA ,_PHI ,_LOG10 };


    //Nested Function!
    int stack_push(char chrToPush)
    {
        if(intStackPos>=STACK_SIZE) return 0;

        stack[intStackPos++]=chrToPush;
        return 1;
    }

    //Nested Function!
    char stack_pop()
    {
        if(intStackPos<0) return '\0';

        return stack[--intStackPos];
    }

    //Nested Function!
    char stack_peek()
    {
        return stack[intStackPos-1];
    }


    //Nested Function!
    int operand_stack_push(double fltToPush)
    {
        if(intOperandStackPos>=STACK_SIZE) return 0;

        operand_stack[intOperandStackPos++]=fltToPush;
        return 1;
    }

    //Nested Function!
    double operand_stack_pop()
    {
        if(intOperandStackPos<0) return 0; //DODGEY!

        return operand_stack[--intOperandStackPos];
    }

    //Nested Function!
    double operand_stack_peek()
    {
        return operand_stack[intOperandStackPos-1];
    }

    //Nested Function!
    int operatorPrecedence(char chrTheOperator)
    {
        switch(chrTheOperator)
        {
            case '+':
            case '-':
                return 1;
            break;

            case '*':
            case '/':
            case '%':
                return 3;
            break;

            case '^':
                return 4;
            break;
        }
        return 0;
    }

    //Nested Function!
    char GetNextChar()
    {
        if(intCount<intLengthOf_strInfixCode)
            return strInfixCode[intCount++];
        else
            return '\0';
    }

    //Nested Function!
    int append(char* s, char c)
    {
        int len = strlen(s);

        if(len>=MAX_TOKEN_LENGTH) return 0;

        s[len] = c;
        s[len+1] = '\0';
        return 1;
    }

    enum EXPECTING
    {
        an_operand,
        an_operator
    } enumExpecting=an_operand;

    enum TOKENSTATE
    {
        token_start,
        token_complete,
        number_whole,
        number_fraction,
        token_function,
        token_fail
    } enumTokenState=token_start;

    char chrTheChar=GetNextChar();

    double doSingleCalculation(char chrOperator)
    {
        double fltOperand1,fltOperand2,fltToReturn;

        // Reset errno
        errno=0;

        //Process MONOMIAL functions
        //make sure we have at least one operand for monomial functions
        if(intOperandStackPos>=1)
        {
            fltOperand2=operand_stack_pop();
            switch(chrOperator)
            {
                case _SQRT:
                    fltToReturn = sqrt(fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
                case _SIN:
                    return sin(fltOperand2);
                break;
                case _COS:
                    return cos(fltOperand2);
                break;
                case _TAN:
                    fltToReturn = tan(fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
                case _ASIN:
                    fltToReturn = asin(fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
                case _ACOS:
                    fltToReturn = acos(fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
                case _ATAN:
                    return atan(fltOperand2);
                break;
                case _SINH:
                    fltToReturn = sinh(fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
                case _COSH:
                    fltToReturn = cosh(fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
                case _TANH:
                    return tanh(fltOperand2);
                break;
                case _LOG:
                    fltToReturn = log(fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
                case _LOG10:
                    fltToReturn = log10(fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
                case _EXP:
                    fltToReturn = exp(fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
            }
        }

        //Process BINOMIAL functions
        //make sure we have at least one more operand for binomial functions
        if(intOperandStackPos>=1)
        {
            fltOperand1=operand_stack_pop();

            switch(chrOperator)
            {
                case '+':
                    return fltOperand1 + fltOperand2;
                break;
                case '-':
                    return fltOperand1 - fltOperand2;
                break;
                case '*':
                    return fltOperand1 * fltOperand2;
                break;
                case '/':
                    if(fltOperand2!=0)
                        return fltOperand1 / fltOperand2;
                    else
                    {
                        blnDivideByZero=1;
                        return 0;
                    }
                break;
                case '%':
                    return fmod(fltOperand1, fltOperand2);
                break;
                case '^':
                    fltToReturn = pow(fltOperand1, fltOperand2);
                    blnInvalidInput=errno;
                    return fltToReturn;
                break;
            }
        }
        //If we get here then no operator was matched.
        //The tokenizing step would have to fail!
        blnFailedToEvaluate=1;
        return 0;
    }

    char GetFuncToken(char *strFuncName)
    {
        int intFuncCount=0;
        for(intFuncCount=0;intFuncCount<MAX_FUNCS;intFuncCount++)
        {
            if(strcmp(strFuncName,arrFunctions[intFuncCount])==0)
            {
                return arrFuncToken[intFuncCount];
            }
        }
        return '\0';
    }

    //Nested Function!
    int GetNextToken(char *strTheToken)
    {
        if(chrTheChar=='\0')
        {
            strcpy(strTheToken,"done");
            return 0;
        }

        enumTokenState=token_start;

        while(enumTokenState!=token_complete && enumTokenState!=token_fail)
        {
            switch(enumTokenState)
            {
                case token_start:
                    if(isspace(chrTheChar))
                    {
                        chrTheChar=GetNextChar();
                        break;
                    }

                    if(isdigit(chrTheChar))
                    {
                        strcpy(strTheToken,"NUMBER{");

                        append(strTheToken,chrTheChar);

                        enumTokenState=number_whole;
                        chrTheChar=GetNextChar();
                        break;
                    }

                    if(isalnum(chrTheChar)&&enumExpecting==an_operand)
                    {
                        strcpy(strTheToken,"");
                        append(strTheToken,chrTheChar);
                        enumTokenState=token_function;
                        chrTheChar=GetNextChar();
                        break;
                    }

                    if(chrTheChar=='.')
                    {
                        strcpy(strTheToken,"NUMBER{.");
                        enumTokenState=number_fraction;
                        chrTheChar=GetNextChar();
                        break;
                    }

                    if(chrTheChar=='-'&&enumExpecting==an_operand)
                    {
                        strcpy(strTheToken,"NUMBER{-");
                        enumTokenState=number_whole;
                        chrTheChar=GetNextChar();
                        break;
                    }

                    switch(chrTheChar)
                    {
                        case '+':
                        case '-':
                        case '*':
                        case '/':
                        case '^':
                        case '%':

                            if(enumExpecting!=an_operator)
                            {
                                enumTokenState=token_fail;
                                break;
                            }

                            strcpy(strTheToken,"");
                            append(strTheToken,chrTheChar);

                            chrTheChar=GetNextChar();
                            enumTokenState=token_complete;
                            enumExpecting=an_operand;
                            blnAtLeastOneOperatorFound=1;
                        break;

                        case '(':
                            if(enumExpecting!=an_operand)
                            {
                                //Send multiplication token as there is an implied multiplication:
                                //E.g.  3(4+1) = 3*(4+1)
                                strcpy(strTheToken,"*");
                                enumTokenState=token_complete;
                                enumExpecting=an_operand;
                                break;
                            }

                            strcpy(strTheToken,"(");

                            chrTheChar=GetNextChar();
                            enumTokenState=token_complete;
                        break;

                        case ')':
                            if(enumExpecting!=an_operator)
                            {
                                enumTokenState=token_fail;
                                break;
                            }

                            strcpy(strTheToken,")");

                            chrTheChar=GetNextChar();
                            enumTokenState=token_complete;
                        break;

                        default:
                            enumTokenState=token_fail;
                    }

                break;

                case number_whole:
                    if(isdigit(chrTheChar))
                    {
                        if(!append(strTheToken,chrTheChar)){enumTokenState=token_fail;break;}
                        chrTheChar=GetNextChar();
                        break;
                    }

                    if(chrTheChar=='.')
                    {
                        if(!append(strTheToken,chrTheChar)){enumTokenState=token_fail;break;}
                        enumTokenState=number_fraction;
                        chrTheChar=GetNextChar();
                        break;
                    }

                    if(strcmp(strTheToken,"NUMBER{-")==0)
                    {
                        enumTokenState=token_fail;
                        break;
                    }

                    enumTokenState=token_complete;
                    enumExpecting=an_operator;
                    if(!append(strTheToken,'}')){enumTokenState=token_fail;break;}
                break;

                case number_fraction:
                    if(isdigit(chrTheChar))
                    {
                        if(!append(strTheToken,chrTheChar)){enumTokenState=token_fail;break;}

                        chrTheChar=GetNextChar();
                        break;
                    }

                    if(chrTheChar=='.')
                    {
                        strcpy(strTheToken,"TOKENFAIL");
                        enumTokenState=token_fail;
                        break;
                    }

                    enumTokenState=token_complete;
                    enumExpecting=an_operator;

                    if(!append(strTheToken,'}')){enumTokenState=token_fail;break;}
                break;

                case token_function:
                    if(isalnum(chrTheChar))
                    {
                        if(!append(strTheToken,chrTheChar)){enumTokenState=token_fail;break;}

                       chrTheChar=GetNextChar();
                       break;
                    }

                    char chrTheFuncToken = GetFuncToken(strTheToken);

                    //handle constants such as pi,e etc
                    switch(chrTheFuncToken)
                    {
                        case _PI:
                            strcpy(strTheToken,"NUMBER{3.14159265358979323846}");
                            goto Finish_Constant_Handling;
                        case _E:
                            strcpy(strTheToken,"NUMBER{2.71828182845904523536}");
                            goto Finish_Constant_Handling;
                        case _C:
                            strcpy(strTheToken,"NUMBER{299792458}");
                            goto Finish_Constant_Handling;
                        case _GAMMA:
                            strcpy(strTheToken,"NUMBER{0.57721566490153286060}");
                            goto Finish_Constant_Handling;
                        case _PHI:
                            strcpy(strTheToken,"NUMBER{1.61803398874989484820}");
                            goto Finish_Constant_Handling;

                            Finish_Constant_Handling:
                                enumTokenState=token_complete;
                                enumExpecting=an_operator;
                                blnAtLeastOneOperatorFound=1;  //maybe someone just wants to know the value of the constant.
                        break;

                        case '\0':
                            enumTokenState=token_fail;
                        break;

                        default:
                            strcpy(strTheToken,"");
                            append(strTheToken,chrTheFuncToken);

                            //chrTheChar=GetNextChar();
                            enumTokenState=token_complete;
                            blnAtLeastOneOperatorFound=1;
                        break;
                    }
                break;
                default:
                    return 0;
            }
        }

        if(enumTokenState==token_fail)
        {
            strcpy(strToken,"TOKENFAIL");
            return 0;
        }
        return 1;
    }

    double fltANumber=0;
    while(GetNextToken(strToken) && !blnFailedToEvaluate)
    {
        if(sscanf(strToken,"NUMBER{%lf}",&fltANumber))
        {
            //printf("%f ",fltANumber);
            operand_stack_push(fltANumber);
        }
        else
        {
            char chrToken=strToken[0];
            if(intStackPos>0)
        {
                //Nested Function!
                int isoperator(char chrAChar)
                {
                    switch(chrAChar)
                    {
                        case '+':
                        case '-':
                        case '*':
                        case '/':
                        case '^':
                        case '%':
                            return 1;
                        break;

                        default:
                            return 0;
                    }
                }

                //Nested Function!
                int isFunction(char chrAChar)
                {
                    int i;
                    for(i=0;i<MAX_FUNCS;i++)
                    {
                        if(arrFuncToken[i]==chrAChar)
                        return 1;
                    }

                    return 0;
                }

                if(isoperator(chrToken))
                //if(!(chrToken=='(' || chrToken==')'))
                {

                    //Nested function!
                    int compareOperators(char chrOp1, char chrOp2)
                    {
                        //for left associative operators such as exponentiation. May have to improve this.
                        if(chrOp1!='^' && chrOp2!='^')
                            return operatorPrecedence(chrOp1)<=operatorPrecedence(chrOp2);
                        else
                            return operatorPrecedence(chrOp1)<operatorPrecedence(chrOp2);
                    }

                    while(intStackPos>0 && isoperator(stack_peek()) && (compareOperators(chrToken,stack_peek())))
                    {
                        char chrAnOperator=stack_pop();
                        //printf("%c ",chrAnOperator);
                        operand_stack_push(doSingleCalculation(chrAnOperator));
                    }

                    stack_push(chrToken);
                }
                else
                {
                    switch(chrToken)
                    {
                        case '(':
                            stack_push(chrToken);
                        break;

                        case ')':
                            while((chrToken=stack_pop())!='(')
                            {
                                if(chrToken=='\0')
                                {
                                    blnFailedToEvaluate=1;
                                    break;
                                }

                                //printf("%c ",chrToken);
                                operand_stack_push(doSingleCalculation(chrToken));
                            }

                            // Check to see if the next token on the stack is a function
                            // if so then calculate it.
                            if(isFunction(stack_peek()))
                                operand_stack_push(doSingleCalculation(stack_pop()));

                        break;
                        default: //functions like sin, tan, etc
                            stack_push(chrToken);
                    }
                }
            }
            else
                stack_push(chrToken);
        }
    }

    if(strcmp(strToken,"TOKENFAIL")==0) goto DIE_QUIETLY;

    if(blnFailedToEvaluate) goto DIE_QUIETLY;

    //parenthesis balancing error
    if(stack_peek()=='(') goto DIE_QUIETLY;


    while(intStackPos>0)
    {
        char chrAnOperator=stack_pop();
        //printf("%c ",chrAnOperator);
        operand_stack_push(doSingleCalculation(chrAnOperator));
    }

    if(blnDivideByZero)
    {
        *fltResult=0;
        return -1;
    }

    if(blnInvalidInput)
    {
        *fltResult=0;
        return -2;
    }

    //without this it would output 123=123, which is correct but...
    if(!blnAtLeastOneOperatorFound) goto DIE_QUIETLY;

    if(strcmp(strToken,"done")==0);
    {
        if(intOperandStackPos==1)
        {
            *fltResult=operand_stack_peek();
            return 1;
        }
        else goto DIE_QUIETLY;
    }

    DIE_QUIETLY:
        *fltResult=0;
        return 0;
}
