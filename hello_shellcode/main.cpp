#include<Windows.h>
#include<iostream> 
void _declspec(naked) shellcode()
{
	//LoadLibraryA 4C 6F 61 64 4C 69 62 72 61 72 79 41    00        长度：0xD
	//GetProcAddress 47 65 74 50 72 6F 63 41 64 64 72 65   73 73  00     长度：0xF
	//user32.dll 75 73 65 72 33 32 2E 64   6C 6C 00             长度：0xB
	// MeesageBoxA 4D 65 73 73 61 67 65 42     6F 78 41 00      长度：0xC
	//hello 51hook 68 65 6C 6C 6F 20 35 31 68 6F 6F 6B   00     长度：0xD
	__asm
	{
		//1保存相关字符串
		pushad
		sub esp, 0x30
		mov byte ptr ds : [esp - 1] , 0x0
		sub esp, 0x1
		push 0x6B6F6F68
		push 0x3135206F
		push 0x6c6c6568
		push 0x41786f
		push 0x42656761
		push 0x7373654d
		mov byte ptr ds : [esp - 1] , 0x0
		sub esp, 0x1
		mov ax, 0x6c6c
		mov word ptr ds : [esp - 2] , ax
		sub esp, 0x2
		push 0x642e3233
		push 0x72657375
		mov byte ptr ds : [esp - 1] , 0x0
		sub esp, 0x1
		mov ax, 0x7373
		mov word ptr ds : [esp - 2] , ax
		sub esp, 0x2
		push 0x65726464
		push 0x41636f72
		push 0x50746547
		mov byte ptr ds : [esp - 1] , 0x0
		sub esp, 0x1
		push 0x41797261
		push 0x7262694c
		push 0x64616f4c
		mov ecx, ebp
		push ecx
		call fun_payload
		//popad


		//2获取模块基址
	fun_GetModule :
		push ebp
		mov ebp, esp
		sub esp, 0xc
		push esi
		mov esi, dword ptr fs : [0x30] //PEB指针
		mov esi, [esi + 0xc]//LDR结构体地址
		mov esi, [esi + 0x1c]//list
		mov esi, [esi]//list第二项
		mov ecx, [esi+0x8]//kernel32.dll基址
		mov eax, esi
		pop esi
		mov esp, ebp
		pop ebp
		retn
		//3获取导出表 根据导出表查找需要的函数
	fun_GetProcAddr
		push ebp
		mov ebp, esp
		sub esp, 0x10
		push esi
		push edi;
		push edx;
		push ebx;
		push ecx;


		//解析PE文件；
		mov edx, [ebp + 0x8]//dllbase
			mov esi, [edx + 0x3c]//1f_ _anew
			lea esi, [edx + esi]//Nt头
			mov esi, [esi + 0x78]//导出表RVA
			
			lea esi, [edx + esi]//导出表VA
			mov edi, [esi + 0x1c]//EAT RVA
			lea edi, [edx + edi]//EAT VA
			mov[ebp - 0x4], edi//eatva
			mov edi,[esi + 0x20]//ENT RVA .
			lea edi, [edx + edi]//ENT va
			mov [ebp - 0x8], edi//ENTVA
			mov edi, [esi + 0x24]//EOT RVA
			lea edi, [edx + edi]//
			mov [ebp - 0xc], edi//EOTVA
			//比较字符串获取API 
			xor eax, eax
			cld
			jmp tag_cmpfirst
	tag_cmpLoop :
		   inc eax
	tag_cmpfirst :
		    mov esi, [ebp - 0x8]//ENT
			mov esi, [esi+eax*4]//RVA
			lea esi, [edx + esi]//函数名称字符串
			mov edi, [ebp + 0xc]//要查找的目标函数名称
		    mov ecx,[ebp+0x10]//循环次数
			repe cmpsb
			jne tag_cmpLoop
			mov esi, [ebp - 0xc]//eot
			xor edi, edi//为了不影响结果清空edi
			mov di,[esi+eax*2]//eat表索引
			mov edx, [ebp - 0x4]//eat
			mov esi, [edx + edi * 4]//函数地址rva
			mov edx,[ebp+0x8]// d1lbase
			lea eax, [edx + esi]// funaddr va
			pop ecx;
			pop ebx;
			pop edx;
			pop edi;
			pop esi;
			mov esp, ebp;
			pop ebp;
			retn 0xc;
	fun_payload:
			push ebp;
			mov ebp, esp;
			sub esp,0x10;
			push esi;
			push edi;
			push edx;
			push ebx;
			push ecx;

			//1.先拿到d1lbase
			call fun_GetModule
			mov [ebp-0x4], eax
			//2.获取LoadLibraryA
			push 0xd
			mov ecx, [ebp + 0x8]//获取字符串首地址push ecx
			push eax
			call fun_GetProcAddr
			mov[ebp - 0x8], eax//LoadLibraryA地址
			//3.获取GetProcAddress
			push 0xf
			lea ecx, [ecx + 0xd]//GetProcAddress
			push ecx//字符串首地址
			push[ebp - 0x4]//dllbase
			call fun_GetProcAddr
			mov[ebp - 0xc], eax//GetProcAddress函数地址

			//4.调用LoadLibraryA加载user32.dl1
			mov ecx, [ebp + 0x8]
			lea ecx, [ecx + 0x1c] //user31.dll字符串地址
			push ecx
			call[ebp - 0x8]//调用loadlibraya获取user32.dl1
			mov [ebp-0x10], eax//user32base
			//5.调用GetProcaddedss获取MessageBoxA地址
			mov ecx,[ebp + 0x8]
			lea ecx, [ecx + 0x27]//MessageBoxA字符串
			push ecx
			push [ebp - 0x10]
			call [ebp - 0xc]
			mov [ebp - 0x14], eax
			//6.输出hello 51hook
			push 0
			push 0
			mov ecx,[ebp + 0x8]
			lea ecx, [ecx + 0x33]
			push ecx
			push 0
			call[ebp - 0x14]
			pop ecx;
			pop ebx;
			pop edx;
			pop edi;
			pop esi;
			mov esp, ebp;
			pop ebp;
			retn 0x4;




	}
}

int main()
{
	std::cout << "51hook" << std::endl;
	shellcode();
	return 0;
}