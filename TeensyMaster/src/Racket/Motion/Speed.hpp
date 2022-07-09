#ifndef SPEED_H
#define SPEED_H
#pragma once
#include "ReciverData.hpp"
//! This is a concrete class, for getting 
class Speed
{
private:
	ReciverData &m_reciverData;
	float _speed ;
	float m_speedSmooth ;
	float m_smoothSensitivity = 100;

public:
	Speed(ReciverData &reciverData) : m_reciverData(reciverData)
	{

	};

	void loop()
	{
		calculateTotal();
		smoothTotal();
	}

	auto smooth()                               { return m_speedSmooth; }
    auto setSmoothSensivity(int smoothSensivity){ return m_smoothSensitivity = smoothSensivity;}
    auto getTotal()                             { return _speed ;                              }

private: 

	void smoothTotal()
	{
		float sensitivity = 100; // a total of 540 degrees per second or greater between all axes will apply the loudest swing sound
		m_speedSmooth= min(1.0f, calculateTotal() / sensitivity);
		
	}

 

	float  calculateTotal()
	{
		_speed = sqrtf(powf(m_reciverData.gx, 2) + powf(m_reciverData.gy, 2) + powf(m_reciverData.gz, 2));
		return _speed;

	}
};
#endif

