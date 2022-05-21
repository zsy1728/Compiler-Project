#include <bits/stdc++.h>
#define SETWF setw(8) << setiosflags(ios::left) 
#define SETWT setw(15) << setiosflags(ios::left)
using namespace std;

const char keyword[50][12] = {"break", "case", "char", "continue", "do", "default", "double",
							  "else", "float", "for", "if", "int", "include", "long", "main",
							   "return", "switch", "typedef", "void", "unsigned", "while"};
const char Monocular_Operator[20] = {'<', '>','+', '-', '*', '/', '!', '%', '~', '&', '|', '^', '='};
const char Binocular_Operator[20][5] = {"++", "--", "&&", "||", "<=", "!=", "==", ">=", "+=", "-=", "*=", "/="};
const char Delimiter[20] = {',', '(', ')', '{', '}', ';',  '#'};

char f(char str[])
{
	if (strcmp(str, keyword[0]) == 0)
		return 'b';
	if (strcmp(str, keyword[1]) == 0)
		return 'c';
	if (strcmp(str, keyword[2]) == 0) // char  a
		return 'a';
	if (strcmp(str, keyword[3]) == 0) // continue   o
		return 'o';
	if (strcmp(str, keyword[4]) == 0)
		return 'd';
	if (strcmp(str, keyword[5]) == 0) // default    n
		return 'n';
	if (strcmp(str, keyword[6]) == 0) // double   z
		return 'z';
	if (strcmp(str, keyword[7]) == 0)
		return 'e';
	if (strcmp(str, keyword[8]) == 0) // float  y
		return 'y';
	if (strcmp(str, keyword[9]) == 0)
		return 'f';
	if (strcmp(str, keyword[10]) == 0)
		return 'i';
	if (strcmp(str, keyword[11]) == 0) // int    h
		return 'h';
	if (strcmp(str, keyword[12]) == 0) // include  p
		return 'p';
	if (strcmp(str, keyword[13]) == 0)
		return 'l';
	if (strcmp(str, keyword[14]) == 0)
		return 'm';
	if (strcmp(str, keyword[15]) == 0)
		return 'r';
	if (strcmp(str, keyword[16]) == 0)
		return 's';
	if (strcmp(str, keyword[17]) == 0)
		return 't';
	if (strcmp(str, keyword[18]) == 0)
		return 'v';
	if (strcmp(str, keyword[19]) == 0)
		return 'u';
	if (strcmp(str, keyword[20]) == 0)
		return 'w';
}

ifstream input;
ofstream output;

FILE *file_source = NULL;

char start; // 开始非终结符
char state[100];
char final[100];
int len_state;
int len_final;

struct Set
{
	char set[100];
	int len = 0;
};
Set nfa[100][100];

Set dfa_set[100]; // dfa状态
int num_dfa_set = 0;  // dfa状态个数
int dfa[150][150];

bool is_Final[150]; // 是不是终态

void showSet(Set set)
{
	for (int i = 0; i < set.len; i++)
		cout << set.set[i];
}

bool is_state(char a)
{
	for (int i = 0; i < len_state; ++i)
	{
		if (a == state[i])
			return true;
	}
	return false;
}
bool isFinal(char a)
{
	for (int i = 0; i < len_final; ++i)
	{
		if (a == final[i])
			return true;
	}
	return false;
}
void createNFA() // 扫秒文件同时存储NFA
{

	int N;
	bool flag = true; //是不是第一个
	char ch;		  //用来读 文法左边的
	char temp;
	char str[10]; //用来读 文法 右边的
	input.open("syntax1.txt");
	input >> N;
	while (N--)
	{
		input >> ch >> temp >> temp >> str;

		if (flag)
		{
			start = ch;
			flag = false;
		}
		if (!is_state(ch))
		{
			state[len_state++] = ch;
		}
		if (!isFinal(str[0]))
		{
			final[len_final++] = str[0];
		}
		if (strlen(str) > 1)
		{

			nfa[ch][str[0]].set[nfa[ch][str[0]].len++] = str[1]; //  通过终结符到达的所有终结符
		}
		else
		{
			nfa[ch][str[0]].set[nfa[ch][str[0]].len++] = 'Y'; //终态
		}
		// printf("%c %c %d\n",ch, str[0],  nfa[ch][str[0]].len);
	}
	cout << "NFA create compelete ------------------" << endl;
}

