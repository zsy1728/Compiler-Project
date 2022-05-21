#include <bits/stdc++.h>
#include <string>
using namespace std;
//��ջ
stack<int> state_stack;
stack<char> sign_stack;

char gram[300][300]; //���ķ�
int length[300];  //�ķ��ĳ���
int number = 0;   //�ķ��ĸ���
bool isV[300];    // buffer of input
char Vn[300];     //���ս��
int size_vn = 0;
char Vt[300]; //�ս��
int size_vt = 0;
bool first[300][300]; // first��
char buffer[300];
int size = 0; // Ҫ�������ַ�������
struct T //ת����  ��Ŀ��ת��
{
    int begin;
    int next;
    char ch;
};
struct project //��Ŀ��
{
    int num;     // �ڼ����ķ�
    int now;     // С����ڵڼ���
    char search; // ����
};
struct action
{
    char ch;
    int next_state;
};

T trans[300];
int size_trans = 0;

int num_proj = 0;
project projs[300][300]; // ��Ŀ���ڼ�����Ŀ�����ڼ����ķ���
int size_proj[300];      // ��Ŀ���ķ�����

action ac_go_table[300][300];
int size_ag_table[300];

//�ļ�����
ifstream grammar_file, input_file;
ofstream projs_file, actGoto_file, firstset_file, procedure_file;

