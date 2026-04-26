
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "parser.y"

/* ============================================================
   parser.y – Complete compiler front‑end with TAC generation
   ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "common.h"

#ifndef VARTYPE_DEFINED
#define VARTYPE_DEFINED
#endif

/* ------------------------------------------------------------------
   Forward declarations for the backend (code generator)
   ------------------------------------------------------------------ */
void generate_emu8086(const char *filename);


/* ------------------------------------------------------------------
   AST Node definitions
   ------------------------------------------------------------------ */
typedef enum {
    NODE_INT, NODE_FLOAT, NODE_ID,
    NODE_ADD, NODE_LT,
    NODE_ASSIGN,
    NODE_PRINTF,
    NODE_BLOCK,
    NODE_LOOP
} NodeKind;

typedef struct ASTNode {
    NodeKind kind;
    union {
        int    ival;                    /* NODE_INT */
        float  fval;                    /* NODE_FLOAT */
        char  *sval;                    /* NODE_ID */
        struct {
            struct ASTNode *left, *right;
        } bin;                          /* NODE_ADD, NODE_LT */
        struct {
            char *name;
            struct ASTNode *expr;
        } assign;                       /* NODE_ASSIGN */
        struct {
            char *fmt;
            struct ASTNode *expr;
        } print;                        /* NODE_PRINTF */
        struct {
            struct ASTNode *body, *cond;
        } loop;                         /* NODE_LOOP */
        struct {
            struct ASTNode **stmts;
            int cnt;
        } block;                        /* NODE_BLOCK */
    } d;
} ASTNode;

void generate_ast_dot(ASTNode *root, const char *filename);

/* Constructor prototypes */
ASTNode* make_int(int val);
ASTNode* make_float(float val);
ASTNode* make_id(char *name);
ASTNode* make_add(ASTNode *l, ASTNode *r);
ASTNode* make_lt(ASTNode *l, ASTNode *r);
ASTNode* make_assign(char *name, ASTNode *expr);
ASTNode* make_printf(char *fmt, ASTNode *expr);
ASTNode* make_block(ASTNode **stmts, int cnt);
ASTNode* make_loop(ASTNode *body, ASTNode *cond);
void append_stmt(ASTNode *block, ASTNode *stmt);

/* ------------------------------------------------------------------
   Symbol table (global scope only)
   ------------------------------------------------------------------ */

#define MAX_SYMBOLS 200
Symbol symtab[MAX_SYMBOLS];
int sym_cnt = 0;

int  sym_insert(char *name, VarType type, ASTNode *init, int is_temp);
Symbol* sym_lookup(char *name);
VarType sym_get_type(char *name);

/* ------------------------------------------------------------------
   Type inference for expressions
   ------------------------------------------------------------------ */
VarType node_type(ASTNode *n);

/* ------------------------------------------------------------------
   Three‑address code (TAC) structures and globals
   ------------------------------------------------------------------ */

#define MAX_QUADS 500
Quad quads[MAX_QUADS];
int nquad = 0;

#define MAX_CONST 100
Constant const_pool[MAX_CONST];
int nconst = 0;

int temp_counter = 0;
int label_counter = 0;

/* TAC helpers */
void emit_quad(OpCode op, char *dest, char *src1, char *src2, VarType dest_type);
char* new_temp(VarType type);
char* new_label(void);
char* new_int_const(int val);
char* new_float_const(float val);

/* Forward declarations for TAC generation */
char* gen_expr(ASTNode *node);
void  gen_stmt(ASTNode *stmt);
void  gen_program(ASTNode *root);

/* ------------------------------------------------------------------
   Root of the AST, set during parsing
   ------------------------------------------------------------------ */
ASTNode *ast_root = NULL;

/* ------------------------------------------------------------------
   Error counter
   ------------------------------------------------------------------ */
int error_count = 0;

/* Override yyerror to count errors */
void yyerror(const char *msg) {
    fprintf(stderr, "Syntax error: %s\n", msg);
    error_count++;
}

/* ------------------------------------------------------------------
   Flex / Bison interface
   ------------------------------------------------------------------ */
extern int yylex();
extern int yyparse();
extern FILE *yyin;



/* Line 189 of yacc.c  */
#line 216 "parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 209 of yacc.c  */
#line 143 "parser.y"

    #ifndef VARTYPE_DEFINED
    #define VARTYPE_DEFINED
    typedef enum { VAR_INT, VAR_FLOAT } VarType;
    #endif



