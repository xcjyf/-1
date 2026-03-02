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
    int type;               // 0: 学生, 1: 老师, 2: 管理员
    long long student_id;
} User;
User current_user;
char current_teacher_class[20] = "";   // 当前登录教师管理的班级
typedef struct Student {
    long long number;
    char name[20];
    char password[20];
    char classname[20];
    float chinese;
    float math;
    float english;
    float sum;
} Student;
typedef struct Node {
    Student stu;
    struct Node* next;
} Node;
typedef struct Appeal {
    int id;
    long long student_id;
    char student_name[20];
    int type;
    char content[200];
    char reply[200];
    int status;
    time_t submit_time;
    time_t reply_time;
    struct Appeal* next;
} Appeal;
Appeal* appealListHead = NULL;
int nextAppealId = 1;
typedef struct Teacher {
    char id[20];
    char password[20];
    char name[20];
    char classname[20];
} Teacher;
//教师端菜单
int teacherMenu() {
    printf("********************************************\n");
    printf("*            欢迎使用教师端管理系统        *\n");
    printf("********************************************\n");
    printf("*                1. 录入学生信息           *\n");
    printf("*                2. 打印学生信息           *\n");
    printf("*                3. 保存学生信息           *\n");
    printf("*                4. 读取学生信息           *\n");
    printf("*                5. 统计学生信息           *\n");
    printf("*                6. 查找学生信息           *\n");
    printf("*                7. 修改学生信息           *\n");
    printf("*                8. 删除学生信息           *\n");
    printf("*                9. 排序学生信息           *\n");
    printf("*               10. 导出学生信息           *\n");
    printf("*               11. 退出登录               *\n");
    printf("********************************************\n");
    int select = -1;
    printf("select> ");
    scanf("%d", &select);
    return select;
}
void registerTeacher() {
    FILE* fp = fopen("teacher.data", "r");
    Teacher teas[100];
    int count = 0;
    if (fp) {
        while (fscanf(fp, "%s %s %s %s", teas[count].id, teas[count].password,
            teas[count].name, teas[count].classname) == 4) {
            count++;
        }
        fclose(fp);
    }

    char newId[20], newName[20], newClass[20], newPwd[20];
    printf("请输入工号：");
    scanf("%s", newId);

    // 检查工号是否已存在
    for (int i = 0; i < count; i++) {
        if (strcmp(teas[i].id, newId) == 0) {
            printf("工号已存在，注册失败！\n");
            return;
        }
    }

    printf("请输入姓名:");
    scanf("%s", newName);
    printf("请输入管理班级:");
    scanf("%s", newClass);
    printf("请设置密码:");
    getchar();  // 清除缓冲区中的换行符
    fgets(newPwd, 20, stdin);
    newPwd[strcspn(newPwd, "\n")] = 0;  // 去除末尾换行
    if (strlen(newPwd) == 0) {
        strcpy(newPwd, newId);  // 密码默认为工号
    }

    // 以追加方式打开文件写入新教师
    fp = fopen("teacher.data", "a");
    if (!fp) {
        printf("无法打开教师文件！\n");
        return;
    }
    fprintf(fp, "%s %s %s %s\n", newId, newPwd, newName, newClass);
    fclose(fp);
    printf("教师注册成功！请登录。\n");
}
//登录系统
int login(Node* head) {
    int choice;
    char username[20], password[20];
    printf("********************************************\n");
    printf("*                请登陆系统                *\n");
    printf("********************************************\n");
    printf("*                1. 教师登录               *\n");
    printf("*                2. 学生登录               *\n");
    printf("*                3. 管理员登录             *\n");
    printf("*                4. 退出程序               *\n");
    printf("********************************************\n");
    printf("select> ");
    scanf("%d", &choice);
    if (choice == 4) {
        return 0;
    }
    printf("工号> ");
    scanf("%s", username);
    printf("密码> ");
    scanf("%s", password);

    if (choice == 1) {
        while (1) {
            FILE* fp = fopen("teacher.data", "r");
            if (!fp) {
                printf("未检测到教师账号，正在创建新账号...\n");
                fp = fopen("teacher.data", "w");
                if (!fp) {
                    printf("无法创建教师账号文件\n");
                    return 0;
                }
                char t_id[20], t_pwd[20], t_name[20], t_class[20];
                printf("工号> ");
                scanf("%s", t_id);
                printf("密码> ");
                scanf("%s", t_pwd);
                printf("姓名> ");
                scanf("%s", t_name);
                printf("管理班级> ");
                scanf("%s", t_class);
                fprintf(fp, "%s %s %s %s\n", t_id, t_pwd, t_name, t_class);
                fclose(fp);
                printf("教师账号创建成功！\n");
                strcpy(current_user.username, t_id);
                current_user.type = 1;
                strcpy(current_teacher_class, t_class);
                return 1;  // 登录成功
            }
            else {
                fclose(fp);  // 文件存在，先关闭
                int sub_choice;
                printf("请选择：1.登录 2.注册 3.返回上级\n");
                printf("选择> ");
                scanf("%d", &sub_choice);

                if (sub_choice == 1) {
                    // 原有登录验证逻辑
                    fp = fopen("teacher.data", "r");
                    if (!fp) {
                        printf("文件打开失败！\n");
                        return 0;
                    }
                    char file_id[20], file_pwd[20], file_name[20], file_class[20];
                    int found = 0;
                    while (fscanf(fp, "%s %s %s %s", file_id, file_pwd, file_name, file_class) == 4) {
                        if (strcmp(username, file_id) == 0 && strcmp(password, file_pwd) == 0) {
                            found = 1;
                            strcpy(current_teacher_class, file_class);
                            break;
                        }
                    }
                    fclose(fp);
                    if (found) {
                        strcpy(current_user.username, username);
                        current_user.type = 1;
                        printf("教师登录成功！管理班级：%s\n", current_teacher_class);
                        return 1;
                    }
                    else {
                        printf("工号或密码错误！\n");
                        // 继续循环，重新选择
                    }
                }
                else if (sub_choice == 2) {
                    registerTeacher();  // 调用注册函数
                    // 注册完成后继续循环，可再次选择登录
                }
                else if (sub_choice == 3) {
                    return 0;  // 返回上级登录界面
                }
                else {
                    printf("无效选择！\n");
                }
            }
        }
    }
    else if (choice == 2) {
        long long stu_num = atoll(username);
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
            current_user.type = 2;
            return 3;
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

//节点创建
Node* creatNode() {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        printf("malloc failed\n");
        return NULL;
    }
    node->next = NULL;
    return node;
}
//录入学生
void entryStudent(Node** head) {
    Node* node = creatNode();
    printf("输入学生学号> ");
    scanf("%lld", &node->stu.number);
    printf("输入学生姓名> ");
    scanf("%s", node->stu.name);
    if (current_user.type == 1) {
        strcpy(node->stu.classname, current_teacher_class);
        printf("班级自动设为：%s\n", current_teacher_class);
    }
    else {
        printf("输入学生班级> ");
        scanf("%s", node->stu.classname);
    }
    printf("输入初始密码> ");
    scanf("%s", node->stu.password);
    printf("输入学生语文成绩> ");
    scanf("%f", &node->stu.chinese);
    printf("输入学生数学成绩> ");
    scanf("%f", &node->stu.math);
    printf("输入学生英语成绩> ");
    scanf("%f", &node->stu.english);
    if (*head == NULL) {
        *head = node;
    }
    else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
    printf("录入成功\n");
}
//打印学生
void printStudent(Node* head) {
    printf("*************************************************************\n");
    printf("* 学号  *  姓名  *  班级  *  语文  *  数学  *  英语 *  总分  *\n");
    printf("*************************************************************\n");
    Node* current = head;
    while (current != NULL) {
        if (current_user.type == 1 && strcmp(current->stu.classname, current_teacher_class) != 0) {
            current = current->next;
            continue;
        }
        printf("* %lld  *  %s  *  %s  *  %.1f  *  %.1f  *  %.1f *  %.1f\n",
            current->stu.number, current->stu.name, current->stu.classname,
            current->stu.chinese, current->stu.math, current->stu.english,
            current->stu.chinese + current->stu.math + current->stu.english);
        current = current->next;
    }
}

//保存学生
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

//读取学生
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
    while (!feof(fp)) {
        Node* node = creatNode();
        if (!node) break;
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

//统计学生
void countStudent(Node* head) {
    Node* current = head;
    int count = 0;
    float chineseSum = 0, mathSum = 0, englishSum = 0;
    while (current != NULL) {
        if (current_user.type == 1 && strcmp(current->stu.classname, current_teacher_class) != 0) {
            current = current->next;
            continue;
        }
        chineseSum += current->stu.chinese;
        mathSum += current->stu.math;
        englishSum += current->stu.english;
        count++;
        current = current->next;
    }
    if (count == 0) {
        printf("无学生数据\n");
        return;
    }
    else {
        printf("共有学生%d人\n", count);
        printf("语文平均分为%.1f\n", chineseSum / count);
        printf("数学平均分为%.1f\n", mathSum / count);
        printf("英语平均分为%.1f\n", englishSum / count);
    }
}
//查找学生
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
        if (current_user.type == 1 && strcmp(current->stu.classname, current_teacher_class) != 0) {
            current = current->next;
            continue;
        }
        if (stunum == current->stu.number) {
            printf("* %lld  *  %s  *  %s  *  %.1f  *  %.1f  *  %.1f *\n",
                current->stu.number, current->stu.name, current->stu.classname,
                current->stu.chinese, current->stu.math, current->stu.english);
            return;
        }
        current = current->next;
    }
    printf("未找到该学生\n");
}

