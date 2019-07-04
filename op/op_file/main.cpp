#include "head.h"
char		choice;
vector<string>vc_of_str;
string  s1, s2;//s1 s2�������������к��ļ����������
int		inum_cur;		// ��ǰĿ¼
char		temp[2 * BLKSIZE];	// ������
User		user;		// ��ǰ���û�
char		bitmap[BLKNUM];	// λͼ����
Inode	inode_array[INODENUM];	// i�ڵ�����
File_table file_array[FILENUM];	// ���ļ�������
char	image_name[10] = "hd.dat";	// �ļ�ϵͳ����
FILE* fp;					// ���ļ�ָ��


const string Commands[] = { "help", "cd", "ls", "mkdir", "touch", "open","cat", "vi", "close", "rm", "su", "clear", "format","exit","rmdir","df" };

//���ݵ�ǰĿ¼�͵ڶ�������ȷ��ת��ȥ��Ŀ¼
int readby() {
	int result_cur = 0; string s = s2;
	if (s.find('/') != -1) {
		s = s.substr(0, s.find_last_of('/') + 1);
	}
	else {
		s = "";
	}
	int temp_cur = inum_cur;
	vector<string>v;
	while (s.find('/') != -1) {
		v.push_back(s.substr(0, s.find_first_of('/')));
		s = s.substr(s.find_first_of('/') + 1);
	}
	if (v.size() == 0) {
		return inum_cur;
	}if (v[0].length() == 0) {
		temp_cur = 0;
	}
	else if (v[0] == "..") {
		if (inode_array[inum_cur].inum > 0) {
			temp_cur = inode_array[inum_cur].iparent;
		}
	}
	else {
		int i;
		for (i = 0; i < INODENUM; i++) {
			if ((inode_array[i].inum > 0) &&
				(inode_array[i].iparent == inum_cur) &&
				(inode_array[i].type == 'd') &&
				inode_array[i].file_name == v[0]) {
				break;
			}
		}
		if (i == INODENUM) {
			return -1;
		}
		else {
			temp_cur = i;
		}
	}
	int i;
	for (unsigned int count = 1; count < v.size(); count++) {
		for (i = 0; i < INODENUM; i++) {
			if ((inode_array[i].inum > 0) &&//�Ƿ�Ϊ��
				(inode_array[i].iparent == temp_cur) &&
				(inode_array[i].type == 'd') &&
				inode_array[i].file_name == v[count]) {
				break;
			}
		}
		if (i == INODENUM) {
			return -1;
		}
		else {
			temp_cur = i;
		}
	}
	result_cur = temp_cur;
	return result_cur;
}

//����ӳ��hd�����������û����ļ����
void format(void)
{
	int   i;
	Inode inode; //�ļ����
	printf("FileSystem will be  format \n");
	printf("All the data will lost!!!\n");
	printf("Are you sure format the fileSystem?(Y/N)?");
	cin >> choice;
	gets_s(temp, 1024);   //                                          ����
	if ((choice == 'y') || (choice == 'Y'))
	{
		fp = fopen(image_name, "w+b");
		if (fp == NULL)
		{
			printf("Can't open  file %s\n", image_name);
			//getchar();
			exit(-1);
		}
		//G44 M171 M196 M47
		//�ɹ��򿪺�λͼ������0
		for (i = 0; i < BLKSIZE; i++)
			fputc('0', fp);
		inode.inum = 0;
		inode.iparent = 0;
		inode.length = 0;
		inode.address[0] = -1;
		inode.address[1] = -1;
		inode.type = 'd';
		strcpy(inode.file_name, "/");
		strcpy(inode.user_name, "all");
		//�������д��
		fwrite(&inode, sizeof(Inode), 1, fp);
		inode.inum = -1;
		//��32���ڵ���и�ʽ��
		for (i = 0; i < 31; i++)
			fwrite(&inode, sizeof(Inode), 1, fp);
		//���ݿ���и�ʽ��
		for (i = 0; i < BLKNUM * BLKSIZE; i++)
			fputc('\0', fp);
		fclose(fp);
		//����û���Ϣ
		fp = fopen("user.txt", "w+");
		if (fp == NULL)
		{
			printf("Can't create file %s\n", "user.txt");
			exit(-1);
		}
		fclose(fp);
		printf("Filesystem created successful.Please first login!\n");
	}
	return;
}