/* Line 209 of yacc.c  */
#line 249 "parser.tab.c"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     FLOAT = 259,
     DO = 260,
     WHILE = 261,
     PRINTF = 262,
     ID = 263,
     STRING = 264,
     INT_NUM = 265,
     FLOAT_NUM = 266
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 154 "parser.y"

    int    ival;          /* INT_NUM */
    float  fval;          /* FLOAT_NUM */
    char  *str;           /* ID, STRING */
    struct ASTNode *node; /* expressions and statements */
    VarType dtype;        /* for Type nonterminal */



/* Line 214 of yacc.c  */
#line 287 "parser.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 299 "parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   45

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  21
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  11
/* YYNRULES -- Number of rules.  */
#define YYNRULES  21
/* YYNRULES -- Number of states.  */
#define YYNSTATES  48

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   266

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      18,    19,     2,    13,    20,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    15,
      12,    14,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    16,     2,    17,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,     7,    10,    16,    18,    20,    30,
      31,    34,    38,    40,    45,    53,    57,    61,    63,    65,
      67,    69
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      22,     0,    -1,    23,    26,    -1,    -1,    23,    24,    -1,
      25,     8,    14,    30,    15,    -1,     3,    -1,     4,    -1,
       5,    16,    27,    17,     6,    18,    30,    19,    15,    -1,
      -1,    27,    28,    -1,    27,     1,    15,    -1,    29,    -1,
       8,    14,    30,    15,    -1,     7,    18,     9,    20,    30,
      19,    15,    -1,    30,    13,    30,    -1,    30,    12,    30,
      -1,    31,    -1,     8,    -1,    10,    -1,    11,    -1,    18,
      30,    19,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   181,   181,   185,   186,   190,   200,   201,   205,   210,
     211,   212,   216,   217,   236,   253,   254,   255,   259,   270,
     271,   272
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "FLOAT", "DO", "WHILE", "PRINTF",
  "ID", "STRING", "INT_NUM", "FLOAT_NUM", "'<'", "'+'", "'='", "';'",
  "'{'", "'}'", "'('", "')'", "','", "$accept", "Program", "DeclList",
  "Declaration", "Type", "DoWhileStmt", "StmtList", "Stmt", "PrintfStmt",
  "Expr", "Term", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,    60,    43,    61,    59,   123,   125,    40,    41,
      44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    21,    22,    23,    23,    24,    25,    25,    26,    27,
      27,    27,    28,    28,    29,    30,    30,    30,    31,    31,
      31,    31
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     0,     2,     5,     1,     1,     9,     0,
       2,     3,     1,     4,     7,     3,     3,     1,     1,     1,
       1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     1,     6,     7,     0,     4,     0,     2,
       9,     0,     0,     0,     0,     0,     0,     0,    10,    12,
      18,    19,    20,     0,     0,    17,    11,     0,     0,     0,
       0,     0,     0,     5,     0,     0,     0,    21,    16,    15,
       0,    13,     0,     0,     0,     0,     8,    14
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,     7,     8,     9,    12,    18,    19,    24,
      25
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -24
static const yytype_int8 yypact[] =
{
     -24,     7,    -2,   -24,   -24,   -24,   -10,   -24,     8,   -24,
     -24,    18,     3,     4,    21,    20,    25,    15,   -24,   -24,
     -24,   -24,   -24,     4,    16,   -24,   -24,    31,     4,    23,
       6,     4,     4,   -24,    24,    22,     4,   -24,    29,   -24,
       4,   -24,    11,    14,    28,    30,   -24,   -24
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -24,   -24,   -24,   -24,   -24,   -24,   -24,   -24,   -24,   -23,
     -24
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      30,     4,     5,     6,    14,    35,    10,     3,    38,    39,
      15,    16,    20,    42,    21,    22,    11,    43,    31,    32,
      17,    29,    23,    31,    32,    37,    31,    32,    31,    32,
      44,    33,    13,    45,    31,    32,    26,    41,    27,    28,
      34,    36,    32,    46,    40,    47
};

