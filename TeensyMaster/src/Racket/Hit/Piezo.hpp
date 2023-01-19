#ifndef PIEZO_H
#define PIEZO_H
#pragma once
	
#include "PeaKDetector.hpp"
#include "Counter.hpp"
#include "I_InputSensorBhv.hpp"

class Piezo
{
private:
   PeakDetector         &_peakDetecor;
   Counter              &_counter;
   I_InputSensorBhv     *_inputValue;
   int _piezoInput = 0;
   

public:
 
	Piezo(PeakDetector &peakDetector,Counter &counter,I_InputSensorBhv &inputValue):
	_peakDetecor(peakDetector),
	_counter(counter),
    _inputValue(&inputValue) 
	{}


	void loop()
	{
		_inputValue->loop();
		_peakDetecor.loop();

        _piezoInput = _inputValue->getValue();
		
		_peakDetecor.setInput(_piezoInput);

		if(_peakDetecor.getHit())
		{
			_counter.add();
			//Serial.println("Hit");
			//Serial.println(m_counter.getSum());
		   
		}	

	}

	int hitPeak()
	{
		return _peakDetecor.getHitPeak();

	}

	void countHit()
	{
		_counter.add();
	}

	boolean isHit(){
		return _peakDetecor.getHit(); 
	}

	int hitSum()
	{
		return _counter.getSum();

	}

	void resetHitSum()
	{
		_counter.reset();
	}
};
#endif