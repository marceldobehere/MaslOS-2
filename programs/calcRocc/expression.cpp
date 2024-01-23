#include "expression.hpp"
#include "common_includes.hpp"
#include "vars.hpp"

bool isDigit(const char c) { return c <= '9' && c >= '0'; }

bool isDigitToken(Token t) { return t.type == '\0'; }

long parensLength(const char *s)
{
    long matched = 0;
    long length = 1;

    if (*s == '(')
    {
        matched++;
        s++;
    }

    while (matched != 0 && *s != '\0')
    {
        if (*s == '(')
        {
            matched++;
        }
        else if (*s == ')')
        {
            matched--;
        }

        length++;
        s++;
    }

    return length;
}

bool isUnary(const char c)
{
    switch (c)
    {
    case '+':
    case '-':
    case '/':
    case '*':
    case ')':
        return true;
    default:
        return false;
    }
}

double getPrec(const char c)
{
    switch (c)
    {
    case '+':
        return 1;
    case '-':
        return 1;
    case '/':
        return 2;
    case '*':
        return 2;
    default:
        return 0;
    }
}

long parseNum(const char *s, double *des)
{
    double num = 0.0;
    long index = 0;
    long floatingCount = 0;
    double floatingNum = 0.0;

    for (index = 0; isDigit(*s) || *s == '.'; ++index)
    {
        if (*s == '.')
        {
            floatingNum = 1.0;
            s++;
            index++;
            floatingCount += parseNum(s, &floatingNum);
            break;
        }

        num *= 10.0;
        num += (double)(*s - '0');
        s++;
    }

    for (int i = floatingCount; i; i--)
    {
        floatingNum /= 10;
    }

    *des = num + floatingNum;
    return index + floatingCount;
}

bool oper(const char op, double lhs, double rhs, double *res)
{
    switch (op)
    {
    case '+':
        *res = lhs + rhs;
        return true;
    case '-':
        *res = lhs - rhs;
        return true;
    case '*':
        *res = lhs * rhs;
        return true;
    case '/':
        if (rhs != 0)
            *res = lhs / rhs;
        else
            *res = 0;
        return true;
    case ')':
        *res = lhs;
        return true;
    default:
        return false;
    }
}

void *mkToken(const char type, double val)
{
    Token *t = new Token();
    t->type = type;
    t->val = val;
    return t;
}

bool tokenize(List<void *> *tokens, const char *s)
{
    while (*s != '\0')
    {
        if (isDigit(*s))
        {
            double num = 0.0;
            s += parseNum(s, &num);
            tokens->Add(mkToken('\0', num));
        }
        else if (isUnary(*s))
        {
            tokens->Add(mkToken(*s, 0));
            s++;
        }
        else if (*s == '(')
        {
            tokens->Add(mkToken('(', parensLength(s)));
            s++;
        }
        else if (*s == ')')
        {
            tokens->Add(mkToken(')', 0));
            s++;
        }
        else if (*s == ' ')
        {
            s++;
        }
        else
        {
            return false;
            s++;
        }
    }

    tokens->Add(mkToken(0, 0));
    return true;
}

bool parse(List<void *> *tokens, long prec, long index, double *res)
{
    double rhs = 0;
    double lhs = 0;
    char op = 0;
    *res = 0;

    if (((Token *)tokens->ElementAt(index))->type == '(')
    {
        if (!parse(tokens, 0, index + 1, &lhs))
            return false;
        index += ((Token *)tokens->ElementAt(index))->val;
        op = ((Token *)tokens->ElementAt(index))->type;
    }
    else
    {
        lhs = ((Token *)tokens->ElementAt(index))->val;
        index++;
        op = ((Token *)tokens->ElementAt(index))->type;
    }

    while (isUnary(((Token *)tokens->ElementAt(index))->type) &&
           getPrec(((Token *)tokens->ElementAt(index))->type) >= prec)
    {
        index++;
        if (!parse(tokens, getPrec(((Token *)tokens->ElementAt(index))->type) + 1,
                   index, &rhs))
            return false;

        if (!oper(op, lhs, rhs, &lhs))
            return false;
    }

    *res = lhs;
    return true;
}