/*����: �û���½����������û��򴴽��û�*/
void login() {
	char* p;
	int  flag;
	char user_name[10];
	char password[10];
	char file_name[10] = "user.txt";
	char choice;    //ѡ���Ƿ�y/n��
	do {
		printf("login:");
		gets_s(user_name);
		printf("password:");
		p = password;
		while (*p = _getch()) {
			if (*p == 0x0d) //������س���ʱ��0x0dΪ�س�����ASCII��
			{
				*p = '\0'; //������Ļس���ת���ɿո�
				break;
			}
			printf("*");   //�������������"*"����ʾ
			p++;
		}
		flag = 0;
		//user�ļ�������/�򲻿�
		//�ȸ�ʽ��
		if ((fp = fopen(file_name, "r+")) == NULL) {
			printf("\nCan't open file %s.\n", file_name);
			printf("This filesystem is not exist now, it will be create~~~\n");
			format();
			//�����û��������½����¼����
			login();
		}
		//�ܴ�user�ж��û��Ƿ����
		while (!feof(fp)) {
			fread(&user, sizeof(User), 1, fp);
			// �Ѿ����ڵ��û�, ��������ȷ
			if (!strcmp(user.user_name, user_name) &&
				!strcmp(user.password, password)) {
				fclose(fp);
				printf("\n");
				return;     //��½�ɹ���ֱ��������½���� 
			}
			// �Ѿ����ڵ��û�, ���������
			else if (!strcmp(user.user_name, user_name)) {
				printf("\nThis user is exist, but password is incorrect.\n");
				flag = 1;    //����flagΪ1����ʾ����������µ�½ 
				fclose(fp);
				break;
			}
		}
		if (flag == 0) {
			printf("\nThis user is not exist.\n");
			break;    //�û������ڣ�������ѭ���������û� 
		}
	} while (flag);

	// �������û�
	if (flag == 0) {
		printf("\nDo you want to creat a new user?(y/n):");
		scanf("%c", &choice);
		//gets_s(temp);
		if ((choice == 'y') || (choice == 'Y')) {
			strcpy(user.user_name, user_name);
			strcpy(user.password, password);
			fwrite(&user, sizeof(User), 1, fp);
			fclose(fp);
			return;
		}
		if ((choice == 'n') || (choice == 'N'))
			login();
	}
}


// ����: ������i�ڵ�����ڴ�

void init(void)
{
	int   i;
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s.\n", image_name);
		exit(-1);
	}
	// ����λͼ
	for (i = 0; i < BLKNUM; i++)
		bitmap[i] = fgetc(fp);
	// ��ʾλͼ
	// ����i�ڵ���Ϣ
	for (i = 0; i < INODENUM; i++)
		fread(&inode_array[i], sizeof(Inode), 1, fp);
	// ��ʾi�ڵ�
	// ��ǰĿ¼Ϊ��Ŀ¼
	inum_cur = 0;
	// ��ʼ�����ļ���
	for (i = 0; i < FILENUM; i++)
		file_array[i].inum = -1;
}

void df() {
	init();
	int count_inode = 0, count_mem = 0;
	for (int i = 0; i < INODENUM; i++) {
		if (inode_array[i].inum > 0)count_inode++;
	}
	for (int i = 0; i < BLKNUM; i++) {
		if (bitmap[i] == '1')count_mem++;
	}
	cout << "Inode use:" << ++count_inode << "/" << INODENUM << endl;
	for (int i = 0; i <count_inode; i++)
		cout << inode_array[i].user_name<<"		" << inode_array[i].inum << "	 " << inode_array[i].file_name <<"	 "<< inode_array[i].iparent<<"	 " << inode_array[i].address[0]
		<<"."<<inode_array[i].address[1] << endl;
	cout << "Store use:" << count_mem << "/" << BLKNUM << endl;
}

