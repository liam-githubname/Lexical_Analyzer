/* $Id: ast.c,v 1.10 2023/10/05 04:22:00 leavens Exp $ */
#include <string.h>
#include <stdlib.h>
#include "utilities.h"
#include "ast.h"
#include "lexer.h"

// macros used below

#define ALLOCATE_AND_INIT_FIELD_PTR(TYPE, VALUE, FIELD) \
    { \
	TYPE *p = (TYPE *) malloc(sizeof(TYPE));	\
        if (p == NULL) {   \
	    bail_with_error("Unable to allocate space for a %s!", "TYPE"); \
	}							       \
        *p = VALUE; \
	ret.FIELD = p; \
    }


#define ALLOCATE_AND_SPLICE_IN(TYPE, VALUE, FIELD, OBJECT) \
    { \
	TYPE *p = (TYPE *) malloc(sizeof(TYPE)); \
	if (p == NULL) {   \
	    bail_with_error("Cannot allocate space for %s!", "TYPE"); \
	}  \
	*p = VALUE; \
	OBJECT.FIELD = p; \
        TYPE *last = ast_last_list_elem(OBJECT.FIELD); \
	if (last == NULL) {	\
	    ret.FIELD = p;	\
	} else {		\
	    last->next = p;	\
	} \
    }


// Return the file location from an AST
file_location *ast_file_loc(AST t) {
    return t.generic.file_loc;
}

// Return the filename from the AST t
const char *ast_filename(AST t) {
    return ast_file_loc(t)->filename;
}

// Return the line number from the AST t
unsigned int ast_line(AST t) {
    return ast_file_loc(t)->line;
}

// Return the type tag of the AST t
AST_type ast_type_tag(AST t) {
    return t.generic.type_tag;
}

// Return a pointer to a fresh copy of t
// that has been allocated on the heap
AST *ast_heap_copy(AST t) {
    AST *ret = (AST *)malloc(sizeof(AST));
    if (ret == NULL) {
	bail_with_error("Cannot allocate an AST heap copy!");
    }
    *ret = t;
    return ret;
}

// Return an AST for a block which contains the given ASTs.
block_t ast_block(const_decls_t const_decls, var_decls_t var_decls,
		  proc_decls_t proc_decls, stmt_t stmt)
{
    block_t ret;
    ret.file_loc = const_decls.file_loc;
    ret.type_tag = block_ast;
    ret.const_decls = const_decls;
    ret.var_decls = var_decls;
    ret.proc_decls = proc_decls;
    ret.stmt = stmt;
    return ret;
}

// Return an AST for an empty const decls
extern const_decls_t ast_const_decls_empty(empty_t empty)
{
    const_decls_t ret;
    ret.file_loc = empty.file_loc;
    ret.type_tag = const_decls_ast;
    return ret;
}


// Return an AST for the const decls
const_decls_t ast_const_decls(const_decls_t const_decls,
			      const_decl_t const_decl)
{
    const_decls_t ret;
    ret.file_loc = const_decls.file_loc;
    ret.type_tag = const_decls_ast;
    ALLOCATE_AND_SPLICE_IN(const_decl_t, const_decl, const_decls, const_decls);
    return ret;
}



// Return an AST for a const_decl
const_decl_t ast_const_decl(const_defs_t const_defs)
{
    const_decl_t ret;
    ret.file_loc = const_defs.file_loc;
    ret.type_tag = const_def_ast;
    ret.const_defs = const_defs;
    ret.next = NULL;
    return ret;
}

// Return an AST for const_defs
extern const_defs_t ast_const_defs_singleton(const_def_t const_def)
{
    const_defs_t ret;
    ret.file_loc = const_def.file_loc;
    ret.type_tag = const_defs_ast;
    ALLOCATE_AND_INIT_FIELD_PTR(const_def_t, const_def, const_defs);
    return ret;
}

// Return an AST for const_defs
extern const_defs_t ast_const_defs(const_defs_t const_defs,
				   const_def_t const_def)
{
    const_defs_t ret;
    ret.file_loc = const_defs.file_loc;
    ret.type_tag = const_defs_ast;
    ALLOCATE_AND_SPLICE_IN(const_def_t, const_def, const_defs, ret);
    return ret;
}

// Return an AST for a const-def
const_def_t ast_const_def(ident_t ident, number_t number)
{
    const_def_t ret;
    ret.file_loc = file_location_copy(ident.file_loc);
    ret.type_tag = const_def_ast;
    ret.next = NULL;
    ret.ident = ident;
    ret.number = number;
    return ret;
}


// Return an AST for varDecls that are empty
var_decls_t ast_var_decls_empty(empty_t empty)
{
    var_decls_t ret;
    ret.file_loc = empty.file_loc;
    ret.type_tag = var_decls_ast;
    ret.var_decls = NULL;
    return ret;
}

