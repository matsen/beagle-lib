/**
 * @file beagle.h
 *
 * @brief This file documents the API as well as header for the
 * Broad-platform Evolutionary Analysis General Likelihood Evaluator
 *
 * OVERVIEW
 *
 * DEFINITION OF KEY CONCEPTS: INSTANCE, BUFFER, etc.
 *
 * @author Likelihood API Working Group
 *
 * @author Daniel Ayres
 * @author Adam Bazinet
 * @author Peter Beerli
 * @author Michael Cummings
 * @author Aaron Darling
 * @author Mark Holder
 * @author John Huelsenbeck
 * @author Paul Lewis
 * @author Michael Ott
 * @author Andrew Rambaut
 * @author Marc Suchard
 * @author David Swofford
 * @author Derrick Zwickl
 *
 */

#ifndef __beagle__
#define __beagle__

/**
 * @anchor BEAGLE_RETURN_CODES
 *
 * @brief Error return codes
 *
 * This enumerates all possible BEAGLE return codes.  Error codes are always negative.
 */
enum BeagleReturnCodes {
    NO_ERROR                     = 0,   /**< Success */
    GENERAL_ERROR                = -1,  /**< Unspecified error */
    OUT_OF_MEMORY_ERROR          = -2,  /**< Not enough memory could be allocated */
    UNIDENTIFIED_EXCEPTION_ERROR = -3,  /**< Unspecified exception */
    UNINITIALIZED_INSTANCE_ERROR = -4,  /**< The instance index is out of range,
                                          *   or the instance has not been created */
    OUT_OF_RANGE_ERROR           = -5   /**< One of the indices specfied exceeded the range of the
                                          *   array */
};

/**
 * @anchor BEAGLE_FLAGS
 *
 * @brief Hardware and implementation capability flags
 *
 * This enumerates all possible hardware and implementation capability flags.
 * Each capability is a bit in a 'long'
 */
enum BeagleFlags {
    DOUBLE = 1 << 0,    /**< Request/require double precision computation */
    SINGLE = 1 << 1,    /**< Request/require single precision computation */
    ASYNCH = 1 << 2,    /**< Request/require asynchronous computation */
    SYNCH  = 1 << 3,    /**< Request/require synchronous computation */
    CPU    = 1 << 16,   /**< Request/require CPU */
    GPU    = 1 << 17,   /**< Request/require GPU */
    FPGA   = 1 << 18,   /**< Request/require FPGA */
    SSE    = 1 << 19,   /**< Request/require SSE */
    CELL   = 1 << 20    /**< Request/require Cell */
};

/**
 * @brief Information about a specific instance
 */
typedef struct {
    int resourceNumber; /**< Resource upon which instance is running */
    long flags;         /**< Bit-flags that characterize the activate
                          *   capabilities of the resource for this instance */
} InstanceDetails;

/**
 * @brief Description of a hardware resource
 */
typedef struct {
    char* name; /**< Name of resource as a NULL-terminated character string */
    long flags; /**< Bit-flags of capabilities on resource */
} Resource;

/**
 * @brief List of hardware resources
 */
typedef struct {
    Resource* list; /**< Pointer list of resources */
    int length;     /**< Length of list */
} ResourceList;

/* using C calling conventions so that C programs can successfully link the beagle library
 * (brace is closed at the end of this file) 
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 *
 * LONG DESCRIPTION
 *
 * @return A list of resources available to the library as a ResourceList array
 */
ResourceList* getResourceList();

