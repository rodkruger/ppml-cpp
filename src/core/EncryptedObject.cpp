#include "core.h"

namespace hermesml {

    EncryptedObject::EncryptedObject(const HEContext &ctx) {
        this->ctx = ctx;
        this->cc = this->ctx.GetCc();
    }

    CryptoContext<DCRTPoly> EncryptedObject::GetCc() const {
        return this->cc;
    }

    HEContext EncryptedObject::GetCtx() const {
        return this->ctx;
    }

}