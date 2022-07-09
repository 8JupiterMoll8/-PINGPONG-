#ifndef PRESSURE_H
#define PRESSURE_H
#pragma once
#include "ReciverData.hpp"
	
class Pressure  
{
	private:
	ReciverData &m_reciverData;
	int m_pressure;


	public:

		Pressure(ReciverData &reciverData):m_reciverData(reciverData)
		{

		}

		void loop()
		{
			m_pressure = m_reciverData.fsr;

		}

		int value()
		{
			return m_pressure;

		}
		

};
#endif