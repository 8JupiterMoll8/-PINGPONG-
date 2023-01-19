#ifndef I_INPUTSENSORBHV_H
#define I_INPUTSENSORBHV_H
#pragma once
	
class I_InputSensorBhv  
{
	private:
	 int m_sensorValue;

	public:

		I_InputSensorBhv(){};
		~I_InputSensorBhv(){};
		virtual void loop()    = 0;
		virtual int getValue() = 0;

};
#endif