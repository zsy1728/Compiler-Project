#include <bits/stdc++.h>
#include <string>
using namespace std;
//堆栈
stack<int> state_stack;
stack<char> sign_stack;

char gram[300][300]; //存文法
int length[300];  //文法的长度
int number = 0;   //文法的个数
bool isV[300];    // buffer of input
char Vn[300];     //非终结符
int size_vn = 0;
char Vt[300]; //终结符
int size_vt = 0;
bool first[300][300]; // first集
char buffer[300];
int size = 0; // 要分析的字符串长度
struct T //转换表  项目集转换
{
    int begin;
    int next;
    char ch;
};
struct project //项目集
{
    int num;     // 第几个文法
    int now;     // 小点点在第几个
    char search; // 望符
};
struct action
{
    char ch;
    int next_state;
};

T trans[300];
int size_trans = 0;

int num_proj = 0;
project projs[300][300]; // 项目【第几个项目】【第几个文法】
int size_proj[300];      // 项目中文法数量

action ac_go_table[300][300];
int size_ag_table[300];

//文件定义
ifstream grammar_file, input_file;
ofstream projs_file, actGoto_file, firstset_file, procedure_file;

void get_grammar() //读文法
{
    char nouse, temp;
    int i = 0;
    gram[0][0] = 'S'; // S‘->`S
    length[0] = 2;
    grammar_file >> number;
    for (i = 1; i <= number; ++i)
    {
        int j = 0;
        grammar_file >> temp;
        grammar_file >> nouse >> nouse;
        while (temp != '$')
        {
            gram[i][j++] = temp;
            isV[temp] = true;
            grammar_file >> temp;
        }
        length[i] = j;
    }
    gram[0][1] = gram[1][0];
    // ASCII码表,统计出各种字符
    for (i = 0; i < 64; ++i) // 终结符
    {
        if (isV[i])
        {
            Vt[size_vt++] = i;
        }
    }
    Vt[size_vt++] = '#';
    // 64 为 @
    for (i = 65; i < 91; ++i) // 非终结符（大写字母）
    {
        if (isV[i])
        {
            Vn[size_vn++] = i;
        }
    }
    for (i = 91; i < 128; ++i) // 终结符
    {
        if (isV[i])
        {
            Vt[size_vt++] = i;
        }
    }
}
void write_first_set()
{
    for (int i = 0; i < size_vn; ++i)
    {
        char ch = Vn[i];
        firstset_file << "first " << ch << " = ";
        for (int j = 0; j < 128; ++j)
        {
            if (first[ch - 'A'][j] == true)
            {
                firstset_file << char(j) << " ";
            }
        }
        firstset_file << endl;
    }
}
void get_first()
{
    bool done = true; //还有能改的地方 就不停的做
    int t, k;
    bool isempty;
    while (done)
    {
        done = false;
        for (int i = 1; i <= number; i++) //  对于每一个文法
        {
            t = 1; // 这文法第一个字符
            isempty = true;
            while (isempty && t < length[i])
            {
                isempty = false;
                if (gram[i][t] >= 'A' && gram[i][t] <= 'Z') // 第i个文法的第t个字符是非终结符
                {
                    for (k = 0; k <= 63; ++k)
                    {
                        // 这个非终结first集有而且没加进去
                        if (first[gram[i][t] - 'A'][k] == true && !first[gram[i][0] - 'A'][k])
                        {
                            first[gram[i][0] - 'A'][k] = true;
                            done = true;
                        }
                    }
                    if (first[gram[i][t] - 'A'][64] == true) // first集有空
                    {
                        isempty = true;
                        ++t;
                    }
                    for (k = 91; k < 128; ++k)
                    {
                        if (first[gram[i][t] - 'A'][k] == true && first[gram[i][0] - 'A'][k] == false)
                        {
                            done = true;
                            first[gram[i][0] - 'A'][k] = true;
                        }
                    }
                }
                else if (first[gram[i][0] - 'A'][gram[i][t]] == false) // 不是非终结符但是还没写入
                {
                    done = true;
                    first[gram[i][0] - 'A'][gram[i][t]] = true;
                }
            }
        }
    }
    write_first_set();
}

