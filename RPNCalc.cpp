#include "rpnCalc.h"
//-------------------------------------------------------------------------------------------
//    Class:		CRPNCalc
//
//    File:			RPNCalc.cpp
//
//    Description:	This file contains the function definitions for CRPNCalc
//
//    Programmer:	Han Jung
//					Cheuk Chi Chow
//					Huy Ngyuen
//   
//    Date:			6/11/2017
// 
//    Version:		1.0
//  
//    Environment:	Intel Xeon PC 
//                  Software:   MS Windows 10 for execution; 
//                  Compiles under Microsoft Visual C++.Net 2017
// 
//	  class:		CRPNCalc
//
//	  Properties:
//				double m_registers[NUMREGS];
//				string m_buffer;
//				deque<double> m_stack;
//				list<string> m_program;
//				istringstream m_instrStream;
//				bool m_error;
//				bool m_helpOn;
//				bool m_on;
//				bool m_programRunning;
//
//	  Non-inline Methods:
//				CRPNCalc(bool on = true);
//				void run();
//				void print(ostream& ostr);  // changes m_error on error, so not const
//				void input(istream& istr);
//
//				private:
//					// private methods
//					void add();
//					void binary_prep(double& d1, double& d2);
//					void clearEntry();
//					void clearAll();
//					void divide();
//					void exp();
//					void getReg(int reg);
//					void loadProgram();
//					void mod();
//					void multiply();
//					void neg();
//					void parse();
//					void recordProgram();
//					void rotateUp();
//					void rotateDown();
//					void runProgram();
//					void saveToFile();
//					void setReg(int reg);
//					void subtract();
//					void unary_prep(double& d);
//	  related functions:
//				ostream &operator <<(ostream &ostr, const CRPNCalc &calc)
//    			istream &operator >>(istream &istr, CRPNCalc &calc)
//
//    History Log:
//				6/3/2017	HJ Initial setups
//				6/9/2017	CC completed version 0.1
//				6/10/2017	HN completed version 1.0
//				6/11/2017	HJ completed version 1.1
// ----------------------------------------------------------------------------	
namespace PB_CALC
{
	//-------------------------------------------------------------------------
	//		method:			CRPNCalc(bool on)
	//		description:	constructor which takes boolean value for m_on as
	//						an argument and runs calculator if m_on is true.
	//		calls:			run()
	//		called by:		main();
	//
	//		parameters:		boolean value
	//			
	//		returns:		n/a
	//		History Log:
	//					5/31/2017 HJ completed version 1.0
	// -------------------------------------------------------------------------
	CRPNCalc::CRPNCalc(bool on) : m_on(on), m_error(false), m_helpOn(true),
		m_programRunning(false)
	{
		for (int i = 0; i < NUMREGS; i++)
			m_registers[i] = 0.0;
		if (m_on)
			run();
	}
	//-------------------------------------------------------------------------
	//		method:			run()
	//		description:	runs the calculator method if m_on is set to true.
	//		calls:			print()
	//						input()
	//						
	//		called by:		run()
	//		parameters:		n/a
	//			
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::run()
	{
		while (m_on)
		{
			system("CLS");
			print(cout);
			input(cin);
		}
	}
	//-------------------------------------------------------------------------
	//		method:			print(ostream& ostr)
	//		description:	prints out authors and help menu and top number of 
	//						the stack
	//		calls:			n/a
	//		called by:		run()
	//
	//		parameters:		ostream& ostr -- ostream to print to.
	//			
	//		returns:		n/a
	//		History Log:
	//					5/31/2017 HJ completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::print(ostream& ostr)
	{
		double d = 0.0;
		ostr << "[RPN Programmable Calculator] by Han Jung, Cheuk Chi Chow and "
			<< "Huy Nguyen" << endl;
		if (m_helpOn)
			cout << helpMenu;
		else
			cout << endl << endl << endl;
		cout << line;
		if (!m_stack.empty())
		{
			d = m_stack.front();
			ostr << d;
		}
		ostr << endl << endl;
		if (m_error)
		{
			ostr << "<<error>>" << endl;
			m_error = false;
		}
	}
	//-------------------------------------------------------------------------
	//		method:			parse()
	//		description:	parses the next command from m_instrStream
	//		calls:			add()
	//						clearEntry()
	//						clearAll()
	//						divide()
	//						exp()
	//						getReg()
	//						loadProgram()
	//						mod()
	//						multiply()
	//						neg()
	//						recordProgram()
	//						rotateDown()
	//						rotateUp()
	//						runProgram()
	//						saveToFile()
	//
	//		called by:		input()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::parse()
	{
		double number = 0;
		while (m_buffer.length() != 0)
		{
			bool delDecimal = false;
			//erase the spaces at the beginning of the string
			while (m_buffer[0] == ' ')
				m_buffer.erase(m_buffer.begin());
			number = atof(m_buffer.c_str());
			//if it is a number
			if ((number != 0 && m_buffer[0] != '+') || m_buffer[0] == '0')
			{
				if (m_buffer[0] == '-')
					m_buffer.erase(m_buffer.begin());
				while ((m_buffer[0] >= '0' && m_buffer[0] <= '9')
					|| m_buffer[0] == '.')
				{
					if (m_buffer[0] == '.')
						if (delDecimal == false)
							delDecimal = true;
						else
							break;
					m_buffer.erase(m_buffer.begin());
					if (m_buffer.length() == 0)
						break;
				}
				m_stack.push_front(number);
			}
			else
			{
				string token;
				//if the beginning is a character
				if (m_buffer.length() >= 2)
				{
					//special situation with CE
					if (toupper(m_buffer[0]) == 'C'
						&& toupper(m_buffer[1]) == 'E')
					{
						m_buffer.erase(m_buffer.begin(), m_buffer.begin() + 2);
						clearEntry();
						continue;
					}
					//special situation with -0
					else if (m_buffer[0] == '-' && m_buffer[1] == '0')
					{
						m_buffer.erase(m_buffer.begin());
						while (m_buffer[0] == '0')
						{
							m_buffer.erase(m_buffer.begin());
							if (m_buffer.length() == 0)
								break;
						}
						m_stack.push_front(number);
						neg();
						continue;
					}
					//special situation with S0-9
					else if (toupper(m_buffer[0]) == 'S' &&
						m_buffer[1] >= '0' && m_buffer[1] <= '9')
					{
						m_buffer.erase(m_buffer.begin()); // delete the 'S' to get the number
						char index = m_buffer[0];
						setReg(static_cast<int>(index) - ZEROINASCII);
						m_buffer.erase(m_buffer.begin()); // delete the number
						continue;
					}
					//special situation with G0-9
					else if (toupper(m_buffer[0]) == 'G' &&
						m_buffer[1] >= '0' && m_buffer[1] <= '9')
					{
						m_buffer.erase(m_buffer.begin()); // delete the 'G' to get the number
						char index = m_buffer[0];
						getReg(static_cast<int>(index) - ZEROINASCII);
						m_buffer.erase(m_buffer.begin()); // delete the number
						continue;
					}
				}
				if (m_buffer.length() != 0)
				{
					token = m_buffer[0];
					if (0 == token.compare("+"))
						add();
					else if (0 == token.compare("-"))
						subtract();
					else if (0 == token.compare("*"))
						multiply();
					else if (0 == token.compare("/"))
						divide();
					else if (0 == token.compare("^"))
						exp();
					else if (0 == token.compare("%"))
						mod();
					else if (0 == token.compare("c") || 0 == token.compare("C"))
						clearAll();
					else if (0 == token.compare("d") || 0 == token.compare("D"))
						rotateDown();
					else if (0 == token.compare("f") || 0 == token.compare("F"))
						saveToFile();
					else if (0 == token.compare("h") || 0 == token.compare("H"))
						m_helpOn = !m_helpOn;
					else if (0 == token.compare("l") || 0 == token.compare("L"))
						loadProgram();
					else if (0 == token.compare("m") || 0 == token.compare("M"))
						neg();
					else if (0 == token.compare("p") || 0 == token.compare("P"))
						recordProgram();
					else if (0 == token.compare("r") || 0 == token.compare("R"))
					{
						//the application only do this method and ignore other methods
						//if they are inputed at the same line
						runProgram();
						return;
					}
					else if (0 == token.compare("u") || 0 == token.compare("U"))
						rotateUp();
					else if (0 == token.compare("x") || 0 == token.compare("X"))
						m_on = false;
					else
						m_error = true;
					m_buffer.erase(m_buffer.begin());
				}
			}
		}
	}
	//-------------------------------------------------------------------------
	//		method:			add()
	//		description:	if possible, pops top 2 elements from the stack,
	//						adds them and pushes the result onto the stack
	//		calls:			binary_prep()
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.1
	//					6/8/2017  HJ completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::add()
	{
		double d1 = 0.0;
		double d2 = 0.0;
		binary_prep(d1, d2);
		if (m_error)
			return;
		m_stack.push_front(d1 + d2);
	}
	//-------------------------------------------------------------------------
	//		method:			binary_prep()
	//		description:	Check if operation requiring two number from stack
	//						is possible and save the popped value to argument's
	//						address if it is possible
	//		calls:			n/a
	//		called by:		add()
	//						subtract()
	//						multiply()
	//						divide()
	//						exp()
	//						mod()			
	//						
	//		parameters:		double& d1 -- address of operand 1
	//						double& d2 -- address of operand 2
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::binary_prep(double& d1, double& d2)
	{

		if (m_stack.size() >= 2)
		{
			d1 = m_stack.front();
			m_stack.pop_front();
			d2 = m_stack.front();
			m_stack.pop_front();
		}
		else
		{
			m_error = true;
		}
	}
	//-------------------------------------------------------------------------
	//		method:			clearEntry()
	//		description:	Clear the last entered number
	//		calls:			n/a
	//		called by:		parse()	
	//						
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HJ completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::clearEntry()
	{
		if (!m_stack.empty())
			m_stack.pop_front();
	}
	//-------------------------------------------------------------------------
	//		method:			clearAll()
	//		description:	Clear all the values stored in the stack
	//		calls:			n/a
	//		called by:		parse()	
	//						
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HJ completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::clearAll()
	{
		while (!m_stack.empty())
			m_stack.pop_front();
	}
	//-------------------------------------------------------------------------
	//		method:			divide()
	//		description:	if possible, pops top 2 elements from the stack, 
	//						divides them and pushes the result onto the stack
	//		calls:			binary_prep()
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.1
	//					6/8/2017  HJ completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::divide()
	{
		double d1 = 0.0;
		double d2 = 0.0;
		binary_prep(d1, d2);
		if (m_error)
			return;
		if (d1 == 0)
		{
			//do nothing, push the numbers back
			m_error = true;
			m_stack.push_front(d2);
			m_stack.push_front(d1);
		}
		else
			m_stack.push_front(d2 / d1);
	}
	//-------------------------------------------------------------------------
	//		method:			exp()
	//		description:	if possible, pops top 2 elements from the stack, 
	//						and exponentiate top value by the next value and 
	//						pushes result back to the top
	//		calls:			binary_prep()
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.1
	//					6/8/2017  CC completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::exp()
	{
		double d1 = 0.0;
		double d2 = 0.0;
		binary_prep(d1, d2);
		if (m_error)
			return;
		if (d1 == 0 && d2 == 0)
		{
			//do nothing, push the numbers back
			m_error = true;
			m_stack.push_front(d1);
			m_stack.push_front(d2);
		}
		else
			m_stack.push_front(pow(d2, d1));
	}
	//-------------------------------------------------------------------------
	//		method:			getReg()
	//		description:	pushes the given register's value onto the stack
	//		calls:			n/a
	//		called by:		parse()
	//		parameters:		int reg -- size of the register
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::getReg(int reg)
	{
		if (reg >= 0 && reg <= 9)
			m_stack.push_front(m_registers[reg]);
		else
			m_error = true;
	}
	//-------------------------------------------------------------------------
	//		method:			loadProgram()
	//		description:	retrieves the filename from the user and loads it 
	//						into m_program
	//		calls:			n/a
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::loadProgram()
	{
		ifstream fin;
		string filename;
		cout << "Please enter the name for the file: ";
		cin >> filename;
		cin.ignore(BUFFERSIZE, '\n');
		filename = filename;
		fin.open(filename);
		if (fin.is_open())
		{
			while (!m_program.empty())
				m_program.pop_front();
			string input;
			while (!fin.eof())
			{
				getline(fin, input);
				m_program.push_back(input);
			}
			fin.close();
		}
	}
	//-------------------------------------------------------------------------
	//		method:			mod()
	//		description:	if possible, pops top 2 elements from the stack, 
	//						and mod top value by the next value and pushes
	//						result back to the top
	//		calls:			binary_prep()
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.1
	//					6/8/2017  HJ completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::mod()
	{
		double d1 = 0.0;
		double d2 = 0.0;
		binary_prep(d1, d2);
		if (m_error)
			return;
		if (d2 == 0)
		{
			//do nothing, push the numbers back
			m_error = true;
			m_stack.push_front(d1);
			m_stack.push_front(d2);
		}
		else
			m_stack.push_front(fmod(d2, d1));
	}
	//-------------------------------------------------------------------------
	//		method:			multiply()
	//		description:	if possible, pops top 2 elements from the stack, 
	//						and multiply top value by the next value and pushes
	//						result back to the top
	//		calls:			binary_prep()
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.1
	//					6/8/2017  CC completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::multiply()
	{
		double d1 = 0.0;
		double d2 = 0.0;
		binary_prep(d1, d2);
		if (m_error)
			return;
		m_stack.push_front(d1 * d2);
	}
	//-------------------------------------------------------------------------
	//		method:			neg()
	//		description:	if possible, pops the first value then multiply it
	//						by negative 1 and pushes the returned value back
	//		calls:			binary_prep()
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.1
	//					6/8/2017  HJ completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::neg()
	{
		double d = 0;
		unary_prep(d);
		if (m_error)
			return;
		d *= -1.0;
		m_stack.push_front(d);
	}
	//-------------------------------------------------------------------------
	//		method:			unary_prep(double& d)
	//		description:	if possible, pops the first value of the stack then
	//						assign it to the address of the argument.
	//		calls:			n/a
	//		called by:		neg()
	//		parameters:		double& d --address of the variable assigning to
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.1
	//					6/8/2017  HJ completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::unary_prep(double& d)
	{
		if (!m_stack.empty())
		{
			d = m_stack.front();
			m_stack.pop_front();
		}
		else
		{
			m_error = true;
		}
	}
	//-------------------------------------------------------------------------
	//		method:			recordProgram()
	//		description:	takes command-line input and loads it into m_program 
	//		calls:			n/a
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/10/2017 HN completed version 1.0
	// -------------------------------------------------------------------------
	void CRPNCalc::recordProgram()
	{
		m_programRunning = true;
		int j = 0;
		while (!m_program.empty())
			m_program.pop_front();
		while (m_programRunning)
		{
			string token;
			int i = 0;
			cout << j << "> ";
			j++;
			getline(cin, token);
			for (; i < token.length(); i++)
				if (toupper(token[i]) == 'P')
					break;
			if (i != token.length())
			{
				m_programRunning = false;
				token.erase(token.begin() + i, token.end());
			}
			m_program.push_back(token);
		}
	}
	//-------------------------------------------------------------------------
	//		method:			rotateDown()
	//		description:	removes the bottom of the stack and adds it to the
	//						top
	//		calls:			n/a
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/8/2017 HJ completed version 1.0
	//-------------------------------------------------------------------------
	void CRPNCalc::rotateDown()
	{
		if (!m_stack.empty())
		{
			double temp = m_stack.back();
			m_stack.pop_front();
			m_stack.push_back(temp);
		}
		else
			m_error = true;
	}
	//-------------------------------------------------------------------------
	//		method:			rotateDown()
	//		description:	removes the top of the stack and adds it to the
	//						bottom
	//		calls:			n/a
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/8/2017 HJ completed version 1.0
	//-------------------------------------------------------------------------
	void CRPNCalc::rotateUp()
	{
		if (!m_stack.empty()) {
			double temp = m_stack.back();
			m_stack.pop_back();
			m_stack.push_front(temp);
		}
		else
		{
			m_error = true;
		}
	}
	//-------------------------------------------------------------------------
	//		method:			runProgram()
	//		description:	runs the program in m_program 
	//		calls:			n/a
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/8/2017 HN completed version 1.0
	//-------------------------------------------------------------------------
	void CRPNCalc::runProgram()
	{
		list<string>::const_iterator sit = m_program.begin();
		while (sit != m_program.end())
		{
			m_buffer = *sit;
			parse();
			sit++;
		}
	}
	//-------------------------------------------------------------------------
	//		method:			saveToFile()
	//		description:	asks the user for a filename and saves m_program
	//						to that file
	//		calls:			n/a
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/8/2017 HN completed version 1.0
	//-------------------------------------------------------------------------
	void CRPNCalc::saveToFile()
	{
		ofstream fout;
		string filename;
		cout << "Please enter the name for the file: ";
		cin >> filename;
		cin.ignore(BUFFERSIZE, '\n');
		filename = filename;
		fout.open(filename, 'w');
		if (fout.is_open())
		{
			list<string>::const_iterator sit = m_program.begin();
			while (sit != m_program.end())
			{
				fout << *sit;
				fout << endl;
				sit++;
			}
			fout.close();
		}
	}
	//-------------------------------------------------------------------------
	//		method:			setReg()
	//		description:	if the index of register is within limits of 0 
	//						and 10 take the top of the stack and asign it to 
	//						given index
	//		calls:			n/a
	//		called by:		parse()
	//		parameters:		int reg --index of the register
	//		returns:		n/a
	//		History Log:
	//					6/8/2017 HN completed version 1.0
	//-------------------------------------------------------------------------
	void CRPNCalc::setReg(int reg)
	{
		if (reg >= 0 && reg <= 9 && !m_stack.empty())
			m_registers[reg] = m_stack.front();
		else
			m_error = true;
	}
	//-------------------------------------------------------------------------
	//		method:			subtract()
	//		description:	Take two values from the stack, subtract them 
	//						then push it back to the register
	//		calls:			n/a
	//		called by:		parse()
	//		parameters:		n/a
	//		returns:		n/a
	//		History Log:
	//					6/11/2016 HJ completed version 1.1
	//					6/8/2017 HN completed version 1.0
	//-------------------------------------------------------------------------
	void CRPNCalc::subtract()
	{
		double d1 = 0.0;
		double d2 = 0.0;
		binary_prep(d1, d2);
		if (m_error)
			return;
		m_stack.push_front(d2 - d1);
	}
	//-------------------------------------------------------------------------
	//		method:			input(istream &istr)
	//		description:	get line from std::cin and parse input
	//		calls:			parse()
	//		called by:		loadProgram()
	//						operator>> () 
	//		parameters:		istream &istr address of the stream to load it to
	//		returns:		n/a
	//		History Log:
	//					6/11/2016 HJ completed version 1.1
	//					6/8/2017 HN completed version 1.0
	//-------------------------------------------------------------------------
	void CRPNCalc::input(istream &istr)
	{
		try
		{
			if (getline(cin, m_buffer))
				parse();
			else
				throw invalid_argument("Could not read input.");
		}
		catch (invalid_argument e)
		{
			cout << e.what() << endl;
		}
	}
	//-------------------------------------------------------------------------
	//		method:			operator <<(ostream &ostr, CRPNCalc &calc)
	//		description:	<< operator overloading for CRPNCalc Class
	//		calls:			print(ostr);
	//		called by:		main()
	//		parameters:		ostream &ostr --address of the ostream to load it 
	//						to
	//						CPRNCalc &calc -- CPRN class to call print() from
	//		returns:		ostream
	//		History Log:
	//					5/31/2016 PB completed version 1.0
	//-------------------------------------------------------------------------
	ostream &operator <<(ostream &ostr, CRPNCalc &calc)
	{
		calc.print(ostr);
		return ostr;
	}
	//-------------------------------------------------------------------------
	//		method:			operator <<(ostream &ostr, CRPNCalc &calc)
	//		description:	>> operator overloading for CRPNCalc Class
	//		calls:			input(ostr);
	//		called by:		main()
	//		parameters:		istream &istr --address of the istream to load it 
	//						to
	//						CPRNCalc &calc -- CPRN class to call print() from
	//		returns:		istream
	//		History Log:
	//					5/31/2016 PB completed version 1.0
	//-------------------------------------------------------------------------
	istream &operator >> (istream &istr, CRPNCalc &calc)
	{
		calc.input(istr);
		return istr;
	}
}