bool is_in_set(char a, Set temp)
{
	for (int i = 0; i < temp.len; ++i)
	{
		if (a == temp.set[i])
			return true;
	}
	return false;
}
void get_closure(Set &temp) //得到一个完整的子集
{
	for (int i = 0; i < temp.len; ++i)
	{
		for (int j = 0; j < nfa[temp.set[i]]['@'].len; ++j)
		{
			if (!is_in_set(nfa[temp.set[i]]['@'].set[j], temp))
			{
				temp.set[temp.len++] = nfa[temp.set[i]]['@'].set[j];
			}
		}
	}
}
bool is_contained_Y(Set temp) //判断是否是终态
{
	for (int i = 0; i < temp.len; ++i)
	{
		if (temp.set[i] == 'Y')
			return true;
	}
	return false;
}
int isMarked(Set temp) //和已有的dfa_set有没有重复的，有就返回重复的编号
{
	for (int i = 0; i < num_dfa_set; ++i)
	{
		if (temp.len == dfa_set[i].len)
		{
			bool flag = true;
			for (int j = 0; j < temp.len; j++)
			{
				if (!is_in_set(temp.set[j], dfa_set[i]))
					flag = false;
			}
			if (flag)
				return i;
		}
	}
	return -1;
}
void NFA_to_DFA()
{
	num_dfa_set = 0;
	Set nowT;
	Set moveT;
	nowT.set[nowT.len++] = start;
	moveT.len = 0;
	stack<Set> s;
	get_closure(nowT);
	s.push(nowT);
	dfa_set[num_dfa_set++] = nowT;
	memset(dfa, -1, sizeof(dfa));

	for (int i = 0; i < 150; ++i)
		is_Final[i] = false;
	if (is_contained_Y(nowT))
		is_Final[num_dfa_set - 1] = true;
	for (int i = 0; i < len_final; i++)
	{
		if (final[i] == '@')
			continue;
		cout << "\t" << final[i];
	}
	cout << endl
		 << endl;
	while (!s.empty())
	{
		nowT = s.top();
		s.pop();
		showSet(nowT);
		cout << ":  ";

		for (int i = 0; i < len_final; ++i) // 对于每一个输入字母
		{
			if (final[i] == '@')
				continue;
			for (int j = 0; j < nowT.len; ++j) // 对于nowT的每个状态
			{
				for (int k = 0; k < nfa[nowT.set[j]][final[i]].len; ++k) // 对于该状态的每一个跳转
				{
					if (!is_in_set(nfa[nowT.set[j]][final[i]].set[k], moveT))
						moveT.set[moveT.len++] = nfa[nowT.set[j]][final[i]].set[k];
				}
			}
			get_closure(moveT);
			cout << "\t";
			showSet(moveT);
			if (moveT.len > 0 && isMarked(moveT) == -1) // 如果没有
			{
				dfa[num_dfa_set - 1][final[i]] = num_dfa_set;
				s.push(moveT);
				dfa_set[num_dfa_set++] = moveT;
				if (is_contained_Y(moveT))
				{
					is_Final[num_dfa_set - 1] = true;
				}
			}
			if (moveT.len > 0 && isMarked(moveT) > -1)
			{
				dfa[isMarked(nowT)][final[i]] = isMarked(moveT);
			}

			moveT.len = 0;
		}
		cout << endl;
	}
	cout << endl;
}

