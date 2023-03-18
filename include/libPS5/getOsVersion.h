#ifndef GETOSVERSION_H
#define GETOSVERSION_H
#include <utils.h>

typedef struct Process
{
	int cpu;
	int soc;
	int frequency;
} PROCESS;
typedef struct Kernel
{
	uint32_t ps4_sdk_version;
	uint32_t sdk_version;
	uint32_t upd_version;
	char revision[8];
	uint8_t cpumode;
	uint8_t cpumodegame;
	char ostype[12];
    char version[256];

} KERNEL;
typedef struct Hw
{
	char machine[8];
	char model[16];
} HW;
typedef struct Manufacture
{
	char month[4];
	int day;
	int year;
} MANUFACTURE;
typedef struct Os
{
	double ps4_sdk_version;
    double sdk_version;
    double upd_version;
} OS;
typedef struct System
{
	KERNEL *k;
	OS *o;
	HW *h;
	MANUFACTURE *m;
	PROCESS *p;
} SYSTEM;

int getCpuFrequency();
int getCpuTemperature();
int getSocSensorTemperature();
void getProcessTempFrequency(PROCESS *process);
void getHw(HW *hw);
void cpuMode(KERNEL *kernel);
void getKernel(KERNEL *kernel);
void getOs(KERNEL *kernel, OS *os);
void formatDate(MANUFACTURE *manufacture, char *version);
void formatRevision(KERNEL *kernel);
double formatFw(uint32_t str);

#endif