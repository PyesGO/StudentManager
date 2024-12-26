#include "student.h"

#include <stdio.h>

#define NAME_MAX_LEN 80

#define INFO_TITLE 	  "学号\t姓名\t选修课\t实验课\t必修课\t总分"
#define INFO_FORMAT 	  "%u\t%s\t%.1f\t%.1f\t%.1f\t%.1f"
#define INFO_DATA_FILE 	  "data.bin"

#define print_with_format(format, ...) \
	printf("\t\t" format, ##__VA_ARGS__)

#define print(str) \
	print_with_format("%s\n", str)

#define print_without_endline(str) \
	print_with_format("%s", str)

#define print_student_info_title() \
	print(INFO_TITLE) 


/* 定义一个全局学生列表，用来存储所有学生的信息 */
static StudentList student_list;


/* 展示菜单 */
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
	print_without_endline("请选择<0-8> -> ");
}

/* 显示所传入的学生信息
 * 参数：
 * 	- object 传入一个“学生对象”，关于“学生对象”的具体实现，
 * 		 可前往查看“student.c”文件
 */
void show_student(StudentObject object)
{
	/* 声明一个学生属性，属性就是学生的个人信息，
	 * 而“object”（学生对象）就是一个个体，可以看作是一个人，
	 * 在外部（外部是指除“student.c”其它所有c源码文件）访问学生的信息
	 * 都要依赖这个属性变量
	 *
	 * 注解：
	 * 	学生的属性用来在外部访问学生信息，而“object”（学生对象）
	 *	是在“student.c”源码内部才能访问的--其实外部也能访问，但十分不建议，
	 *	因为“object”涉及了内存的管理，如果在外部没有使用好，就会导致内存泄漏、
	 *	访问越界等坏情况，典型事故就是输出很多不知0或者其它随机数字甚至直接程序崩溃，
	 *	这真的会让人抓狂
	 *
	 * 	而这些坏情况“student.c”文件内部都已经处理妥当了，因此尽可能不要在
	 * 	外部使用“object->name”这种，或其它的方式进行直接的访问
	 *
	 * 可能有的疑问：
	 * 	关于“学生对象”的具体解释，在下面名为“show_all_student”函数的注释中有解释
	 */
	StudentAttribute attr;

	/* 将“学生对象”的所有属性全部导出到 attr 变量中，
	 * 往后我们就可以直接访问该同学的信息了
	 *
	 * 注解：
	 * 	“导出”底层就是将“object”（学生对象）的内容复制，
	 * 	然后粘贴到“attr”变量中；
	 *
	 * 	要知晓具体的底层实现，可查看“student.c”源码文件中的
	 * 	“student_object_copy”函数（这是一个私有函数，外部无法访问）
	 */
	student_object_attr_export(object, &attr);

	/* 这里判断下返回的“学生对象”中是不是没名字的，
	 * 没名字的“学生对象”就不要显示到屏幕上了
	 */
	if (attr.info.name == NULL)
		return;

	print_with_format(INFO_FORMAT "\n", attr.info.num, attr.info.name,
		   	  attr.scores.optional, attr.scores.experimental,
		   	  attr.scores.required, attr.scores.total);
}

