#ifndef INPUTSENSORRAW_H
#define INPUTSENSORRAW_H
#pragma once
#include "I_InputSensorBhv.hpp"
#include "ReciverData.hpp"
	
class InputSensorRaw  : public I_InputSensorBhv
{
	private:
	ReciverData &m_reciver;

	public:

		InputSensorRaw(ReciverData &reciverData):m_reciver(reciverData)
		{}

		virtual void loop()
		{
			// not used

		}

		virtual int getValue() override
		{
			return m_reciver.pz;

		}
		

};
#endif