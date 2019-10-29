#include <Wire.h>
#include <Chrono.h>

// Reads X,Y,Z of Accelerometer and Gyroscope, and Temperature and interprets it

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; // Sensor readings
double mAcX, mAcY, mAcZ, mTmp, mGyX, mGyY, mGyZ; // Metric values of sensor readings
double pitch, roll, yaw; // Metric orientation interpretation
int16_t OffsetAcX, OffsetAcY, OffsetAcZ, OffsetGyX, OffsetGyY, OffsetGyZ; // Offsetset values set in calibration
int16_t cycle; // Keeps track of cycle iteration

// Declare addresses
#define MPU 0x68 // I2C address of the MPU-6050
#define PWR_MGMT_1 0x6B // Power Management register
#define GYRO_CONFIG 0x1B // Gyro Sensor Configuration register for range/precision
#define ACC_CONFIG 0x1C // Accelerometer Sensor Configuration register for range/precision

// Accelerometer and Gyroscope Settings - ALWAYS ADJUST DEPENDING SENSITIVITIES ACC_SENS AND GYRO_SENS!
#define FS_SEL 3 // 0 = +/-250°/s, 1 = +/-500°/s, 2 = +/-1000°/s, 3 = +/-2000°/s
#define AFS_SEL 3 // 0 = +/-2g, 1 = +/-4g, 2 = +/-8g, 3 = +/-16g
// Depending sensitivities
#define ACC_SENS 2048.0 // FS_SEL: 0 -> 16384.0, 1 -> 8192.0, 2 -> 4096.0, 3 -> 2048.0
#define GYRO_SENS 16.4 // AFS_SEL: 0 -> 131.0, 1 -> 65.5, 2 -> 32.8, 3 -> 16.4

const int16_t NO_CAL_CYCLES = 2000; // Number of measurements taken during calibration

#define GRAV_ACC 9.81
#define M_PI 3.14159265359

// Used to keep track of time between measurements (for Compelentary filter)
unsigned long timeInMs;
double timeInSec;
Chrono chrono; // Counts time itself

void setup()
{
  // Somehow starting it up like this avoids issues of 0 measures, TODO why
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  Serial.end();
  Wire.end();


  // Actual program begin
  Wire.begin();
  Wire.beginTransmission(MPU);

  // Set up Accelerometer Configuration
  Wire.write(ACC_CONFIG); // Point to register
  byte acc = AFS_SEL * 8;
  Wire.write(acc); // sets up Sensitivity and Full Scale Range for Accelerometer

  // Set up Gyroscope Configuration
  Wire.write(GYRO_CONFIG);
  byte gyro = FS_SEL * 8;
  Wire.write(gyro);

  Wire.write(PWR_MGMT_1); // Power management register
  Wire.write(0); // Wake up the MPU
  Wire.endTransmission(true);
  Serial.begin(9600); // Range 9600 to 250000 possible

  Serial.print("Accelerometer Configuration mode: "); Serial.println(AFS_SEL);
  Serial.print("Gyroscope Configuration mode: "); Serial.println(FS_SEL);

  calibrate();
  chrono.restart();
}

void loop()
{
  measure();
  calcSIUnits();
  complementaryFilter();
  cycle++;
  //outputOrientation();
}

// Helper-method to join two byte of measurements into a 16bit int
static inline int16_t read_pair(void)
{
  int16_t val;

  val = Wire.read();
  val <<= 8;
  val |= Wire.read();

  return val;
}

// Measures values
void measure()
{
  timeInMs = chrono.elapsed();
  timeInSec = timeInMs / 1000.0;

  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // Request a total of 14 registers (readpair reads two each for higher and lower 8 bits of measurement)
  AcX = read_pair() + OffsetAcX;
  AcY = read_pair() + OffsetAcY;
  AcZ = read_pair() + OffsetAcZ;
  Tmp = read_pair();
  GyX = read_pair() + OffsetGyX;
  GyY = read_pair() + OffsetGyY;
  GyZ = read_pair() + OffsetGyZ;

  chrono.restart();
}

void calcSIUnits()
{
  mAcX = (AcX / ACC_SENS) * GRAV_ACC; // in m/s
  mAcY = (AcY / ACC_SENS) * GRAV_ACC;
  mAcZ = (AcZ / ACC_SENS) * GRAV_ACC;
  mTmp = Tmp / 340.00 + 36.53; // in °C
  mGyX = GyX / GYRO_SENS; //in °/s
  mGyY = GyY / GYRO_SENS;
  mGyZ = GyZ / GYRO_SENS;
}

// Avoid drift by using both Gyroscope and Accelerometer
void complementaryFilter()
{
  pitch += ((double) GyX / GYRO_SENS) * timeInSec;
  roll += ((double) GyY / GYRO_SENS) * timeInSec;
  yaw += ((double) GyZ / GYRO_SENS) * timeInSec;

  int forceThreshold = abs(mAcX) + abs(mAcY) + abs(mAcZ);
  // If total acc measured is between 0.5G and 2G (i.e. not signitificantly impacted): use non-drifting accelerometer values
  if (forceThreshold > 4.905 && forceThreshold < 19.62)
  {
    // Rotating around the x axis results in a vector on the y axis
    double pitchAcc = atan2((double)AcY, (double)AcZ) * 180 / M_PI;
    pitch = pitch * 0.98 + pitchAcc * 0.02;
    double rollAcc = atan2((double)AcX, (double)AcZ) * 180 / M_PI;
    roll = roll * 0.98 + rollAcc * 0.02;
    double yawAcc = atan2((double)AcY, (double)AcX) * 180 / M_PI;
    yaw = yaw * 0.98 + yawAcc * 0.02;
  }
}

