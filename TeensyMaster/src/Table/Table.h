#ifndef TABLE_H
#define TABLE_H
#pragma once
#include "Piezo.hpp"
class Table
{

private:
Piezo &_piezo;

public:
    Table(Piezo &piezo):_piezo(piezo)
	{}

    auto loop()        {        _piezo.loop();        }
    auto resetHitSum() {        _piezo.resetHitSum(); }
    auto hitPeak()    { return _piezo.hitPeak();     }
    auto isHit()  { return _piezo.isHit();       }
	auto hitSum()     { return _piezo.hitSum();      }
};
#endif