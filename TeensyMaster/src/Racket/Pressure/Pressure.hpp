#ifndef PRESSURE
#define PRESSURE
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

		boolean isPressed(){

			if( m_pressure > 10) 
			{
				return true;

			} else {
				return false;
			}
		}
		

};
#endif /* PRESSURE */