//修改学生成绩
void changeStudent(Node* head) {
    printf("请输入要修改学生学号\n");
    long long stunum;
    scanf("%lld", &stunum);
    Node* current = head;
    while (current != NULL) {
        if (current_user.type == 1 && strcmp(current->stu.classname, current_teacher_class) != 0) {
            current = current->next;
            continue;
        }
        if (stunum == current->stu.number) {
            printf("请输入要修改学生的语文成绩 数学成绩 英语成绩> ");
            scanf("%f%f%f", &current->stu.chinese, &current->stu.math, &current->stu.english);
            current->stu.sum = current->stu.chinese + current->stu.math + current->stu.english;
            printf("修改成功\n");
            return;
        }
        current = current->next;
    }
    printf("未找到该学生\n");
}

//删除学生
Node* deleteStudent(Node* head) {
    if (head == NULL) {
        printf("学生列表为空\n");
        return NULL;
    }
    long long num;
    printf("请输入要删除学生的学号");
    scanf("%lld", &num);
    if (head->stu.number == num) {
        if (current_user.type == 1 && strcmp(head->stu.classname, current_teacher_class) != 0) {
            printf("无权限删除其他班级学生！\n");
            return head;
        }
        printf("已找到学生: %lld %s\n", head->stu.number, head->stu.name);
        printf("确认删除? (y/n)>");
        getchar();
        char a;
        scanf("%c", &a);
        if (a == 'y' || a == 'Y') {
            Node* temp = head;
            head = head->next;
            free(temp);
            printf("删除成功\n");
            return head;
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
            if (current_user.type == 1 && strcmp(current->stu.classname, current_teacher_class) != 0) {
                printf("无权限删除其他班级学生！\n");
                return head;
            }
            printf("已找到学生:%lld %s\n", current->stu.number, current->stu.name);
            printf("确认删除? (y/n)>");
            getchar();
            char a;
            scanf("%c", &a);
            if (a == 'y' || a == 'Y') {
                prev->next = current->next;
                free(current);
                printf("删除成功\n");
            }
            else {
                printf("取消删除\n");
            }
            return head;
        }
        prev = current;
        current = current->next;
    }
    printf("未找到该学号相对应的学生\n");
    return head;
}

