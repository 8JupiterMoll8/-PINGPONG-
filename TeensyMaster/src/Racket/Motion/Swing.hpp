#ifndef SWING_H
#define SWING_H
#pragma once
#include "ReciverData.hpp"
class Swing
{

public:
	Swing(ReciverData &ReciverData) : m_reciverData(ReciverData) {}
	
public:
	void loop(){
		 calculateSwing(nCurrentTime, m_reciverData.gx, m_reciverData.gy, m_reciverData.gz);
	}

public:
	auto A_Gain()   { return m_A_Gain;  }
    auto B_Gain()   { return m_B_Gain;  }
	auto Hum_Gain() { return m_Hum_Gain;}

private:
	ReciverData &m_reciverData;
	elapsedMillis nCurrentTime;
    float m_A_Gain ;
	float m_B_Gain ;
	float m_Hum_Gain;


private:
	void calculateSwing(float msTime, float gx, float gy, float gz)
	{
    	float Sensitivity = 75; // a total of 540 degrees per second or greater between all axes will apply the loudest swing sound
		float totalSpeed  = sqrtf(powf(gx, 2) + powf(gy, 2) + powf(gz, 2));
		float Strength    = min(1.0f, totalSpeed / Sensitivity);

		float m_flMultiHumCyclePeriod = 2 * PI / 1; // a full 2PI transition takes 1 seconds
		float flCycleTime = (msTime / 1000.0f) * m_flMultiHumCyclePeriod;

		float A_Amount = (sinf(flCycleTime) + 1) / 2;
		float B_Amount = (sinf(flCycleTime + PI) + 1) / 2;

		//HUMM
		float MaxGainHum = .75; // 1 for loudest hum, or something less than 1 so swing sound is comparatively louder.
		float MinGainHum = .25;
		float Overdrive = .1;
        
		
		#define MAX_GAIN_SWING (1 - MinGainHum + Overdrive)

		m_A_Gain  = A_Amount * Strength * MAX_GAIN_SWING;
		m_B_Gain  = B_Amount * Strength * MAX_GAIN_SWING;
		m_Hum_Gain = MinGainHum + (1 - A_Amount * Strength - B_Amount * Strength) * (MaxGainHum - MinGainHum);

		//Serial.println(totalSpeed);
		//Serial.println(SwingStrength);
		// Serial.print(" ");
		// Serial.print(  A_Amount);
		// Serial.print(" ");
		// Serial.println(  B_Amount);
	}
};
#endif

