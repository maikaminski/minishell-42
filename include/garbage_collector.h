#ifndef GC_H
# define GC_H

# include <stdlib.h>

// garbage collector
typedef struct s_garbage
{
	void	*ptr;
	struct s_garbage 	*next;
} t_garbage;

#endif