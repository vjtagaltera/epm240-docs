
/* https://cyberdynesystems.ai/how-to-speed-up-deep-learning-inference-using-tensorrt/ */

#include "cudaWrapper.h"
#include "ioHelper.h"
#include 
#include 
#include 
#include 
#include 
#include 
#include 
#include 

using namespace nvinfer1;
using namespace std;
using namespace cudawrapper;

static Logger gLogger;

// Maxmimum absolute tolerance for output tensor comparison against reference
constexpr double ABS_EPSILON = 0.005;
// Maxmimum relative tolerance for output tensor comparison against reference
constexpr double REL_EPSILON = 0.05;

ICudaEngine* createCudaEngine(string const& onnxModelPath, int batchSize)
{
    unique_ptr builder{createInferBuilder(gLogger)};
    unique_ptr network{builder->createNetwork()};
    unique_ptr parser{nvonnxparser::createParser(*network, gLogger)};

    if (!parser->parseFromFile(onnxModelPath.c_str(), static_cast(ILogger::Severity::kINFO)))
    {
        cout << "ERROR: could not parse input engine." << endl;
        return nullptr; 
    }

    // Build TensorRT engine optimized based on batch size of input data
    builder->setMaxBatchSize(batchSize); //???

    return builder->buildCudaEngine(*network); // build and return TensorRT engine
}

static int getBindingInputIndex(IExecutionContext* context)

{
    return !context->getEngine().bindingIsInput(0); // 0 (false) if bindingIsInput(0), 1 (true) otherwise
}

void launchInference(IExecutionContext* context, cudaStream_t stream, vector const& inputTensor, vector& outputTensor, void** bindings, int batchSize)
{
    int inputId = getBindingInputIndex(context);

    cudaMemcpyAsync(bindings[inputId], inputTensor.data(), inputTensor.size() * sizeof(float), cudaMemcpyHostToDevice, stream);
    context->enqueue(batchSize, bindings, stream, nullptr);
    cudaMemcpyAsync(outputTensor.data(), bindings[1 - inputId], outputTensor.size() * sizeof(float), cudaMemcpyDeviceToHost, stream);
}

void softmax(vector& tensor, int batchSize)
{
    size_t batchElements = tensor.size() / batchSize;

    for (int i = 0; i < batchSize; ++i)
    {
        float* batchVector = &tensor[i * batchElements];
        double maxValue = *max_element(batchVector, batchVector + batchElements);
        double expSum = accumulate(batchVector, batchVector + batchElements, 0.0, [=](double acc, float value) { return acc + exp(value - maxValue); });

        transform(batchVector, batchVector + batchElements, batchVector, [=](float input) { return static_cast(std::exp(input - maxValue) / expSum); });
    }
}

void verifyOutput(vector const& outputTensor, vector const& referenceTensor)
{
    for (size_t i = 0; i < referenceTensor.size(); ++i)
    {
        double reference = static_cast(referenceTensor[i]);
        // Check absolute and relative tolerance
        if (abs(outputTensor[i] - reference) > max(abs(reference) * REL_EPSILON, ABS_EPSILON))
        {
            cout << "ERROR: mismatch at position " << i;
            cout << " expected " << reference << ", but was " << outputTensor[i] << endl;
            return;
        }
    }

    cout << "OK" << endl;
}

int main(int argc, char* argv[])
{
    // declaring cuda engine
    unique_ptr engine{nullptr};

    // declaring execution context
    unique_ptr context{nullptr};
    vector inputTensor;
    vector outputTensor;
    vector referenceTensor;
    void* bindings[2]{0};
    vector inputFiles;
    CudaStream stream;

    if (argc != 3)
    {
        cout << "usage: " << argv[0] << "  " << endl; 
        return 1; 
    } 

    string onnxModelPath(argv[1]); 
    inputFiles.push_back(string{argv[2]}); //???
    int batchSize = inputFiles.size(); 

    // Create Cuda Engine 
    engine.reset(createCudaEngine(onnxModelPath, batchSize)); 
    if (!engine) return 1; 

    // Assume networks takes exactly 1 input tensor and outputs 1 tensor 
    assert(engine->getNbBindings() == 2);
    assert(engine->bindingIsInput(0) ^ engine->bindingIsInput(1));

    for (int i = 0; i < engine->getNbBindings(); ++i)
    {
        Dims dims{engine->getBindingDimensions(i)};
        size_t size = accumulate(dims.d, dims.d + dims.nbDims, batchSize, multiplies());
        // Create CUDA buffer for Tensor
        cudaMalloc(&bindings[i], size * sizeof(float));

        // Resize CPU buffers to fit Tensor
        if (engine->bindingIsInput(i))
            inputTensor.resize(size);
        else
            outputTensor.resize(size);
    }

    // Read input tensor from ONNX file
    if (readTensor(inputFiles, inputTensor) != inputTensor.size())
    {
        cout << "Couldn't read input Tensor" << endl; 
        return 1; 
    } 

    // Create Execution Context 
    context.reset(engine->createExecutionContext());

    launchInference(context.get(), stream, inputTensor, outputTensor, bindings, batchSize);
    // wait until the work is finished
    cudaStreamSynchronize(stream);

    vector referenceFiles;
    for (string path : inputFiles)
        referenceFiles.push_back(path.replace(path.rfind("input"), 5, "output"));

    // try to read and compare against reference tensor from protobuf file
    referenceTensor.resize(outputTensor.size());
    if (readTensor(referenceFiles, referenceTensor) != referenceTensor.size())
    {
        cout << "Couldn't read reference Tensor" << endl;
        return 1;
    }

    // Apply a softmax on the CPU to create a normalized distribution suitable for measuring relative error in probabilities.
    softmax(outputTensor, batchSize);
    softmax(referenceTensor, batchSize);

    verifyOutput(outputTensor, referenceTensor);

    for (void* ptr : bindings)
        cudaFree(ptr);

    return 0;
}