bool isInteger(char a)
{
	if (a >= '0' && a <= '9')
		return true;
	return false;
}
bool isLetter(char a)
{
	if (a >= 'a' && a <= 'z')
		return true;
	if (a >= 'A' && a <= 'Z')
		return true;
	return false;
}
bool isKeyword(char a[])
{
	int len = strlen(a);
	for (int j = 0; j < 21; ++j)
	{
		if (strlen(keyword[j]) == len)
		{
			if (strcmp(keyword[j], a) == 0)
				return true;
		}
	}
	return false;
}
bool isMO(char a)
{
	for (int i = 0; i < 13; ++i)
	{
		if (Monocular_Operator[i] == a)
			return true;
	}
	return false;
}
bool isBO(char a[])
{
	for (int i = 0; i < 12; ++i)
	{
		if (strcmp(Binocular_Operator[i], a) == 0)
			return true;
	}
	return false;
}
bool isDelimiter(char a)
{
	for (int i = 0; i < 7; ++i)
	{
		if (Delimiter[i] == a)
			return true;
	}
	return false;
}
bool analysis(char str[])
{
	char now_state = 0;
	for (int i = 0; i < strlen(str); ++i)
	{
		now_state = dfa[now_state][str[i]];
		if (now_state == -1)
			return false;
	}
	if (is_Final[now_state] == true)
		return true;
	return false;
}
void scan()
{
	char str[100];
	char ch = fgetc(file_source);
	bool finish = false;
	int row = 0;
	while (!finish)
	{
		int flag = -1;
		int index = 0;
		if (isInteger(ch)) //多一个ch
		{
			while (isLetter(ch) || isInteger(ch) || ch == '.')
			{
				flag = 1;
				str[index++] = ch;
				ch = fgetc(file_source);
			}
			str[index] = '\0';
		}
		if (flag == 1) // 确实是数字开头
		{
			if (analysis(str))
			{
				cout << SETWF << row << SETWT << str << "常量" << endl;
				output << 3;
			}
			else
			{
				cout << str << " ----出错，不是常量" << endl;
			}
			index = 0;
			flag = -1;
		}
		if (isLetter(ch))
		{
			while (isLetter(ch) || isInteger(ch))
			{
				flag = 2;
				str[index++] = ch;
				ch = fgetc(file_source);
			}
			str[index] = '\0';
		}
		if (flag == 2) // 确实是字母开头
		{
			if (isKeyword(str))
			{
				cout  << SETWF << row << SETWT << str << "关键字" << endl;
				// cout << str << " "
				// 	 << "关键字" << endl;
				output << f(str);
			}
			else
			{
				if (analysis(str))
				{
					cout << SETWF << row << SETWT << str << "标识符" << endl;
					output << 2;
				}
				else
				{
					cout << str << " "
						 << "----出错，不是标识符" << endl;
				}
			}
			index = 0;
			flag = -1;
		}
		if (isDelimiter(ch))
		{
			cout << SETWF << row << SETWT << ch << "界符" << endl;
			if (ch == '#')
				output << '*';
			else
				output << ch;
			if ((ch = fgetc(file_source)) == EOF)
			{
				finish = true;
				break;
			}
		}
		if (isMO(ch))
		{
			str[index++] = ch;
			if ((ch = fgetc(file_source)) == EOF)
			{
				finish = true;
			}
			str[index++] = ch;
			str[index] = '\0';
			if (finish == false && isBO(str))
			{
				cout << SETWF << row << SETWT << str << "运算符" << endl;

				output<<str;
				ch = fgetc(file_source);
			}
			else
			{
				cout << SETWF << row << SETWT << str[0] << "运算符" << endl;
				output << str[0];
			}
			index = 0;
		}

		if (ch == ' ' || ch == '\n' || ch == '\t' || ch == EOF)
		{
			if(ch == '\n'){
				row++;
				ch = fgetc(file_source);
				continue;
			}
			else if (ch == EOF){
				finish = true;
				break;
			}else{
				ch = fgetc(file_source);
			}
			
		}
	}
	output << '#';
}
int main()
{

	createNFA();

	NFA_to_DFA();

	for (int i = 0; i < num_dfa_set; i++)
	{
		showSet(dfa_set[i]);
		cout << "\t" << i;
		cout << endl;
	}
	file_source = fopen("mySource.txt", "r+");
	output.open("myOutput.txt");

	cout << endl
		 << "-----------start-----------" << endl;

	scan();

	cout << endl
		 << "-----------bye-----------" << endl;

	fclose(file_source);
	output.close();
	system("pause");
}