// Return an AST varDecls that have a var_decl
var_decls_t ast_var_decls(var_decls_t var_decls, var_decl_t var_decl)
{
    var_decls_t ret;
    ret.file_loc = var_decls.file_loc;
    ret.type_tag = var_decls_ast;
    ALLOCATE_AND_SPLICE_IN(var_decl_t, var_decl, var_decls, ret);
    return ret;
}

// Return an AST for a var_decl
var_decl_t ast_var_decl(idents_t idents)
{
    var_decl_t ret;
    ret.file_loc = idents.file_loc;
    ret.type_tag = var_decl_ast;
    ret.next = NULL;
    ret.idents = idents;
    return ret;
}


// Return an AST made for one ident
extern idents_t ast_idents_singleton(ident_t ident)
{
    idents_t ret;
    ret.file_loc = ident.file_loc;
    ret.type_tag = idents_ast;
    ALLOCATE_AND_INIT_FIELD_PTR(ident_t, ident, idents);
    return ret;
}

// Return an AST made for idents
extern idents_t ast_idents(idents_t idents, ident_t ident)
{
    idents_t ret;
    ret.file_loc = ident.file_loc;
    ret.type_tag = idents_ast;
    return ret;
}

// Return an AST for proc_decls
proc_decls_t ast_proc_decls_empty(empty_t empty)
{
    proc_decls_t ret;
    ret.file_loc = empty.file_loc;
    ret.type_tag = proc_decls_ast;
    ret.proc_decls = NULL;
    return ret;
}

// Return an AST for proc_decls
proc_decls_t ast_proc_decls(proc_decls_t proc_decls,
			    proc_decl_t proc_decl)
{
    proc_decls_t ret;
    ret.file_loc = proc_decls.file_loc;
    ret.type_tag = proc_decls_ast;
    ALLOCATE_AND_INIT_FIELD_PTR(proc_decl_t, proc_decl, proc_decls);
    return ret;
}

// Return an AST for a proc_decl
proc_decl_t ast_proc_decl(ident_t ident, block_t block)
{
    proc_decl_t ret;
    ret.file_loc = file_location_copy(ident.file_loc);
    ret.type_tag = proc_decl_ast;
    ret.next = NULL;
    ret.name = ident.name;
    ALLOCATE_AND_INIT_FIELD_PTR(block_t, block, block);
    return ret;
}

// Return an AST for a skip statement
skip_stmt_t ast_skip_stmt(file_location *file_loc) {
    skip_stmt_t ret;
    ret.file_loc = file_loc;
    ret.type_tag = skip_stmt_ast;
    return ret;
}

// Return an AST for a write statement
write_stmt_t ast_write_stmt(expr_t expr) {
    write_stmt_t ret;
    ret.file_loc = expr.file_loc;
    ret.expr = expr;
    return ret;
}

// Return an AST for a read statement
read_stmt_t ast_read_stmt(ident_t ident) {
    read_stmt_t ret;
    ret.file_loc = file_location_copy(ident.file_loc);
    ret.type_tag = read_stmt_ast;
    ret.name = ident.name;
    return ret;
}

// Return an immediate data holding an address
while_stmt_t ast_while_stmt(condition_t condition, stmt_t body) {
    while_stmt_t ret;
    ret.file_loc = condition.file_loc;
    ret.type_tag = while_stmt_ast;
    ret.condition = condition;
    ALLOCATE_AND_INIT_FIELD_PTR(stmt_t, body, body);
    return ret;
}

// Return an AST for an instruction
// with the given information
if_stmt_t ast_if_stmt(condition_t condition, stmt_t then_stmt,
		      stmt_t else_stmt)
{
    if_stmt_t ret;
    ret.file_loc = condition.file_loc;
    ret.type_tag = if_stmt_ast;
    ret.condition = condition;
    ALLOCATE_AND_INIT_FIELD_PTR(stmt_t, then_stmt, then_stmt);
    ALLOCATE_AND_INIT_FIELD_PTR(stmt_t, else_stmt, else_stmt);
    return ret;
}

// Return an AST for a begin statement
// containing the given list of statements
begin_stmt_t ast_begin_stmt(stmts_t stmts)
{
    begin_stmt_t ret;
    ret.file_loc = stmts.file_loc;
    ret.type_tag = begin_stmt_ast;
    ret.stmts = stmts;
    return ret;
}

// Return an AST for a call statment
 call_stmt_t ast_call_stmt(ident_t ident)
{
    call_stmt_t ret;
    ret.file_loc = file_location_copy(ident.file_loc);
    ret.type_tag = call_stmt_ast;
    ret.name = ident.name;
    return ret;
}

