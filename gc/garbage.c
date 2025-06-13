#include "garbage_collector.h"

void	gc_malloc(size_t size, t_garbage **gc)
{
	void	*node;
	t_garbage	*gc_node;

	node = malloc(size);
	if (!node)
		return (NULL);
	gc_node = malloc(sizeof(t_garbage));
	if (!gc_node)
	{
		free(gc_node);
		return (NULL);
	}
	gc_node->ptr = node;
	gc_node->next = *gc;
	*gc = gc_node;
	return (node);
}

int	gc_add_ptr(void *ptr, t_garbage **gc)
{

}

void	gc_free_all(t_garbage **gc)
{

}