/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstcreate.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: whazami <whazami@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 17:46:00 by whazami           #+#    #+#             */
/*   Updated: 2021/06/08 17:46:04 by whazami          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstcreate(void *contents, size_t lst_size, size_t type_size)
{
	t_list	*begin_list;
	size_t	i;

	begin_list = ft_lstnew(&contents[0]);
	if (!begin_list)
		return (NULL);
	i = 1;
	while (i < lst_size)
	{
		ft_lstadd_back(&begin_list, ft_lstnew(&contents[i * type_size]));
		i++;
	}
	return (begin_list);
}