void StrListForCom() {
	vc_of_str.clear();
	vc_of_str.push_back("cd");
	vc_of_str.push_back("mkdir");
	vc_of_str.push_back("ls");
	vc_of_str.push_back("vi");
	vc_of_str.push_back("sudo");
	vc_of_str.push_back("rm");
	vc_of_str.push_back("touch");
	vc_of_str.push_back("help");
	vc_of_str.push_back("cat");
	vc_of_str.push_back("clear");
	vc_of_str.push_back("rmdir");
	vc_of_str.push_back("df");
}

void StrListForAdd() {
	vc_of_str.clear();
	int temp_cur;
	temp_cur = readby();
	for (int i = 0; i < INODENUM; i++) {
		if ((inode_array[i].inum > 0) &&
			(inode_array[i].iparent == temp_cur)) {
			if (inode_array[i].type == 'd' && !strcmp(inode_array[i].user_name, user.user_name))
			{
				string temp = inode_array[i].file_name;
				temp += '/';
				vc_of_str.push_back(temp);
			}
			if (inode_array[i].type == '-' && !strcmp(inode_array[i].user_name, user.user_name))
			{
				vc_of_str.push_back(inode_array[i].file_name);
			}
		}

	}
}

// ���: 0-14Ϊϵͳ����, 15Ϊ�������
int analyse()
{
	string s = "";
	s1 = ""; 
	s2 = "";
	int tabcount = 0;
	int res = 0;
	while (1) {
		s1 = s;
		if (s.find(' ') == -1)
			s2 = "";
		else {
			while (s1.length() > 0 && s1[s1.length() - 1] == ' ') {
				s1 = s1.substr(0, s1.length() - 1);
			}
			while (s1.length() > 0 && s1[0] == ' ') {
				s1 = s1.substr(1);
			}
			if (s1.find(' ') == -1)
				s2 = "";
			else
				s2 = s1.substr(s1.find_first_of(' ') + 1);
			while (s2.length() > 0 && s2[s2.length() - 1] == ' ') {
				s2 = s2.substr(0, s2.length() - 1);
			}
			while (s2.length() > 0 && s2[0] == ' ') {
				s2 = s2.substr(1);
			}
			s1 = s1.substr(0, s1.find_first_of(' '));
		}

		int ch = _getch();
		if (ch == 8) {				//�˸�
			if (!s.empty()) {
				printf("%c", 8);
				printf(" ");
				printf("%c", 8);
				s = s.substr(0, s.length() - 1);
			}
		}
		else if (ch == 13) {		//�س�
			for (res = 0; res < 16; res++) {
				if (s1 == Commands[res])break;
			}
			break;
		}
		/*else if (ch == 9) {			//tab
			int count = 0; vector<int>v;
			string tstr;
			if (s.find(' ') != -1) {
				tstr = s.substr(s.find_last_of(' ') + 1);
				if (tstr.find('/') != -1) {
					tstr = tstr.substr(tstr.find_last_of('/') + 1);
				}
				StrListForAdd();
			}
			else {
				tstr = s;
				StrListForCom();
			}
			for (unsigned int i = 0; i < vc_of_str.size(); i++) {
				if (vc_of_str[i].length() >= tstr.length() && vc_of_str[i].substr(0, tstr.length()) == tstr) {
					count++; v.push_back(i);
				}
			}
			cout << "count:" << count<<endl;
			cout << "tstr:" << tstr<<endl;
			if (count < 1) {
				if (s.find(' ') == -1) {
					s.push_back(' ');
					printf(" ");
				}
				tabcount = -1;
			}
			if (count == 1) {
				for (unsigned int i = tstr.length(); i < vc_of_str[v[0]].length(); i++) {
					s.push_back(vc_of_str[v[0]][i]);
					printf("%c", vc_of_str[v[0]][i]);
				}
				if (s.find(' ') == -1) {
					s.push_back(' ');
					printf(" ");
				}
				tabcount = -1;
			}
			if (count > 1 && tabcount) {
				cout << "\n";
				cout << vc_of_str[v[0]];
				for (unsigned int i = 1; i < v.size(); i++) {
					cout << "    " << vc_of_str[v[i]];
				}
				cout << endl;
				pathset();
				cout << s;
				tabcount = -1;
			}

		}*/
		else if (ch == ' ') {
			printf("%c", ch);
			s.push_back(ch);
		}
		else {
			printf("%c", ch);
			s.push_back(ch);
		}
		//���ڴ�������tab
		/*if (ch == 9) {
			tabcount++;
		}
		else {
			tabcount = 0;
		}*/
	}
	if (s1 == "") {
		return -1;
	}
	printf("\n");
	return res;
}


