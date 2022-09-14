
/**
 * @file compiler.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-14
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <lexer.h>

/**
 * @addtogroup compiler
 * 
 * @{
 */

void Compiler_compileToNASM(
	const char* path,
	const char* target);

/**
 * @}
 */

#endif
