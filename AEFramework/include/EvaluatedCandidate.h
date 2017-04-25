#ifndef EVALUATED_CANDIDATE_H
#define EVALUATED_CANDIDATE_H


template <class T>
class EvaluatedCandidate
{
    public:
        T* candidate;
        double fitness;
        EvaluatedCandidate(){}

        EvaluatedCandidate(T* candidate, double fitness){
            this->candidate = candidate;
            this->fitness = fitness;
        }

        virtual ~EvaluatedCandidate(){
            delete candidate;
        }

        EvaluatedCandidate<T>* clone(){
            return new EvaluatedCandidate<T>(candidate->clone(), fitness);
        }
};

#endif