void getSearch(project temp) //得到向前搜索符
{
    size = 0;
    bool flag = true;
    int nownow = temp.now;
    int i;
    while (flag == true)
    {
        flag = false;
        if (nownow + 1 >= length[temp.num])
        {
            buffer[size++] = temp.search;
            return;
        }
        else if (gram[temp.num][nownow + 1] < 'A' || gram[temp.num][nownow + 1] > 'Z')
        {
            buffer[size++] = gram[temp.num][nownow + 1];
            return;
        }
        else if (gram[temp.num][nownow + 1] >= 'A' && gram[temp.num][nownow + 1] <= 'Z')
        {
            for (i = 0; i < 64; ++i)
            {
                if (first[gram[temp.num][nownow + 1] - 'A'][i] == true)
                    buffer[size++] = i;
            }
            for (i = 91; i < 128; ++i)
            {
                if (first[gram[temp.num][nownow + 1] - 'A'][i] == true)
                {
                    buffer[size++] = i;
                }
            }
            if (first[gram[temp.num][nownow + 1] - 'A'][64] == true)
            {
                ++nownow;
                flag = true;
            }
        }
    }
}
bool isRepeat(project temp, int T) //当前项目集元素是否重复
{
    int i;
    for (i = 0; i < size_proj[T]; ++i)
    {
        if (projs[T][i].num == temp.num && projs[T][i].now == temp.now && projs[T][i].search == temp.search)
        {
            return true;
        }
    }
    return false;
}
void getClosure(int T) //求项目集族
{
    project temp;
    int i, j, k;
    for (i = 0; i < size_proj[T]; ++i)
    {
        if (gram[projs[T][i].num][projs[T][i].now] >= 'A' && gram[projs[T][i].num][projs[T][i].now] <= 'Z')
        {
            for (j = 0; j < 300; ++j)
            {
                size = 0;
                if (gram[j][0] == gram[projs[T][i].num][projs[T][i].now])
                {
                    getSearch(projs[T][i]);
                    for (k = 0; k < size; ++k)
                    {
                        temp.num = j;
                        temp.now = 1;
                        temp.search = buffer[k];
                        if (isRepeat(temp, T) == false)
                        {
                            projs[T][size_proj[T]++] = temp;
                        }
                    }
                }
            }
        }
    }
    return;
}
int isContained() //当前项目集 和以前比较
{
    int i;
    int sum = 0;
    int j;
    int k;
    for (i = 0; i < num_proj; ++i)
    {
        sum = 0; //记录有多少是匹配的
        if (size_proj[num_proj] == size_proj[i])
        {
            for (j = 0; j < size_proj[num_proj]; ++j)
            {
                for (k = 0; k < size_proj[i]; ++k)
                {
                    if (projs[i][k].num == projs[num_proj][j].num && projs[i][k].now == projs[num_proj][j].now && projs[i][k].search == projs[num_proj][j].search)
                    {
                        ++sum;
                        break;
                    }
                }
            }
        }
        if (sum == size_proj[num_proj])
        {
            return i;
        }
    }
    return 0;
}
void write_set()
{
    for (int i = 0; i <= num_proj; i++)
    {
        projs_file << "I" << i << ":" << endl;
        for (int j = 0; j < size_proj[i]; ++j)
        {
            int row = projs[i][j].num;
            projs_file << gram[row][0] << "->";
            for (int k = 1; k < length[row]; k++)
            {
                if (k == projs[i][j].now)
                    projs_file << "·";
                projs_file << gram[row][k];
            }
            if (projs[i][j].now == length[row])
                projs_file << "·";
            projs_file << " , " << projs[i][j].search << endl;
        }
        projs_file << endl;
    }
}
void make_set()
{
    projs[0][0].num = 0;
    projs[0][0].now = 1;
    projs[0][0].search = '#';
    size_proj[0] = 1;
    getClosure(0);
    project buf[50];
    int buf_size = 0;
    project tp;
    int nextt_state;
    int index;
    for (index = 0; index < num_proj + 1; ++index) //对于项目
    {
        for (int j = 0; j < size_vt; ++j) // 对于某个终结符 找可以产生个新项目
        {
            buf_size = 0;
            for (int p = 0; p < size_proj[index]; ++p) // 对于项目每一个文法
            {
                // 如果小点没到头 而且下标是非终结符
                if (projs[index][p].now < length[projs[index][p].num] && gram[projs[index][p].num][projs[index][p].now] == Vt[j])
                {
                    tp.num = projs[index][p].num;
                    tp.search = projs[index][p].search;
                    tp.now = projs[index][p].now + 1; // tp 动了一个点
                    buf[buf_size++] = tp;
                }
            }
            if (buf_size != 0) //产生一个新的项目集
            {
                num_proj++;
                for (int t = 0; t < buf_size; ++t) // 缓冲区每一个都弄
                {
                    projs[num_proj][size_proj[num_proj]++] = buf[t];
                }
                getClosure(num_proj);
                nextt_state = isContained(); //检查第num_proj个项目集是否重复
                if (nextt_state != 0)         //重复，则转换到已有的那个项目集中去
                {
                    size_proj[num_proj--] = 0;
                    trans[size_trans].begin = index;
                    trans[size_trans].next = nextt_state;
                    trans[size_trans++].ch = Vt[j];
                }
                else // 不重复
                {
                    trans[size_trans].begin = index;
                    trans[size_trans].next = num_proj;
                    trans[size_trans++].ch = Vt[j];
                }
            }
        }

        for (int j = 0; j < size_vn; ++j) // 对于某个非终结符 找可以产生个新项目
        {
            buf_size = 0;
            for (int p = 0; p < size_proj[index]; ++p)
            {
                if (projs[index][p].now < length[projs[index][p].num] && gram[projs[index][p].num][projs[index][p].now] == Vn[j])
                {
                    tp.num = projs[index][p].num;
                    tp.search = projs[index][p].search;
                    tp.now = projs[index][p].now + 1;
                    buf[buf_size++] = tp;
                }
            }
            if (buf_size != 0) //产生一个新的项目集
            {
                ++num_proj;
                for (int t = 0; t < buf_size; ++t)
                {
                    projs[num_proj][size_proj[num_proj]++] = buf[t];
                }
                getClosure(num_proj);
                nextt_state = isContained();

                if (nextt_state != 0)
                {
                    size_proj[num_proj--] = 0;
                    trans[size_trans].begin = index;
                    trans[size_trans].next = nextt_state;
                    trans[size_trans++].ch = Vn[j];
                }
                else
                {
                    trans[size_trans].begin = index;
                    trans[size_trans].next = num_proj;
                    trans[size_trans++].ch = Vn[j];
                }
            }
        }
    }
    write_set();
}