// Return an AST for an assignment statement
assign_stmt_t ast_assign_stmt(ident_t ident, expr_t expr)
{
    assign_stmt_t ret;
    ret.file_loc = file_location_copy(ident.file_loc);
    ret.type_tag = assign_stmt_ast;
    ret.name = ident.name;
    ALLOCATE_AND_INIT_FIELD_PTR(expr_t, expr, expr);
    return ret;
}

// Return an AST for the list of statements 
stmts_t ast_stmts_singleton(stmt_t stmt) {
    stmts_t ret;
    ret.file_loc = stmt.file_loc;
    ret.type_tag = stmts_ast;
    ALLOCATE_AND_INIT_FIELD_PTR(stmt_t, stmt, stmts);
    return ret;
}

// Return an AST for the list of statements 
stmts_t ast_stmts(stmts_t stmts, stmt_t stmt) {
    stmts_t ret;
    ret.file_loc = stmts.file_loc;
    ret.type_tag = stmts_ast;
    ret.stmts = stmts.stmts;
    stmt_t *s = (stmt_t *) malloc(sizeof(stmt_t));
    if (s == NULL) {
	bail_with_error("Cannot allocate space for %s!", "stmt_t");
    }
    stmt_t *last = ast_last_list_elem(ret.stmts);
    last->next = s;
    return ret;
}

// Return an AST for the given statment
stmt_t ast_stmt_assign(assign_stmt_t s)
{
    stmt_t ret;
    ret.file_loc = s.file_loc;
    ret.type_tag = stmt_ast;
    ret.next = NULL;
    ret.stmt_kind = assign_stmt;
    ret.stmt.assign_stmt = s;
    return ret;
}

// Return an AST for the given statment
stmt_t ast_stmt_call(call_stmt_t s)
{
    stmt_t ret;
    ret.file_loc = s.file_loc;
    ret.type_tag = stmt_ast;
    ret.next = NULL;
    ret.stmt_kind = call_stmt;
    ret.stmt.call_stmt = s;
    return ret;
}

// Return an AST for the given statment
stmt_t ast_stmt_begin(begin_stmt_t s)
{
    stmt_t ret;
    ret.file_loc = s.file_loc;
    ret.type_tag = stmt_ast;
    ret.next = NULL;
    ret.stmt_kind = begin_stmt;
    ret.stmt.begin_stmt = s;
    return ret;
}

// Return an AST for the given statment
stmt_t ast_stmt_if(if_stmt_t s)
{
    stmt_t ret;
    ret.file_loc = s.file_loc;
    ret.type_tag = stmt_ast;
    ret.next = NULL;
    ret.stmt_kind = if_stmt;
    ret.stmt.if_stmt = s;
    return ret;
}

// Return an AST for the given statment
stmt_t ast_stmt_while(while_stmt_t s)
{
    stmt_t ret;
    ret.file_loc = s.file_loc;
    ret.type_tag = stmt_ast;
    ret.next = NULL;
    ret.stmt_kind = while_stmt;
    ret.stmt.while_stmt = s;
    return ret;
}

// Return an AST for the given statment
stmt_t ast_stmt_read(read_stmt_t s)
{
    stmt_t ret;
    ret.file_loc = s.file_loc;
    ret.type_tag = stmt_ast;
    ret.next = NULL;
    ret.stmt_kind = read_stmt;
    ret.stmt.read_stmt = s;
    return ret;
}

// Return an AST for the given statment
stmt_t ast_stmt_write(write_stmt_t s)
{
    stmt_t ret;
    ret.file_loc = s.file_loc;
    ret.type_tag = stmt_ast;
    ret.next = NULL;
    ret.stmt_kind = write_stmt;
    ret.stmt.write_stmt = s;
    return ret;
}

// Return an AST for the given statment
stmt_t ast_stmt_skip(skip_stmt_t s)
{
    stmt_t ret;
    ret.file_loc = s.file_loc;
    ret.type_tag = stmt_ast;
    ret.next = NULL;
    return ret;
}

// Return an AST for an odd condition
odd_condition_t ast_odd_condition(expr_t expr)
{
    odd_condition_t ret;
    ret.file_loc = expr.file_loc;
    ret.type_tag = odd_condition_ast;
    ret.expr = expr;
    return ret;
}

// Return an AST for an initializer with the given value
rel_op_condition_t ast_rel_op_condition(expr_t expr1, token_t rel_op,
					expr_t expr2)
{
    rel_op_condition_t ret;
    ret.file_loc = expr1.file_loc;
    ret.type_tag = rel_op_condition_ast;
    ret.expr1 = expr1;
    ret.rel_op = rel_op;
    ret.expr2 = expr2;
    return ret;
}

// Return an AST for an odd condition
condition_t ast_condition_odd(odd_condition_t odd_cond)
{
    condition_t ret;
    ret.file_loc = odd_cond.file_loc;
    ret.type_tag = condition_ast;
    ret.cond_kind = ck_odd;
    ret.condition.odd_cond = odd_cond;
    return ret;
}