/**
 * @brief Create a single instance
 *
 * This function creates a single instance of the BEAGLE library and can be called
 * multiple times to create multiple data partition instances each returning a unique
 * identifier.
 *
 * @param tipCount              Number of tip data elements (input)
 * @param partialsBufferCount   Number of partials buffers to create (input)
 * @param compactBufferCount    Number of compact state representation buffers to create (input)
 * @param stateCount            Number of states in the continuous-time Markov chain (input)
 * @param patternCount          Number of site patterns to be handled by the instance (input)
 * @param eigenBufferCount      Number of rate matrix eigen-decomposition buffers to allocate
 *                               (input)
 * @param matrixBufferCount     Number of rate matrix buffers (input)
 * @param categoryCount         Number of rate categories (input)
 * @param resourceList          List of potential resources on which this instance is allowed
 *                               (input, NULL implies no restriction)
 * @param resourceCount         Length of resourceList list (input)
 * @param preferenceFlags       Bit-flags indicating preferred implementation charactertistics,
 *                               see BeagleFlags (input)
 * @param requirementFlags      Bit-flags indicating required implementation characteristics,
 *                               see BeagleFlags (input) 
 *
 * @return the unique instance identifier (<0 if failed, see @ref BEAGLE_RETURN_CODES
 * "BeagleReturnCodes")
 */
int createInstance(int tipCount,
                   int partialsBufferCount,
                   int compactBufferCount,
                   int stateCount,
                   int patternCount,
                   int eigenBufferCount,
                   int matrixBufferCount,
                   int categoryCount,
                   int* resourceList,
                   int resourceCount,
                   long preferenceFlags,
                   long requirementFlags);

/**
 * @brief Initialize the instance
 *
 * This function initializes the instance by selecting the hardware upon this instance will run,
 * allocating memory and populating this memory of values that may have been set.
 *
 * @param instance      Instance number to initialize (input)
 * @param returnInfo    Pointer to return hardware details 
 *
 * @returns Information about the implementation and hardware on which this instance will run
 */
int initializeInstance(int instance,
                       InstanceDetails* returnInfo);

/**
 * @brief Finalize this instance
 *
 * This function finalizes the instance by releasing allocated memory
 *
 * @param instance  Instance number 
 *
 * @return error code
 */
int finalize(int instance);

/**
 * @brief Set an instance partials buffer
 *
 * This function copies an array of partials into an instance buffer. The inPartials array should
 * be stateCount * patternCount * categoryCount in length. For most applications this will be used
 * to set the partial likelihoods for the observed states. In such cases, the partials should be
 * copied categoryCount times.
 *
 * @param instance      Instance number in which to set a partialsBuffer (input)
 * @param bufferIndex   Index of destination partialsBuffer (input)
 * @param inPartials    Pointer to partials values to set (input) 
 *
 * @return error code
 */
int setPartials(int instance,
                int bufferIndex,
                const double* inPartials);

/**
 * @brief Get partials from an instance buffer
 *
 * This function copies an instance buffer into the array outPartials. The outPartials array should
 * be stateCount * patternCount * categoryCount in length.
 *
 * @param instance      Instance number from which to get partialsBuffer (input)
 * @param bufferIndex   Index of source partialsBuffer (input)
 * @param outPartials   Pointer to which to receive partialsBuffer (output) 
 *
 * @return error code
 */
int getPartials(int instance,
                int bufferIndex,
                double* outPartials);

/**
 * @brief Set the compact state representation for tip node
 *
 * This function copies a compact state representation into an instance buffer.
 * Compact state representation is an array of states: 0 to stateCount - 1 (missing = stateCount).
 * The inStates array should be patternCount in length (replication across categoryCount is not
 * required).
 *
 * @param instance  Instance number (input)
 * @param tipIndex  Index of destination compactBuffer (input)
 * @param inStates  Pointer to compact states (input) 
 *
 * @return error code
 */
int setTipStates(int instance,
                 int tipIndex,
                 const int* inStates);

/**
 * @brief Set an eigen-decomposition buffer
 *
 * This function copies an eigen-decomposition into an instance buffer.
 *
 * @param instance              Instance number (input)
 * @param eigenIndex            Index of eigen-decomposition buffer (input)
 * @param inEigenVectors        Flattened matrix (stateCount x stateCount) of eigen-vectors (input)
 * @param inInverseEigenVectors Flattened matrix (stateCount x stateCount) of inverse-eigen- vectors
 *                               (input)
 * @param inEigenValues         Vector of eigenvalues 
 * 
 * @return error code
 */
