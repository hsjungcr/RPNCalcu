#include "rpnCalc.h"
//-------------------------------------------------------------------------------------------
//    Class:		CRPNCalc
//
//    File:             RPNCalc.cpp
//
//    Description:	This file contains the function definitions for CRPNCalc
//
//    Programmer:		
//   
//    Date:             
// 
//    Version:          1.0
//  
//    Environment:	Intel Xeon PC 
//                  	Software:   MS Windows 7 for execution; 
//                  	Compiles under Microsoft Visual C++.Net 2010
// 
//	  class CRPNCalc:
//
//	  Properties:
//				double m_registers[10];
//				stack<string> m_stack;
//				vector<list<string>> m_programs(NUMPROGRAMS);
//				istringstream m_inStrStream;
//	
//
//	  Non-inline Methods:
//	
//				CRPNCalc(bool on)
//				run()
//				print(ostream& ostr)
//				parse()
//				void add() -- 
//				void bin_prep(double& d1, double& d2) -- 
//				void clear() -- 
//				void clearAll() -- 
//				void divide() -- 
//				void exp() -- 
//				void getReg(int reg) -- 
//				void loadProgram() -- 
//				void mod() -- 
//				void multiply() -- 
//				void neg() -- 
//				void parse() -- 
//				void recordProgram() -- 
//				void rotateUp() -- 
//				void rotateDown() -- 
//				void runProgram() -- 
//				void saveToFile() -- 
//				void setReg(int reg) -- 
//				void subtract() -- 
//				void unary_prep(double& d) -- 

//	  related functions:
//				ostream &operator <<(ostream &ostr, const CRPNCalc &calc)
//    				istream &operator >>(istream &istr, CRPNCalc &calc)
//
//    History Log:
//                           
// ----------------------------------------------------------------------------	
namespace PB_CALC
{
	// ----------------------------------------------------------------------------
	//	constructor
	// ----------------------------------------------------------------------------
	CRPNCalc::CRPNCalc(bool on) : m_on(on), m_error(false), m_helpOn(true),
		m_programRunning(false)
	{
		for (int i = 0; i < NUMREGS; i++)
			m_registers[i] = 0.0;
		if (m_on)
			run();
	}

	// ----------------------------------------------------------------------------
	//	starts the calculator running
	// ----------------------------------------------------------------------------
	void CRPNCalc::run()
	{
		while (m_on)
		{
			system("CLS");
			print(cout);
			input(cin);
		}
	}

	// ----------------------------------------------------------------------------
	//	prints out calculator screen
	// ----------------------------------------------------------------------------
	void CRPNCalc::print(ostream& ostr)
	{
		double d = 0.0;
		ostr << "[RPN Programmable Calculator] by Han Jung, Cheuk Chi Chow and "
			<< "Hui Nguyen" << endl;
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

	// ----------------------------------------------------------------------------
	//	parses the next command from m_instrStream
	// ----------------------------------------------------------------------------
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
						clearAll();
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
						clearEntry();
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

	// ----------------------------------------------------------------------------
	//	if possible, pops top 2 elements from the stack, adds them
	//	  and pushes the result onto the stack
	// ----------------------------------------------------------------------------	
	void CRPNCalc::add()
	{
		bool isEmpty = false;
		double sum, operandA, operandB;
		if (!m_stack.empty())
		{
			operandA = m_stack.front();
			m_stack.pop_front();
		}
		else
		{
			isEmpty = true;
		}
		if (!m_stack.empty())
		{
			operandB = m_stack.front();
			m_stack.pop_front();
		}
		else
		{
			isEmpty = true;
		}
		if (!isEmpty) {
			sum = operandA + operandB;

			m_stack.push_front(sum);
		}
		else
		{
			m_error = true;
		}
	}

	// ----------------------------------------------------------------------------
	//	sets the args to the popped values from the stack, if possible
	//	  set error state otherwise
	// ----------------------------------------------------------------------------

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

	// ----------------------------------------------------------------------------
	//	removes the top element from the stack
	// ----------------------------------------------------------------------------
	void CRPNCalc::clearEntry()
	{
		if (!m_stack.empty())
			m_stack.pop_front();
	}

	// ----------------------------------------------------------------------------
	//	empties the stack
	// ----------------------------------------------------------------------------
	void CRPNCalc::clearAll()
	{
		while (!m_stack.empty())
			m_stack.pop_front();
	}

	// ----------------------------------------------------------------------------
	//	if possible, pops top 2 elements from the stack, divides them
	//	  and pushes the result onto the stack
	// ----------------------------------------------------------------------------
	void CRPNCalc::divide()
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
			m_stack.push_front(d1 / d2);
	}