//排序辅助
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
    if (order == 0) return sum_a < sum_b;
    else return sum_a > sum_b;
}

//快速排序
Node* quickSort(Node* head, int sort_by, int order) {
    if (head == NULL || head->next == NULL) return head;
    Node* pivot = head;
    Node* current = head->next;
    pivot->next = NULL;
    Node* leftHead = NULL, * leftTail = NULL;
    Node* rightHead = NULL, * rightTail = NULL;
    while (current != NULL) {
        Node* next = current->next;
        current->next = NULL;
        if (shouldSwap(&(current->stu), &(pivot->stu), sort_by, order)) {
            if (leftHead == NULL) leftHead = leftTail = current;
            else {
                leftTail->next = current;
                leftTail = current;
            }
        }
        else {
            if (rightHead == NULL) rightHead = rightTail = current;
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
        while (leftTail->next != NULL) leftTail = leftTail->next;
        leftTail->next = pivot;
    }
    else newHead = pivot;
    pivot->next = rightHead;
    return newHead;
}
//导出学生信息
void exportStudentHuman(Node* head, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("无法创建文件");
        return;
    }
    Node* current = head;
    while (current != NULL) {
        if (current_user.type == 1 && strcmp(current->stu.classname, current_teacher_class) != 0) {
            current = current->next;
            continue;
        }
        fprintf(fp, "%lld\t%s\t%s\t%s\t%.1f\t%.1f\t%.1f\n",
            current->stu.number, current->stu.name, current->stu.classname,
            current->stu.password, current->stu.chinese, current->stu.math, current->stu.english);
        current = current->next;
    }
    printf("成功导出文本文件: %s\n", filename);
    fclose(fp);
}

