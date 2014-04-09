#include <sstream>

#include "Student.h"

Student::Student(const string& name, long matNumber) 
    : Person(name), matNumber_(matNumber)
{
}


Student::~Student(void)
{
}

long Student::getMatNumber() const
{
    return matNumber_;
}

string Student::saySomething() const 
{
    string number;
    stringstream strstream;
    strstream << matNumber_;
    strstream >> number;
    return "My martikelnumber is " + number;
}