#pragma once
#include<iostream>
#include<Windows.h>
#include<conio.h>
#include<time.h>
#include<list>
#include<vector>
#include<map>
#include<unordered_map>
#include<fstream>
#include<string>
#include<stack>
#include<algorithm>
#include<cctype>
using namespace std;
#define FG_BLACK        0
#define FG_BLUE         FOREGROUND_BLUE
#define FG_GREEN        FOREGROUND_GREEN
#define FG_CYAN         (FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FG_RED          FOREGROUND_RED
#define FG_MAGENTA      (FOREGROUND_RED | FOREGROUND_BLUE)
#define FG_YELLOW       (FOREGROUND_RED | FOREGROUND_GREEN)
#define FG_WHITE        (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FG_BRIGHT       FOREGROUND_INTENSITY
#define BG_BLACK        0
#define BG_BLUE         BACKGROUND_BLUE
#define BG_GREEN        BACKGROUND_GREEN
#define BG_CYAN         (BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BG_RED          BACKGROUND_RED
#define BG_MAGENTA      (BACKGROUND_RED | BACKGROUND_BLUE)
#define BG_YELLOW       (BACKGROUND_RED | BACKGROUND_GREEN)
#define BG_WHITE        (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BG_BRIGHT       BACKGROUND_INTENSITY
void getRowColbyLeftClick(int& rpos, int& cpos)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Events;
	INPUT_RECORD InputRecord;
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	do
	{
		ReadConsoleInput(hInput, &InputRecord, 1, &Events);
		if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
			rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			break;
		}
	} while (true);
}
void getRowCol(int& rpos, int& cpos)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		cpos = csbi.dwCursorPosition.X;
		rpos = csbi.dwCursorPosition.Y;
	}
}
void gotoRowCol(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}
bool isCursorAtEnd(int cpos)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	return cpos == csbi.dwSize.X;
}
bool isCapacityReached(int cpos)
{
	cpos += 2;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	return cpos >= csbi.dwSize.X;
}
bool isCursorAtFront()
{
	int rpos, cpos;
	getRowCol(rpos, cpos);
	return cpos == 0;
}
void SetClr(int clr)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
}
struct Pos
	{
		int ri;
		int ci;
		Pos(int ri = 0, int ci = 0)
		{
			this->ri = ri;
			this->ci = ci;
		}
	};
