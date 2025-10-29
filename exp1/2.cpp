#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <stdexcept>

using namespace std;

typedef enum { ADD, SUB, MUL, DIV, POW, FAC, L_P, R_P, EOE } Operator;
#define N_OPTR 9

const char pri[N_OPTR][N_OPTR] = {
    { '>','>','<','<','<','<','<','>','>' },
    { '>','>','<','<','<','<','<','>','>' },
    { '>','>','>','>','<','<','<','>','>' },
    { '>','>','>','>','<','<','<','>','>' },
    { '>','>','>','>','>','<','<','>','>' },
    { '>','>','>','>','>','>',' ','>','>' },
    { '<','<','<','<','<','<','<','=',' ' },
    { ' ',' ',' ',' ',' ',' ',' ',' ',' ' },
    { '<','<','<','<','<','<','<',' ','=' }
};

Operator opToEnum(char c) {
    switch (c) {
        case '+': return ADD;
        case '-': return SUB;
        case '*': return MUL;
        case '/': return DIV;
        case '^': return POW;
        case '!': return FAC;
        case '(': return L_P;
        case ')': return R_P;
        case '\0': return EOE;
        default: throw runtime_error("无效运算符");
    }
}

long long factorial(int n) {
    if (n < 0) throw runtime_error("阶乘仅支持非负整数");
    long long res = 1;
    for (int i = 1; i <= n; ++i) res *= i;
    return res;
}

long long calculate(long long num1, long long num2, Operator op) {
    switch (op) {
        case ADD: return num1 + num2;
        case SUB: return num1 - num2;
        case MUL: return num1 * num2;
        case DIV: 
            if (num2 == 0) throw runtime_error("除数不能为0");
            return num1 / num2;
        case POW: {
            long long res = 1;
            for (int i = 0; i < num2; ++i) res *= num1;
            return res;
        }
        case FAC: return factorial(num1);
        default: throw runtime_error("无效运算");
    }
}

long long evaluate(const string& expr) {
    stack<long long> numStack;
    stack<Operator> opStack;
    opStack.push(EOE);

    int i = 0;
    while (true) {
        char c = expr[i];
        if (isdigit(c)) {
            long long num = 0;
            while (i < expr.size() && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            numStack.push(num);
        } else {
            Operator op = opToEnum(c);
            Operator topOp = opStack.top();
            char relation = pri[topOp][op];

            switch (relation) {
                case '<':
                    opStack.push(op);
                    i++;
                    break;
                case '>':
                    opStack.pop();
                    if (topOp == FAC) {
                        long long num = numStack.top();
                        numStack.pop();
                        numStack.push(calculate(num, 0, topOp));
                    } else {
                        long long num2 = numStack.top(); numStack.pop();
                        long long num1 = numStack.top(); numStack.pop();
                        numStack.push(calculate(num1, num2, topOp));
                    }
                    break;
                case '=':
                    opStack.pop();
                    i++;
                    break;
                default:
                    throw runtime_error("表达式格式错误");
            }

            if (opStack.top() == EOE && i >= expr.size()) break;
        }
    }

    if (numStack.size() != 1) throw runtime_error("表达式格式错误");
    return numStack.top();
}

int main() {
    string tests[] = {
        "3+5*2",
        "(3+5)*2",
        "5^2",
        "5!",
        "((3+2)!-4)*2"
    };

    for (const string& s : tests) {
        try {
            cout << s << " = " << evaluate(s) << endl;
        } catch (const exception& e) {
            cout << s << " 错误: " << e.what() << endl;
        }
    }

    return 0;
}