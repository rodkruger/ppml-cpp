
#include "hemath.h"

namespace hermesml {

    Constants::Constants(const HEContext &ctx) : EncryptedObject(ctx) {
        Plaintext plaintext = this->GetCc()->MakePackedPlaintext({0});
        this->encryptedZero = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        plaintext = this->GetCc()->MakePackedPlaintext({1});
        this->encryptedOne = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);
        // this->GetCc()->EvalChebyshevFunction()
    }

    Ciphertext<DCRTPoly> Constants::Zero() {
        return this->encryptedZero;
    }

    Ciphertext<DCRTPoly> Constants::One() {
        return this->encryptedOne;
    }

}