void write_action_goto(){
    for(int i = 0;i < size_vt;i ++){
        actGoto_file<<"\t"<<Vt[i];
    }
    actGoto_file <<"\t";
    for(int i = 0;i < size_vn;i ++){
        actGoto_file<<"\t"<<Vn[i];
    }
    actGoto_file <<endl;
    for (int i = 0; i <= num_proj; ++i) // 写
    {
        actGoto_file << i;
        for(int j = 0;j < size_vt;j ++){
            actGoto_file<<"\t";
            for(int k = 0;k < size_ag_table[i];k ++){
                if(ac_go_table[i][k].ch == Vt[j]){
                    int st = ac_go_table[i][k].next_state;
                    if(st < 0) actGoto_file<<"r"<< - st;
                    else if(st == 0) actGoto_file<<"acc";
                    else actGoto_file<<"S"<<st;       
                }
            }
        }
        actGoto_file <<"\t";
        for(int j = 0;j < size_vn;j ++){
            actGoto_file<<"\t";
            for(int k = 0;k < size_ag_table[i];k ++){
                if(ac_go_table[i][k].ch == Vn[j]){
                    int st = ac_go_table[i][k].next_state;
                    actGoto_file<<st;
                }
            }
        }
        actGoto_file <<endl;
    }
}
void get_actionGoto()
{
    for (int i = 0; i <= num_proj; ++i) // 对于每一个项目
    {
        for (int j = 0; j < size_proj[i]; ++j) // 对于项目里每一文法
        {
            if (projs[i][j].now == length[projs[i][j].num] ||
                (projs[i][j].now == 1 && length[projs[i][j].num] == 2 && gram[projs[i][j].num][1] == '@'))
            {
                //若项目 A→a?,a 属于Ik ，则对a为任何终结符或‘#’，置ACTION[k,a] = rj ，j为产生式编号
                ac_go_table[i][size_ag_table[i]].ch = projs[i][j].search;
                ac_go_table[i][size_ag_table[i]++].next_state = projs[i][j].num * (-1);
            }
        }
    }
    for (int i = 0; i < size_trans; ++i)
    {
        int t1 = trans[i].begin;
        int t2 = trans[i].next;
        char tp = trans[i].ch;
        ac_go_table[t1][size_ag_table[t1]].ch = tp;
        ac_go_table[t1][size_ag_table[t1]++].next_state = t2;
    }
    write_action_goto();
}
void write_stack(int x)
{
    stack<int> a;
    stack<char> c;
    if (x == 1) //状态
    {
        while (state_stack.empty() == false)
        {
            a.push(state_stack.top());
            state_stack.pop();
        }
        stringstream ss;
        while (a.empty() == false)
        {
            ss<< a.top() << ',';
            
            state_stack.push(a.top());
            a.pop();
        }
        procedure_file <<setw(92)<<setiosflags(ios::left)<< ss.str();
    }
    if (x == 2) //符号
    {
        while (sign_stack.empty() == false)
        {
            c.push(sign_stack.top());
            sign_stack.pop();
        }
        stringstream ss;
        while (c.empty() == false)
        {
            ss<<c.top();
            sign_stack.push(c.top());
            c.pop();
        }
        procedure_file <<setw(37)<<setiosflags(ios::left)<< ss.str();
    }
    
}
void analysis()
{
    size = 0;
    while (input_file >> buffer[size] && buffer[size++] != '#');
    int now_state = 0;
    int now_index = 0;
    bool error = false;
    bool done = false;
    char now_in ;

    state_stack.push(0);
    sign_stack.push('#');
    procedure_file <<setw(37)<<setiosflags(ios::left)<< "符号栈";
    procedure_file <<setw(93)<<setiosflags(ios::left)<< "状态栈";
    procedure_file <<setw(80)<<setiosflags(ios::left)<< "输入串";
    procedure_file <<setw(15)<<setiosflags(ios::left)<< "ACTION";
    procedure_file <<setw(10)<<setiosflags(ios::left)<< "GOTO";
    procedure_file << endl;

    while (done == false && error == false)
    {
        now_state = state_stack.top();
        now_in = buffer[now_index];
        write_stack(2);
        write_stack(1);
        stringstream buf;
        for (int i = now_index; i < size; ++i)
            buf << buffer[i];
        procedure_file <<setw(80)<<setiosflags(ios::left)<<buf.str();
        error = true;
        for (int i = 0; i < size_ag_table[now_state]; ++i){ // 对于该状态可接受的字符
            if (ac_go_table[now_state][i].ch == now_in)
            {
                error = false;
                if (ac_go_table[now_state][i].next_state == 0) // YES
                {
                    procedure_file << "YES" << endl;
                    cout << "YES" << endl;
                    done = true;
                    break;
                }
                else if (ac_go_table[now_state][i].next_state > 0) // 移进
                {
                    procedure_file << 'S' << ac_go_table[now_state][i].next_state << endl;

                    state_stack.push(ac_go_table[now_state][i].next_state);
                    sign_stack.push(ac_go_table[now_state][i].ch);
                    ++now_index;
                    break;
                }
                else if (ac_go_table[now_state][i].next_state < 0) // 规约
                {
                    int tp = ac_go_table[now_state][i].next_state * (-1);
                    procedure_file << 'r' << tp << "              ";
                    int len = length[tp] - 1;
                    if (gram[tp][1] == '@')
                        --len;
                    for (int k = 0; k < len; ++k) // 文法有几个字符就去掉几个状态
                    {
                        state_stack.pop();
                        sign_stack.pop();
                    }
                    sign_stack.push(gram[tp][0]); // 放一个非终结符进去
                    int aa = state_stack.top();
                    for (int k = 0; k < size_ag_table[aa]; ++k)
                    {
                        if (ac_go_table[aa][k].ch == gram[tp][0]) // 找它
                        {
                            state_stack.push(ac_go_table[aa][k].next_state);
                            procedure_file << ac_go_table[aa][k].next_state << endl;
                        }
                    }
                    break;
                }
            }
        }
    }
    if (!done)
    {
        cout << "NO" << endl;
        cout << "出错原因可能是未找到：";
        for (int i = 0; i < size_ag_table[now_state]; ++i)
        {
            cout << ac_go_table[now_state][i].ch << " ";
        }
        cout << endl;
    }
    cout << endl
		 << "-----------文件写入完成-----------" << endl << endl;
}
void open_file()
{
    grammar_file.open("syntax2.txt");
    input_file.open("myOutput.txt");
    projs_file.open("projects.txt");
    actGoto_file.open("actGotoTable.txt");
    firstset_file.open("first.txt");
    procedure_file.open("temp.txt");
}
void close_file()
{
    grammar_file.close();
    input_file.close();
    projs_file.close();
    actGoto_file.close();
    firstset_file.close();
    procedure_file.close();
}
int main()
{
    open_file();
    get_grammar();
    get_first();
    make_set();
    get_actionGoto();
    analysis();
    close_file();
    system("pause");
    return 0;
}
