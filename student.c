#include "student.h"

#include <stdlib.h>


struct _student_list {
	struct _student_object student;
	struct _student_list *next;
};

#define list_next_is_null(list_addr) \
	(((list_addr)->next) == NULL)
#define list_is_empty(list_addr) \
	((*((unsigned int *)(list_addr)) == 0) && (list_next_is_null(list_addr)))
#define list_generate_next(list_addr_variable) \
	list_addr_variable = ((list_addr_variable)->next)

#define swap_variable(variable0, variable1) do {	\
	variable0 = (variable0) ^ (variable1);		\
	variable1 = (variable0) ^ (variable1);		\
	variable0 = (variable0) ^ (variable1);		\
} while (0)


student_base_t student_object_get_name_length(struct _student_object *object)
{
	student_base_t count;
	unsigned char *name_addr;

	count = 0;
	name_addr = object->info.name;

	while (*(name_addr++) != '\0')
		++count;

	return count;
}

student_bool_t student_list_is_empty(struct _student_list *restrict list)
{
	return list_is_empty(list);
}

static student_ret_t student_object_name_create(struct _student_object *object,
						student_base_t name_length)
{
	/* 字符串尾部要放空字符'0'，所以length + 1 */
	object->info.name =
		(unsigned char *)malloc(sizeof(unsigned char) * (name_length + 1));

	if ((object->info.name) == NULL)
		return STUDENT_MEM_ERROR;

	*(object->info.name + name_length) = '\0';
	return STUDENT_OK;
}

static void student_object_name_copy(struct _student_object *restrict src_object,
				     struct _student_object *restrict dest_object)
{
	student_base_t name_length;
	unsigned char *src_name_addr, *dest_name_addr;

	name_length = student_object_get_name_length(src_object);
	student_object_name_create(dest_object, name_length);

	src_name_addr = src_object->info.name;
	dest_name_addr = dest_object->info.name;

	while (name_length--)
		*(dest_name_addr++) = *(src_name_addr++);
}

static inline void student_object_name_remove(struct _student_object *object)
{
	if ((object->info.name) != NULL)
		free(object->info.name);
}

static void student_list_init(struct _student_list *list)
{
	unsigned int *u32_list;

	u32_list = (unsigned int *)list;

	*u32_list = 0;
	*u32_list += 
		((sizeof(struct _student_list) - sizeof(void *)) / sizeof(unsigned int));
	*u32_list = 0;
}

static void student_object_copy(struct _student_object *restrict object_src,
				struct _student_object *restrict object_dest)
{
	size_t u8_bytes, u32_bytes, uint32_size;
	unsigned char *u8_src, *u8_dest;
	unsigned int *u32_src, *u32_dest;

	u8_src = (unsigned char *)object_src;
	u8_dest = (unsigned char *)object_dest;
	u32_src = (unsigned int *)object_src;
	u32_dest = (unsigned int *)object_dest;
	uint32_size = sizeof(unsigned int);

	u8_bytes = sizeof(struct _student_object);
	if (u8_bytes >= uint32_size)
		u32_bytes = u8_bytes / uint32_size;
	else
		u32_bytes = 0;

	while (u32_bytes--) {
		*(u32_dest++) = *(u32_src++);
		u8_bytes -= uint32_size;
	}
	while (u8_bytes--) {
		*(u8_dest++) = *(u8_src++);
	}
}

void student_object_attr_export(struct _student_object *restrict object,
				struct _student_object *restrict attr)
{
	student_object_copy(object, attr);
}

void student_object_modify(struct _student_object *object,
			   struct _student_object *attr)
{
	student_object_name_remove(object);
	student_object_copy(attr, object);
	student_object_name_copy(attr, object);
}

static inline void student_object_free(struct _student_object *object)
{
	student_object_name_remove(object);
	free(object);
}

struct _student_list *student_list_create(void)
{
	struct _student_list *list;

	list = (struct _student_list *)malloc(sizeof(struct _student_list));
	if (list == NULL)
		return NULL;

	student_list_init(list);
	return list;
}

static inline struct _student_object *student_object_create(void)
{
	return (struct _student_object *)student_list_create();
}

struct _student_object *student_list_generate(struct _student_list **list)
{
	struct _student_object *object;

	if ((*list == NULL) || list_is_empty(*list))
		return NULL;

	object = (struct _student_object *)(*list);

	if (list_next_is_null(*list))
		*list = NULL;
	else
		list_generate_next(*list);

	return object;
}

struct _student_object *student_list_get_last(struct _student_list *list)
{
	while (! list_next_is_null(list))
		list_generate_next(list);

