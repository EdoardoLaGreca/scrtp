# style

This file contains some rough style guidelines on how to write code in this repository.

The C standard used in the source code of this repo is ANSI C (a.k.a. C89/C90).

## Indentation

It is hereby strictly prohibited not to use tabs for indentation. In other words, the tab character is the only accepted character for indentation.

Below, a quote from the Linux kernel style guide.

> Now, some people will claim that having 8-character indentations makes the code move too far to the right, and makes it hard to read on a 80-character terminal screen. The answer to that is that if you need more than 3 levels of indentation, you’re screwed anyway, and should fix your program.

## Comments

The only way to make comments is by using the standard ANSI C syntax.

```C
/* I'm a comment */
```

### Code comments

Code comments *always* start with a lowercase letter because it is aesthetically pleasant (except when grammar rules state the opposite, e.g. the "I" pronoun).

Avoid writing long comments that contain periods or full stops.

Comments can be placed in various positions, based on which piece of code they relate to. As a rule of thumb, place comments in their most reasonable place so that it is mostly obvious what piece of code they concern, without damaging the code readability.

```C
/* this comment is OK and it relates to the block of code below*/
line1;
line2;
line3; /* this comment is OK and it relates to the line of code on the left */
/* this comment may be ok, depending on the context */

/* this comment is NOT OK */ line4;
```

### Function comments

Function comments are only written in header files.

Non-static functions must be well documented, without being excessively verbose. This means that comments must be written in a way that briefly, but clearly, explains what the function does and what it returns.

Function comments must be written above the function, as multi-line comments, and must start with a capital letter. Each line of the comment starts with an asterisk and the first line is left blank.

```C
/*
 * This function does this and that.
 * It returns this + that.
 */
myfunc_type myfunc(type1 this, type2 that);
```

It is preferable to write what the function does and what it returns in separate lines. However, it is not strictly required.

## Variables

Variables of the same type can be declared all in one line as well as in separate lines. This is not true if the type involves a pointer: in this case, each variable must be declared on a separate line.

If a variable's type involves pointers, the asterisk(s) are written on the side of the type (left side), not on the side of the variable's name.

```C
var_type var_name; /* OK */
var_type var_name1, var_name2; /* OK */
var_point* var_name; /* OK */
var_point* var_name1, * var_name2; /* NOT OK */
```

## If, Struct, etc...

Flow control statements like `if`, `while`, `for`, `do...while` are written all in the same way as shown below.

```C
if (condition) {
	code;
}
```

```C
while (condition) {
	code;
}
```

```C
for (init; condition; increment) {
	code;
}
```

```C
do {
	code;
} while (condition);
```

If the condition in `if`, `while`, or `do-while` is made of more than two smaller conditions chained together (i.e. comparison operations separated by logical operators), each condition, starting from the second, is written on its own line, which begins with the logical operator and is indented in the same way as the code following the condition. For clarity, you must leave a blank line between the last condition and the code following it. An example is provided below. If you need to use such a number of comparison operations in a `for` condition, use a `while` instead.

```C
/* example */
if (op1 > op2
	|| op3 < op2
	|| op3 > op1) {

	code;
}
```

The `switch...case` statement is a bit different as `case`s are vertically aligned to the switch keyword.

```C
switch (expr) {
case val1:
	break;
case val2:
case val3:
	break;
default:
	break;
}
```

Other statements and specifiers are written as follows.

```C
enum name {
	val1,
	val2,
	val3
};
```

```C
struct name {
	type1 field1;
	type2 field2;
	type3 field3;
};
```

```C
union name {
	type1 field1;
	type2 field2;
	type3 field3;
};
```

```C
typedef existing_type alias;
/* or */
typedef struct {
	type1 field1;
	type2 field2;
	type3 field3;
} typename;
```

## Functions

Functions are written in this way:

```C
func_type
func_name(type1 arg1, type2 arg2)
{
	code;
}
```

If the function is static, the `static` keyword goes before the function type, on the same line.

```C
static func_type
func_name(type1 arg1, type2 arg2)
{
	code;
}
```

In the header file, the function type and the rest of its header are written in the same line, with argument names left in place.

## Naming conventions

All the defined names (local variable names, function names, type names, structure names, enum names, etc...) must use [snake_case](https://en.wikipedia.org/wiki/Snake_case), except for global variables and macros defined using the `#define` directive, which also use snake_case but with all capital letters.

The name of non-static functions that are declared in a module (not in `main.c`) must begin with the module name in which they are defined. For example, if a non-static function called `my_func` is declared in the `mod` module (`mod.c` + `mod.h`), its actual name must be `mod_my_func`.   Static functions can have any name.

Avoid an excessive use of underscores. As a rule of thumb, if the name can be understood even without a certain underscore, that underscore can be removed.   Also, avoid at all using underscores before or after names.

```C
#define MACRO1 some_value

type1 GLOBAL_VAR;

void
myfunc()
{
	type2 local_var;
}
```

## Modules

A module is meant as a pair of source file (ending in `.c`) and a header file (ending in `.h`).

The order of the contents of header files and source files must be (from top to bottom):

 1. include guard (only in header files)
 2. `#define` directives for system/local header files
 3. `#include` directive(s) for system header files (using `<` and `>`)
 4. `#include` directive(s) for local header files (using `"`)
 5. `#define` directive(s)
 6. enumeration(s) (using `enum`)
 7. data structures(s) (using `struct`)
 8. type definition(s) (using `typedef`)
 9. global variable(s)
 10. static function(s) (only in source files)
 11. non-static function(s)

It is preferable to use the `#pragma once` include guard instead of the standard one because, even though it is not standard, it is quite widespread and easier to read and write.

A header file and its respective source file should have the same content order for better organization, but it is not mandatory.

## Commits

[Conventional Commits](https://www.conventionalcommits.org) must be used for commit messages.