/* 把记录中的所有学生信息显示到屏幕上 */
void show_all_student(void)
{
	/* 定义了一个列表迭代器
	 * 迭代器是什么：
	 * 	首先保存学生信息的是一个列表（Student List），
	 * 	列表里有许多个学生的学生信息；
	 * 	迭代器就是从第一个学生信息开始返回，
	 * 	每调用一次，都会返回第n+1个学生的学生信息
	 * 	例如：
	 * 		第一次调用：
	 * 		object = student_list_generate(&list_generator);
	 *
	 * 		它返回的是整个学生列表的第一元素，
	 * 		也就是第一个学生的学生信息；
	 *
	 * 		第二次调用：
	 * 		object = student_list_generate(&list_generator);
	 *
	 * 		此时它返回的是整个列表里的第二个元素，
	 * 		即第二个学生的学生信息
	 *
	 * 		第三次调用就第三个学生，依此类推。
	 * 		直到列表最后一个学生信息返回后，
	 * 		再调用它就会返回NULL，此时就不应该再调用它了
	 */
	StudentList list_generator;

	/* 定义了一个“学生对象”
	 * “对象”是什么：
	 * 	“对象”就是对某一具象的统称，比如手机有很多
	 * 	品牌、型号，但是平时都叫手机，很少会直接叫具体型号，
	 * 	因为它是多样化的，就要有一个标志性的词方便称呼
	 *
	 * 为什么使用“学生对象”：
	 * 	如果不使用对象，一个学生时定义一个变量name就好了
	 * 	例如：
	 * 		char *name = "张三";
	 *
	 * 	两个学生时就：
	 * 		char *name1 = "张三";
	 * 		char *name2 = "李四";
	 *
	 * 	都是可行的，但若有n个呢？
	 *
	 * 	何况不止名字还有成绩等信息；
	 *
	 * 	显然，这里是有规律的，每个学生都是有名字的，
	 * 	那我把它弄成模型，这样每个对象的名字具体叫什么我
	 * 	先不关心，到时候再取名，总之肯定是有名儿的；
	 * 	其它成绩呀、学号呀也一样
	 *
	 * 	这种使用“对象”的编程方式会更贴合人对现实世界的理解
	 */
	StudentObject object;

	/* 赋值迭代器要迭代的列表 */
	list_generator = student_list;

	/* 此时返回“object”为学生列表中的第一个“学生对象” */
	object = student_list_generate(&list_generator);


	print_student_info_title();

	/* 循环迭代学生列表里的元素（直到最后一个停止） */
	while (object != NULL) {
		/* 将该学生显示到屏幕上 */
		show_student(object);

		/* 第二次调用，返回第二个元素，因为在循环里，
		 * 往后还会有n次调用（也会返回第n个元素）
		 */
		object = student_list_generate(&list_generator);
	}
}

/* 清理标准输入（stdin）的多余字符 */
void clear_stdin(void)
{
	/* 获取字符直到为换行符时（回车）停止 */
	while (getchar() != '\n')
		continue;
}

unsigned char getchar_lower_case(void)
{
	unsigned char got_char;

	got_char = getchar();
	if ((got_char > 64) && (got_char < 91))
		got_char += 0x20;

	return got_char;
}

/* 从标准输入（一般是控制台）中读取字符
 * 参数：
 * 	- attr 学生的属性，是一个指针（要修改内容就要用到指针）
 * 	- name_max_length 名字最大长度（主要是限制从控制台获取的名字长度）
 */
void get_student_info_from_stdin(StudentAttribute *attr,
				 student_base_t name_max_length)
{
	/* 字符串的偏移量，
	 * 通俗讲就是字符串中第几个字符的意思
	 *
	 * 注解：
	 * 	这里的“字符串”指的是学生的姓名
	 */
	size_t offset;
	/* 这个变量用来接收getchar()函数的字符 */
	unsigned char got_char;

	/* 无追加换行符打印 */
	print_without_endline("请输入学生姓名: ");

	/* 先将字符串偏移置零，
	 * 使其指向字符串的第一个字符
	 */
	offset = 0;
	/* 开始获取字符，直到遇到换行符时停止 */
	got_char = getchar();
	while (got_char != '\n') {
		/* 获取到的字符通过指针写入到对应变量 */
		*((attr->info.name) + offset) = got_char;
		/* 写入一个字符后，字符串的偏移+1 */
		++offset;

		/* 判断是否超出了字符数的最大限制，
		 * 减1是因为字符串末尾要留出一个字节
		 * 来存放空字符'\0'
		 *
		 * 为啥：
		 * 	这个是C语言“字符串”的定义
		 */
		if (offset >= (name_max_length - 1)) {
			/* 如果超出了，字符串偏移置零，
			 * 重头获取字符并写入
			 */
			offset = 0;
			print_with_format("超出名字最大长度%d个字符！", name_max_length - 1);
			print_without_endline("请重新输入：");
			/* 清除多余的字符 */
			clear_stdin();
		}

		/* 获取下一个字符 */
		got_char = getchar();
	}

	/* name 是一个字符串，
	 * 所以要在它的尾部添加一个空字符
	 */
	*((attr->info.name) + offset) = '\0';

	print_without_endline("请输入学号: ");
	scanf("%d", &(attr->info.num));

	print_without_endline("请输入选修课成绩：");
	scanf("%f", &(attr->scores.optional));

	print_without_endline("请输入必修课成绩：");
	scanf("%f", &(attr->scores.required));

	print_without_endline("请输入实验课成绩：");
	scanf("%f", &(attr->scores.experimental));

	student_object_scores_sum(attr);
}

