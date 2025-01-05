#include <iostream>
#include "context.h"

using namespace hermesml;

int main(int argc, const char *argv[]) {
    const auto heContext = HEContextFactory::ckksHeContext();
    const auto cc = heContext.GetCc();
    const auto publicKey = heContext.GetPublicKey();
    const auto privateKey = heContext.GetPrivateKey();

    std::cout << "Multiplicative depth: " << heContext.GetMultiplicativeDepth() << std::endl;
    std::cout << "Levels after bootstrapping: " << heContext.GetLevelsAfterBootstrapping() << std::endl;

    const auto plaintext1 = cc->MakeCKKSPackedPlaintext(std::vector<double>{1});
    const auto plaintext2 = cc->MakeCKKSPackedPlaintext(std::vector<double>{1});

    const auto etext1 = cc->Encrypt(publicKey, plaintext1);
    const auto etext2 = cc->Encrypt(publicKey, plaintext2);

    uint16_t levelsLeft;
    Ciphertext<DCRTPoly> eresult;
    Plaintext dresult;

    // This multiplication can run forever without bootstrapping. The next result does not depend from the previous, so,
    // the multiplicative depth does not increase loop by loop.
    levelsLeft = heContext.GetMultiplicativeDepth();
    for (int i = 0; i < 100; i++) {
        eresult = cc->EvalMult(etext1, etext2);
        levelsLeft--;
        std::cout << "\r" << i << ":EvalMult()" << " - " << levelsLeft << " levels remaining" << std::flush;
    }

    cc->Decrypt(privateKey, eresult, &dresult);
    std::cout << std::endl << "    >>> Result: " << dresult->GetCKKSPackedValue()[0] << std::endl;

    // The multiplication below can only run for (multiplicative depth) times, until the error turns too high. The last
    // round should be bootstrapping.
    eresult = etext2;
    levelsLeft = heContext.GetMultiplicativeDepth();
    for (int i = 0; i < levelsLeft; i++) {
        eresult = cc->EvalMult(etext1, eresult);
        levelsLeft--;
        std::cout << "\r" << i << ":EvalMult()" << " - " << levelsLeft << " levels remaining" << std::flush;
    }

    cc->Decrypt(privateKey, eresult, &dresult);
    std::cout << std::endl << "    >>> Result: " << dresult->GetCKKSPackedValue()[0] << std::endl;

    // The multiplication below can run forever, :)
    levelsLeft = heContext.GetMultiplicativeDepth();
    eresult = etext2;
    for (int i = 0; i < 1000; i++) {
        if (levelsLeft == 1) {
            // The last level should execute a bootstrapping or the last arithmetic operation
            std::cout << std::endl << "    Bootstrapping" << std::endl;
            eresult = cc->EvalBootstrap(eresult);
            levelsLeft = heContext.GetLevelsAfterBootstrapping();
        }

        eresult = cc->EvalMult(etext1, eresult);
        levelsLeft--;
        std::cout << "\r" << i << ":EvalMult()" << " - " << levelsLeft << " levels remaining" << std::flush;
    }

    cc->Decrypt(privateKey, eresult, &dresult);
    std::cout << std::endl << "    >>> Result: " << dresult->GetCKKSPackedValue()[0] << std::endl;

    return 0;
}