// ����: ��num��i�ڵ㱣�浽hd.dat
void save_inode(int num)
{
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s\n", image_name);
		exit(-1);
	}
	fseek(fp, 512 + num * sizeof(Inode), SEEK_SET);
	fwrite(&inode_array[num], sizeof(Inode), 1, fp);
	fclose(fp);

}

// ����: ����һ�����ݿ�
int get_blknum(void)
{
	int i;
	for (i = 0; i < BLKNUM; i++)
		if (bitmap[i] == '0') break;
	// δ�ҵ��������ݿ�
	if (i == BLKNUM)
	{
		printf("Data area is full.\n");
		exit(-1);
	}
	bitmap[i] = '1';
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s\n", image_name);
		exit(-1);
	}
	fseek(fp, i, SEEK_SET);//λͼ
	fputc('1', fp);
	fclose(fp);
	return i;
}

// ����: ��i�ڵ��Ϊnum���ļ�����temp 
void read_blk(int num)
{
	int  i, len;
	char ch;
	int  add0, add1;
	len = inode_array[num].length;
	add0 = inode_array[num].address[0];
	if (len > 512)
		add1 = inode_array[num].address[1];
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s.\n", image_name);
		exit(-1);
	}
	fseek(fp, 1536 + add0 * BLKSIZE, SEEK_SET);
	ch = fgetc(fp);
	for (i = 0; (i < len) && (ch != '\0') && (i < 512); i++)
	{
		temp[i] = ch;
		ch = fgetc(fp);
	}
	if (i >= 512)
	{
		fseek(fp, 1536 + add1 * BLKSIZE, SEEK_SET);
		ch = fgetc(fp);
		for (; (i < len) && (ch != '\0'); i++)
		{
			temp[i] = ch;
			ch = fgetc(fp);
		}
	}
	temp[i] = '\0';
	fclose(fp);
}

// ����: ��temp����������hd��������
void write_blk(int num)
{
	int  i, len;
	int  add0, add1;
	add0 = inode_array[num].address[0];
	len = inode_array[num].length;
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s.\n", image_name);
		exit(-1);
	}
	fseek(fp, 1536 + add0 * BLKSIZE, SEEK_SET);
	for (i = 0; (i < len) && (temp[i] != '\0') && (i < 512); i++)
		fputc(temp[i], fp);
	if (i == 512)
	{
		add1 = inode_array[num].address[1];
		fseek(fp, 1536 + add1 * BLKSIZE, SEEK_SET);
		for (; (i < len) && (temp[i] != '\0'); i++)
			fputc(temp[i], fp);
	}
	fputc('\0', fp);
	fclose(fp);
}

// ����: �ͷ��ļ����Ϊnum���ļ�ռ�õĿռ�
void release_blk(int num)
{
	FILE* fp;
	if ((fp = fopen(image_name, "r+b")) == NULL)
	{
		printf("Can't open file %s\n", image_name);
		exit(-1);
	}
	bitmap[num] = '0';
	fseek(fp, num, SEEK_SET);
	fputc('0', fp);
	fclose(fp);
}

void help() {
	/*����: ��ʾ��������*/
	printf("command: \n\
	help   ---  show help menu \n\
	cd     ---  change directory \n\
	clear  ---  clear the screen \n\
	ls     ---  show all the files and directories in particular directory \n\
	mkdir  ---  make a new directory   \n\
	touch  ---  create a new file \n\
	cat    ---  open and read an exist file \n\
	vi     ---  open and write something to a particular file \n\
	rm     ---  delete a exist file \n\
	su     ---  switch current user \n\
	clear  ---  clear the screen \n\
	format ---  format a exist filesystem \n\
	exit   ---  exit this system \n\
	rmdir  ---  delete a directory \n\
	df     ---  show the useage of storage \n");
}


