/*
*    EOS - Experimental Operating System
*    Boot page directory generator
     Generates boot page directory which identity mapps all ram
*/
#include <bits/stdc++.h>
using namespace std;

int main()
{
    ofstream out("bpg_generated.txt");
    for (uint32_t i = 0; i < 1024; i++)
    {
        if (i == (0xC0000000 >> 22)) //map 4mb after kernel virt base. tak i bylo do etogo zhe smotri kak bylo v boot.s tam zhe zakommencheno kak bylo
            out << ".long 0x" << setfill('0') << setw(8) << hex << (0x00000083) << endl;
        else
        out << ".long 0x" << setfill('0') << setw(8) << hex << (0x00000083 + i*0x00400000) << endl;
    }
	return 0;
}
