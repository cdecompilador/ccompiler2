#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "util.h"

struct buf {
    size_t len;
    u8 *ptr;
};

int
is_in_bounds(struct buf b, size_t at)
{
    return (int)(at < b.len);
}

enum tok_type {
    TOK_UNKNOWN = 0,
    TOK_OPEN_PAREN,
    TOK_CLOSE_PAREN,
    TOK_OPEN_BRACE,
    TOK_CLOSE_BRACE,
    TOK_NUM,
    TOK_PLUS,
    TOK_MINUS,
    TOK_SLASH,
    TOK_RETURN,
    TOK_IF,
    TOK_ELSE,
    TOK_IDENT,
};

struct tok {
    enum tok_type ty;
    struct buf src;
};

void
print_token(struct tok t)
{
    const char* op;

    switch (t.ty)
    {
    case TOK_NUM: { printf("TOKEN_NUM("); } break;
    case TOK_CLOSE_PAREN: { printf("TOKEN(')')"); return; } break;
    case TOK_OPEN_PAREN: { printf("TOKEN('(')"); return; } break;
    case TOK_CLOSE_BRACE: { printf("TOKEN('}')"); return; } break;
    case TOK_OPEN_BRACE: { printf("TOKEN('{')"); return; } break;
    case TOK_PLUS: { printf("TOKEN('+')"); return; } break;
    case TOK_MINUS: { printf("TOKEN('-')"); return; } break;
    case TOK_SLASH: { printf("TOKEN('/')"); return; } break;
    case TOK_RETURN: { printf("TOK_RETURN"); return; } break;
    case TOK_IF: { printf("TOK_IF"); return; } break;
    case TOK_ELSE: { printf("TOK_ELSE"); return; } break;
    case TOK_IDENT: { printf("TOK_IDENT("); } break;
    case TOK_UNKNOWN: { printf("TOK_UNKNOWN{}"); return; } break;
    default:
        FAIL("unreachable");
    }

    printf("%.*s", t.src.len, t.src.ptr);
    printf(")");
}

int sv_currp, sv_mark;

struct scanner {
    struct buf src;
    int currp, mark;
} s;

int next(char *res)
{
    if (!is_in_bounds(s.src, s.currp))
        return false;

    if (res != NULL)
        *res = *(s.src.ptr + s.currp);

    s.currp++;

    return true;
}

int peek(char *res)
{
    if (!is_in_bounds(s.src, s.currp))
        return false;

    if (res != NULL)
        *res = *(s.src.ptr + s.currp);

    return true;
}

void
mark(void)
{
    s.mark = s.currp;
}

int
endmark(struct buf *b)
{
    size_t l;

    if (!b || s.currp - s.mark <= 0)
        return false;

    l = s.currp - s.mark;

    b->ptr = s.src.ptr + s.mark;
    b->len = l;

    return true;
}

void
ssave(void)
{
    sv_currp = s.currp;
    sv_mark = s.mark;
}

void
srestore(void)
{

    s.currp = sv_currp;
    s.mark = sv_mark;
}

void
scanner(const char* str)
{
    s = (struct scanner) { 
        .src = (struct buf) { .ptr = str, .len = strlen(str) },
        .currp = 0
    };
}

int
scanint(struct tok *t)
{
    char c;
    struct buf b;

    while (true) {
        if (!peek(&c))
            return false;

        switch (c)
        {
        case '0':
        case '1': case '2': case '3': case '4': case '5': 
        case '6': case '7': case '8': case '9':
            next(NULL);
            continue;
        }

        break;
    }

    if (!endmark(&b))
        FAIL("mark failed");

    t->src = b;
    t->ty = TOK_NUM;

    return true;
}

int
identeq(struct buf b, const char *s)
{
    char c1, c2;

    /* FIXME: This `strlen` could be cached somehow to avoid repeting this
     * calculation per token */
    if (strlen(s) != b.len) 
        return false;

    for (int i = 0; i < b.len; i++) {
        c1 = *(s + i);
        c2 = *(b.ptr + i);

        if (c1 != c2 && c1 != tolower(c2))
            return false;
    }

    return true;
}

const char *kws[] = {
    "return", "if", "else",
};

const enum tok_type kws_tokty[] = {
    TOK_RETURN, TOK_IF, TOK_ELSE
};

int
scan_ident_kw(struct tok *t)
{
    char c;
    struct buf b;

    while (true) {
        if (!peek(&c)) return false;
        if (isalpha((int)c) || isdigit((int)c)) {
            next(&c);
            continue;
        }
        break;
    }

    if (!endmark(&b))
        FAIL("mark failed");

    for (int i = 0; i < countof(kws); i++)
        if (identeq(b, kws[i])) {
            t->ty = kws_tokty[i];
            break;
        }

    t->src = b;
    t->ty = (t->ty == TOK_UNKNOWN) ? TOK_IDENT : t->ty;

    return true;
}

#define STOK_CASE(c, tok_enum)\
    case c: {\
        if (!endmark(&b)) FAIL("mark failed");\
        t->ty = tok_enum;\
        t->src = b;\
        return true;\
    } break

