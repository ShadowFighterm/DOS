#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<queue>
#include<ctime>
#include<map>
#include<unordered_map>
#include<algorithm>
#include"Editor.h"
using namespace std;
string DisplayRootPath;
class DOS
{
private:
	struct User
	{
		string AuthorName;
		string RegNum;
		string CourseName;
		User(string AuthorName, string RegNum, string CourseName)
		{
			this->AuthorName = AuthorName;
			this->RegNum = RegNum;
			this->CourseName = CourseName;
		}
		friend ostream& operator <<(ostream& cout, const User* user)
		{
			cout << user->AuthorName << "\t" << user->RegNum << endl << user->CourseName << endl;
			return cout;
		}
	};
	struct File
	{
		int size;
		string name;
		string extension;
		string date;
		string time;
		long long NumOfChar;
		long long NumOfLines;
		double AvgNumOfCharPerLine;
		File(string name = {}, string extension = {})
		{
			this->name = name;
			this->extension = extension;
			this->date = "0/0/0";
			this->time = "00:00:00";
			this->size = this->NumOfChar = this->NumOfLines = this->AvgNumOfCharPerLine = 0;
		}
		File(const File* f)
		{
			this->size = f->size;
			this->name = f->name;
			this->extension = f->extension;
			this->date = f->date;
			this->time = f->time;
			this->NumOfChar = f->NumOfChar;
			this->NumOfLines = f->NumOfLines;
			this->AvgNumOfCharPerLine = f->AvgNumOfCharPerLine;

		}
		static void Display(const File* file)
		{
			cout << file->date << "\t" << file->time << "\t\t" << file->size << "\t\t\t" << file->name + "." + file->extension << endl;
		}
		friend ostream& operator<<(ostream& cout, const File* file)
		{
			cout << "File size: " << file->size << "\nDate of creation: " << file->date << "\nTime of creation: " << file->time
				<< "\nNumber of chars: " << file->NumOfChar << "\nNumber of lines: " << file->NumOfLines
				<< "\nAverage char per line : " << file->AvgNumOfCharPerLine << endl;
			return cout;
		}
	};
	struct Directory
	{
		int depth;
		string name;
		string path;
		string date;
		string time;
		Directory* parent;
		map<pair<string, string>, File*>files;
		map<string, Directory*>directories;
		Directory(string path = "V\\:>", string name = "V", int depth = 0, Directory* parent = nullptr)
		{
			this->path = path;
			this->name = name;
			this->parent = parent;
			this->depth = depth;
			this->date = "0/0/0";
			this->time = "00:00:00";
		}
		void Format()
		{
			this->files.clear();
			this->directories.clear();
		}
		static void Display(const Directory* dir)
		{
			cout << dir->date << "\t" << dir->time << "\t\t<DIR>" << "\t\t\t" << dir->name << endl;
		}
		static void DisplayFile(const File* file)
		{
			File::Display(file);
		}
		friend ostream& operator<<(ostream& cout, const Directory* dir)
		{
			for (auto i = dir->directories.begin(); i != dir->directories.end(); i++)
			{
				Display(i->second);
			}
			for (auto i = dir->files.begin(); i != dir->files.end(); i++)
			{
				DisplayFile(i->second);
			}
			return cout;
		}
	};
	struct PrintItem
	{
		time_t time;
		string FileName;
		PrintItem(string FileName, time_t time)
		{
			this->FileName = FileName;
			this->time = time;
		}
		bool operator<(const PrintItem& p)
		{
			return this->time < p.time;
		}
	};
	User* user;
	string RootPath;
	Directory* CurrDir;
	map<string, Directory*> directories;
	Editor* editor;
	list<PrintItem*>PriorityPrint;
	list<PrintItem*>print;
public:
	DOS()
	{
		this->editor = new Editor();
		this->RootPath = "V\\:>";
		DisplayRootPath = "V";
		this->directories[this->RootPath] = new Directory();
		this->CurrDir = this->directories[this->RootPath];
		this->user = new User("Muhammad Mannan", "Bscs22083", "DSA (Data Structure and Algorithm");
	}
	Directory* FindFileInAllDir(string name, string extension)
	{
		for (auto i = this->directories.begin(); i != this->directories.end(); i++)
		{
			if (i->second->files.find(make_pair(name, extension)) != i->second->files.end())
				return i->second;
		}
		return nullptr;
	}
	void LOADTREE()
	{
		ifstream in("tree.txt");
		int NoOfDir;
		in >> NoOfDir;
		for (int i = 0; i < NoOfDir; i++)
		{
			string name, path, ParentPath, time, date;
			int NoOfFile;
			in >> name >> path >> ParentPath >> time >> date >> NoOfFile;
			Directory* parent = this->directories[ParentPath];
			Directory* nw = new Directory(path, name, parent->depth + 1, parent);
			this->directories[path] = nw;
			parent->directories[name] = nw;
			for (int j = 0; j < NoOfFile; j++)
			{
				string extension;
				int size;
				long long NumOfChar, NumOfLines;
				double AvgNumOfLines;
				in >> name >> extension >> size >> time >> date >> NumOfChar >> NumOfLines >> AvgNumOfLines;
				nw->files[make_pair(name, extension)] = new File(name, extension);
			}
		}
	}
	void SAVE()
	{
		ofstream out("tree.txt");
		out << this->directories.size() - 1 << "\n";
		for (auto i = this->directories.begin(); i != this->directories.end(); i++)
		{
			if (i->second->path == this->RootPath)
				continue;
			out << i->second->name << " " << i->second->path << " " << i->second->parent->path << " " << i->second->time << " "
				<< i->second->date << "\n" << i->second->files.size() << "\n";
			for (auto j = i->second->files.begin(); j != i->second->files.end(); j++)
			{
				out << j->second->name << " " << j->second->extension << " " << j->second->size << " " << j->second->time << " "
					<< j->second->date << " " << j->second->NumOfChar << " " << j->second->NumOfLines << " "
					<< j->second->AvgNumOfCharPerLine << "\n";
			}
		}
	}
	void EXIT()
	{
		SAVE();
		exit(0);
	}
	void CDChange()
	{
		if (cin.peek() == '\n')
		{
			size_t pos = this->CurrDir->path.find_last_of(">");
			cout << this->CurrDir->path.substr(0, pos) << "\n\n";
		}
		else
		{
			string name;
			cin >> name;
			if (this->CurrDir->directories.find(name) == this->CurrDir->directories.end())
			{
				cout << "Directory does not exist in the current directory...\n\n";
				return;
			}
			this->CurrDir = this->CurrDir->directories[name];
		}
	}
	void CDDot()
	{
		cout << endl;
	}
	void CDReset()
	{
		this->CurrDir = this->directories[this->RootPath];
	}
	void CDPrev()
	{
		if (this->CurrDir->path != this->RootPath)
			this->CurrDir = this->CurrDir->parent;
	}
	void CREATE()
	{
		string file;
		if (this->CurrDir->path == this->RootPath)
		{
			cout << "File cannot be created in root directory...\n";
			return;
		}
		if (cin.peek() == '\n')
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		cin >> file;
		size_t pos = file.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string name = file.substr(0, pos), extension = file.substr(pos + 1);
		if (extension != "txt" && extension != "text")
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string decision;
		if (this->CurrDir->files.find(make_pair(name, extension)) != this->CurrDir->files.end())
		{
			cout << "Do you want to replace the file? (Y(yes)/N(No))\n";
			cin >> decision;
			transform(decision.begin(), decision.end(), decision.begin(), toupper);
			if (decision == "N")
				name += "-Copy";
			else if(decision != "Y")
				cout << "Operation was cancelled!!\n";
		}
		this->CurrDir->files[make_pair(name, extension)] = new File(name, extension);
	}
	void DEL()
	{
		string file;
		if (cin.peek() == '\n')
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		cin >> file;
		size_t pos = file.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string name = file.substr(0, pos), extension = file.substr(pos + 1);
		size_t erased = this->CurrDir->files.erase(make_pair(name, extension));
		if (erased == 0)
			cout << "File does not exist in the current directory...\n";
	}
	void DIR()
	{
		cout << this->CurrDir;
	}
	void RENAME()
	{
		string curr, nw;
		if (cin.peek() == '\n')
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		cin >> curr;
		if (cin.peek() == '\n')
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		cin >> nw;
		size_t pos = curr.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string CurrName = curr.substr(0, pos), CurrExtension = curr.substr(pos + 1);
		pos = nw.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string NewName = nw.substr(0, pos), NewExtension = nw.substr(pos + 1);
		if (NewExtension != CurrExtension)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		if (this->CurrDir->files.find(make_pair(CurrName, CurrExtension)) == this->CurrDir->files.end())
		{
			cout << "File does not exist in the current directory...\n";
			return;
		}
		this->CurrDir->files[make_pair(NewName, NewExtension)] = this->CurrDir->files[make_pair(CurrName, CurrExtension)];
		this->CurrDir->files[make_pair(NewName, NewExtension)]->name = NewName;
		this->CurrDir->files.erase(make_pair(CurrName, CurrExtension));
	}
	void FORMAT()
	{
		this->CurrDir->Format();
	}
	void HELP()
	{
		string command;
		if (cin.peek() == '\n')
		{
			cout << "ATTRIB: Displays file attributes.\n" << "CD: Displays the name of or changes the current directory.\n"
				<< "CONVERT: Asks two types and converts extension of all files of one type to another type.\n" <<
				"COPY: Copies one file in the current directory to another location (directory).\n" <<
				"CREATE: Creates a file with the name provided and allows the user to enter contents of the file.\n" <<
				"DEL: Delete a file whose name is provided in input.\n" << "DIR: Displays a list of files and subdirectories in a directory.\n"
				<< "EDIT: Opens a file and allows the user to edit and save the contents of the file.\n" << "EXIT: Quits the program\n"
				<< "FIND: Searches for a file in the current virtual directory whose name is provided as input.\n" <<
				"FINDF: Searches for a text string in the currently open file or the file whose name is provided as input.\n"
				<< "FINDSTR: Searches for strings in all files of the current virtual directory, prints names of files with the string.\n"
				<< "FORMAT: Formats the current virtual directory i.e. empties the current directory and all subdirectories.\n" <<
				"LOADTREE: Load a given tree in your tree structure. Given tree is in a file named tree.txt.\n" << "MKDIR: Creates a virtual directory.\n"
				<< "MOVE: Moves one file (whose name is provided as input) from one directory to another directory.\n" <<
				"PPRINT: Adds a text file to the priority based print queue, and displays the current priority queue.\n" <<
				"PROMPT: Changes the Windows command prompt (for example from V:\> to V$).\n" <<
				"PRINT: Adds a text file to the print queue, and displays the current queue.\n" <<
				"PQUEUE: Shows current state of the priority based print queue, with time left for each element.\n" <<
				"PWD: Prints working directory.\n" << "QUEUE: Shows current state of the print queue, with time left for each element.\n"
				<< "RENAME: Renames a file whose current and new name is provided as input.\n" <<
				"RMDIR: Removes a directory along with its contents.\n" << "SAVE: Saves the currently open file to disk.\n"
				<< "TREE: Displays the complete directory structure.\n" << "VER: Displays the version of the program.\n";
			return;
		}
		cin >> command;
		transform(command.begin(), command.end(), command.begin(), toupper);
		if (command == "ATTRIB")
			cout << "Displays file attributes. Syntax : ATTRIB FileName.extension\n";
		else if (command == "CD")
			cout << "Displays the name of or changes the current directory. Syntax : CD\\NewDir or CD\n";
		else if (command == "CD.")
			cout << "Prints working directory. Syntax : CD.\n";
		else if (command == "CD..")
			cout << "Change directory to previous directory. Syntax : CD..\n";
		else if (command == "CD\\")
			cout << "Changes directory to root directory (i.e. V:\\>). Syntax : CD\\\n";
		else if (command == "CONVERT")
			cout << "Asks two types and converts extension of all files of one type to another type. Syntax : CONVERT extension1 extension 2\n";
		else
			cout << "Invalid or missing command...\n";
	}
	void MKDIR()
	{
		string name, path;
		if (cin.peek() == '\n')
		{
			cout << "Invalid or missing file name...\n";
			return;
		}
		cin >> name;
		string decision;
		if (this->CurrDir->directories.find(name) != this->CurrDir->directories.end())
		{
			cout << "Do you want to replace the file? (Y(yes)/N(No))\n";
			cin >> decision;
			transform(decision.begin(), decision.end(), decision.begin(), toupper);
			if (decision != "N")
				name += "-Copy";
		}
		if (this->CurrDir->path != this->RootPath)
		{
			size_t pos;
			pos = this->CurrDir->path.find_last_of(">");
			path = this->CurrDir->path.substr(0, pos) + "\\" + name + ">";
		}
		else
			path = this->CurrDir->name + ":\\" + name + ">";
		Directory* nw = new Directory(path, name, this->CurrDir->depth + 1, this->CurrDir);
		this->directories[path] = nw;
		this->CurrDir->directories[name] = nw;
	}
	void CONVERT()
	{
		string ExtChange, ExtTo, error = "Invalid or missing extension...\n";
		if (cin.peek() == '\n')
		{
			cout << error;
			return;
		}
		cin >> ExtChange;
		if (cin.peek() == '\n' || ExtChange != "text" && ExtChange != "txt")
		{
			cout << error;
			return;
		}
		cin >> ExtTo;
		if (ExtTo != "txt" && ExtTo != "text")
		{
			cout << error;
			return;
		}
		for (auto i = this->CurrDir->files.begin(); i != this->CurrDir->files.end();)
		{
			if (i->second->extension != ExtChange)
			{
				++i;
				continue;
			}

			File* f = i->second;
			auto eraseIterator = i++;

			this->CurrDir->files.erase(eraseIterator);

			if (this->CurrDir->files.find(make_pair(f->name, ExtTo)) != this->CurrDir->files.end())
			{
				string decision;
				cout << "Do you want to replace the file? (Y(yes)/N(No))\n";
				cin >> decision;
				transform(decision.begin(), decision.end(), decision.begin(), toupper);
				if (decision != "Y")
					f->name += "-Copy";
			}

			f->extension = ExtTo;
			this->CurrDir->files[make_pair(f->name, f->extension)] = f;
		}

	}
	void COPY()
	{
		string file, dir, error = "Invalid or missing filename or directory...\n";
		if (cin.peek() == '\n')
		{
			cout << error;
			return;
		}
		cin >> file;
		if (cin.peek() == '\n')
		{
			cout << error;
			return;
		}
		cin >> dir;
		size_t pos = file.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string name = file.substr(0, pos), extension = file.substr(pos + 1);
		if (extension != "txt" && extension != "text")
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		if (this->CurrDir->files.find(make_pair(name, extension)) == this->CurrDir->files.end())
		{
			cout << "File does not exist in the current directory...\n";
			return;
		}
		File* f = this->CurrDir->files[make_pair(name, extension)];
		bool found = false;
		for (auto i = this->directories.begin(); i != this->directories.end(); i++)
		{
			if (i->second->name == dir)
			{
				found = true;
				if (i->second->files.find(make_pair(name, extension)) != i->second->files.end())
				{
					string decision;
					cout << "Do you want to replace the file? (Y(yes)/N(No))\n";
					cin >> decision;
					transform(decision.begin(), decision.end(), decision.begin(), toupper);
					if (decision != "Y")
						f->name += "-Copy";
				}
				i->second->files[make_pair(f->name, f->extension)] = new File(f);
				break;
			}
		}
		if (!found)
			cout << "Directory does not exist...\n";
	}
	void MOVE()
	{
		string file, dir, error = "Invalid or missing filename or directory...\n";
		if (cin.peek() == '\n')
		{
			cout << error;
			return;
		}
		cin >> file;
		if (cin.peek() == '\n')
		{
			cout << error;
			return;
		}
		cin >> dir;
		size_t pos = file.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string name = file.substr(0, pos), extension = file.substr(pos + 1);
		if (extension != "txt" && extension != "text")
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		if (this->CurrDir->files.find(make_pair(name, extension)) == this->CurrDir->files.end())
		{
			cout << "File does not exist in the current directory...\n";
			return;
		}
		File* f = this->CurrDir->files[make_pair(name, extension)];
		bool found = false;
		for (auto i = this->directories.begin(); i != this->directories.end(); i++)
		{
			if (i->second->name == dir)
			{
				found = true;
				if (i->second->files.find(make_pair(name, extension)) != i->second->files.end())
				{
					string decision;
					cout << "Do you want to replace the file? (Y(yes)/N(No))\n";
					cin >> decision;
					transform(decision.begin(), decision.end(), decision.begin(), toupper);
					if (decision != "Y")
						f->name += "-Copy";
				}
				i->second->files[make_pair(f->name, f->extension)] = f;
				break;
			}
		}
		if (!found)
		{
			cout << "Directory does not exist...\n";
			return;
		}
		this->CurrDir->files.erase(make_pair(name, extension));
	}
	void RMDIR()
	{
		string dir;
		if (cin.peek() == '\n')
		{
			cout << "Invalid or missing directory...\n";
			return;
		}
		cin >> dir;
		if (dir == "V")
		{
			cout << "Root directory cannot be deleted...\n";
			return;
		}
		bool found = false;
		for (auto i = this->directories.begin(); i != this->directories.end(); i++)
		{
			if (i->second->name == dir)
			{
				found = true;
				i->second->parent->directories.erase(dir);
				this->directories.erase(i->second->path);
				break;
			}
		}
		if (!found)
			cout << "Directory does not exist...\n";
	}
	void PWD()
	{
		if (cin.peek() == '\n')
		{
			size_t pos = this->CurrDir->path.find_last_of(">");
			cout << this->CurrDir->path.substr(0, pos) << "\n\n";
		}
		else
			cout << "Invalid or missing command...\n";

	}
	void TREE(Directory* dir)
	{
		for (int i = 0; i < dir->depth; i++)
			cout << "\t";
		cout << dir->name << endl;
		for (auto i = dir->directories.begin(); i != dir->directories.end(); i++)
		{
			TREE(i->second);
		}
	}
	void VER()
	{
		cout << "DOS version 22.083\nRelease date: 9 November 2023\n\n";
	}
	void EDIT()
	{
		string file;
		if (cin.peek() == '\n')
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		cin >> file;
		size_t pos = file.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string name = file.substr(0, pos), extension = file.substr(pos + 1);
		if (extension != "txt" && extension != "text")
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		if (this->CurrDir->files.find(make_pair(name, extension)) == this->CurrDir->files.end())
		{
			cout << "File does not exist in current directory...\n";
			return;
		}
		editor->Run(file);
		system("cls");
	}
	void FINDF()
	{
		string file;
		if (cin.peek() == '\n')
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		cin >> file;
		size_t pos = file.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string name = file.substr(0, pos), extension = file.substr(pos + 1);
		if (extension != "txt" && extension != "text")
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		if (!FindFileInAllDir(name,extension))
		{
			cout << "File does not exist in current directory...\n";
			return;
		}
		if (cin.peek() == '\n')
		{
			cout << "Invalid or missing word...\n";
			return;
		}
		string word;
		cin >> word;
		bool exit = editor->OpenFile(file);
		if (exit)
			return;
		vector<vector<Pos>>range;
		editor->DoOperation("FINDF " + word, range);
		system("cls");
		editor->ColoredPrint(range);
	}
	void PROMPT()
	{
		string prompt;
		if (cin.peek() == '\n')
		{
			cout << "Invalid or missing command...\n";
			return;
		}
		cin >> prompt;
		transform(prompt.begin(), prompt.end(), prompt.begin(), toupper);
		if (prompt != "V$" && prompt != "P$G$")
		{
			cout << "Invalid or missing command...\n";
			return;
		}
		if (prompt == "V$")
			DisplayRootPath = "V$";
		else
			DisplayRootPath = "V";
	}
	void QUEUE()
	{
		for (auto i = this->print.begin(); i != this->print.end(); i++)
		{
			tm t;
			localtime_s(&t, &(*i)->time);
			cout << (*i)->FileName << "\t\t";
			printf("%02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec);
		}
	}
	void PQUEUE()
	{
		for (auto i = this->PriorityPrint.begin(); i != this->PriorityPrint.end(); i++)
		{
			tm t;
			localtime_s(&t, &(*i)->time);
			cout << (*i)->FileName << "\t\t";
			printf("%02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec);
		}
	}
	void PRINT()
	{
		string file;
		if (cin.peek() == '\n')
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		cin >> file;
		size_t pos = file.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string name = file.substr(0, pos), extension = file.substr(pos + 1);
		if (extension != "txt" && extension != "text")
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		Directory* d = FindFileInAllDir(name, extension);
		if (!d)
		{
			cout << "File does not exist...\n";
			return;
		}
		srand(time(0));
		int random = rand();
		time_t t = time(nullptr);
		(random % 2 == 0) ? this->print.push_back(new PrintItem(file, t)) : this->print.push_front(new PrintItem(file, t));
		QUEUE();
	}
	void PPRINT()
	{
		string file;
		if (cin.peek() == '\n')
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		cin >> file;
		size_t pos = file.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string name = file.substr(0, pos), extension = file.substr(pos + 1);
		if (extension != "txt" && extension != "text")
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		Directory* d = FindFileInAllDir(name, extension);
		if (!d)
		{
			cout << "File does not exist...\n";
			return;
		}
		time_t t = time(nullptr);
		this->PriorityPrint.push_back(new PrintItem(file, t));
		PQUEUE();
	}
	void ATTRIB()
	{
		string file;
		if (cin.peek() == '\n')
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		cin >> file;
		size_t pos = file.find_last_of(".");
		if (pos == 0 || pos == string::npos)
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		string name = file.substr(0, pos), extension = file.substr(pos + 1);
		if (extension != "txt" && extension != "text")
		{
			cout << "Invalid file name or type...\n";
			return;
		}
		Directory* d = FindFileInAllDir(name, extension);
		if (!d)
		{
			cout << "File does not exist...\n";
			return;
		}
		auto it = d->files.find(make_pair(name, extension));
		cout << (*it).second;
	}
	void DoOperation(string& command)
	{
		transform(command.begin(), command.end(), command.begin(), toupper);
		if (command == "ATTRIB")
			ATTRIB();
		else if (command == "CD" || command == "PWD")
			CDChange();
		else if (command == "CD\\")
			CDReset();
		else if (command == "CD..")
			CDPrev();
		else if (command == "CD.")
			CDDot();
		else if (command == "CREATE")
			CREATE();
		else if (command == "DEL")
			DEL();
		else if (command == "EXIT")
			EXIT();
		else if (command == "DIR")
			DIR();
		else if (command == "RENAME")
			RENAME();
		else if (command == "HELP")
			HELP();
		else if (command == "MKDIR")
			MKDIR();
		else if (command == "LOADTREE")
			LOADTREE();
		else if (command == "CONVERT")
			CONVERT();
		else if (command == "COPY")
			COPY();
		else if (command == "MOVE")
			MOVE();
		else if (command == "RMDIR")
			RMDIR();
		else if (command == "PWD")
			PWD();
		else if (command == "SAVE")
			SAVE();
		else if (command == "TREE")
			TREE(this->directories[this->RootPath]);
		else if (command == "CLS")
			system("cls");
		else if (command == "VER")
			VER();
		else if (command == "EDIT")
			EDIT();
		else if (command == "FINDF")
			FINDF();
		else if (command == "PROMPT")
			PROMPT();
		else if (command == "PRINT")
			PRINT();
		else if (command == "PPRINT")
			PPRINT();
		else if (command == "QUEUE")
			QUEUE();
		else if (command == "PQUEUE")
			PQUEUE();
		else
			cout << "Invalid command...\n";
		while (cin.peek() != '\n')
			cin.ignore();
	}
	void Run()
	{
		string command;
		cout << this->user;
		size_t pos;
		while (true)
		{
			pos = this->CurrDir->path.find_first_of('\\');
			cout << ((this->CurrDir->path == this->RootPath) ? DisplayRootPath + "\\:>" : DisplayRootPath + ":" + this->CurrDir->path.substr(pos));
			cin >> command;
			DoOperation(command);
		}
	}
};