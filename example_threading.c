#include <Python.h>

#define NUM_THREADS 5
#define NUM_ITERATIONS 50

// 线程参数结构
typedef struct
{
    int thread_id;
    const char *name;
    PyObject *pClass;
} ThreadArgs;

// 生成随机字符串的函数
void generate_random_string(char *buffer, int length, const char *charset)
{
    int charset_len = strlen(charset);
    for (int i = 0; i < length; ++i)
    {
        int index = rand() % charset_len; // 从字符集中随机选一个字符
        buffer[i] = charset[index];
    }
}

// 线程函数
void *thread_func(void *args)
{
    ThreadArgs *targs = (ThreadArgs *)args;
    int tid = targs->thread_id;
    printf("Thread %d started\n", tid);

    PyGILState_STATE gstate;

    gstate = PyGILState_Ensure();
    PyObject *pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(targs->name));
    PyGILState_Release(gstate);

    gstate = PyGILState_Ensure();
    PyObject *pInstance = PyObject_CallObject(targs->pClass, pArgs);
    Py_DECREF(pArgs);
    if (pInstance == NULL)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to create instance of HelloWorld\n");
        PyGILState_Release(gstate);
        return NULL;
    }
    PyGILState_Release(gstate);

    // 获取 say_hi 方法
    gstate = PyGILState_Ensure();
    PyObject *pFuncSayHi = PyObject_GetAttrString(pInstance, "say_hi");
    if (!pFuncSayHi || !PyCallable_Check(pFuncSayHi))
    {
        PyErr_Print();
        fprintf(stderr, "Failed to get say_hi function\n");
        Py_DECREF(pInstance);
        PyGILState_Release(gstate);
        Py_Finalize();
        return NULL;
    }
    // 调用 say_hi 方法
    PyObject_CallObject(pFuncSayHi, NULL);
    PyGILState_Release(gstate);

    // 获取 add 函数
    gstate = PyGILState_Ensure();
    PyObject *pFuncAdd = PyObject_GetAttrString(pInstance, "add");
    if (!pFuncAdd || !PyCallable_Check(pFuncAdd))
    {
        PyErr_Print();
        fprintf(stderr, "Failed to get add function\n");
        Py_DECREF(pFuncSayHi);
        Py_DECREF(pInstance);
        PyGILState_Release(gstate);
        Py_Finalize();
        return NULL;
    }
    // 构造参数并调用 add 函数
    PyObject *pArgsAdd = PyTuple_Pack(2, PyLong_FromLong(3), PyLong_FromLong(4));
    if (!pArgsAdd)
    {
        PyErr_Print();
        Py_DECREF(pFuncAdd);
        Py_DECREF(pFuncSayHi);
        Py_DECREF(pInstance);
        PyGILState_Release(gstate);
        Py_Finalize();
        return NULL;
    }
    PyObject *pResultAdd = PyObject_CallObject(pFuncAdd, pArgsAdd);
    if (!pResultAdd)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to call add function\n");
        Py_DECREF(pArgsAdd);
        Py_DECREF(pFuncAdd);
        Py_DECREF(pFuncSayHi);
        Py_DECREF(pInstance);
        PyGILState_Release(gstate);
        Py_Finalize();
        return NULL;
    }
    // 处理返回值
    long addResult = PyLong_AsLong(pResultAdd);
    PyGILState_Release(gstate);
    printf("Add Result: %ld\n", addResult);

    // 获取 random_number 方法
    gstate = PyGILState_Ensure();
    PyObject *pFuncRandomNumber = PyObject_GetAttrString(pInstance, "random_number");
    if (!pFuncRandomNumber || !PyCallable_Check(pFuncRandomNumber))
    {
        PyErr_Print();
        fprintf(stderr, "Failed to get random_number function\n");
        Py_DECREF(pResultAdd);
        Py_DECREF(pArgsAdd);
        Py_DECREF(pFuncAdd);
        Py_DECREF(pFuncSayHi);
        Py_DECREF(pInstance);
        PyGILState_Release(gstate);
        Py_Finalize();
        return NULL;
    }
    PyGILState_Release(gstate);

    srand(time(NULL) + tid);
    // 循环多次调用 random_number 方法
    for (int i = 0; i < NUM_ITERATIONS; i++)
    {
        gstate = PyGILState_Ensure();
        PyObject *pRandomNumber = PyObject_CallObject(pFuncRandomNumber, NULL);
        long randomNumber = PyLong_AsLong(pRandomNumber);
        printf("Thread %d, Iteration %d, Random Number: %ld\n", tid, i, randomNumber);
        Py_DECREF(pRandomNumber);
        PyGILState_Release(gstate);
        usleep(10000);
    }

    // 释放资源
    gstate = PyGILState_Ensure();
    Py_DECREF(pFuncRandomNumber);
    Py_DECREF(pResultAdd);
    Py_DECREF(pArgsAdd);
    Py_DECREF(pFuncAdd);
    Py_DECREF(pFuncSayHi);
    Py_DECREF(pInstance);
    PyGILState_Release(gstate);

    printf("Thread %d end\n", tid);
    return NULL;
}

int main()
{
    // 主线程初始化一次 Python 解释器
    Py_Initialize();

    // 添加当前目录到 sys.path（确保能导入 example.py）
    PyObject *pSys = PyImport_ImportModule("sys");
    if (!pSys)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to import sys module\n");
        return 1;
    }
    PyObject *pPath = PyObject_GetAttrString(pSys, "path");
    if (!pPath)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to get sys.path\n");
        Py_DECREF(pSys);
        return 1;
    }
    int status = PyList_Append(pPath, PyUnicode_FromString("."));
    if (status == -1)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to append path\n");
        Py_DECREF(pPath);
        Py_DECREF(pSys);
        return 1;
    }
    Py_DECREF(pPath);
    Py_DECREF(pSys);

    // 导入 example 模块
    PyObject *pModule = PyImport_ImportModule("example");
    if (!pModule)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to import example module\n");
        Py_Finalize();
        return 1;
    }
    // 导入 HelloWorld 类
    PyObject *pClass = PyObject_GetAttrString(pModule, "HelloWorld");
    if (!pClass)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to import HelloWorld class\n");
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
    }

    PyEval_SaveThread(); // 释放 GIL

    srand(time(NULL)); // 初始化随机种子

    const char *charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // 创建线程
    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        printf("Create thread %d\n", i);
        char buffer[5]; // 长度为 5 个结束符
        generate_random_string(buffer, 5, charset);

        thread_args[i].thread_id = i;
        thread_args[i].name = buffer;
        thread_args[i].pClass = pClass;

        int rc = pthread_create(&threads[i], NULL, thread_func, &thread_args[i]);
        if (rc)
        {
            fprintf(stderr, "Error creating thread %d\n", i);
            return EXIT_FAILURE;
        }
        // usleep(100000);
    }
    printf("create threads success\n");

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    PyGILState_STATE gstate = PyGILState_Ensure();

    Py_DECREF(pClass);
    Py_DECREF(pModule);

    // 结束 Python 解释器
    Py_Finalize();
    return 0;
}
