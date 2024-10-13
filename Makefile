NAME = webserv

SRCS = $(wildcard *.cpp */*.cpp)

CPP = c++

HEADERS = $(wildcard *.hpp */*.hpp)
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

OBJS = $(SRCS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CPP) $(CPPFLAGS) $^ -o $@

%.o : %.cpp $(HEADERS)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all
