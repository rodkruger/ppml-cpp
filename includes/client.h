//
// Created by rkruger on 23/10/24.
//

#ifndef CLIENT_H
#define CLIENT_H
#include "core.h"

namespace hermesml {
    class Client : EncryptedObject {
    public:
        explicit Client(const HEContext &ctx);

        [[nodiscard]] std::vector<Ciphertext<DCRTPoly> > Encrypt(const std::vector<int64_t> &data) const;

        [[nodiscard]] std::vector<Ciphertext<DCRTPoly> > Encrypt(const std::vector<std::vector<int64_t> > &data) const;

        [[nodiscard]] std::vector<Ciphertext<DCRTPoly> > EncryptCKKS(
            const std::vector<std::vector<double> > &data) const;

        [[nodiscard]] std::vector<Ciphertext<DCRTPoly> > EncryptCKKS(const std::vector<double> &data) const;
    };
}

#endif //CLIENT_H
