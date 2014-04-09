#pragma once

#include "person.h"

class Student : public Person
{

private:
    const long matNumber_;

public:
    Student(const string& name, long matNumber);
    virtual ~Student(void);

    long getMatNumber() const;

    virtual string saySomething() const;
};

