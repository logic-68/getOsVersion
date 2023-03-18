#include <utils.h>

#define VERSION "1.0.1"
#define NAME_APP "GetOsVersion"
#define DEBUG_SOCKET
#define DEBUG_ADDR IP(192, 168, 1, 155);
#define DEBUG_PORT 5655

int sock, nthread_run, display_temp;


void *nthread_func(void *args)
{
	PROCESS *process = args;
	time_t t1, t2;
	t1 = 0;
	while (nthread_run)
	{
		if (display_temp)
		{
			t2 = f_time(NULL);
			if ((t2 - t1) >= 20)
			{
				t1 = t2;
				printf_notification("Temp CPU:%i°C %iMhz SOC:%i°C\n", process->cpu, process->frequency, process->soc);
				display_temp = 0;
			}
		}
		else 
		{
			display_temp = 1;
			t1 = 0;
		}	
		f_sceKernelSleep(7);
	}

	return NULL;
}
/*************************************************DISPLAY******************************************************/
void displayFreeBSD(KERNEL kernel, MANUFACTURE manufacture)
{
	printfsocket("%s %s %i %s %i\n", kernel.ostype, kernel.revision, manufacture.day, manufacture.month, manufacture.year);
}

void displayHw(HW hw)
{
	printfsocket("%s Model: %s\n", hw.machine, hw.model);
}
void infosSdk(KERNEL kernel)
{
	printfsocket("\nkern.ps4_sdk_version: 0x%zx\nkernel.sdk_version: 0x%zx\nmachdep.upd_version: 0x%zx", kernel.ps4_sdk_version, kernel.sdk_version, kernel.upd_version);
}
void infoCpuMode(KERNEL kernel)
{
	printfsocket("\nCpu Mode:%i Cpu Mode Game:%i\n", kernel.cpumode, kernel.cpumodegame);
}
void infosOs(OS os)
{
	printfsocket("\nPS4 SDK:%g FW:%g UPD:%g\n", os.ps4_sdk_version, os.sdk_version, os.upd_version);
}
void infosTemp(PROCESS process)
{
	printfsocket("\nCPU:%iC / %iMhz SOC:%iC\n", process.cpu, process.frequency, process.soc);
}

