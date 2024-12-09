//
// Created by rkruger on 23/10/24.
//

#ifndef CLIENT_H
#define CLIENT_H
#include "core.h"

namespace hermesml {

    class Client : EncryptedObject {
    public:
        explicit Client(HEContext ctx);
        std::vector<Ciphertext<DCRTPoly>> Encrypt(std::vector<int64_t> data) ;
        std::vector<Ciphertext<DCRTPoly>> Encrypt(std::vector<std::vector<int64_t>> data) ;
        std::vector<Ciphertext<DCRTPoly>> EncryptCKKS(std::vector<std::vector<double>> data) ;
        std::vector<Ciphertext<DCRTPoly>> EncryptCKKS(std::vector<double> data) ;
    };

}

#endif //CLIENT_H
