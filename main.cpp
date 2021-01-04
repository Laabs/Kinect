#include "stdafx.h"
#include "included.h"

///#include <fstream>


// 应用程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
//waitKey(0);
	//kbhit()
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#ifdef _DEBUG //条件编译，即在debug模式下编译时（如果预（宏）定义了_DEBUG）则执行该语句块中的语句
	AllocConsole();
	_cwprintf(L"Battle Control  Online! \n");
#endif

	//********************************************************************************************
	//在release下面分配一控制台
	AllocConsole();
	_cwprintf(L"Battle Control  Online! \n");
	//********************************************************************************************

	if (SUCCEEDED(CoInitialize(NULL)))  //Initializes the COM library on the current thread 
										//and identifies the concurrency model as single-thread apartment (STA)
										//This parameter is reserved and must be NULL
	{
		///		ofstream file;
		{
			ThisApp app;
			///			file.open("C://Users//rudy//Desktop//file//file.txt", ios::trunc);
			if (SUCCEEDED(app.Initialize(hInstance, nCmdShow)))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize(); //Closes the COM library on the current thread, unloads all DLLs loaded by the thread
						  ///		file.close();    //frees any other resources that the thread maintains, and forces all RPC connections on the thread to close. 
						  //This function has no parameters and does not return a value 
	}
#ifdef _DEBUG
	_cwprintf(L"Battle Control Terminated! \n");
	FreeConsole();
#endif

	///***********************************************************************************
	///释放控制台
	_cwprintf(L"Battle Control Terminated! \n");
	FreeConsole();
	///***********************************************************************************

	return 0;
}