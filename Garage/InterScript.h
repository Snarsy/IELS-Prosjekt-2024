const uint8_t numSensors = 5;
const uint16_t sensorThreshold = 200;
uint16_t lineSensorValues[numSensors];

uint16_t readSensors()
{
  return lineSensors.readLine(lineSensorValues);
}

bool aboveLine(uint8_t sensorIndex)
{
  return lineSensorValues[sensorIndex] > sensorThreshold;
}

