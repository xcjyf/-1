#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <windows.h>
#define APPEAL_FILE "appeals.dat"
typedef struct User {
	char username[20];
	char password[20];
	int type;               // 0: 学生, 1: 老师
	long long student_id;
} User;
User current_user;
typedef struct Student {
	long long number;
	char name[20];
	char password[20];
	float chinese;
	float math;
	float english;
	float sum;
}Student;
typedef struct Node {
	Student stu;
	struct Node* next;
}Node;
typedef struct Appeal {
	int id;                  // 申诉唯一ID
	long long student_id;    // 提交申诉的学生学号
	char student_name[10];   // 学生姓名（方便显示）
	int type;                // 0: 成绩申诉, 1: 密码找回
	char content[200];       // 申诉内容
	char reply[200];         // 管理员回复内容
	int status;              // 0: 待处理, 1: 已处理
	time_t submit_time;      // 提交时间
	time_t reply_time;       // 回复时间
	struct Appeal* next;
} Appeal;
Appeal* appealListHead = NULL;  // 全局申诉链表头
int nextAppealId = 1;            // 下一个申诉ID
int teacherMenu() {
	printf("********************************************\n");
	printf("*            欢迎使用教师端管理系统        *\n");
	printf("********************************************\n");
	printf("*                  请选择功能              *\n");
	printf("********************************************\n");
	printf("*                1,录入学生信息            *\n");
	printf("*                2,打印学生信息            *\n");
	printf("*                3,保存学生信息            *\n");
	printf("*                4,读取学生信息            *\n");
	printf("*                5,统计学生信息            *\n");
	printf("*                6,查找学生信息            *\n");
	printf("*                7,修改学生信息            *\n");
	printf("*                8,删除学生信息            *\n");
	printf("*                9,排序学生信息            *\n");
	printf("*                10,导出学生信息           *\n");
	printf("*                11,退出登录               *\n");
	printf("********************************************\n");
	int select = -1;
	printf("select>");
	scanf("%d", &select);
	return select;
}
int login(Node* head) {
	int choice;
	char username[20], password[20];
	printf("********************************************\n");
	printf("*                请登陆系统                *\n");
	printf("********************************************\n");
	printf("*                1,教师登录                *\n");
	printf("*                2,学生登录                *\n");
	printf("*                3,管理员登录              *\n");
	printf("*                4,退出程序                *\n");
	printf("********************************************\n");
	printf("select> ");
	scanf("%d", &choice);
	if (choice == 4){
		return 0;
	}
	printf("账号>");
	scanf("%s", username);
	printf("密码>");
	scanf("%s", password);
	if (choice == 1) {
		FILE* fp = fopen("teacher.data", "r");
		if (!fp) {
			printf("新账号注册,已自动登录\n");
			fp = fopen("teacher.data", "w");
			if (!fp) {
				printf("无法创建教师账号文件\n");
				return 1;
			}
			fprintf(fp, "%s %s\n", username, password);
			fclose(fp);
			printf("教师账号创建成功！\n");
			strcpy(current_user.username, username);
			current_user.type = 1;
			return 1;
		} else {
			char file_user[20], file_pass[20];
			if (fscanf(fp, "%s %s", file_user, file_pass) != 2) {
				printf("教师账号文件格式错误！\n");
				fclose(fp);
				return 0;
			}
			fclose(fp);
			if (strcmp(username, file_user) == 0 && strcmp(password, file_pass) == 0) {
				strcpy(current_user.username, username);
				current_user.type = 1;
				printf("教师登录成功！\n");
				return 1;
			}
			else {
				printf("用户名或密码错误！\n");
				return 0;
			}
		}
	} else if (choice == 2) {
		long long stu_num = atoll(username);   // 将字符串学号转为整数
		Node* p = head;
		while (p != NULL) {
			if (p->stu.number == stu_num && strcmp(p->stu.password, password) == 0) {
				strcpy(current_user.username, username);
				current_user.type = 0;
				current_user.student_id = stu_num;
				printf("学生登录成功！\n");
				return 2;
			}
			p = p->next;
		}
		printf("学号或密码错误！\n");
		return 0;
	}
	else if (choice == 3) {
		FILE* fp = fopen("admin.data", "r");
		if (!fp) {
			printf("未检测到管理员账号，正在自动创建新账号...\n");
			fp = fopen("admin.data", "w");
			if (!fp) {
				printf("无法创建管理员账号文件！\n");
				return 0;
			}
			fprintf(fp, "%s %s\n", username, password);
			fclose(fp);
			printf("管理员账号创建成功！\n");
			strcpy(current_user.username, username);
			current_user.type = 2;   // 管理员类型
			return 3;                 // 返回管理员登录成功标识
		}
		else {
			char file_user[20], file_pass[20];
			if (fscanf(fp, "%s %s", file_user, file_pass) != 2) {
				printf("管理员账号文件格式错误！\n");
				fclose(fp);
				return 0;
			}
			fclose(fp);
			if (strcmp(username, file_user) == 0 && strcmp(password, file_pass) == 0) {
				strcpy(current_user.username, username);
				current_user.type = 2;
				printf("管理员登录成功！\n");
				return 3;
			}
			else {
				printf("用户名或密码错误！\n");
				return 0;
			}
		}
	}
	return 0;
}
Node* creatNode() {
	Node* node = (Node*)malloc(sizeof(Node));
	if (!node) {
		printf("malloc failed\n");
		return NULL;
	}
	node->next = NULL;
	return node;
}
void entryStudent(Node** head) {
	Node* node = creatNode();
	printf("输入学生学号>");
	scanf("%lld", &node->stu.number);
	printf("输入学生姓名>");
	scanf("%s", node->stu.name);
	printf("输入初始密码>");
	scanf("%s", node->stu.password);
	printf("输入学生语文成绩>");
	scanf("%f", &node->stu.chinese);
	printf("输入学生数学成绩>");
	scanf("%f", &node->stu.math);
	printf("输入学生英语成绩>");
	scanf("%f", &node->stu.english);
	if (*head == NULL) {
		*head = node;
	} else {
		Node* current = *head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = node;
	}
	printf("录入成功");
}
void printStudent(Node* head) {
	printf("*****************************************************\n");
	printf("* 学号  *  姓名  *  语文  *  数学  *  英语 *  总分  *\n");
	printf("*****************************************************\n");
	Node* current = head;
	while (current != NULL) {
		printf("* %lld  *  %s  *  %.1f  *  %.1f  *  %.1f *  %.1f\n", current->stu.number, current->stu.name, current->stu.chinese, current->stu.math, current->stu.english, current->stu.chinese + current->stu.math + current->stu.english);
		current = current->next;
	}
}
void saveStudent(Node* head) {
	FILE* fp = fopen("students.data", "wb");
	if (!fp) {
		perror("file open failed");
		return;
	}
	Node* currentNode = head;
	while (currentNode != NULL) {
		fwrite(&currentNode->stu, sizeof(Student), 1, fp);
		currentNode = currentNode->next;
	}
	printf("保存成功\n");
	fclose(fp);
}
void readStudent(Node** head) {
	FILE* fp = fopen("students.data", "rb");
	if (!fp) {
		perror("file open failed");
		return;
	}
	while (*head != NULL) {
		Node* temp = *head;
		*head = (*head)->next;
		free(temp);
	}
	Node* tail = NULL;
	while (!feof(fp)) { //检测是否读完
		Node* node = creatNode();
		if (!node) {
			break;
		}
		size_t len = fread(&node->stu, sizeof(Student), 1, fp);
		if (len == 0) {
			free(node);
			break;
		}
		if (*head == NULL) {
			*head = node;
			tail = node;
		}
		else {
			tail->next = node;
			tail = node;
		}
	}
	printf("已读取\n");
	fclose(fp);
}
void countStudent(Node* head) {
	Node* current = head;
	int count = 0;
	while (current != NULL) {
		current = current->next;
		count++;
	}
	if (count == 0) {
		printf("无学生数据\n");
		return;
	} else {
		printf("共有学生%d人\n", count);
	}
	current = head;
	int chineseSum = 0;
	int mathSum = 0;
	int englishSum = 0;
	while (current != NULL) {
		chineseSum += current->stu.chinese;
		mathSum += current->stu.math;
		englishSum += current->stu.english;
		current = current->next;
	}
	float chineseAve = (float)chineseSum / count;
	float mathAve = (float)mathSum / count;
	float englishAve = (float)englishSum / count;
	printf("语文平均分为%.1f\n", chineseAve);
	printf("数学平均分为%.1f\n", mathAve);
	printf("英语平均分为%.1f\n", englishAve);
}
void findStudent(Node* head) {
	if (head == NULL) {
		printf("学生列表为空！\n");
		return;
	}
	printf("请输入学生学号\n");
	long long stunum;
	scanf("%lld", &stunum);
	Node* current = head;
	while (current != NULL) {
		if (stunum == current->stu.number) {
			printf("* %lld  *  %s  *  %.1f  *  %.1f  *  %.1f *\n", current->stu.number, current->stu.name, current->stu.chinese, current->stu.math, current->stu.english);
			return;
		}
		current = current->next;
	}
	printf("未找到该学生");
	return;
}
void changeStudent(Node* head) {
	printf("请输入要修改学生学号\n");
	long long stunum;
	scanf("%lld", &stunum);
	Node* current = head;
	while (current != NULL) {
		if (stunum == current->stu.number) {
			printf("请输入要修改学生的语文成绩 数学成绩 英语成绩>");
			scanf("%f%f%f", &current->stu.chinese, &current->stu.math, &current->stu.english);
			printf("修改成功\n");
		}
		current = current->next;
	}
	return;
}
Node* deleteStudent(Node* head) {
	if (head == NULL) {
		printf("学生列表为空\n");
		return NULL;
	}
	long long num;
	printf("请输入要删除学生的学号");
	scanf("%lld", &num);
	if (head->stu.number == num) {
		printf("已找到学生: %lld %s\n", head->stu.number, head->stu.name);
		printf("确认删除? (y/n)>");

		// 清除输入缓冲区
		getchar();
		char a;
		scanf("%c", &a);
		if (a == 'y' || a == 'Y') {
			Node* temp = head;
			head= head->next;
			free(temp);
			printf("删除成功\n");
			return head;  //返回新的头指针
		}
		else {
			printf("取消删除\n");
			return head;
		}
	}
	Node* current = head->next;
	Node* prev = head;
	while (current != NULL) {
		if (current->stu.number == num) {
			printf("已找到学生:%d %s\n", current->stu.number, current->stu.name);
			printf("确认删除? (y / n)>");
			getchar();
			char a;
			scanf("%c", &a);
			if (a == 'y' || a == 'Y') {
				prev->next = current->next;
				free(current);
				printf("删除成功\n");
			} else {
				printf("取消删除\n");
			}
			return;
		}
		current = current->next;
		prev = prev->next;
	}
	printf("未找到该学号相对应的学生\n");
	return head;
}
int shouldSwap(Student* a, Student* b, int sort_by, int order) {
	float sum_a, sum_b;
	switch (sort_by) {
		case 0:
			sum_a = a->chinese + a->math + a->english;
			sum_b = b->chinese + b->math + b->english;
			break;
		case 1:
			sum_a = a->chinese;
			sum_b = b->chinese;
			break;
		case 2:
			sum_a = a->math;
			sum_b = b->math; 
			break;
		case 3: 
			sum_a = a->english; 
			sum_b = b->english; 
			break;
		default:
			return 0;
	}
	if (order == 0) {
		return sum_a < sum_b;
	} else {
		return sum_a > sum_b;
	}
}
Node* quickSort(Node* head, int sort_by, int order) {
	if (head == NULL || head->next == NULL) {
		return head;
	}
	Node* pivot = head;
	Node* current = head->next;
	pivot->next = NULL;
	Node* leftHead = NULL, * leftTail = NULL;
	Node* rightHead = NULL, * rightTail = NULL;
	while (current != NULL) {
		Node* next = current->next;
		current->next = NULL;
		if (shouldSwap(&(current->stu), &(pivot->stu), sort_by, order)) {
			if (leftHead == NULL) {
				leftHead = leftTail = current;
			}
			else {
				leftTail->next = current;
				leftTail = current;
			}
		}
		else {
			if (rightHead == NULL) {
				rightHead = rightTail = current;
			}
			else {
				rightTail->next = current;
				rightTail = current;
			}
		}
		current = next;
	}
	leftHead = quickSort(leftHead, sort_by, order);
	rightHead = quickSort(rightHead, sort_by, order);
	Node* newHead = NULL;
	if (leftHead != NULL) {
		newHead = leftHead;
		Node* leftTail = leftHead;
		while (leftTail->next != NULL) {
			leftTail = leftTail->next;
		}
		leftTail->next = pivot;
	}
	else {
		newHead = pivot;
	}
	pivot->next = rightHead;
	return newHead;
}
void exportStudentHuman(Node* head, const char* filename) {
	FILE* fp = fopen(filename, "w");
	if (!fp) {
		perror("无法创建文件");
		return;
	}
	Node* current = head;
	while (current != NULL) {
		fprintf(fp, "%lld\t%s\t%s\t%.1f\t%.1f\t%.1f\n",
			current->stu.number,
			current->stu.name,
			current->stu.password,
			current->stu.chinese,
			current->stu.math,
			current->stu.english);
		current = current->next;
	}
	printf("成功导出文本文件: %s\n", filename);
	fclose(fp);
}
int studentMenu() {
	printf("********************************************\n");
	printf("*             欢迎使用学生管理系统         *\n");
	printf("********************************************\n");
	printf("*                  请选择身份              *\n");
	printf("********************************************\n");
	printf("*                1,查看信息                *\n");
	printf("*                2,修改密码                *\n");
	printf("*                3,提交申诉                *\n");
	printf("*                4,查看申诉记录            *\n");
	printf("*                5,退出登录                *\n");
	printf("********************************************\n");
	int select = -1;
	printf("select>");
	scanf("%d", &select);
	return select;
}
void changeStudentPassword(Node* head, long long student_id) {
	Node* current = head;
	while (current != NULL) {
		if (current->stu.number == student_id) {
			char oldPassword[20], newPassword[20];
			printf("请输入旧密码\n");
			scanf("%s", oldPassword);
			if (strcmp(current->stu.password, oldPassword) != 0) {
				printf("密码错误\n");
				return;
			}
			printf("请输入新密码\n");
			scanf("%s", newPassword);
			char confirmPassword[20];
			printf("请输入新密码\n");
			scanf("%s", confirmPassword);
			if (strcmp(newPassword, confirmPassword) == 0) {
				strcpy(current->stu.password, newPassword);
				printf("密码修改成功！\n");
				saveStudent(head);
			} else {
				printf("两次输入的密码不一致\n");
			}
			return;
		}
		current = current->next;
	}
	printf("未找到学生信息\n");
}
void viewStudent(Node* head, long long student_id) {
	Node* current = head;
	Node* target = NULL;
	while (current != NULL) {
		if (current->stu.number == student_id) {
			target = current;
			break;
		}
		current = current->next;
	}
	if (target == NULL) {
		printf("未找到学生信息\n");
		return;
	}
	float total = target->stu.chinese + target->stu.math + target->stu.english;
	float chinese = target->stu.chinese;
	float math = target->stu.math;
	float english = target->stu.english;
	int rank_total = 1, rank_chinese = 1, rank_math = 1, rank_english = 1;
	Node* p = head;
	while (p != NULL) {
		if (p == target) {  // 跳过自己
			p = p->next;
			continue;
		}
		float p_total = p->stu.chinese + p->stu.math + p->stu.english;
		if (p_total > total) rank_total++;
		if (p->stu.chinese > chinese) rank_chinese++;
		if (p->stu.math > math) rank_math++;
		if (p->stu.english > english) rank_english++;
		p = p->next;
	}
	printf("学号: %lld\n", target->stu.number);
	printf("姓名: %s\n", target->stu.name);
	printf("语文成绩: %.1f (班级排名: %d)\n", chinese, rank_chinese);
	printf("数学成绩: %.1f (班级排名: %d)\n", math, rank_math);
	printf("英语成绩: %.1f (班级排名: %d)\n", english, rank_english);
	printf("总分: %.1f (班级排名: %d)\n", total, rank_total);
}
void saveAppeals() {
	FILE* fp = fopen(APPEAL_FILE, "wb");
	if (!fp) return;
	Appeal* p = appealListHead;
	while (p) {
		fwrite(p, sizeof(Appeal), 1, fp);
		p = p->next;
	}
	fclose(fp);
}
void submitAppeal(Node* head, long long student_id) {
	// 先找到学生姓名
	Node* p = head;
	char stu_name[10] = "";
	while (p) {
		if (p->stu.number == student_id) {
			strcpy(stu_name, p->stu.name);
			break;
		}
		p = p->next;
	}
	if (strlen(stu_name) == 0) {
		printf("未找到您的信息！\n");
		return;
	}
	Appeal* a = (Appeal*)malloc(sizeof(Appeal));
	a->id = nextAppealId++;
	a->student_id = student_id;
	strcpy(a->student_name, stu_name);
	printf("请选择申诉类型：0-成绩申诉，1-密码找回\n");
	scanf("%d", &a->type);
	printf("请输入申诉内容（不超过200字）：\n");
	getchar(); // 清除缓冲区
	fgets(a->content, 200, stdin);
	a->content[strcspn(a->content, "\n")] = 0; // 去除换行
	a->status = 0;                      // 待处理
	a->submit_time = time(NULL);
	a->reply[0] = '\0';                  // 空回复
	a->reply_time = 0;
	a->next = NULL;
	if (appealListHead == NULL) {
		appealListHead = a;
	}
	else {
		Appeal* tail = appealListHead;
		while (tail->next) tail = tail->next;
		tail->next = a;
	}
	saveAppeals();
	printf("申诉提交成功！您的申诉ID为：%d\n", a->id);
}
void viewMyAppeals(long long student_id) {
	Appeal* p = appealListHead;
	printf("*               您的申诉记录              *\n");
	int found = 0;
	while (p) {
		if (p->student_id == student_id) {
			found = 1;
			printf("申诉ID: %d\n", p->id);
			printf("类型: %s\n", p->type == 0 ? "成绩申诉" : "密码找回");
			printf("内容: %s\n", p->content);
			printf("状态: %s\n", p->status == 0 ? "待处理" : "已处理");
			printf("提交时间: %s", ctime(&p->submit_time));
			if (p->status == 1) {
				printf("回复: %s\n", p->reply);
				printf("回复时间: %s", ctime(&p->reply_time));
			}
		}
		p = p->next;
	}
	if (!found) printf("暂无申诉记录。\n");
}
void mangerMenu() {
	printf("********************************************\n");
	printf("*               管理员后台管理             *\n");
	printf("********************************************\n");
	printf("*                1. 查看代办               *\n");
	printf("*                2. 管理学生信息           *\n");
	printf("*                3. 数据统计               *\n");
	printf("*                4. 修改学生成绩           *\n");
	printf("*                5. 查看学生密码           *\n");
	printf("*                6. 退出登录               *\n");
	printf("********************************************\n");
}
void appealMenu() {
	printf("********************************************\n");
	printf("*                申诉管理                  *\n");
	printf("********************************************\n");
	printf("*            1. 查看所有申诉               *\n");
	printf("*            2. 查看待处理申诉             *\n");
	printf("*            3. 处理申诉                   *\n");
	printf("*            4. 删除已处理申诉             *\n");
	printf("*            5. 返回上一级                 *\n");
	printf("********************************************\n");
}
void adminViewAllAppeals() {
	if (appealListHead == NULL) {
		printf("暂无申诉记录。\n");
		return;
	}
	Appeal* p = appealListHead;
	printf("*                所有申诉记录              *\n");
	while (p) {
		printf("申诉ID: %d\n", p->id);
		printf("学生: %s (学号%lld)\n", p->student_name, p->student_id);
		printf("类型: %s\n", p->type == 0 ? "成绩申诉" : "密码找回");
		printf("内容: %s\n", p->content);
		printf("状态: %s\n", p->status == 0 ? "待处理" : "已处理");
		printf("提交时间: %s", ctime(&p->submit_time));
		if (p->status == 1) {
			printf("回复: %s\n", p->reply);
			printf("回复时间: %s", ctime(&p->reply_time));
		}
		p = p->next;
	}
}
void adminDeleteProcessedAppeals() {
	Appeal* p = appealListHead;
	Appeal* prev = NULL;
	int count = 0;
	while (p) {
		if (p->status == 1) {
			// 删除该节点
			if (prev == NULL) {
				appealListHead = p->next;
				free(p);
				p = appealListHead;
			}
			else {
				prev->next = p->next;
				free(p);
				p = prev->next;
			}
			count++;
		}
		else {
			prev = p;
			p = p->next;
		}
	}
	if (count > 0) {
		saveAppeals();
		printf("已删除 %d 条已处理的申诉。\n", count);
	}
	else {
		printf("没有已处理的申诉可删除。\n");
	}
}
void loadAppeals() {
	FILE* fp = fopen(APPEAL_FILE, "rb");
	if (!fp) return;
	Appeal* tail = NULL;
	while (1) {
		Appeal* a = (Appeal*)malloc(sizeof(Appeal));
		if (!a) break;
		if (fread(a, sizeof(Appeal), 1, fp) != 1) {
			free(a);
			break;
		}
		a->next = NULL;
		if (appealListHead == NULL) {
			appealListHead = a;
			tail = a;
		}
		else {
			tail->next = a;
			tail = a;
		}
		if (a->id >= nextAppealId) nextAppealId = a->id + 1;
	}
	fclose(fp);
}
void adminViewPendingAppeals() {
	Appeal* p = appealListHead;
	int found = 0;
	printf("*               待处理申诉列表             *\n");
	while (p) {
		if (p->status == 0) {
			found = 1;
			printf("申诉ID: %d\n", p->id);
			printf("学生: %s (学号%lld)\n", p->student_name, p->student_id);
			printf("类型: %s\n", p->type == 0 ? "成绩申诉" : "密码找回");
			printf("内容: %s\n", p->content);
			printf("提交时间: %s", ctime(&p->submit_time));
			printf("----------------------------\n");
		}
		p = p->next;
	}
	if (!found) printf("暂无待处理的申诉。\n");
}
void adminMarkAppealDone() {
	int id;
	printf("请输入要处理的申诉ID：");
	scanf("%d", &id);
	Appeal* p = appealListHead;
	while (p) {
		if (p->id == id) {
			if (p->status == 1) {
				printf("该申诉已被处理过，如需修改回复请先删除或联系开发人员。\n");
				return;
			}
			printf("请输入回复内容（不超过200字）：\n");
			getchar(); // 清除缓冲区
			fgets(p->reply, 200, stdin);
			p->reply[strcspn(p->reply, "\n")] = 0; // 去除换行
			p->reply_time = time(NULL);
			p->status = 1;
			saveAppeals();   // 保存到文件
			printf("申诉 %d 已处理，回复已保存。\n", id);
			return;
		}
		p = p->next;
	}
	printf("未找到该申诉ID。\n");
}
void viewStudentPassword(Node* head) {
	if (head == NULL) {
		printf("学生列表为空！\n");
		return;
	}
	long long stunum;
	printf("请输入学生学号：");
	scanf("%lld", &stunum);
	Node* current = head;
	while (current != NULL) {
		if (current->stu.number == stunum) {
			printf("学号：%lld\n", current->stu.number);
			printf("姓名：%s\n", current->stu.name);
			printf("密码：%s\n", current->stu.password); // 显示密码
			printf("语文：%.1f  数学：%.1f  英语：%.1f\n",
				current->stu.chinese, current->stu.math, current->stu.english);
			return;
		}
		current = current->next;
	}
	printf("未找到该学号的学生！\n");
}
void exportStudentPasswords(Node* head, const char* filename) {
	FILE* fp = fopen(filename, "w");
	if (!fp) {
		perror("无法创建文件");
		return;
	}
	Node* p = head;
	fprintf(fp, "学号\t姓名\t密码\n");  // 表头增加姓名
	while (p) {
		fprintf(fp, "%lld\t%s\t%s\n", p->stu.number, p->stu.name, p->stu.password);
		p = p->next;
	}
	fclose(fp);
	printf("学号、姓名和密码已成功导出到文件：%s\n", filename);
}
void importStudentsFromFile(Node** head, const char* filename) {
	FILE* fp = fopen(filename, "r");
	if (!fp) {
		perror("无法打开文件");
		return;
	}
	char line[256];
	int count = 0;
	while (fgets(line, sizeof(line), fp) != NULL) {
		line[strcspn(line, "\n")] = 0;
		if (strlen(line) == 0) continue;
		long long num;
		char name[20], pwd[20];      // 改为20
		float chinese, math, english;
		int parsed = sscanf(line, "%lld %s %s %f %f %f", &num, name, pwd, &chinese, &math, &english);
		if (parsed == 6) {
			// 完整格式：学号 姓名 密码 语文 数学 英语
			// 检查学号是否已存在
			Node* p = *head;
			int exists = 0;
			while (p) {
				if (p->stu.number == num) {
					printf("学号 %lld 已存在，跳过。\n", num);
					exists = 1;
					break;
				}
				p = p->next;
			}
			if (exists) continue;
			Node* newNode = creatNode();
			newNode->stu.number = num;
			strcpy(newNode->stu.name, name);
			strcpy(newNode->stu.password, pwd);
			newNode->stu.chinese = chinese;
			newNode->stu.math = math;
			newNode->stu.english = english;
			newNode->stu.sum = chinese + math + english; // 计算总分

			// 尾插法插入链表
			if (*head == NULL) {
				*head = newNode;
			}
			else {
				Node* tail = *head;
				while (tail->next != NULL) tail = tail->next;
				tail->next = newNode;
			}
			count++;
		}
		else if (parsed == 3) {
			// 兼容旧格式：只有学号 姓名 密码，成绩默认为0
			Node* p = *head;
			int exists = 0;
			while (p) {
				if (p->stu.number == num) {
					printf("学号 %lld 已存在，跳过。\n", num);
					exists = 1;
					break;
				}
				p = p->next;
			}
			if (exists) continue;

			Node* newNode = creatNode();
			newNode->stu.number = num;
			strcpy(newNode->stu.name, name);
			strcpy(newNode->stu.password, pwd);
			newNode->stu.chinese = 0;
			newNode->stu.math = 0;
			newNode->stu.english = 0;
			newNode->stu.sum = 0;

			if (*head == NULL) {
				*head = newNode;
			}
			else {
				Node* tail = *head;
				while (tail->next != NULL) tail = tail->next;
				tail->next = newNode;
			}
			count++;
			printf("行格式不足6字段，成绩默认为0：%s\n", line);
		}
		else {
			printf("忽略无效行: %s\n", line);
		}
	}

	fclose(fp);
	if (count > 0) {
		saveStudent(*head);  // 保存到二进制文件
		printf("成功导入 %d 名学生。\n", count);
	}
	else {
		printf("未导入任何学生。\n");
	}
}
int main() {
	Node* head = NULL;
	readStudent(&head);
	loadAppeals();
	while (1) {
		// 直接调用登录函数，传入链表头以便学生验证
		int login_result = login(head);

		if (login_result == 1) {   // 教师登录成功
			while (1) {
				switch (teacherMenu()) {
				case 1: 
					entryStudent(&head);
					break;
				case 2:
					printStudent(head); 
					break;
				case 3: 
					saveStudent(head); 
					break;
				case 4: 
					readStudent(&head); 
					break;
				case 5:
					countStudent(head);
					break;
				case 6: 
					findStudent(head); 
					break;
				case 7: 
					changeStudent(head); 
					break;
				case 8: 
					head = deleteStudent(head); 
					break;
				case 9:
					if (head == NULL) {
						printf("学生列表为空，无法排序\n");
					}
					else {
						int sort_by, order;
						printf("请选择排序依据：0-总分，1-语文，2-数学，3-英语\n");
						scanf("%d", &sort_by);
						printf("请选择排序方式：0-升序，1-降序\n");
						scanf("%d", &order);
						head = quickSort(head, sort_by, order);
						printf("排序完成！\n");
					}
					break;
				case 10: {
					char filename[100];
					printf("请输入要导出的文件名（例如 students.txt）: ");
					scanf("%s", filename);
					exportStudentHuman(head, filename);
					break;
				}
				case 11:
					printf("退出登录！\n");
					goto logout_teacher;
				default: 
					printf("无效的选择！\n"); 
					break;
				}
				system("pause");
				system("cls");
			}
		logout_teacher:
			// 退出教师登录后回到主登录界面
			continue;   // 继续外层循环，重新显示登录菜单
		} else if (login_result == 2) {   // 学生登录成功
			while (1) {
				switch (studentMenu()) {
				case 1:
					viewStudent(head, current_user.student_id);
					break;
				case 2:
					changeStudentPassword(head, current_user.student_id);
					break;
				case 3:
					submitAppeal(head, current_user.student_id);   // 提交申诉
					break;
				case 4:
					viewMyAppeals(current_user.student_id);        // 查看申诉记录
					break;
				case 5:
					printf("退出登录！\n");
					goto logout_student;
				default:
					printf("无效的选择！\n");
					break;
				}
				system("pause");
				system("cls");
			}
		logout_student:
			continue;
		}
		else if (login_result == 3) {   // 管理员登录成功
			while (1) {
				mangerMenu();
				int op;
				printf("请选择操作：");
				scanf("%d", &op);
				switch (op) {
				case 1: // 申诉管理
					while (1) {
						appealMenu();   // 显示申诉管理菜单
						int sub_op;
						printf("请选择操作：");
						scanf("%d", &sub_op);
						switch (sub_op) {
						case 1:
							adminViewAllAppeals();
							break;
						case 2:
							adminViewPendingAppeals();
							break;
						case 3:
							adminMarkAppealDone();
							break;
						case 4:
							adminDeleteProcessedAppeals();
							break;
						case 5:
							goto back_to_admin_main;  // 返回管理员主菜单
						default:
							printf("无效选择！\n");
						}
						system("pause");
						system("cls");
					}
				back_to_admin_main:
					break;
				case 2: // 管理学生信息
					while (1) {
						printf("\n========== 管理员-学生信息管理 ==========\n");
						printf("           1. 增加学生\n");
						printf("           2. 删除学生\n");
						printf("           3. 查找学生\n");
						printf("           4. 修改学生成绩\n");
						printf("           5. 统计平均成绩\n");
						printf("           6. 导出学号和密码\n");
						printf("           7. 从文件导入学生\n");   // 新增选项
						printf("           8. 返回上级菜单\n");
						printf("请选择操作：");
						int op;
						scanf("%d", &op);
						switch (op) {
						case 1:
							entryStudent(&head);
							break;
						case 2:
							head = deleteStudent(head);
							break;
						case 3:
							findStudent(head);
							break;
						case 4:
							changeStudent(head);
							break;
						case 5:
							countStudent(head);
							break;
						case 6: {
							char filename[100];
							printf("请输入要导出的文件名（例如 passwords.txt）: ");
							scanf("%s", filename);
							exportStudentPasswords(head, filename);
							break;
						}
						case 7: {
							char filename[100];
							printf("请输入要导入的文件名（例如 import.txt）: ");
							scanf("%s", filename);
							importStudentsFromFile(&head, filename);   // 调用导入函数
							break;
						}
						case 8:
							goto studentManageEnd;
						default:
							printf("无效选择！\n");
						}
						system("pause");
						system("cls");
					}
				studentManageEnd:
					break;
				case 3: // 数据统计（可复用 countStudent 等）
					countStudent(head);
					break;
				case 4: // 修改学生成绩
					changeStudent(head);
					printf("是否保存修改到文件？(y/n): ");
					getchar();   // 吸收之前的换行
					if (getchar() == 'y' || getchar() == 'Y') {
						saveStudent(head);
						printf("保存成功！\n");
					}
					break;
				case 5: // 查看学生密码
					viewStudentPassword(head);
					break;
				case 6: // 退出登录
					saveStudent(head); // 退出前自动保存
					goto logout_admin;
				default:
					printf("无效选项！\n");
				}
				system("pause");
				system("cls");
			}
		logout_admin:
			continue;
		} else {
			while (head != NULL) {
				Node* temp = head;
				head = head->next;
				free(temp);
			}
			printf("程序退出，感谢使用！\n");
			break;
		}
	}
	return 0;
}