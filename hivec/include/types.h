
/**
 * @file types.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-05
 */

#ifndef _TYPES_H_
#define _TYPES_H_

enum _Storage
{
	STORAGE_BOOL,
	STORAGE_CHAR,
	STORAGE_F32,
	STORAGE_F64,
	STORAGE_I16,
	STORAGE_I32,
	STORAGE_I64,
	STORAGE_I8,
	STORAGE_NULL,
	STORAGE_RUNE,
	STORAGE_STRING,
	STORAGE_U16,
	STORAGE_U32,
	STORAGE_U64,
	STORAGE_U8,
	STORAGE_UINTPTR,
	STORAGE_VOID,

	STORAGE_ALIAS,
	STORAGE_ARRAY,
	STORAGE_ENUM,
	STORAGE_PROCEDURE,
	STORAGE_POINTER,
	STORAGE_STRUCT,
	STORAGE_FCONST,
	STORAGE_ICONST,
	STORAGE_RCONST,
};

struct _Identifier;

struct _TypeAlias
{
	struct _Identifier;
	bool exported;
};

struct _TypeArray
{
};

struct _TypeEnum
{
};

struct _TypeProcedure
{
};

struct _TypePointer
{
};

struct _TypeStruct
{
};

struct _TypeConst
{
};

struct _Type
{
};

#endif
