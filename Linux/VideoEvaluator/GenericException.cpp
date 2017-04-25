#include "GenericException.h"
GenericException::GenericException(const char* Msg) throw(){
    this->Msg = Msg;
}

const char* GenericException::what() const throw(){
    return Msg;
}