void displaySystem(SYSTEM system)
{
	HW *hw;
	hw = system.h;

	KERNEL *kernel;
	kernel = system.k;

	MANUFACTURE *manufacture;
	manufacture = system.m;

	OS *os;
	os = system.o;

	PROCESS *process;
	process = system.p;

	displayFreeBSD(*kernel, *manufacture);
	displayHw(*hw);	
	infosOs(*os);
	infoCpuMode(*kernel);
	infosSdk(*kernel);
	infosTemp(*process);

	printf_notification("%s %s %i %s %i", kernel->ostype, kernel->revision, manufacture->day, manufacture->month, manufacture->year);
	f_sceKernelSleep(7);

	printf_notification("%s Model: %s\n", hw->machine, hw->model);
	f_sceKernelSleep(7);

	printf_notification("PS4 SDK:%g FW:%g UPD:%g\nCpu Mode:%i Cpu Mode Game:%i", os->ps4_sdk_version, os->sdk_version, os->upd_version, kernel->cpumode, kernel->cpumodegame);
	f_sceKernelSleep(7);
	
	ScePthread nthread;
	if(f_scePthreadCreate(&nthread, NULL, nthread_func, process, "nthread")){
		f_free(process);
	}
}
void selectSystem(SYSTEM *system)
{
	HW *hw = (HW *)f_malloc(sizeof(HW));
	system->h = hw;
	getHw(hw);

	KERNEL *kernel = (KERNEL *)f_malloc(sizeof(KERNEL));
	system->k = kernel;
	getKernel(kernel);
	cpuMode(kernel);

	char version[256];
	f_strcpy(version, kernel->version);	
	formatRevision(kernel); 

	MANUFACTURE *manufacture = (MANUFACTURE *)f_malloc(sizeof(MANUFACTURE));
	system->m = manufacture;
	formatDate(manufacture, version);

	OS *os = (OS *)f_malloc(sizeof(OS));
	system-> o= os;
	getOs(kernel, os);

	PROCESS *process = (PROCESS *)f_malloc(sizeof(PROCESS));
	system->p = process;
	getProcessTempFrequency(process);
}
int payload_main(struct payload_args *args)
{
	dlsym_t *dlsym = args->dlsym;

	int libKernel = 0x2001;

	dlsym(libKernel, "sceKernelSleep", &f_sceKernelSleep);
	dlsym(libKernel, "sceKernelLoadStartModule", &f_sceKernelLoadStartModule);
	dlsym(libKernel, "sceKernelDebugOutText", &f_sceKernelDebugOutText);
	dlsym(libKernel, "sceKernelSendNotificationRequest", &f_sceKernelSendNotificationRequest);
	dlsym(libKernel, "sceKernelUsleep", &f_sceKernelUsleep);
	dlsym(libKernel, "scePthreadMutexLock", &f_scePthreadMutexLock);
	dlsym(libKernel, "scePthreadMutexUnlock", &f_scePthreadMutexUnlock);
	dlsym(libKernel, "scePthreadExit", &f_scePthreadExit);
	dlsym(libKernel, "scePthreadMutexInit", &f_scePthreadMutexInit);
	dlsym(libKernel, "scePthreadCreate", &f_scePthreadCreate);
	dlsym(libKernel, "scePthreadMutexDestroy", &f_scePthreadMutexDestroy);
	dlsym(libKernel, "scePthreadJoin", &f_scePthreadJoin);
	dlsym(libKernel, "sceKernelGetFsSandboxRandomWord", &f_sceKernelGetFsSandboxRandomWord);

	dlsym(libKernel, "sceKernelGetCpuTemperature", &f_sceKernelGetCpuTemperature);
	dlsym(libKernel, "sceKernelGetSocSensorTemperature", &f_sceKernelGetSocSensorTemperature);
	dlsym(libKernel, "sceKernelGetHwSerialNumber", &f_sceKernelGetHwSerialNumber);
	dlsym(libKernel, "sceKernelGetCpuFrequency", &f_sceKernelGetCpuFrequency);

	dlsym(libKernel, "sysctlbyname", &f_sysctlbyname);
	dlsym(libKernel, "socket", &f_socket);
	dlsym(libKernel, "bind", &f_bind);
	dlsym(libKernel, "listen", &f_listen);
	dlsym(libKernel, "accept", &f_accept);
	dlsym(libKernel, "open", &f_open);
	dlsym(libKernel, "read", &f_read);
	dlsym(libKernel, "write", &f_write);
	dlsym(libKernel, "close", &f_close);
	dlsym(libKernel, "stat", &f_stat);
	dlsym(libKernel, "fstat", &f_fstat);
	dlsym(libKernel, "rename", &f_rename);
	dlsym(libKernel, "rmdir", &f_rmdir);
	dlsym(libKernel, "mkdir", &f_mkdir);
	dlsym(libKernel, "getdents", &f_getdents);
	dlsym(libKernel, "unlink", &f_unlink);
	dlsym(libKernel, "readlink", &f_readlink);
	dlsym(libKernel, "lseek", &f_lseek);
	dlsym(libKernel, "puts", &f_puts);
	dlsym(libKernel, "mmap", &f_mmap);
	dlsym(libKernel, "munmap", &f_munmap);
	dlsym(libKernel, "sceKernelReboot", &f_sceKernelReboot);

	int libNet = f_sceKernelLoadStartModule("libSceNet.sprx", 0, 0, 0, 0, 0);
	dlsym(libNet, "sceNetSocket", &f_sceNetSocket);
	dlsym(libNet, "sceNetConnect", &f_sceNetConnect);
	dlsym(libNet, "sceNetHtons", &f_sceNetHtons);
	dlsym(libNet, "sceNetAccept", &f_sceNetAccept);
	dlsym(libNet, "sceNetSend", &f_sceNetSend);
	dlsym(libNet, "sceNetInetNtop", &f_sceNetInetNtop);
	dlsym(libNet, "sceNetSocketAbort", &f_sceNetSocketAbort);
	dlsym(libNet, "sceNetBind", &f_sceNetBind);
	dlsym(libNet, "sceNetListen", &f_sceNetListen);
	dlsym(libNet, "sceNetSocketClose", &f_sceNetSocketClose);
	dlsym(libNet, "sceNetHtonl", &f_sceNetHtonl);
	dlsym(libNet, "sceNetInetPton", &f_sceNetInetPton);
	dlsym(libNet, "sceNetGetsockname", &f_sceNetGetsockname);
	dlsym(libNet, "sceNetRecv", &f_sceNetRecv);
	dlsym(libNet, "sceNetErrnoLoc", &f_sceNetErrnoLoc);
	dlsym(libNet, "sceNetSetsockopt", &f_sceNetSetsockopt);

	int libC = f_sceKernelLoadStartModule("libSceLibcInternal.sprx", 0, 0, 0, 0, 0);
	dlsym(libC, "vsprintf", &f_vsprintf);
	dlsym(libC, "memset", &f_memset);
	dlsym(libC, "sprintf", &f_sprintf);
	dlsym(libC, "snprintf", &f_snprintf);
	dlsym(libC, "snprintf_s", &f_snprintf_s);
	dlsym(libC, "strcat", &f_strcat);
	dlsym(libC, "free", &f_free);
	dlsym(libC, "memcpy", &f_memcpy);
	dlsym(libC, "strcpy", &f_strcpy);
	dlsym(libC, "strncpy", &f_strncpy);
	dlsym(libC, "sscanf", &f_sscanf);
	dlsym(libC, "scanf", &f_scanf);
	dlsym(libC, "scanf", &f_scanf);
	dlsym(libC, "malloc", &f_malloc);
	dlsym(libC, "calloc", &f_calloc);
	dlsym(libC, "strlen", &f_strlen);
	dlsym(libC, "strcmp", &f_strcmp);
	dlsym(libC, "strchr", &f_strchr);
	dlsym(libC, "strrchr", &f_strrchr);
	dlsym(libC, "gmtime_s", &f_gmtime_s);
	dlsym(libC, "time", &f_time);
	dlsym(libC, "localtime", &f_localtime);
	dlsym(libC, "closedir", &f_closedir);
	dlsym(libC, "opendir", &f_opendir);
	dlsym(libC, "readdir", &f_readdir);
	dlsym(libC, "fclose", &f_fclose);
	dlsym(libC, "fopen", &f_fopen);
	dlsym(libC, "strstr", &f_strstr);
	dlsym(libC, "fseek", &f_fseek);
	dlsym(libC, "ftell", &f_ftell);
	dlsym(libC, "fread", &f_fread);
	dlsym(libC, "usleep", &f_usleep);
	dlsym(libC, "fputs", &f_fputs);
	dlsym(libC, "fgetc", &f_fgetc);
	dlsym(libC, "fgets", &f_fgets);
	dlsym(libC, "feof", &f_feof);
	dlsym(libC, "fprintf", &f_fprintf);
	dlsym(libC, "realloc", &f_realloc);
	dlsym(libC, "seekdir", &f_seekdir);
	dlsym(libC, "strtok", &f_strtok);
	dlsym(libC, "strtol", &f_strtol);
	dlsym(libC, "atoi", &f_atoi);
	dlsym(libC, "atof", &f_atof);
	dlsym(libC, "atol", &f_atol);
	dlsym(libC, "isspace", &f_isspace);
	dlsym(libC, "ferror", &f_ferror);
	dlsym(libC, "strdup", &f_strdup);

	int libNetCtl = f_sceKernelLoadStartModule("libSceNetCtl.sprx", 0, 0, 0, 0, 0);
	dlsym(libNetCtl, "sceNetCtlInit", &f_sceNetCtlInit);
	dlsym(libNetCtl, "sceNetCtlTerm", &f_sceNetCtlTerm);
	dlsym(libNetCtl, "sceNetCtlGetInfo", &f_sceNetCtlGetInfo);

	int libSysModule = f_sceKernelLoadStartModule("libSceSysmodule.sprx", 0, 0, 0, 0, 0);
	dlsym(libSysModule, "sceSysmoduleLoadModuleInternal", &f_sceSysmoduleLoadModuleInternal);
	dlsym(libSysModule, "sceSysmoduleUnloadModuleInternal", &f_sceSysmoduleUnloadModuleInternal);

	int sysModule = f_sceSysmoduleLoadModuleInternal(SCE_SYSMODULE_INTERNAL_USER_SERVICE);

	int libUserService = f_sceKernelLoadStartModule("libSceUserService.sprx", 0, 0, 0, 0, 0);
	dlsym(libUserService, "sceUserServiceInitialize", &f_sceUserServiceInitialize);
	dlsym(libUserService, "sceUserServiceGetInitialUser", &f_sceUserServiceGetInitialUser);
	dlsym(libUserService, "sceUserServiceGetLoginUserIdList", &f_sceUserServiceGetLoginUserIdList);
	dlsym(libUserService, "sceUserServiceGetUserName", &f_sceUserServiceGetUserName);
	dlsym(libUserService, "sceUserServiceTerminate", &f_sceUserServiceTerminate);

	struct sockaddr_in server;
	server.sin_len = sizeof(server);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = DEBUG_ADDR;
	server.sin_port = f_sceNetHtons(DEBUG_PORT);
	f_memset(server.sin_zero, 0, sizeof(server.sin_zero));
	sock = f_sceNetSocket("debug", AF_INET, SOCK_STREAM, 0);
	f_sceNetConnect(sock, (struct sockaddr *)&server, sizeof(server));

	nthread_run = 1;
	display_temp = 1;

	char *userName;
	int32_t userId;

	if (sysModule == 0)
	{
		get_User_Name(&userName, &userId);
		printf_notification("Welcome %s to:\n%s V%s\n\n by ★Logic-68★", userName, NAME_APP, VERSION);
		f_sceKernelSleep(7);
	}
	sysModule = f_sceSysmoduleUnloadModuleInternal(SCE_SYSMODULE_INTERNAL_USER_SERVICE);
	//////////////////////////////////////////////////////////////////////////////////////
	SYSTEM system;
	selectSystem(&system);
	displaySystem(system);
	return 0;
}