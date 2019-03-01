#include "substrate.h"
#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "zz", __VA_ARGS__)

//配置，这里是hook可运行程序（即NDK小程序）的写法，下面那个就是hook dvm的写法
MSConfig(MSFilterExecutable,"/system/bin/app_process")
//MSConfig(MSFilterLibrary, "libdvm.so");

//旧的函数地址，目的为了保留指向原来函数的入口，在新的函数执行
//完后，一般会再调用该函数，以确保程序的正常运行
char* (* hello)(void);

//新的函数，替代hook的函数，返回修改后的值
const char* newHello(void)
{
    //直接返回新的字符串
    return "fuck the world";
    //执行原函数，确保程序运行正常，但这里代码简单，可以直接返回字符串即可
    //return hello();
}

//通过so库的绝对路径和函数名，找到其函数的映射地址
void* lookup_symbol(char* libraryname,char* symbolname)
{
    //获取so库的句柄
    void *handle = dlopen(libraryname, RTLD_GLOBAL | RTLD_NOW);
    if (handle != NULL){
        //根据so库句柄和符号名（即函数名）获取函数地址
        void * symbol = dlsym(handle, symbolname);
        if (symbol != NULL){
            return symbol;
        }else{
            LOGD("dl error: %s", dlerror());
            return NULL;
        }
    }else{
        return NULL;
    }
}

MSInitialize
{
    //获取hook函数的地址,最好不要用下面MS提供的方法
    char strLib[]={"/data/data/com.yusakul.hello/lib/libhello.so"};
    char strFunc[]={"createHello"};
    void * symbol = lookup_symbol(strLib,strFunc);
    //MSImageRef  image=MSGetImageByName("/data/data/com.yusakul.hello/lib/libhello.so");
    //void *symbol=MSFindSymbol(image, "createHello");

    //这里将旧函数的入口（参数一）指向hello(参数三），然后执行新函数（参数二）
    MSHookFunction(symbol, (void*)&newHello, (void**)&hello);
}