//�����ļ�·��
void pathset()
{
	string s;
	if (inode_array[inum_cur].inum == 0)
		s = "";
	else {
		int temp = inum_cur;
		while (inode_array[temp].inum != 0) {
			s = inode_array[temp].file_name + s;
			s = '/' + s;
			temp = inode_array[temp].iparent;
		}
	}
	cout << user.user_name << "@" << "opfile" << ":~" << s << "# ";
}

// ����: �л�Ŀ¼(cd .. ���� cd dir1)
void cd()
{
	int temp_cur;
	if (s2.length() == 0) {
		temp_cur = 0;
	}
	else {  
		
		if (s2[0] == '/') {
			cout << "Input error!" << endl;
			command();
		}
		else if (s2[s2.length() - 1] != '/')
			s2 += '/';
		temp_cur = readby();
	}
	if (temp_cur != -1) {
		inum_cur = temp_cur;
	}
	else {
		cout << "No Such Directory" << endl;
	}
}


// ����: ��ʾ��ǰĿ¼�µ���Ŀ¼���ļ�(dir)
void dir(void)
{
	int temp_cur;
	int i = 0;
	if (s2.length() == 0) {
		temp_cur = inum_cur;
	}
	else {
		if (s2[s2.length() - 1] != '/')s2 += '/';
		temp_cur = readby();
		if (temp_cur == -1 || inode_array[temp_cur].type != 'd') {
			cout << "No Such Directory" << endl;
			return;
		}
	}
	if (temp_cur != -1 && inode_array[temp_cur].type == 'd')
		for (i = 0; i < INODENUM; i++)
		{
			if ((inode_array[i].inum > 0) &&
				(inode_array[i].iparent == temp_cur)
				&& !strcmp(inode_array[i].user_name, user.user_name))
			{
				if (inode_array[i].type == 'd')
				{
					printf("%-20s<DIR>\n", inode_array[i].file_name);
				}
				if (inode_array[i].type == '-')
				{
					printf("%-20s%12d bytes\n", inode_array[i].file_name, inode_array[i].length);
				}
			}
		}
}

void rm(int inode) {
	int i;
	for (i = 0; i < INODENUM; i++) {
		if (inode_array[i].iparent == inode && !strcmp(inode_array[i].user_name, user.user_name)) {
			if (inode_array[i].type == 'd')rm(i);
			else delet(i);
		}
	}
	delet(inode);
}

//rmdir dir��rmdir dir/a/b
void rmdir()
{
	if (s2.length() == 0) {
		printf("No Such Directory\n");
		return;
	}
	if (s2[s2.length() - 1] != '/')s2 += '/';
	int temp_cur = readby();
	if (temp_cur == -1)printf("No Such Directory\n");
	else {
		if (inode_array[temp_cur].type != 'd') {
			printf("That's A File!\n");
		}
		else {
			int temp = inode_array[inum_cur].iparent;
			while (temp != 0) {
				if (temp == temp_cur || temp_cur == 0) {
					printf("Can't delete your father directory\n");
					return;
				}
				temp = inode_array[temp].iparent;
			}
			rm(temp_cur);
		}
	}
	return;
}

// ����: �ڵ�ǰĿ¼�´�����Ŀ¼(mkdir dir1)
void mkdir(void)
{
	int i;
	if (s2.length() == 0) {
		cout << "Please input name" << endl;
		return;
	}
	// ����i�ڵ�����, ����δ�õ�i�ڵ�
	for (i = 0; i < INODENUM; i++)
	{
		if (inode_array[i].iparent == inum_cur && inode_array[i].type == 'd'
			&& inode_array[i].file_name == s2 && inode_array[i].inum > 0
			&& !strcmp(inode_array[i].user_name, user.user_name))
		{
			break;
		}
	}
	if (i != INODENUM) {
		printf("There is directory having same name.\n");
		return;
	}
	for (i = 0; i < INODENUM; i++)
		if (inode_array[i].inum < 0) break;
	if (i == INODENUM)
	{
		printf("Inode is full.\n");
		exit(-1);
	}
	inode_array[i].inum = i;
	strcpy(inode_array[i].file_name, s2.data());
	inode_array[i].type = 'd';
	strcpy(inode_array[i].user_name, user.user_name);
	inode_array[i].iparent = inum_cur;
	inode_array[i].length = 0;
	save_inode(i);
}