	// ----------------------------------------------------------------------------
	//	if possible, pops top 2 elements from the stack,
	//	  raises one element to the other power
	//	  and pushes the result onto the stack
	// ----------------------------------------------------------------------------
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
			m_stack.push_front(pow(d1, d2));
	}

	// ----------------------------------------------------------------------------
	//	pushes the given register's value onto the stack
	// ----------------------------------------------------------------------------
	void CRPNCalc::getReg(int reg)
	{
		if (reg >= 0 && reg <= 9)
			m_stack.push_front(m_registers[reg]);
		else
			m_error = true;
	}

	// ----------------------------------------------------------------------------
	//	retrieves the filename from the user and loads it into m_program
	// ----------------------------------------------------------------------------
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

	// ----------------------------------------------------------------------------
	//	if possible, pops top 2 elements from the stack, mods them
	//	  and pushes the result onto the stack
	// ----------------------------------------------------------------------------
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
			m_stack.push_front(fmod(d1, d2));
	}

	// ----------------------------------------------------------------------------
	//	if possible, pops top 2 elements from the stack, multiplies them
	//	  and pushes the result onto the stack
	// ----------------------------------------------------------------------------
	void CRPNCalc::multiply()
	{
		double d1 = 0.0;
		double d2 = 0.0;
		binary_prep(d1, d2);
		if (m_error)
			return;
		m_stack.push_front(d1 * d2);
	}

	// ----------------------------------------------------------------------------
	//	resets the top element of the stack to it's negative
	//	top element = front
	// ----------------------------------------------------------------------------
	void CRPNCalc::neg()
	{
		double d = 0;
		unary_prep(d);
		if (m_error)
			return;
		d *= -1.0;
		m_stack.push_front(d);
	}

	// ----------------------------------------------------------------------------
	//	sets the arg to the popped value from the stack, if possible
	//	  sets error state otherwise
	// ----------------------------------------------------------------------------
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

	// ----------------------------------------------------------------------------
	//	takes command-line input and loads it into m_program 
	// ----------------------------------------------------------------------------
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

	// ----------------------------------------------------------------------------
	//	removes the bottom of the stack and adds it to the top
	// ----------------------------------------------------------------------------
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

	// ----------------------------------------------------------------------------
	//	removes the top of the stack and adds it to the bottom
	// ----------------------------------------------------------------------------
	void CRPNCalc::rotateUp()
	{
		if (!m_stack.empty()) {
			double temp = m_stack.front();
			m_stack.pop_front();
			m_stack.push_back(temp);
		}
		else
		{
			m_error = true;
		}
	}

	// ----------------------------------------------------------------------------
	//	runs the program in m_program 
	// ----------------------------------------------------------------------------
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

	// ----------------------------------------------------------------------------
	//	asks the user for a filename and saves m_program to that file
	// ----------------------------------------------------------------------------
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

	// ----------------------------------------------------------------------------
	//	gets the value from the top of the stack
	//	  and places it into the given register
	// ----------------------------------------------------------------------------
	void CRPNCalc::setReg(int reg)
	{
		if (reg >= 0 && reg <= 9 && !m_stack.empty())
			m_registers[reg] = m_stack.front();
		else
			m_error = true;
	}

	// ----------------------------------------------------------------------------
	//	if possible, pops top 2 elements from the stack, subtracts them
	//	  and pushes the result onto the stack
	// ----------------------------------------------------------------------------
	void CRPNCalc::subtract()
	{
		double d1 = 0.0;
		double d2 = 0.0;
		binary_prep(d1, d2);
		if (m_error)
			return;
		m_stack.push_front(d1 - d2);
	}

	// ----------------------------------------------------------------------------
	//	inputs a line from the given stream
	// ----------------------------------------------------------------------------
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

	// ----------------------------------------------------------------------------
	//	ostream's << defined for CRPNCalc
	// ----------------------------------------------------------------------------
	ostream &operator <<(ostream &ostr, CRPNCalc &calc)
	{
		calc.print(ostr);
		return ostr;
	}


	// ----------------------------------------------------------------------------
	//	istream's >> defined for CRPNCalc
	// ----------------------------------------------------------------------------
	istream &operator >> (istream &istr, CRPNCalc &calc)
	{
		calc.input(istr);
		return istr;
	}
}