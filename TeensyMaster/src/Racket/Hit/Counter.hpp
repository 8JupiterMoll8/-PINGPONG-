#ifndef COUNTER_H
#define COUNTER_H
#pragma once

class Counter
{
private:
	int counter = 0;
	int m_startNumber =0;

public:
    Counter(){}
	Counter(int startNumber):m_startNumber(startNumber)
	{}

	void setStartNumber(int startNumber ) {  m_startNumber = startNumber; }
    int getSum(){ return counter; }
    void add()  { counter++;      }
	void reset(){ counter = 0;    }
};
#endif