/* $Id: ast.h,v 1.11 2023/10/05 04:22:00 leavens Exp $ */
#ifndef _AST_H
#define _AST_H
#include <stdbool.h>
#include "machine_types.h"
#include "file_location.h"

// types of ASTs (type tags)
typedef enum {
    block_ast, const_decls_ast, var_decls_ast, proc_decls_ast,
    const_decl_ast, const_defs_ast, const_def_ast, var_decl_ast,
    idents_ast, proc_decl_ast, stmt_ast, assign_stmt_ast, call_stmt_ast,
    begin_stmt_ast, if_stmt_ast, while_stmt_ast, read_stmt_ast,
    write_stmt_ast, skip_stmt_ast, stmts_ast, condition_ast,
    rel_op_condition_ast, odd_condition_ast, expr_ast,
    binary_op_expr_ast, token_ast, number_ast, ident_ast, empty_ast
} AST_type;

// The following types for structs named N_t
// are returned by the parser.
// The struct N_t is the type of information kept in the AST
// that is related to the nonterminal N in the abstract syntax.

// The generic struct type (generic_t) has the fields that
// should be in all alternatives for ASTs.
typedef struct {
    file_location *file_loc;
    AST_type type_tag; // says what field of the union is active
    void *next; // for lists
} generic_t;

// empty ::=
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
} empty_t;

// label ::= ident
typedef struct ident_s {
    file_location *file_loc;
    AST_type type_tag;
    struct ident_s *next; // for lists
    const char *name;
} ident_t;

// (possibly signed) numbers
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *text;
    word_type value;
} number_t;

// tokens as ASTs
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *text;
    int code;
} token_t;

// kinds of expressions
typedef enum { expr_bin, expr_ident, expr_number } expr_kind_e;

// forward declaration for expressions
struct expr_s;

// expr ::= expr arithOp expr
// arithOp ::= + | - | * | /
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    struct expr_s *expr1;
    token_t arith_op;
    struct expr_s *expr2;
} binary_op_expr_t;
    
// expr ::= expr arithOp expr | ident | number
typedef struct expr_s {
    file_location *file_loc;
    AST_type type_tag;
    expr_kind_e expr_kind;
    union {
	binary_op_expr_t binary;
	ident_t ident;
	number_t number;
    } expr;
} expr_t;

// kinds of conditions
typedef enum { ck_odd, ck_rel } condition_kind_e;

typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    expr_t expr;
} odd_condition_t;

typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    expr_t expr1;
    token_t rel_op;
    expr_t expr2;
} rel_op_condition_t;

// condition ::= odd expr | expr relOp expr
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    condition_kind_e cond_kind;
    union {
	odd_condition_t odd_cond;
	rel_op_condition_t rel_op_cond;
    } condition;
} condition_t;

// kinds of statements
typedef enum { assign_stmt, call_stmt, begin_stmt, if_stmt, while_stmt,
	       read_stmt, write_stmt, skip_stmt } stmt_kind_e;

// forward declaration of statement AST struct, for recursions
struct stmt_s;

// stmts ::= { stmt }
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    struct stmt_s *stmts;
} stmts_t;

// stmt ::= ident := expr
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *name;
    struct expr_s *expr;
} assign_stmt_t;

// stmt ::= call ident
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *name;
} call_stmt_t;

// stmt ::= begin { stmt } end
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    stmts_t stmts;
} begin_stmt_t;

// stmt ::= if condition then stmt else stmt
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    condition_t condition;
    struct stmt_s *then_stmt;
    struct stmt_s *else_stmt;
} if_stmt_t;

// stmt ::= whhile condition stmt
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    condition_t condition;
    struct stmt_s *body;
} while_stmt_t;

// stmt ::= read ident
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const char *name;
} read_stmt_t;

// stmt ::= write expr
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    expr_t expr;
} write_stmt_t;

// stmt ::= skip
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
} skip_stmt_t;

// stmt ::= assignStmt | callStmt | beginStmt | ifStmt
//        | whileStmt | readStmt | writeStmt | skip
typedef struct stmt_s {
    file_location *file_loc;
    AST_type type_tag;
    struct stmt_s *next; // for lists
    stmt_kind_e stmt_kind;
    union {
	assign_stmt_t assign_stmt;
	call_stmt_t call_stmt;
	begin_stmt_t begin_stmt;
	if_stmt_t if_stmt;
	while_stmt_t while_stmt;
	read_stmt_t read_stmt;
	write_stmt_t write_stmt;
	skip_stmt_t skip_stmt;
    } stmt;
} stmt_t;