int
matchtok(char c, struct tok *t)
{
    struct buf b;

    t->ty = TOK_UNKNOWN;

skipws:
    switch (c)
    {
    case ' ': case '\n':
    case '\r': case '\t':
        mark();
        if (!next(&c)) 
            return false;
        goto skipws;
    case '1': case '2': case '3': case '4': case '5': 
    case '6': case '7': case '8': case '9': {
        if (!scanint(t))
            FAIL("expected {integer}");
    } break;
    STOK_CASE('+', TOK_PLUS);
    STOK_CASE('-', TOK_MINUS);
    STOK_CASE('/', TOK_SLASH);
    STOK_CASE('(', TOK_OPEN_PAREN);
    STOK_CASE(')', TOK_CLOSE_PAREN);
    }

    if (isalpha((int)c))
        if (!scan_ident_kw(t))
            FAIL("expected identifier");

    if (t->ty == TOK_UNKNOWN) 
        FAIL("unexpected character %c", c);

    return true;
}

#undef STOK

int
nexttok(struct tok *t)
{
    char c;

    mark();
    
    if (!next(&c))
        return false;

    return matchtok(c, t);
}

int
peektok(struct tok *t)
{
    int r;

    ssave();

    r = nexttok(t);

    srestore();
    
    return r;
}

enum unit_ty {
    UNIT_INT,
    UNIT_STR
};

struct strlit {
    struct buf s;
};

struct intlit {
    u32 val;
};

struct unit {
    enum unit_ty ty;
    union {
        struct strlit strlit;
        struct intlit intlit;
    };
};

int
parse_int(struct buf *b, u32 *val)
{
    int d;

    *val = 0;

    for (int i = 0; i < b->len; i++) {
        d = *(b->ptr + i) - '0';
        *val = 10 * (*val) + d;
    }

    return true;
}

/* Look ahead token */
struct tok lh;

int
match(enum tok_ty ty)
{
    if (ty == lh.ty) 
        return nexttok(&lh);
    else
        return false;
}

int
unit(struct unit *u)
{
    u32 val;

    switch (lh.ty) {
    case TOK_NUM: {
        match(TOK_NUM);

        if (!parse_int(&lh.src, &val))
            return false;
        u->ty = UNIT_INT;
        u->intlit = (struct intlit){ val };
    } break;
    default:
        FAIL("unexpected");
    }
    
    return true;
}

struct binop {
    struct expr *l, *r;
};

enum expr_ty {
    EXPR_ADD,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV,
    EXPR_EXPR,
    EXPR_UNIT,
};

struct expr {
    enum expr_ty ty;
    union {
        struct binop binop;
        struct expr *expr;
        struct unit unit;
    };
};

/*
E -> E + E | E - E
E -> E * E | E / E
E -> <num> | '(' E ')'

E -> E + T | E - T
T -> T * F | T / F
F -> <num> | '(' E ')'

E -> T E'
E' -> + T E' | - T E' | epsilon
T -> F T'
T' -> * F T' | / F T' | epsilon
F -> <num> | '(' E ')'

2 + 2

E -> T E' -> F T' E' 
    -> <num> T' E'
  -> <num> E' 
  -> <num> + T E' 
  -> <num> + F T' E'
  -> <num> + <num> 
*/

int
expr(struct expr *e)
{
    switch (lh.ty)
    {
    case TOK_NUM:
        unit(); operator(); expr();
    case TOK_OPEN_PAREN:
        match('('); expr(); match(')');
    }
}

/*
int
term(struct expr *e)
{
    factor(
}

int
_expr(struct expr *e)
{
    switch (lh.ty)
    {
    case TOK_NUM:
    case TOK_OPEN_PAREN:
        
    }
}

int
factor(struct expr *e)
{
    u32 val;

    switch (lh.ty)
    {
    case TOK_NUM:
        match(TOK_NUM);
        if (!parse_int(&lh.src, &val))
            FAIL("unexpected");
    case TOK_OPEN_PAREN:
        FAIL("todo");
    }
}

int
_term(struct expr *e)
{
    
}

#define PFAIL { srestore(); return false; }

int
parse_expr(struct expr *e)
{
    struct unit l, r;
    struct tok op;

    ssave();

    if (!parse_unit(&l)) {
        if (!nexttok(&op)) PFAIL;
    }

    return true;
}
*/

enum stmt_ty {
    STMT_ASSIGN,
    STMT_IF,
};

struct assign_stmt {
    struct buf tgt_name;
    struct expr expr;
};

struct if_stmt {
    struct expr cond;
    int has_else;
};

struct stmt {
    enum stmt_ty ty;
    union {
        struct assign_stmt assign_stmt;
        struct if_stmt if_stmt;
    };
};

struct program {
    struct stmt **stmt;
};

int
parse_program()
{
    return 0;
}

int
main(int argc, char **argv)
{
    char c;
    struct tok t, ts[100] = {};
    struct buf tgt_b;

    const char *src = "123 if hello + return ) ";
    scanner(src);
 
    /*
    while (true) {
        if (!nexttok(&t))
            break;

        print_token(t);
        printf("\n");
    }
    */

    peektok(&t);
    printf("%.*s\n", t.src.len, t.src.ptr);

    ssave();

    nexttok(&t);
    nexttok(&t);
    nexttok(&t);

    srestore();

    peektok(&t);
    printf("%.*s\n", t.src.len, t.src.ptr);

    /*
    struct unit u;

    parse_unit(&u);

    printf("%d\n", u.intlit.val);
    */

    // printf("%.*s\n", tgt_b.len, tgt_b.ptr);

    return 0;
}
