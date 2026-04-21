#ifndef AERONODE_NETWORK_H
#define AERONODE_NETWORK_H

#include <AeronodeConfig.h>

#if AERONODE_ONLINE == 1
#include <Arduino.h>

void initAeroNetwork(void);

void sendMetrics(AirQuality data);
#endif		//Aeronode online end
#endif
