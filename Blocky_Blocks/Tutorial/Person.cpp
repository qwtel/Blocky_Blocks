#pragma once

#include <iostream>
using namespace std;

#include "Person.h"

Person::Person(const string& name) : name_(name)
{

}

Person::~Person(void)
{
}

const string& Person::getName() const
{
    return name_;
}

void Person::setName(const string& name)
{
    name_ = name;
}

string Person::saySomething() const
{
    return (string("Hi, I'm ") += name_) += ". ";
}