// A forward declaration for blocks
struct block_s;

// procDecl ::= procedure ident block
typedef struct proc_decl_s {
    file_location *file_loc;
    AST_type type_tag;
    struct proc_decl_s *next; // for lists
    const char *name;
    struct block_s *block;
} proc_decl_t;

// proc-decls ::= { proc-decl }
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    proc_decl_t *proc_decls;
} proc_decls_t;

// idents ::= { ident }
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    ident_t *idents;
} idents_t;

// var-decl ::= var idents
typedef struct var_decl_s {
    file_location *file_loc;
    AST_type type_tag;
    struct var_decl_s *next; // for lists
    idents_t idents;
} var_decl_t;

// var-decls ::= { var-decl }
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    var_decl_t *var_decls;
} var_decls_t;

// constDef ::= ident = number
typedef struct const_def_s {
    file_location *file_loc;
    AST_type type_tag;
    struct const_def_s *next; // for lists
    ident_t ident;
    number_t number;
} const_def_t;

// const-defs ::= const-def | const-defs , const-def
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const_def_t *const_defs;
} const_defs_t;

// const-decl ::= const const-defs
typedef struct const_decl_s {
    file_location *file_loc;
    AST_type type_tag;
    struct const_decl_s *next; // for lists
    const_defs_t const_defs;
} const_decl_t;

// const-decls ::= { const-decl }
typedef struct {
    file_location *file_loc;
    AST_type type_tag;
    const_decl_t *const_decls;
} const_decls_t;

// block ::= const-decls var-decls proc-decls stmt
typedef struct block_s {
    file_location *file_loc;
    AST_type type_tag;
    const_decls_t const_decls;
    var_decls_t var_decls;
    proc_decls_t proc_decls;
    stmt_t stmt;
} block_t;

// program ::= block

// The AST definition used by the parser generator (bison)
typedef union AST_u {
    generic_t generic;
    block_t block;
    const_decls_t const_decls;
    const_decl_t const_decl;
    const_defs_t const_defs;
    const_def_t const_def;
    var_decls_t var_decls;
    var_decl_t var_decl;
    idents_t idents;
    proc_decls_t proc_decls;
    proc_decl_t proc_decl;
    stmt_t stmt;
    assign_stmt_t assign_stmt;
    call_stmt_t call_stmt;
    begin_stmt_t begin_stmt;
    if_stmt_t if_stmt;
    while_stmt_t while_stmt;
    read_stmt_t read_stmt;
    write_stmt_t write_stmt;
    skip_stmt_t skip_stmt;
    stmts_t stmts;
    condition_t condition;
    rel_op_condition_t rel_op_condition;
    odd_condition_t odd_condition;
    expr_t expr;
    binary_op_expr_t binary_op_expr;
    token_t token;
    number_t number;
    ident_t ident;
    empty_t empty;
} AST;

// Return the file location from an AST
extern file_location *ast_file_loc(AST t);

// Return the filename from the AST t
extern const char *ast_filename(AST t);

// Return the line number from the AST t
extern unsigned int ast_line(AST t);

// Return the type tag of the AST t
extern AST_type ast_type_tag(AST t);

// Return a pointer to a fresh copy of t
// that has been allocated on the heap
extern AST *ast_heap_copy(AST t);

// Return an AST for a block which contains the given ASTs.
extern block_t ast_block(const_decls_t const_decls, var_decls_t var_decls, proc_decls_t proc_decls, stmt_t stmt);

// Return an AST for an empty const decls
extern const_decls_t ast_const_decls_empty(empty_t empty);

// Return an AST for the const decls
extern const_decls_t ast_const_decls(const_decls_t const_decls,
				     const_decl_t const_decl);

// Return an AST for a const_decl
extern const_decl_t ast_const_decl(const_defs_t const_defs);

// Return an AST for const_defs
extern const_defs_t ast_const_defs_singleton(const_def_t const_def);

// Return an AST for const_defs
extern const_defs_t ast_const_defs(const_defs_t const_defs,
				   const_def_t const_def);