int setEigenDecomposition(int instance,
                          int eigenIndex,
                          const double* inEigenVectors,
                          const double* inInverseEigenVectors,
                          const double* inEigenValues);
    
/**
 * @brief Set category rates
 *
 * This function sets the vector of category rates for an instance.
 *
 * @param instance              Instance number (input)
 * @param inCategoryRates       Array containing categoryCount rate scalers (input)
 * 
 * @return error code
 */
int setCategoryRates(int instance,
                     const double* inCategoryRates);

/**
 * @brief Calculate a list of transition probability matrices
 *
 * This function calculates a list of transition probabilities matrices and their first and
 * second derivatives (if requested).
 *
 * @param instance                  Instance number (input)
 * @param eigenIndex                Index of eigen-decomposition buffer (input)
 * @param probabilityIndices        List of indices of transition probability matrices to update
 *                                   (input)
 * @param firstDerivativeIndices    List of indices of first derivative matrices to update
 *                                   (input, NULL implies no calculation)
 * @param secondDervativeIndices    List of indices of second derivative matrices to update
 *                                   (input, NULL implies no calculation)
 * @param edgeLengths               List of edge lengths with which to perform calculations (input)
 * @param count                     Length of lists 
 *
 * @return error code
 */
int updateTransitionMatrices(int instance,
                             int eigenIndex,
                             const int* probabilityIndices,
                             const int* firstDerivativeIndices,
                             const int* secondDervativeIndices,
                             const double* edgeLengths,
                             int count);

/**
 * @brief Set a finite-time transition probability matrix
 *
 * This function copies a finite-time transition probability matrix into a matrix buffer. This function
 * is used when the application wishes to explicitly set the transition probability matrix rather than
 * using the setEigenDecomposition and updateTransitionMatrices functions. The inMatrix array should be
 * of size stateCount * stateCount * categoryCount and will contain one matrix for each rate category.
 *
 * @param instance      Instance number (input)
 * @param matrixIndex   Index of matrix buffer (input)
 * @param inMatrix      Pointer to source transition probability matrix (input)
 *
 * @return error code
 */
int setTransitionMatrix(int instance,
                        int matrixIndex,
                        const double* inMatrix);

/**
 * @brief Calculate or queue for calculation partials using a list of operations
 *
 * This function either calculates or queues for calculation a list partials. Implementations
 * supporting SYNCH may queue these calculations while other implementations perform these
 * operations immediately.  Implementations supporting GPU may perform all operations in the list
 * simultaneously.
 *
 * Operations list is a list of 6-tuple integer indices, with one 6-tuple per operation.
 * Format of 6-tuple operation: {destinationPartials,
 *                               destinationScalingFactors, (this index must be > tipCount)
 *                               child1Partials,
 *                               child1TransitionMatrix,
 *                               child2Partials,
 *                               child2TransitionMatrix}
 *
 * @param instance          List of instances for which to update partials buffers (input)
 * @param instanceCount     Length of instance list (input)
 * @param operations        List of 6-tuples specifying operations (input)
 * @param operationCount    Number of operations (input)
 * @param rescale           Specify whether (=1) or not (=0) to recalculate scaling factors 
 *
 * @return error code
 */
int updatePartials(const int* instance,
                   int instanceCount,
                   const int* operations,
                   int operationCount,
                   int rescale);

/**
 * @brief Block until all calculations that write to the specified partials have completed.
 *
 * This function is optional and only has to be called by clients that "recycle" partials.
 *
 * If used, this function must be called after an updatePartials call and must refer to
 * indices of "destinationPartials" that were used in a previous updatePartials
 * call.  The library will block until those partials have been calculated.
 *
 * @param instance                  List of instances for which to update partials buffers (input)
 * @param instanceCount             Length of instance list (input)
 * @param destinationPartials       List of the indices of destinationPartials that must be
 *                                   calculated before the function returns
 * @param destinationPartialsCount  Number of destinationPartials (input) 
 *
 * @return error code
 */