//学生端菜单
int studentMenu() {
    printf("********************************************\n");
    printf("*             欢迎使用学生管理系统         *\n");
    printf("********************************************\n");
    printf("*                1. 查看信息               *\n");
    printf("*                2. 修改密码               *\n");
    printf("*                3. 提交申诉               *\n");
    printf("*                4. 查看申诉记录           *\n");
    printf("*                5. 退出登录               *\n");
    printf("********************************************\n");
    int select = -1;
    printf("select> ");
    scanf("%d", &select);
    return select;
}

//学生修改密码
void changeStudentPassword(Node* head, long long student_id) {
    Node* current = head;
    while (current != NULL) {
        if (current->stu.number == student_id) {
            char oldPassword[20], newPassword[20], confirmPassword[20];
            printf("请输入旧密码\n");
            scanf("%s", oldPassword);
            if (strcmp(current->stu.password, oldPassword) != 0) {
                printf("密码错误\n");
                return;
            }
            printf("请输入新密码\n");
            scanf("%s", newPassword);
            printf("请确认新密码\n");
            scanf("%s", confirmPassword);
            if (strcmp(newPassword, confirmPassword) == 0) {
                strcpy(current->stu.password, newPassword);
                printf("密码修改成功！\n");
                saveStudent(head);
            }
            else {
                printf("两次输入的密码不一致\n");
            }
            return;
        }
        current = current->next;
    }
    printf("未找到学生信息\n");
}

//学生查看信息
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
        if (p == target) {
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
    printf("班级: %s\n", target->stu.classname);
    printf("语文成绩: %.1f (班级排名: %d)\n", chinese, rank_chinese);
    printf("数学成绩: %.1f (班级排名: %d)\n", math, rank_math);
    printf("英语成绩: %.1f (班级排名: %d)\n", english, rank_english);
    printf("总分: %.1f (班级排名: %d)\n", total, rank_total);
}

//申诉保存
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

//提交申诉
void submitAppeal(Node* head, long long student_id) {
    Node* p = head;
    char stu_name[20] = "";
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
    getchar();
    fgets(a->content, 200, stdin);
    a->content[strcspn(a->content, "\n")] = 0;
    a->status = 0;
    a->submit_time = time(NULL);
    a->reply[0] = '\0';
    a->reply_time = 0;
    a->next = NULL;
    if (appealListHead == NULL) appealListHead = a;
    else {
        Appeal* tail = appealListHead;
        while (tail->next) tail = tail->next;
        tail->next = a;
    }
    saveAppeals();
    printf("申诉提交成功！您的申诉ID为：%d\n", a->id);
}
//查看我的申诉
void viewMyAppeals(long long student_id) {
    Appeal* p = appealListHead;
    printf("********************************************\n");
    printf("*             您的申诉记录                 *\n");
    printf("********************************************\n");
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
            printf("----------------------------\n");
        }
        p = p->next;
    }
    if (!found) printf("暂无申诉记录。\n");
}

