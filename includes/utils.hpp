/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obelaizi <obelaizi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 12:26:12 by aait-mal          #+#    #+#             */
/*   Updated: 2024/03/30 00:51:43 by obelaizi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include "../includes/webserve.hpp"
std::vector<std::string> splitWithChar(std::string s, char delim);
std::vector<std::string> splitString(const std::string& input, const std::string& delimiter);
std::string ftToString(size_t n);
char hexToCharacters(const std::string& hex);
std::string decodeUrl(const std::string &srcString);
std::string lower(std::string str);
size_t hexToDec(const std::string &s, Request &request);
size_t custAtoi(const std::string &s);
void writeOnFile(const std::string &filename, const std::string &content);
std::vector<std::string> customSplitRequest(const std::string& input, const std::string& delimiter);
bool checkLimitRead(Request &request, size_t bodySize);

#endif
