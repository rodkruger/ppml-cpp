#include "openfhe.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

using namespace lbcrypto;

// Function to compute the homomorphic Manhattan distance between two encrypted points
Ciphertext<DCRTPoly> HomomorphicManhattanDistance(
    const std::vector<Ciphertext<DCRTPoly>>& encrypted_point1,
    const std::vector<Ciphertext<DCRTPoly>>& encrypted_point2,
    CryptoContext<DCRTPoly> cc,
    KeyPair<DCRTPoly> keys) {

    auto distance = cc->Encrypt(keys.publicKey, cc->MakePackedPlaintext({0}));
    
    for (size_t i = 0; i < encrypted_point1.size(); ++i) {
        // Calculate the difference between the two coordinates
        auto diff = cc->EvalSub(encrypted_point1[i], encrypted_point2[i]);
        
        // Square the difference to approximate absolute value
        auto abs_diff = cc->EvalMult(diff, diff); // Homomorphic squaring to remove the sign
        
        // Add to total distance
        distance = cc->EvalAdd(distance, abs_diff);
    }
    
    return distance;
}

// Function to perform k-NN using homomorphic encryption
std::vector<Ciphertext<DCRTPoly>> HomomorphicKNN(
    const std::vector<std::vector<Ciphertext<DCRTPoly>>>& encrypted_training_data,
    const std::vector<int>& training_labels,
    const std::vector<Ciphertext<DCRTPoly>>& encrypted_test_point,
    size_t k,
    CryptoContext<DCRTPoly> cc,
    KeyPair<DCRTPoly> keys) {
    
    size_t num_training_points = encrypted_training_data.size();
    std::vector<Ciphertext<DCRTPoly>> encrypted_distances;
    
    // Step 1: Compute distances from the test point to all training points
    for (size_t i = 0; i < num_training_points; ++i) {
        auto distance = HomomorphicManhattanDistance(encrypted_training_data[i], encrypted_test_point, cc, keys);
        encrypted_distances.push_back(distance);
    }

    // Step 2: Decrypt distances for sorting (because sorting in encrypted domain is non-trivial)
    std::vector<int> decrypted_distances(num_training_points);
    
    for (size_t i = 0; i < num_training_points; ++i) {
        Plaintext plaintext_distance;
        cc->Decrypt(keys.secretKey, encrypted_distances[i], &plaintext_distance);
        decrypted_distances[i] = plaintext_distance->GetPackedValue()[0]; // Get the decrypted distance as an integer
    }
    
    // Step 3: Sort the decrypted distances and find k-nearest neighbors
    std::vector<size_t> indices(num_training_points);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::sort(indices.begin(), indices.end(), [&decrypted_distances](size_t i1, size_t i2) {
        return decrypted_distances[i1] < decrypted_distances[i2];
    });
    
    // Step 4: Determine the majority label among the k-nearest neighbors
    std::vector<int> label_count(2, 0); // Assuming binary classification (0 or 1)
    
    for (size_t i = 0; i < k; ++i) {
        label_count[training_labels[indices[i]]]++; // Increment the count for the label of the ith nearest neighbor
    }
    
    int predicted_label = (label_count[0] > label_count[1]) ? 0 : 1; // Majority vote
    
    // Step 5: Encrypt the predicted label for output
    Plaintext plaintext_label = cc->MakePackedPlaintext({predicted_label});
    auto encrypted_label = cc->Encrypt(keys.publicKey, plaintext_label);
    
    return { encrypted_label };
}

int main() {
    using namespace lbcrypto;

    // Step 1: Set up the BFV context
    CCParams<CryptoContextBFVRNS> parameters;
    parameters.SetPlaintextModulus(65537);  // Prime number!
    parameters.SetMultiplicativeDepth(2);
    parameters.SetSecurityLevel(HEStd_128_classic);

    // Generate the crypto context
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);

    // Enable necessary algorithms (encryption, decryption, etc.)
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);

    // Step 2: Key generation
    auto keys = cc->KeyGen();

    // From this point --> Provider

    // Generate relinearization keys
    cc->EvalMultKeyGen(keys.secretKey);

    // Generate keys for rotation (optional, but necessary for some schemes)
    cc->EvalAtIndexKeyGen(keys.secretKey, {1, -1});

    // Step 2: Encrypt the training data and test point
    // Example training data with 3 points (2D points) and binary labels {0, 1, 0}
    std::vector<std::vector<int64_t>> training_data = { {1, 2}, {3, 4}, {5, 6} };
    std::vector<int> training_labels = { 0, 1, 0 };
    
    std::vector<std::vector<Ciphertext<DCRTPoly>>> encrypted_training_data;
    for (const auto& point : training_data) {
        std::vector<Ciphertext<DCRTPoly>> encrypted_point;
        for (int64_t value : point) {
            Plaintext plaintext_point = cc->MakePackedPlaintext({value});
            encrypted_point.push_back(cc->Encrypt(keys.publicKey, plaintext_point));
        }
        encrypted_training_data.push_back(encrypted_point);
    }
    
    // Encrypt a test point {2, 3}
    std::vector<int64_t> test_point = {2, 3};
    std::vector<Ciphertext<DCRTPoly>> encrypted_test_point;
    for (int64_t value : test_point) {
        Plaintext plaintext_test = cc->MakePackedPlaintext({value});
        encrypted_test_point.push_back(cc->Encrypt(keys.publicKey, plaintext_test));
    }
    
    // Step 3: Perform homomorphic k-NN
    size_t k = 1; // We will use k = 1 for simplicity
    auto encrypted_prediction = HomomorphicKNN(encrypted_training_data, training_labels, encrypted_test_point, k, cc, keys);
    
    // Step 4: Decrypt and display the result
    Plaintext result;
    cc->Decrypt(keys.secretKey, encrypted_prediction[0], &result);
    
    std::cout << "Predicted label: " << result->GetPackedValue()[0] << std::endl;
    
    return 0;
}

// IDE: CLion