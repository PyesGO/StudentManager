## 概述  
帮助大学生们快速完成无聊的`用C语言编写学生管理系统`作业😆  
`student.c` 作为一个模块给外部调用，相关API声明在 `student.h` 中。  
#### 主要类型：
|类型名|注解|
|-|-|
|`StudentObject`|学生对象，存放学生个体信息的数据结构。由于涉及其内存管理，所以一般只在模块内部使用，当需要在外部访问时建议使用`StudentAttribute`搭配`student_object_attr_export`方法|
|`StudentList`|学生列表，继承自`StudentObject`，可与`StudentObject`同等使用，`StudentList`自身也是一个`StudentObject`|
|`StudentAttribute`|学生属性，提供在模块外部对`StudentObject`的访问|

#### 主要方法：
|方法名|参数类型|返回类型|概述|
|-|-|-|-|
|`student_list_create`|`StudentList`|`StudentList`|创建并初始化一个学生列表|
|`student_list_is_empty`|`StudentList`|`bool`|判断StudentList是否为空列表|
|`student_list_append`|`StudentAttribute`|`StudentObject` or `NULL`|在学生列表`StudentList`末端添加一个`StudentObject`（学生对象），并且返回添加成功的`StudentObject`学生对象。如果添加失败则返回`NULL`|
|`student_list_remove`|`StudentObject`|`student_ret_t`|移除列表中的某一个学生对象|
|`student_list_remove_with_num`|`student_base_t`|`student_ret_t`|通过匹配学号来删除学生，匹配失败返回状态码`STUDENT_NOT_FOUND`|
|`student_list_sort_by_score`|`StudentList`|无|通过学生总分来从小到大排序，排序将直接对列表进行修改|
|`student_list_delete`|`StudentList`|无|释放列表内存（emmm... 我好像没有写list clear），释放后的列表不可再使用|
|`student_list_get_last`|`StudentList`|`StudentObject`|返回列表的最后一个学生对象|
|`student_list_get_with_num`|`StudentList`|`StudentObject` or `NULL`|通过匹配学号来返回学生对象，匹配失败返回`NULL`|
|`student_list_generate`|`StudentList`|`StudentObject`|对`StudentList`进行迭代返回。第一次调用返回列表的第一个学生对象，往后每调用一次返回第n+1的学生对象，直到最后一个返回后再调用就返回`NULL`|
|`student_object_swap`|`StudentObject` `StudentObject`|无|将两个学生对象的信息互换|
|`student_object_attr_export`|`StudentObject` `StudentAttribute`|无|将学生对象的信息导出为可提供外部访问属性|
|`student_object_modify`|`StudentAttribute`|无|修改学生信息|
|`student_object_scores_sum`|`StudentAttribute`|无|对学生的所有成绩进行求和|
|`student_object_get_name_length`|`StudentObject`|`student_base_t`|返回学生对象的名字长度|

`student.c`对`StudentObject`的耦合并不高，保持`_student_object`结构体中`info.name`和`info.num`以及`scores.total`的存在即可，除此之外其它操作都使用指针，并不关心结构体内部的具体内容，方便对其改写或扩展。

模块具体使用方式可查看`example.c`中的源代码。
### Example 程序图片预览
![screen-shots](https://github.com/user-attachments/assets/8928c06c-f8b5-40a5-9a91-ec1a6d97cebc)
![screen-shots-01](https://github.com/user-attachments/assets/b3c9bc07-c64c-4f5a-bfaa-fe5dd0384d5f)
![screen-shots-02](https://github.com/user-attachments/assets/fa0322b5-ea57-4717-afc2-a86de95ec31c)
