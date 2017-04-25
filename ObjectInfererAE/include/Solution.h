#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <iostream>

using namespace std;

class Solution
{
    public:
        Solution(vector<float>* solution);
        virtual ~Solution();
        Solution* clone();
        vector<float>* getSolution() const;
        void setSolution(vector<float>* solution);
        bool operator==(const Solution &other) const;
        bool operator!=(const Solution &other) const;
        void print(int* RAMMap);
        void print(std::ofstream& of, int* RAMMap);
    private:
        vector<float>* solution;
};

#endif // SOLUTION_H