void get_grammar() //���ķ�
{
    char nouse, temp;
    int i = 0;
    gram[0][0] = 'S'; // S��->`S
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
    // ASCII���,ͳ�Ƴ������ַ�
    for (i = 0; i < 64; ++i) // �ս��
    {
        if (isV[i])
        {
            Vt[size_vt++] = i;
        }
    }
    Vt[size_vt++] = '#';
    // 64 Ϊ @
    for (i = 65; i < 91; ++i) // ���ս������д��ĸ��
    {
        if (isV[i])
        {
            Vn[size_vn++] = i;
        }
    }
    for (i = 91; i < 128; ++i) // �ս��
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
    bool done = true; //�����ܸĵĵط� �Ͳ�ͣ����
    int t, k;
    bool isempty;
    while (done)
    {
        done = false;
        for (int i = 1; i <= number; i++) //  ����ÿһ���ķ�
        {
            t = 1; // ���ķ���һ���ַ�
            isempty = true;
            while (isempty && t < length[i])
            {
                isempty = false;
                if (gram[i][t] >= 'A' && gram[i][t] <= 'Z') // ��i���ķ��ĵ�t���ַ��Ƿ��ս��
                {
                    for (k = 0; k <= 63; ++k)
                    {
                        // ������ս�first���ж���û�ӽ�ȥ
                        if (first[gram[i][t] - 'A'][k] == true && !first[gram[i][0] - 'A'][k])
                        {
                            first[gram[i][0] - 'A'][k] = true;
                            done = true;
                        }
                    }
                    if (first[gram[i][t] - 'A'][64] == true) // first���п�
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
                else if (first[gram[i][0] - 'A'][gram[i][t]] == false) // ���Ƿ��ս�����ǻ�ûд��
                {
                    done = true;
                    first[gram[i][0] - 'A'][gram[i][t]] = true;
                }
            }
        }
    }
    write_first_set();
}

void getSearch(project temp) //�õ���ǰ������
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
bool isRepeat(project temp, int T) //��ǰ��Ŀ��Ԫ���Ƿ��ظ�
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
void getClosure(int T) //����Ŀ����
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
int isContained() //��ǰ��Ŀ�� ����ǰ�Ƚ�
{
    int i;
    int sum = 0;
    int j;
    int k;
    for (i = 0; i < num_proj; ++i)
    {
        sum = 0; //��¼�ж�����ƥ���
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
                    projs_file << "��";
                projs_file << gram[row][k];
            }
            if (projs[i][j].now == length[row])
                projs_file << "��";
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
    for (index = 0; index < num_proj + 1; ++index) //������Ŀ
    {
        for (int j = 0; j < size_vt; ++j) // ����ĳ���ս�� �ҿ��Բ���������Ŀ
        {
            buf_size = 0;
            for (int p = 0; p < size_proj[index]; ++p) // ������Ŀÿһ���ķ�
            {
                // ���С��û��ͷ �����±��Ƿ��ս��
                if (projs[index][p].now < length[projs[index][p].num] && gram[projs[index][p].num][projs[index][p].now] == Vt[j])
                {
                    tp.num = projs[index][p].num;
                    tp.search = projs[index][p].search;
                    tp.now = projs[index][p].now + 1; // tp ����һ����
                    buf[buf_size++] = tp;
                }
            }
            if (buf_size != 0) //����һ���µ���Ŀ��
            {
                num_proj++;
                for (int t = 0; t < buf_size; ++t) // ������ÿһ����Ū
                {
                    projs[num_proj][size_proj[num_proj]++] = buf[t];
                }
                getClosure(num_proj);
                nextt_state = isContained(); //����num_proj����Ŀ���Ƿ��ظ�
                if (nextt_state != 0)         //�ظ�����ת�������е��Ǹ���Ŀ����ȥ
                {
                    size_proj[num_proj--] = 0;
                    trans[size_trans].begin = index;
                    trans[size_trans].next = nextt_state;
                    trans[size_trans++].ch = Vt[j];
                }
                else // ���ظ�
                {
                    trans[size_trans].begin = index;
                    trans[size_trans].next = num_proj;
                    trans[size_trans++].ch = Vt[j];
                }
            }
        }

        for (int j = 0; j < size_vn; ++j) // ����ĳ�����ս�� �ҿ��Բ���������Ŀ
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
            if (buf_size != 0) //����һ���µ���Ŀ��
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
    for (int i = 0; i <= num_proj; ++i) // д
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
    for (int i = 0; i <= num_proj; ++i) // ����ÿһ����Ŀ
    {
        for (int j = 0; j < size_proj[i]; ++j) // ������Ŀ��ÿһ�ķ�
        {
            if (projs[i][j].now == length[projs[i][j].num] ||
                (projs[i][j].now == 1 && length[projs[i][j].num] == 2 && gram[projs[i][j].num][1] == '@'))
            {
                //����Ŀ A��a?,a ����Ik �����aΪ�κ��ս����#������ACTION[k,a] = rj ��jΪ����ʽ���
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
    if (x == 1) //״̬
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
    if (x == 2) //����
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
    procedure_file <<setw(37)<<setiosflags(ios::left)<< "����ջ";
    procedure_file <<setw(93)<<setiosflags(ios::left)<< "״̬ջ";
    procedure_file <<setw(80)<<setiosflags(ios::left)<< "���봮";
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
        for (int i = 0; i < size_ag_table[now_state]; ++i){ // ���ڸ�״̬�ɽ��ܵ��ַ�
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
                else if (ac_go_table[now_state][i].next_state > 0) // �ƽ�
                {
                    procedure_file << 'S' << ac_go_table[now_state][i].next_state << endl;

                    state_stack.push(ac_go_table[now_state][i].next_state);
                    sign_stack.push(ac_go_table[now_state][i].ch);
                    ++now_index;
                    break;
                }
                else if (ac_go_table[now_state][i].next_state < 0) // ��Լ
                {
                    int tp = ac_go_table[now_state][i].next_state * (-1);
                    procedure_file << 'r' << tp << "              ";
                    int len = length[tp] - 1;
                    if (gram[tp][1] == '@')
                        --len;
                    for (int k = 0; k < len; ++k) // �ķ��м����ַ���ȥ������״̬
                    {
                        state_stack.pop();
                        sign_stack.pop();
                    }
                    sign_stack.push(gram[tp][0]); // ��һ�����ս����ȥ
                    int aa = state_stack.top();
                    for (int k = 0; k < size_ag_table[aa]; ++k)
                    {
                        if (ac_go_table[aa][k].ch == gram[tp][0]) // ����
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
        cout << "����ԭ�������δ�ҵ���";
        for (int i = 0; i < size_ag_table[now_state]; ++i)
        {
            cout << ac_go_table[now_state][i].ch << " ";
        }
        cout << endl;
    }
    cout << endl
		 << "-----------�ļ�д�����-----------" << endl << endl;
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
