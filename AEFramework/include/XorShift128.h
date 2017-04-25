#ifndef XORSHIFT128_H
#define XORSHIFT128_H
#include <stdint.h>
#include <Random.h>

class XorShift128 : public Random{
    private:
        uint32_t* kn;
        float* fn;
        float* wn;
        uint64_t s[2];
        uint64_t next();
        float r4_nor2(uint32_t kn[128], float fn[128], float wn[128] );
        void r4_nor2_setup ( uint32_t kn[128], float fn[128], float wn[128] );
    public:
        XorShift128 ();
        virtual ~XorShift128 ();

        unsigned int nextInt();
        unsigned int nextInt(unsigned int n);
        float nextFloat();
        double nextDouble();
        double nextGaussian();
        bool nextBool();
};

#endif
