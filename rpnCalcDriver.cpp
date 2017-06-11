//----------------------------------------------------------------------------
// CalcDriver.cpp
//
// functions: main()
//----------------------------------------------------------------------------
#include <iostream>
#include <cstdlib>
#include "RPNCalc.h"

using namespace std;
//----------------------------------------------------------------------------
//	Function:		main()
//	Title:			Driver for RPN Calculator
//	Description:	This file contains function main()
//					which creates and starts a calculator
//	Programmer:		Han S. Jung
//					Chi Cheuk Chow
//					Huy Nguyen
//	Version:		1.0
//	Environment:	Intel Xeon PC 
//					Software:   MS Windows 7 for execution; 
//					Compiles under Microsoft Visual C++.Net 2012
//	Calls:			CRPNCalc constructor
//	Returns:		EXIT_SUCCESS  = successful 
//	History Log:
//					6/10/17  HJ  completed version 1.0
//----------------------------------------------------------------------------
int main(void)
{
	using PB_CALC::CRPNCalc;
	CRPNCalc myCalc;
	cout << endl << "Press \"enter\" to continue";
	cin.get();

	return EXIT_SUCCESS;
}