//管理员菜单
void mangerMenu() {
    printf("********************************************\n");
    printf("*               管理员后台管理             *\n");
    printf("********************************************\n");
    printf("*                1. 申诉管理               *\n");
    printf("*                2. 管理学生信息           *\n");
    printf("*                3. 数据统计               *\n");
    printf("*                4. 修改学生成绩           *\n");
    printf("*                5. 查看学生密码           *\n");
    printf("*                6. 管理教师账号           *\n");
    printf("*                7. 退出登录               *\n");
    printf("********************************************\n");
}
//申诉管理子菜单
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
//管理员查看所有申诉
void adminViewAllAppeals() {
    if (appealListHead == NULL) {
        printf("暂无申诉记录。\n");
        return;
    }
    Appeal* p = appealListHead;
    printf("********************************************\n");
    printf("*             所有申诉记录                 *\n");
    printf("********************************************\n");
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
        printf("----------------------------\n");
        p = p->next;
    }
}

//管理员删除已处理申诉
void adminDeleteProcessedAppeals() {
    Appeal* p = appealListHead;
    Appeal* prev = NULL;
    int count = 0;
    while (p) {
        if (p->status == 1) {
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

//加载申诉
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

//管理员查看待处理申诉
void adminViewPendingAppeals() {
    Appeal* p = appealListHead;
    int found = 0;
    printf("********************************************\n");
    printf("*           待处理申诉列表                 *\n");
    printf("********************************************\n");
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

//管理员处理申诉
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
            getchar();
            fgets(p->reply, 200, stdin);
            p->reply[strcspn(p->reply, "\n")] = 0;
            p->reply_time = time(NULL);
            p->status = 1;
            saveAppeals();
            printf("申诉 %d 已处理，回复已保存。\n", id);
            return;
        }
        p = p->next;
    }
    printf("未找到该申诉ID。\n");
}

//管理员查看学生密码
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
            printf("班级：%s\n", current->stu.classname);
            printf("密码：%s\n", current->stu.password);
            printf("语文：%.1f  数学：%.1f  英语：%.1f\n",
                current->stu.chinese, current->stu.math, current->stu.english);
            return;
        }
        current = current->next;
    }
    printf("未找到该学号的学生！\n");
}

//管理员导出学号密码
void exportStudentPasswords(Node* head, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("无法创建文件");
        return;
    }
    Node* p = head;
    fprintf(fp, "学号\t姓名\t班级\t密码\n");
    while (p) {
        fprintf(fp, "%lld\t%s\t%s\t%s\n", p->stu.number, p->stu.name, p->stu.classname, p->stu.password);
        p = p->next;
    }
    fclose(fp);
    printf("学号、姓名、班级和密码已成功导出到文件：%s\n", filename);
}

//管理员从文件导入学生
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
        char name[20], pwd[20], classname[20];
        float chinese, math, english;
        int parsed = sscanf(line, "%lld %s %s %s %f %f %f", &num, name, pwd, classname, &chinese, &math, &english);
        if (parsed == 7) {
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
            strcpy(newNode->stu.classname, classname);
            newNode->stu.chinese = chinese;
            newNode->stu.math = math;
            newNode->stu.english = english;
            newNode->stu.sum = chinese + math + english;
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
        else {
            printf("忽略无效行: %s (应为7字段: 学号 姓名 密码 班级 语文 数学 英语)\n", line);
        }
    }
    fclose(fp);
    if (count > 0) {
        saveStudent(*head);
        printf("成功导入 %d 名学生。\n", count);
    }
    else {
        printf("未导入任何学生。\n");
    }
}
//管理员修改学生成绩
void adminChangeStudent(Node* head) {
    if (head == NULL) {
        printf("学生列表为空！\n");
        return;
    }
    char target_class[20];
    printf("请输入要修改的班级：");
    scanf("%s", target_class);
    Node* p = head;
    int found = 0;
    printf("该班级学生列表：\n");
    while (p) {
        if (strcmp(p->stu.classname, target_class) == 0) {
            printf("学号：%lld  姓名：%s\n", p->stu.number, p->stu.name);
            found = 1;
        }
        p = p->next;
    }
    if (!found) {
        printf("该班级无学生！\n");
        return;
    }
    long long stu_num;
    printf("请输入要修改成绩的学生学号：");
    scanf("%lld", &stu_num);
    p = head;
    while (p) {
        if (p->stu.number == stu_num && strcmp(p->stu.classname, target_class) == 0) {
            printf("当前成绩：语文 %.1f 数学 %.1f 英语 %.1f\n", p->stu.chinese, p->stu.math, p->stu.english);
            printf("请输入新成绩（语文 数学 英语）：");
            scanf("%f%f%f", &p->stu.chinese, &p->stu.math, &p->stu.english);
            p->stu.sum = p->stu.chinese + p->stu.math + p->stu.english;
            saveStudent(head);
            printf("修改成功！\n");
            return;
        }
        p = p->next;
    }
    printf("该班级中未找到该学号！\n");
}