	return (struct _student_object *)list;
}

struct _student_object *student_list_get_with_num(struct _student_list *list,
						  student_base_t num)
{
	struct _student_object *object;

	object = student_list_generate(&list);
	while (object != NULL) {
		if ((object->info.num) == num)
			return object;
		else
			object = student_list_generate(&list);
	}

	return NULL;
}

struct _student_object *student_list_append(struct _student_list *list,
					    struct _student_object *attr)
{
	struct _student_object *object;

	if (list_is_empty(list)) {
		object = (struct _student_object *)list;
	} else {
		object = student_object_create();

		if (object == NULL)
			return object;
		if (! list_next_is_null(list))
			list = (struct _student_list *)student_list_get_last(list);

		list->next = (struct _student_list *)object;
	}

	student_object_copy(attr, object);
	student_object_name_copy(attr, object);
	return object;
}

student_ret_t student_list_remove(struct _student_list **list,
				  struct _student_object *object)
{
	struct _student_list *previous_list_node;

	if ((struct _student_object *)(*list) == object) {
		if (list_next_is_null(*list)) {
			student_object_name_remove(object);
			student_list_init(*list);
			return STUDENT_CAN_NOT_REMOVE;
		} else {
			*list = (*list)->next;
			student_object_free(object);
			return STUDENT_OK;
		}
	}

	previous_list_node = &(**list);
	list = &((*list)->next);

	while (*list != NULL) {
		if (*list == (struct _student_list *)object) {
			if (list_next_is_null(*list))
				previous_list_node->next = NULL;
			else
				previous_list_node->next = (*list)->next;

			student_object_free(object);
			return STUDENT_OK;
		}

		previous_list_node = *list;
		list = &((*list)->next);
	}

	return STUDENT_CAN_NOT_REMOVE;
}

student_ret_t student_list_remove_with_num(struct _student_list **list,
					   student_base_t num)
{
	struct _student_object *object;

	object = student_list_get_with_num(*list, num);
	if (object == NULL)
		return STUDENT_NOT_FOUND;

	student_list_remove(list, object);
	return STUDENT_OK;
}

student_base_t student_list_count(struct _student_list *list)
{
	student_base_t count;

	count = 0;
	if (list_is_empty(list))
		return count;

	while (! list_next_is_null(list)) {
		++count;
		list_generate_next(list);
	}

	return (count + 1);
}

void student_object_swap(struct _student_object *restrict object0,
			 struct _student_object *restrict object1)
{
	size_t u8_bytes, u32_bytes, uint32_size;
	unsigned char *u8_object0, *u8_object1;
	unsigned int *u32_object0, *u32_object1;
	
	u8_bytes = sizeof(struct _student_object);
	
	u8_object0 = (unsigned char *)object0;
	u8_object1 = (unsigned char *)object1;
	u32_object0 = (unsigned int *)object0;
	u32_object1 = (unsigned int *)object1;
	uint32_size = sizeof(unsigned int);

	if (u8_bytes >= uint32_size)
		u32_bytes = u8_bytes / uint32_size;
	else
		u32_bytes = 0;

	while (u32_bytes--) {
		swap_variable(*u32_object0, *u32_object1);
		++u32_object0;
		++u32_object1;

		u8_bytes -= uint32_size;
	}
	
	while (u8_bytes--) {
		swap_variable(*u8_object0, *u8_object1);
		++u8_object0;
		++u8_object1;
	}
}

void student_object_scores_sum(struct _student_object *object)
{
	size_t member_count;
	float *f32_object;

	member_count = sizeof(struct _student_scores) / sizeof(float);
	member_count -= 1;

	f32_object = (float *)(&(object->scores));

	object->scores.total = 0;
	while (member_count--)
		(object->scores.total) += *(f32_object++);
}

void student_list_sort_by_score(struct _student_list *list)
{
	student_base_t count;
	struct _student_list *list_node;

	if (list_is_empty(list))
		return;
	
	count = student_list_count(list);
	while (--count) {
		list_node = list;
		while (! list_next_is_null(list_node)) {
			if ((list_node->student.scores.total)
			    > (list_node->next->student.scores.total))
				student_object_swap((struct _student_object *)(list_node),
						    (struct _student_object *)(list_node->next));

			list_generate_next(list_node);
		}
	}
}

void student_list_delete(struct _student_list *list)
{
	if (list_is_empty(list))
		goto free_self;

	while (student_list_remove(&list, (struct _student_object *)list) 
	      != STUDENT_CAN_NOT_REMOVE)
		continue;
	
free_self:
	free(list);
}

