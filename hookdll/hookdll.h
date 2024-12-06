#pragma once
#include<Windows.h>
#include<iostream>
#include <fstream>
#include<string>


extern "C" __declspec(dllexport)  BOOL InstallHook();
extern "C" __declspec(dllexport)  BOOL UnInstallHook();


