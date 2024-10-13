/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiPipe.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-mal <aait-mal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 14:25:07 by aait-mal          #+#    #+#             */
/*   Updated: 2024/03/06 14:27:55 by aait-mal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/cgi.hpp"

Pipe::Pipe() {
    if (pipe(pipefd) == -1) {
        std::cerr << "Error creating pipe.\n";
        throw std::runtime_error("Error creating pipe");
    }
}

Pipe::~Pipe() {
    close(pipefd[0]);
    close(pipefd[1]);
}

int Pipe::getReadEnd() {
    return pipefd[0];
}

int Pipe::getWriteEnd() {
    return pipefd[1];
}