#pragma once

#include <iostream>
using namespace std;

class Person 
{

private:
    string name_;

public: 
    Person(const string& name);
    virtual ~Person(void);

    const string& getName() const;
    void setName(const string& name);

    virtual string saySomething() const;
};