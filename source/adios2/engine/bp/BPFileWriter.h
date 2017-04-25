/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * BPWriter.h
 *
 *  Created on: Dec 16, 2016
 *      Author: wfg
 */

#ifndef ADIOS2_ENGINE_BP_BPFILEWRITER_H_
#define ADIOS2_ENGINE_BP_BPFILEWRITER_H_

#include "adios2/ADIOSConfig.h"
#include "adios2/capsule/heap/STLVector.h"
#include "adios2/core/Engine.h"
#include "adios2/utilities/format/bp1/BP1.h"

namespace adios
{

class BPFileWriter : public Engine
{

public:
    /**
     * Constructor for Writer writes in BP format into a single heap capsule,
     * manages several transports
     * @param name unique name given to the engine
     * @param accessMode
     * @param mpiComm
     * @param method
     * @param debugMode
     */
    BPFileWriter(ADIOS &adios, const std::string &name,
                 const std::string accessMode, MPI_Comm mpiComm,
                 const Method &method);
    ~BPFileWriter();

    void Write(Variable<char> &variable, const char *values);
    void Write(Variable<unsigned char> &variable, const unsigned char *values);
    void Write(Variable<short> &variable, const short *values);
    void Write(Variable<unsigned short> &variable,
               const unsigned short *values);
    void Write(Variable<int> &variable, const int *values);
    void Write(Variable<unsigned int> &variable, const unsigned int *values);
    void Write(Variable<long int> &variable, const long int *values);
    void Write(Variable<unsigned long int> &variable,
               const unsigned long int *values);
    void Write(Variable<long long int> &variable, const long long int *values);
    void Write(Variable<unsigned long long int> &variable,
               const unsigned long long int *values);
    void Write(Variable<float> &variable, const float *values);
    void Write(Variable<double> &variable, const double *values);
    void Write(Variable<long double> &variable, const long double *values);
    void Write(Variable<std::complex<float>> &variable,
               const std::complex<float> *values);
    void Write(Variable<std::complex<double>> &variable,
               const std::complex<double> *values);
    void Write(Variable<std::complex<long double>> &variable,
               const std::complex<long double> *values);
    void Write(VariableCompound &variable, const void *values);

    void Write(const std::string &variableName, const char *values);
    void Write(const std::string &variableName, const unsigned char *values);
    void Write(const std::string &variableName, const short *values);
    void Write(const std::string &variableName, const unsigned short *values);
    void Write(const std::string &variableName, const int *values);
    void Write(const std::string &variableName, const unsigned int *values);
    void Write(const std::string &variableName, const long int *values);
    void Write(const std::string &variableName,
               const unsigned long int *values);
    void Write(const std::string &variableName, const long long int *values);
    void Write(const std::string &variableName,
               const unsigned long long int *values);
    void Write(const std::string &variableName, const float *values);
    void Write(const std::string &variableName, const double *values);
    void Write(const std::string &variableName, const long double *values);
    void Write(const std::string &variableName,
               const std::complex<float> *values);
    void Write(const std::string &variableName,
               const std::complex<double> *values);
    void Write(const std::string &variableName,
               const std::complex<long double> *values);
    void Write(const std::string &variableName, const void *values);

    void Advance(float timeout_sec = 0.0);

    /**
     * Closes a single transport or all transports
     * @param transportIndex, if -1 (default) closes all transports, otherwise
     * it
     * closes a transport in m_Transport[transportIndex]. In debug mode the
     * latter
     * is bounds-checked.
     */
    void Close(const int transportIndex = -1);

private:
    format::BP1Writer m_BP1Writer;

    /** set to false after first Close is reached so metadata
        doesn't have to be accommodated for a subsequent Close */
    bool m_IsFirstClose = true;

    /** data buffer exponential growth factor */
    float m_GrowthFactor = 1.5;

    ///< true: due to buffer overflow
    bool m_TransportFlush = false;

    /** set to true if advance is called, this
        prevents flattening the data and metadata in Close*/
    bool m_CloseProcessGroup = false;

    void Init();
    void InitParameters();
    void InitTransports();
    void InitProcessGroup();

    void WriteProcessGroupIndex();

    /**
     * Common function for primitive (including std::complex) writes
     * @param group
     * @param variableName
     * @param variable
     */
    template <class T>
    void WriteVariableCommon(Variable<T> &variable, const T *values)
    {
        if (m_BP1Writer.m_MetadataSet.Log.IsActive == true)
        {
            m_BP1Writer.m_MetadataSet.Log.Timers[0].SetInitialTime();
        }

        // set variable
        variable.m_AppValues = values;
        m_WrittenVariables.insert(variable.m_Name);

        // if first timestep Write create a new pg index
        if (m_BP1Writer.m_MetadataSet.DataPGIsOpen == false)
        {
            WriteProcessGroupIndex();
        }

        // pre-calculate new metadata and payload sizes
        //        m_TransportFlush = CheckBufferAllocation(
        //        m_BP1Writer.GetVariableIndexSize( variable ) +
        //        variable.PayLoadSize(),
        //                                                  m_GrowthFactor,
        //                                                  m_MaxBufferSize,
        //                                                  m_Buffer.m_Data );

        // WRITE INDEX to data buffer and metadata structure (in memory)//
        m_BP1Writer.WriteVariableMetadata(variable);

        if (m_TransportFlush == true) // in batches
        {
            // write pg index

            // flush to transports

            // reset relative positions to zero, update absolute position
        }
        else // Write data to buffer
        {
            m_BP1Writer.WriteVariablePayload(variable);
        }

        // not needed after write
        variable.m_AppValues = nullptr;

        if (m_BP1Writer.m_MetadataSet.Log.IsActive == true)
        {
            m_BP1Writer.m_MetadataSet.Log.Timers[0].SetTime();
        }
    }
};

} // end namespace adios

#endif /* ADIOS2_ENGINE_BP_BPFILEWRITER_H_ */