int waitForPartials(const int* instance,
                    int instanceCount,
                    const int* destinationPartials,
                    int destinationPartialsCount);

/**
 * @brief Calculate site log likelihoods at a root node
 *
 * This function integrates a list of partials at a node with respect to a set of partials-weights
 * and state frequencies to return the log likelihoods for each site
 *
 * @param instance              Instance number (input)
 * @param bufferIndices         List of partialsBuffer indices to integrate (input)
 * @param inWeights             List of weights to apply to each partialsBuffer (input). There
 *                               should be one categoryCount sized set for each of
 *                               parentBufferIndices
 * @param inStateFrequencies    List of state frequencies for each partialsBuffer (input). There
 *                               should be one set for each of parentBufferIndices
 * @param scalingFactorsIndices List of scalingFactors indices to accumulate over (input). There
 *                               should be one set for each of parentBufferIndices
 * @param scalingFactorsCount   List of scalingFactorsIndices sizes for each partialsBuffer (input)
 * @param count                 Number of partialsBuffer to integrate (input)
 * @param outLogLikelihoods     Pointer to destination for resulting log likelihoods (output) 
 *
 * @return error code
 */
int calculateRootLogLikelihoods(int instance,
                                const int* bufferIndices,
                                const double* inWeights,
                                const double* inStateFrequencies,
                                const int* scalingFactorsIndices,
                                int* scalingFactorsCount,
                                int count,
                                double* outLogLikelihoods);

/**
 * @brief Calculate site log likelihoods and derivatives along an edge
 *
 * This function integrates a list of partials at a parent and child node with respect
 * to a set of partials-weights and state frequencies to return the log likelihoods
 * and first and second derivatives for each site
 *
 * @param instance                  Instance number (input)
 * @param parentBufferIndices       List of indices of parent partialsBuffers (input)
 * @param childBufferIndices        List of indices of child partialsBuffers (input)
 * @param probabilityIndices        List indices of transition probability matrices for this edge
 *                                   (input)
 * @param firstDerivativeIndices    List indices of first derivative matrices (input)
 * @param secondDerivativeIndices   List indices of second derivative matrices (input)
 * @param inWeights                 List of weights to apply to each partialsBuffer (input)
 * @param inStateFrequencies        List of state frequencies for each partialsBuffer (input). There
 *                                   should be one set for each of parentBufferIndices
 * @param scalingFactorsIndices     List of scalingFactors indices to accumulate over (input). There
 *                                   should be one set for each of parentBufferIndices
 * @param scalingFactorsCount       List of scalingFactorsIndices sizes for each partialsBuffer
 *                                   (input)
 * @param count                     Number of partialsBuffers (input)
 * @param outLogLikelihoods         Pointer to destination for resulting log likelihoods (output)
 * @param outFirstDerivatives       Pointer to destination for resulting first derivatives (output)
 * @param outSecondDerivatives      Pointer to destination for resulting second derivatives (output) 
 *
 * @return error code
 */
int calculateEdgeLogLikelihoods(int instance,
                                const int* parentBufferIndices,
                                const int* childBufferIndices,
                                const int* probabilityIndices,
                                const int* firstDerivativeIndices,
                                const int* secondDerivativeIndices,
                                const double* inWeights,
                                const double* inStateFrequencies,
                                const int* scalingFactorsIndices,
                                int* scalingFactorsCount,
                                int count,
                                double* outLogLikelihoods,
                                double* outFirstDerivatives,
                                double* outSecondDerivatives);

/* using C calling conventions so that C programs can successfully link the beagle library
 * (closing brace)
 */
#ifdef __cplusplus
}
#endif

#endif // __beagle__