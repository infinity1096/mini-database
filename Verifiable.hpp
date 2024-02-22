#ifndef VERIFIABLE_HPP
#define VERIFIABLE_HPP

class IVerifiable {
    public:
    virtual bool verify() const =0;
};

#endif