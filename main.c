#include "student.h"

#include <stdio.h>

#define NAME_MAX_LEN 40
#define DATA_TITLE "学号\t名字\t成绩\n"

#define print_with_format(format, ...) \
	printf("\t\t" format, ##__VA_ARGS__)


/* 定义一个全局`Student List`(单向链表)
 * `Student List`相关代码向详见`student.c`，
 * 相关定义详见`student.h` */
static StudentList student_list;


void clear_stdin(void)
{
	while (getchar() != '\n')
		continue;
}

void priv_print(char *string, char end)
{
	printf("\t\t%s%c", string, end);
}

void print(char *str)
{
	priv_print(str, '\n');
}

void print_nonew_line(char *str)
{
	priv_print(str, '\0');
}

void show_menu(void)
{
	print("|--------------学生信息管理系统-----------|");
	print("|		1.录入学生信息		  |");
	print("|		2.查找学生信息		  |");
	print("|		3.删除学生信息		  |");
	print("|		4.修改学生信息		  |");
	print("|		5.插入学生信息		  |");
	print("|		6.排序			  |");
	print("|		7.统计学生总数		  |");
	print("|		8.显示所有学生信息	  |");
	print("|		0.退出系统		  |");
	print("|-----------------------------------------|");
	print_nonew_line("请选择<0-8> -> ");
}

void show_student(StudentObject object)
{
	StudentAttribute attr;

	student_object_attr_export(object, &attr);
	if (attr.name == NULL)
		return;

	print_with_format("%d\t%s\t%.1f\n", attr.num, attr.name, attr.score);
}

void show_title(void)
{
	print_nonew_line(DATA_TITLE);
}

void show_all_student(void)
{
	StudentList list_generator;
	StudentObject object;

	list_generator = student_list;
	object = student_list_generate(&list_generator);

	show_title();
	while (object != NULL) {
		show_student(object);
		object = student_list_generate(&list_generator);
	}
}

void get_student_from_stdin(StudentAttribute *attr,
			    student_base_t name_max_length)
{
	/* 字符串的偏移量 */
	size_t offset;
	unsigned char got_char;

	print_nonew_line("请输入学生姓名: ");

	offset = 0;
	got_char = getchar();
	while (got_char != '\n') {
		/* 获取到的字符通过指针写入到对应变量 */
		*((attr->name) + offset) = got_char;
		/* 写入一个字符后，字符串的偏移+1 */
		++offset;

		if (offset >= (name_max_length - 1)) {
			offset = 0;
			print_with_format("超出名字最大长度%d个字符！", name_max_length - 1);
			print_nonew_line("请重新输入：");
			clear_stdin();
		}

		/* 获取下一个字符 */
		got_char = getchar();
	}

	/* name 是一个字符串，
	 * 所以要在它的尾部添加一个空字符 */
	*((attr->name) + offset) = '\0';

	print_nonew_line("请输入学号: ");
	scanf("%d", &(attr->num));

	print_nonew_line("请输入该学生的成绩: ");
	scanf("%f", &(attr->score));
}

void insert_student(void)
{
	StudentAttribute attr;
	unsigned char name[NAME_MAX_LEN];

	attr.name = name;
	get_student_from_stdin(&attr, NAME_MAX_LEN);
	student_list_append(student_list, &attr);

	show_title();
	show_student(student_list_get_last(student_list));
}

void remove_student(void)
{
	StudentAttribute attr;

	print_nonew_line("请输入要删除的学号：");
	scanf("%d", &(attr.num));

	if (student_list_remove_with_num(&student_list, attr.num) == STUDENT_OK)
		print("成功删除！");
	else
		print("学生不存在");
}

void find_student(void)
{
	StudentAttribute attr;
	StudentObject object;

	print_nonew_line("请输入要查找的学号：");
	scanf("%d", &(attr.num));

	object = student_list_get_with_num(student_list, attr.num);
	if (object == NULL) {
		print("未找到该学生");
		return;
	}

	show_title();
	show_student(object);
}

void count_students(void)
{
	print_with_format("总共%d人\n", student_list_count(student_list));
}

void order_students(void)
{
	student_list_sort_by_score(student_list);
	show_all_student();
}

void modify_student(void)
{
	StudentAttribute attr;
	StudentObject object;
	unsigned char name[NAME_MAX_LEN];

	print_nonew_line("请输入要修改学生的学号：");
	scanf("%d", &(attr.num));
	clear_stdin();

	object = student_list_get_with_num(student_list, attr.num);
	if (object == NULL) {
		print("指定的学生不存在！");
		return;
	}

	attr.name = name;
	get_student_from_stdin(&attr, NAME_MAX_LEN);
	student_object_modify(object, &attr);

	show_title();
	show_student(object);
}

void pause(void)
{
	print_nonew_line("按下回车键继续...");
	clear_stdin();
	getchar();
}

void load_from_file(void)
{
	int fstat;
	FILE *fp;
	StudentAttribute attr;
	unsigned char name[NAME_MAX_LEN];
	
	fp = fopen("data.txt", "r");
	if (fp == NULL) {
		print("data文件不存在！");
		return;
	} else {
		print("检测到data文件，准备导入");
	}

	attr.name = name;
	while (fstat != EOF) {
		fstat = fscanf(fp, "%u\t%s\t%f", &(attr.num), attr.name, &(attr.score));
		if (fstat == EOF)
			continue;

		student_list_append(student_list, &attr);
	}

	fclose(fp);
	print("导入完成！");
}

void save_to_file(void)
{
	FILE *fp;
	StudentList list_generator;
	StudentObject object;
	StudentAttribute attr;

	if (student_list_is_empty(student_list)) {
		print("当前无学生信息录入");
		return;
	}

	fp = fopen("data.txt", "w");
	if (fp == NULL) {
		print("打开文件失败！");
		return;
	}

	list_generator = student_list;
	object = student_list_generate(&list_generator);
	while (object != NULL) {
		student_object_attr_export(object, &attr);
		fprintf(fp, "%u\t%s\t%f\n", attr.num, attr.name, attr.score);
		object = student_list_generate(&list_generator);
	}

	fclose(fp);
	print("成功保存！");
}

int main(void)
{
	unsigned char continue_flag;

	student_list = student_list_create();
	load_from_file();

	continue_flag = 1;
	while (continue_flag) {
		show_menu();

		switch (getchar()) {
		case '0':
			continue_flag = 0;
			break;
		case '1':
			save_to_file();
			pause();
			break;
		case '2':
			clear_stdin();
			find_student();
			pause();
			break;
		case '3':
			clear_stdin();
			remove_student();
			pause();
			break;
		case '4':
			clear_stdin();
			modify_student();
			pause();
			break;
		case '5':
			clear_stdin();
			insert_student();
			pause();
			break;
		case '6':
			order_students();
			pause();
			break;
		case '7':
			count_students();
			pause();
			break;
		case '8':
			show_all_student();
			pause();
			break;
		case '\n':
			break;
		default:
			print("请输入正确的序号");
			pause();
			break;
		}
	}

	student_list_delete(student_list);
	return 0;
}
