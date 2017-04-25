#ifndef NEATLINK_H
#define NEATLINK_H

#include <iostream>
#include "Random.h"

class NEATLink {
    public:
        //IDs of neurons connected by this link
        int fromNeuron, toNeuron;

        float weight;

        bool recurrent;

        bool enabled;

        int innovationId;

        NEATLink(int in, int out, float weight, bool enabled, int innovationId);
        NEATLink(int in, int out, float weight, bool recurrent, bool enabled, int innovationId);
        virtual ~NEATLink();

        void resetWeight(Random* rng);
        void mutateWeight(Random* rng);
        NEATLink* clone();

    friend bool operator<(const NEATLink& l, const NEATLink& r) {
        return (l.innovationId < r.innovationId);
    }

    //overload << for streaming
    friend std::ostream& operator <<(std::ostream &os, const NEATLink &link) {
        os << "LINK\n\tInnovID: " << link.innovationId << "\n"
           << "\tFrom: " << link.fromNeuron << "\n"
           << "\tTo: " << link.toNeuron << "\n"
           << "\tWeight: " << link.weight << "\n"
           << "\tEnabled: " << link.enabled << "\n"
           << "\tRecurrent: " << link.recurrent << "\n";
        return os;
    }

};

#endif // NEATLINK_H