// ����: �ڵ�ǰĿ¼�´����ļ�(creat file1)
void touch(void)
{
	if (s2.length() == 0) {
		printf("Please input filename.\n");
		return;
	}
	int i, temp_cur; 
	string temps1, temps2;
	if (s2.find('/') != -1) {
		temps1 = s2.substr(0, s2.find_last_of('/') + 1);
		temps2 = s2.substr(s2.find_last_of('/') + 1);
		s2 = temps1;
		temp_cur = readby();
		if (temp_cur == -1) {
			printf("No Such Directory\n");
		}
	}
	else {
		temps2 = s2;
		temp_cur = inum_cur;
	}
	for (i = 0; i < INODENUM; i++)
		if ((inode_array[i].inum > 0) &&
			(inode_array[i].type == '-') &&
			temps2 == inode_array[i].file_name &&
			inode_array[i].iparent == temp_cur &&
			!strcmp(inode_array[i].user_name, user.user_name)) break;
	if (i != INODENUM) {
		printf("There is same file\n");
		return;
	}
	for (i = 0; i < INODENUM; i++)
		if (inode_array[i].inum < 0) break;
	if (i == INODENUM)
	{
		printf("Inode is full.\n");
		exit(-1);
	}
	inode_array[i].inum = i;
	strcpy(inode_array[i].file_name, temps2.data());
	inode_array[i].type = '-';
	strcpy(inode_array[i].user_name, user.user_name);
	inode_array[i].iparent = temp_cur;
	inode_array[i].length = 0;
	save_inode(i);
}

//��鵱ǰI�ڵ���ļ��Ƿ����ڵ�ǰ�û�
int check(int i)
{
	int j;
	char* uuser, * fuser;
	uuser = user.user_name;
	fuser = inode_array[i].user_name;
	j = strcmp(fuser, uuser);
	if (j == 0)  return 1;
	else      return 0;
}

void open(int mymode) {
	/*����: �򿪵�ǰĿ¼�µ��ļ�(open file1)*/
	int i, inum, mode, filenum, chk;
	if (s2.length() == 0) {
		printf("This file doesn't exist.\n");
		return;
	}
	int temp_cur; string temps1, temps2;
	if (s2.find('/') != -1) {
		temps1 = s2.substr(0, s2.find_last_of('/') + 1);
		temps2 = s2.substr(s2.find_last_of('/') + 1);
		string temps = s2;
		s2 = temps1;
		temp_cur = readby();
		s2 = temps;
		if (temp_cur == -1) {
			cout << "No Such Directory" << endl;
		}
	}
	else {
		temps2 = s2;
		temp_cur = inum_cur;
	}
	for (i = 0; i < INODENUM; i++)
		if ((inode_array[i].inum > 0) &&
			(inode_array[i].type == '-') &&
			temps2 == inode_array[i].file_name &&
			!strcmp(inode_array[i].user_name, user.user_name) &&
			inode_array[i].iparent == temp_cur)     //�ж��Ƿ��ڵ�ǰĿ¼�� 
			break;
	if (i == INODENUM) {
		cout << "This is no " + temps2 + " file...\n";
		return;
	}
	inum = i;
	chk = check(i);               //�����ļ��Ƿ�Ϊ��ǰ�û����ļ�
	mode = mymode;
	if (chk != 1) {
		printf("This file is not yours !\n");
		return;
	}
	if ((mode < 1) || (mode > 3)) {
		printf("Open mode is wrong.\n");
		return;
	}
	filenum = i;
	file_array[filenum].inum = inum;
	strcpy(file_array[filenum].file_name, inode_array[inum].file_name);
	file_array[filenum].mode = mode;
	file_array[filenum].offset = 0;
}