void calibrate()
{
  Serial.println("CALIBRATING...");
  int32_t TotalAcX, TotalAcY, TotalAcZ, TotalGyX, TotalGyY, TotalGyZ;

  for (int n = 0; n < NO_CAL_CYCLES; n++)
  {
    measure();
    TotalAcX += AcX;
    TotalAcY += AcY;
    TotalAcZ += AcZ;
    TotalGyX += GyX;
    TotalGyY += GyY;
    TotalGyZ += GyZ;
  }

  OffsetGyX = -(TotalGyX / NO_CAL_CYCLES);
  OffsetGyY = -(TotalGyY / NO_CAL_CYCLES);
  OffsetGyZ = -(TotalGyZ / NO_CAL_CYCLES);

  // Catches unrealistic Offset
  if (OffsetGyX > 400 || OffsetGyX < -400 || OffsetGyY > 400 | OffsetGyY < -400 || OffsetGyZ > 400 || OffsetGyZ < -400)
  {
    Serial.println("High Offset measurements - Program stopped. Restart and ensure that the sensor does not move during calibration.");
    Serial.print("OffsetGyX = "); Serial.println(OffsetGyX);
    Serial.print("OffsetGyY = "); Serial.println(OffsetGyY);
    Serial.print("OffsetGyZ = "); Serial.println(OffsetGyZ);
    Serial.end();
  }

  Serial.println("Calibration successful. Offsets:");
  Serial.print("OffsetAcX = "); Serial.println(OffsetAcX);
  Serial.print("OffsetAcY = "); Serial.println(OffsetAcY);
  Serial.print("OffsetAcZ = "); Serial.println(OffsetAcZ);
  Serial.print("OffsetGyX = "); Serial.println(OffsetGyX);
  Serial.print("OffsetGyY = "); Serial.println(OffsetGyY);
  Serial.print("OffsetGyZ = "); Serial.println(OffsetGyZ);
  Serial.println("");
}

// Outputs data to Serial Monitor
void outputReadings()
{
  Serial.print("Reading Sensor Values: "); Serial.println(cycle);

  Serial.print("Accelerometer: ");
  Serial.print("AcX = "); Serial.print(AcX + OffsetAcX);
  Serial.print("| AcY = "); Serial.print(AcY + OffsetAcY);
  Serial.print("| AcZ = "); Serial.println(AcZ + OffsetAcZ);

  // Equation for temperature in degrees C from datasheet
  Serial.print("Tmp: "); Serial.println(Tmp);

  Serial.print("Gyroscope: ");
  Serial.print("GyX = "); Serial.print(GyX + OffsetGyX);
  Serial.print("| GyY = "); Serial.print(GyY + OffsetGyY);
  Serial.print("| GyZ = "); Serial.println(GyZ + OffsetGyZ);

  Serial.println("");
}

// Outputs metric unit conversion of data to Serial Monitor
void outputMetric()
{
  Serial.print("Metric Measurement: "); Serial.println(cycle);

  Serial.print("Accelerometer: ");
  Serial.print("X = "); Serial.print(mAcX); Serial.print("m/s^2");
  Serial.print("| Y = "); Serial.print(mAcY); Serial.print("m/s^2");
  Serial.print("| Z = "); Serial.print(mAcZ); Serial.println("m/s^2");

  // Equation for temperature in degrees C from datasheet
  Serial.print("Tmp: "); Serial.print(mTmp); Serial.println("°C");

  Serial.print("Gyroscope: ");
  Serial.print("X = "); Serial.print(mGyX); Serial.print("°/s");
  Serial.print("| Y = "); Serial.print(mGyY); Serial.print("°/s");
  Serial.print("| Z = "); Serial.print(mGyZ); Serial.println("°/s");

  Serial.println("");
}

// Outputs relative pitch, roll and yaw
void outputOrientation()
{
  Serial.print("Cycle = ");
  Serial.println(cycle);

  Serial.print("Pitch = ");
  if (pitch > 10.0)
    Serial.print("UP = ");
  else if (pitch < -10.0)
    Serial.print("DOWN = ");
  else
    Serial.print("CENTRE = ");
  Serial.print(pitch);

  Serial.print("   |    ");

  Serial.print("Roll = ");
  if (roll > 10.0)
    Serial.print("RIGHT = ");
  else if (roll < -10.0)
    Serial.print("LEFT = ");
  else
    Serial.print("CENTRE = ");
  Serial.print(roll);

  Serial.print("   |    ");

  Serial.print("Yaw = ");
  if (yaw < 10.0)
    Serial.print("RIGHT = ");
  else if (yaw > -10.0)
    Serial.print("LEFT = ");
  else
    Serial.print("CENTRE = ");
  Serial.println(yaw);
}