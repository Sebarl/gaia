#include "Solution.h"
#include <fstream>
#include "Constants.h"

Solution::Solution(std::vector<float>* solution){
    this->solution = solution;
}

Solution::~Solution(){
    //for(unsigned int i = 0; i < solution->size(); i++){
    //    delete (*solution)[i];
    //}
    delete solution;
}

Solution* Solution::clone(){
    std::vector<float>* otherSolution = new std::vector<float>();
    for(unsigned int i = 0; i < solution->size(); i++){
        otherSolution->push_back((*solution)[i]);
    }
    return new Solution(otherSolution);
}

std::vector<float>* Solution::getSolution() const{
    return solution;
}

void Solution::setSolution(std::vector<float>* solution){
    this->solution = solution;
}

bool Solution::operator==(const Solution &other) const{
    std::vector<float>* otherSolution = other.getSolution();
    for(unsigned int i = 0; i < otherSolution->size(); i++){
        if((*otherSolution)[i] != (*solution)[i]){
            return false;
        }
    }
    return true;
}

bool Solution::operator!=(const Solution &other) const{
    return !(*this == other);
}

void Solution::print(int* RAMMap){
    std::cout << this->solution->size() << " ";
	for(unsigned int i = 0; i < this->solution->size(); i++){
		std::cout <<  RAMMap[i] << ":" << (*solution)[i] << " ";
	}
}

void Solution::print(std::ofstream& of, int* RAMMap){
    of << this->solution->size() << " ";
	for(unsigned int i = 0; i < this->solution->size(); i++){
		of <<  RAMMap[i] << ":" << (*solution)[i] << " ";
	}
}