void cat() {
	int i, inum;
	open(1);
	string temps1, temps2; int temp_cur;
	if (s2.find('/') != -1) {
		temps1 = s2.substr(0, s2.find_last_of('/') + 1);
		temps2 = s2.substr(s2.find_last_of('/') + 1);
		string temps = s2;
		s2 = temps1;
		temp_cur = readby();
		s2 = temps;
	}
	else {
		temps2 = s2;
		temp_cur = inum_cur;
	}
	for (i = 0; i < FILENUM; i++)
		if ((file_array[i].inum > 0) &&
			(temps2 == file_array[i].file_name))
			break;
	inum = file_array[i].inum;
	if (inode_array[inum].length > 0) {
		read_blk(inum);
		for (i = 0; temp[i] != '\0'; i++)
			printf("%c", temp[i]);
		printf("\n");
	}
	//close();
}

void vi() {
	/*����: ���ļ���д���ַ�(write file1)*/
	int i, inum, length;
	open(3);
	for (i = 0; i < FILENUM; i++)
		if ((file_array[i].inum > 0) &&
			s2 == file_array[i].file_name) break;
	if (i == FILENUM) {
		cout << "Open " << s2 << " false.\n";
		return;
	}
	inum = file_array[i].inum;
	//printf("The length of %s:%d\n", inode_array[inum].file_name, inode_array[inum].length);
	if (inode_array[inum].length == 0) {
		printf("The length you want to write(0-1024):");
		cin >> length;
		if ((length < 0) || (length > 1024)) {
			printf("You can't creat a file which data less than 0 byte or more than 1024 bytes.'.\n");
			return;
		}
		gets_s(temp);
		inode_array[inum].length = length;
		inode_array[inum].address[0] = get_blknum();
		//cout << inode_array[inum].address[0] << "********************"<< endl;
		if (length > 512) {
			inode_array[inum].address[1] = get_blknum();
			//cout << inode_array[inum].address[1] << "********************" << endl;
		}
			
			
		save_inode(inum);
		printf("Input the data(Enter to end):\n");
		gets_s(temp);
		write_blk(inum);
	}
	else {
		printf("Are you sure to write this file? Data in it will all be deleted.(y/n):");
		scanf("%c", &choice);
		if ((choice == 'y') || (choice == 'Y')) {
			printf("The length you want to write(0-1024):");
			scanf("%d", &length);
			gets_s(temp);
			if ((length < 0) || (length > 1024)) {
				printf("You can't creat a file which data less than 0 byte or more than 1024 bytes.'.\n");
				return;
			}
			inode_array[inum].length = length;
			inode_array[inum].address[0] = get_blknum();
			if (length > 512)
				inode_array[inum].address[1] = get_blknum();
			save_inode(inum);
			printf("Input the data(Enter to end):\n");
			gets_s(temp);
			write_blk(inum);
		}
	}
	close();
	int temp_cur = inum_cur;
	init();   //��Ҫ��ʼ��һ�£������ļ���һ���ļ�������� 
	inum_cur = temp_cur;
}

void close() {
	/*����: �ر��Ѿ��򿪵��ļ�(close file1)*/
	int i;
	int temp_cur; string temps1, temps2;
	if (s2.find('/') != -1) {
		temps1 = s2.substr(0, s2.find_last_of('/') + 1);
		temps2 = s2.substr(s2.find_last_of('/') + 1);
		string temps = s2;
		s2 = temps1;
		temp_cur = readby();
		s2 = temps;
	}
	else {
		temps2 = s2;
		temp_cur = inum_cur;
	}
	for (i = 0; i < FILENUM; i++)
		if ((file_array[i].inum > 0) &&
			(temps2 == file_array[i].file_name)) break;
	if (i == FILENUM) {
		printf("This file didn't be opened.\n");
		return;
	}
	else
		file_array[i].inum = -1;
}