// Return an AST for a const-def
extern const_def_t ast_const_def(ident_t ident, number_t number);

// Return an AST for varDecls that are empty
extern var_decls_t ast_var_decls_empty(empty_t empty);

// Return an AST varDecls that have a var_decl
extern var_decls_t ast_var_decls(var_decls_t var_decls, var_decl_t var_decl);

// Return an AST for a var_decl
extern var_decl_t ast_var_decl(idents_t idents);

// Return an AST made for one ident
extern idents_t ast_idents_singleton(ident_t ident);

// Return an AST made for idents
extern idents_t ast_idents(idents_t idents, ident_t ident);

// Return an AST for proc_decls
extern proc_decls_t ast_proc_decls_empty(empty_t empty);

// Return an AST for proc_decls
extern proc_decls_t ast_proc_decls(proc_decls_t proc_decls,
				   proc_decl_t proc_decl);

// Return an AST for a proc_decl
extern proc_decl_t ast_proc_decl(ident_t ident, block_t block);

// Return an AST for a skip statement
extern skip_stmt_t ast_skip_stmt(file_location *file_loc); 

// Return an AST for a write statement
extern write_stmt_t ast_write_stmt(expr_t expr); 

// Return an AST for a read statement
extern read_stmt_t ast_read_stmt(ident_t ident); 

// Return an immediate data holding an address
extern while_stmt_t ast_while_stmt(condition_t condition, stmt_t body);

// Return an AST for an instruction
// with the given information
extern if_stmt_t ast_if_stmt(condition_t condition, stmt_t then_stmt,
			     stmt_t else_stmt);

// Return an AST for a begin statement
// containing the given list of statements
extern begin_stmt_t ast_begin_stmt(stmts_t stmts);

// Return an AST for a call statment
extern call_stmt_t ast_call_stmt(ident_t ident);

// Return an AST for an assignment statement
extern assign_stmt_t ast_assign_stmt(ident_t ident, expr_t expr);

// Return an AST for the list of statements 
extern stmts_t ast_stmts_singleton(stmt_t stmt);

// Return an AST for the list of statements 
extern stmts_t ast_stmts(stmts_t stmts, stmt_t stmt);

// Return an AST for the given statment
extern stmt_t ast_stmt_assign(assign_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_call(call_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_begin(begin_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_if(if_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_while(while_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_read(read_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_write(write_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_skip(skip_stmt_t s);

// Return an AST for an odd condition
extern odd_condition_t ast_odd_condition(expr_t expr);

// Return an AST for an initializer with the given value
extern rel_op_condition_t ast_rel_op_condition(expr_t expr1, token_t rel_op,
					       expr_t expr2);

// Return an AST for an odd condition
extern condition_t ast_condition_odd(odd_condition_t odd_cond);

// Return an AST for a relational condition
extern condition_t ast_condition_rel(rel_op_condition_t rel_op_cond);

// Return an AST for a binary op expression
extern binary_op_expr_t ast_binary_op_expr(expr_t expr1, token_t arith_op,
					   expr_t expr2);

// Return an expression AST for a binary operation expresion
extern expr_t ast_expr_binary_op(binary_op_expr_t e);

// Return an expression AST for an identifier
extern expr_t ast_expr_ident(ident_t e);

// Return an expression AST for a negated number
extern expr_t ast_expr_negated_number(token_t sign, number_t number);

// Return an expression AST for a positive number
extern expr_t ast_expr_pos_number(token_t sign, number_t number);

// Return an AST for the given token
extern token_t ast_token(file_location *file_loc, const char *text, int code);

// Return an AST for an identifier
// found in the file named fn, on line ln, with the given name.
extern ident_t ast_ident(file_location *file_loc, const char *name);

// Return an AST for a (signed) number with the given value
extern number_t ast_number(token_t sgn, word_type value);

// Return an AST for empty found in the given file location
empty_t ast_empty(file_location *file_loc);

// Requires: lst is a pointer to a non-circular 
//           linked list with next pointers
//           as in generic_t
// Return a pointer to the last element in lst.
// This only returns NULL if lst == NULL.
extern void *ast_last_list_elem(void *lst);

// Requires: lst is a pointer to a non-circular 
//           linked list with next pointers
//           as in generic_t
// Return the number of elements in the linked list lst
extern int ast_list_length(void *lst);

#endif
