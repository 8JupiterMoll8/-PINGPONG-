#ifndef INPUTSENSORSMOOTH_H
#define INPUTSENSORSMOOTH_H
#pragma once
#include "ResponsiveAnalogRead.h"
#include "I_InputSensorBhv.hpp"

class InputSensorSmooth : public I_InputSensorBhv
{
	private:
	ResponsiveAnalogRead &m_smoothAnalogRead;

	public:

		InputSensorSmooth(ResponsiveAnalogRead &smoothAnalogRead ):m_smoothAnalogRead(smoothAnalogRead)
		{

		};

		virtual void loop() override
		{
			
			m_smoothAnalogRead.update();

		}

		virtual int getValue () override
		{
			return m_smoothAnalogRead.getValue();
		}	

};
#endif