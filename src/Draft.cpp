
#include <iostream>
#include "context.h"
#include "client.h"

using namespace hermesml;

void EvalChebyshevFunction() {

    std::cout << "# Generate crypto context" << std::endl;

    auto ckksCtx = HEContextFactory().ckksHeContext();
    auto ckksClient = Client(ckksCtx);
    auto cc = ckksCtx.GetCc();
    auto publicKey = ckksCtx.GetPublicKey();
    auto privateKey = ckksCtx.GetPrivateKey();

    std::cout << "      Ring dimension: " << cc->GetRingDimension() << std::endl;
    std::cout << "      Modulus: " << cc->GetModulus() << std::endl;
    std::cout << "      Multiplicative depth: " << ckksCtx.GetMultiplicativeDepth() << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    uint32_t polyDegree = 10;
    double lowerBound = 0;
    double upperBound = 2;

    std::vector<std::complex<double>> input{2.0};
    Plaintext plaintext = cc->MakeCKKSPackedPlaintext(input);
    auto ciphertext = cc->Encrypt(publicKey, plaintext);

    auto result = cc->EvalChebyshevFunction([](double x) -> double { return std::sqrt(x); },
                                ciphertext,
                                lowerBound,
                                upperBound,
                                polyDegree);

    Plaintext plaintextDec;
    cc->Decrypt(privateKey, result, &plaintextDec);

    std::vector<std::complex<double>> finalResult = plaintextDec->GetCKKSPackedValue();
    std::cout << "Actual output\n\t" << finalResult << std::endl << std::endl;
}

int main() {

    EvalChebyshevFunction();

    return 0;
}