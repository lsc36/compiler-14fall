#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// This file is for reference only, you are not required to follow the implementation. //

#define MAX_LEVEL_DEPTH 1024

int HASH(char * str) {
	int idx=0;
	while (*str){
		idx = idx << 1;
		idx+=*str;
		str++;
	}
	return (idx & (HASH_TABLE_SIZE-1));
}

SymbolTable symbolTable;

SymbolTableEntry* newSymbolTableEntry(int nestingLevel)
{
    SymbolTableEntry* symbolTableEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
    symbolTableEntry->nextInHashChain = NULL;
    symbolTableEntry->prevInHashChain = NULL;
    symbolTableEntry->nextInSameLevel = NULL;
    symbolTableEntry->sameNameInOuterLevel = NULL;
    symbolTableEntry->attribute = NULL;
    symbolTableEntry->name = NULL;
    symbolTableEntry->nestingLevel = nestingLevel;
    return symbolTableEntry;
}

SymbolTableEntry* makeSymbolTableEntry(int nestingLevel, char* name, SymbolAttribute* attr)
{
    SymbolTableEntry* symbolTableEntry = newSymbolTableEntry(nestingLevel);
	symbolTableEntry->name = name;
	symbolTableEntry->attribute = attr;
	return symbolTableEntry;
}


/*
 * 意義不明
void removeFromHashTrain(int hashIndex, SymbolTableEntry* entry)
{
}

void enterIntoHashTrain(int hashIndex, SymbolTableEntry* entry)
{
}
*/

void removeEntry(int hash, SymbolTableEntry* entry)
{
	//printf("try remove %s\n", entry->name);
	SymbolTableEntry* prev = entry->prevInHashChain;
	SymbolTableEntry* next = entry->nextInHashChain;
	SymbolTableEntry* down = entry->sameNameInOuterLevel;
	if (down == NULL) {
		if (next != NULL) {
			next->prevInHashChain = prev;
		}
		if (prev != NULL) {
			prev->nextInHashChain = next;
		}
		else {
			symbolTable.hashTable[hash]->nextInHashChain = next;
		}
	}
	else {
		down->prevInHashChain = prev;
		prev->nextInHashChain = down;
		down->nextInHashChain = next;
		if (next != NULL) {
			next->prevInHashChain = down;
		}
		if (prev != NULL) {
			prev->nextInHashChain = down;
		}
		else {
			symbolTable.hashTable[hash]->nextInHashChain = down;
		}
	}
	//printf("relink OK\n");
	free(entry);
}

void initializeSymbolTable()
{
	for (int i = 0; i < HASH_TABLE_SIZE; i++) {
		symbolTable.hashTable[i] = newSymbolTableEntry(0);
	}
	symbolTable.currentLevel = 0;
	// TODO: 先用大陣列矇混一下，有空再修改
	symbolTable.scopeDisplay = (SymbolTableEntry**)malloc(sizeof(SymbolTableEntry*) * MAX_LEVEL_DEPTH);
	for (int i = 0; i < MAX_LEVEL_DEPTH; i++) {
		symbolTable.scopeDisplay[i] = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
	}
	// 為什麼要算它大小？
	symbolTable.scopeDisplayElementCount = 0;
	
	
	// 先將 int, float, void 加入
	enterSymbol("int", NULL);
	enterSymbol("float", NULL);
	enterSymbol("void", NULL);
}

void freeEntryLink(SymbolTableEntry* s) {
	while (s != NULL) {
		SymbolTableEntry* s_save = s->nextInHashChain;
		free(s);
		s = s_save;
	}
}

void symbolTableEnd()
{
	for (int hash = 0; hash < HASH_TABLE_SIZE; hash++) {
		SymbolTableEntry* s = symbolTable.hashTable[hash];
		freeEntryLink(s);
	}
	// hashTable 跟 scopeDisplay 重複，所以只要刪除一個就好
}

SymbolTableEntry* retrieveSymbol(char* symbolName)
{
	int hash = HASH(symbolName);
	SymbolTableEntry* s = symbolTable.hashTable[hash]->nextInHashChain;
	while (s != NULL) {
		if (strcmp(symbolName, s->name) == 0) {
			return s;
			break;
		}
		s = s->nextInHashChain;
	}
	return NULL;
}

void addNewSymbolName(int hash, SymbolTableEntry* newEntry)
{
	SymbolTableEntry *s = symbolTable.hashTable[hash]->nextInHashChain;
	newEntry->nextInHashChain = s;
	if (s != NULL) {
		s->prevInHashChain = newEntry;
	}
	symbolTable.hashTable[hash]->nextInHashChain = newEntry;
	newEntry->prevInHashChain = symbolTable.hashTable[hash];
	
	newEntry->nextInSameLevel = symbolTable.scopeDisplay[symbolTable.currentLevel]->nextInSameLevel;
	symbolTable.scopeDisplay[symbolTable.currentLevel]->nextInSameLevel = newEntry;
}

SymbolTableEntry* enterSymbol(char* symbolName, SymbolAttribute* attribute)
{
	int hash = HASH(symbolName);
	SymbolTableEntry* newEntry = makeSymbolTableEntry(symbolTable.currentLevel, symbolName, attribute);
	SymbolTableEntry* s = symbolTable.hashTable[hash]->nextInHashChain;
	while (s != NULL) {
		if (strcmp(symbolName, s->name) == 0) {
			//printf("variable %s cover outer level\n", s->name);
			newEntry->nextInHashChain = s->nextInHashChain;
			newEntry->prevInHashChain = s->prevInHashChain;
			if (s->nextInHashChain != NULL) {
				s->nextInHashChain->prevInHashChain = newEntry;
			}
			s->prevInHashChain->nextInHashChain = newEntry;
			newEntry->sameNameInOuterLevel = s;
			newEntry->nextInSameLevel = symbolTable.scopeDisplay[symbolTable.currentLevel]->nextInSameLevel;
			symbolTable.scopeDisplay[symbolTable.currentLevel]->nextInSameLevel = newEntry;
			return newEntry;
		}
		s = s->nextInHashChain;
	}
	// name 沒有出現過，自己新增
	addNewSymbolName(hash, newEntry);
	return newEntry;
}

//remove the symbol from the current scope
// 不是只有在 close scope 時調用？
void removeSymbol(char* symbolName)
{
	int hash = HASH(symbolName);
	SymbolTableEntry* s = symbolTable.hashTable[hash]->nextInHashChain;
	while (s != NULL) {
		if (strcmp(s->name, symbolName) == 0) {
			removeEntry(hash, s);
			return;
		}
		s = s->nextInHashChain;
	}
}

int declaredLocally(char* symbolName)
{
	int hash = HASH(symbolName);
	SymbolTableEntry* s = symbolTable.hashTable[hash]->nextInHashChain;
	while (s != NULL) {
		if (strcmp(s->name, symbolName) == 0 && s->nestingLevel == symbolTable.currentLevel) {
			return 1;
		}
		s = s->nextInHashChain;
	}
	return 0;
}

void openScope()
{
	symbolTable.currentLevel++;
	symbolTable.scopeDisplay[symbolTable.currentLevel]->nextInSameLevel = NULL;
}

void closeScope()
{
	SymbolTableEntry* s = symbolTable.scopeDisplay[symbolTable.currentLevel]->nextInSameLevel;
	while (s != NULL) {
		SymbolTableEntry *ss = s->nextInSameLevel;
		removeEntry(HASH(s->name), s);
		s = ss;
	}
	symbolTable.currentLevel--;
}