//教师账号管理菜单
void adminTeacherMenu() {
    printf("********************************************\n");
    printf("*           管理员-教师账号管理           *\n");
    printf("********************************************\n");
    printf("*           1. 查看所有教师                *\n");
    printf("*           2. 添加教师                    *\n");
    printf("*           3. 删除教师                    *\n");
    printf("*           4. 修改教师信息                *\n");
    printf("*           5. 返回上级菜单                *\n");
    printf("********************************************\n");
}

//查看所有教师
void adminViewAllTeachers() {
    FILE* fp = fopen("teacher.data", "r");
    if (!fp) {
        printf("暂无教师数据或文件不存在。\n");
        return;
    }
    Teacher tea;
    printf("********************************************\n");
    printf("*                教师列表                  *\n");
    printf("********************************************\n");
    printf("工号\t姓名\t管理班级\t密码\n");
    while (fscanf(fp, "%s %s %s %s", tea.id, tea.password, tea.name, tea.classname) == 4) {
        printf("%s\t%s\t%s\t%s\n", tea.id, tea.name, tea.classname, tea.password);
    }
    fclose(fp);
}

//添加教师
void adminAddTeacher() {
    Teacher newTea;
    FILE* fp = fopen("teacher.data", "a+");
    if (!fp) {
        printf("无法打开教师文件！\n");
        return;
    }
    rewind(fp);
    printf("请输入新教师工号：");
    scanf("%s", newTea.id);
    Teacher temp;
    int exists = 0;
    while (fscanf(fp, "%s %s %s %s", temp.id, temp.password, temp.name, temp.classname) == 4) {
        if (strcmp(temp.id, newTea.id) == 0) {
            exists = 1;
            break;
        }
    }
    if (exists) {
        printf("工号 %s 已存在！添加失败。\n", newTea.id);
        fclose(fp);
        return;
    }
    printf("请输入教师姓名：");
    scanf("%s", newTea.name);
    printf("请输入管理班级：");
    scanf("%s", newTea.classname);
    char pwd[20];
    printf("请设置密码（直接回车默认为工号）：");
    getchar();
    fgets(pwd, 20, stdin);
    pwd[strcspn(pwd, "\n")] = 0;
    if (strlen(pwd) == 0) strcpy(pwd, newTea.id);
    strcpy(newTea.password, pwd);
    fprintf(fp, "%s %s %s %s\n", newTea.id, newTea.password, newTea.name, newTea.classname);
    fclose(fp);
    printf("教师添加成功！\n");
}

//删除教师
void adminDeleteTeacher() {
    char delId[20];
    printf("请输入要删除的教师工号：");
    scanf("%s", delId);
    FILE* fp = fopen("teacher.data", "r");
    if (!fp) {
        printf("教师文件不存在！\n");
        return;
    }
    Teacher teas[100];
    int count = 0;
    while (fscanf(fp, "%s %s %s %s", teas[count].id, teas[count].password, teas[count].name, teas[count].classname) == 4) {
        count++;
    }
    fclose(fp);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(teas[i].id, delId) == 0) {
            found = 1;
            for (int j = i; j < count - 1; j++) teas[j] = teas[j + 1];
            count--;
            break;
        }
    }
    if (!found) {
        printf("未找到工号为 %s 的教师！\n", delId);
        return;
    }
    fp = fopen("teacher.data", "w");
    if (!fp) {
        printf("无法写入教师文件！\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s %s %s %s\n", teas[i].id, teas[i].password, teas[i].name, teas[i].classname);
    }
    fclose(fp);
    printf("教师删除成功！\n");
}