class Editor
{
private:
	struct Line
	{
		list<char>chars;
		void Insert(char input, list<char>::iterator& it, int& ci)
		{
			auto temp = it;
			temp = (temp == this->chars.end()) ? temp : ++temp;
			if (input == '\n' && this->chars.size() && this->chars.back() == '\n' && temp == this->chars.end())
				return;
			if (it != this->chars.end() && *it == '\n')
			{
				for (int i = 0; i < 2 && it != this->chars.begin(); i++)
					it--;
				this->chars.pop_back();
				this->chars.pop_back();
				ci--; ci--;
			}
			if (it == this->chars.end())
			{
				this->chars.push_back(' ');
				this->chars.push_back(input);
				it = this->chars.begin();
			}
			else if (it == this->chars.begin())
			{
				this->chars.insert(it, ' ');
				this->chars.insert(it, input);
				it = this->chars.begin();
			}
			else
			{
				temp = it;temp++;
				this->chars.insert(temp, ' ');
				temp = it;temp++;temp++;
				this->chars.insert(temp, input);
			}
		}
		void Remove(list<char>::iterator& it)
		{
			if (it == this->chars.begin())
				return;
			if (this->chars.size())
			{
				bool begin = false;
				auto temp = it; it--;
				this->chars.erase(temp);
				temp = it;
				if (it == this->chars.begin())
				{
					if (this->chars.size() == 0)
						it = this->chars.end();
					else
						begin = true;
				}
				else
					it--;
				this->chars.erase(temp);
				it = (begin) ? this->chars.begin() : it;
				cout << "\b\b" << "  " << "\b\b";
			}
		}
		void Read(const string& str)
		{
			for (int i = 0; i < str.size(); i++)
				this->chars.push_back(str[i] + 10);
			if (this->chars.size() && this->chars.back() == '\n')
				return;
			this->chars.push_back(' ');
			this->chars.push_back('\n');
		}
		void FIND(string opr, vector<vector<Pos>>& range, int ri)
		{
			if (opr.empty())
			{
				cout << "Invalid or missing word...";
				return;
			}
			if (!this->chars.size())
				return;
			transform(opr.begin(), opr.end(), opr.begin(), tolower);
			bool first = true, found = false;
			int oi = 0;
			Pos rangei;
			rangei.ri = ri; rangei.ci = 1;
			for (auto i = ++this->chars.begin(); i != this->chars.end();)
			{
				if (tolower(*i) == opr[oi])
				{
					found = true;
					if (first)
					{
						vector<Pos>push;
						range.push_back(push);
						first = false;
					}
					range.back().push_back(rangei);
					oi++;
					if (oi == opr.length())
					{
						oi = 0;
						first = true;
						found = false;
					}
				}
				else if (found)
				{
					oi = 0;
					found = false;
					first = true;
					range.pop_back();
				}
				rangei.ci += 2;
				for (int j = 0; j < 2 && i != this->chars.end(); j++)
					i++;
			}
		}
		void FINDFROMCURSOR(string command, string opr, vector<vector<Pos>>& range, int ri, const Pos& MPos,
			const list<char>::iterator& curr)
		{
			if (opr.empty())
			{
				cout << "Invalid or missing word...";
				return;
			}
			if (!this->chars.size())
				return;
			transform(opr.begin(), opr.end(), opr.begin(), tolower);
			bool first = true, found = false;
			int oi = 0;
			Pos rangei;
			rangei.ri = ri; rangei.ci = 1;
			auto i = ++this->chars.begin();
			if (MPos.ri == ri && command == "FINDNEXT" || MPos.ri == ri && command == "FINDNEXTALL")
			{
				i = curr;
				rangei.ci = MPos.ci - 1;
			}
			auto currnext = curr;
			if (MPos.ri != ri)
				currnext = this->chars.end();
			else
			{
				for (int i = 0; i < 2 && currnext != this->chars.end(); i++)
					currnext++;
			}
			for (; i != ((command == "FINDNEXT" || command == "FINDNEXTALL") ? this->chars.end() : 
				(MPos.ri == ri)?currnext:this->chars.end());)
			{
				if (tolower(*i) == opr[oi])
				{
					found = true;
					if (first)
					{
						vector<Pos>push;
						range.push_back(push);
						first = false;
					}
					range.back().push_back(rangei);
					oi++;
					if (oi == opr.length())
					{
						if (command == "FINDNEXT")
							return;
						oi = 0;
						first = true;
						found = false;
					}
				}
				else if (found)
				{
					oi = 0;
					found = false;
					first = true;
					range.pop_back();
				}
				rangei.ci += 2;
				for (int j = 0; j < 2 && i != this->chars.end(); j++)
					i++;
			}
		}
		void FindExactWords(string opr, vector<vector<Pos>>& range, int ri)
		{
			if (!this->chars.size())
				return;
			transform(opr.begin(), opr.end(), opr.begin(), tolower);
			bool first = true, found = false;
			int oi = 0;
			Pos rangei;
			rangei.ri = ri; rangei.ci = 1;
			for (auto i = ++this->chars.begin(); i != this->chars.end();)
			{
				auto tempprev = i;
				auto tempnext = i;
				for (int j = 0; j < 2 && tempprev != this->chars.begin(); j++)
					tempprev--;
				for (auto j = 0; j < 2 && tempnext != this->chars.end(); j++)
					tempnext++;
				if (*i == opr[oi] && ((oi == 0) ? *tempprev == ' ' : true) && 
					((oi == opr.length() - 1)?(tempnext == this->chars.end() || *tempnext == ' ' || *tempnext == '\n') : true))
				{
					found = true;
					if (first)
					{
						vector<Pos>push;
						range.push_back(push);
						first = false;
					}
					range.back().push_back(rangei);
					oi++;
					if (oi == opr.length())
					{
						oi = 0;
						first = true;
						found = false;
					}
				}
				else if (found)
				{
					oi = 0;
					found = false;
					first = true;
					range.pop_back();
				}
				rangei.ci += 2;
				for (int j = 0; j < 2 && i != this->chars.end(); j++)
					i++;
			}
		}
		void FindExactWordsFromCursor(string command, string opr, vector<vector<Pos>>& range, int ri,
			const Pos& MPos, const list<char>::iterator& curr)
		{
			if (opr.empty())
			{
				cout << "Invalid or missing word...";
				return;
			}
			if (!this->chars.size())
				return;
			bool first = true, found = false;
			int oi = 0;
			Pos rangei;
			rangei.ri = ri; rangei.ci = 1;
			auto i = ++this->chars.begin();
			if (MPos.ri == ri && command == "REPLACENEXT" || MPos.ri == ri && command == "REPLACENEXTALL")
			{
				i = curr;
				rangei.ci = MPos.ci - 1;
			}
			auto currnext = curr;
			if (MPos.ri != ri)
				currnext = this->chars.end();
			else
			{
				for (int i = 0; i < 2 && currnext != this->chars.end(); i++)
					currnext++;
			}
			for (; i != ((command == "REPLACENEXT" || command == "REPLACENEXTALL" || command == "REPLACEPREVALL")
				? this->chars.end() : currnext);)
			{
				auto tempprev = i;
				auto tempnext = i;
				for (int j = 0; j < 2 && tempprev != this->chars.begin(); j++)
					tempprev--;
				for (auto j = 0; j < 2 && tempnext != this->chars.end(); j++)
					tempnext++;
				if (*i == opr[oi] && ((oi == 0) ? *tempprev == ' ' : true) &&
					((oi == opr.length() - 1) ? (tempnext == this->chars.end() || *tempnext == ' ' || *tempnext == '\n') : true))
				{
					found = true;
					if (first)
					{
						vector<Pos>push;
						range.push_back(push);
						first = false;
					}
					range.back().push_back(rangei);
					oi++;
					if (oi == opr.length())
					{
						if (command == "REPLACENEXT")
							return;
						oi = 0;
						first = true;
						found = false;
					}
				}
				else if (found)
				{
					oi = 0;
					found = false;
					first = true;
					range.pop_back();
				}
				rangei.ci += 2;
				for (int j = 0; j < 2 && i != this->chars.end(); j++)
					i++;
			}
		}
		void REPLACE(string opr, bool& replaced, int ri, list<char>::iterator& curr, Pos& MPos)
		{
			replaced = false;
			string it, with;
			size_t pos = opr.find_first_of(" ");
			if (pos == 0 || pos == string::npos)
			{
				cout << "Missing paramters in command...";
				return;
			}
			it = opr.substr(0, pos);
			with = opr.substr(pos + 1);
			vector<vector<Pos>>range;
			FindExactWords(it, range, ri);
			if (range.empty())
				return;
			replaced = true;
			for (int i = 0; i < range.size(); i++)
			{
				auto itr = this->chars.begin();
				advance(itr, range[i][0].ci);
				int wi = 0;
				for (int j = 0; j < range[i].size(); j++)
				{
					if (wi == with.length())
					{
						bool replace = (ri == MPos.ri && curr != this->chars.end() && curr == itr) ? true : false;
						auto prev = itr;
						auto temp = itr;
						temp--; temp--; prev--;
						Pos pos;
						getRowCol(pos.ri, pos.ci);
						gotoRowCol(ri + 1, this->chars.size() - 3);
						cout << "    ";
						gotoRowCol(pos.ri, pos.ci);
						this->chars.erase(itr);
						this->chars.erase(prev);
						itr = temp;
						for (int j = i + 1; j < range.size(); j++)
						{
							for (int k = 0; k < range[j].size(); k++)
								range[j][k].ci -= 2;
						}
						if (MPos.ri == ri && MPos.ci >= range[i][wi].ci + 1)
							MPos.ci -= 2;
						if (replace)
							curr = temp;
					}
					else
						*itr = with[wi++];
					for (int j = 0; j < 2 && itr != this->chars.end(); j++)
						itr++;
				}
				int limit = range[i][0].ci + (wi * 2) + 1;
				for (; wi < with.length(); wi++)
				{
					this->chars.insert(itr, with[wi]);
					this->chars.insert(itr, ' ');
					for (int j = i + 1; j < range.size(); j++)
					{
						for (int k = 0; k < range[j].size(); k++)
							range[j][k].ci += 2;
					}
					if (MPos.ri == ri && MPos.ci >= limit)
						MPos.ci += 2;
				}
			}
		}
		void REPLACEFROMCURSOR(string command, string opr, vector<vector<Pos>>& range, int ri,
			Pos& MPos, list<char>::iterator& curr)
		{
			string it, with;
			size_t pos = opr.find_first_of(" ");
			if (pos == 0 || pos == string::npos)
			{
				cout << "Missing paramters in command...";
				return;
			}
			it = opr.substr(0, pos);
			with = opr.substr(pos + 1);
			FindExactWordsFromCursor(command, it, range, ri, MPos, curr);
			if (range.empty())
				return;
			if (command == "REPLACEPREV")
			{
				vector<Pos>temp = range.back();
				range.clear();
				range.push_back(temp);
			}
			for (int i = 0; i < range.size(); i++)
			{
				auto itr = this->chars.begin();
				advance(itr, range[i][0].ci);
				int wi = 0;
				for (int j = 0; j < range[i].size(); j++)
				{
					if (wi == with.length())
					{
						bool replace = (ri == MPos.ri && curr != this->chars.end() && curr == itr) ? true : false;
						auto prev = itr;
						auto temp = itr;
						temp--; temp--; prev--;
						Pos pos;
						getRowCol(pos.ri, pos.ci);
						gotoRowCol(ri + 1, this->chars.size() - 3);
						cout << "    ";
						gotoRowCol(pos.ri, pos.ci);
						this->chars.erase(itr);
						this->chars.erase(prev);
						itr = temp;
						for (int j = i + 1; j < range.size(); j++)
						{
							for (int k = 0; k < range[j].size(); k++)
								range[j][k].ci -= 2;
						}
						if (MPos.ri == ri && MPos.ci >= range[i][wi].ci + 1)
							MPos.ci -= 2;
						if (replace)
							curr = temp;
					}
					else
						*itr = with[wi++];
					for (int j = 0; j < 2 && itr != this->chars.end(); j++)
						itr++;
				}
				int limit = range[i][0].ci + (wi * 2) + 1;
				for (; wi < with.length(); wi++)
				{
					this->chars.insert(itr, with[wi]);
					this->chars.insert(itr, ' ');
					for (int j = i + 1; j < range.size(); j++)
					{
						for (int k = 0; k < range[j].size(); k++)
							range[j][k].ci += 2;
					}
					if (MPos.ri == ri && MPos.ci >= limit)
						MPos.ci += 2;
				}
			}
		}
		void ADDPREFIX(string opr, int ri, Pos& MPos)
		{
			string word, prefix;
			size_t pos = opr.find_first_of(" ");
			if (pos == 0 || pos == string::npos)
			{
				cout << "Missing parameters in command...";
				return;
			}
			word = opr.substr(0, pos);
			prefix = opr.substr(pos + 1);
			vector<vector<Pos>>range;
			FindExactWords(word, range, ri);
			if (range.empty())
				return;
			for (int j = 0; j < range.size(); j++)
			{
				auto itr = this->chars.begin();
				advance(itr, range[j][0].ci);
				for (int i = 0; i < prefix.size(); i++)
				{
					if (MPos.ri == ri && MPos.ci >= range[j][0].ci + 1)
						MPos.ci += 2;
					for (int k = j + 1; k < range.size(); k++)
					{
						for (int l = 0; l < range[j].size(); l++)
							range[k][l].ci += 2;
					}
					this->chars.insert(itr, prefix[i]);
					this->chars.insert(itr, ' ');
				}
			}
		}
		void ADDPOSTFIX(string opr, int ri, Pos& MPos)
		{
			string word, prefix;
			size_t pos = opr.find_first_of(" ");
			if (pos == 0 || pos == string::npos)
			{
				cout << "Missing parameters in command...";
				return;
			}
			word = opr.substr(0, pos);
			prefix = opr.substr(pos + 1);
			vector<vector<Pos>>range;
			FindExactWords(word, range, ri);
			if (range.empty())
				return;
			for (int j = 0; j < range.size(); j++)
			{
				auto itr = this->chars.begin();
				advance(itr, range[j][range[j].size() - 1].ci);
				for (int k = 0; k < 2 && itr != this->chars.end() && *itr != '\n'; k++)
					itr++;
				for (int i = 0; i < prefix.size(); i++)
				{
					if (MPos.ri == ri && MPos.ci >= range[j][range[j].size() - 1].ci + 3)
						MPos.ci += 2;
					for (int k = j + 1; k < range.size(); k++)
					{
						for (int l = 0; l < range[j].size(); l++)
							range[k][l].ci += 2;
					}
					this->chars.insert(itr, prefix[i]);
					this->chars.insert(itr, ' ');
				}
			}
		}
		void AVGWORDLENGTH(vector<int>& length)
		{
			if (!this->chars.size())
				return;
			int l = 0;
			for (auto i = ++this->chars.begin(); i != this->chars.end() && *i != '\n'; i++)
			{
				if (*i == ' ')
					continue;
				l++;
				auto tempnext = i;
				for (int j = 0; j < 2 && tempnext != this->chars.end(); j++)
					tempnext++;
				if (tempnext == this->chars.end() || *tempnext == '\n' || *tempnext == ' ')
				{
					length.push_back(l);
					l = 0;
				}
			}
		}
		void COUNT(string opr, int ri, int& count)
		{
			vector<vector<Pos>>range;
			FIND(opr, range, ri);
			count += range.size();
		}
		void SPECIALCOUNT(int &count)
		{
			if (this->chars.empty())
				return;
			for (auto i = ++this->chars.begin(); i != this->chars.end();)
			{
				if (*i != '\n' && *i != ' ' && !isalpha(*i) && !isdigit(*i))
					count++;
				for (int j = 0; j < 2 && i != this->chars.end(); j++)
					i++;
			}
		}
		void SENTENCECOUNT(int &count)
		{
			if (this->chars.size() == 170 || this->chars.size() == 172)
				count++;
		}
		void MERGE(Line* line)
		{
			for (auto i = line->chars.begin(); i != line->chars.end(); i++)
				this->chars.push_back(*i);
		}
		void SAVE(ofstream& out)
		{
			for (auto i = this->chars.begin(); i != this->chars.end(); i++)
				out << char(*i - 10);
		}
		void StringToMap(string opr, multimap<char, char>& mp)
		{
			mp.clear();
			for (int i = 0; i < opr.length(); i++)
				mp.insert(make_pair(opr[i], opr[i]));
		}
		void FINDCOMB(string opr, multimap<char, char>& mp, vector<vector<Pos>>& range, int ri)
		{
			if (!this->chars.size())
				return;
			bool first = true, found = false;
			Pos rangei;
			rangei.ri = ri; rangei.ci = 1;
			for (auto i = ++this->chars.begin(); i != this->chars.end();)
			{
				auto tempprev = i;
				for (int j = 0; j < 2 && tempprev != this->chars.begin(); j++)
					tempprev--;
				auto mpitr = mp.find(*i);
				if (mpitr != mp.end() && ((!found) ? (tempprev == this->chars.begin() || *tempprev == ' ') : true))
				{
					mp.erase(mpitr);
					found = true;
					if (first)
					{
						vector<Pos>push;
						range.push_back(push);
						first = false;
					}
					range.back().push_back(rangei);
					if (mp.empty())
					{
						StringToMap(opr, mp);
						first = true;
						found = false;
					}
				}
				else if (found)
				{
					found = false;
					first = true;
					StringToMap(opr, mp);
					if (*i != ' ' && *i != '\n')
						range.pop_back();
				}
				rangei.ci += 2;
				for (int j = 0; j < 2 && i != this->chars.end(); j++)
					i++;
			}
			StringToMap(opr, mp);
		}
		friend ostream& operator<<(ostream& cout, const Line* line)
		{
			for (auto i = line->chars.begin(); i != line->chars.end(); i++)
			{
				if (*i == '\n')
					continue;
				cout << *i;
			}
			return cout;
		}
	};
	struct Document
	{
		string name;
		string password;
		Pos MPos;
		list<Line*>lines;
		list<Line*>::iterator CurrLineItr;
		list<char>::iterator CurrWordItr;
		Document(string name)
		{
			this->name = name;
			this->MPos.ri = this->MPos.ci = 0;
		}
		void MoveDataNextLine()
		{
			auto it = this->CurrLineItr;
			auto temp = this->CurrWordItr;
			temp = (temp == (*it)->chars.end()) ? temp : ++temp;
			if (this->CurrWordItr == (*it)->chars.end() || *this->CurrWordItr == '\n' && temp == (*it)->chars.end())
				return;
			if ((*it)->chars.back() == '\n')
			{
				(*it)->chars.pop_back();
				(*it)->chars.pop_back();
			}
			stack<char>st;
			while ((*it)->chars.back() != '\n')
			{
				st.push((*it)->chars.back());
				(*it)->chars.pop_back();
			}
			it++;
			while (!st.empty())
			{
				(*it)->chars.push_back(st.top());
				st.pop();
			}
			(*it)->chars.push_back(' ');
			(*it)->chars.push_back('\n');
		}
		void Insert(char input)
		{
			if (isCapacityReached((*(this->CurrLineItr))->chars.size()) && input != '\n')
				return;
			(*(this->CurrLineItr))->Insert(input, this->CurrWordItr, this->MPos.ci);
			this->MPos.ci += 2;
			if (this->CurrWordItr == (*(this->CurrLineItr))->chars.begin())
				this->CurrWordItr++;
			else
			{
				for (int i = 0; i < 2 && this->CurrWordItr != (*(this->CurrLineItr))->chars.end(); i++)
					this->CurrWordItr++;
			}
			if (input == '\n')
			{
				if (input == '\n')
				{
					auto temp = this->CurrLineItr; temp++;
					this->lines.insert(temp, new Line());
					MoveDataNextLine();
					system("cls");
				}
				this->MPos.ri++;
				this->CurrLineItr++;
				this->MPos.ci = 0;
				this->CurrWordItr = (*(this->CurrLineItr))->chars.begin();
			}
		}
		void MoveDataPreviousLine()
		{
			auto it = this->CurrLineItr;
			auto temp = it; temp--;
			(*temp)->chars.pop_back();
			(*temp)->chars.pop_back();
			(*temp)->chars.push_back(' ');
			(*temp)->chars.push_back(' ');
			while (!(*it)->chars.empty())
			{
				(*temp)->chars.push_back((*it)->chars.front());
				(*it)->chars.pop_front();
			}
			if ((*temp)->chars.back() != '\n')
			{
				(*temp)->chars.push_back(' ');
				(*temp)->chars.push_back('\n');
			}
		}
		void Remove()
		{
			this->MPos.ci -= 2;
			if (this->CurrWordItr == (*(this->CurrLineItr))->chars.begin() && this->MPos.ri != 0)
			{
				auto temp = this->CurrLineItr; temp--;
				if (isCapacityReached((*temp)->chars.size()))
				{
					this->MPos.ci = 0;
					return;
				}
				MoveDataPreviousLine();
				temp = this->CurrLineItr; temp--;
				this->lines.erase(this->CurrLineItr);
				this->CurrLineItr = temp;
				this->CurrWordItr = (*(this->CurrLineItr))->chars.begin();
				advance(this->CurrWordItr, (*(this->CurrLineItr))->chars.size() - 1);
				this->MPos.ri--;
				this->MPos.ci = (*(this->CurrLineItr))->chars.size();
				this->MPos.ci -= 2;
				system("cls");
			}
			gotoRowCol(this->MPos.ri, (*(this->CurrLineItr))->chars.size());
			(*(this->CurrLineItr))->Remove(this->CurrWordItr);
			if (this->MPos.ci < 0)
				this->MPos.ci = 0;
		}
		void Read(const string& str)
		{
			Line* line = new Line();
			this->lines.push_back(line);
			line->Read(str);
			this->MPos.ci = 0;
			this->MPos.ri++;
		}
		void MoveCursorUp()
		{
			if (this->MPos.ri != 0)
			{
				this->MPos.ri--;
				this->CurrLineItr--;
				this->MPos.ci = 0;
				this->CurrWordItr = (*(this->CurrLineItr))->chars.begin();
				gotoRowCol(this->MPos.ri, this->MPos.ci);
			}
		}
		void MoveCursorDown()
		{
			if (this->MPos.ri != this->lines.size() - 1)
			{
				this->MPos.ri++;
				if ((*(this->CurrLineItr))->chars.size() == 0 || (*(this->CurrLineItr))->chars.back() != '\n')
				{
					(*(this->CurrLineItr))->chars.push_back(' ');
					(*(this->CurrLineItr))->chars.push_back('\n');
				}
				this->CurrLineItr++;
				this->MPos.ci = 0;
				this->CurrWordItr = (*(this->CurrLineItr))->chars.begin();
				gotoRowCol(this->MPos.ri, this->MPos.ci);
			}
		}
		void MoveCursorRight()
		{
			if (!isCursorAtEnd(this->MPos.ci) && this->MPos.ci != (*(this->CurrLineItr))->chars.size())
			{
				this->MPos.ci += 2;
				if (this->CurrWordItr == (*(this->CurrLineItr))->chars.begin())
					this->CurrWordItr++;
				else
				{
					for (int i = 0; i < 2 && this->CurrWordItr != (*(this->CurrLineItr))->chars.end(); i++)
						this->CurrWordItr++;
				}
				gotoRowCol(this->MPos.ri, this->MPos.ci);
			}
		}
		void MoveCursorLeft()
		{
			if (this->MPos.ci != 0)
			{
				this->MPos.ci -= 2;
				for (int i = 0; i < 2 && this->CurrWordItr != (*(this->CurrLineItr))->chars.begin(); i++)
					this->CurrWordItr--;
				gotoRowCol(this->MPos.ri, this->MPos.ci);
			}
		}
		void TOUPPER(string command)
		{
			auto temp = this->CurrWordItr;
			auto it = this->CurrLineItr;
			if (temp == (*it)->chars.end() || temp == (*it)->chars.begin()
				|| *temp == '\n')
			{
				cout << "Word not found...";
				return;
			}
			if (command == "TOUPPER")
				*temp = toupper(*temp);
			else
			{
				while (temp != (*it)->chars.begin() && *temp != ' ')
				{
					*temp = toupper(*temp);
					for (int i = 0; i < 2 && temp != (*it)->chars.begin(); i++)
						temp--;
				}
			}
		}
		void TOLOWER(string command)
		{
			auto temp = this->CurrWordItr;
			auto it = this->CurrLineItr;
			if (temp == (*it)->chars.end() || temp == (*it)->chars.begin()
				|| *temp == '\n')
			{
				cout << "Word not found...";
				return;
			}
			if (command == "TOLOWER")
				*temp = tolower(*temp);
			else
			{
				while (temp != (*it)->chars.begin() && *temp != ' ')
				{
					*temp = tolower(*temp);
					for (int i = 0; i < 2 && temp != (*it)->chars.begin(); i++)
						temp--;
				}
			}
		}
		void FIND(string opr, vector<vector<Pos>>& range)
		{
			int ri = 0;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
			{
				(*i)->FIND(opr, range, ri);
				ri++;
			}
		}
		void FINDFROMCURSOR(string command, string opr, vector<vector<Pos>>& range)
		{
			int ri = ((command == "FINDNEXT" || command == "FINDNEXTALL") ? MPos.ri : 0);
			auto i = ((command == "FINDNEXT" || command == "FINDNEXTALL") ? this->CurrLineItr : this->lines.begin());
			auto currnext = this->CurrLineItr; currnext++;
			for (; i != ((command == "FINDNEXT" || command == "FINDNEXTALL") ? this->lines.end() : currnext); i++)
			{
				(*i)->FINDFROMCURSOR(command, opr, range, ri, this->MPos, this->CurrWordItr);
				if (command == "FINDNEXT" && range.size() == 1)
					return;
				ri++;
			}
			if (command == "FINDPREV" && range.size())
			{
				vector<Pos>temp = range.back();
				range.clear();
				range.push_back(temp);
			}
		}
		void REPLACE(string command, string opr)
		{
			int ri = 0;
			bool replaced;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
			{
				replaced = false;
				(*i)->REPLACE(opr, replaced, ri, this->CurrWordItr, this->MPos);
				ri++;
				if (replaced && command == "REPLACE")
					return;
			}
		}
		void REPLACEFROMCURSOR(string command, string opr, vector<vector<Pos>>& range)
		{
			bool first = true;
			int ri = ((command == "REPLACENEXT" || command == "REPLACENEXTALL" || command == "REPLACEPREV") ? MPos.ri : 0);
			auto i = ((command == "REPLACENEXT" || command == "REPLACENEXTALL" || command == "REPLACEPREV")
				? this->CurrLineItr : this->lines.begin());
			auto currnext = this->CurrLineItr; currnext++;
			for (; first || i != ((command == "REPLACENEXT" || command == "REPLACENEXTALL") ? this->lines.end() :
				((command == "REPLACEPREVALL") ? currnext : this->lines.end()));
				((command == "REPLACEPREV") ? i-- : i++))
			{
				(*i)->REPLACEFROMCURSOR(command, opr, range, ri, this->MPos, this->CurrWordItr);
				if (command == "REPLACENEXT" && range.size() == 1 || command == "REPLACEPREV" && range.size() == 1)
				{
					range.clear();
					return;
				}
				range.clear();
				(command == "REPLACEPREV") ? ri-- : ri++;
				first = ((command != "REPLACEPREV") ? false : ((i == this->lines.begin()) ? false : true));
			}
		}
		void ADDPREFIX(string opr)
		{
			int ri = 0;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
			{
				(*i)->ADDPREFIX(opr, ri, this->MPos);
				ri++;
			}
		}
		void ADDPOSTFIX(string opr)
		{
			int ri = 0;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
			{
				(*i)->ADDPOSTFIX(opr, ri, this->MPos);
				ri++;
			}
		}
		void AVGWORDLENGTH()
		{
			vector<int>length;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
				(*i)->AVGWORDLENGTH(length);
			if (length.empty())
				return;
			int sum = 0;
			for (int i = 0; i < length.size(); i++)
				sum += length[i];
			cout << (double(sum) / double(length.size()));
		}
		void COUNT(string opr)
		{
			int count = 0, ri = 0;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
			{
				(*i)->COUNT(opr, ri, count);
				ri++;
			}
			cout << count;
		}
		void SPECIALCOUNT()
		{
			int count = 0;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
				(*i)->SPECIALCOUNT(count);
			cout << count;
		}
		void SENTENCECOUNT()
		{
			int count = 0;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
				(*i)->SENTENCECOUNT(count);
			cout << count;
		}
		void PARACOUNT()
		{
			int count = 0;
			if (this->lines.size() == 2)
			{
				cout << 0;
				return;
			}
			for (auto i = ++++this->lines.begin(); i != this->lines.end(); i++)
			{
				auto tempprev = i;
				count++;
				for (int j = 0; j < 2; j++)
				{
					tempprev--;
					if ((*tempprev)->chars.size() > 2 || (*tempprev)->chars.size() == 2
						&& (*tempprev)->chars.back() != '\n')
					{
						count--;
						break;
					}
				}
			}
			cout << count;
		}
		void MAXWORDLENGTH()
		{
			vector<int>length;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
				(*i)->AVGWORDLENGTH(length);
			if (length.empty())
				cout << 0;
			int max = length[0];
			for (int i = 1; i < length.size(); i++)
			{
				if (length[i] > max)
					max = length[i];
			}
			cout << max;
		}
		void MINWORDLENGTH()
		{
			vector<int>length;
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
				(*i)->AVGWORDLENGTH(length);
			if (length.empty())
				cout << 0;
			int min = length[0];
			for (int i = 1; i < length.size(); i++)
			{
				if (length[i] < min)
					min = length[i];
			}
			cout << min;
		}
		void MERGE(Document* doc)
		{
			for (auto i = doc->lines.begin(); i != doc->lines.end(); i++)
			{
				this->lines.push_back(new Line());
				this->lines.back()->MERGE(*i);
			}
		}
		void LARGESTPARAWORDLENGTH()
		{
			int max = 0;
			vector<int>length;
			if (this->lines.size() == 0)
			{
				cout << "No paragraph exist...";
				return;
			}
			bool found = false;
			for (auto i = ++++this->lines.begin(); i != this->lines.end(); i++)
			{
				auto tempprev = i;
				int count = 0;
				for (int j = 0; j < 2; j++)
				{
					tempprev--;
					if ((*tempprev)->chars.size() > 2 || (*tempprev)->chars.size() == 2
						&& (*tempprev)->chars.back() != '\n')
						break;
					count++;
				}
				if (count == 2 && !found)
					found = true;
				else if (count == 2 && found)
				{
					if (length.size() > max)
						max = length.size();
					length.clear();
				}
				if (found)
					(*i)->AVGWORDLENGTH(length);
			}
			cout << ((max > length.size()) ? max : length.size());
		}
		void SETPASSWORD(string opr)
		{
			if (opr.empty())
			{
				cout << "Invalid or missing password...";
				return;
			}
			this->password = opr;
		}
		void REMOVEPASSWORD()
		{
			this->password = "";
		}
		void SAVE(ofstream& out)
		{
			int count = 0;
			for (int i = 0; i < this->password.size(); i++)
				out << char(this->password[i] - 10);
			out << '\n';
			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
			{
				(*i)->SAVE(out);
				if (count != this->lines.size() - 1)
					out << '\n';
				count++;
			}
		}
		static bool CompareVectorsByRi(const vector<Pos>& a, const vector<Pos>& b)
		{
			return a[0].ri < b[0].ri;
		}
		void FINDCOMB(string opr, vector<vector<Pos>>& range)
		{
			if (opr.empty())
			{
				"Missing parameters in command...";
				return;
			}
			multimap<char, char>mp;
			for (int i = 0; i < opr.length(); i++)
				mp.insert(make_pair(opr[i], opr[i]));
			int ri = 0;

			for (auto i = this->lines.begin(); i != this->lines.end(); i++)
			{
				(*i)->FINDCOMB(opr, mp, range, ri);
				ri++;
			}
			sort(range.begin(), range.end(), CompareVectorsByRi);
		}
		friend ostream& operator<<(ostream& cout, const Document* doc)
		{
			for (auto i = doc->lines.begin(); i != doc->lines.end(); i++)
			{
				cout << *i;
				cout << endl;
			}
			return cout;
		}
	};
	unordered_map<string, Document*>docs;
	Document* CurrDoc;
public:
	Editor()
	{
		this->CurrDoc = nullptr;
	}
	void MERGE(string command, string opr)
	{
		if (opr.empty())
		{
			cout << "Missing parameters in command...";
			return;
		}
		string first, second;
		size_t pos = opr.find_first_of(" ");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Missing parameters in command...";
			return;
		}
		first = opr.substr(0, pos);
		second = opr.substr(pos);
		Document* FirstFile, * SecondFile;
		OpenFile(first);
		FirstFile = this->CurrDoc;
		OpenFile(second);
		SecondFile = this->CurrDoc;
		this->CurrDoc = FirstFile;
		if (command == "MERGE")
			FirstFile->MERGE(SecondFile);
		else
		{
			string name = "newmergedfile.txt";
			Document* nw = new Document(name);
			nw->MERGE(FirstFile);
			nw->MERGE(SecondFile);
			this->docs[name] = nw;
			this->CurrDoc = nw;
		}
	}
	bool OpenFile(string fname)
	{
		ifstream in(fname);
		if (docs.find(fname) == docs.end())
		{
			this->CurrDoc = new Document(fname);
			if (in.is_open())
			{
				string input;
				getline(in, input);
				for (int i = 0; i < input.size(); i++)
					input[i] = char(input[i] + 10);
				this->CurrDoc->password = input;
				string p;
				while (p != this->CurrDoc->password)
				{
					cout << "Enter password\\To exit enter exit: ";
					cin >> p;
					if (p == "exit")
					{
						system("cls");
						delete this->CurrDoc;
						return true;
					}
				}
				while (getline(in, input))
					this->CurrDoc->Read(input);
			}
			this->docs[fname] = this->CurrDoc;
			this->CurrDoc->lines.push_back(new Line());
			auto it = this->CurrDoc->lines.begin();
			advance(it, this->CurrDoc->lines.size() - 1);
			this->CurrDoc->CurrLineItr = it;
			this->CurrDoc->CurrWordItr = (*it)->chars.begin();
		}
		else
			this->CurrDoc = this->docs[fname];
		return false;
	}
	void SaveFile()
	{
		ofstream out(this->CurrDoc->name);
		this->CurrDoc->SAVE(out);
		cout << "Succesfully saved...";
	}
	void EditingMode()
	{
		system("cls");
		cout << this->CurrDoc;
		gotoRowCol(this->CurrDoc->MPos.ri, this->CurrDoc->MPos.ci);
		while (true)
		{
			char input;
			if (_kbhit)
			{
				input = _getch();
				switch (input)
				{
				case '\r':
					this->CurrDoc->Insert('\n');
					gotoRowCol(0, 0);
					cout << this->CurrDoc;
					break;
				case '\b':
					this->CurrDoc->Remove();
					cout << input << input << ' ' << ' ' << input << input;
					gotoRowCol(0, 0);
					cout << this->CurrDoc;
					break;
				case -32:
					input = _getch();
					switch (input)
					{
					case 'H':
						this->CurrDoc->MoveCursorUp();
						break;
					case 'P':
						this->CurrDoc->MoveCursorDown();
						break;
					case 'M':
						this->CurrDoc->MoveCursorRight();
						break;
					case 'K':
						this->CurrDoc->MoveCursorLeft();
						break;
					case 'G':
						ProcessingMode();
						return;
					}
					break;
				default:
					this->CurrDoc->Insert(input);
					gotoRowCol(0, 0);
					cout << this->CurrDoc;
					break;
				}
				gotoRowCol(this->CurrDoc->MPos.ri, this->CurrDoc->MPos.ci);
			}
		}
	}
	bool DoOperation(string input, vector<vector<Pos>>& range)
	{
		string command = input, opr;
		size_t pos = input.find_first_of(" ");
		if (pos != 0 || pos != string::npos)
		{
			opr = input.substr(pos + 1);
			command = input.substr(0, pos);
		}
		transform(command.begin(), command.end(), command.begin(), toupper);
		if (command == "TOUPPER" || command == "TOUPPERALL")
			this->CurrDoc->TOUPPER(command);
		else if (command == "TOLOWER" || command == "TOLOWERALL")
			this->CurrDoc->TOLOWER(command);
		else if (command == "FIND" || command == "FINDF")
			this->CurrDoc->FIND(opr, range);
		else if (command == "FINDNEXT" || command == "FINDNEXTALL" || command == "FINDPREV" || command == "FINDPREVALL")
			this->CurrDoc->FINDFROMCURSOR(command, opr, range);
		else if (command == "REPLACE" || command == "REPLACEALL")
			this->CurrDoc->REPLACE(command, opr);
		else if (command == "FINDCOMB")
			this->CurrDoc->FINDCOMB(opr, range);
		else if (command == "REPLACENEXT" || command == "REPLACENEXTALL" || command == "REPLACEPREV" ||
			command == "REPLACEPREVALL")
			this->CurrDoc->REPLACEFROMCURSOR(command, opr, range);
		else if (command == "ADDPREFIX")
			this->CurrDoc->ADDPREFIX(opr);
		else if (command == "ADDPOSTFIX")
			this->CurrDoc->ADDPOSTFIX(opr);
		else if (command == "AVGWORDLENGTH")
			this->CurrDoc->AVGWORDLENGTH();
		else if (command == "COUNT")
			this->CurrDoc->COUNT(opr);
		else if (command == "SPECIALCOUNT")
			this->CurrDoc->SPECIALCOUNT();
		else if (command == "MAXWORDLENGTH")
			this->CurrDoc->MAXWORDLENGTH();
		else if (command == "MERGE" || command == "MERGENEW")
			MERGE(command, opr);
		else if (command == "PARACOUNT")
			this->CurrDoc->PARACOUNT();
		else if (command == "LARGESTPARAWORDLENGTH")
			this->CurrDoc->LARGESTPARAWORDLENGTH();
		else if (command == "SETPASSWORD")
			this->CurrDoc->SETPASSWORD(opr);
		else if (command == "REMOVEPASSWORD")
			this->CurrDoc->REMOVEPASSWORD();
		else if (command == "SAVE")
			SaveFile();
		else if (command == "EXIT")
			return true;
		else
			cout << "Invalid command...";
		if (command == "SETPASSWORD" || command == "REMOVEPASSWORD")
			SaveFile();
		return false;
	}
	void ColoredPrint(vector<vector<Pos>>& range)
	{
		Pos pos;
		pos.ri--; pos.ci--;
		Pos index;
		for (auto i = this->CurrDoc->lines.begin(); i != this->CurrDoc->lines.end(); i++)
		{
			pos.ri++;
			pos.ci = -1;
			for (auto j = (*i)->chars.begin(); j != (*i)->chars.end(); j++)
			{
				pos.ci++;
				if (*j == '\n')
					continue;
				if (!range.empty() && index.ci == range[index.ri].size())
				{
					index.ci = 0;
					index.ri = (index.ri == range.size() - 1) ? index.ri : index.ri + 1;
				}
				if (pos.ri == this->CurrDoc->MPos.ri && pos.ci == this->CurrDoc->MPos.ci - 1)
				{
					if (!range.empty() && pos.ri == range[index.ri][index.ci].ri && pos.ci == range[index.ri][index.ci].ci)
						index.ci++;
					SetClr(FG_CYAN);
				}
				else if (!range.empty() && pos.ri == range[index.ri][index.ci].ri && pos.ci == range[index.ri][index.ci].ci)
				{
					index.ci++;
					SetClr(FG_GREEN);
				}
				else
					SetClr(FG_WHITE);
				cout << *j;
			}
			cout << endl;
		}
		SetClr(FG_WHITE);
	}
	void ProcessingMode()
	{
		system("cls");
		vector<vector<Pos>>range;
		cout << "error\\output: \n"; ColoredPrint(range); cout << "command: ";
		char input;
		string command, opr;
		Pos curr;
		bool exit;
		while (true)
		{
			if (_kbhit())
			{
				input = _getch();
				switch (input)
				{
				case -32:
					input = _getch();
					switch (input)
					{
					case 'R':
						EditingMode();
						return;
					}
					break;
				case '\r':
					for (int i = 0; i < command.length(); i++)
						cout << "\b" << " " << "\b";
					gotoRowCol(0, 0);
					cout << "error\\output: ";
					getRowCol(curr.ri, curr.ci);
					cout << "                                                                                           ";
					gotoRowCol(curr.ri, curr.ci);
					exit = DoOperation(command, range);
					if (exit)
						return;
					cout << endl; ColoredPrint(range); cout << "command: ";
					range.clear();
					command = "";
					break;
				case'\b':
					for (int i = 0; i < command.length(); i++)
						cout << "\b \b";
					if(!command.empty())
						command.pop_back();
					break;
				default:
					for (int i = 0; i < command.length(); i++)
						cout << "\b \b";
					command += input;
					break;
				}
				cout << command;
			}
		}
	}
	void Run(string fname)
	{
		bool exit = OpenFile(fname);
		if (exit)
			return;
		ProcessingMode();
	}
};