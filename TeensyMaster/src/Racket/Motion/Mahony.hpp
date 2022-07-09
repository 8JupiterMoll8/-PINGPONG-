#ifndef MAHONY_H
#define MAHONY_H
#pragma once
#include "SensorFusion.h"
#include "ReciverData.hpp"
class Mahony
{
private:
	ReciverData  &m_reciverData;
	SF &m_fusion;
    float m_roll, m_pitch, m_yaw;
public:
	Mahony(ReciverData &reciverData, SF &fusion ) : m_reciverData(reciverData),m_fusion(fusion)
	{
	}

	void loop()
	{
		calculateRollPitchYaw(m_reciverData.gx, m_reciverData.gy, m_reciverData.gz, m_reciverData.ax, m_reciverData.ay, m_reciverData.az);
	}

	float roll()
	{
		return m_roll;

	}

	float pitch()
	{
		return m_pitch;

	}
	float yaw()
	{
		return m_yaw;

	}

	void debug()
	{
	     Serial.print(" ");
		  Serial.print(m_yaw);
		  Serial.print(" ");
		  Serial.print(m_pitch);
		  Serial.print(" ");
		  Serial.println(m_roll);
	}

private:
	void calculateRollPitchYaw(float gx, float gy, float gz, float ax, float ay, float az)
	{

	    float deltat;
		deltat = m_fusion.deltatUpdate();						 //this have to be done before calling the fusion update
		m_fusion.MahonyUpdate(gx, gy, gz, ax, ay, az, deltat);   //mahony is suggested if there isn't the mag and the mcu is slow

		
		m_roll  = m_fusion.getRoll();
		m_pitch = m_fusion.getPitch();
		m_yaw   = m_fusion.getYaw();
	}
};
#endif