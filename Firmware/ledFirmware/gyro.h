#ifndef GYRO
#define GYRO

#include "gyro_accel.h"
#include "settings.h"

#define dt 20                        // time difference in milli seconds
#define rad2degree 57.3              // Radian to degree conversion
#define Filter_gain 0.95             // e.g.  angle = angle_gyro*Filter_gain + angle_accel*(1-Filter_gain)
// *********************************************************************
//    Global Variables
// *********************************************************************


class MPU6050 {
public:
  MPU6050(extraSettings* s)
  {
      m_settings = s;
      angleY    = 0;
      rawAngleY = 0;
  }

  void init()
  {
    on();
    firstRead();
  }

  void on()
  {

    Wire.begin(m_settings->sdaPin,m_settings->sclPin);
//    Wire.beginTransmission(MPU);
//    Wire.write(0x6B);  // PWR_MGMT_1 register
//    Wire.write(0);     // set to zero (wakes up the MPU-6050)
//    Wire.endTransmission(true);
    MPU6050_ResetWake();
//    MPU6050_SetGains(0,1);// Setting the lows scale
//    MPU6050_SetDLPF(0); // Setting the DLPF to inf Bandwidth for calibration
    //MPU6050_OffsetCal();
//    MPU6050_SetDLPF(6); // Setting the DLPF to lowest Bandwidth
    m_activated = true;
  }
  void off()
  {
    Wire.begin(m_settings->sdaPin,m_settings->sclPin);
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(1);     // apagar?
    Wire.endTransmission(true);
    m_activated = false;
  }

  void firstRead()
  {
      if(!m_activated)
      {
              Serial.println("Request for read the gyro, but is not activated...");
              return;
      }

      Wire.beginTransmission(MPU);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
      AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
      AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
      AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
      Temp = Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
      GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
      GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
      GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
      Temp = Temp/340.00+36.53;
      lastAcX  = AcX;
      lastAcY  = AcY;
      lastAcZ  = AcZ;
      lastGyX  = GyX;
      lastGyX  = GyY;
      lastGyX  = GyZ;
      lastTemp = Temp;
      if( (GyX > -300) && (GyX < -200) )
          GyX = -300;

      calcAngles(false);
  }

  void fullRead()
  {
      if(!m_activated)
      {
              Serial.println("Request for read the gyro, but is not activated...");
              return;
      }

    lastAcX  = AcX;
    lastAcY  = AcY;
    lastAcZ  = AcZ;
    lastGyX  = GyX;
    lastGyX  = GyY;
    lastGyX  = GyZ;
    lastTemp = Temp;
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
    AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Temp = Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

    Wire.beginTransmission(MPU);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,14,true);  // request a total of 14 registers

    int16_t secondRead;
    secondRead =  Wire.read()<<8|Wire.read();
    AcX  += (secondRead -  AcX) /2;
    secondRead =  Wire.read()<<8|Wire.read();
    AcY  += (secondRead -  AcY) /2;
    secondRead =  Wire.read()<<8|Wire.read();
    AcZ  += (secondRead -  AcZ) /2;
    secondRead =  Wire.read()<<8|Wire.read();
    Temp += (secondRead -  Temp) /2;
    secondRead =  Wire.read()<<8|Wire.read();
    GyX  += (secondRead -  GyX) /2;
    secondRead =  Wire.read()<<8|Wire.read();
    GyY  += (secondRead -  GyY) /2;
    secondRead =  Wire.read()<<8|Wire.read();
    GyZ  += (secondRead -  GyZ) /2;


    acxRising = AcX > lastAcX;
    acyRising = AcY > lastAcY;
    aczRising = AcZ > lastAcZ;

    Temp = Temp/340.00+36.53;
    calcAngles();
  }

void calcAngles(bool filter = true)
{
    float angle = abs(AcY*90/15500);
    if(angle > 90)
        return;

    float calcAngleY = rawAngleY;

    if(GyX > -250)
    {
        if(AcY > 0)
        {
            if(acyRising)
                calcAngleY = 180+angle;
            else
                calcAngleY = 360-angle;
        }
        else if(AcY < 0)
        {
            if(acyRising)
                calcAngleY = 180-angle;
            else
                calcAngleY = angle;
        }
    }
    else// if(GyX < -285)
    {
        if(AcY > 0)
        {
            if(acyRising)
                calcAngleY = 360-angle;
            else
                calcAngleY = 180+angle;
        }
        else if(AcY < 0)
        {
            if(acyRising)
                calcAngleY = angle;
            else
                calcAngleY = 180-angle;
        }
    }

    rawAngleY = calcAngleY;
    if(!filter)
    {
        angleY = rawAngleY;
        return;
    }

    float angleDiff = getAngleDifference(calcAngleY,angleY);

//filtrado de datos basico. (misma direccion, rango dentro del esperado)
        float expectedIncrement;

        if(abs(GyX) < 32000)
        {
            expectedIncrement = GyX * 40.0f /32768.0f;
            if(abs(expectedIncrement) < 10)
            {
                expectedIncrement = 10;
                if(GyX < 0)
                    expectedIncrement *= -1;
            }
        }
        else
        {
            expectedIncrement = 100;
            if(GyX < 0)
                expectedIncrement *= -1;
        }

        if(expectedIncrement<0)
        {
            if( (angleDiff > expectedIncrement) && (angleDiff < 0) )
                    angleY = rawAngleY;
        }
        else
        {
            if( (angleDiff < expectedIncrement) && (angleDiff > 0) )
                    angleY = rawAngleY;
        }
}

  int16_t   AcX,AcY,AcZ,GyX,GyY,GyZ,lastAcX,lastAcY,lastAcZ,lastGyX,lastGyY,lastGyZ, angleX, angleY, angleZ, rawAngleX, rawAngleY, rawAngleZ, lastAngleX, lastAngleY, lastAngleZ;
  bool      acxRising,acyRising,aczRising;
  float     Temp,lastTemp;

protected:
    bool            m_activated = false;
    const int       MPU         =0x68;  // I2C address of the MPU-6050
    extraSettings*  m_settings;

    float getAngleDifference(const float& a, const float& b)
    {
        float distanceA = 0;
        float distanceB = 0;

        distanceA = a-b;
        distanceB = a-b;
        if(distanceB >0)
        {
            distanceB -= 360;
        }
        else
        {
            distanceB += 360;
        }


        if(abs(distanceA) < abs(distanceB))
            return distanceA;
        else
            return distanceB;
    }
};

#endif // GYRO