// Return an AST for a relational condition
condition_t ast_condition_rel(rel_op_condition_t rel_op_cond)
{
    condition_t ret;
    ret.file_loc = rel_op_cond.file_loc;
    ret.type_tag = condition_ast;
    ret.cond_kind = ck_rel;
    ret.condition.rel_op_cond = rel_op_cond;
    return ret;
}

// Return an AST for an odd condition
binary_op_expr_t ast_binary_op_expr(expr_t expr1, token_t arith_op,
				    expr_t expr2)
{
    binary_op_expr_t ret;
    ret.file_loc = expr1.file_loc;
    ret.type_tag = binary_op_expr_ast;

    ALLOCATE_AND_INIT_FIELD_PTR(expr_t, expr1, expr1);

    ret.arith_op = arith_op;
    
    ALLOCATE_AND_INIT_FIELD_PTR(expr_t, expr2, expr2);
    return ret;
}

// Return an expression AST for a binary operation expresion
expr_t ast_expr_binary_op(binary_op_expr_t e)
{
    expr_t ret;
    ret.file_loc = e.file_loc;
    ret.type_tag = expr_ast;
    ret.expr_kind = expr_bin;
    ret.expr.binary = e;
    return ret;
}

// Return an expression AST for an signed number
expr_t ast_expr_negated_number(token_t sign, number_t number)
{
    expr_t ret;
    ret.file_loc = file_location_copy(sign.file_loc);
    ret.type_tag = expr_ast;
    ret.expr_kind = expr_number;
    ret.expr.number = number;
    // negate the value
    ret.expr.number.value = - ret.expr.number.value;
    return ret;
}

// Return an expression AST for an signed number
expr_t ast_expr_pos_number(token_t sign, number_t number)
{
    expr_t ret;
    ret.file_loc = file_location_copy(sign.file_loc);
    ret.type_tag = expr_ast;
    ret.expr_kind = expr_number;
    ret.expr.number = number;
    return ret;
}

// Return an AST for the given token
token_t ast_token(file_location *file_loc, const char *text, int code)
{
    token_t ret;
    ret.file_loc = file_loc;
    ret.type_tag = token_ast;
    ret.text = text;
    ret.code = code;
    return ret;
}

// Return an AST for a (signed) number with the given value
number_t ast_number(token_t sgn, word_type value)
{
    number_t ret;
    ret.file_loc = file_location_copy(sgn.file_loc);
    ret.type_tag = number_ast;
    ret.value = value;
    return ret;
}

// Return an AST for an identifier
ident_t ast_ident(file_location *file_loc, const char *name)
{
    ident_t ret;
    ret.file_loc = file_loc;
    ret.type_tag = ident_ast;
    ret.name = name;
    return ret;
}

// Return an AST for an expression that's a binary expression
expr_t ast_expr_binary_op_expr(binary_op_expr_t e)
{
    expr_t ret;
    ret.file_loc = e.file_loc;
    ret.type_tag = binary_op_expr_ast;
    ret.expr_kind = expr_bin;
    ret.expr.binary = e;
    return ret;
}

// Return an AST for an expression that's an identifier
expr_t ast_expr_ident(ident_t e)
{
    expr_t ret;
    ret.file_loc = e.file_loc;
    ret.type_tag = expr_ast;
    ret.expr_kind = expr_ident;
    ret.expr.ident = e;
    return ret;
}

// Return an AST for an expression that's a number
expr_t ast_expr_number(number_t e)
{
    expr_t ret;
    ret.file_loc = e.file_loc;
    ret.type_tag = expr_ast;
    ret.expr_kind = expr_number;
    ret.expr.number = e;
    return ret;
}

// Return an AST for empty found in the given file location
empty_t ast_empty(file_location *file_loc)
{
    empty_t ret;
    ret.file_loc = file_loc;
    ret.type_tag = empty_ast;
    return ret;
}

// Requires: lst is a pointer to a non-circular 
//           linked list with next pointers
//           as in generic_t
// Return a pointer to the last element in lst.
// This only returns NULL if lst == NULL.
void *ast_last_list_elem(void *lst)
{
    if (lst == NULL) {
	return lst;
    }
    // assert lst is not NULL
    void *prev = NULL;
    while (lst != NULL) {
	prev = lst;
	lst = ((generic_t *)lst)->next;
    }
    // here lst == NULL;
    return prev;
}

// Requires: lst is a pointer to a non-circular 
//           linked list with next pointers
//           as in generic_t
// Return the number of elements in the linked list lst
int ast_list_length(void *lst)
{
    int ret = 0;
    generic_t *p = (generic_t *) lst;
    while (p != NULL) {
	p = p->next;
	ret++;
    }
    return ret;
}