void insert_student(void)
{
	StudentAttribute attr;
	StudentObject object;
	unsigned char name[NAME_MAX_LEN];

	attr.info.name = name;

	clear_stdin();
	get_student_info_from_stdin(&attr, NAME_MAX_LEN);
	object = student_list_append(student_list, &attr);
	if (object == NULL) {
		print("申请内存失败，未能成功添加该学生信息");
		return;
	}

	print_student_info_title();
	show_student(object);
}

void remove_student(void)
{
	StudentAttribute attr;
	StudentObject object;

	clear_stdin();
	print_without_endline("请输入要删除的学号：");
	scanf("%u", &(attr.info.num));

	object = student_list_get_with_num(student_list, attr.info.num);
	if (object == NULL) {
		print("未找到该学号的学生");
		return;
	}

	print("找到如下学生：");
	print_student_info_title();
	show_student(object);

	clear_stdin();
	print_without_endline("确认删除吗？[Y/N] ");
	if (getchar_lower_case() == 'y') {
		student_list_remove(&student_list, object);
		print("已执行删除！");
		print("请在返回主菜单后选择序号“1”进行录入以保存设置");
	} else {
		print("用户取消，未执行删除");
	}
}

void find_student(void)
{
	StudentAttribute attr;
	StudentObject object;

	clear_stdin();
	print_without_endline("请输入要查找的学号：");
	scanf("%d", &(attr.info.num));

	object = student_list_get_with_num(student_list, attr.info.num);
	if (object == NULL) {
		print("未找到该学生");
		return;
	}

	print_student_info_title();
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

	clear_stdin();
	print_without_endline("请输入要修改学生的学号：");
	scanf("%u", &(attr.info.num));

	object = student_list_get_with_num(student_list, attr.info.num);
	if (object == NULL) {
		print("指定的学生不存在！");
		return;
	}

	print("找到如下学生：");
	print_student_info_title();
	show_student(object);

	attr.info.name = name;

	clear_stdin();
	get_student_info_from_stdin(&attr, NAME_MAX_LEN);
	student_object_modify(object, &attr);

	print("成功修改");
	print_student_info_title();
	show_student(object);
}

void pause(void)
{
	print_without_endline("按下回车键继续...");
	clear_stdin();
	getchar();
}

void load_from_file(void)
{
	FILE *fp;
	int fstat;
	StudentAttribute attr;
	unsigned char name[NAME_MAX_LEN];
	
	fp = fopen(INFO_DATA_FILE, "rb");
	if (fp == NULL) {
		print("data文件不存在！");
		return;
	} else {
		print("检测到data文件，准备导入");
	}

	fstat = ~EOF;
	attr.info.name = name;
	while (fstat != EOF) {
		fstat = fscanf(fp, "%s", attr.info.name);
		if (fstat == EOF)
			continue;

		fgetc(fp);
		fread(&(attr.info.num), 1, (sizeof(StudentAttribute) - sizeof(unsigned char *)), fp);
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
		clear_stdin();
		print("注意：当前为空列表，将清空文件原有的所有数据");
		print_without_endline("确认继续吗？[Y/N] ");
		if (getchar_lower_case() != 'y') {
			print("用户取消，未修改文件");
			return;
		}
	}

	fp = fopen(INFO_DATA_FILE, "wb");
	if (fp == NULL) {
		print("打开文件失败！");
		return;
	}

	list_generator = student_list;
	object = student_list_generate(&list_generator);
	while (object != NULL) {
		student_object_attr_export(object, &attr);

		fprintf(fp, "%s", attr.info.name);
		fputc(' ', fp);
		fwrite(&(attr.info.num), 1, (sizeof(StudentAttribute) - sizeof(unsigned char *)), fp);

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
			find_student();
			pause();
			break;
		case '3':
			remove_student();
			pause();
			break;
		case '4':
			modify_student();
			pause();
			break;
		case '5':
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
