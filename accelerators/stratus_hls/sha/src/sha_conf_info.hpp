// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __SHA_CONF_INFO_HPP__
#define __SHA_CONF_INFO_HPP__

#include <systemc.h>

//
// Configuration parameters for the accelerator.
//
class conf_info_t
{
public:

    //
    // constructors
    //
    conf_info_t()
    {
        /* <<--ctor-->> */
        this->input_size = 8192;
        this->input_v_size = 2;
    }

    conf_info_t(
        /* <<--ctor-args-->> */
        int32_t input_size, 
        int32_t input_v_size
        )
    {
        /* <<--ctor-custom-->> */
        this->input_size = input_size;
        this->input_v_size = input_v_size;
    }

    // equals operator
    inline bool operator==(const conf_info_t &rhs) const
    {
        /* <<--eq-->> */
        if (input_size != rhs.input_size) return false;
        if (input_v_size != rhs.input_v_size) return false;
        return true;
    }

    // assignment operator
    inline conf_info_t& operator=(const conf_info_t& other)
    {
        /* <<--assign-->> */
        input_size = other.input_size;
        input_v_size = other.input_v_size;
        return *this;
    }

    // VCD dumping function
    friend void sc_trace(sc_trace_file *tf, const conf_info_t &v, const std::string &NAME)
    {}

    // redirection operator
    friend ostream& operator << (ostream& os, conf_info_t const &conf_info)
    {
        os << "{";
        /* <<--print-->> */
        os << "input_size = " << conf_info.input_size << ", ";
        os << "input_v_size = " << conf_info.input_v_size << "";
        os << "}";
        return os;
    }

        /* <<--params-->> */
        int32_t input_size;
        int32_t input_v_size;
};

#endif // __SHA_CONF_INFO_HPP__
