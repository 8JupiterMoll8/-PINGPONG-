#ifndef TABLE_H
#define TABLE_H
#pragma once
#include "Piezo.hpp"
class Table
{

private:
Piezo &piezo_;

public:
    Table(Piezo &piezo):piezo_(piezo)
	{}

    auto loop()        {        piezo_.loop();        }
    auto resetHitSum() {        piezo_.resetHitSum(); }
    auto hitPeak()    { return piezo_.hitPeak();     }
    auto isHit()  { return piezo_.isHit();       }
	auto hitSum()     { return piezo_.hitSum();      }
};
#endif