void su() {
	/*����: �л���ǰ�û�(logout)*/
	char* p;
	int flag;
	string user_name;
	char password[10];
	char file_name[10] = "user.txt";
	fp = fopen(file_name, "r");           //��ʼ��ָ�룬���ļ�ϵͳ��ָ��ָ���ļ�ϵͳ���׶�(��ֻ����ʽ���ļ�)
										  /*if (argc != 2){
										  printf("command su must have one object. \n");
										  return;								  }*/
	do {
		user_name = s2;
		printf("password:");
		p = password;
		while (*p = _getch()) {
			if (*p == 0x0d) { 		//������س���ʱ��0x0dΪ�س�����ASCII��
				*p = '\0'; 			//������Ļس���ת���ɿո�
				break;
			}
			printf("*");   //�������������"*"����ʾ
			p++;
		}
		flag = 0;
		while (!feof(fp)) {
			fread(&user, sizeof(User), 1, fp);
			// �Ѿ����ڵ��û�, ��������ȷ
			if ((user.user_name == user_name) &&
				!strcmp(user.password, password)) {
				fclose(fp);
				printf("\n");
				return;     //��½�ɹ���ֱ��������½���� 
			}
			// �Ѿ����ڵ��û�, ���������
			else if ((user.user_name == user_name)) {
				printf("\nThis user is exist, but password is incorrect.\n");
				flag = 1;    //����flagΪ1����ʾ����������µ�½ 
				fclose(fp);
				break;
			}
		}
		if (flag == 0) {
			printf("\nThis user is not exist.\n");
			break;     //�û������ڣ�ֱ������ѭ����������һ��ָ�������
		}
	} while (flag);
}

//����Inode�ڵ��ɾ�洢
void delet(int innum)
{

	inode_array[innum].inum = -1;
	if (inode_array[innum].length >= 0)
	{
		release_blk(inode_array[innum].address[0]);
		if (inode_array[innum].length >= 512)
			release_blk(inode_array[innum].address[1]);
	}
	//save_inode(innum);
}

// ����: ɾ���ļ�
void rmfile(void)
{
	if (s2.length() == 0) {
		printf("This file doesn't exist.\n");
		return;
	}
	int i, temp_cur; string temps1, temps2;
	if (s2.find('/') != -1) {
		temps1 = s2.substr(0, s2.find_last_of('/') + 1);
		temps2 = s2.substr(s2.find_last_of('/') + 1);
		s2 = temps1;
		temp_cur = readby();
	}
	else {
		temps2 = s2;
		temp_cur = inum_cur;
	}
	for (i = 0; i < INODENUM; i++)
		if ((inode_array[i].inum > 0) &&
			(inode_array[i].type == '-') &&
			temps2 == inode_array[i].file_name &&
			inode_array[i].iparent == temp_cur &&
			!strcmp(inode_array[i].user_name, user.user_name)) break;
	if (i == INODENUM)
	{
		printf("This file doesn't exist.\n");
		return;
	}
	delet(i);
}

// ����: �˳���ǰ�û�(logout)
void logout()
{
	login();
}

// ����: �˳��ļ�ϵͳ(quit)
void quit()
{
	char choice;
	printf("Do you want to exist(y/n):");
	cin >> choice;
	gets_s(temp);
	if ((choice == 'y') || (choice == 'Y'))
		exit(-1);
}

// ����: ��ʾ����
void errcmd()
{
	printf("Command Error!!!\n");
}

//����ڴ��д��ڵ��û���
void free_user()
{
	int i;
	for (i = 0; i < 10; i++)
		user.user_name[i] = '\0';
}
// ����: ѭ��ִ���û����������, ֱ��logout
// "help", "cd", "dir", "mkdir", "touch", "open","read", "write", "close", "delete", "logout", "clear", "format","quit","rd"

void command(void)
{
	system("cls");
	do
	{
		pathset();
		switch (analyse())
		{
		case -1:
			printf("\n");
			break;
		case 0:
			help();
			break;
		case 1:
			cd();
			break;
		case 2:
			dir();
			break;
		case 3:
			mkdir();
			break;
		case 4:
			touch();
			break;
		case 5:
			//open();
			break;
		case 6:
			cat();
			break;
		case 7:
			vi();
			break;
		case 8:
			//close();
			break;
		case 9:
			rmfile();
			break;
		case 10:
			su();
			break;
		case 11:
			system("cls");
			break;
		case 12:
			format();
			init();
			free_user();
			login();
			break;
		case 13:
			quit();
			break;
		case 14:
			rmdir();
			break;
		case 15:
			df();
			break;
		case 16:
			errcmd();
			break;
		default:
			break;
		}
	} while (1);
}


// ������
int main(void)
{
	login();
	init();
	command();
	return 0;
}
