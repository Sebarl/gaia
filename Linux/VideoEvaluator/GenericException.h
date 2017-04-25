#ifndef GENERIC_EXCEPTION_H
#define GENERIC_EXCEPTION_H

#include <exception>

class GenericException: public std::exception{
  public:
      GenericException(const char* Msg) throw();
      virtual const char* what() const throw();
  private:
        const char* Msg;
};

#endif
