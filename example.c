#include <Python.h>

int main()
{
    // 初始化 Python 解释器
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

    // 创建对象
    char name[256] = "XiaoMing";

    PyObject *pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(name));

    PyObject *pInstance = PyObject_CallObject(pClass, pArgs);
    Py_DECREF(pArgs);

    if (pInstance == NULL)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to create instance of HelloWorld\n");
        Py_XDECREF(pClass);
        Py_XDECREF(pModule);
        return 1;
    }

    // 获取 say_hi 方法
    PyObject *pFuncSayHi = PyObject_GetAttrString(pInstance, "say_hi");
    if (!pFuncSayHi || !PyCallable_Check(pFuncSayHi))
    {
        PyErr_Print();
        fprintf(stderr, "Failed to get say_hi function\n");
        Py_DECREF(pInstance);
        Py_DECREF(pClass);
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
    }

    // 调用 say_hi 方法
    PyObject_CallObject(pFuncSayHi, NULL);

    // 获取 add 函数
    PyObject *pFuncAdd = PyObject_GetAttrString(pInstance, "add");
    if (!pFuncAdd || !PyCallable_Check(pFuncAdd))
    {
        PyErr_Print();
        fprintf(stderr, "Failed to get add function\n");
        Py_DECREF(pFuncSayHi);
        Py_DECREF(pInstance);
        Py_DECREF(pClass);
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
    }

    // 构造参数并调用 add 函数
    PyObject *pArgsAdd = PyTuple_Pack(2, PyLong_FromLong(3), PyLong_FromLong(4));
    if (!pArgsAdd)
    {
        PyErr_Print();
        Py_DECREF(pFuncAdd);
        Py_DECREF(pFuncSayHi);
        Py_DECREF(pInstance);
        Py_DECREF(pClass);
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
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
        Py_DECREF(pClass);
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
    }

    // 处理返回值
    long addResult = PyLong_AsLong(pResultAdd);

    printf("Add Result: %ld\n", addResult);

    // 获取 random_number 方法
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
        Py_DECREF(pClass);
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
    }

    // 调用 random_number 方法
    PyObject *pRandomNumber = PyObject_CallObject(pFuncRandomNumber, NULL);
    long randomNumber = PyLong_AsLong(pRandomNumber);
    printf("Random Number: %ld\n", randomNumber);

    // 释放资源
    Py_DECREF(pRandomNumber);
    Py_DECREF(pFuncRandomNumber);
    Py_DECREF(pResultAdd);
    Py_DECREF(pArgsAdd);
    Py_DECREF(pFuncAdd);
    Py_DECREF(pFuncSayHi);
    Py_DECREF(pInstance);
    Py_DECREF(pClass);
    Py_DECREF(pModule);

    // 结束 Python 解释器
    Py_Finalize();

    return 0;
}
