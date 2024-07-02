#pragma once

#include "common.h"

/** Enumerated Token Types.
 */
enum token_type_e {

    // Single-character tokens.
    TOKEN_LEFT_PAREN,           ///< "("
    TOKEN_RIGHT_PAREN,          ///< ")"
    TOKEN_LEFT_BRACE,           ///< "{"
    TOKEN_RIGHT_BRACE,          ///< "}"
    TOKEN_COMMA,                ///< ","
    TOKEN_DOT,                  ///< "."
    TOKEN_MINUS,                ///< "-"
    TOKEN_PLUS,                 ///< "+"
    TOKEN_SEMICOLON,            ///< ";"
    TOKEN_SLASH,                ///< "/"
    TOKEN_STAR,                 ///< "*"

    // One or two charcter tokens.
    TOKEN_BANG,                 ///< "!"
    TOKEN_BANG_EQUAL,           ///< "!="
    TOKEN_EQUAL,                ///< "="
    TOKEN_EQUAL_EQUAL,          ///< "=="
    TOKEN_GREATER,              ///< ">"
    TOKEN_GREATER_EQUAL,        ///< ">="
    TOKEN_LESS,                 ///< "<"
    TOKEN_LESS_EQUAL,           ///< "<="

    // Literals.
    TOKEN_IDENTIFIER,           ///< regex: [A-Za-z_][0-9A-Za-z_]*
    TOKEN_STRING,               ///< regex: "[^"]*"
    TOKEN_NUMBER,               ///< regex: [0-9]+ | [0-9]+\\.[0-9]+

    // Keywords
    TOKEN_AND,                  ///< "and"
    TOKEN_CLASS,                ///< "class"
    TOKEN_ELSE,                 ///< "else"
    TOKEN_FALSE,                ///< "false"
    TOKEN_FOR,                  ///< "for"
    TOKEN_FUN,                  ///< "fun"
    TOKEN_IF,                   ///< "if"
    TOKEN_NIL,                  ///< "nil"
    TOKEN_OR,                   ///< "or"
    TOKEN_PRINT,                ///< "print"
    TOKEN_RETURN,               ///< "return"
    TOKEN_SUPER,                ///< "super"
    TOKEN_THIS,                 ///< "this"
    TOKEN_TRUE,                 ///< "true"
    TOKEN_VAR,                  ///< "var"
    TOKEN_WHILE,                ///< "while"

    TOKEN_ERROR,                ///< not a valid token
    TOKEN_EOF,                  ///< end of the file
};

/** Data associated with each token.
 *
 * NOTE: Token structures are often returned by functions
 * directly (not via pointers).
 *
 * The lexeme at Token::start is commonly NOT terminated by a null
 * byte, as we are referring directly into the original source string.
 */
struct token_s {
    TokenType type;             ///< token type from the enumeration
    const char *start;          ///< pointer to the lexeme
    int length;                 ///< length of the lexeme
    int line;                   ///< line number of the token
};

/** State of the Scanner.
 *
 * This is internal to scanner.c and exposed here for use by
 * scanner_bist and scanner_post testing.
 */
struct scanner_s {
    const char *start;          ///< start of the current lexeme
    const char *current;        ///< next character to scan
    int line;                   ///< line number at current
};

extern void initScanner (
    const char *source);

extern Token scanToken (
    );

extern void postScanner (
    );
extern void bistScanner (
    );
