//
// C++ Implementation: stringlist.cpp
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "stringlist.h"

#include <stdio.h>

#include <string>

stringlist::stringlist() {
    num = 0;
    strings = NULL;
}

// copy constructor
stringlist::stringlist(const stringlist &oldlist){
    /*
    num = oldlist.num;
    strings = new std::string[num];
    for(unsigned int i = 0; i < num; i++)
	strings[i] = oldlist.strings[i];
	*/
}

stringlist::~stringlist() {
    // clean up....
    for(unsigned int i = 0; i < num; i++)
        delete strings[i];
    if(strings)
        delete [] strings;
}

// string getString(unsigned int)
// return string of given index
std::string stringlist::getString(unsigned int index) {
    // return empty string....
    if(index >= num)
        return  "";
    return *strings[index];
}

// void addString(const string&)
// add a string to list
void stringlist::addString(const std::string &text) {
    // create new list
    std::string **newlist = new std::string*[num + 1];
    // copy over old list
    for(unsigned int i = 0; i < num; i++)
        newlist[i] = strings[i];
    // add a copy of the new string
    newlist[num] = new std::string(text);
    if(strings && num > 0)
        delete [] strings;
    strings = newlist;
    num ++;
}

// unsigned int count()
// return number of strings
unsigned int stringlist::count() {
    return num;
}