//修改教师信息
void adminModifyTeacher() {
    char modId[20];
    printf("请输入要修改的教师工号：");
    scanf("%s", modId);
    FILE* fp = fopen("teacher.data", "r");
    if (!fp) {
        printf("教师文件不存在！\n");
        return;
    }
    Teacher teas[100];
    int count = 0;
    while (fscanf(fp, "%s %s %s %s", teas[count].id, teas[count].password, teas[count].name, teas[count].classname) == 4) {
        count++;
    }
    fclose(fp);
    int found = -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(teas[i].id, modId) == 0) {
            found = i;
            break;
        }
    }
    if (found == -1) {
        printf("未找到工号为 %s 的教师！\n", modId);
        return;
    }
    printf("当前信息：工号=%s, 姓名=%s, 班级=%s, 密码=%s\n", teas[found].id, teas[found].name, teas[found].classname, teas[found].password);
    printf("请选择要修改的项：\n");
    printf("1. 修改姓名\n");
    printf("2. 修改班级\n");
    printf("3. 修改密码\n");
    printf("4. 取消\n");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
    case 1:
        printf("请输入新姓名：");
        scanf("%s", teas[found].name);
        break;
    case 2:
        printf("请输入新班级：");
        scanf("%s", teas[found].classname);
        break;
    case 3: {
        char newPwd[20];
        printf("请输入新密码：");
        scanf("%s", newPwd);
        strcpy(teas[found].password, newPwd);
        break;
    }
    default:
        printf("取消修改。\n");
        return;
    }
    fp = fopen("teacher.data", "w");
    if (!fp) {
        printf("无法写入教师文件！\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s %s %s %s\n", teas[i].id, teas[i].password, teas[i].name, teas[i].classname);
    }
    fclose(fp);
    printf("教师信息修改成功！\n");
}

//主函数
int main() {
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
    Node* head = NULL;
    readStudent(&head);
    loadAppeals();
    while (1) {
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
                    if (head == NULL) printf("学生列表为空，无法排序\n");
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
                }
                system("pause");
                system("cls");
            }
        logout_teacher:
            continue;
        }
        else if (login_result == 2) {   // 学生登录成功
            while (1) {
                switch (studentMenu()) {
                case 1: 
                    viewStudent(head, current_user.student_id); 
                    break;
                case 2: 
                    changeStudentPassword(head, current_user.student_id);
                    break;
                case 3: 
                    submitAppeal(head, current_user.student_id); 
                    break;
                case 4: 
                    viewMyAppeals(current_user.student_id); 
                    break;
                case 5: 
                    printf("退出登录！\n"); 
                    goto logout_student;
                default: 
                    printf("无效的选择！\n");
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
                        appealMenu();
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
                            goto back_to_admin_main;
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
                        printf("********************************************\n");
                        printf("*         管理员-学生信息管理              *\n");
                        printf("********************************************\n");
                        printf("*           1. 增加学生                    *\n");
                        printf("*           2. 删除学生                    *\n");
                        printf("*           3. 查找学生                    *\n");
                        printf("*           4. 修改学生成绩                *\n");
                        printf("*           5. 统计平均成绩                *\n");
                        printf("*           6. 导出学号和密码              *\n");
                        printf("*           7. 从文件导入学生              *\n");
                        printf("*           8. 返回上级菜单                *\n");
                        printf("********************************************\n");
                        int op2;
                        printf("请选择操作：");
                        scanf("%d", &op2);
                        switch (op2) {
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
                            importStudentsFromFile(&head, filename);
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
                case 3: // 数据统计
                    countStudent(head);
                    break;
                case 4: // 修改学生成绩（管理员专用，带班级选择）
                    adminChangeStudent(head);
                    break;
                case 5: // 查看学生密码
                    viewStudentPassword(head);
                    break;
                case 6: // 管理教师账号
                    while (1) {
                        adminTeacherMenu();
                        int t_op;
                        printf("请选择操作：");
                        scanf("%d", &t_op);
                        switch (t_op) {
                        case 1: 
                            adminViewAllTeachers(); 
                            break;
                        case 2: 
                            adminAddTeacher(); 
                            break;
                        case 3: 
                            adminDeleteTeacher(); 
                            break;
                        case 4: 
                            adminModifyTeacher(); 
                            break;
                        case 5: 
                            goto teacherManageEnd;
                        default: 
                            printf("无效选择！\n");
                        }
                        system("pause");
                        system("cls");
                    }
                teacherManageEnd:
                    break;
                case 7: // 退出登录
                    saveStudent(head);
                    goto logout_admin;
                default:
                    printf("无效选项！\n");
                }
                system("pause");
                system("cls");
            }
        logout_admin:
            continue;
        }
        else {
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