static const yytype_uint8 yycheck[] =
{
      23,     3,     4,     5,     1,    28,    16,     0,    31,    32,
       7,     8,     8,    36,    10,    11,     8,    40,    12,    13,
      17,     6,    18,    12,    13,    19,    12,    13,    12,    13,
      19,    15,    14,    19,    12,    13,    15,    15,    18,    14,
       9,    18,    13,    15,    20,    15
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    22,    23,     0,     3,     4,     5,    24,    25,    26,
      16,     8,    27,    14,     1,     7,     8,    17,    28,    29,
       8,    10,    11,    18,    30,    31,    15,    18,    14,     6,
      30,    12,    13,    15,     9,    30,    18,    19,    30,    30,
      20,    15,    30,    30,    19,    19,    15,    15
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 181 "parser.y"
    { ast_root = (yyvsp[(2) - (2)].node); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 185 "parser.y"
    { (yyval.node) = NULL; ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 191 "parser.y"
    {
          if (!sym_insert((yyvsp[(2) - (5)].str), (yyvsp[(1) - (5)].dtype), (yyvsp[(4) - (5)].node), 0)) {
              fprintf(stderr, "Semantic error: redeclaration of '%s'\n", (yyvsp[(2) - (5)].str));
              error_count++;
          }
      ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 200 "parser.y"
    { (yyval.dtype) = VAR_INT; ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 201 "parser.y"
    { (yyval.dtype) = VAR_FLOAT; ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 206 "parser.y"
    { (yyval.node) = make_loop((yyvsp[(3) - (9)].node), (yyvsp[(7) - (9)].node)); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 210 "parser.y"
    { (yyval.node) = make_block(NULL, 0); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 211 "parser.y"
    { append_stmt((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)); (yyval.node) = (yyvsp[(1) - (2)].node); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 212 "parser.y"
    { yyerrok; ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 218 "parser.y"
    {
          Symbol *sym = sym_lookup((yyvsp[(1) - (4)].str));
          if (!sym) {
              fprintf(stderr, "Semantic error: undeclared variable '%s'\n", (yyvsp[(1) - (4)].str));
              error_count++;
          } else {
              VarType lhs = sym->type;
              VarType rhs = node_type((yyvsp[(3) - (4)].node));
              if (lhs != rhs) {
                  fprintf(stderr, "Semantic error: type mismatch in assignment to '%s'\n", (yyvsp[(1) - (4)].str));
                  error_count++;
              }
          }
          (yyval.node) = make_assign((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].node));
      ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 237 "parser.y"
    {
          char *fmt = (yyvsp[(3) - (7)].str);
          ASTNode *expr = (yyvsp[(5) - (7)].node);
          VarType etype = node_type(expr);
          int ok = 0;
          if (strcmp(fmt, "\"%d\"") == 0 && etype == VAR_INT) ok = 1;
          else if (strcmp(fmt, "\"%f\"") == 0 && etype == VAR_FLOAT) ok = 1;
          if (!ok) {
              fprintf(stderr, "Semantic error: format/type mismatch in printf\n");
              error_count++;
          }
          (yyval.node) = make_printf(fmt, expr);
      ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 253 "parser.y"
    { (yyval.node) = make_add((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 254 "parser.y"
    { (yyval.node) = make_lt((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 260 "parser.y"
    {
          if (!sym_lookup((yyvsp[(1) - (1)].str))) {
              fprintf(stderr, "Semantic error: undeclared variable '%s'\n", (yyvsp[(1) - (1)].str));
              error_count++;
              /* Provide a safe dummy node so further checks don't crash */
              (yyval.node) = make_int(0);
          } else {
              (yyval.node) = make_id((yyvsp[(1) - (1)].str));
          }
      ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 270 "parser.y"
    { (yyval.node) = make_int((yyvsp[(1) - (1)].ival)); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 271 "parser.y"
    { (yyval.node) = make_float((yyvsp[(1) - (1)].fval)); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 272 "parser.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); ;}
    break;



/* Line 1455 of yacc.c  */
#line 1678 "parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 275 "parser.y"


/* ============================================================
   AST constructors
   ============================================================ */
ASTNode* make_node(NodeKind kind) {
    ASTNode *n = (ASTNode*)malloc(sizeof(ASTNode));
    n->kind = kind;
    return n;
}

ASTNode* make_int(int val) {
    ASTNode *n = make_node(NODE_INT);
    n->d.ival = val;
    return n;
}

ASTNode* make_float(float val) {
    ASTNode *n = make_node(NODE_FLOAT);
    n->d.fval = val;
    return n;
}

ASTNode* make_id(char *name) {
    ASTNode *n = make_node(NODE_ID);
    n->d.sval = strdup(name);
    return n;
}

ASTNode* make_add(ASTNode *l, ASTNode *r) {
    ASTNode *n = make_node(NODE_ADD);
    n->d.bin.left = l;
    n->d.bin.right = r;
    return n;
}

ASTNode* make_lt(ASTNode *l, ASTNode *r) {
    ASTNode *n = make_node(NODE_LT);
    n->d.bin.left = l;
    n->d.bin.right = r;
    return n;
}

ASTNode* make_assign(char *name, ASTNode *expr) {
    ASTNode *n = make_node(NODE_ASSIGN);
    n->d.assign.name = strdup(name);
    n->d.assign.expr = expr;
    return n;
}

ASTNode* make_printf(char *fmt, ASTNode *expr) {
    ASTNode *n = make_node(NODE_PRINTF);
    n->d.print.fmt = strdup(fmt);
    n->d.print.expr = expr;
    return n;
}

ASTNode* make_block(ASTNode **stmts, int cnt) {
    ASTNode *n = make_node(NODE_BLOCK);
    if (cnt > 0) {
        n->d.block.stmts = malloc(cnt * sizeof(ASTNode*));
        memcpy(n->d.block.stmts, stmts, cnt * sizeof(ASTNode*));
        n->d.block.cnt = cnt;
    } else {
        n->d.block.stmts = NULL;
        n->d.block.cnt = 0;
    }
    return n;
}

void append_stmt(ASTNode *block, ASTNode *stmt) {
    if (!block || block->kind != NODE_BLOCK) return;
    block->d.block.stmts = realloc(block->d.block.stmts,
                                   (block->d.block.cnt + 1) * sizeof(ASTNode*));
    block->d.block.stmts[block->d.block.cnt++] = stmt;
}

ASTNode* make_loop(ASTNode *body, ASTNode *cond) {
    ASTNode *n = make_node(NODE_LOOP);
    n->d.loop.body = body;
    n->d.loop.cond = cond;
    return n;
}

/* ============================================================
   Symbol table operations
   ============================================================ */
int sym_insert(char *name, VarType type, ASTNode *init, int is_temp) {
    for (int i = 0; i < sym_cnt; i++)
        if (strcmp(symtab[i].name, name) == 0)
            return 0;               /* duplicate */
    if (sym_cnt >= MAX_SYMBOLS) {
        fprintf(stderr, "Symbol table overflow!\n");
        exit(1);
    }
    symtab[sym_cnt].name = strdup(name);
    symtab[sym_cnt].type = type;
    symtab[sym_cnt].init = init;
    symtab[sym_cnt].is_temp = is_temp;
    sym_cnt++;
    return 1;
}

Symbol* sym_lookup(char *name) {
    for (int i = 0; i < sym_cnt; i++)
        if (strcmp(symtab[i].name, name) == 0)
            return &symtab[i];
    return NULL;
}

VarType sym_get_type(char *name) {
    Symbol *s = sym_lookup(name);
    return s ? s->type : VAR_INT;
}

/* ============================================================
   Type inference for AST nodes
   ============================================================ */
VarType node_type(ASTNode *n) {
    switch (n->kind) {
        case NODE_INT:   return VAR_INT;
        case NODE_FLOAT: return VAR_FLOAT;
        case NODE_ID:    return sym_get_type(n->d.sval);
        case NODE_ADD: {
            VarType t1 = node_type(n->d.bin.left);
            VarType t2 = node_type(n->d.bin.right);
            return (t1 == VAR_FLOAT || t2 == VAR_FLOAT) ? VAR_FLOAT : VAR_INT;
        }
        case NODE_LT:    return VAR_INT;  /* comparison yields int (boolean) */
        default:         return VAR_INT;
    }
}

/* ============================================================
   Three‑address code generation
   ============================================================ */
void emit_quad(OpCode op, char *dest, char *src1, char *src2, VarType type) {
    if (nquad >= MAX_QUADS) {
        fprintf(stderr, "Too many quads!\n");
        exit(1);
    }
    quads[nquad].op = op;
    quads[nquad].dest = dest ? strdup(dest) : NULL;
    quads[nquad].src1 = src1 ? strdup(src1) : NULL;
    quads[nquad].src2 = src2 ? strdup(src2) : NULL;
    quads[nquad].dest_type = type;
    nquad++;
}

char* new_temp(VarType type) {
    char buf[20];
    sprintf(buf, "_t%d", temp_counter++);
    sym_insert(buf, type, NULL, 1);  /* register as temporary */
    return strdup(buf);
}

char* new_label(void) {
    char buf[20];
    sprintf(buf, "L%d", label_counter++);
    return strdup(buf);
}

char* new_int_const(int val) {
    char buf[20];
    sprintf(buf, "_ci%d", nconst);
    const_pool[nconst].label = strdup(buf);
    const_pool[nconst].type = VAR_INT;
    const_pool[nconst].val.ival = val;
    nconst++;
    return strdup(buf);
}

char* new_float_const(float val) {
    char buf[20];
    sprintf(buf, "_cf%d", nconst);
    const_pool[nconst].label = strdup(buf);
    const_pool[nconst].type = VAR_FLOAT;
    const_pool[nconst].val.fval = val;
    nconst++;
    return strdup(buf);
}

char* gen_expr(ASTNode *node) {
    switch (node->kind) {
        case NODE_INT:   return new_int_const(node->d.ival);
        case NODE_FLOAT: return new_float_const(node->d.fval);
        case NODE_ID: {
            if (!sym_lookup(node->d.sval))
                return strdup("_error_");
            return strdup(node->d.sval);
        }
        case NODE_ADD: {
            char *left = gen_expr(node->d.bin.left);
            char *right = gen_expr(node->d.bin.right);
            VarType type = node_type(node);
            char *dest = new_temp(type);
            emit_quad(OP_ADD, dest, left, right, type);
            return dest;
        }
        case NODE_LT: {
            char *left = gen_expr(node->d.bin.left);
            char *right = gen_expr(node->d.bin.right);
            char *dest = new_temp(VAR_INT);
            emit_quad(OP_LT, dest, left, right, VAR_INT);
            return dest;
        }
        default:
            fprintf(stderr, "Internal error: bad expr node\n");
            return strdup("_error_");
    }
}

void gen_stmt(ASTNode *stmt) {
    if (!stmt) return;
    switch (stmt->kind) {
        case NODE_ASSIGN: {
            char *rhs = gen_expr(stmt->d.assign.expr);
            VarType type = sym_get_type(stmt->d.assign.name);
            emit_quad(OP_ASSIGN, stmt->d.assign.name, rhs, NULL, type);
            break;
        }
        case NODE_PRINTF: {
            ASTNode *expr = stmt->d.print.expr;
            char *arg = gen_expr(expr);
            if (strstr(stmt->d.print.fmt, "%d"))
                emit_quad(OP_PRINT_INT, NULL, arg, NULL, VAR_INT);
            else
                emit_quad(OP_PRINT_FLOAT, NULL, arg, NULL, VAR_FLOAT);
            break;
        }
        case NODE_BLOCK:
            for (int i = 0; i < stmt->d.block.cnt; i++)
                gen_stmt(stmt->d.block.stmts[i]);
            break;
        case NODE_LOOP: {
            char *start = new_label();
            emit_quad(OP_LABEL, start, NULL, NULL, 0);
            gen_stmt(stmt->d.loop.body);
            char *cond = gen_expr(stmt->d.loop.cond);
            emit_quad(OP_JMPTRUE, NULL, cond, start, 0);
            break;
        }
        default:
            fprintf(stderr, "Internal error: bad stmt node\n");
    }
}

void gen_program(ASTNode *root) {
    /* First, generate initialisation code for declared variables */
    for (int i = 0; i < sym_cnt; i++) {
        if (symtab[i].is_temp) continue;  /* skip temporaries */
        if (symtab[i].init) {
            char *rhs = gen_expr(symtab[i].init);
            emit_quad(OP_ASSIGN, symtab[i].name, rhs, NULL, symtab[i].type);
        } else {
            /* Default initialisation: 0 for int, 0.0 for float */
            if (symtab[i].type == VAR_INT)
                emit_quad(OP_ASSIGN, symtab[i].name, new_int_const(0), NULL, VAR_INT);
            else
                emit_quad(OP_ASSIGN, symtab[i].name, new_float_const(0.0f), NULL, VAR_FLOAT);
        }
    }
    /* Then the program body */
    gen_stmt(root);
    emit_quad(OP_HALT, NULL, NULL, NULL, 0);
}

/* ------------------------------------------------------------------
   AST visualisation – Graphviz DOT format
   ------------------------------------------------------------------ */
static int node_id_counter = 0;

static void ast_to_dot_rec(FILE *f, ASTNode *n, int parent_id) {
    if (!n) return;
    int my_id = node_id_counter++;
    const char *shape = "ellipse";
    char label[256] = "";

    switch (n->kind) {
        case NODE_INT:
            snprintf(label, sizeof(label), "INT(%d)", n->d.ival);
            shape = "box";
            break;
        case NODE_FLOAT:
            snprintf(label, sizeof(label), "FLOAT(%g)", n->d.fval);
            shape = "box";
            break;
        case NODE_ID:
            snprintf(label, sizeof(label), "ID(%s)", n->d.sval);
            shape = "box";
            break;
        case NODE_ADD:
            snprintf(label, sizeof(label), "ADD");
            break;
        case NODE_LT:
            snprintf(label, sizeof(label), "LT");
            break;
        case NODE_ASSIGN:
            snprintf(label, sizeof(label), "ASSIGN(%s)", n->d.assign.name);
            break;
        case NODE_PRINTF: 
            /* fmt is like "%f", including the surrounding double quotes */
            char *raw = n->d.print.fmt;
            char clean[256];
            int j = 0;
            for (int i = 0; raw[i] != '\0'; i++) {
                if (raw[i] == '"') {
                    clean[j++] = '\\';   /* escape the quote for DOT */
                }
                clean[j++] = raw[i];
            }
            clean[j] = '\0';

            /* Now clean is \"%f\" */
            /* Assemble label: PRINTF(\"%f\") */
            snprintf(label, sizeof(label), "PRINTF(%s)", clean);
            shape = "diamond";
            break;
        case NODE_BLOCK:
            snprintf(label, sizeof(label), "BLOCK[%d]", n->d.block.cnt);
            shape = "tab";
            break;
        case NODE_LOOP:
            snprintf(label, sizeof(label), "DO-WHILE");
            shape = "house";
            break;
        default:
            snprintf(label, sizeof(label), "???");
            break;
    }

    fprintf(f, "  n%d [label=\"%s\", shape=%s];\n", my_id, label, shape);
    if (parent_id >= 0)
        fprintf(f, "  n%d -> n%d;\n", parent_id, my_id);

    /* Add children according to node kind */
    switch (n->kind) {
        case NODE_ADD:
        case NODE_LT:
            ast_to_dot_rec(f, n->d.bin.left, my_id);
            ast_to_dot_rec(f, n->d.bin.right, my_id);
            break;
        case NODE_ASSIGN:
            ast_to_dot_rec(f, n->d.assign.expr, my_id);
            break;
        case NODE_PRINTF:
            ast_to_dot_rec(f, n->d.print.expr, my_id);
            break;
        case NODE_BLOCK:
            for (int i = 0; i < n->d.block.cnt; i++)
                ast_to_dot_rec(f, n->d.block.stmts[i], my_id);
            break;
        case NODE_LOOP:
            ast_to_dot_rec(f, n->d.loop.body, my_id);
            ast_to_dot_rec(f, n->d.loop.cond, my_id);
            break;
        default:
            break;  /* leaf nodes */
    }
}

void generate_ast_dot(ASTNode *root, const char *filename) {
    if (!root) return;
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror(filename);
        return;
    }
    node_id_counter = 0;
    fprintf(f, "digraph AST {\n");
    fprintf(f, "  node [fontname=\"Courier\"];\n");
    ast_to_dot_rec(f, root, -1);
    fprintf(f, "}\n");
    fclose(f);
    printf("AST written to %s\n", filename);
}

/* ============================================================
   Main driver – parse, generate TAC, call backend
   ============================================================ */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    yyin = f;
    yyparse();
    fclose(f);

    if (error_count > 0) {
        fprintf(stderr, "Compilation aborted due to errors.\n");
        return 1;
    }

    /* Generate TAC */
    gen_program(ast_root);

    /* Generate Graphviz .dot file */
    if (ast_root) {
        generate_ast_dot(ast_root, "ast.dot");
    }   

    printf("Before generate_emu8086, symtab:\n");
    for (int i = 0; i < sym_cnt; i++)
        printf("  [%d] %s\n", i, symtab[i].name);

    /* Write emu8086 assembly */
    generate_emu8086("output.asm");

    return 0;
}
