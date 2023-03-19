#include <libPS5/getOsVersion.h>

int getCpuFrequency()
{
    return f_sceKernelGetCpuFrequency() / (double)(1000 * 1000);
}
int getCpuTemperature()
{
    int cpu;
    if (!f_sceKernelGetCpuTemperature(&cpu))
        return cpu;
    else
        return 0;
}
int getSocSensorTemperature()
{
    int soc;
    if (!f_sceKernelGetSocSensorTemperature(0, &soc))
        return soc;
    else
        return 0;
}
void getProcessTempFrequency(PROCESS *process)
{
    process->cpu = getCpuTemperature();
    process->soc = getSocSensorTemperature();
    process->frequency = getCpuFrequency();
}
void getHw(HW *hw)
{
    size_t olden;
    olden = sizeof(hw->machine);
    f_sysctlbyname("hw.machine", (char *)&hw->machine, &olden, NULL, 0);
    
    olden = sizeof(hw->model);
    f_sysctlbyname("hw.model", (char*)&hw->model, &olden, NULL, 0);
}
void getCpuMode(KERNEL *kernel)
{
    size_t olden;
    olden = sizeof(kernel->cpumode);
    f_sysctlbyname("kern.cpumode", &kernel->cpumode, &olden, NULL, 0);

    olden = sizeof(kernel->cpumodegame);
    f_sysctlbyname("kern.cpumode_game ", &kernel->cpumodegame, &olden, NULL, 0);
}
void getKernel(KERNEL *kernel)
{
    size_t olden;
    olden = sizeof(kernel->ostype);
    f_sysctlbyname("kern.ostype", (char *)&kernel->ostype, &olden, NULL, 0);

    olden = sizeof(kernel->ps4_sdk_version);
    f_sysctlbyname("kern.ps4_sdk_version", (char *)&kernel->ps4_sdk_version, &olden, NULL, 0);

    olden = sizeof(kernel->sdk_version);
    f_sysctlbyname("kern.sdk_version", (char *)&kernel->sdk_version, &olden, NULL, 0);

    olden = sizeof(kernel->upd_version);
    f_sysctlbyname("machdep.upd_version", (char *)&kernel->upd_version, &olden, NULL, 0);

    olden = sizeof(kernel->version);
    f_sysctlbyname("kern.version", (char *)&kernel->version, &olden, NULL, 0);
}
void getOs(KERNEL *kernel, OS *os)
{
    os->ps4_sdk_version = formatFw(kernel->ps4_sdk_version);
    os->sdk_version = formatFw(kernel->sdk_version);
    os->upd_version = formatFw(kernel->upd_version);
}
void formatDate(MANUFACTURE *manufacture, char *version)
{
    const char *sep = " ";
    char *strToken = f_strtok(version, sep);
    int i = 0;
    while (strToken != NULL)
    {
        if (i == 1)
            f_strcpy(manufacture->month, strToken);
        else if (i == 2)
            manufacture->day = (int)f_atoi(strToken);
        else if (i == 3)
            manufacture->year = f_atoi(strToken);
        strToken = f_strtok(NULL, sep);
        i++;
    }
}
void formatRevision(KERNEL *kernel)
{
    char version[256];
    f_strcpy(version, kernel->version);
    const char *sep = "/";
    char *strToken = f_strtok(kernel->version, sep);
    while (strToken != NULL)
        strToken = f_strtok(NULL, sep);
    f_strcpy(kernel->revision, kernel->version);
    f_strcpy(kernel->version, version);
}
double formatFw(uint32_t str)
{
    char buf[6];
    double fw;
    f_snprintf(buf, 0x6, "%2x.%03x", str >> 0x18, str >> 0xc & 0xfff);
	f_sscanf(buf, "%lg", &fw);
    return fw;
}
void freeStruct(KERNEL *kernel, MANUFACTURE *manufacture, HW *hw, OS *os, PROCESS *process)
{
    f_free(kernel);
    f_free(manufacture);
    f_free(hw);
    f_free(os);
    f_free(process);
}